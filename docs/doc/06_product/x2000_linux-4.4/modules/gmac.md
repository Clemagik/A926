# 一、模块简介
X2000芯片有2个完全独立的gmac控制器， 控制器支持RGMII和RMII接口。

RGMII（Reduced Gigabit Media Independent Interface）是Reduced GMII（吉比特介质独立接口）。RGMII均采用4位数据接口，工作时钟125MHz，并且在上升沿和下降沿同时传输数据，因此传输速率可达1000Mbps。RGMII数据结构符合IEEE以太网标准，接口定义见IEEE 802.3-2000, RGMII支持10/100/1000兆的总线接口速度。

RMII \(Reduced Media Independent Interface\) 简化媒体独立接口,是IEEE 802.3u标准中除MII接口之外的另一种实现。RMII支持10兆和100兆的总线接口速度。

# 二、u-boot

uboot只支持一个gmac控制器工作，需要在对应板级头文件（include/configs/板级.h）中配置选择的控制器和对应控制器的接口模式。

配置文件include/configs/板级.h 如下：
```
/* Select GMAC Controller */
#define CONFIG_GMAC1          /*根据实际测试，配置对应的GMAC1、GMAC0*/

/* Select GMAC Interface mode */
#define CONFIG_RGMII          /*根据实际测试，配置对应RGMII、RMII*/

CONFIG_GMAC_PHY_RESET        /* 按照电路图配置phy的reset gpio */

CONFIG_GMAC_TX_CLK_DELAY     /* RGMII模式时 设置tx_clk delay */
CONFIG_GMAC_RX_CLK_DELAY     /* RGMII模式时 设置rx_clk delay */
```

如果kernel配置双网络控制器，当通过uboot传参启动网络文件系统时在设置bootargs时需要指定启动网络文件系统的的设备，设置形式如下：
```
setenv bootargs 'console=ttyS2,115200 mem=128M@0x0 ip=192.168.4.43:192.168.4.1:192.168.4.1:255.255.255.0::eth0:off nfsroot=192.168.4.93:/home/user/x2000-v12/system rw'  /*根据选择的网口，修改eth0:off或eth1:off*/
```

# 三、内核空间

驱动代码位置：
```
drivers/net/ethernet/ingenic
 ├── ingenic_mac.c
 ├── synopGMAC_Dev.c
 ├── synopGMAC_plat.c
 ```

## 1. 设备树配置

**1 、RGMII在arch/mips/boot/dts/ingenic/板级.dts下配置：**

```
设备mac0的rgmii配置:
&mac0 {
        pinctrl-names = "default", "reset";
        pinctrl-0 = <&mac0_rgmii_p0_normal>, <&mac0_rgmii_p1_normal>;
        pinctrl-1 = <&mac0_rgmii_p0_rst>, <&mac0_rgmii_p1_normal>;
        status = "okay";        /*根据需求配置okay 或 disable*/
        ingenic,rst-gpio = <&gpb 0 GPIO_ACTIVE_LOW INGENIC_GPIO_NOBIAS>;
        ingenic,rst-ms = <10>;
        ingenic,mac-mode = <RGMII>;
        ingenic,mode-reg = <0xb00000e4>;
        ingenic,rx-clk-delay = <0x2>;
        ingenic,tx-clk-delay = <0x3f>;
};


设备mac1的rgmii配置:
&mac1 {
        pinctrl-names = "default", "reset";
        pinctrl-0 = <&mac1_rgmii_p0_normal>, <&mac1_rgmii_p1_normal>;
        pinctrl-1 = <&mac1_rgmii_p0_rst>, <&mac1_rgmii_p1_normal>;
        status = "okay";         /*根据需求配置okay 或 disable*/
        ingenic,rst-gpio = <&gpb 26 GPIO_ACTIVE_LOW INGENIC_GPIO_NOBIAS>;
        ingenic,rst-ms = <10>;
        ingenic,mac-mode = <RGMII>;
        ingenic,mode-reg = <0xb00000e8>;
        ingenic,rx-clk-delay = <0x2>;
        ingenic,tx-clk-delay = <0x3f>;
};
```

