#!/bin/bash

if [ "$1" == "--adjust-epd" ]; then
    vcom_path="/sys/"
    for f in $(find /sys/bus/i2c/drivers/sy7636a/*/vadj) ; do
        [ -f /etc/athena/epd ] && cat /etc/athena/epd > $f
    done
fi
if [ "$1" == "--pre-set-environment" ]; then
    cmd=$(grep '#' /etc/environments.d/$2 | tr '#' ' ; ')
    bash -c "echo Executing reconfig for $2 now. ${cmd}"
fi
