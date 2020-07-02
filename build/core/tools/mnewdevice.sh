#!/bin/bash

# input args [devicename]
#
#1. copy core/templates/device/template devicename
#2. replace vendorsetup.sh template to [devicename]
#3. replace template_base.mk	to [devicename]_base.mk
#4. show others template which need to be modified by manual.
#

if [ $# == 0 ]; then
echo "error args"
echo $0 [devicename]
exit
fi

newdevice=$1

if [ -e device/$newdevice ]; then
echo "$newdevice is already exist"
exit
fi

cp build/core/templates/device/template device/$newdevice -rf
mv device/$newdevice/template_base.mk device/$newdevice/${newdevice}_base.mk
sed -e "s/template/${newdevice}/g" device/${newdevice}/vendorsetup.sh -i

echo "******************** Attention **********************"
echo "Need to below such contents to construct a minimal device."
grep "template" device/${newdevice} -rn -B1 --color

echo "*****************************************************"
echo ""
echo "device: [$newdevice] created under device/$newdevice"
echo ""

tree device/$newdevice







