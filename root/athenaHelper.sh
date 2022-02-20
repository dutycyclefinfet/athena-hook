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
