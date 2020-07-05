#!/bin/sh

count=0

while [ ! -e "/sys/class/net/wlan0/address" ]; do
    if [ "$count" != "10" ]; then
        let count=count+1;
        echo "waiting for /sys/class/net/wlan0/address ....."
        sleep 1;
    else
        echo "failed to wait /sys/class/net/wlan0/address ....."
        echo "do not set mac address, do not wifi_up"
        exit 1;
    fi
done

echo "env_wifi_wpa_supplicant_conf ${env_wifi_wpa_supplicant_conf}"
echo "env_wifi_mac_addr_path ${env_wifi_mac_addr_path}"
echo "env_wifi_enable_when_system_up ${env_wifi_enable_when_system_up}"

if [ ! -e ${env_wifi_wpa_supplicant_conf} ]; then
    mkdir -p ${env_wifi_wpa_supplicant_conf%/*}
    echo "ctrl_interface=/var/run/wpa_supplicant" >  ${env_wifi_wpa_supplicant_conf}
    echo "update_config=1"                        >> ${env_wifi_wpa_supplicant_conf}
    echo "country=GB"                             >> ${env_wifi_wpa_supplicant_conf}
fi

echo "env_wifi_hostap_conf ${env_wifi_hostap_conf}"

if [ ! -e ${env_wifi_hostap_conf} ]; then
    mkdir -p ${env_wifi_hostap_conf%/*}
    cp /etc/hostapd.conf ${env_wifi_hostap_conf} -v
fi

WIFI_MAC_ADDR=$(macgen)
echo -n "$WIFI_MAC_ADDR">/tmp/wifimac.txt

ifconfig wlan0 hw ether $WIFI_MAC_ADDR
ifconfig -a | grep wlan1 > /dev/null
if [ $? -eq 0 ]; then
  ifconfig wlan1 hw ether $(echo -n D2 && echo -n ${WIFI_MAC_ADDR:2})
fi

update_wifi_efuse.sh

if [ "${env_wifi_enable_when_system_up}" == "y" ];then
    wifi_up.sh
fi
