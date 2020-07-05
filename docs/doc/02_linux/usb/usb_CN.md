[TOC]
# USB 专项测试步骤及环境搭建

##一. uboot usb 相关功能测试:

###A. 相关代码位置:
####1. dwc2-device控制器驱动
	drivers/usb/gadget/jz47xx_dwc2_udc.c

####2. dwc2-host控制器驱动
	drivers/usb/host/dwc2.c

####3. ohci驱动
	driver/serial/jz_usb_serial.c

####4. device-function
	(1)	drivers/usb/gadget/g_serial.c     作为串口烧录设备
	(2)	drivers/usb/gadget/g_fastboot.c   作为快速烧录设备
	(3) drivers/usb/gadget/hid/hid.c	作为hid作为键盘或鼠标设备

####5. host-function
	（1）   common/uvc.c                 支持摄像头
	（2）   common/usb_storage.c       支持u盘
	（3）   common/usb_kbd.c            支持hid
	（4）   driver/serial/jz_serial     支持串口

####6. 测试代码
	 (1)./common/cmd_usb.c               测试摄像头、U盘、hid
	 (2) ./common/cmd_usbserial.c      测试作为串口烧录设备
	 (3) ./common/cmd_fastboot.c       测试快速烧录设备摄像头、U盘、hid
	 (4) ./common/cmd_hid.c	测试hid功能

###B. 测试流程
####1.相关宏定义
库里最新下载的代码原则上不需要修改源码,只需根据需求定义打开相应的宏就可以. 以halley2 为例,  ./include/configs/halley2.h中定义了几个宏, 依次打开可以测试相应的功能,最好不要同时打开可能会造成冲突. 具体的宏如下:

* Host 端相关宏定义：

		打开宏定义： CONFIG_CMD_USB　可以测试USB HOST相关功能,如摄像头、U盘、hid等功能. 测试摄像头需要加上　CONFIG_JZ_MMC_MSC0　测试，把拍摄的照片直接保存在sd卡里面。
		-/*#define CONFIG_CMD_USB*/             /* USB host command */
		+#define CONFIG_CMD_USB         /* USB host command */
 		#ifdef CONFIG_CMD_USB
 		#define CONFIG_FAT_WRITE       /* Support fatfs write */
 		#define CONFIG_USB_UVC         /* Support uvc. */

		+#define CONFIG_JZ_MMC_MSC0　　　/*Supoort mmc for usb camera*/

 		#define CONFIG_USB_STORAGE     /* Support u-disk. */
 		#define CONFIG_USB_DWC2                /* DWC2 Host Driver. */
 		#define CONFIG_USB_DRV_VBUS    GPIO_PB(25)
 		#endif

 		去掉lcd 的配置，否则超出了256k的uboot 的大小的限制。
		-#define CONFIG_LCD
		+/*#define CONFIG_LCD */

* Device 端相关宏定义：

		/*#define CONFIG_CMD_FASTBOOT   *//* USB device command */
		/*#define CONFIG_CMD_USBSERIAL*/
		 #define CONFIG_CMD_USBHID
		这三个宏不要同时打开,最好一个一个测试 usb device功能.
		

####2.常用命令
编译后烧录到halley2板子上,开机后串口中按任意按键进入uboot 命令行模式,可以输入help usb查看可用的命令.
常用命令:

* Host

[1]插入u盘后
```
# usb start (扫描所有连接的usb device,如U盘,usbcamera等)
# usb info(显示扫描到的设备如U盘信息)
# usb tree(usb 设备树结构)
```
[2] 挂载u盘后可以使用命令
```
# fatls usb 0
# fatinfo usb 0
# fatload usb 0 0x30000000 uboot.bin
```

[3] uboot usb camera 测试命令
```
# mmc rescan
# usb start
# uvctest
```
插入一张sd卡（fat32格式）,命令运行成功后会将拍摄的照片保存到sd卡里面。

* Device

[1] uboot 通过 fastboot 命令加载内核镜像

 `使用方法：`
