#!/bin/sh

#set -m
while [ true ]
do
case "$1" in
    440)
	    mount -t functionfs adb /dev/usb-ffs/adb
	    /sbin/adbd &
	    sleep 1
	    echo 13500000.otg > /sys/kernel/config/usb_gadget/demo/UDC
	    wait
	    umount /dev/usb-ffs/adb
	    ;;
    310)
	    /sbin/adbd
	    wait
	    ;;
    *)
	    echo "Usage: $0 {440|310}"
	    exit 1
esac

done
