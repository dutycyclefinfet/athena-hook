#ifndef __ATHENAHOOK_H__
#define __ATHENAHOOK_H__

#include <QObject>
#include <QStringList>
#include <QFile>
#include <QCryptographicHash>
#include <filesystem>
#include <string>
#include "Utils.h"

class AthenaHook : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QStringList pluginsList READ pluginList_get CONSTANT);
    Q_PROPERTY(QString pluginsPath READ pluginPath_get CONSTANT);
    Q_PROPERTY(QStringList pluginsHashList READ pluginHashList_get CONSTANT);
    Q_PROPERTY(QString pluginsHash READ pluginHash_get CONSTANT);
    
    QStringList m_pluginList;
    QStringList m_hashList;
    QString m_hash;
private:
    QStringList listDir(QString path) {
        m_pluginList.clear();
        for (const auto & entry : std::filesystem::directory_iterator(path.toStdString())) {
            std::string name = entry.path().filename();
            if (name[0] != '.') {
                m_pluginList << QString::fromStdString(name);
            }
        }
        return m_pluginList;
    }
public:
    QStringList pluginList_get() {
        m_pluginList = listDir(sysInfo.xochitlPluginsPath);
        
        return m_pluginList;
    };
    QString pluginPath_get() {
        return sysInfo.xochitlPluginsPath;
    };
    QStringList pluginHashList_get() {
        pluginList_get();
        
        m_hashList.clear();
        for (const QString& plugin : m_pluginList) {
            QFile file(sysInfo.xochitlPluginsPath + "/" + plugin + "/manifest.json");
            if (file.open(QIODevice::ReadOnly)) {
                m_hashList << QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
            }
        }
        return m_hashList;
    };
    QString pluginHash_get() {
        pluginHashList_get();
        
        QString hash;
        for (const QString& fHash : m_hashList) {
            hash.append(fHash);
        }
        m_hash = QCryptographicHash::hash(hash.toLocal8Bit(), QCryptographicHash::Md5).toHex();
        return m_hash;
    };
    
    AthenaHook() : QObject(nullptr) {
    }
};

#endif //__ATHENAHOOK_H__
