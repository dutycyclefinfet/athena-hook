#ifndef __UTILS_H__
#define __UTILS_H__

#include <QFile>
#include <QTextStream>
#include <QString>

struct SystemInfo {
    QString xochitlPluginsCommon;
    QString xochitlPluginsPath;
    QString athenaRoot;
    bool athenaIsRunning;
};
extern SystemInfo sysInfo;

class Utils {
public:
    static void readAndSplit(QStringList &output, const QString& path, const QString &sep) {
        QFile file(path);

        if (!file.open(QIODevice::ReadOnly))
            return;
        
        QTextStream in(&file);

        QString line = in.readLine();
        output = line.split(sep, Qt::SkipEmptyParts);
    }
    static void read(QString &output, const QString &path, const QString &sep, const QString def = "") {
        QStringList l;

        readAndSplit(l, path, sep);

        if(l.size() > 0) {
            output = l[0];
        } else {
            output = def;
        }
    }
    static void write(const QString& data, const QString& path) {
        QFile file(path);

        if (!file.open(QIODevice::WriteOnly))
            return;
        
        QTextStream out(&file);
        out << data;
    }
};

#endif //__UTILS_H__