在 PC 端使用以下命令安装 fastboot 应用程序:
```
$ sudo apt-get install android-tools-fastboot
```
(1) 进入开发板调试串口,在 uboot 启动过程中,敲击任意按键,打断 uboot 引导镜像过
程,进入 uboot shell 模式,输入fastboot 命令:
```
 # fastboot
```
(2) 在 PC 端存放 kernel 镜像文件(uImage)的路径下,执行以下命令:
```
$ sudo fastboot boot uImage
```
命令执行成功后,即可下载 kernel 镜像(uImage)到内存并启动 kernel。
Fastboot 是使用 USB 端口把 uImage 下载到内存并启动 kernel 命令,只用于调试 kernel镜像,不支持 android fastboot 其他功能。
[2]　uboot  serial 功能
uboot 需要打开`include/configs/halley2.h` 文件以下两个宏定义
```
#define CONFIG_CMD_USBSERIAL
#define CONFIG_ARDUINO
```
开发板端进入uboot shell 模式执行以下命令：
```
# gser
```

Manhatton demo 路径：
packages/example/Sample/usb_test/acm
demo 编译：

```
$ cd <Manahhton_Project>
$ make HOST-usb_acm

pc 端运行的目标文件：out/host/tools/usb_acm
```
PC 出现设备节点：/dev/ttyACM0，运行usb_acm 程序:

```
$ sudo ./usb_acm
```
[3] usb hid 功能
uboot 需要打开`include/configs/halley2.h` 文件以下两个宏定义
```
#define  CONFIG_CMD_USBHID
#define CONFIG_ARDUINO
```
开发板端进入uboot shell 模式执行以下命令：
```
# hid
```
在PC端(测试环境ubuntu 18.04)出现  /dev/hidrawX 文件

在PC端(测试环境ubuntu 18.04)执行 lsusb
出现
Bus 003 Device 029: ID 0525:a4ac Netchip Technology, Inc.
其中ID为 0525:a4ac

在PC端(测试环境ubuntu 18.04)打开记事本，按下halley2的SW2按键,记事本出现"123"字符串。

##二. kernel usb 相关功能测试:

###kernel 代码位置（基于kernel3.10）

usb相关代码位置， 实现功能
####1.  dwc2控制器驱动
```
（1）driver/usb/dwc2   （host / device）   （老客户使用的较多）
（2）driver/staging/dwc2      （host / device）   （主要优势： 支持splite）
```
####2.  usbdevice_function代码
```
（1）  driver/usb/gadget/f_uac1.c      作为使用audio1.0的声卡
（2）  driver/usb/gadget/f_uac2.c      作为使用audio2.0的声卡
（3）  driver/usb/gadget/f_uvc.c       作为摄像头
（4）  drive/usb/gadget/f_adb.c       作为使用f_adb的adb设备
（5）  driver/usb/gadget/f_seria.c     作为串口
（6）  driver/usb/gadget/f_mass_storage.c  作为U盘
（7）  driver/usb/gadget/f_hid.c       作为hid
```
####3. usbhost_function代码
```
（1）  sound/usb/                      支持声卡
（2）  drivers/media/usb/              支持显卡
（3）  drivers/usb/storage/            支持U盘
（4）  drivers/usb/serial/             支持串口
（5）  drivers/hid/usbhid              支持hid
```
####4.  ohci控制器驱动
```
    drivers/usb/host/ohci-jz.c  等
```
####5.  phy驱动
```
    arch/mips/...../cpm_usb.c
```
####6.  测试应用
```
（1） grab	测试“支持摄像头”
（2） webcam     测试“作为摄像头”
（3） usbhid     测试“作为hid”
（4） getevent   测试“支持hid”
（5） usb配置脚本
```

资源相关文件路径:
```
A) 板级资源定义路径:arch/mips/xburst/soc-x1000/common/platform.c
B) 平台设备注册文件路径:
arch/mips/xburst/soc-x1000/chip-x1000/halley2/common/ board_base.c
C) GPIO 申请文件路径:
在 “ arch/mips/xburst/soc-x1000/chip-x1000/halley2/common/misc.c ” 中 申 请 userusb 的
dete_pin,id_pin,bus_pin
D) usb测试所需脚本和资源: 本文档用到的相关测试脚本.
```
usb 控制器驱动的目录如下:
```
drivers/usb/dwc2/
|——dwc2
|——dwc2_jz.c
#usb 控制器实现
|——core.c
#usb core 层在控制器中的具体实现
|——gadget.c
#usb 一些工具在控制器的具体实现
|——rh.c
|——debugfs.c
#debugfs 虚拟文件系统的实现
|——ep0.c
|——host-ddma.c
```
测试步骤:

