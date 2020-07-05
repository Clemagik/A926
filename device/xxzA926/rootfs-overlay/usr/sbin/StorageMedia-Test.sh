#!/bin/bash


function GetRandomSize()
{
	BIGFILE_LIMIT=`busybox expr $DATA_FREECAP \/ 5`
	# RandomNum=$(cat /dev/urandom | head -n 10 | cksum | awk -F ' ' '{print $1}')
	RandomNum=$(cat /dev/urandom | head -n 10 | cksum)
	RandomNum=${RandomNum%% *}
	RANDOM_SIZE=$(($RandomNum%$BIGFILE_LIMIT+$MINISIZE))
	RANDOM_SIZE_UN=${RANDOM_SIZE#-}

	echo "NAND_COPY_TEST: random size: $RANDOM_SIZE_UN"
	if [ $RANDOM_SIZE_UN -gt 52428800 ];then
		RANDOM_SIZE_UN=52428800
	fi
	return $RANDOM_SIZE_UN
}



function GetBigfile()
{
	echo "random file is $FILE_PATH, bs=$BS"
	busybox dd if=/dev/urandom of=$FILE_PATH bs=$BS count=1 conv=fsync > /dev/null 2>&1
	sync
	if [ -f $FILE_PATH ]; then
		# FILESIZE=`ls -l $FILE_PATH | busybox awk '{print $5}'`
		FILESIZE=`ls -l $FILE_PATH`
		FILESIZE="${FILESIZE% *}"
		FILESIZE="${FILESIZE% *}"
		FILESIZE="${FILESIZE% *}"
		FILESIZE="${FILESIZE% *}"
		FILESIZE="${FILESIZE% *}"
		FILESIZE="${FILESIZE##* }"
		if [ $FILESIZE -eq 0 ]; then
			echo "NAND_COPY_TEST: ERROR: $FILE_PATH size is 0, exit::::$CNT" > /dev/console
			exit
		fi
	else
		echo "NAND_COPY_TEST: error, can't creat $FILE_PATH, exit::::$CNT" > /dev/console
		exit
	fi
}

function CopyBigfile()
{
	case $1 in
		"nTon" )
			busybox dd if=$FILE_NAND of=/tmpfile bs=$BS count=1 > /dev/null 2>&1
			sync
			busybox dd if=/tmpfile of=$FILE_NAND_BACK bs=$BS count=1 > /dev/null 2>&1
			;;
		"sTos" )
			busybox dd if=$FILE_SDCARD of=/tmpfile bs=$BS count=1 conv=fsync > /dev/null 2>&1
			sync
			busybox dd if=/tmpfile of=$FILE_SDCARD_BACK bs=$BS count=1 conv=fsync > /dev/null 2>&1
			;;
		"dTod" )
			cp $FILE_DDR $FILE_DDR_BACK
			;;
		"sTon" )
			cp $FILE_SDCARD $FILE_NAND
			;;
		"nTos" )
			cp $FILE_NAND $FILE_SDCARD
			;;
		"nTod" )
			cp $FILE_NAND $FILE_DDR
			;;
		"dTon" )
			cp $FILE_DDR $FILE_NAND
			;;
		"sTod" )
			cp $FILE_SDCARD $FILE_DDR
			;;
		"dTos" )
			cp $FILE_DDR $FILE_SDCARD
			;;
		*) echo 'choice err ,exit!!'
			exit
			;;
	esac
}

function DiffFile()
{
	echo "difffile $1"
	case $1 in
		"nTon" )
			diff $FILE_NAND $FILE_NAND_BACK
			;;
		"sTos" )
			diff $FILE_SDCARD $FILE_SDCARD_BACK
			;;
		"dTod" )
			diff $FILE_DDR $FILE_DDR_BACK
			;;
		"sTon" )
			diff $FILE_SDCARD $FILE_NAND
			;;
		"nTos" )
			diff $FILE_NAND $FILE_SDCARD
			;;
		"nTod" )
			diff $FILE_NAND $FILE_DDR
			;;
		"dTon" )
			diff $FILE_DDR $FILE_NAND
			;;
		"sTod" )
			diff $FILE_SDCARD $FILE_DDR
			;;
		"dTos" )
			diff $FILE_DDR $FILE_SDCARD
			;;
		*) echo 'choice err ,exit!!'
			exit
			;;
	esac
	TMP=$?
	if [ $TMP -eq 0 ]; then
		echo "diff right "
		echo ""
	else
		echo " diff error::::$CNT "
		exit
	fi
}

function DelFile()
{
	if [ -f $FILE_SDCARD ];then
		rm $FILE_SDCARD
		echo "rm $FILE_SDCARD"
	fi
	if [ -f $FILE_NAND ];then
		rm $FILE_NAND
		echo "rm $FILE_NAND"
	fi
	if [ -f $FILE_DDR ];then
		rm $FILE_DDR
		echo " rm $FILE_DDR "
	fi
	if [ -f $FILE_SDCARD_BACK ];then
		rm $FILE_SDCARD_BACK
		echo "rm $FILE_SDCARD_BACK"
	fi
	if [ -f $FILE_NAND_BACK ];then
		rm $FILE_NAND_BACK
		echo "rm $FILE_NAND_BACK"
	fi
	if [ -f $FILE_DDR_BACK ];then
		rm $FILE_DDR_BACK
		echo "rm $FILE_DDR_BACK"
	fi
	if [ -f /tmpfile ];then
		rm /tmpfile
		echo "rm /tmpfile"
	fi
}

