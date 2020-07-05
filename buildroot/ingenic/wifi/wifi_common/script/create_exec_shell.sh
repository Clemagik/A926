#!/bin/bash

DESTDIR=$1
WIFIMAC_ADDR_PATH=$2
MAC_FILE=${WIFIMAC_ADDR_PATH}/wifimac.txt

echo "#!/bin/sh" > S41network_firmware.sh
echo "" >> S41network_firmware.sh
echo "find /proc/net/ -name 'rtl*' | grep rtl > /dev/null" >> S41network_firmware.sh
echo "" >> S41network_firmware.sh
echo "if [ \$? -ne 0 ]; then" >> S41network_firmware.sh
echo "    MACADDR=\$(cat /sys/class/net/wlan0/address)" >> S41network_firmware.sh
echo "    grep \$MACADDR /lib/firmware/*.* > /dev/null" >> S41network_firmware.sh
echo "    if [ \$? -ne 0 ]; then" >> S41network_firmware.sh
echo "        echo \">>>>>> Broadcom wifi has macaddr\"" >> S41network_firmware.sh
echo "        exit" >> S41network_firmware.sh
echo "    else" >> S41network_firmware.sh
echo "        echo \">>>>>> Broadcom wifi has no macaddr\"" >> S41network_firmware.sh
echo "    fi" >> S41network_firmware.sh
echo "else" >> S41network_firmware.sh
echo "    echo \">>>>>> reatlek wifi\"" >> S41network_firmware.sh
echo "fi" >> S41network_firmware.sh
echo "" >> S41network_firmware.sh
echo "WIFI_ADDR=\$(macgen)" >> S41network_firmware.sh
echo "" >> S41network_firmware.sh
echo "if [ ! -e $MAC_FILE ]; then" >> S41network_firmware.sh
echo "    mkdir -p ${WIFIMAC_ADDR_PATH}" >> S41network_firmware.sh
echo "    echo \${WIFI_ADDR} > $MAC_FILE" >> S41network_firmware.sh
echo "fi" >> S41network_firmware.sh
echo "" >> S41network_firmware.sh
echo "ifconfig wlan0 hw ether \${WIFI_ADDR}" >> S41network_firmware.sh
echo "ifconfig -a | grep wlan1 > /dev/null" >> S41network_firmware.sh
echo "if [ \$? -eq 0 ]; then" >> S41network_firmware.sh
echo "  ifconfig wlan1 hw ether \$(echo -n D2 && echo -n \${WIFI_ADDR:2})" >> S41network_firmware.sh
echo "fi" >> S41network_firmware.sh

mkdir -p ${DESTDIR}/${WIFIMAC_ADDR_PATH}
mkdir -p ${DESTDIR}/etc/firmware/wifi/
chmod 777 S41network_firmware.sh
cp -vrf S41network_firmware.sh             ${DESTDIR}/etc/init.d/
