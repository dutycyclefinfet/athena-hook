#ifndef __ATHENABASE_H__
#define __ATHENABASE_H__

#include <QString>
#include <QStringList>
#include <QProcess>

#include "Utils.h"

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

    static QProcessRet runProcess(const QString& path, const QString& cmd, const QString additional_arg = "", timeout=-1) {
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
