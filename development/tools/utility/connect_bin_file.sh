#  this script will combine two or more files into one file at the specified offset.
#
# yolanda lee 2016.5.25

#!/bin/sh

pwd=$PWD
F_out=out.bin
#
read -p "Input the number of files to be connected: " COUNT
read -p "Input the name of File1: " F1

a=2
while(($a<=$COUNT))
do
	echo "*******************************************************************************"
	echo "**Please input the name of File$a!"
	read F_name

	echo "## Please input the offset of File$a! eg: Enter 40000 for offset 0x40000"
	read offset

	i=$((16#$offset))
	F_SIZE=$(($i/1024))

	echo "F_SIZE : $F_SIZE"
	echo "F_name : $F_name"

	dd bs=1024 count=$F_SIZE if=/dev/zero of=$(pwd)/junk1
	cat $(pwd)/$F1 $(pwd)/junk1 > $(pwd)/junk2
	dd bs=1024 count=$F_SIZE if=$(pwd)/junk2 of=$(pwd)/junk3
	cat $(pwd)/junk3 $F_name > $F_out

	F1=$F_out

	rm -rf $(pwd)/junk*
	let "a++"
done
