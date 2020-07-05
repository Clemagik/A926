# 蓝牙-WIFI模块

WIFI、蓝牙使用的是AP6214A芯片，WIFI使用SDIO接口进行通信，而蓝牙使用uart接口进行通信。

# 编译配置\(buildroot\)

```
    一般发布的软件版本中都会默认配置WIFI和Bluetooth测试应用， 如需要自己更改Bluetooth和WiFi的编译选项，可以通过以下方式进行配置。buildroot中的开发板配置文件在configs下。
```

## 1. 配置过程

```
根据开发板选用相应的配置文件，halley3选用halley3_v10_defconfig配置。

$ make halley3_v10_defconfig

$ make menuconfig
```

## 2. 配置选项

### 2.1 蓝牙（Bluetooth）配置

```
--- ingenic packages 
 [*]   bluetooth
         --- bluetooth
            select bluebooth chip  --->
             [*] BCM43438
                   select BCM43438 firmware (BCM43438_A1)  --->
                   select bluetooth protocol (bluez)  --->
             [ ] RTK_8723
         (/dev/ttyS1) bluetooth device
         [ ]   pulseaudio config for bluetooth
         [ ]   play to bluetooth
         (halley3) prefix of bluetooth device name
 [*]   bluetooth manager
 [ ]   bluetooth media control demo
 [*]   bluez auto agent for demo
 -*-   bluez alsa
```

### 2.2 WIFI配置

```
[*]   wifi
        --- wifi
        [ ]   common wifi device
                  --select wifi chip
                          [*] WIFI_BCM
                                      WIFI BCM Default Select (WIFI_BCM_43438)  --->
                          [ ] WIFI_RTK
        -*-   wifi use wpa_supplicant
        (/etc/wpa_supplicant.conf) wpa_supplicant.conf file path
        (/usr/data/hostapd.conf) hostapd.conf file path
        (wlan0) hostap interface
        [*]     wifi auto startup
        [*]     wifi use ap mode
        (C8:93:46:45:C3:80) wifi mac address
        (/data/misc/wifi/) wifi mac address path
```

## 3.Target packages配置

```
Location: 
   -> Target packages 
     -> Networking applications
-*- bluez-utils 5.x  
[ ] build OBEX support 
-*- build CLI client 
-*- install deprecated tool
-*- dnsmasq
[*] dhcp support
-*- hostapd
-*-   Enable WPS
[*] ntp
[*]   ntpdate
[*] tcpdump 
[*]   smb dump support
-*- wpa_supplicant
[*]   Enable nl80211 support
-*-   Install wpa_cli binary
-*-   Install wpa_client shared library
-*-   Install wpa_passphrase binary
```

## 命令使用

### 蓝牙

#### &gt;蓝牙匹配流程

##### 1&gt;使用hciconfig -a查看蓝牙信息

```
# hciconfig -a
hci0:   Type: Primary  Bus: UART
        BD Address: D0:31:10:AF:EB:02  ACL MTU: 1021:8  SCO MTU: 64:1
        UP RUNNING PSCAN ISCAN 
        RX bytes:1231 acl:0 sco:0 events:63 errors:0
        TX bytes:1360 acl:0 sco:0 commands:63 errors:0
        Features: 0xbf 0xfe 0xcf 0xfe 0xdb 0xff 0x7b 0x87
        Packet type: DM1 DM3 DM5 DH1 DH3 DH5 HV1 HV2 HV3 
        Link policy: RSWITCH SNIFF 
        Link mode: SLAVE ACCEPT 
        Name: 'halley3_EB02'
        Class: 0x040414
        Service Classes: Rendering
        Device Class: Audio/Video, Loudspeaker
        HCI Version: 4.1 (0x7)  Revision: 0xe7
        LMP Version: 4.1 (0x7)  Subversion: 0x2209
        Manufacturer: Broadcom Corporation (15)
```

##### 2&gt;连接设备开启蓝牙，搜索 halley3\_EB02，点击该设备进行配对