**2 、RMII在arch/mips/boot/dts/ingenic/板级.dts下配置：**

```
设备mac0的rmii配置:
&mac0 {
        pinctrl-names = "default", "reset";
        pinctrl-0 = <&mac0_rmii_p0_normal>, <&mac0_rmii_p1_normal>;
        pinctrl-1 = <&mac0_rmii_p0_rst>, <&mac0_rmii_p1_normal>;
        status = "okay";       /*根据需求配置okay 或 disable*/
        ingenic,rst-gpio = <&gpb 0 GPIO_ACTIVE_LOW INGENIC_GPIO_NOBIAS>;
        ingenic,rst-ms = <10>;
        ingenic,mac-mode = <RMII>;
        ingenic,mode-reg = <0xb00000e4>;
};

设备mac1的rmii配置:
&mac1 {
        pinctrl-names = "default", "reset";
        pinctrl-0 = <&mac1_rmii_p0_normal>, <&mac1_rmii_p1_normal>;
        pinctrl-1 = <&mac1_rmii_p0_rst>, <&mac1_rmii_p1_normal>;
        status = "disable";      /*根据需求配置okay 或 disable*/
        ingenic,rst-gpio = <&gpb 26 GPIO_ACTIVE_LOW INGENIC_GPIO_NOBIAS>;
        ingenic,rst-ms = <10>;
        ingenic,mac-mode = <RMII>;
        ingenic,mode-reg = <0xb00000e8>;
};
```
**3 、需要根据具体设备更改的配置选项：**

依据开发板设计更改mac设备对应phy的复位gpio,复位有效电平,复位需要的保持的时间

如果工作在RGMII模式下需要配置TXCLK和RXCLK的delay,配置的精度是19.5ps,配置值范围0-128,delay的时间范围0-2.5ns.具体配置值需要由phy来确定,TXCLK和RXCLK和data保证2ns左右的delay,所以如果phy端已经做了delay,mac控制器端就不需要设置或补足差值

## 2. 驱动配置

**1、在电脑端输入 make menuconfig， 然后配置：**

```
--- Network device support
[*]   Network core driver support
[*]   Ethernet driver support  --->
    --- Ethernet driver support
    <*> ingenic on-chip MAC support
    [*] Ingenic mac dma interfaces
            Ingenic mac dma bus interfaces (MAC_AXI_BUS)  --->
<*>   PHY Device support and infrastructure  --->
```

# 四、用户空间

**1、测试网络是否正常：**

执行ifconfig -a 命令查看支持的网络设备

```
# ifconfig -a
eth0      Link encap:Ethernet  HWaddr 1E:01:7F:E6:D3:26     /*设备树配置GMAC0后显示设备接口eth0*/
          BROADCAST MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

eth1      Link encap:Ethernet  HWaddr 3E:4C:9D:F4:74:4B     /*设备树配置GMAC1后显示设备接口eth
          BROADCAST MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

lo        Link encap:Local Loopback
          LOOPBACK  MTU:65536  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)
```

配置网络

```
ifconfig eth0 IP                /*根据实际选择的网口配置eth0或eth1*/
```

使用ping命令查看网络连接情况

```
ping IP -I eth0                /*通过-I 选项可以指定ping 命令使用的网口*/
```

**2 、网络性能测试:**

找一台有千兆以太网功能的电脑，使用网线直连，使用iperf3命令测试，电脑端作为iperf3命令的服务端，开发板端作为命令的客户端

电脑端执行命令
```
iperf3  -s
```
测试单向发送性能
```
iperf3  -c 192.168.4.105 -u -b 1000M -l 65507 -t 10
```
测试单向接收性能
```
iperf3  -c 192.168.4.105 -u -b 1000M -l 65507 -t 10 -R
```
