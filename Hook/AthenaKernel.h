#ifndef __ATHENAKERNEL_H__
#define __ATHENAKERNEL_H__

#include <QObject>
#include "AthenaBase.h"
#include "Utils.h"

class AthenaKernel : public QObject, public AthenaBase
{
    Q_OBJECT
    // CPU
    Q_PROPERTY(int cpuUndervolt READ cpuUndervolt_get WRITE cpuUndervolt_set NOTIFY cpuUndervolt_changed);
    Q_PROPERTY(int cpuCurrentVoltage READ cpuCurrentVoltage_get NOTIFY cpuCurrentVoltage_changed);
    Q_PROPERTY(int cpu0Frequency READ cpu0Frequency_get NOTIFY cpu0Frequency_changed);
    Q_PROPERTY(int cpu1Frequency READ cpu1Frequency_get NOTIFY cpu1Frequency_changed);
    // Kernel
    Q_PROPERTY(bool bootAthena READ bootAthena_get WRITE bootAthena_set NOTIFY bootAthena_changed);
    Q_PROPERTY(bool overlayWipe READ overlayWipe_get WRITE overlayWipe_set NOTIFY overlayWipe_changed);
    Q_PROPERTY(bool isAthena READ isAthena_get CONSTANT);
    Q_PROPERTY(bool wasCrashed READ wasCrashed_get CONSTANT);

    const QList<int> s_cpuUndervolts = {-100, -75, -50, -25, 0, 25, 50, 75};
    const QStringList s_cpuUndervolts_files = {
        "dtb/zero-sugar_-100mV.dtb", "dtb/zero-sugar_-75mV.dtb", "dtb/zero-sugar_-50mV.dtb",
        "dtb/zero-sugar_-25mV.dtb", "dtb/zero-sugar_0mV.dtb", "dtb/zero-sugar_25mV.dtb",
        "dtb/zero-sugar_50mV.dtb", "dtb/zero-sugar_75mV.dtb"};
    const QString s_zero_sugar_dtb_path = "boot/zero-sugar.dtb";
    const QString s_last_log_path = "var/log/kdump/last_log_name";
    const QString s_current_cpu_voltage_path = "/sys/class/regulator/regulator.9/microvolts";
    const QString s_current_cpu0_frequency_path = "/sys/bus/cpu/devices/cpu0/cpufreq/cpuinfo_cur_freq";
    const QString s_current_cpu1_frequency_path = "/sys/bus/cpu/devices/cpu1/cpufreq/cpuinfo_cur_freq";
    
    int m_cpuUndervolt;
    int m_cpuVoltage;
    int m_cpu0Frequency;
    int m_cpu1Frequency;
    
    bool m_bootAthena;
    bool m_overlayWipe;
    bool m_isAthena;
    bool m_wasCrashed;

private:
    bool _isAthenaBoot() {
        // auto process = runProcess("/sbin/fw_printenv", "athena_fail");

        return (system("fw_printenv athena_fail | grep athena_fail=0 1>/dev/null 2>/dev/null")==0);
    }

    void _setAthenaBoot(bool boot) {
        // QString val = boot? "0" : "1";
        // runProcess("/sbin/fw_setenv", "athena_fail", val);
        if (boot) {
            system("fw_setenv athena_fail 0");
        } else {
            system("fw_setenv athena_fail 1");
        }
    }
public:
    // CPU
    int cpuUndervolt_get() {
        QString current_undervolt = Utils::readLink(athenaPath(s_zero_sugar_dtb_path));

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
            Utils::symLink(s_cpuUndervolts_files.at(ix), athenaPath(s_zero_sugar_dtb_path));

            emit cpuUndervolt_changed(val);
        }
    }

    int cpuCurrentVoltage_get() {
        QString buf;
        Utils::read(buf, s_current_cpu_voltage_path, " ");
        m_cpuVoltage = buf.toInt();
        return m_cpuVoltage;
    }

    int cpu0Frequency_get() {
        QString buf;
        Utils::read(buf, s_current_cpu0_frequency_path, " ");
        m_cpu0Frequency = buf.toInt();
        return m_cpu0Frequency;
    }

    int cpu1Frequency_get() {
        QString buf;
        Utils::read(buf, s_current_cpu1_frequency_path, " ");
        m_cpu1Frequency = buf.toInt();
        return m_cpu1Frequency;
    }
    
    // Bootloader
    bool bootAthena_get() {
        m_bootAthena = _isAthenaBoot();
        
        return m_bootAthena;
    }
    void bootAthena_set(bool val) {
        _setAthenaBoot(val);

        emit bootAthena_changed(val);
    }
    bool isAthena_get() {
        return AthenaBase::isAthenaRunning();
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
    void cpuCurrentVoltage_changed(int);
    void cpu0Frequency_changed(int);
    void cpu1Frequency_changed(int);
    void bootAthena_changed(bool);
    void overlayWipe_changed(bool);
    void wasCrashed_changed(bool);
};

#endif //__ATHENAKERNEL_H__
