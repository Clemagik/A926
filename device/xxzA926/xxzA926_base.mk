#Tools and libraries necessary by program

#kernel & uboot
#PRODUCT_MODULES := $(KERNEL_TARGET_IMAGE) \
	$(UBOOT_TARGET_FILE)
PRODUCT_MODULES := 	uboot \
			kernel

IOTDEV := \
	iotdev-exe \
	iotdev-lib \
	iotdev-etc \
	iotdev-include \
	iotdev-firmware \

ADB := \
	adb_device_model1 \
	adbd \

#APP := \
	jzrtspserver

PRODUCT_MODULES += \
	$(IOTDEV) \
	$(ADB) \
#	$(APP)
