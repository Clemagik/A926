include $(BUILD_SYSTEM)/build_fs.mk

#if the model has system,use it;or use the common fs.
#test function: exist ----> 0
#               no   ----> 1
#TARGET_FS:= $(if $(shell test -d $(TARGET_FS_SRC_PATH)),$(TARGET_FS_SRC_PATH),$(TARGET_COMMON_FS_PATH))
TARGET_FS:= $(TARGET_COMMON_FS_PATH)

define patch_target_fs
$(if $(shell if [ -f $(TARGET_MODEL_PATH)/system_rm_redundancy.sh ];then echo True;fi),$(shell $(TARGET_MODEL_PATH)/system_rm_redundancy.sh))
$(if $(shell if [ -d $(TARGET_MODEL_PATH)/system.patch ];then echo True;fi),$(shell cp -rf $(TARGET_MODEL_PATH)/system.patch/* $(TARGET_FS_BUILD)))
endef

rootfs:$(TARGET_FS_BUILD)
	$(patch_target_fs)
$(TARGET_FS_BUILD):
	mkdir -p $@
	if [ -f $(TARGET_MODEL_PATH)/rootfs.tar.gz ];then \
		fakeroot tar -zxvf $(TARGET_MODEL_PATH)/rootfs.tar.gz  -C $@/ ;  \
	else \
		fakeroot tar -zxvf $(TARGET_COMMON_FS_PATH)/rootfs.tar.gz -C $@/ ; \
	fi


include $(BUILD_SYSTEM)/build_system_image.mk
systemimage:$(ALL_MODULES) $(OUT_IMAGE_DIR)/system.$(FILE_SYSTEM_TYPE) moduledesc
rootfsimage:check_dir rm_image $(OUT_IMAGE_DIR)/system.$(FILE_SYSTEM_TYPE)

$(OUT_IMAGE_DIR)/system.$(FILE_SYSTEM_TYPE):
	$(if $(shell if [ -f $(TOP_DIR)/device/$(TARGET_DEVICE)/$(TARGET_DEVICE).sh ];then echo True;fi),$(shell echo $(TOP_DIR)/device/$(TARGET_DEVICE)/$(TARGET_DEVICE).sh))
ifeq ($(strip $(FILE_SYSTEM_TYPE)),jffs2)
	$(build_filesystem_to_jffs2)
endif
ifeq ($(strip $(FILE_SYSTEM_TYPE)),ubi)
	$(build_filesystem_to_ubi)
endif
ifeq ($(strip $(FILE_SYSTEM_TYPE)),ext4)
	$(build_filesystem_to_ext4)
endif
ifeq ($(strip $(FILE_SYSTEM_TYPE)),cramfs)
	$(build_filesystem_to_cramfs)
endif

MD5=md5sum
md5file=`which $(MD5)`

$(OUT_IMAGE_DIR)/originfs.ext2:systemimage
	$(build_filesystem_to_ext4)

$(OUT_IMAGE_DIR)/readonlyfs.$(FILE_SYSTEM_TYPE):$(OUT_IMAGE_DIR)/originfs.ext2
	$(eval TARGET_FS_BUILD:=$(TARGET_INSTALL_PATH)/readonlyfs)
	@tar xjf $(TOP_DIR)/device/$(TARGET_DEVICE)/readonlyfs.tar.bz2 -C $(TARGET_INSTALL_PATH)
	@cp -f $(OUT_IMAGE_DIR)/originfs.ext2 $(TARGET_FS_BUILD)
	@md5text=`$(MD5) $(TARGET_FS_BUILD)/originfs.ext2`
	@echo $(md5text%' '*) > $(TARGET_FS_BUILD)/originfs.ext2.md5
	@cp -f $(TARGET_MODEL_PATH)/system.readonlyfs/etc/init.d/* $(TARGET_FS_BUILD)/etc/init.d/
	@cp -f $(TARGET_MODEL_PATH)/system.readonlyfs/usr/bin/* $(TARGET_FS_BUILD)/usr/bin/
ifeq ($(strip $(FILE_SYSTEM_TYPE)),jffs2)
	$(build_filesystem_to_jffs2)
endif
ifeq ($(strip $(FILE_SYSTEM_TYPE)),ubi)
	$(build_filesystem_to_ubi)
endif
ifeq ($(strip $(FILE_SYSTEM_TYPE)),ext4)
	$(warning "Does not support ext4 yet!")
endif
ifeq ($(strip $(FILE_SYSTEM_TYPE)),cramfs)
$(warning "Does not support cramfs yet!")
endif

readonlyfs:$(OUT_IMAGE_DIR)/readonlyfs.$(FILE_SYSTEM_TYPE)
