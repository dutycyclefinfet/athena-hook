#ifndef __ATHENAOPKG_H__
#define __ATHENAOPKG_H__

#include <QProcess>
#include <QObject>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <stdio.h>
#include <unistd.h>

struct QProcessRet {
    QString std;
    int err;
    int status;
};
class AthenaOPKG : public QObject
{
    Q_OBJECT
private:
    QString _sanitize(QString name) {
        return name;
    }

    QProcessRet _opkg(const QString& cmd, const QString additonal_arg = "") {
        QProcessRet ret;
        QProcess process;
        QStringList args;

        args << _sanitize(cmd);

        if(additonal_arg != "") {
            args << _sanitize(additonal_arg);
        }

        process.start("/opt/bin/opkg", args);
        process.waitForFinished(-1); //will wait forever until finished

        if (process.exitStatus() == QProcess::CrashExit) {
            ret.status = process.error();
        } else {
            ret.status = 0;
        }
        ret.err = process.exitCode();
        ret.std = process.readAllStandardOutput();

        return ret;
    }

    QStringList _pkgOutputToList(const QString &output) {
        QStringList pkg_list = output.split(QRegExp("[\r\n]"),Qt::SkipEmptyParts);
        QStringList result;

        for(const auto& p : pkg_list) {
            const QRegExp match("^([a-zA-Z0-9-]*)");
            
            if (match.indexIn(p) != 1) {
                result << match.cap(1);
            }
        }

        return result;
    }

public:
    Q_INVOKABLE QStringList getPackages() {
        auto ret = _opkg("find", "*");

        return _pkgOutputToList(ret.std);
    }
    Q_INVOKABLE QStringList getInstalled() {
        auto ret = _opkg("list-installed");

        return _pkgOutputToList(ret.std);
    }
    Q_INVOKABLE QStringList getUpgradable() {
        auto ret = _opkg("list-upgradable");

        return _pkgOutputToList(ret.std);
    }
    Q_INVOKABLE QStringList getInfo(QString packageName) {
        auto ret = _opkg("info", packageName);
        
        return ret.std.split(QRegExp("[\r\n]"),Qt::SkipEmptyParts);
    }
    Q_INVOKABLE QString upgrade(QString packageName) {
        auto ret = _opkg("upgrade");
        
        return (!ret.err && !ret.status) ? "" : ret.std;
    }
    Q_INVOKABLE QString install(QString packageName) {
        auto ret = _opkg("install", packageName);
        
        return (!ret.err && !ret.status) ? "" : ret.std;
    }
    Q_INVOKABLE QString remove(QString packageName) {
        auto ret = _opkg("remove", packageName);
        
        return (!ret.err && !ret.status) ? "" : ret.std;
    }
    Q_INVOKABLE bool update() {
        auto ret = _opkg("update");
        
        return (!ret.err && !ret.status);
    }
};

#endif //__ATHENAOPKG_H__
