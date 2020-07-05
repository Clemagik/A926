################################################################################
#
# wifi_rtk
#
################################################################################
WIFI_RTK_VERSION = 1.0

WIFI_RTK_SOURCE = wifi_rtk-$(WIFI_RTK_VERSION)
WIFI_RTK_SITE_METHOD = local
WIFI_RTK_SITE = ingenic/wifi/wifi_rtk

WIFI_RTK_INSTALL_STAGING = YES
WIFI_RTK_INSTALL_TARGET = YES
WIFI_RTK_DEPENDENCIES = host-pkgconf wpa_supplicant

define WIFI_RTK_INSTALL_TARGET_CMDS
        $(INSTALL) -m 0755 -D ingenic/wifi/wifi_rtk/wpa_bin/download_wifi_firmware.sh $(TARGET_DIR)/bin/
        $(INSTALL) -m 0755 -D ingenic/wifi/wifi_rtk/wpa_bin/S41network_firmware $(TARGET_DIR)/etc/init.d/
        $(INSTALL) -m 0755 -D ingenic/wifi/wifi_rtk/wpa_bin/wifi_up.sh $(TARGET_DIR)/bin/
        $(INSTALL) -m 0755 -D ingenic/wifi/wifi_rtk/wpa_bin/wifi_down.sh $(TARGET_DIR)/bin/
        $(INSTALL) -m 0755 -D ingenic/wifi/wifi_rtk/wpa_bin/rtwpriv $(TARGET_DIR)/usr/bin/
        $(INSTALL) -m 0755 -D ingenic/wifi/wifi_rtk/wpa_bin/update_wifi_efuse.sh $(TARGET_DIR)/usr/bin/
endef

$(eval $(generic-package))
