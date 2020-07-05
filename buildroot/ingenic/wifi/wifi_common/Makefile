DESTDIR ?= /

.PHONY: install
install:
	@if [ ${RTL8723DS}x = "y"x ]; then echo ">>> rtl8723ds"; fi
	@if [ ${BCM43362_A2}x = "y"x ]; then \
		echo ">>> bcm43362 a2"; \
		mkdir -p ${DESTDIR}/lib/firmware/; \
		cp -vf ${SRCDIR}/../wifi_bcm/wifi_bcm_43362/fw_bcmdhd-43362a2-apsta.bin ${DESTDIR}/lib/firmware/; \
		cp -vf ${SRCDIR}/../wifi_bcm/wifi_bcm_43362/nvram_ap6181.txt ${DESTDIR}/lib/firmware/; \
	fi
	@if [ ${BCM43438_A0}x = "y"x ]; then \
		echo ">>> bcm43438 a0"; \
		mkdir -p ${DESTDIR}/lib/firmware/; \
		cp -vf ${SRCDIR}/../wifi_bcm/wifi_bcm_43438/fw_43438_a0 ${DESTDIR}/lib/firmware/; \
		cp -vf ${SRCDIR}/../wifi_bcm/wifi_bcm_43438/nv_43438_a0 ${DESTDIR}/lib/firmware/; \
		ln -sf ./fw_43438_a0 ${DESTDIR}/lib/firmware/BCM43438_A0; \
		ln -sf ./nv_43438_a0 ${DESTDIR}/lib/firmware/BCM43438_A0.cal; \
	fi
	@if [ ${BCM43438_A1}x = "y"x ]; then \
		echo ">>> bcm43438 a1"; \
		mkdir -p ${DESTDIR}/lib/firmware/; \
		cp -vf ${SRCDIR}/../wifi_bcm/wifi_bcm_43438/fw_43438_a1 ${DESTDIR}/lib/firmware/; \
		cp -vf ${SRCDIR}/../wifi_bcm/wifi_bcm_43438/nv_43438_a1 ${DESTDIR}/lib/firmware/; \
		ln -sf ./fw_43438_a1 ${DESTDIR}/lib/firmware/BCM43438_A1; \
		ln -sf ./nv_43438_a1 ${DESTDIR}/lib/firmware/BCM43438_A1.cal; \
	fi
	@if [ $(BCM4345)x = "y"x ]; then \
		echo ">>> bcm4345";	\
		mkdir -p ${DESTDIR}/lib/firmware/; \
		cp -vf ${SRCDIR}/../wifi_bcm/wifi_bcm_4345/fw_bcm43455c0_ag.bin ${DESTDIR}/lib/firmware/; \
		cp -vf ${SRCDIR}/../wifi_bcm/wifi_bcm_4345/nvram_ap6255.txt ${DESTDIR}/lib/firmware/; \
		ln -sf ./fw_bcm43455c0_ag.bin ${DESTDIR}/lib/firmware/BCM43455;	\
		ln -sf ./nvram_ap6255.txt ${DESTDIR}/lib/firmware/BCM43455.cal;	\
	fi
	./script/create_exec_shell.sh ${DESTDIR} ${WIFIMAC_ADDR_PATH}