在发布版本中会默认添加 usb 驱动, usb 分为 host 端和 device 端,如果想要自己改变 usb
的编译配置可以通过以下进行配置:

###PART A.  usb-host

####1.usb-mass_storage:
在电脑端输入 make menuconfig,然后配置:
```
Device Drives
->USB support
->DesignWare USB2 DRD Core Support
->Driver Mode
->Both host and device.
```
同时在本级目录下选上 USB Mass Storage support,然后配置:
```
->DeviceDrivers
->SCSI device support
```
编译配置： USB_STORAGE
现在 usb-host-mass_storage 功能就配置成功。当有 u 盘插入时会有如下提示:
```
[22.092290] jz-dwc2 jz-dwc2: set vbus on(on) for host mode
[22.202174] USB connect
[22.902209] usb 1-1: new high speed USB device number 2 using dwc2
[23.328262] usb 1-1: New USB device found, idVendor=0930, idProduct=6545
[23.342090] usb 1-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[23.356745] usb 1-1: Product: DataTraveler 2.0
[23.365858] usb 1-1: Manufacturer: Kingston
[23.374570] usb 1-1: SerialNumber: C86000BDBA09EF60CA285106
[23.412410] scsi0 : usb-storage 1-1:1.0
[24.475824] scsi 0:0:0:0: Direct-Access Kingston DataTraveler 2.0 PMAP PQ: 0 ANSI: 4
[25.751099] sd 0:0:0:0: [sda] 30497664 512-byte logical blocks: (15.6 GB/14.5 GiB)
[25.768695] sd 0:0:0:0: [sda] Write Protect is off
[25.779135] sd 0:0:0:0: [sda] No Caching mode page present
[25.790501] sd 0:0:0:0: [sda] Assuming drive cache: write through
[25.807171] sd 0:0:0:0: [sda] No Caching mode page present
[25.832513] sd 0:0:0:0: [sda] Assuming drive cache: write throug
[25.879083] sda:sdal
[25.895075] sd 0:0:0:0: [sda] No Caching mode page present
[25.932372] sd 0:0:0:0: [sda] Assuming drive cache: write through
[25.964595] sd 0:0:0:0: [sda] Attached SCSI removable disk
```
验证   ：  君正设备 mnt/sdx1 mount u盘盘符

####2.USB Camera
#####2.1. USB Camera
驱动配置
再上述的基础上,配置:
```
-> Device Drivers
-> Multimedia support
-> Media USB Adapters
-> USB Video Class
```
就是可以使用 USB Camera。

#####2.2. grab 测试
grab 以及其他测试脚本,可在与本文档一起发布的文件, "usb测试所需脚本和资源" 目录下获取.

grab 应用代码:packages/example/App/grab
将应用编译进文件系统,具体方式不再赘述,详细过程请参见《Manhattan_platform 编译系统
使用指南》,整体编译后 grab 应用路径:/usr/bin/grab。
打开串口,开发板上电启动之后,执行以下命令使用 USB Camera 功能:
```
# grab -w 320 -h 240 -c 10 -r 5 -y -d /dev/video1
```
 (/dev/video1取决于usb camera生成的设备节点)
注:在当前目录下执行以下命令可查看具体参数说明:
```
#grab --help
```
串口打印如下则 USB Camera 照相成功:
```
savejpeg.c:get_pictureYUYV:293
```
照相成功后会在当前路径下产生“p-x.jpg”照片文件,其中“x”为”0~9”。可在 pc 端输入以下命令查看图片:
下面以查看“p-0.jpg”为例:
```
$adb pull /p-0.jpg .
```
命令执行成功后即可在本地查看图片”p-0.jpg”是否正确。

