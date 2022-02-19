#ifndef __ATHENASETTINGS_H__
#define __ATHENASETTINGS_H__

#include <QObject>
#include <QString>
#include <QStringList>
#include "AthenaBase.h"

class AthenaSettings : public QObject, public AthenaBase
{
    Q_OBJECT
    // USB
    Q_PROPERTY(QStringList usbModes READ usbModes_get CONSTANT);
    Q_PROPERTY(QString usbMode READ usbMode_get WRITE usbMode_set NOTIFY usbMode_changed);
    // CPU
    Q_PROPERTY(QStringList cpuGovernors READ cpuGovernors_get CONSTANT);
    Q_PROPERTY(QString cpuGovernor READ cpuGovernor_get WRITE cpuGovernor_set NOTIFY cpuGovernor_changed);
    // ZRAM
    Q_PROPERTY(int zRAM READ zRAM_get WRITE zRAM_set NOTIFY zRAM_changed);
    // Battery
    Q_PROPERTY(int batteryLimit READ batteryLimit_get WRITE batteryLimit_set NOTIFY batteryLimit_changed);

    const QString s_usb_path = "etc/athena/usb";
    const QString s_usb_modes_path = "etc/athena/usb_modes";
    const QString s_zram_path = "etc/athena/zram";
    const QString s_cpu0_available_governors_path = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors";
    const QString s_cpu0_governor_path = "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor";
    const QString s_cpu1_governor_path = "/sys/devices/system/cpu/cpu1/cpufreq/scaling_governor";

    QStringList m_usbModes;
    QString m_usbMode;
    
    QStringList m_cpuGovernors;
    QString m_cpuGovernor;
    
    int m_zRAM;
    
    int m_batteryLimit;

private:
    QProcessRet _restartUsbDaemon() {
        return runProcess("/bin/systemctl", "restart", "athena-usb-daemon");
    }

public:
    // USB
    QStringList usbModes_get() {
        m_usbModes.clear();
        readAndSplit(m_usbModes, athenaPath(s_usb_modes_path), "|");
        
        return m_usbModes;
    };
    QString usbMode_get() {
        read(m_usbMode, athenaPath(s_usb_path), "|");
        return m_usbMode;
    }
    void usbMode_set(QString val) {
        QString old_val = usbMode_get();
        if (val != old_val) {
            write(val, athenaPath(s_usb_path));

            if (AthenaBase::isAthenaRunning()) {
                _restartUsbDaemon();
            }
        }

        emit usbMode_changed(val);
    }
    
    // CPU
    QStringList cpuGovernors_get() {
        m_cpuGovernors.clear();
        readAndSplit(m_cpuGovernors, s_cpu0_available_governors_path, " ");
        return m_cpuGovernors;
    };
    QString cpuGovernor_get() {
        m_cpuGovernor = "";
        read(m_cpuGovernor, s_cpu0_governor_path, " ");
        return m_cpuGovernor;
    }
    void cpuGovernor_set(QString val) {
        write(val, s_cpu0_governor_path);
        write(val, s_cpu1_governor_path);

        emit cpuGovernor_changed(val);
    }
    
    // ZRAM
    int zRAM_get() {
        QString buf;
        read(buf, athenaPath(s_zram_path), " ");
        m_zRAM = buf.toInt();
        return m_zRAM;
    }
    void zRAM_set(int val) {
        write(QString::number(val), athenaPath(s_zram_path));

        emit zRAM_changed(val);
    }
    
    // Battery
    int batteryLimit_get() {
        return m_batteryLimit;
    }
    void batteryLimit_set(int val) {
        if ((val > 0) && (val < 100)) {
            m_batteryLimit = val;
        }

        emit batteryLimit_changed(val);
    }

    AthenaSettings() : QObject(nullptr) {
        cpuGovernors_get();
    }
signals:
    void usbMode_changed(QString);
    void cpuGovernor_changed(QString);
    void zRAM_changed(int);
    void batteryLimit_changed(int);
};

#endif //__ATHENASETTINGS_H__
