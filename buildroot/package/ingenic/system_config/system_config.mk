################################################################################
#
# system_config
#
################################################################################

# The system_config can't depend on the toolchain, since all packages depends on the
# system_config and the toolchain is a target package, as is system_config.
# Hence, system_config would depends on the toolchain and the toolchain would depend
# on system_config.
SYSTEM_CONFIG_ADD_TOOLCHAIN_DEPENDENCY = NO

# The system_config also handles the merged /usr case in the sysroot

ifeq ($(BR2_PACKAGE_SYSTEM_CONFIG),y)

SYSTEM_CONFIG_INSTALL_STAGING = YES

define SYSTEM_CONFIG_INSTALL_TARGET_CMDS
	$(SED) '/::sysinit:\/bin\/mkdir -p \/dev\/pts/i::sysinit:\/bin\/rm -rf \/dev/*' \
		$(TARGET_DIR)/etc/inittab
endef

define SYSTEM_CONFIG_INSTALL_STAGING_CMDS

endef

SYSTEM_CONFIG_LOGIN_TTY_PORT = $(call qstrip,$(BR2_SYSTEM_CONFIG_LOGIN_TTY_PORT))

################ ENABLE_ROOT_LOGIN #######################
ifeq ($(BR2_SYSTEM_CONFIG_ENABLE_ROOT_LOGIN),y)

ifeq ($(BR2_INIT_SYSV),y)

# In sysvinit inittab, the "id" must not be longer than 4 bytes, so we
# skip the "tty" part and keep only the remaining.
define SYSTEM_CONFIG_SET_LOGIN
	$(SED) '/# GENERIC_SERIAL$$/s~^.*#~$(shell echo $(SYSTEM_CONFIG_LOGIN_TTY_PORT) | tail -c+4)::respawn:/bin/sh #~' \
		$(TARGET_DIR)/etc/inittab
endef

else ifeq ($(BR2_INIT_BUSYBOX),y)

# Add getty to busybox inittab
define SYSTEM_CONFIG_SET_LOGIN
	$(SED) '/# GENERIC_SERIAL$$/s~^.*#~$(SYSTEM_CONFIG_LOGIN_TTY_PORT)::respawn:-/bin/sh #~' \
		$(TARGET_DIR)/etc/inittab
endef



endif
TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_SET_LOGIN

else

define SYSTEM_CONFIG_CLEAR_LOGIN
	$(SED) '/# GENERIC_SERIAL$$/s~^.*#~#~' \
		$(TARGET_DIR)/etc/inittab
endef

TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_CLEAR_LOGIN

endif # BR2_INIT_BUSYBOX || BR2_INIT_SYSV
############# ENABLE_ROOT_LOGIN end ######################

############## SET_DNS_SERVER #########################
ifeq ($(BR2_SYSTEM_CONFIG_SET_DNS_SERVER),y)

define SYSTEM_CONFIG_SET_DNS
	rm -f $(TARGET_DIR)/etc/resolv.conf
	cp -vrf package/ingenic/system_config/resolv.conf $(TARGET_DIR)/etc/
endef

TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_SET_DNS

endif		#BR2_SYSTEM_CONFIG_SET_DNS_SERVER
############ SET_DNS_SERVER end ###################


############## MOUNT_USR_DATA_UBI #########################
ifeq ($(BR2_SYSTEM_CONFIG_MOUNT_USR_DATA_UBI),y)

define SYSTEM_CONFIG_MOUNT_USR_DATA_UBI
	cp -vrf package/ingenic/system_config/S21mount $(TARGET_DIR)/etc/init.d/
endef

TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_MOUNT_USR_DATA_UBI

else

define SYSTEM_CONFIG_MOUNT_USR_DATA_UBI
	rm -f $(TARGET_DIR)/etc/init.d/S21mount
endef

TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_MOUNT_USR_DATA_UBI

endif

############ SYSTEM_CONFIG_MOUNT_USR_DATA_UBI end ###################

############## enable adb service #########################
ifeq ($(BR2_SYSTEM_CONFIG_ENABLE_ADB),y)

define SYSTEM_CONFIG_ENABLE_ADB
	cp -vrf package/ingenic/system_config/S90adb $(TARGET_DIR)/etc/init.d/
	cp -vrf package/ingenic/system_config/adb $(TARGET_DIR)/etc/init.d/
endef

TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_ENABLE_ADB

ifeq ($(BR2_SYSTEM_CONFIG_START_ADB_SERVER),y)
define SYSTEM_CONFIG_ENABLE_ADB_SERVER
	cp -vrf package/ingenic/system_config/adbserver.sh $(TARGET_DIR)/sbin/
	cp -vrf package/ingenic/system_config/adb_Hotplug.sh $(TARGET_DIR)/sbin/
