#!/bin/sh

while [ true ];do
/bin/sleep 1.2


if [ -d "/sys/kernel/config/usb_gadget/demo/functions/ffs.adb" ]

then
	read string < /sys/kernel/config/usb_gadget/demo/UDC

	if [ -z "$string" ]

	then
		echo "13500000.otg" > /sys/kernel/config/usb_gadget/demo/UDC
	fi

fi
done