####3.  支持usb audio
编译配置： SND_USB_AUDIO
```
Symbol: SND_USB_AUDIO [=n]
Type  : tristate
Prompt: USB Audio/MIDI driver
  Location:
    -> Device Drivers
      -> Sound card support (SOUND [=y])
        -> Advanced Linux Sound Architecture (SND [=y])
(1)       -> USB sound devices (SND_USB [=n])
  Defined at sound/usb/Kconfig:12
  Depends on: SOUND [=y] && !M68K && !UML && SND [=y] && SND_USB [=n] && USB [=y]
  Selects: SND_HWDEP [=n] && SND_RAWMIDI [=n] && SND_PCM [=y]
```
编译配置： STAGING
```
There is no help available for this option.
Prompt: DWC2 Mode Selection
  Location:
    -> Device Drivers
      -> Staging drivers (STAGING [=y])
        -> DesignWare USB2 DRD Core Support (USB_DWC2 [=y])
  Defined at drivers/staging/dwc2/Kconfig:18
  Depends on: STAGING [=y] && USB_DWC2 [=y]
  Selected by: STAGING [=y] && USB_DWC2 [=y] && m
```
取消传统USB dwc2 驱动配置（dwc2 控制器驱动二选一）：
```

Symbol: USB_JZ_DWC2 [=n]
Type  : tristate
Prompt: DesignWare USB2 DRD Core Support
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
  Defined at drivers/usb/dwc2/Kconfig:1
  Depends on: USB_SUPPORT [=y]


```
设备参数： 海备斯、mojo声卡
验证   ： 君正设备执行  aplay -D hw:1,0  caiqin_short_48khz_16bit_2.wav

(海贝斯测试ok,需要修改配置文件 stageing 下的usb audio 驱动)

####4.  支持hid
编译配置： USB_MOUSE
```
Symbol: USB_HID [=n]
Type  : tristate
Prompt: USB HID transport layer
  Location:
    -> Device Drivers
      -> HID support
        -> USB HID support

Symbol: USB_MOUSE [=m]
Type  : tristate
Prompt: USB HIDBP Mouse (simple Boot) support
  Location:
    -> Device Drivers
      -> HID support
        -> USB HID support
          -> USB HID Boot Protocol drivers
  Defined at drivers/hid/usbhid/Kconfig:66
  Depends on: USB_HID [=n]!=y && EXPERT [=y] && USB [=y] && INPUT [=y]
```
设备参数： 能找到的所有鼠标
验证   ：  君正设备执行 cat /dev/input/eventx

####5.  支持serial
编译配置： USB_SERIAL
```
CONFIG_USB_SERIAL_FTDI_SIO:

Say Y here if you want to use a FTDI SIO single port USB to serial
converter device. The implementation I have is called the USC-1000.
This driver has also be tested with the 245 and 232 devices.

See <http://ftdi-usb-sio.sourceforge.net/> for more
information on this driver and the device.

To compile this driver as a module, choose M here: the
module will be called ftdi_sio.

Symbol: USB_SERIAL_FTDI_SIO [=m]
Type  : tristate
Prompt: USB FTDI Single Port Serial Driver
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
        -> USB Serial Converter support (USB_SERIAL [=m])
  Defined at drivers/usb/serial/Kconfig:148
  Depends on: USB_SUPPORT [=y] && USB [=y] && USB_SERIAL [=m]
```
测试步骤:
设备参数：ftdi设备(usb 转串口设备)
Manhatton demo 路径：
packages/example/Sample/usb_test/serial
demo编译：
`host 端:`
```
# cd packages/example/Sample/usb_test/serial
# mm
```
`pc 端:`

```
# cd <Manhatton_Project>
# make HOST-usb_serial
```
目标文件：
host 端：
out/product/halley2/system/testsuite/usb_test/kernel_host/serial_host_sample/serial_host_sample
pc 端：
out/host/tools/usb_serial

* 验证
 pc 端节点：/dev/ttyUSBX

`host设备:`

```
# ./serial_host_sample
```
`pc端:`
```
$ sudo ./usb_serial
```

