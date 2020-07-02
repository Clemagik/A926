TOP := .
TOPDIR := .
ALL_MODULES:=
ALL_MODULES_CLEAN:=
MMA_MODULES :=
ALL_INSTALL_MODULES:=
ALL_PREBUILT_MODULES:= prebuild
THIRDPART_CLEAN_MODULES:=
tags_to_install :=
BUILD_SYSTEM := $(TOPDIR)/build/core

#all support tag is : eng user
ALL_SUPPORT_MODULE_TAGS := eng user optional userdebug

ifeq ($(TARGET_BUILD_VARIANT),eng)
tags_to_install := eng user
endif

ifeq ($(TARGET_BUILD_VARIANT),userdebug)
tags_to_install := eng user userdebug
endif

ifeq ($(TARGET_BUILD_VARIANT),user)
tags_to_install := user
endif

include $(BUILD_SYSTEM)/device-tools-chain.mk
include $(BUILD_SYSTEM)/host-tools-chain.mk
include $(BUILD_SYSTEM)/config.mk
include $(BUILD_SYSTEM)/device_ruler.mk
include $(BUILD_SYSTEM)/host_ruler.mk
include $(BUILD_SYSTEM)/definitions-host.mk

BUILD_TYPE := c cpp cc S

EXCEPT-FILES:=build include Makefile out

include $(SDK_BUILD_MODULE)/Build.mk

.PHONY:all
all: $(ALL_MODULES)


