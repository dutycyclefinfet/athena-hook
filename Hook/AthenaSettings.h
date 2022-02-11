#ifndef __ATHENASETTINGS_H__
#define __ATHENASETTINGS_H__

#include <QObject>
#include <QVariant>
#include <stdio.h>
#include <unistd.h>
#include "Utils.h"

class AthenaSettings : public QObject
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

    QString s_usb_path = "etc/athena/usb";
    QString s_usb_modes_path = "etc/athena/usb_modes";
    QString s_zram_path = "etc/athena/zram";

    QStringList m_usbModes;
    QString m_usbMode;
    
    QStringList m_cpuGovernors;
    QString m_cpuGovernor;
    
    int m_zRAM;
    
    int m_batteryLimit;
private:
    const char* athenaPath(const QString& path) {
        return QString(sysInfo.athenaRoot + path).toStdString().c_str();
    }
public:
    // USB
    QStringList usbModes_get() {
        m_usbModes.clear();
        Utils::readAndSplit(m_usbModes, athenaPath(s_usb_modes_path), "|");
        
        return m_usbModes;
    };
    QString usbMode_get() {
        Utils::read(m_usbMode, athenaPath(s_usb_modes_path), " ");
        return m_usbMode;
    }
    void usbMode_set(QString val) {
        Utils::write(val, athenaPath(s_usb_modes_path));
        if ((m_usbMode != usbMode_get()) && sysInfo.athenaIsRunning) {
            system("systemctl restart athena-usb-daemon");
        }

        emit usbMode_changed(val);
    }
    
    // CPU
    QStringList cpuGovernors_get() {
        m_cpuGovernors.clear();
        Utils::readAndSplit(m_cpuGovernors, "/sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors", " ");
        return m_cpuGovernors;
    };
    QString cpuGovernor_get() {
        m_cpuGovernor = "";
        Utils::read(m_cpuGovernor, "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor", " ");
        return m_cpuGovernor;
    }
    void cpuGovernor_set(QString val) {
        Utils::write(val, "/sys/devices/system/cpu/cpu0/cpufreq/scaling_governor");
        Utils::write(val, "/sys/devices/system/cpu/cpu1/cpufreq/scaling_governor");

        emit cpuGovernor_changed(val);
    }
    
    // ZRAM
    int zRAM_get() {
    QString buf;
        Utils::read(buf, athenaPath(s_zram_path), " ");
        m_zRAM = QString(buf).toInt();
        return m_zRAM;
    }
    void zRAM_set(int val) {
        Utils::write(QString(val), athenaPath(s_zram_path));

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