####6.  支持printer
Host 编译配置： USB_PRINTER
```
CONFIG_USB_PRINTER:

Say Y here if you want to connect a USB printer to your computer's
USB port.

To compile this driver as a module, choose M here: the
module will be called usblp.

Symbol: USB_PRINTER [=y]
Type  : tristate
Prompt: USB Printer support
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
        -> Support for Host-side USB (USB [=y])
  Defined at drivers/usb/class/Kconfig:21
  Depends on: USB_SUPPORT [=y] && USB [=y]


Symbol: USB_DWC2 [=y]
 Type  : tristate
 Prompt: DesignWare USB2 DRD Core Support
   Location:
     -> Device Drivers
       -> Staging drivers (STAGING [=y])
   Defined at drivers/staging/dwc2/Kconfig:1
   Depends on: STAGING [=y] && (USB [=y] || USB_GADGET [=y])


 Symbol: USB_JZ_DWC2 [=n]
Type  : tristate
Prompt: DesignWare USB2 DRD Core Support
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
  Defined at drivers/usb/dwc2/Kconfig:1
  Depends on: USB_SUPPORT [=y]

```
Device测试设备为另一台君正设备模拟的打印机, 把另一块相同的halley2板子配置成printer的device（详见PART B. 7.printer usb gadget 小结）,用usb线把两个开发板相连.device 端需要配置成full speed 模式才能被host 端设备识别：
```
Symbol: USB_G_PRINTER [=y]
Type  : tristate
Prompt: Printer Gadget
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
        -> USB Gadget Support (USB_GADGET [=y])
          -> USB Gadget Drivers (<choice> [=y])
  Defined at drivers/usb/gadget/Kconfig:997
  Depends on: <choice>
  Selects: USB_LIBCOMPOSITE [=y]

Symbol: USB_JZ_DWC2 [=y]
 Type  : tristate
 Prompt: DesignWare USB2 DRD Core Support
   Location:
     -> Device Drivers
       -> USB support (USB_SUPPORT [=y])
   Defined at drivers/usb/dwc2/Kconfig:1
   Depends on: USB_SUPPORT [=y]

Symbol: USB_DWC2_FULLSPEED_DEVICE [=y]
Type  : boolean
Prompt: dwc2 full speed gadget
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
        -> DesignWare USB2 DRD Core Support (USB_JZ_DWC2 [=y])

```

* 验证
`device 端设备执行:`
```
# echo xxxx > /dev/usb/lp0
```
`host 端设备执行`
```
# cat /dev/g_printer
```


###PART B.   usb-device

目前 gadget 功能使用 android gadget , android gadget 目前支持 mass_storage, adb, rndis 功能。
```
Device Drivers
->USB support
->USB Gadget support
->USB Gadget Drivers (Android Gadget)
```
下面分别说明上述三个功能如可开启:

####1. USB adb & mass_storage

在文件系统中输入下面命令:
```
# cd /sys/class/android_usb/android0
# echo 0 > enable
# echo 18d1 > idVendor
# echo d002 > idProduct
# echo mass_storage,adb > functions
# echo 1 > enable
# /sbin/adbd &
```
现在 usb 支持了 adb 和 mass_storage 功能。

具体验证方法:
```
####(1)验证 adb:
在电脑端输入 adb shell 出现/sys/devices/virtual/android_usb/android0 # 说明 adb 开启成功。
(2)验证 mass_storage:

PC 端:
$ dd if=/dev/zero of=fat32.img bs=1k count=2048 #制作一个大小为 2M 的空文件
$ mkfs.vfat fat32.img #把这个文件格式化为 vfat 格式

开发板端:
# echo fat32.img > sys/devices/platform/jz-dwc2/dwc2/gadget/lun0/file
如果成功会在电脑端弹出 u 盘的盘符。
```

工程中为了方便用户的使用,加入了 mass_storage demo,用户可按照如下步骤进行验证:

#####1.1. Mass_storage demo 简介:
此功能主旨是为用户提供更加丰富的调试功能,以便于用户开发。此功能基于 usb
mass_storage,同时结合开发板上的存储设备(如:/dev/mmcblk0p1, /dev/ram0)来形成。

#####1.2. 使用方法:
首先在开发板上准备一个存储设备分区,如 sd 卡,nor 分区,emmc nand 分区等。
在开发板上执行如下命令:
```
# udc_mass_storage.sh <设备分区> <挂载目录>
```
以 mmc 为例,命令为:udc_mass_storage.sh /dev/mmcblk0p1 /mnt/udc
就会在 PC 端弹出一个 u 盘,此时 PC 端可以像操作普通 U 盘一样,操作目标板挂载目
录所弹出的盘符。

