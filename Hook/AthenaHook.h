#ifndef __ATHENAHOOK_H__
#define __ATHENAHOOK_H__

#include <QObject>
#include <QStringList>
#include <filesystem>
#include <string>
#include "Utils.h"

class AthenaHook : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QStringList pluginList READ pluginList_get CONSTANT);
    Q_PROPERTY(QString pluginPath READ pluginPath_get CONSTANT);
    
    QStringList m_pluginList;
private:
    QStringList listDir(QString path) {
        QStringList list;
        for (const auto & entry : std::filesystem::directory_iterator(path.toStdString())) {
            std::string name = entry.path().filename();
            if (name[0] != '.') {
                list << QString::fromStdString(name);
            }
        }
        return list;
    }
public:
    QStringList pluginList_get() {
        m_pluginList = listDir(sysInfo.xochitlPluginsPath);
        
        return m_pluginList;
    };
    QString pluginPath_get() {
        return sysInfo.xochitlPluginsPath;
    };
    
    AthenaHook() : QObject(nullptr) {
    }
};

#endif //__ATHENAHOOK_H__
