#!/bin/sh

prg_name=$0
partition_name=$1
dev_path=$1
mount_path=$2
try_count=$3

# 显示错误和使用方法并退出
usage_exit()
{
	echo $1
	echo "usage: $prg_name partition_name/dev_path mount_path"
	exit 1
}

# 显示错误并退出
error_exit()
{
	echo $1
	exit 1
}

# 参数个数必须等于2
if [ "$mount_path" = "" ]; then
	usage_exit "error: too few args"
fi

# 参数个数必须等于2或者3
if [ "$4" != "" ]; then
	 usage_exit "error: too many args"
fi

# 优先匹配分区名
num=`cat /proc/mtd | grep "\"$partition_name\"" | cut -d ":" -f 1`
num=${num#*mtd}

if [ "$num" = "" ]; then
	# 获得最标准的绝对路径，如果传进来的是一个路径
	dev_path=`readlink -f $dev_path`

	# 匹配 /dev/mtdN 或者 /dev/mtdblockN 这两种情况
	# 不支持 /dev/mtdNro /dev/mtdblockNro
	if [ "${dev_path:0:5}" = "/dev/" ]; then
		tmp0=${dev_path:5}
		tmp1=${tmp0#mtd}
		let tmp11=tmp1+0

		tmp2=${tmp0#mtdblock}
		let tmp22=tmp2+0

		if [ "$tmp1" = "$tmp11" ]; then
			num=$tmp1
		fi

		if [ "$tmp2" = "$tmp22" ]; then                 
		        num=$tmp2
		fi
	 
		if [ "$num" = "" ]; then
			usage_exit "error: only support mtd device"
		fi
	fi

	if [ "$num" = "" ]; then
		error_exit "error: can not find the mtd partition $partition_name"
	fi
fi


if [ ! -e "/dev/mtd$num" ]; then
	error_exit "error: why /dev/mtd$num is not exist ?"
fi


ubi_name=

# 查看系统是否已经识别到了mtd分区里面的ubi分区
for ubi in `ls /sys/devices/virtual/ubi/`
do
	if [ "`cat /sys/devices/virtual/ubi/$ubi/mtd_num`" = "$num" ]; then
		ubi_name=$ubi
	fi
done

if [ "$ubi" = "" ]; then
	ubi = 0
fi

# 如果没有，那么为这个mtd分区创建一个ubi的序号
if [ "$ubi_name" = "" ]; then
	ubi=${ubi#ubi}
	let ubi=ubi+1

	ubi_name=ubi$ubi


	ubiattach -m $num -d $ubi
	if [ $? != 0 ]; then
		flash_erase /dev/mtd$num 0 0
		ubiattach -m $num -d $ubi
	fi
fi

count=0

while true;
do
	if [ -e "/dev/$ubi_name" ]; then
		break;
	fi


	let count=count+1
	if [ "$count" = "10" ]; then
        error_exit "error: why /dev/$ubi_name is not exist ?"
	fi

	sleep 1
done

if [ ! -e "/dev/$ubi_name" ]; then
        error_exit "error: why /dev/$ubi_name is not exist ?"
fi

# 如果mtd分区里面没有ubi文件系统，那么格式化一个
if [ ! -e "/dev/${ubi_name}_0" ]; then
	ubimkvol /dev/$ubi_name -N user_data -m
	sleep 1
fi

mkdir -p $mount_path
mount -o sync -t ubifs /dev/${ubi_name}_0 $mount_path
if [ $? != 0 ]; then
	if [ "$try_count" != "" ]; then
		error_exit "mount ubi error again"
	fi
	# ubi的分区能识别但是mount不了的情况
	# 擦除分区,重试一次
	flash_erase /dev/mtd$num 0 0
	ubidetach -m $num
	$prg_name $partition_name $mount_path 1
fi