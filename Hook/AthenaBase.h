#ifndef __ATHENABASE_H__
#define __ATHENABASE_H__

#include <QString>
#include <QStringList>
#include <QProcess>
#include <QTextStream>
#include <QFile>
#include <filesystem>
#include <unistd.h>

class AthenaBase
{
private:
    static QString m_xochitlPluginsCommon;
    static QString m_xochitlPluginsPath;
    static QString m_athenaRoot;
    static bool m_athenaIsRunning;

    static QString _sanitize(const QString& name) {
        return name;
    }

    static bool _isAthenaRunning() {
        return (system("uname -a | grep athena 1>/dev/null 2>/dev/null")==0);
    }

protected:
    struct QProcessRet {
        QString std;
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

        return ret;
    }

    static QString athenaPath(const QString& path) {
        return m_athenaRoot + path;
    }
    
    static void readAndSplit(QStringList &output, const QString& path, const QString &sep = "") {
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
    
    static void read(QString &output, const QString &path, const QString &sep = "", const QString def = "") {
        QStringList l;

        readAndSplit(l, path, sep);

        if(l.size() > 0) {
            output = l[0];
        } else {
            output = def;
        }
    }
    
    static void write(const QString& data, const QString& path) {
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
};

#endif //__ATHENABASE_H__
