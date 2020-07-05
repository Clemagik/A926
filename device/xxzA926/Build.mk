include $(DEVICE_PATH)/kernel.mk
include $(DEVICE_PATH)/uboot.mk

ifeq ($(strip $(TARGET_EXT2_SUPPORT)),ota)
include $(DEVICE_PATH)/kernel-recovery.mk
endif