```
AutoAgent: [NEW] Device 9C:A5:C0:20:26:A7 vivo X6S A
GattServer: Device connected
AutoAgent: Accept pairing (yes/no): yes
udhcpc: sending discover
[BT] property changed: Modalias
[BT] property changed: UUIDs
[BT] property changed: ServicesResolved
[BT] property changed: Paired
[BT] INFO: device connected
====================>call dev_connect_callback 1 
call dev_connect_callback fail ,m_isAvailable is false
AutoAgent: [CHG] Device 9C:A5:C0:20:26:A7 vivo X6S A Modalias: bluetooth:v001Dp1200d1436
AutoAgent: [CHG] Device 9C:A5:C0:20:26:A7 vivo X6S A UUIDs: 00001103-0000-1000-8000-00805f9b34fb
AutoAgent: [CHG] Device 9C:A5:C0:20:26:A7 vivo X6S A ServicesResolved: yes
AutoAgent: [CHG] Device 9C:A5:C0:20:26:A7 vivo X6S A Paired: yes
AutoAgent: Authorize service 0000110d-0000-1000-8000-00805f9b34fb (yes/no): yes
[BT] property changed: UUIDs
AutoAgent: [CHG] Device 9C:A5:C0:20:26:A7 vivo X6S A UUIDs: 00001103-0000-1000-8000-00805f9b34fb
AutoAgent: [CHG] Device 9C:A5:C0:20:26:A7 vivo X6S A UUIDs: 00001105-0000-1000-8000-00805f9b34fb
AutoAgent: [CHG] Device 9C:A5:C0:20:26:A7 vivo X6S A UUIDs: 00001106-0000-1000-8000-00805f9b34fb
[BT] connected bluetooth MediaControl1 /org/bluez/hci0/dev_9C_A5_C0_20_26_A7/fd0
bluealsa: bluez.c:767: Endpoint method call: org.bluez.MediaEndpoint1.SetConfiguration()
bluealsa: ctl.c:667: Sending notification: 1 => 14
bluealsa-aplay: aplay.c:735: Fetching available transports
bluealsa: bluez.c:696: A2DP Sink (SBC) configured for device 9C:A5:C0:20:26:A7
bluealsa: bluez.c:698: Configuration: channels: 2, sampling: 44100
bluealsa: transport.c:746: State transition: 0 -> 0
bluealsa-aplay: aplay.c:800: Creating PCM worker 9C:A5:C0:20:26:A7
bluealsa: bluez.c:867: Registering endpoint: /A2DP/SBC/Sink/2
bluetoothd[181]: Endpoint registered: sender=:1.2 path=/A2DP/SBC/Sink/2
bluealsa-aplay: ../src/shared/ctl-client.c:105: Connecting to socket: /var/run/bluealsa/hci0
bluealsa: ctl.c:636: Received new connection: 16
bluealsa: ctl.c:649: New client accepted: 16
bluealsa-aplay: ../src/shared/ctl-client.c:375: Requesting PCM open for 9C:A5:C0:20:26:A7
bluealsa: ctl.c:343: PCM requested for 9C:A5:C0:20:26:A7 type 1 stream 1
bluealsa-aplay: aplay.c:385: Starting PCM loop
udhcpc: sending discover
bluetoothd[181]: Can't open input device: No such file or directory (2)
bluetoothd[181]: AVRCP: failed to init uinput for 9C:A5:C0:20:26:A7
[BT] connected bluetooth player /org/bluez/hci0/dev_9C_A5_C0_20_26_A7/player0
bluealsa: bluez.c:1314: Signal: PropertiesChanged: org.bluez.MediaPlayer1: Repeat
bluealsa: bluez.c:1314: Signal: PropertiesChanged: org.bluez.MediaPlayer1: Shuffle
bluealsa: bluez.c:1314: Signal: PropertiesChanged: org.bluez.MediaPlayer1: Status
bluealsa: bluez.c:1314: Signal: PropertiesChanged: org.bluez.MediaPlayer1: Track
bluealsa: bluez.c:1314: Signal: PropertiesChanged: org.bluez.MediaPlayer1: Position
```

#### &gt;常用命令

* bt\_up.sh启动蓝牙（默认启动执行）
* bt\_down.sh关闭蓝牙
* hciconfig -a查看蓝牙信息
* bluetoothctl命令

##### 1&gt;执行bluetoothctl命令

```
# bluetoothctl 
[NEW] Controller D0:31:10:AF:EB:02 halley3_EB02 [default]
[NEW] Device 9C:A5:C0:20:26:A7 vivo X6S A
Agent registered
[vivo X6S A]#
```

##### 2&gt;输入help显示以下属性\(列举部分\)

```
[vivo X6S A]# help
Available commands:
  list                       List available controllers
  show [ctrl]                Controller information
  select <ctrl>              Select default controller
  devices                    List available devices
  paired-devices             List paired devices
  system-alias <name>        Set controller alias
  reset-alias                Reset controller alias

  scan <on/off>              Scan for devices
  info [dev]                 Device information
  pair [dev]                 Pair with device
  trust [dev]                Trust device
  untrust [dev]              Untrust device
  block [dev]                Block device
  unblock [dev]              Unblock device
  remove <dev>               Remove device
  connect <dev>              Connect device
  disconnect [dev]           Disconnect device
  list-attributes [dev]      List attributes
  set-alias <alias>          Set device alias
  select-attribute <attribute/UUID>
                             Select attribute
  attribute-info [attribute/UUID]
                             Select attribute
  read                       Read attribute value
  write <data=[xx xx ...]>   Write attribute value
  acquire-write              Acquire Write file descriptor
  release-write              Release Write file descriptor
  acquire-notify             Acquire Notify file descriptor
  release-notify             Release Notify file descriptor
  notify <on/off>            Notify attribute value

  version                    Display version
  quit                       Quit program
  exit                       Quit program
  help                       Display help about this program
```

