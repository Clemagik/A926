


ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),nand)
# **** uboot ****
LOCAL_PATH :=$(my-dir)
include $(CLEAR_VARS)

ifneq ($(strip $(TARGET_EXT2_SUPPORT)),ota)
UBOOT_BUILD_CONFIG:=halley5_uImage_sfc_nand
else
UBOOT_BUILD_CONFIG:=halley5_xImage_sfc_nand
endif # end ota

UBOOT_TARGET_FILE:=u-boot-with-spl.bin
LOCAL_MODULE := uboot
UBOOT_PATH := $(TOP_DIR)/u-boot
LOCAL_MODULE_TAGS := optional
$(info $(BUILD_UBOOT) 1234 ---- )
include $(BUILD_UBOOT)

# copy above to add a new uboot config.
#
# **** end uboot ****
endif # end nand
