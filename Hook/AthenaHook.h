#ifndef __ATHENAHOOK_H__
#define __ATHENAHOOK_H__

#include <QObject>
#include <QStringList>
#include <QFile>
#include <QCryptographicHash>
#include <QProcessEnvironment>
#include <QQuickItem>
#include "AthenaBase.h"
#include "Utils.h"

class AthenaHook : public QObject, public AthenaBase
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

public:
    Q_INVOKABLE QString env(const QString& name) {
        return QProcessEnvironment::systemEnvironment().value(name);
    };
    QQuickItem* rootItem_get() {
        return m_rootItem;
    };
    QString rootPrefix_get() {
        m_rootPrefix = AthenaBase::athenaRoot();
        
        return m_rootPrefix;
    };
    QStringList pluginList_get() {
        m_pluginList = Utils::listDir(AthenaBase::xochitlPluginsPath());
        
        return m_pluginList;
    };
    QString pluginPath_get() {
        return AthenaBase::xochitlPluginsPath();
    };
    QStringList pluginHashList_get() {
        pluginList_get();
        
        m_hashList.clear();
        for (const auto& plugin : m_pluginList) {
            QFile file(AthenaBase::xochitlPluginsPath() + "/" + plugin + "/manifest.json");
            if (file.open(QIODevice::ReadOnly)) {
                m_hashList << QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5).toHex();
            }
        }
        return m_hashList;
    };
    QString pluginHash_get() {
        pluginHashList_get();
        
        QString hash;
        for (const auto& fHash : m_hashList) {
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
