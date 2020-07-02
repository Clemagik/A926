#
# this used for 'make rootfsimage' command, Realize the function of packaging images directly
#
check_ota_dir:
	@if [ -e $(OUT_IMAGE_DIR) ];then echo;else \
		mkdir -p $(OUT_IMAGE_DIR);\
		fi
	@if [ ! -e $(TARGET_FS_BUILD) ]; then \
		echo "******************************************************************"; \
		echo " ERROR : can no find  $(TARGET_FS_BUILD) dir";\
		echo "  Can not get  TARGET_DEVICE info ,you should ' source build/envsetup.sh ' firstly ,then ' lunch ' choice the config,finally ' make ' to build the project "; \
		echo "******************************************************************"; \
		elif [ ! -e $(TARGET_INSTALL_PATH)/systemOTA ];then \
		echo "******************************************************************"; \
		echo " ERROR : can no find  $(TARGET_INSTALL_PATH)/systemOTA dir";\
		echo "  Can not get  TARGET_DEVICE info ,you should ' source build/envsetup.sh ' firstly ,then ' lunch ' choice the config,finally ' make ' to build the project "; \
		echo "******************************************************************"; \
		else echo ;\
		fi
rm_ota_image:
	@if [ -f $(OUT_IMAGE_DIR)/update_fs.cramfs ];then rm $(OUT_IMAGE_DIR)/update_fs.cramfs;fi
	@if [ -f $(OUT_IMAGE_DIR)/update_fs.cramfs ];then rm $(OUT_IMAGE_DIR)/user_fs.cramfs;fi