#####1.3. 数据 copy
A. Pc 到目标板:
此种情况下与普通的 u 盘使用的方式是一样的,将文件通过 pc 直接 copy
到目标板挂载目录即可。
B. 目标板到 pc:此种情况下与普通的 u 盘使用的方式是一样的,将目标板“挂载目录”
中的文件通过 PC 端得盘符直接 copy 到 PC 上即可,需要注意的是,使用此种功能时,如果
需要 copy 的文件不在目标板的“挂载目录”下,不能直接在目标板上 copy 所需文件到“挂
载目录”下,因为此时目标板的“挂载目录”为只读模式。用户需将 copy 文件准备到“挂载
目录”下后,才能使用此功能将所需文件 copy 到 PC 上。

####2. USB RNDIS功能:
Ubuntu USB 网络测试环境搭建
Ubuntu PC要确认支持RNDIS功能, 如果不支持,百度一下,安装相关工具和驱动.
举例仅供参考,具体因各个PC具体环境自己安装.

 USB Host安装，以RHEL5为例，直接运行如下命令即可安装 rndis_host.ko
```
     modprobe rndis_host
```
  它将安装 rndis_host.ko和usbnet.ko

    [root@hxy]# find /lib/modules/2.6.18-194.el5/ -name "rndis*"
    /lib/modules/2.6.18-194.el5/kernel/drivers/usb/net/rndis_host.ko
    [root@hxy]# modprobe rndis_host
    [root@hxy]# lsmod | grep rndis_host
    rndis_host 10433 0
    cdc_ether 9793 1 rndis_host
    usbnet 19913 2 rndis_host,cdc_ether

在halley2发板上编译好内核模块
```
# cd /sys/class/android_usb/android0
# echo 0 > enable
# echo 18d1 > idVendor
# echo d002 > idProduct
# echo rndis > functions
# echo 1 > enable
```
配置完后同时在电脑端和设备端出现 usb0 网络端点,可用 ifconfig -a 查看,把这两个 usb0
配置成两个不同的 ip,可用 ping 命令来检测是否成功。

分别在主机和开发板之间抽入USB线,其中开发板使用D型的Slave端接口（不要插在ＨＯＳＴ接口上！） 让USB网卡结点生效,这里一般是usb0
```
 # ifconfig usb0 192.168.3.101 up
```
在主机端做类似配置,配成另一个ip即可
```
$ ifconfig usb0 192.168.3.120 up

    # ifconfig usb0 192.168.3.120 up
    usb 1-1: new high speed USB device using ehci_hcd and address 5
    usb 1-1: configuration #1 chosen from 2 choices
    usb0: register 'cdc_ether' at usb-0000:00:1d.7-1, CDC Ethernet Device, 32:90:26:92:52:a9
```
这时在两端即可以互相 ping 通。

####3.USB Audio Gadget 功能
#####3.1.USB Audio Gadget 功能
以模块方式选择USB AUDIO 驱动
```
Prompt: USB Gadget Drivers
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
        -> USB Gadget Support (USB_GADGET [=y])


Symbol: USB_AUDIO [=m]
 Type  : tristate
 Prompt: Audio Gadget
   Location:
     -> Device Drivers
       -> USB support (USB_SUPPORT [=y])
         -> USB Gadget Support (USB_GADGET [=y])
           -> USB Gadget Drivers (<choice> [=m])
```
选中UAC1:
```
CONFIG_GADGET_UAC1:

If you instead want older UAC Spec-1.0 driver that also has audio
paths hardwired to the Audio codec chip on-board and doesn't work
without one.

Symbol: GADGET_UAC1 [=y]
Type  : boolean
Prompt: UAC 1.0 (Legacy)
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
        -> USB Gadget Support (USB_GADGET [=y])
          -> USB Gadget Drivers (<choice> [=y])
            -> Audio Gadget (USB_AUDIO [=y])
  Defined at drivers/usb/gadget/Kconfig:772
  Depends on: <choice> && USB_AUDIO [=y]
```
#####3.2.编译方式
```
$ make uImage /*烧录新编译生成的 uImage */
$　make modules /*以模块的方式编译 USB Audio*/
```
生成文件路径:
kernel/drivers/usb/gadget/
生成文件:
g_audio.ko
libcomposite.ko
将上述生成的.ko 文件 push 到开发板中:
```
$ adb push libcomposite.ko /
$ adb push g_audio.ko /
```
Note:
经过上述重选的驱动中没有支持 adb ,不能正常使用 adb 命令,建议先将上述的 g_audio.ko
libcomposite.ko 文件 push 到开发板中(在有 adb 支持的驱动条件下),之后重新烧录新编译的
uImage.

