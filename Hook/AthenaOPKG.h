#ifndef __ATHENAOPKG_H__
#define __ATHENAOPKG_H__

#include <QObject>
#include <QtConcurrent>
#include "AthenaBase.h"

class OPKGThread : public QThread, AthenaBase
{
    Q_OBJECT
    
    QStringList m_allPackages;
    QStringList m_installedPackages;
    QStringList m_upgradablePackages;
    QString m_state;
    
    QString packageName;
    QString cmd;
    int flags;
private:
    QProcessRet _opkg(const QString& cmd, const QString additional_arg = "") {
        return runProcess("/opt/bin/opkg", cmd, additional_arg);
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

    void _update() {
        QProcessRet ret;
        
        if (flags & 0x01) {
            m_state = "Updating cache...\nPlease wait...";
            emit state_changed(m_state);
            ret = _opkg("update");
        }
        
        if (flags & 0x02) {
            m_state = "Listing packages\nPlease wait...";
            emit state_changed(m_state);
            ret = _opkg("find", "*");
            m_allPackages = _pkgOutputToList(ret.std);
            emit allPackages_changed(m_allPackages);
        }
        
        if (flags & 0x04) {
            m_state = "Listing upgradable packages\nPlease wait...";
            emit state_changed(m_state);
            ret = _opkg("list-upgradable");
            m_upgradablePackages = _pkgOutputToList(ret.std);
            emit installedPackages_changed(m_installedPackages);
        }
        
        if (flags & 0x08) {
            m_state = "Listing installed packages\nPlease wait...";
            emit state_changed(m_state);
            ret = _opkg("list-installed");
            m_installedPackages = _pkgOutputToList(ret.std);
            emit upgradablePackages_changed(m_upgradablePackages);
        }
        
        m_state = "";
        emit state_changed(m_state);
    }
    void _do(QString text1, QString text2, QString text3, QString text4) {
        QProcessRet ret;
        
        m_state = text2 + " " + packageName + "...\nPlease wait...";
        emit state_changed(m_state);
        ret = _opkg(text1, packageName);
        
        if (!ret.err && !ret.status) {
            m_state = text3 + " " + packageName + "!";
        } else {
            m_state = "Error while " + text4 + " " + packageName + "!\n" + ret.std + "\n\n" + ret.stderr;
        }
        emit state_changed(m_state);
    }
public:
    void run() override {
        if (cmd == "upgrade") {
            _do("upgrade", "Upgrading", "Upgraded", "upgrading");
        }
        if (cmd == "install") {
            _do("install", "Installing", "Installed", "installing");
        }
        if (cmd == "remove") {
            _do("remove", "Removing", "Removed", "removing");
        }
        if (cmd == "update") {
            _update();
        }
    }
    void execute(QString c, QString p, int f = 0x0f) {
        flags = f;
        cmd = c;
        packageName = p;
        start();
    }
    
    QString state_get() {
        return m_state;
    }
    QStringList allPackages_get() {
        return m_allPackages;
    }
    QStringList upgradablePackages_get() {
        return m_upgradablePackages;
    }
    QStringList installedPackages_get() {
        return m_installedPackages;
    }
signals:
    void allPackages_changed(QStringList);
    void installedPackages_changed(QStringList);
    void upgradablePackages_changed(QStringList);
    void state_changed(QString);
};
class AthenaOPKG : public QObject, public AthenaBase
{
    Q_OBJECT
    Q_PROPERTY(QStringList allPackages READ allPackages_get NOTIFY allPackages_changed);
    Q_PROPERTY(QStringList installedPackages READ installedPackages_get NOTIFY installedPackages_changed);
    Q_PROPERTY(QStringList upgradablePackages READ upgradablePackages_get NOTIFY upgradablePackages_changed);
    Q_PROPERTY(QString state READ state_get NOTIFY state_changed);
    
private:
    QStringList m_allPackages;
    QStringList m_installedPackages;
    QStringList m_upgradablePackages;
    QString m_state;
    
    OPKGThread opkgThread;
    
    QProcessRet _opkg(const QString& cmd, const QString additional_arg = "") {
        return runProcess("/opt/bin/opkg", cmd, additional_arg);
    }
public:
    Q_INVOKABLE void update(int flags = 0x0f) {
        opkgThread.execute("update", "", flags);
    }
    Q_INVOKABLE void upgrade(QString packageName, bool block = true) {
        opkgThread.execute("upgrade", packageName);
        if (block)
            opkgThread.wait();
    }
    Q_INVOKABLE void install(QString packageName, bool block = true) {
        opkgThread.execute("install", packageName);
        if (block)
            opkgThread.wait();
    }
    Q_INVOKABLE void remove(QString packageName, bool block = true) {
        opkgThread.execute("remove", packageName);
        if (block)
            opkgThread.wait();
    }
    Q_INVOKABLE QStringList getInfo(QString packageName) {
        auto ret = _opkg("info", packageName);
        
        return ret.std.split(QRegExp("[\r\n]"),Qt::SkipEmptyParts);
    }
    
    QString state_get() {
        return opkgThread.state_get();
    }
    QStringList allPackages_get() {
        return opkgThread.allPackages_get();
    }
    QStringList upgradablePackages_get() {
        return opkgThread.upgradablePackages_get();
    }
    QStringList installedPackages_get() {
        return opkgThread.installedPackages_get();
    }
    
    AthenaOPKG() {
        connect(&opkgThread, &OPKGThread::allPackages_changed, this, &AthenaOPKG::allPackages_changed);
        connect(&opkgThread, &OPKGThread::installedPackages_changed, this, &AthenaOPKG::installedPackages_changed);
        connect(&opkgThread, &OPKGThread::upgradablePackages_changed, this, &AthenaOPKG::upgradablePackages_changed);
        connect(&opkgThread, &OPKGThread::state_changed, this, &AthenaOPKG::state_changed);
    }
signals:
    void allPackages_changed(QStringList);
    void installedPackages_changed(QStringList);
    void upgradablePackages_changed(QStringList);
    void state_changed(QString);
};

#endif //__ATHENAOPKG_H__
