# Makefile define rulles which make kernel.
#	Vars we need from local module:
#
#    BUSYBOX_BUILD_CONFIG: 	# mandatory. which %_defconfig files to configure busybox
#    BUSYBOX_INSTALL_DIR:  	# optional.  where busybox should be installed.
#    BUSYBOX_PATH:	     	# optional.  where the busybox source is.
#    BUSYBOX_CONFIG_PATH:  	# optional.  where the busybox config is, if empty, config file must in source code.
#
#
#	TODO: add package BUSYBOX_INSTALL_DIR to target fs image.
#	      add BUSYBOX_ROOTFS_OVERLAY if necessary.
#



ifeq ($(BUSYBOX_BUILD_CONFIG),)
$(error **** No BUSYBOX_BUILD_CONFIG defined ****)
endif


# Default values
BUSYBOX_PATH?=$(TOP_DIR)/busybox
BUSYBOX_INSTALL_DIR?=$(TOP_DIR)/$(TARGET_INSTALL_PATH)/system-busybox
BUILD_BUSYBOX_TARGET:=$(BUSYBOX_INSTALL_DIR)/bin/busybox

BUILD_BUSYBOX_INTERMEDIATE:=$(TOP_DIR)/$(OUT_DEVICE_OBJ_DIR)/$(LOCAL_MODULE)-intermediate


$(info LOCAL_MODULE $(LOCAL_MODULE))
$(info BUILD_BUSYBOX_INTERMEDIATE $(BUILD_BUSYBOX_INTERMEDIATE))
$(info BUSYBOX_INSTALL_DIR $(BUSYBOX_INSTALL_DIR))


# if KERNEL_CONFIG_PATH has value, rename defconfig. else use config under kernel src.
ifneq ($(BUSYBOX_CONFIG_PATH),)
BUSYBOX_DEFCONFIG:=ingenic_build_$(TARGET_BOARD_PLATFORM)_$(TARGET_PRODUCT_BOARD)_$(LOCAL_MODULE)_defconfig
$(shell cp $(BUSYBOX_CONFIG_PATH)/$(BUSYBOX_BUILD_CONFIG) $(BUSYBOX_PATH)/configs/$(BUSYBOX_DEFCONFIG))
else
BUSYBOX_DEFCONFIG:=$(BUSYBOX_BUILD_CONFIG)
endif

$(BUILD_BUSYBOX_TARGET):$(BUILD_BUSYBOX_INTERMEDIATE)
	cd $(BUSYBOX_PATH)	\
	&& make O=$(BUILD_BUSYBOX_INTERMEDIATE) CONFIG_PREFIX=$(BUSYBOX_INSTALL_DIR) install \
	&& cd -


.PHONY: $(BUILD_BUSYBOX_INTERMEDIATE)
$(BUILD_BUSYBOX_INTERMEDIATE):
	mkdir -p $@
	cd $(BUSYBOX_PATH) && make O=$(BUILD_BUSYBOX_INTERMEDIATE) $(BUSYBOX_DEFCONFIG) \
	&& make O=$(BUILD_BUSYBOX_INTERMEDIATE) -j$(MAKE_JLEVEL)	\
	&& cd -

LOCAL_MODULE_BUILD=$(LOCAL_MODULE)
include $(BUILD_SYSTEM)/module_install.mk
ifneq ($(LOCAL_FILTER_MODULE),)
ALL_MODULES += $(LOCAL_FILTER_MODULE)
ALL_BUILD_MODULES += $(BUILD_BUSYBOX_TARGET)
endif

$(LOCAL_MODULE):$(BUILD_BUSYBOX_TARGET)

$(LOCAL_MODULE)-clean:
	rm -rf $(BUILD_KERNEL_INTERMEDIATE)


