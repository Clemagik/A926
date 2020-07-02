#if the model has system,use it;or use the common fs.
#test function: exit ----> 0
#				no	 ----> 1
TARGET_FS:= $(TARGET_COMMON_FS_PATH)
include $(BUILD_SYSTEM)/build_fs.mk


define patch_target_fs
$(if $(shell if [ -f $(TARGET_MODEL_PATH)/system_rm_redundancyOTA.sh ];then echo True;fi),$(shell $(TARGET_MODEL_PATH)/system_rm_redundancyOTA.sh))
$(if $(shell if [ -d $(TARGET_MODEL_PATH)/systemOTA.patch ];then echo True;fi),$(shell cp -rf $(TARGET_MODEL_PATH)/systemOTA.patch/* $(TARGET_INSTALL_PATH)/systemOTA))
$(if $(shell if [ -f $(TARGET_MODEL_PATH)/system_rm_redundancy.sh ];then echo True;fi),$(shell $(TARGET_MODEL_PATH)/system_rm_redundancy.sh))
$(if $(shell if [ -d $(TARGET_MODEL_PATH)/system.patch ];then echo True;fi),$(shell cp -rf $(TARGET_MODEL_PATH)/system.patch/* $(TARGET_FS_BUILD)))
endef


rootfs:$(TARGET_INSTALL_PATH)/systemOTA $(TARGET_FS_BUILD)
	$(patch_target_fs)
$(TARGET_INSTALL_PATH)/systemOTA:
	mkdir -p $@
	fakeroot tar -zxvf $(TARGET_MODEL_PATH)/rootfsOTA.tar.gz  -C $@/
$(TARGET_FS_BUILD):
	mkdir -p $@
	if [ -f $(TARGET_MODEL_PATH)/rootfs.tar.gz ];then \
		fakeroot tar -zxvf $(TARGET_MODEL_PATH)/rootfs.tar.gz  -C $@/ ;  \
	else \
		fakeroot tar -zxvf $(TARGET_COMMON_FS_PATH)/rootfs.tar.gz -C $@/ ; \
	fi


ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),norflash)
include $(BUILD_SYSTEM)/build_ota_system_image.mk
systemimage:$(ALL_MODULES) $(OUT_IMAGE_DIR)/update_fs.cramfs $(OUT_IMAGE_DIR)/user_fs.cramfs ota_post
rootfsimage:check_ota_dir rm_ota_image $(OUT_IMAGE_DIR)/update_fs.cramfs $(OUT_IMAGE_DIR)/user_fs.cramfs

$(OUT_IMAGE_DIR)/update_fs.$(FILE_SYSTEM_TYPE):
	$(if $(shell if [ -f $(TARGET_MODEL_PATH)/system_patchOTA.sh ];then echo True;fi),$(shell $(TARGET_MODEL_PATH)/system_patchOTA.sh))
ifeq ($(strip $(FILE_SYSTEM_TYPE)),cramfs)
	mkfs.cramfs $(TARGET_INSTALL_PATH)/systemOTA $@
endif

$(OUT_IMAGE_DIR)/user_fs.$(FILE_SYSTEM_TYPE):
	$(if $(shell if [ -f $(TARGET_MODEL_PATH)/system_patch.sh ];then echo True;fi),$(shell $(TARGET_MODEL_PATH)/system_patch.sh))
ifeq ($(strip $(FILE_SYSTEM_TYPE)),cramfs)
	mkfs.cramfs $(TARGET_FS_BUILD) $@
endif

ota_post:
	cp $(OUT_IMAGE_DIR)/$(KERNEL_TARGET_IMAGE) $(DEVELOPMENT_TOOLS_PATH)/ota/src_image/$(KERNEL_TARGET_IMAGE);	\
	cp $(OUT_IMAGE_DIR)/update_fs.$(FILE_SYSTEM_TYPE) $(DEVELOPMENT_TOOLS_PATH)/ota/src_image/update_fs.$(FILE_SYSTEM_TYPE);	\
	cp $(OUT_IMAGE_DIR)/user_fs.$(FILE_SYSTEM_TYPE) $(DEVELOPMENT_TOOLS_PATH)/ota/src_image/user_fs.$(FILE_SYSTEM_TYPE);
endif

ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),spinand)
include $(BUILD_SYSTEM)/build_ota_system_image.mk
systemimage:$(ALL_MODULES) $(OUT_IMAGE_DIR)/system.$(FILE_SYSTEM_TYPE) $(OUT_IMAGE_DIR)/rootfs.cpio.gz ota_post
rootfsimage:check_ota_dir rm_ota_image $(OUT_IMAGE_DIR)/system.$(FILE_SYSTEM_TYPE) $(OUT_IMAGE_DIR)/rootfs.cpio.gz


$(OUT_IMAGE_DIR)/system.$(FILE_SYSTEM_TYPE):
	$(build_filesystem_to_ubi)
	cp $(OUT_IMAGE_DIR)/system.$(FILE_SYSTEM_TYPE) .
	ubinize -o $(OUT_IMAGE_DIR)/updaterfs.$(FILE_SYSTEM_TYPE) -p $(ROOTFS_UBIFS_EBSIZE) -m $(ROOTFS_UBIFS_MINIOSIZE) -s $(ROOTFS_UBIFS_PAGESIZE) $(TARGET_MODEL_PATH)/ubinize.cfg
	rm system.$(FILE_SYSTEM_TYPE)

$(OUT_IMAGE_DIR)/rootfs.cpio.gz:
	cd $(TOP_DIR)/$(TARGET_INSTALL_PATH)/systemOTA/;\
	fakeroot rm dev/console;\
	fakeroot find . | cpio --quiet -o -H newc > ../rootfs.cpio.img;\
	mv ../rootfs.cpio.img ../image/rootfs.cpio.img;
	mv $(OUT_IMAGE_DIR)/rootfs.cpio.img $(OUT_IMAGE_DIR)/rootfs.cpio
	gzip -9 -n $(OUT_IMAGE_DIR)/rootfs.cpio

ota_post:
	cp $(OUT_IMAGE_DIR)/$(KERNEL_TARGET_IMAGE) $(DEVELOPMENT_TOOLS_PATH)/ota-nand/src_image/$(KERNEL_TARGET_IMAGE);	\
	cp $(OUT_IMAGE_DIR)/rootfs.cpio.gz $(DEVELOPMENT_TOOLS_PATH)/ota-nand/src_image/rootfs.cpio.gz;	\
	cp $(OUT_IMAGE_DIR)/updaterfs.$(FILE_SYSTEM_TYPE) $(DEVELOPMENT_TOOLS_PATH)/ota-nand/src_image/system.$(FILE_SYSTEM_TYPE);	\
	cd $(DEVELOPMENT_TOOLS_PATH)/ota-nand;	\
	make ota;

endif