function PlusCount()
{
	CNT=`busybox expr $CNT + 1`
	echo "NAND_COPY_TEST: sleep 1s for recycle::::::::::::::::::::::::::::::::::::::::cnt=$CNT" > /dev/console
	sleep 1
}


function main()
{
	FILE_SDCARD=/mnt/sd/bigfile_s
	FILE_SDCARD_BACK=/mnt/sd/bigfile_back_s
	FILE_NAND=/bigfile_n  #nor/nand used
	FILE_NAND_BACK=/bigfile_back_n  #nor/nand used
	FILE_DDR=/tmp/bigfile_d
	FILE_DDR_BACK=/tmp/bigfile_back_d
	MINISIZE=1

	# DATA_FREECAP=`busybox df | busybox awk '{if($6=="/") print $4}'`
	DATA_FREECAP="$(busybox df | busybox sed -n '\/dev\/root/p;')"
	DATA_FREECAP="${DATA_FREECAP% *}"
	DATA_FREECAP="${DATA_FREECAP% *}"
	DATA_FREECAP="${DATA_FREECAP:0:-1}"
	DATA_FREECAP="${DATA_FREECAP##* }"
	DATA_FREECAP=`busybox expr $DATA_FREECAP \* 1024`

	DATA_FREECAP=$((200*1024*1024))

	# echo "**********************************************"
	# echo "* * Please input which module you want test ."
	# echo "**********************************************"
	# echo "* Enter  1 : Read/Write test in the nand/nor ."
	# echo "* Enter  2 : Read/Write test in the sdcard ."
	# echo "* Enter  3 : Read/Write test in the ddr ."
	# echo "* Enter  4 : Read/Write test between nand/nor and sdcard ."
	# echo "* Enter  5 : Read/Write test between nand/nor and ddr ."
	# echo "* Enter  6 : Read/Write test between ddr and sdcard ."
	# echo
	# if read -t 5  copy_module
	# then
	# 	echo ""
	# else
	# 	echo ""
	# 	echo "Enter default Module"
	# 	echo ""
	# 	copy_module=7
	# fi
	case $copy_module in
		1)  echo 'You select 1, nand/nor <-> nand/nor'
			FILE_PATH=$FILE_NAND
			ATOB=nTon
			BTOA=nTon
			echo
			;;
		2)  echo 'You select 2, sd <-> sd'
			FILE_PATH=$FILE_SDCARD
			ATOB=sTos
			BTOA=sTos
			echo
			;;
		3)  echo 'You select 3, ddr <-> ddr'
			FILE_PATH=$FILE_DDR
			ATOB=dTod
			BTOA=dTod
			echo
			;;
		4)  echo 'You select 4, sd <-> nand/nor'
			FILE_PATH=$FILE_SDCARD
			ATOB=sTon
			BTOA=nTos
			echo
			;;
		5)  echo 'You select 5, nand/nor <-> ddr'
			FILE_PATH=$FILE_NAND
			ATOB=nTod
			BTOA=dTon
			echo
			;;
		6)  echo 'You select 6, ddr <-> sd'
			FILE_PATH=$FILE_DDR
			ATOB=dTos
			BTOA=sTod
			echo
			;;
		7) echo 'Default test ->copy file in nand once!'
			FILE_PATH=$FILE_NAND
			ATOB=nTon
			BTOA=nTon
			GetRandomSize
			BS=$RANDOM_SIZE_UN
			GetBigfile
			CopyBigfile $ATOB
			sync
			DiffFile $ATOB
			CopyBigfile $BTOA
			sync
			DiffFile $BTOA
			DelFile
			exit
			;;
		*) echo 'CHOICE ERROR! Exit!!'
			exit
			;;
	esac
	# echo "***************************************************"
	# echo "* * Please choice which Size you want copy ."
	# echo "***************************************************"
	# echo "* Enter  1 : auto random Size "
	# echo "* Enter  2 : setting a fix Size "
	# echo
	# if read -t 5  test_module
	# then
	# 	echo ""
	# else
	# 	echo "Enter default Module"
	# 	test_module=1
	# fi
	case $test_module in
		1)  echo 'You select 1, RANDOM_SIZE'
			echo
			;;
		2)  echo 'You select 2, Fix SIZE'
			echo
			;;
		*)  echo 'choice err ,exit!!'
			exit
			;;
	esac
	DelFile
	if [ "$test_module" = "1" ]; then
		while [ 1 ]; do
			GetRandomSize
			BS=$RANDOM_SIZE_UN
			GetBigfile
			CopyBigfile $ATOB
			sync
			DiffFile $ATOB
			CopyBigfile $BTOA
			sync
			DiffFile $BTOA
			DelFile
			PlusCount
		done
	else
		# echo "***************************************************"
		# echo "please input how many Kbytes you want test at a time."
		# echo
		# read SIZE

		while [ 1 ];do
			# if [ $SIZE -gt $DATA_FREECAP ]; then
			# 	echo "you input $SIZE"
			# 	echo "the nand/nor free size is $DATA_FREECAP"
			# 	echo "there is not enough disk space, please input again ."
			# 	read SIZE
			# fi
			SIZE=1024
			KSIZE=`busybox expr $SIZE \* 1024`
			echo "NAND_COPY_TEST:  size = $KSIZE "
			BS=$KSIZE
			GetBigfile
			CopyBigfile $ATOB
			sync
			DiffFile $ATOB
			CopyBigfile $BTOA
			sync
			DiffFile $BTOA
			DelFile
			PlusCount
		done
	fi
}

# main
test_module=1
copy_module=3
CNT=0
main
# main end
