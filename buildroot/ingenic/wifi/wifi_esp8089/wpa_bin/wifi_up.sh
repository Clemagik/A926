#!/bin/sh
rfkill unblock wifi
ifconfig wlan0 up

wpa_supplicant -B -i wlan0 -c ${env_wifi_wpa_supplicant_conf} &
udhcpc -i wlan0 &


