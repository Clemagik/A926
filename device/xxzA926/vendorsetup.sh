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
# [TARGET_DEVICE.TARGET_DEVICE_SUBVERSION]_[TARGET_STORAGE_MEDIUM]_[TARGET_EXT_SUPPORT]_[TARGET_EXT2_SUPPORT]-eng
# [TARGET_DEVICE.TARGET_DEVICE_SUBVERSION]_[TARGET_STORAGE_MEDIUM]_[TARGET_EXT_SUPPORT]_[TARGET_EXT2_SUPPORT]-user
# [TARGET_DEVICE.TARGET_DEVICE_SUBVERSION]_[TARGET_STORAGE_MEDIUM]_[TARGET_EXT_SUPPORT]_[TARGET_EXT2_SUPPORT]-userdebug
#


# use nand as main fs storage.
add_lunch_combo xxzA926_nand_4.4.93-eng
add_lunch_combo xxzA926_nand_4.4.93-user
add_lunch_combo xxzA926_nand_4.4.93-userdebug

