# emmc模块

## 1.Emmc控制器简介

DWC_mshc是一种可高度配置和可编程的高性能移动存储主控制器，其数据传输的总接口位AXI。Emmc 是英文 Embedded Multi-Media Card（嵌入式多媒体卡）的缩写，x2000支持emmc协议Electrical Standard（5.1）。

* GPIO功能描述

|Name|I/O|Funtion|
|-|-|-|
MSC0_CLK|GPIO-PD17|FUNTION0
MSC0_CMD|GPIO-PD18|FUNTION0
MSC0_DATA|GPIO-(PD19~PD26)|FUNTION0

## 2.内核空间

## 2.1.驱动相关的源码

```c
drivers/mmc/host
 ├── sdhci.c                
 ├── sdhci-ingenic.c        /*控制器通过这个配置*/

```

## 2.2.设备树配置

```c
&msc0 {
        status = "okay";                /*配置“okay”编译emmc驱动，“disable”反之*/
        cap-mmc-highspeed;              /*配置emmc支持的传输模式*/
        non-removable;
        max-frequency = <50000000>;     /*通过配置该参数来指定当前模式下最大频率*/
        bus-width = <4>;                /*配置数据总线宽度，支持1bit、4bit、8bit*/
        non-removable;
        voltage-ranges = <1800 3300>;   /*指定电压范围*/

        /* special property */
        ingenic,wp-gpios = <0>;
        ingneic,cd-gpios = <0>;
        ingenic,rst-gpios = <0>;
};
```

## 2.3.驱动配置

```c
Device Drivers  --->
  <*> MMC/SD/SDIO card support  --->
            --- MMC/SD/SDIO card support
      [ ]   MMC debugging
            *** MMC/SD/SDIO Card Drivers ***
      <*>   MMC block device driver
      (8)     Number of minors per block device
      [*]     Use bounce buffer for simple hosts
      < >   SDIO UART/GPS class support
      < >   MMC host test driver
            *** MMC/SD/SDIO Host Controller Drivers ***
      <*>   Ingenic(XBurst2)  MMC/SD Card Controller(MSC) support
      -*-   Secure Digital Host Controller Interface support
```

## 3.用户空间

## 3.1.驱动加载成功

```c
[    1.575803] mmc1: new ultra high speed SDR104 SDHC card at address aaaa
[    1.593001] mmcblk0: mmc1:aaaa SC16G 14.8 GiB 
[    1.601920] Alternate GPT is invalid, using primary GPT.
[    1.607432]  mmcblk0: p1 p2 p3 p4 p5 p6 p7
```

## 3.2.设备节点

```c
/dev/mmcblk0
/dev/mmcblk0p1~p7 /*对应emmc的分区*/
```

## 3.3.测试方法
1. 测试emmc读写

* 源码位置
```c
testsuite/StorageMedia-Test.sh
```

* 测试方法
./StorageMedia-Test.sh