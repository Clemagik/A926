
DESTDIR ?= /

.PHONY: install
install:
	cp -vrf etc/* $(DESTDIR)/etc/
	sed -i 's/your_wifi_interface/$(HOSTAP_INTERFACE)/' $(DESTDIR)/etc/hostapd.conf
	sed -i 's/your_wifi_interface/$(HOSTAP_INTERFACE)/' $(DESTDIR)/etc/udhcpd.conf
	cp -vrf wifi_ap_mode_stop.sh $(DESTDIR)/usr/bin/
	cp -vrf wifi_ap_mode_start.sh $(DESTDIR)/usr/bin/
