#!/bin/sh

# This file is executed by build/envsetup.sh, and can use anything
# defined in envsetup.sh.
#
# In particular, you can add lunch options with the add_lunch_combo
# function: add_lunch_combo generic-nand-eng
#
#   Note:
#   eng         --> compile in engineering mode
#   user        --> compile in user mode
#   userdebug   --> compile in userdebug mode, with all modules tag with
#                   eng,user,userdebug. gdb can be used to debug.
#
#  each word split by '_',
#  TARGET_DEVICE.TARGET_DEVICE_SUBVERSION split by '.',
#	1. char '.' is not allowed in TARGET_DEVICE_SUBVERSION
#	2. TARGET_DEVICE_SUBVERSION can be empty
#
# [TARGET_DEVICE.TARGET_DEVICE_SUBVERSION]_[TARGET_STORAGE_MEDIUM]_[TARGET_EXT_SUPPORT]-eng
# [TARGET_DEVICE.TARGET_DEVICE_SUBVERSION]_[TARGET_STORAGE_MEDIUM]_[TARGET_EXT_SUPPORT]-user
# [TARGET_DEVICE.TARGET_DEVICE_SUBVERSION]_[TARGET_STORAGE_MEDIUM]_[TARGET_EXT_SUPPORT]-userdebug
#

# use nand as main fs storage. replace 'template' to the boardname.
add_lunch_combo template_nand-eng
add_lunch_combo template_nand-user
add_lunch_combo template_nand-userdebug

# use nor as main fs storage
add_lunch_combo template_nor-eng
add_lunch_combo template_nor-user
add_lunch_combo template_nor-userdebug

# use msc as main fs storage
add_lunch_combo template_msc-eng
add_lunch_combo template_msc-user
add_lunch_combo template_msc-userdebug

# add board with subversion v12
#add_lunch_combo template.v12_nand-eng
#add_lunch_combo template.v12_nand-user
#add_lunch_combo template.v12_nand-userdebug

