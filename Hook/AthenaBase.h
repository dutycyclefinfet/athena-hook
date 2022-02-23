#ifndef __ATHENABASE_H__
#define __ATHENABASE_H__

#include <QString>
#include <QStringList>
#include <QProcess>
#include <QTextStream>
#include <QFile>
#include <filesystem>
#include <unistd.h>
#include <QHash>
#include <QThread>
#include <sys/inotify.h>
#include <stdio.h>
#include <QDebug>

class InotifyThread : public QThread
{
    Q_OBJECT
    
private:
    QHash<QString, std::function<void()>> m_inotifyS;
    QHash<int, std::function<void()>> m_inotifyI;
    char buffer[sizeof(struct inotify_event)*20 + 1] = {0};
    int m_inotify_fd = 0;
    
    void reload() {
        if (m_inotify_fd) {
            close(m_inotify_fd);
        }
        
        m_inotify_fd = inotify_init();
        if (m_inotify_fd) {
            m_inotifyI.clear();
        
            QHashIterator<QString, std::function<void()>> i(m_inotifyS);
            while (i.hasNext()) {
                i.next();
                
                int ret = inotify_add_watch(m_inotify_fd, i.key().toStdString().c_str(), IN_MODIFY|IN_CREATE|IN_DELETE);
                if (ret) {
                    m_inotifyI[ret] = i.value();
                }
            }
        }
    }
public:
    void run() override {
        while (true) {
            reload();
            ssize_t len = read(m_inotify_fd, buffer, sizeof(buffer));
            
            if (len > 0) {
                for (ssize_t i=0; i+(ssize_t)sizeof(struct inotify_event) <= len;) {
                    inotify_event* event = (inotify_event*)((&buffer)[i]);
                    if (m_inotifyI.contains(event->wd)) {
                        m_inotifyI[event->wd]();
                    }
                    i += sizeof(struct inotify_event) + event->len;
                }
            }
        }
    }
    int add(const QString &path, std::function<void()> lambda) {
        m_inotifyS[path] = lambda;
        
        return m_inotifyS.size();
    }
};
class SysNotify
{
    QString path;
    QString cache;
    std::function<void()> lambda;
public:
    void check() {
        QFile file(path);
        if (!file.open(QIODevice::ReadOnly))
            return;
        QTextStream in(&file);
        QString data = in.read(1024);
        if (data != cache) {
            cache = data;
            lambda();
        }
    }
    SysNotify(QString p, std::function<void()> l) {
        path = p;
        lambda = l;
    }
};
class SysNotifyThread : public QThread
{
    Q_OBJECT
    
private:
    QList<SysNotify*> m_sysfsnotify;
public:
    void run() override {
        while (true) {
            QList<SysNotify*>::iterator i;
            for (i = m_sysfsnotify.begin(); i != m_sysfsnotify.end(); ++i) {
                (*i)->check();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
        }
    }
    int add(const QString &path, std::function<void()> lambda) {
        m_sysfsnotify.push_back(new SysNotify(path, lambda));
        return m_sysfsnotify.size();
    }
};

class AthenaBase
{
private:
    static QString m_xochitlPluginsCommon;
    static QString m_xochitlPluginsPath;
    static QString m_athenaRoot;
    static bool m_athenaIsRunning;
    
    static InotifyThread fileWatcher;
    static SysNotifyThread sysfsWatcher;

    static QString _sanitize(const QString& name) {
        return name;
    }

    static bool _isAthenaRunning() {
        return (system("uname -a | grep athena 1>/dev/null 2>/dev/null")==0);
    }
    

protected:
    struct QProcessRet {
        QString std;
        QString stderr;
        int err;
        int status;
    };

    static QProcessRet runProcess(const QString& path, const QString& cmd, const QString additional_arg = "", int timeout=-1) {
        QProcessRet ret;
        QProcess process;
        QStringList args;

        args << _sanitize(cmd);

        if (additional_arg != "") {
            args << _sanitize(additional_arg);
        }

        process.start(path, args);
        process.waitForFinished(timeout); //will wait forever until finished

        if (process.exitStatus() == QProcess::CrashExit) {
            ret.status = process.error();
        } else {
            ret.status = 0;
        }
        ret.err = process.exitCode();
        ret.std = process.readAllStandardOutput();
        ret.stderr = process.readAllStandardError();

        return ret;
    }

    static QString athenaPath(const QString& path) {
        return m_athenaRoot + path;
    }
    
    static void readFileAndSplit(QStringList &output, const QString& path, const QString &sep = "") {
        QFile file(path);

        if (!file.open(QIODevice::ReadOnly))
            return;
        
        QTextStream in(&file);

        QString line = in.readLine() + sep;
        if (sep != "") {
            output = line.split(sep, Qt::SkipEmptyParts);
        } else {
            output.push_back(line);
        }
    }
    
    static void readFile(QString &output, const QString &path, const QString &sep = "", const QString def = "") {
        QStringList l;

        readFileAndSplit(l, path, sep);

        if(l.size() > 0) {
            output = l[0];
        } else {
            output = def;
        }
    }
    
    static void writeFile(const QString& data, const QString& path) {
        QFile file(path);

        if (!file.open(QIODevice::WriteOnly))
            return;
        
        QTextStream out(&file);
        out << data;
    }

    static QStringList listDir(const QString& path, bool hiddenFiles=false) {
        QStringList l;
        for (const auto & entry : std::filesystem::directory_iterator(path.toStdString())) {
            const std::string name = entry.path().filename();
            if (name[0] != '.' || hiddenFiles) {
                l << QString::fromStdString(name);
            }
        }
        return l;
    }

    static QString readLinkC(const QString& path) {
        char buf[1024];
        auto written = readlink(path.toStdString().c_str(), buf, sizeof(buf)-1);
        if (written > 0) {
            buf[written] = '\0';
            return QString::fromLocal8Bit(buf);
        } else {
            return "";
        }
    }

    static int symLinkC(const QString &fromPath, const QString &toPath) {
        return symlink(fromPath.toStdString().c_str(), toPath.toStdString().c_str());
    }
    
    int _addFileMonitor(const QString &path, std::function<void()> lambda) {
        return fileWatcher.add(path, lambda);
    }
    int _addSysfsMonitor(const QString &path, std::function<void()> lambda) {
        return sysfsWatcher.add(path, lambda);
    }

public:
    static void init() {
        m_athenaIsRunning = _isAthenaRunning();

        if (m_athenaIsRunning) {
            m_athenaRoot = "/";
        } else {
            m_athenaRoot = "/home/.rootdir/";
        }
        
        m_xochitlPluginsPath = "/home/root/.xochitlPlugins/";
        m_xochitlPluginsCommon = m_xochitlPluginsPath + ".common/";
    }

    static const QString& xochitlPluginsCommon() {
        return m_xochitlPluginsCommon;
    }

    static const QString& xochitlPluginsPath() {
        return m_xochitlPluginsPath;
    }

    static const QString& athenaRoot() {
        return m_athenaRoot;
    }

    static bool isAthenaRunning() {
        return m_athenaIsRunning;
    }
    
    static void finish() {
        fileWatcher.start();
        sysfsWatcher.start();
    }
};

#define addFileMonitor(a, b) _addFileMonitor(a, [=]() {emit b ## _changed(b ## _get());})
#define addSysfsMonitor(a, b) _addSysfsMonitor(a, [=]() {emit b ## _changed(b ## _get());})

#endif //__ATHENABASE_H__
