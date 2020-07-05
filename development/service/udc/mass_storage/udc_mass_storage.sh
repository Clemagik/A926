#!/bin/sh

if [ $# -lt 2 ]
then
	echo "Usage:	udc_mass_storage <device file, such as:/dev/ram0> <obj dir, such as:/mnt/udc> &"
	exit 1
fi

mount_dev()
{
	buf=`mount|grep "$dev_file on $obj_dir"`
	if [ -z "$buf" ]
	then
		mkdir -p $obj_dir
		count=5
		while [ $count -gt 0 ]
		do
			mount $dev_file $obj_dir
			if [ $? -eq 0 ]; then
				break
			fi
			count=$((count - 1))
		done
		if [ $count -eq 0 ]
		then
			echo "notice : mount $dev_file failed!!!"
			exit 1
		fi
	fi
}

mount_func() {
	while true
	do
		if [ ! -f /usr/bin/udc_mass_storage ]
		then
			exit 1
		fi
		/usr/bin/udc_mass_storage
		case "$?" in
			0)
				echo "usb mass storage configure"
				echo $dev_file > /sys/class/android_usb/f_mass_storage/lun/file
				mount $obj_dir -o remount -r
				;;
			1)
				echo "usb mass storage disconnect"
				mount $obj_dir -o remount -rw
				;;
			*)
				;;
		esac
	done
}


dev_format()
{
	mount_dev
	buf=`mount|grep "$dev_file"|awk 'NR==1 {print $5}'`
	echo "------------->$buf"
	if [ "$buf" == "vfat" ] ;then
		{
			return
		}
	else
		{
			echo "NOTICE: this device need to be formatted for VFATS.  you confirm to format device ?"
			read -p "Please input (Y/N): " yn
			if [ "$yn" == "Y" ] || [ "$yn" == "y" ]; then
				echo "OK, continue"
				echo "Please wait format device ....."
				umount $obj_dir
				count=5
				while [ $count -gt 0 ]
				do
					mkfs.fat $dev_file
					if [ $? -eq 0 ];then
						break
					fi
					count=$((count - 1))
				done
			else
				echo "NOTICE :mass storage functions support only VFAT format !!! "
				echo "if there is need to add your format, please modify this file !!!"
				exit 1
			fi
		}
	fi
}

init_func()
{
	if [ ! -e $dev_file ]
	then
		echo "notice : $dev_file don't exit, please config kernel !!!"
		exit 1
	fi

	if [ ! -d $obj_dir ]
	then
		echo "notice : $obj_dir don't exit, please create it!!!"
		exit 1
	fi

	buf=`mount|grep "$dev_file"`
	echo "1-------->$buf"
	[ ! -z "$buf" ] && {
	echo "notice : $dev_file have been mounted!!!"
	exit 1
}
}

dev_file=$1
obj_dir=$2
main()
{
	init_func
	dev_format
	echo $dev_file > /sys/class/android_usb/f_mass_storage/lun/file
	mount_func
}

main
