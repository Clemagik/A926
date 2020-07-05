
DESTDIR ?= /

define cp_bcm_43438
	mkdir -p ${DESTDIR}/lib/firmware/
	rm -f ${DESTDIR}/lib/firmware/BCM43438_A1
	rm -f ${DESTDIR}/lib/firmware/BCM43438_A1.cal
	cp -vrf --parents ${FIRMWARE_PATH}/nv*     ${DESTDIR}/lib/firmware/
	cp -vrf --parents ${FIRMWARE_PATH}/fw*     ${DESTDIR}/lib/firmware/
	ln -s wifi_bcm_43438/fw_43438_a1 ${DESTDIR}/lib/firmware/BCM43438_A1
	ln -s wifi_bcm_43438/nv_43438_a1 ${DESTDIR}/lib/firmware/BCM43438_A1.cal
endef

define cp_bcm_other
	mkdir -p ${DESTDIR}/firmware/
	cp -vrf ${FIRMWARE_PATH}/nv*     ${DESTDIR}/firmware/
	cp -vrf ${FIRMWARE_PATH}/fw*     ${DESTDIR}/firmware/
endef

ifeq ($(FIRMWARE_PATH),wifi_bcm_43438)
	cp_firmware_cmd = $(cp_bcm_43438)
else
	cp_firmware_cmd = $(cp_bcm_other)
endif

.PHONY: install
install:
	cp -vrf wpa_bin/wifi_up.sh $(DESTDIR)/bin/
	cp -vrf wpa_bin/wifi_down.sh $(DESTDIR)/bin/
	cp -vrf wpa_bin/download_wifi_firmware.sh $(DESTDIR)/bin/
	cp -vrf wpa_bin/S41network_firmware $(DESTDIR)/etc/init.d/
	$(cp_firmware_cmd)
