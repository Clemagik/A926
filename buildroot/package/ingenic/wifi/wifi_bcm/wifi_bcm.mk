################################################################################
#
# wifi_bcm
#
################################################################################
WIFI_BCM_VERSION = 1.0

WIFI_BCM_SOURCE = wifi_bcm-$(WIFI_BCM_VERSION)
WIFI_BCM_SITE_METHOD = local
WIFI_BCM_SITE = ingenic/wifi/wifi_bcm

WIFI_BCM_INSTALL_STAGING = YES
WIFI_BCM_INSTALL_TARGET = YES
WIFI_BCM_DEPENDENCIES = host-pkgconf

WIFI_BIN-$(BR2_PACKAGE_WPA_SUPPLICANT) := wpa_bin

ifeq ($(BR2_PACKAGE_WIFI_BCM_43438), y)
FIRMWARE_PATH-$(BR2_PACKAGE_WIFI_BCM_43438) := wifi_bcm_43438
endif

ifeq ($(BR2_PACKAGE_WIFI_BCM_43362), y)
FIRMWARE_PATH-$(BR2_PACKAGE_WIFI_BCM_43362) := wifi_bcm_43362
endif

ifeq ($(BR2_PACKAGE_WIFI_BCM_AP6256), y)
FIRMWARE_PATH-$(BR2_PACKAGE_WIFI_BCM_AP6256) := wifi_bcm_ap6256
endif

WIFIMAC_ADDR := $(BR2_PACKAGE_WIFIMAC_ADDR)
WIFIMAC_ADDR_PATH := $(BR2_PACKAGE_WIFIMAC_ADDR_PATH)

define WIFI_BCM_INSTALL_TARGET_CMDS
	$(TARGET_MAKE_ENV) $(MAKE) -C $(@D) DESTDIR=$(TARGET_DIR) install \
		WIFI_BIN=$(WIFI_BIN-y) \
		FIRMWARE_PATH=$(FIRMWARE_PATH-y) \
                WIFIMAC_ADDR=$(WIFIMAC_ADDR) \
                WIFIMAC_ADDR_PATH=$(WIFIMAC_ADDR_PATH) \

endef

$(eval $(generic-package))
