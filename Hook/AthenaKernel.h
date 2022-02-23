#ifndef __ATHENAKERNEL_H__
#define __ATHENAKERNEL_H__

#include <QObject>
#include <QDir>
#include <QFileInfo>
#include "AthenaBase.h"

class AthenaKernel : public QObject, public AthenaBase
{
    Q_OBJECT
    // CPU
    Q_PROPERTY(int cpuUndervolt READ cpuUndervolt_get WRITE cpuUndervolt_set NOTIFY cpuUndervolt_changed);
    Q_PROPERTY(QList<int> cpuUndervolts READ cpuUndervolts_get NOTIFY cpuUndervolts_changed);
    Q_PROPERTY(int epdCurrentVoltage READ epdCurrentVoltage_get NOTIFY epdCurrentVoltage_changed);
    Q_PROPERTY(int epdAdjustmentVoltage READ epdAdjustmentVoltage_get WRITE epdAdjustmentVoltage_set NOTIFY epdAdjustmentVoltage_changed);
    Q_PROPERTY(int cpuCurrentVoltage READ cpuCurrentVoltage_get NOTIFY cpuCurrentVoltage_changed);
    Q_PROPERTY(int cpu0Frequency READ cpu0Frequency_get NOTIFY cpu0Frequency_changed);
    Q_PROPERTY(int cpu1Frequency READ cpu1Frequency_get NOTIFY cpu1Frequency_changed);
    // Kernel
    Q_PROPERTY(bool bootAthena READ bootAthena_get WRITE bootAthena_set NOTIFY bootAthena_changed);
    Q_PROPERTY(bool overlayWipe READ overlayWipe_get WRITE overlayWipe_set NOTIFY overlayWipe_changed);
    Q_PROPERTY(bool isAthena READ isAthena_get CONSTANT);
    Q_PROPERTY(bool wasCrashed READ wasCrashed_get CONSTANT);
    
    const QString s_undervolt_dtb_dir = "boot/dtb";
    const QString s_undervolt_dtb_link_prefix = "dtb/";
    const QString s_zero_sugar_dtb_path = "boot/zero-sugar.dtb";
    const QString s_last_log_path = "var/log/kdump/last_log_name";
    const QString s_current_cpu_voltage_path = "/sys/class/regulator/regulator.9/microvolts";
    const QString s_current_epd_voltage_path = "/sys/class/regulator/regulator.24/microvolts";
    const QString s_epd_vadj_path = "/etc/athena/epd";
    const QString s_current_cpu0_frequency_path = "/sys/bus/cpu/devices/cpu0/cpufreq/cpuinfo_cur_freq";
    const QString s_current_cpu1_frequency_path = "/sys/bus/cpu/devices/cpu1/cpufreq/cpuinfo_cur_freq";
    
    QList<int> s_cpuUndervolts;
    QStringList s_cpuUndervolts_files;
    int m_cpuUndervolt;
    
    int m_cpuVoltage;
    int m_cpu0Frequency;
    int m_cpu1Frequency;
    
    int m_epdVoltage;
    int m_epdAdjVoltage;
    
    bool m_bootAthena;
    bool m_overlayWipe;
    bool m_isAthena;
    bool m_wasCrashed;

private:
    void _getUndervolts() {
        if (s_cpuUndervolts.size() == 0) {
            QDir dtbDir(athenaPath(s_undervolt_dtb_dir));
            QFileInfoList fileInfo = dtbDir.entryInfoList();
            for (int i = 0; i < fileInfo.size(); ++i) {
                auto fName = fileInfo[i].fileName();
                const QRegExp match("\\w+_(-?[0-9]+)mV\\.dtb");
                
                if (match.exactMatch(fName)) {
                    s_cpuUndervolts << match.cap(1).toInt();
                    s_cpuUndervolts_files << s_undervolt_dtb_link_prefix + fName;
                }
            }
        }
    }
    
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
    QList<int> cpuUndervolts_get() {
        _getUndervolts();
        
        return s_cpuUndervolts;
    }
    int cpuUndervolt_get() {
        _getUndervolts();
        
        QString current_undervolt = readLinkC(athenaPath(s_zero_sugar_dtb_path));
        int ix = s_cpuUndervolts_files.indexOf(current_undervolt);
        m_cpuUndervolt = s_cpuUndervolts.at(ix != -1 ? ix : 0);
        return m_cpuUndervolt;
    }
    void cpuUndervolt_set(int val) {
        _getUndervolts();
        
        int ix = s_cpuUndervolts.indexOf(val);
        if ((ix != -1) && (val != m_cpuUndervolt)) {
            QFile::remove(athenaPath(s_zero_sugar_dtb_path));
            symLinkC(s_cpuUndervolts_files.at(ix), athenaPath(s_zero_sugar_dtb_path));

            emit cpuUndervolt_changed(val);
        }
    }

    int epdAdjustmentVoltage_get() {
        QString buf;
        readFile(buf, s_epd_vadj_path, " ");
        
        m_epdAdjVoltage = buf.toInt()/1000;
        return m_epdAdjVoltage;
    }
    void epdAdjustmentVoltage_set(int val) {
        if ((val>-500) && (val<500)) {
            writeFile(QString::number(val*1000), s_epd_vadj_path);
        }
        
        emit epdAdjustmentVoltage_changed(epdAdjustmentVoltage_get());
        emit epdCurrentVoltage_changed(epdCurrentVoltage_get());
    }

    int epdCurrentVoltage_get() {
        QString buf;
        readFile(buf, s_current_epd_voltage_path, " ");
        
        m_epdVoltage = -buf.toInt();
        return m_epdVoltage;
    }
    
    int cpuCurrentVoltage_get() {
        QString buf;
        readFile(buf, s_current_cpu_voltage_path, " ");
        m_cpuVoltage = buf.toInt();
        return m_cpuVoltage;
    }

    int cpu0Frequency_get() {
        QString buf;
        readFile(buf, s_current_cpu0_frequency_path, " ");
        m_cpu0Frequency = buf.toInt();
        return m_cpu0Frequency;
    }

    int cpu1Frequency_get() {
        QString buf;
        readFile(buf, s_current_cpu1_frequency_path, " ");
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
            writeFile("", athenaPath("wipe_me"));
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
        addFileMonitor(s_zero_sugar_dtb_path, cpuUndervolt);
        addSysfsMonitor(s_current_cpu_voltage_path, cpuCurrentVoltage);
        addSysfsMonitor(s_current_epd_voltage_path, epdCurrentVoltage);
        addSysfsMonitor(s_current_cpu0_frequency_path, cpu0Frequency);
        addSysfsMonitor(s_current_cpu1_frequency_path, cpu1Frequency);
    }
signals:
    void epdAdjustmentVoltage_changed(int);
    void epdCurrentVoltage_changed(int);
    void cpuUndervolt_changed(int);
    void cpuUndervolts_changed(int);
    void cpuCurrentVoltage_changed(int);
    void cpu0Frequency_changed(int);
    void cpu1Frequency_changed(int);
    void bootAthena_changed(bool);
    void overlayWipe_changed(bool);
    void wasCrashed_changed(bool);
};

#endif //__ATHENAKERNEL_H__
