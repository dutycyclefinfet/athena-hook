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
    Q_PROPERTY(bool kernelBootAthena READ kernelBootAthena_get WRITE kernelBootAthena_set NOTIFY kernelBootAthena_changed);
    Q_PROPERTY(bool kernelOverlayWipe READ kernelOverlayWipe_get WRITE kernelOverlayWipe_set NOTIFY kernelOverlayWipe_changed);
    Q_PROPERTY(bool kernelIsAthena READ kernelIsAthena_get CONSTANT);
    Q_PROPERTY(bool kernelWasCrashed READ kernelWasCrashed_get CONSTANT);

    QList<int> s_cpuUndervolts = {-100, -75, -50, -25, 0, 25, 50, 75};
    QStringList s_cpuUndervolts_files = {
        "dtb/zero-sugar_-100mV.dtb", "dtb/zero-sugar_-75mV.dtb", "dtb/zero-sugar_-50mV.dtb",
        "dtb/zero-sugar_-25mV.dtb", "dtb/zero-sugar_0mV.dtb", "dtb/zero-sugar_25mV.dtb",
        "dtb/zero-sugar_50mV.dtb", "dtb/zero-sugar_75mV.dtb"};
    QString s_zero_sugar_dtb_path = "boot/athena/zero-sugar.dtb";
    QString s_last_log_path = "var/log/kdump/last_log_name";
    
    int m_cpuUndervolt;
    
    bool m_kernelBootAthena;
    bool m_kernelOverlayWipe;
    bool m_kernelIsAthena;
    bool m_kernelWasCrashed;

private:
    const char* athenaPath(const QString& path) {
        return QString(sysInfo.athenaRoot + path).toStdString().c_str();
    }
public:
    // CPU Undervolt
    int cpuUndervolt_get() {
        char buf[1024];
        readlink(athenaPath(s_zero_sugar_dtb_path), buf, sizeof(buf));
        for (auto name : s_cpuUndervolts_files) {
            if (strcmp(buf, name.toStdString().c_str()) != -1) {
                m_cpuUndervolt = s_cpuUndervolts[s_cpuUndervolts_files.indexOf(name)];
                
                return m_cpuUndervolt;
            }
        }
        return -9999;
    }
    void cpuUndervolt_set(int val) {
        int ix = s_cpuUndervolts.indexOf(val);
        if ((ix > 0) && (val != m_cpuUndervolt)) {
            remove(athenaPath(s_zero_sugar_dtb_path));
            symlink(s_cpuUndervolts_files.at(ix).toStdString().c_str(), athenaPath(s_zero_sugar_dtb_path));

            emit cpuUndervolt_changed(val);
        }
    }
    
    // Bootloader
    bool kernelBootAthena_get() {
        m_kernelBootAthena = (system("fw_printenv athena_fail | grep athena_fail=0 1>/dev/null 2>/dev/null")==0);
        
        return m_kernelBootAthena;
    }
    void kernelBootAthena_set(bool val) {
        if (val) {
            system("fw_setenv athena_fail 0");
        } else {
            system("fw_setenv athena_fail 1");
        }

        emit kernelBootAthena_changed(val);
    }
    bool kernelIsAthena_get() {
        return sysInfo.athenaIsRunning;
    }
    
    // Wipe
    bool kernelOverlayWipe_get() {
        m_kernelOverlayWipe = (access(athenaPath("wipe_me"), F_OK)==0);
        return m_kernelOverlayWipe;
    }
    void kernelOverlayWipe_set(bool val) {
        if (val) {
            Utils::write("", athenaPath("wipe_me"));
        } else {
            remove(athenaPath("wipe_me"));
        }

        emit kernelOverlayWipe_changed(val);
    }
    
    // Kdump
    bool kernelWasCrashed_get() {
        m_kernelWasCrashed = (access(athenaPath(s_last_log_path), F_OK)==0);
        
        return m_kernelWasCrashed;
    }
    void kernelWasCrashed_set(bool val) {
        if (!val) {
            remove(athenaPath(s_last_log_path));
        }

        emit kernelWasCrashed_changed(val);
    }

    AthenaKernel() : QObject(nullptr) {
    }
signals:
    void cpuUndervolt_changed(int);
    void kernelBootAthena_changed(bool);
    void kernelOverlayWipe_changed(bool);
    void kernelWasCrashed_changed(bool);
};

#endif //__ATHENAKERNEL_H__
