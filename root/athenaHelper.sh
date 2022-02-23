#!/bin/bash

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
