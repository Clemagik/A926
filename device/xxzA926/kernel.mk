KERNEL_VER=$(strip $(TARGET_EXT_SUPPORT))
include $(CLEAR_VARS)
LOCAL_MODULE=kernel
KERNEL_PATH=$(TOP_DIR)/kernel-$(KERNEL_VER)
#KERNEL_CONFIG_PATH:=$(DEVICE_PATH)/kernel_cfg/$(KERNEL_VER)
KERNEL_CONFIG_PATH:=$(KERNEL_PATH)/arch/mips/configs
LOCAL_MODULE_TAGS :=optional

ifneq ($(strip $(TARGET_EXT2_SUPPORT)),ota)
KERNEL_TARGET_IMAGE:=uImage
KERNEL_IMAGE_PATH:=arch/mips/boot/
else
KERNEL_TARGET_IMAGE:=xImage
KERNEL_IMAGE_PATH:=arch/mips/boot/compressed/
endif # end ota

ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),nand)
KERNEL_BUILD_CONFIG:=xxzA926_linux_sfc_nand_defconfig
endif # end nand

include $(BUILD_KERNEL)
