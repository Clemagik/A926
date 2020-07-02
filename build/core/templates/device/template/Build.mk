ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),msc)

# **** uboot ****
LOCAL_PATH :=$(my-dir)
include $(CLEAR_VARS)
UBOOT_BUILD_CONFIG:=uboot_template_xImage_msc0
UBOOT_TARGET_FILE:=u-boot-with-spl.bin
LOCAL_MODULE := uboot
UBOOT_PATH := $(TOP_DIR)/u-boot
LOCAL_MODULE_TAGS := optional
include $(BUILD_UBOOT)

# copy above to add a new uboot config.

# **** end uboot ****

# *** kernel ****
include $(CLEAR_VARS)
LOCAL_MODULE=kernel
KERNEL_PATH=$(TOP_DIR)/kernel
KERNEL_BUILD_CONFIG:=template_kernel_defconfig
KERNEL_CONFIG_PATH:=$(DEVICE_PATH)
KERNEL_IMAGE_PATH:=arch/mips/boot/compressed
KERNEL_TARGET_IMAGE:=xImage
LOCAL_MODULE_TAGS :=optional
include $(BUILD_KERNEL)
# copy above to add a new kernel config.

# *** end kernel ****
endif # end msc

ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),nor)

# **** uboot ****
LOCAL_PATH :=$(my-dir)
include $(CLEAR_VARS)
UBOOT_BUILD_CONFIG:=uboot_template_xImage_nor
UBOOT_TARGET_FILE:=u-boot-with-spl.bin
LOCAL_MODULE := uboot
UBOOT_PATH := $(TOP_DIR)/u-boot
LOCAL_MODULE_TAGS := optional
$(info $(BUILD_UBOOT) 1234 ---- )
include $(BUILD_UBOOT)

# copy above to add a new uboot config.

# **** end uboot ****

# *** kernel ****
include $(CLEAR_VARS)
LOCAL_MODULE=kernel
KERNEL_PATH=$(TOP_DIR)/kernel
KERNEL_BUILD_CONFIG:=template_kernel_defconfig
KERNEL_CONFIG_PATH:=$(DEVICE_PATH)
KERNEL_IMAGE_PATH:=arch/mips/boot/compressed
KERNEL_TARGET_IMAGE:=xImage
LOCAL_MODULE_TAGS :=optional
include $(BUILD_KERNEL)
# copy above to add a new kernel config.

# *** end kernel ****
endif # end nor

ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),nand)

# **** uboot ****
LOCAL_PATH :=$(my-dir)
include $(CLEAR_VARS)
UBOOT_BUILD_CONFIG:=uboot_template_xImage_nand
UBOOT_TARGET_FILE:=u-boot-with-spl.bin
LOCAL_MODULE := uboot
UBOOT_PATH := $(TOP_DIR)/u-boot
LOCAL_MODULE_TAGS := optional
include $(BUILD_UBOOT)

# copy above to add a new uboot config.

# **** end uboot ****

# *** kernel ****
include $(CLEAR_VARS)
LOCAL_MODULE=kernel
KERNEL_PATH=$(TOP_DIR)/kernel
KERNEL_BUILD_CONFIG:=template_kernel_defconfig
KERNEL_CONFIG_PATH:=$(DEVICE_PATH)
KERNEL_IMAGE_PATH:=arch/mips/boot/compressed
KERNEL_TARGET_IMAGE:=xImage
LOCAL_MODULE_TAGS :=optional
include $(BUILD_KERNEL)
# copy above to add a new kernel config.

# *** end kernel ****
endif # end nand

