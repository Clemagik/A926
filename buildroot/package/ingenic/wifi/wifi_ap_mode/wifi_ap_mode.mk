################################################################################
#
# wifi_ap_mode
#
################################################################################
WIFI_AP_MODE_VERSION = 1.0

WIFI_AP_MODE_SOURCE = wifi_ap_mode-$(WIFI_AP_MODE_VERSION)
WIFI_AP_MODE_SITE_METHOD = local
WIFI_AP_MODE_SITE = ingenic/wifi/wifi_ap_mode

WIFI_AP_MODE_INSTALL_STAGING = YES
WIFI_AP_MODE_INSTALL_TARGET = YES
WIFI_AP_MODE_DEPENDENCIES = host-pkgconf

WIFI_AP_MODE_PRIVATE_CONFIGS = HOSTAP_INTERFACE=${BR2_PACKAGE_WIFI_HOSTAP_INTERFACE}

define WIFI_AP_MODE_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) DESTDIR=$(TARGET_DIR) $(WIFI_AP_MODE_PRIVATE_CONFIGS) install \

endef

$(eval $(generic-package))
