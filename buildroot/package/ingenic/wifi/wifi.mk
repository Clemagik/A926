include $(sort $(wildcard package/ingenic/wifi/*/*.mk))

wifi_export_env := package/ingenic/system_config/export_env.sh
wifi_env_file := $(TARGET_DIR)/etc/profile.d/env_setup.sh

ifeq ($(BR2_PACKAGE_WIFI),y)

BR2_PACKAGE_WIFI_HOSTAP_INTERFACE ?= "wlan1"
define WIFI_SETUP_ENV
	$(wifi_export_env) env_wifi_mac_addr_path "$(BR2_PACKAGE_WIFIMAC_ADDR_PATH)" $(wifi_env_file)
	$(wifi_export_env) env_wifi_enable_when_system_up "$(BR2_PACKAGE_WIFI_STARTUP)" $(wifi_env_file)
	$(wifi_export_env) env_wifi_wpa_supplicant_conf "$(BR2_PACKAGE_WIFI_WPA_SUPPLICANT_CONF)" $(wifi_env_file)
	$(wifi_export_env) env_wifi_hostap_conf "$(BR2_PACKAGE_WIFI_HOSTAP_CONF)" $(wifi_env_file)
	$(wifi_export_env) env_wifi_hostap_interface "$(BR2_PACKAGE_WIFI_HOSTAP_INTERFACE)" $(wifi_env_file)
endef

TARGET_FINALIZE_HOOKS += WIFI_SETUP_ENV

endif