endef
TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_ENABLE_ADB_SERVER
else
define SYSTEM_CONFIG_ENABLE_ADB_SERVER
	rm -f $(TARGET_DIR)/sbin/adbserver.sh
	rm -f $(TARGET_DIR)/sbin/adb_Hotplug.sh
endef
TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_ENABLE_ADB_SERVER
endif

adb_export_env := package/ingenic/system_config/export_env.sh
adb_env_file := $(TARGET_DIR)/etc/profile.d/env_setup.sh

define ADB_SETUP_ENV
	$(adb_export_env) env_adb_device_name_prefix "$(BR2_SYSTEM_CONFIG_ADB_DEVICES_NAME_PREFIX)" $(adb_env_file)
	$(adb_export_env) env_adb_device_use_diffrent_name "$(BR2_SYSTEM_CONFIG_ADB_DEVICE_USE_DIFFERENT_NAME)" $(adb_env_file)
endef

TARGET_FINALIZE_HOOKS += ADB_SETUP_ENV
else

define SYSTEM_CONFIG_ENABLE_ADB
	rm -f $(TARGET_DIR)/etc/init.d/S90adb
	rm -f $(TARGET_DIR)/sbin/adbserver.sh
	rm -rf $(TARGET_DIR)/etc/init.d/adb
	rm -f $(TARGET_DIR)/sbin/adb_Hotplug.sh
endef

TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_ENABLE_ADB

endif

ifeq ($(BR2_SYSTEM_CONFIG_CACHEDNS),y)
define SYSTEM_CONFIG_ENABLE_CACHEDNS
	cp -avT package/ingenic/system_config/cachedns_files $(TARGET_DIR)
endef
TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_ENABLE_CACHEDNS
endif 

############ enable adb service end ###################

############## UTILS_MOUNT_UBIFS #########################
ifeq ($(BR2_SYSTEM_CONFIG_UTILS_MOUNT_UBIFS),y)

define SYSTEM_CONFIG_UTILS_MOUNT_UBIFS
	cp -vrf package/ingenic/system_config/mount_ubifs.sh $(TARGET_DIR)/usr/bin/
	cp -vrf package/ingenic/system_config/reboot_mon.sh $(TARGET_DIR)/usr/bin/
endef

TARGET_FINALIZE_HOOKS += SYSTEM_CONFIG_UTILS_MOUNT_UBIFS

endif
############## UTILS_MOUNT_UBIFS end #########################

############## qt4 touchscreen device #########################

qt4_tp_export_env := package/ingenic/system_config/export_env.sh
qt4_tp_env_file := $(TARGET_DIR)/etc/profile.d/env_setup.sh

define QT4_TP_DEV_SETUP_ENV
	$(qt4_tp_export_env) QWS_MOUSE_PROTO "$(BR2_SYSTEM_CONFIG_QT4_TP_DEV)" $(qt4_tp_env_file)
endef

TARGET_FINALIZE_HOOKS += QT4_TP_DEV_SETUP_ENV

############## qt4 touchscreen device end #########################

############## qt4 keyboard device #########################

qt4_keyboard_export_env := package/ingenic/system_config/export_env.sh
qt4_keyboard_env_file := $(TARGET_DIR)/etc/profile.d/env_setup.sh

define QT4_KEYBOARD_DEV_SETUP_ENV
	$(qt4_keyboard_export_env) QWS_KEYBOARD "$(BR2_SYSTEM_CONFIG_QT4_KEYBOARD_DEV)" $(qt4_keyboard_env_file)
endef

TARGET_FINALIZE_HOOKS += QT4_KEYBOARD_DEV_SETUP_ENV

############## qt4 keyboard device end #########################

############## qt4 qws size #########################

qt4_qws_size_export_env := package/ingenic/system_config/export_env.sh
qt4_qws_size_env_file := $(TARGET_DIR)/etc/profile.d/env_setup.sh

define QT4_QWS_SIZE_SETUP_ENV
	$(qt4_qws_size_export_env) QWS_SIZE "$(BR2_SYSTEM_CONFIG_QT4_QWS_SIZE)" $(qt4_qws_size_env_file)
endef

TARGET_FINALIZE_HOOKS += QT4_QWS_SIZE_SETUP_ENV

############## qt4 qws size end #########################

############## qt4 qws display #########################

qt4_qws_display_export_env := package/ingenic/system_config/export_env.sh
qt4_qws_display_env_file := $(TARGET_DIR)/etc/profile.d/env_setup.sh

define QT4_QWS_DISPLAY_SETUP_ENV
	$(qt4_qws_display_export_env) QWS_DISPLAY "$(BR2_SYSTEM_CONFIG_QT4_QWS_DISPLAY)" $(qt4_qws_display_env_file)
endef

TARGET_FINALIZE_HOOKS += QT4_QWS_DISPLAY_SETUP_ENV

############## qt4 qws display end #########################
endif

$(eval $(generic-package))
