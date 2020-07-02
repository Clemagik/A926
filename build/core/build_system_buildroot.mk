include $(BUILD_SYSTEM)/build_buildroot.mk

FILE_SYSTEM_TYPE:=$(strip $(FILE_SYSTEM_TYPE))

BUILD_SYSTEMIMAGE_TARGET:=$(OUT_IMAGE_DIR)/system.$(FILE_SYSTEM_TYPE)

define MODULES_DEP_ROOTFS
$(1):rootfs
endef
$(foreach m,$(ALL_MODULES), $(eval $(call MODULES_DEP_ROOTFS,$(m))))

systemimage:rootfs $(ALL_MODULES) post-image

rootfs: buildroot

post-image: buildroot-post-image $(BUILD_SYSTEMIMAGE_TARGET)

$(BUILD_SYSTEMIMAGE_TARGET):$(BUILD_BUILDROOT_INTERMEDIATE_OBJ)
	cp -f $^ $@

.PHONY:rootfs systemimage post-image

#################################################################################
OTA_INSTALL_STEMP=$(OUT_DEVICE_OBJ_DIR)/ota/.stamp_install
KERNEL_BIN=$(OUT_IMAGE_DIR)/kernel
KERNEL_RECOVERY_BIN=$(OUT_IMAGE_DIR)/kernel_recovery
SYSTEM_BIN=$(OUT_IMAGE_DIR)/system.ubifs

OTGPACKGE_TEMP_DIR=$(OUT_DEVICE_OBJ_DIR)/ota/allpackage
OTGPACKGE_OUT_DIR=$(OUT_IMAGE_DIR)/ota

PACKAGE_CONFIG=packages/updater/ota_package_maker/example/config
PACKAGE_PYTOOL=otapackage

OUT_IMAGE_ZERO=$(OUT_IMAGE_DIR)/ota_zero
PRODUCT_DEVICE_PATH:=$(DEVICE_PATH)
KEYNAME=$(PRODUCT_DEVICE_PATH)/security/releasekey

OTA_FUNC_DIR_STEMP=$(OUT_DEVICE_OBJ_DIR)/ota/ota_package_maker/.stamp_install
$(OTA_FUNC_DIR_STEMP):
	mkdir -p $(dir $@)
	cp -rfT packages/updater/ota_package_maker $(dir $@)
	@install -D /dev/null $@

$(OTA_INSTALL_STEMP):$(KERNEL_BIN) $(KERNEL_RECOVERY_BIN) $(SYSTEM_BIN) $(OTA_FUNC_DIR_STEMP)
	mkdir -p $(OUT_IMAGE_DIR)
	dd if=/dev/zero of=$(OUT_IMAGE_DIR)/zero bs=1k count=512
	@mkdir -p $(OTGPACKGE_TEMP_DIR)/nand
	@mkdir -p $(OTGPACKGE_OUT_DIR)
	cp $(KERNEL_BIN) $(OTGPACKGE_TEMP_DIR)/nand && \
	cp $(KERNEL_RECOVERY_BIN) $(OTGPACKGE_TEMP_DIR)/nand && \
	cp $(SYSTEM_BIN) $(OTGPACKGE_TEMP_DIR)/nand && \
	cd $(dir $(OTA_FUNC_DIR_STEMP)); $(PYTHON_TOOL) -m $(PACKAGE_PYTOOL) --output=$(TOP_DIR)/$(OTGPACKGE_OUT_DIR) --imgpath=$(TOP_DIR)/$(OTGPACKGE_TEMP_DIR) --configpath=$(TOP_DIR)/$(PACKAGE_CONFIG) --publickey=$(KEYNAME).x509.pem --privatekey=$(KEYNAME).pk8;cd - && \
	cd $(OTGPACKGE_OUT_DIR);7zr a ota-package -r global.xml  nand sha1Tab VERSION;cd -
	@install -D /dev/null $(OTA_INSTALL_STEMP)
	@echo "ota package create to " $(OUT_IMAGE_DIR)/ota


.PHONY:ota_mkpackage

ota_mkpackage:$(OTA_INSTALL_STEMP)
