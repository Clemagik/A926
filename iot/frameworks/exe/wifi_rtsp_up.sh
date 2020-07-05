#!/bin/sh
ifconfig wlan0 down
ifconfig wlan0 up
ifconfig lo up

ifconfig wlan0 192.168.1.200 netmask 255.255.255.0

wpa_supplicant -Dnl80211 -iwlan0 -c/etc/wpa_supplicant.conf -B

ifconfig wlan0 down
ifconfig wlan0 up

ifconfig wlan0 192.168.1.200 netmask 255.255.255.0
