#ifndef __ATHENAHOOK_H__
#define __ATHENAHOOK_H__

#include <QObject>
#include <QStringList>
#include <QFile>
#include <QCryptographicHash>
#include <QProcessEnvironment>
#include <QQuickItem>
#include <filesystem>
#include <string>
#include "Utils.h"

class AthenaHook : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QQuickItem* rootItem READ rootItem_get CONSTANT);
    Q_PROPERTY(QString rootPrefix READ rootPrefix_get CONSTANT);
    Q_PROPERTY(QStringList pluginsList READ pluginList_get CONSTANT);
    Q_PROPERTY(QString pluginsPath READ pluginPath_get CONSTANT);
    Q_PROPERTY(QStringList pluginsHashList READ pluginHashList_get CONSTANT);
    Q_PROPERTY(QString pluginsHash READ pluginHash_get CONSTANT);
    
    QQuickItem* m_rootItem;
    QString m_rootPrefix;
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
    Q_INVOKABLE QString env(QString name) {
        return QProcessEnvironment::systemEnvironment().value(name);
    };
    QQuickItem* rootItem_get() {
        return m_rootItem;
    };
    QString rootPrefix_get() {
        m_rootPrefix = sysInfo.athenaRoot;
        
        return m_rootPrefix;
    };
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
    
    AthenaHook(QQuickItem* _rootItem) : QObject(nullptr) {
        m_rootItem = _rootItem;
    }
};

#endif //__ATHENAHOOK_H__
