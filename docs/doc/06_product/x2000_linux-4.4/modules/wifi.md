# wifi模块

## 1.wifi模块简介
X2000使用的ap6256芯片， Ap6256是一个wifi和蓝牙功能模块，它包括用于WiFi的SDIO接口，和用于蓝牙的UART/PCM接口。

* GPIO功能描述

|Name|I/O|Description|
|-|-|-|
|WL_REG_ON|I|Power up/down internal regulators used by WiFi section|
|WL_HOST_WAKE|O|WLAN to wake-up HOST|
|GND|-|Ground connections|
|SDIO_DATA_1|I/O|SDIO data line 1|
|SDIO_DATA_2|I/O|SDIO data line 2|
|SDIO_DATA_3|I/O|SDIO data line 3|
|SDIO_DATA_4|I/O|SDIO data line 4|
|SDIO_DATA_CMD|I/O|SDIO command line|
|SDIO_DATA_CLK|I/O|SDIO clock line|

* GPIO接口

```c
SDIO GPIO-PD08～GPIO-PD13 FUNCTION0
WL_REG_ON GPIO-PD19     
WL_WAKE_HOST GPIO-PD20
```

## 2.内核空间

## 2.1.驱动相关的源码

```c
drivers/mmc/host/
├── sdhci.c
├── sdhci-ingenic.c
├── ingenic_sdio.c
```

### 2.2.设备树配置

```c
&msc1 {
        status = "okay";                  /*配置“okay”编译SDIO驱动，“disable”反之*/
        pinctrl-names ="default","enable", "disable";
        pinctrl-0 = <&msc1_4bit>;
        pinctrl-1 = <&rtc32k_enable>;
        pinctrl-2 = <&rtc32k_disable>;

        max-frequency = <50000000>;
        bus-width = <4>;
        voltage-ranges = <1800 3300>;
        /*non-removable;*/

        ingenic,sdio_clk = <1>;
        keep-power-in-suspend;

        /* special property */
        ingenic,wp-gpios = <0>;
        ingneic,cd-gpios = <0>;
        ingenic,rst-gpios = <0>;
        ingenic,removal-manual; /*removal-dontcare, removal-nonremovable, removal-removable, removal-manual*/

        bcmdhd_wlan: bcmdhd_wlan {
                 compatible = "android,bcmdhd_wlan";
                 ingenic,sdio-irq = <&gpd 20 IRQ_TYPE_LEVEL_HIGH INGENIC_GPIO_NOBIAS>;
                 ingenic,sdio-reset = <&gpd 19 GPIO_ACTIVE_LOW INGENIC_GPIO_NOBIAS>;
        };
};
```

### 2.3.驱动配置

```c
Device Drivers
--- Network device support 
[*]   Network core driver support
[*]   Wireless LAN  --->
        <*>   Broadcom FullMAC wireless cards support
        (/firmware/fw_bcm43456c5_ag.bin) Firmware path  /*固件路径*/
        (/firmware/nvram_ap6256.txt) NVRAM path         /*固件路径*/
                Enable Chip Interface (SDIO bus interface support)  --->
                Interrupt type (Out-of-Band Interrupt)  --->
```

## 3.用户空间

## 3.1.驱动加载成功
```c
[    1.124644] Dongle Host Driver, version 1.363.59.144.11 (r)
[    1.124644] Compiled from 
[    1.125120] Register interface [wlan0]  MAC: 00:90:4c:11:22:33
[    1.125120] 
[    1.125195] dhd_module_init: Exit err=0
```
## 3.2.测试方法

### 3.2.1.配置/etc/wpa\_supplicant.conf

```c
# cat /etc/wpa_supplicant.conf 
ctrl_interface=/var/run/wpa_supplicant
update_config=1
country=GB"

network={
    ssid="Guest"        /*连接WiFi账户*/
    psk="ingenic*123"   /*连接Wifi密码*/
    bssid=
    priority=1
}
```

### 3.2.2.执行wifi\_up.sh

