#!/bin/bash

if [ "$1" == "--adjust-epd" ]; then
    vcom_path="/sys/"
    for f in $(find /sys/bus/i2c/drivers/sy7636a/*/vadj) ; do
        [ -f /etc/athena/epd ] && cat /etc/athena/epd > $f
    done
fi
if [ "$1" == "--pre-set-environment" ]; then
    /etc/environments.d/$2
fi
