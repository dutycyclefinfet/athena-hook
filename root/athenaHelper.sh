#!/bin/bash

if [ "$1" == "--initialize-athena" ]; then
    # Remove stuff unnecessary on Athena from xochitl.service:
    sed -i '/Environment=.*$/d' /lib/systemd/system/xochitl.service
    sed -i '/StartLimitInterval=.*$/d' /lib/systemd/system/xochitl.service
    sed -i '/OnFailure=.*$/d' /lib/systemd/system/xochitl.service
    sed -i '/WatchdogSec=.*$/d' /lib/systemd/system/xochitl.service
    
    # Reload ldconfig
    ldconfig
    
    # Remove stuff unnecessary on Athena from xochitl.service:
    systemctl daemon-reload
    systemctl enable adjust-epd
    systemctl enable rm2fb
    systemctl enable zram
    
    # Switch sshd
    systemctl enable sshd
    systemctl enable sshd-watchdog
    systemctl start sshdgenkeys
    systemctl disable dropbear.socket
    
    # Start/stop services
    systemctl stop xochitl
    systemctl stop rm2fb
    systemctl start adjust-edp
    systemctl start zram
    systemctl start rm2fb
    systemctl stop dropbear.socket
    killall dropbear 2>/dev/null
    killall -0 dropbear && sleep 5 ; killall -9 dropbear 2>/dev/null
    systemctl start sshd
    systemctl start sshd-watchdog
    systemctl start xochitl
    
    # Disable self
    systemctl disable athenaInitialize
    exit 0
fi
if [ "$1" == "--adjust-epd" ]; then
    vcom_path="/sys/"
    for f in $(find /sys/bus/i2c/drivers/sy7636a/*/vadj) ; do
        [ -f /etc/athena/epd ] && cat /etc/athena/epd > $f
    done
fi
if [ "$1" == "--pre-set-environment" ]; then
    grep '#' /etc/environments.d/$2 > /tmp/athenaHelper.tmp0
    grep '#' /etc/environments.d/$2 | sed 's|^#||g' > /tmp/athenaHelper.tmp1
    bash /tmp/athenaHelper.tmp1
    cat /tmp/athenaHelper.tmp0 >> /etc/environments.d/$2
    rm /tmp/athenaHelper.tmp0 /tmp/athenaHelper.tmp1
fi
if [ "$1" == "--reconfigure-zram" ]; then
    swapon | grep zram0 > /dev/null && swapoff /dev/zram0
    [ -f /etc/athena/zram ] && zram=( $(cat /etc/athena/zram) ) || zram=0
    echo ${zram}M > /sys/block/zram0/disksize
    mkswap /dev/zram0
    swapon /dev/zram0
fi
if [ "$1" == "--sshd-watchdog" ]; then
    while true ; do
        sleep 10
        [ ! -f /tmp/sshd.pid ] && touch /tmp/sshd.pid

        for oldPID in $(cat /tmp/sshd.pid | grep -oE '[0-9]+:' | grep -oE '[0-9]+'); do
            kill -0 ${oldPID} 2>/dev/null || sed -i "/${oldPID}:/d" /tmp/sshd.pid
        done
    done
fi