```c
# wifi_up.sh 
[   13.285811] dhd_open: Enter 843c4800
[   13.290329] dhd_conf_read_config: Ignore config file /firmware/config.txt
[   13.297356] Final fw_path=/firmware/fw_bcm43456c5_ag.bin
[   13.302873] Final nv_path=/firmware/nvram_ap6256.txt
[   13.308002] Final clm_path=/firmware/clm_bcmdhd.blob
[   13.313152] Final conf_path=/firmware/config.txt
[   13.317926] dhd_set_bus_params: set use_rxchain 0
[   13.322804] dhd_set_bus_params: set txglomsize 36
[   13.328047] dhd_os_open_image: /firmware/fw_bcm43456c5_ag.bin (579388 bytes) open success
[   13.409630] dhd_os_open_image: /firmware/nvram_ap6256.txt (2440 bytes) open success
[   13.418211] NVRAM version: AP6256_NVRAM_V1.3_10092019.txt
[   13.500573] random: nonblocking pool is initialized
[   13.505678] dhd_bus_init: enable 0x06, ready 0x06 (waited 0us)
[   13.511910] bcmsdh_oob_intr_register: Enter
[   13.516230] bcmsdh_oob_intr_register: HW_OOB enabled
[   13.521376] bcmsdh_oob_intr_register OOB irq=72 flags=0x4
[   13.527009] bcmsdh_oob_intr_register: enable_irq_wake
[   13.532703] Disable tdls_auto_op failed. -1
[   13.537028] dhd_conf_set_intiovar: set WLC_SET_BAND 142 0
[   13.542795] dhd_preinit_ioctls: Set tcpack_sup_mode 0
[   13.548281] dhd_apply_default_clm: Ignore clm file /firmware/clm_bcmdhd.blob
[   13.556873] Firmware up: op_mode=0x0005, MAC=c0:84:7d:31:c8:c8
[   13.562917] dhd_conf_set_country: set country CN, revision 38
[   13.590855] Country code: CN (CN/38)
[   13.594898] dhd_conf_set_intiovar: set roam_off 1
[   13.608835] Firmware version = wl0: Sep 27 2019 15:21:10 version 7.45.96.53 (5a84613@shgit) 
                                (r745790) FWID 01-54faa385 es7.c5.n4.a3
[   13.621079]   Driver: 1.363.59.144.11 (r)
[   13.621079]   Firmware: wl0: Sep 27 2019 15:21:10 version 7.45.96.53 (5a84613@shgit) 
                                (r745790) FWID 01-54faa385 es7.c5.n4.a3 
[   13.637339]   clm = 9.2.9
[   13.640263] dhd_txglom_enable: enable 1
[   13.644226] dhd_conf_set_txglom_params: swtxglom=0, txglom_ext=0, txglom_bucket_size=0
[   13.652431] dhd_conf_set_txglom_params: txglomsize=36, deferred_tx_len=36, bus_txglom=-1
[   13.660809] dhd_conf_set_txglom_params: tx_in_rx=1, txinrx_thres=-1, dhd_txminmax=1
[   13.668723] dhd_conf_set_txglom_params: tx_max_offset=0, txctl_tmo_fix=1
[   13.675669] sdioh_set_mode: set txglom_mode to multi-desc
[   13.681259] dhd_preinit_ioctls: not define PROP_TXSTATUS
[   13.688554] dhd_conf_set_intiovar: set ampdu_hostreorder 0
[   13.694884] dhd_pno_init: Support Android Location Service
[   13.701057] tsk Enter, tsk = 0x84461498
[   13.705033] tsk->terminated = 0
[   13.719718] wl_host_event: Invalid ifidx 0 for wl0
[   13.724797] wl_host_event: Invalid ifidx 0 for wl0
[   13.748733] dhd_open: Exit ret=0
# udhcpc: started, v1.26.2
Successfully initialized wpa_supplicant
udhcpc: sending discover
[   16.320669] Connecting with 8c:0c:90:98:47:cd ssid "Guest", len (5) channel=153
[   16.320669] 
[   16.367807] wl_bss_connect_done succeeded with 8c:0c:90:98:47:cd
[   16.401524] wl_bss_connect_done succeeded with 8c:0c:90:98:47:cd
udhcpc: sending discover
udhcpc: sending select for 10.4.30.146
udhcpc: lease of 10.4.30.146 obtained, lease time 86400
deleting routers
adding dns 219.141.136.10
adding dns 202.106.0.20
```

### 3.2.3.执行ping命令连网

```c
# ping www.baidu.com
PING www.baidu.com (220.181.38.150): 56 data bytes
64 bytes from 220.181.38.150: seq=0 ttl=53 time=14.347 ms
64 bytes from 220.181.38.150: seq=1 ttl=53 time=9.873 ms
64 bytes from 220.181.38.150: seq=2 ttl=53 time=12.889 ms
```