#####3.3.使用方式
开发板端:
```
# insmod libcomposite.ko
#insmod g_audio.ko
[40.929424] start set AIC register....
udhcpc: sending discover
[41.596439] g_audio gadget: play, Hardware params: access 3, format 2, channels 2, rate 48000
[41.605365] start set AIC register....
[41.611408] g_audio gadget: audio_buf_size 48000, req_buf_size 384, req_count 256
[41.626167] g_audio gadget: Linux USB Audio Gadget, version: Feb 2, 2012
[41.633090] g_audio gadget: g_audio ready
```
(需要君正设备事先生成/dev/snd/pcmC0D0c， /dev/snd/controlC0)
PC 端: 以ubuntu为例, 打开右上角的声音设置选项,  点选audio gadget output选项, 就可以通过电脑往halley2开发板上播放音乐了.

####4.  serial usb gadget


S90serial 配置脚本：
```
#!/bin/sh
#
# Start serial....
#

case "$1" in
  start)
	echo "Starting serial..."
	if [ -d /sys/class/android_usb/android0 ]
	then

		echo 1 > /sys/devices/virtual/android_usb/android0/f_acm/instances
		cd /sys/class/android_usb/android0
		value=`cat ./functions`
		if [ "$value" != "acm" ]
		then
			echo 0 > enable
			echo 18d1 > idVendor
			echo d002 > idProduct
			echo acm > functions
			echo 1 > enable
		fi
		cd -
	else
		echo "notice : mass storage and adb don't use, kernel config error"
	fi
	;;
  stop)
	;;
  restart|reload)
	;;
  *)
	echo "Usage: $0 {start|stop|restart}"
	exit 1
esac

exit $?

```
* 编译配置： USB_G_ANDROID
* 验证

 `君正设备执行:`
```
 # ./S90serial start
 # echo xxx  > /dev/ttyGS0
```
`PC执行:`
```
  $  sudo cat /dev/ttyACMX
```
####5.  camera usb gadget
编译配置： USB_G_ANDROID
gadget选项中选择Android Composite Gadget后,在此基础上同时选择 		  USB_ANDROID_WEBCAM

```
CONFIG_USB_ANDROID_WEBCAM:

Enable webcam function in android


Symbol: USB_ANDROID_WEBCAM [=y]
Type  : boolean
Prompt: Use webcam in android
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
        -> USB Gadget Support (USB_GADGET [=y])
          -> USB Gadget Drivers (<choice> [=y])
            -> Android Composite Gadget (USB_G_ANDROID [=y])
  Defined at drivers/usb/gadget/Kconfig:1078
  Depends on: <choice> && USB_G_ANDROID [=y]

```
usb webcamera 和usb audio　属于等时传输,建议使用staging driver dwc2 驱动,staging driver dwc2 驱动（等时传输建议选中）配置：
```
Symbol: USB_DWC2 [=y]
Type  : tristate
Prompt: DesignWare USB2 DRD Core Support
  Location:
    -> Device Drivers
      -> Staging drivers (STAGING [=y])

Symbol: USB_JZ_DWC2 [=n]
Type  : tristate
Prompt: DesignWare USB2 DRD Core Support
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])

CONFIG_USB_ANDROID_WEBCAM:

Enable webcam function in android


Symbol: USB_ANDROID_WEBCAM [=y]
Type  : boolean
Prompt: Use webcam in android
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
        -> USB Gadget Support (USB_GADGET [=y])
          -> USB Gadget Drivers (<choice> [=y])
            -> Android Composite Gadget (USB_G_ANDROID [=y])
  Defined at drivers/usb/gadget/Kconfig:1078
  Depends on: <choice> && USB_G_ANDROID [=y]
```
 取消传统USB dwc2 驱动配置（dwc2 控制器驱动二选一）：