### WiFi

#### &gt;WiFi匹配流程

##### 1&gt;启动后，配置/etc/wpa\_supplicant.conf中WiFi名字（ssid）和密码（psk）

```
# cat etc/wpa_supplicant.conf 
ctrl_interface=/var/run/wpa_supplicant
update_config=1
country=GB"

network={
    ssid="test"
    scan_ssid=1
    psk="123456ab"
    bssid=
    priority=1
}
```

##### 2&gt;配置完成后执行wifi\_up.sh

```
# wifi_up.sh 
[ 3109.423516] BCM:
[ 3109.423516] Dongle Host Driver, version 1.141.66
[ 3109.423516] Compiled in drivers/net/wireless/bcmdhd on Aug 15 2019 at 10:18:32
[ 3109.437878] BCM:wl_android_wifi_on in
[ 3109.441679] BCM:wifi_platform_set_power = 1
[ 3109.446035] BCM:=========== WLAN placed in RESET ON  ========
[ 3109.468504] sdio_reset_comm():
[ 3109.510720] mmc1: queuing unknown CIS tuple 0x80 (2 bytes)
[ 3109.519490] mmc1: queuing unknown CIS tuple 0x80 (3 bytes)
[ 3109.528253] mmc1: queuing unknown CIS tuple 0x80 (3 bytes)
[ 3109.539463] mmc1: queuing unknown CIS tuple 0x80 (7 bytes)
[ 3109.551901] mmc1: queuing unknown CIS tuple 0x81 (9 bytes)
[ 3109.726455] BCM:F1 signature OK, socitype:0x1 chip:0xa9a6 rev:0x1 pkg:0x4
[ 3109.734014] BCM:DHD: dongle ram size is set to 524288(orig 524288) at 0x0
[ 3109.741272] concate_revision: chip:a9a6
[ 3109.745264] fw_path:/lib/firmware/wifi_bcm_43438/fw_43438_a1 nv_path:/lib/firmware/wifi_bcm_43438/nv_43438_a1
[ 3109.798742] BCM:dhdsdio_write_vars: Download, Upload and compare of NVRAM succeeded.
[ 3109.860463] BCM:dhd_bus_init: enable 0x06, ready 0x06 (waited 0us)
[ 3109.867795] BCM:wifi_platform_get_mac_addr
[ 3109.873532] BCM:dhd_get_concurrent_capabilites: Get P2P failed (error=-23)
[ 3109.880663] BCM:Firmware up: op_mode=0x0001, MAC=28:ed:e0:94:8b:b1
[ 3109.891291] BCM:dhd_preinit_ioctls pspretend_threshold for HostAPD failed  -23
[ 3109.903149] BCM:Firmware version = wl0: Jun 27 2017 11:07:16 version 7.46.57.4.ig.r5d4 (A1 Station/P2P) FWID 01-bd86c2ef es7.c5.n4.a3.ap1
[ 3109.916651] BCM:dhd_wlfc_hostreorder_init(): successful bdcv2 tlv signaling, 64
# Successfully initialized wpa_supplicant
udhcpc: started, v1.25.1
udhcpc: sending discover
nl80211: Could not re-add multicast membership for vendor events: -2 (No such file or directory)
udhcpc: sending discover
[ 3129.713940] BCM:6CFG80211-ERROR) wl_cfg80211_connect : BCM:Connectting withff:ff:ff:ff:ff:ff channel (0) ssid "test", len (4)
[ 3130.829290] BCM:wl_bss_connect_done succeeded with 9c:a5:c0:20:26:a8
[ 3130.840666] BCM:wl_bss_connect_done succeeded with 9c:a5:c0:20:26:a8
udhcpc: sending discover
udhcpc: sending select for 192.168.43.150
udhcpc: lease of 192.168.43.150 obtained, lease time 3600
deleting routers
adding dns 192.168.43.1
```

##### 3&gt;使用ping www.baidu.com验证网络连接成功

```
# ping www.baidu.com
PING www.baidu.com (61.135.169.121): 56 data bytes
64 bytes from 61.135.169.121: seq=0 ttl=55 time=33.770 ms
64 bytes from 61.135.169.121: seq=1 ttl=55 time=50.481 ms
64 bytes from 61.135.169.121: seq=2 ttl=55 time=50.138 ms
64 bytes from 61.135.169.121: seq=3 ttl=55 time=50.936 ms
64 bytes from 61.135.169.121: seq=4 ttl=55 time=53.542 ms
```

##### 4&gt;使用wifi\_down.sh关闭WiFi



