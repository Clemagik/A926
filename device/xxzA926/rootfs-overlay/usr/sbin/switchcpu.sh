#!/bin/sh
while true;do
	  ls &
	  ps &
	  ls &
	  ps &
	 ls &
	  ps &
	 ls &
	  ps &

	  echo 0 > /sys/devices/system/cpu/cpu1/online
	  ps
	  echo 0 > /sys/devices/system/cpu/cpu2/online
	  ps
	  echo 0 > /sys/devices/system/cpu/cpu3/online
	  ls
	  echo 1 > /sys/devices/system/cpu/cpu2/online
	  ps
	  echo 1 > /sys/devices/system/cpu/cpu1/online
	  ls
	  echo 1 > /sys/devices/system/cpu/cpu3/online
	  ps
	  echo 0 > /sys/devices/system/cpu/cpu3/online
	  ls
	  echo 1 > /sys/devices/system/cpu/cpu3/online
	  ps
done