```

Symbol: USB_JZ_DWC2 [=n]
Type  : tristate
Prompt: DesignWare USB2 DRD Core Support
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
  Defined at drivers/usb/dwc2/Kconfig:1
  Depends on: USB_SUPPORT [=y]


```
S90camera 配置脚本：
```
#!/bin/sh
#
# Start camera....
#

case "$1" in
  start)
	echo "Starting camera..."
	if [ -d /sys/class/android_usb/android0 ]
	then
		cd /sys/class/android_usb/android0
		value=`cat ./functions`
		if [ "$value" != "webcam" ]
		then
			echo 0 > enable
			echo 18d1 > idVendor
			echo d002 > idProduct
			echo webcam > functions
			echo 1 > enable
		fi
		cd -
	else
		echo "notice : mass storage and adb don't use, kernel config error"
	fi
	;;
  stop)
	;;
  restart|reload)
	;;
  *)
	echo "Usage: $0 {start|stop|restart}"
	exit 1
esac

exit $?

```
开发板端取消默认自动配置usb adb 的功能：
```
# mv /etc/init.d/S90adb /etc/init.d/KS90adb
# reboot
```
执行配置脚本：
```
#  ./S90camera start
```
* 验证
Manhatton demo 路径：
packages/example/Sample/usb_test/webcamera

demo 编译：
```
$ cd packages/example/Sample/usb_test/webcamera
$ mma
目标文件：out/product/halley2/obj/DEPANNER/webcam_device_sample-intermediate/webcam_device_sample
```
` 君正设备执行`


	#  ./webcam_device_sample -u /dev/videoX -d -f 0 -n 2 -r 1 -s 1 -i yuv420_720p.yuv

 LINUXPC执行:
```
$ xawtv
```
 WINDOWPC执行:
 ```
 amcap
 ```

注意: 目前测试webcam程序只是反复传送yuv420_720p.yuv这张固定图片到pc端, 并不从真正的camera采集数据传送到pc端, 所以pc端无法得到真实的camera数据.

####6.  hid usb gadget
* 编译配置：USB_G_HID
```
Prompt: USB Gadget Drivers
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
        -> USB Gadget Support (USB_GADGET [=y])
  Defined at drivers/usb/gadget/Kconfig:543
  Depends on: USB_SUPPORT [=y] && USB_GADGET [=y]
  Selected by: USB_SUPPORT [=y] && USB_GADGET [=y] && m

然后选择:

│  Use the arrow keys to navigate this window or press the hotkey of │
│  the item you wish to select followed by the <SPACE BAR>. Press    │
│  <?> for additional information about this option.                 │
│ ┌─────────^(-)───────────────────────────────────────────────────┐ │
│ │     ( ) SLP Gadget based on Android                            │ │
│ │     ( ) Android Composite Gadget                               │ │
│ │     ( ) CDC Composite Device (Ethernet and ACM)                │ │
│ │     ( ) CDC Composite Device (ACM and mass storage)            │ │
│ │     ( ) Multifunction Composite Gadget                         │ │
│ │     (X) HID Gadget                                             │ │
│ └─────────┴(+)───────────────────────────────────────────────────┘ │
├────────────────────────────────────────────────────────────────────┤
│                       <Select>      < Help >                       │
```
* 验证

`君正设备执行:`
```
# echo xxx  > /dev/hidg0
```
`PC执行:`
```
  $  sudo cat /dev/hidrawX
```

注意: 遇到hidg0设备节点没有生成问题, 需要修改hid.c 中module_call(hidg_init)函数为late_initcall(hidg_init)
sudo dmesg -c 看pc端usb 插拔相关 log
修改后测试通过.

Manhatton 工程测试demo路径：
packages/example/Sample/usb_test/hid

* 编译
```
#cd packages/example/Sample/usb_test/hid
# mm

目标文件：out/product/halley2/system/testsuite/usb_test/kernel_device/usbhid_device_sample/usbhid_device_sample

```

* 验证
开发板端执行：

```
＃ ./usbhid_device_sample
```

####7.  printer usb gadget
* 编译配置： USB_G_PRINTER
```
Symbol: USB_G_PRINTER [=y]
Type  : tristate
Prompt: Printer Gadget
  Location:
    -> Device Drivers
      -> USB support (USB_SUPPORT [=y])
        -> USB Gadget Support (USB_GADGET [=y])
(1)       -> USB Gadget Drivers (<choice> [=y])
  Defined at drivers/usb/gadget/Kconfig:997
  Depends on: <choice>
  Selects: USB_LIBCOMPOSITE [=y]
```
* 验证

`君正设备执行:`
```
# echo xxx  > /dev/g_printer
```
`PC执行:`
```
$  cat /dev/usb/lpx
```
