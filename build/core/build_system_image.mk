#
# this used for 'make rootfsimage' command, Realize the function of packaging images directly
#
check_dir:
	@if [ -e $(OUT_IMAGE_DIR) ];then echo;else \
		mkdir -p $(OUT_IMAGE_DIR);\
	fi
	@if [ -e $(TARGET_FS_BUILD) ];then echo;else \
		echo "***********************i*******************************************"; \
		echo " ERROR : can no find  $(TARGET_FS_BUILD) dir";\
		echo "  Can not get  TARGET_DEVICE info ,you should ' source build/envsetup.sh ' firstly ,then ' lunch ' choice the config,finally ' make ' to build the project "; \
		echo "***********************i*******************************************"; \
		exit;\
	fi

rm_image:
	@if [ -f $(OUT_IMAGE_DIR)/system.$(FILE_SYSTEM_TYPE) ];then rm $(OUT_IMAGE_DIR)/system.$(FILE_SYSTEM_TYPE);fi
