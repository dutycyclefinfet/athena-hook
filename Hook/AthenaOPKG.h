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
    QProcessRet _system(QString cmd, QString arg) {
        QProcess process;
        QProcessRet ret;
        QStringList args;
        args << "-c"
             << ("\"" + cmd + " " + _sanitize(arg) + "\"");
        process.start("/bin/bash", args);
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
public:
    Q_INVOKABLE QStringList getPackages() {
        auto ret = _system("opkg find \\* | grep -oe '^[a-zA-Z0-9-]*'", "");
        
        return ret.std.split(QRegExp("[\r\n]"),Qt::SkipEmptyParts);
    }
    Q_INVOKABLE QStringList getUpgradable() {
        auto ret = _system("opkg list-upgradable | grep -oe '^[a-zA-Z0-9-]*'", "");
        
        return ret.std.split(QRegExp("[\r\n]"),Qt::SkipEmptyParts);
    }
    Q_INVOKABLE QStringList getInfo(QString packageName) {
        auto ret = _system("opkg info", packageName);
        
        return ret.std.split(QRegExp("[\r\n]"),Qt::SkipEmptyParts);
    }
    Q_INVOKABLE QString upgrade(QString packageName) {
        auto ret = _system("opkg upgrade", packageName);
        
        return (!ret.err && !ret.status) ? "" : ret.std;
    }
    Q_INVOKABLE QString install(QString packageName) {
        auto ret = _system("opkg install", packageName);
        
        return (!ret.err && !ret.status) ? "" : ret.std;
    }
    Q_INVOKABLE QString remove(QString packageName) {
        auto ret = _system("opkg remove", packageName);
        
        return (!ret.err && !ret.status) ? "" : ret.std;
    }
    Q_INVOKABLE bool update() {
        auto ret = _system("opkg update", "");
        
        return (!ret.err && !ret.status);
    }
};

#endif //__ATHENAOPKG_H__
