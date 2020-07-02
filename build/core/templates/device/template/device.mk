PRODUCT:=product
TARGET_PRODUCT_BOARD:=$(TARGET_DEVICE)
TARGET_BOARD_PLATFORM:="template_soctype"
TARGET_BOARD_ARCH:="mips"

# For xburst2
#TARGET_ARCH_VARIANT:=mips32r2_fp64
# For xburst
TARGET_ARCH_VARIANT:=mips32r2

LOCAL_PATH := $(shell dirname $(lastword $(MAKEFILE_LIST)))
DEVICE_PATH := $(TOP_DIR)/$(LOCAL_PATH)

MAKE_JLEVEL ?= 4


#rules to make uboot&kernel&rootfs according to storage medium
#
# Supported FILE_SYSTEM_TYPE:
# ['ubifs', 'jiffs2', 'ext2', "yaffs2", "cramfs"]
#
ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),msc)
#rootfs
BUILDROOT_CONFIG:=template_buildroot_defconfig
#BUILDROOT_CONFIG_PATH:=$(DEVICE_PATH)
BUILDROOT_PATH:=buildroot
ROOTFS_OVERLAY_DIR:=$(DEVICE_PATH)/rootfs-overlay
FILE_SYSTEM_TYPE:=ext2
endif #end msc

ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),nor)
BUILDROOT_CONFIG:=template_buildroot_defconfig
#BUILDROOT_CONFIG_PATH:=$(DEVICE_PATH)
BUILDROOT_PATH:=buildroot
ROOTFS_OVERLAY_DIR:=$(DEVICE_PATH)/rootfs-overlay
FILE_SYSTEM_TYPE:=jffs2

endif #end nor


ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),nand)
BUILDROOT_CONFIG:=template_buildroot_defconfig
#BUILDROOT_CONFIG_PATH:=$(DEVICE_PATH)
BUILDROOT_PATH:=buildroot
ROOTFS_OVERLAY_DIR:=$(DEVICE_PATH)/rootfs-overlay
FILE_SYSTEM_TYPE:=ubifs

endif #end nand
