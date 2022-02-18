#ifndef __ATHENAOPKG_H__
#define __ATHENAOPKG_H__

#include <QObject>
#include <QtConcurrent>
#include "AthenaBase.h"

class AthenaOPKG : public QObject, public AthenaBase
{
    Q_OBJECT
    Q_PROPERTY(QStringList allPackages READ allPackages_get NOTIFY allPackages_changed);
    Q_PROPERTY(QStringList installedPackages READ installedPackages_get NOTIFY installedPackages_changed);
    Q_PROPERTY(QStringList upgradablePackages READ upgradablePackages_get NOTIFY upgradablePackages_changed);
    Q_PROPERTY(QString state READ state_get NOTIFY state_changed);
    
    QStringList m_allPackages;
    QStringList m_installedPackages;
    QStringList m_upgradablePackages;
    QString m_state;
    
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
        
        m_state = "Updating cache";
        emit state_changed(m_state);
        ret = _opkg("update");
        
        m_state = "Listing packages";
        emit state_changed(m_state);
        ret = _opkg("find", "*");
        m_allPackages = _pkgOutputToList(ret.std);
        emit installedPackages_changed(m_allPackages);
        
        m_state = "Listing upgradable packages";
        emit state_changed(m_state);
        ret = _opkg("list-upgradable");
        m_upgradablePackages = _pkgOutputToList(ret.std);
        emit installedPackages_changed(m_installedPackages);
        
        m_state = "Listing installed packages";
        emit state_changed(m_state);
        ret = _opkg("list-installed");
        m_installedPackages = _pkgOutputToList(ret.std);
        emit upgradablePackages_changed(m_upgradablePackages);
        
        m_state = "";
        emit state_changed(m_state);
    }
public:
    Q_INVOKABLE void update(bool blocking = false) {
        if (blocking) {
            _update();
        } else {
            QtConcurrent::run([=]() {
                update(true);
            });
        }
    }
    Q_INVOKABLE QStringList getInfo(QString packageName) {
        auto ret = _opkg("info", packageName);
        
        return ret.std.split(QRegExp("[\r\n]"),Qt::SkipEmptyParts);
    }
    Q_INVOKABLE QString upgrade(QString packageName) {
        auto ret = _opkg("upgrade", packageName);
        
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

#endif //__ATHENAOPKG_H__
