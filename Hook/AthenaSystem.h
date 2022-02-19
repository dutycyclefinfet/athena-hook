#ifndef __ATHENASYSTEM_H__
#define __ATHENASYSTEM_H__

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVariantMap>
#include <QVariantList>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QProcessEnvironment>
#include <QQuickItem>
#include <sys/sysinfo.h>
#include "AthenaBase.h"

#include <QDebug>


class AthenaSystem : public QObject, public AthenaBase
{
    Q_OBJECT
    
    const QString s_battery_dir = "/sys/class/power_supply/";
    
public:
    Q_INVOKABLE QStringList dir(const QString& path, bool hiddenFiles = false) {
        return listDir(path, hiddenFiles);
    };
    Q_INVOKABLE bool copy(const QString& path, const QString& dest) {
        return QFile::copy(path, dest);
    };
    Q_INVOKABLE bool move(const QString& path, const QString& dest) {
        return QFile::rename(path, dest);
    };
    Q_INVOKABLE bool remove(const QString& path) {
        return QFile::remove(path);
    };
    Q_INVOKABLE bool exists(const QString& path) {
        return QFile::exists(path);
    }
    Q_INVOKABLE bool link(const QString& path, const QString& name) {
        return QFile::link(path, name);
    }
    
    Q_INVOKABLE QString readLink(const QString& path) {
        return QFile::symLinkTarget(path);
    }
    Q_INVOKABLE int getPermissions(const QString& path) {
        return (int)QFile::permissions(path);
    }
    Q_INVOKABLE int setPermissions(const QString& path, int permissions) {
        return QFile::setPermissions(path, (QFileDevice::Permission)permissions);
    }
    
    Q_INVOKABLE QString getEnv(const QString& name) {
        return QProcessEnvironment::systemEnvironment().value(name);
    };
    Q_INVOKABLE int setEnv(const QString& name, const QString& val) {
        return setenv(name.toStdString().c_str(), val.toStdString().c_str(), 1);
    };
    Q_INVOKABLE void restartXochitl() {
        system("systemctl restart xochitl");
    };
    
    Q_INVOKABLE QString dmesg() {
        QProcessRet ret = runProcess("/usr/bin/dmesg", "");
        if (!ret.err && !ret.status) {
            return ret.std;
        }
        return "";
    }
    
    Q_INVOKABLE int getFreeRAM() {
        if ((get_phys_pages() < get_avphys_pages()) || (sysconf(_SC_PAGESIZE)<=0)) {
            return get_avphys_pages()*sysconf(_SC_PAGESIZE);
        } else {
            return -1;
        }
    }
    Q_INVOKABLE int getTotalRAM() {
        if ((get_phys_pages() < get_avphys_pages()) || (sysconf(_SC_PAGESIZE)<=0)) {
            return get_phys_pages()*sysconf(_SC_PAGESIZE);
        } else {
            return -1;
        }
    }
    
    Q_INVOKABLE QVariantList getBatteries() {
        QVariantList ret;
        
        QDir psuDir(s_battery_dir);
        QFileInfoList psuInfo = psuDir.entryInfoList();
        for (int ix0 = 0; ix0 < psuInfo.size(); ++ix0) {
            auto battName = psuInfo[ix0].fileName();
            
            if (psuInfo[ix0].isDir() && battName[0] != '.') {
                QVariantMap battHash;
                auto dName = s_battery_dir + battName + "/";
                battHash.insert("name", battName);
                
                QDir battDir(dName);
                QFileInfoList battInfo = battDir.entryInfoList();
                for (int ix0 = 0; ix0 < battInfo.size(); ++ix0) {
                    QString key = battInfo[ix0].fileName();
                    QString value;
                    
                    if (battInfo[ix0].isFile() && key[0] != '.') {
                        read(value, battInfo[ix0].absoluteFilePath());
                        battHash.insert(key, value);
                    }
                }
                ret.push_back(battHash);
            }
        }
        return ret;
    }
    
    AthenaSystem() : QObject(nullptr) {
    }
};

#endif //__ATHENASYSTEM_H__
