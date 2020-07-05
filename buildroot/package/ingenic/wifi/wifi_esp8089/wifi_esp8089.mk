################################################################################
#
# wifi_esp8089
#
################################################################################
WIFI_ESP8089_VERSION = 1.0

WIFI_ESP8089_SOURCE = wifi_rtk-$(WIFI_ESP8089_VERSION)
WIFI_ESP8089_SITE_METHOD = local
WIFI_ESP8089_SITE = ingenic/wifi/wifi_esp8089

WIFI_ESP8089_INSTALL_STAGING = YES
WIFI_ESP8089_INSTALL_TARGET = YES
WIFI_ESP8089_DEPENDENCIES = host-pkgconf wpa_supplicant

define WIFI_ESP8089_INSTALL_TARGET_CMDS
        $(INSTALL) -m 0755 -D ingenic/wifi/wifi_esp8089/wpa_bin/download_wifi_firmware.sh $(TARGET_DIR)/bin/
        $(INSTALL) -m 0755 -D ingenic/wifi/wifi_esp8089/wpa_bin/S41network_firmware $(TARGET_DIR)/etc/init.d/
        $(INSTALL) -m 0755 -D ingenic/wifi/wifi_esp8089/wpa_bin/wifi_up.sh $(TARGET_DIR)/bin/
        $(INSTALL) -m 0755 -D ingenic/wifi/wifi_esp8089/wpa_bin/wifi_down.sh $(TARGET_DIR)/bin/
endef

$(eval $(generic-package))
