#ifndef __ATHENAKERNEL_H__
#define __ATHENAKERNEL_H__

#include <QObject>
#include <QVariant>
#include <stdio.h>
#include <unistd.h>
#include "Utils.h"

class AthenaKernel : public QObject
{
    Q_OBJECT
    // CPU voltage
    Q_PROPERTY(int cpuUndervolt READ cpuUndervolt_get WRITE cpuUndervolt_set NOTIFY cpuUndervolt_changed);
    // Kernel
    Q_PROPERTY(bool bootAthena READ bootAthena_get WRITE bootAthena_set NOTIFY bootAthena_changed);
    Q_PROPERTY(bool overlayWipe READ overlayWipe_get WRITE overlayWipe_set NOTIFY overlayWipe_changed);
    Q_PROPERTY(bool isAthena READ isAthena_get CONSTANT);
    Q_PROPERTY(bool wasCrashed READ wasCrashed_get CONSTANT);

    QList<int> s_cpuUndervolts = {-100, -75, -50, -25, 0, 25, 50, 75};
    QStringList s_cpuUndervolts_files = {
        "dtb/zero-sugar_-100mV.dtb", "dtb/zero-sugar_-75mV.dtb", "dtb/zero-sugar_-50mV.dtb",
        "dtb/zero-sugar_-25mV.dtb", "dtb/zero-sugar_0mV.dtb", "dtb/zero-sugar_25mV.dtb",
        "dtb/zero-sugar_50mV.dtb", "dtb/zero-sugar_75mV.dtb"};
    QString s_zero_sugar_dtb_path = "boot/athena/zero-sugar.dtb";
    QString s_last_log_path = "var/log/kdump/last_log_name";
    
    int m_cpuUndervolt;
    
    bool m_bootAthena;
    bool m_overlayWipe;
    bool m_isAthena;
    bool m_wasCrashed;

private:
    QString athenaPath(const QString& path) {
        return sysInfo.athenaRoot + path;
    }
public:
    // CPU Undervolt
    int cpuUndervolt_get() {
        QString current_undervolt = QFile::symLinkTarget(athenaPath(s_zero_sugar_dtb_path));

        for (const auto& name : s_cpuUndervolts_files) {
            if (name == current_undervolt) {
                m_cpuUndervolt = s_cpuUndervolts[s_cpuUndervolts_files.indexOf(name)];

                return m_cpuUndervolt;
            }
        }
        return -9999;
    }
    void cpuUndervolt_set(int val) {
        int ix = s_cpuUndervolts.indexOf(val);
        if ((ix > 0) && (val != m_cpuUndervolt)) {
            QFile::remove(athenaPath(s_zero_sugar_dtb_path));
            QFile::link(s_cpuUndervolts_files.at(ix), athenaPath(s_zero_sugar_dtb_path));

            emit cpuUndervolt_changed(val);
        }
    }
    
    // Bootloader
    bool bootAthena_get() {
        m_bootAthena = (system("fw_printenv athena_fail | grep athena_fail=0 1>/dev/null 2>/dev/null")==0);
        
        return m_bootAthena;
    }
    void bootAthena_set(bool val) {
        if (val) {
            system("fw_setenv athena_fail 0");
        } else {
            system("fw_setenv athena_fail 1");
        }

        emit bootAthena_changed(val);
    }
    bool isAthena_get() {
        return sysInfo.athenaIsRunning;
    }
    
    // Wipe
    bool overlayWipe_get() {
        m_overlayWipe = QFile::exists(athenaPath("wipe_me"));
        return m_overlayWipe;
    }
    void overlayWipe_set(bool val) {
        if (val) {
            Utils::write("", athenaPath("wipe_me"));
        } else {
            QFile::remove(athenaPath("wipe_me"));
        }

        emit overlayWipe_changed(val);
    }
    
    // Kdump
    bool wasCrashed_get() {
        m_wasCrashed = QFile::exists(athenaPath(s_last_log_path));
        
        return m_wasCrashed;
    }
    void wasCrashed_set(bool val) {
        if (!val) {
            QFile::remove(athenaPath(s_last_log_path));
        }

        emit wasCrashed_changed(val);
    }

    AthenaKernel() : QObject(nullptr) {
    }
signals:
    void cpuUndervolt_changed(int);
    void bootAthena_changed(bool);
    void overlayWipe_changed(bool);
    void wasCrashed_changed(bool);
};

#endif //__ATHENAKERNEL_H__
