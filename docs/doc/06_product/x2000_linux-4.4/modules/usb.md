# usb模块

# 一、模块简介

通用串行总线 \(Universal Serial Bus，USB\) 是一种新兴的并逐渐取代其他接口标准的数据通信方式，由 Intel、Compaq、Digital、IBM、Microsoft、NEC 及 Northern Telecom 等计算机公司和通信公司于 1995 年联合制定，并逐渐形成了行业标准。USB 总线作为一种高速串行总线，其极高的传输速度可以满足高速数据传输的应用环境要求，且该总线还兼有供电简单（可总线供电）、安装配置便捷（支持即插即用和热插拔）、 扩展端口简易（通过集线器最多可扩展 127 个外设）、传输方式多样化（4 种传输模式），以及兼容良好（产品升级后向下兼容）等优点。

# 二、内核空间

控制器代码结构:

```
drivers/usb/dwc2/
├── core.c
├── core.h
├── core_intr.c
├── debugfs.c
├── debug.h
├── gadget.c
├── hcd.c
├── hcd_ddma.c
├── hcd.h
├── hcd_intr.c
├── hcd_queue.c
├── hw.h
├── Kconfig
├── Makefile
├── pci.c
└── platform.c
```

## 1. 设备树配置

**１、arch/mips/boot/dts/ingenic/x2000-v12.dtsi**

```
 otg_phy: otg_phy {
                        compatible = "ingenic,innophy";
                };
```

```
otg: otg@0x13500000 {
                        compatible = "ingenic,dwc2-hsotg";
                        reg = <0x13500000 0x40000>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_OTG>;
                        ingenic,usbphy=<&otg_phy>;
                        status = "disabled";
                };
```

**２、arch/mips/boot/dts/ingenic/zebra.dts**

```
&otg {
        g-use-dma;
        dr_mode = "otg";
        status = "okay";
};

&otg_phy {
        dr_mode = "otg";
        compatible = "ingenic,innophy", "syscon";
        reg = <0x10000000 0x100>;
        ingenic,drvvbus-gpio = <&gpe 22 GPIO_ACTIVE_HIGH INGENIC_GPIO_NOBIAS>;
        status = "okay";
};
```

## 三、控制器OTG使用说明

### 3.1控制器作为host

#### 3.1.1　USB Mass Storage support

驱动配置：

```
(1)VFAT (Windows-95) fs support
Symbol: VFAT_FS [=y]                     
Type  : tristate                         
Prompt: VFAT (Windows-95) fs support     
  Location:                              
    -> File systems                      
      -> DOS/FAT/NT Filesystems          
  Defined at fs/fat/Kconfig:60           
  Depends on: BLOCK [=y]                 
  Selects: FAT_FS [=y]                   


(2)Codepage 437 (United States, Canada) 
Symbol: NLS_CODEPAGE_437 [=y]                             
Type  : tristate                                          
Prompt: Codepage 437 (United States, Canada)              
  Location:                                               
    -> File systems                                       
      -> Native language support (NLS [=y])               
  Defined at fs/nls/Kconfig:39                            
  Depends on: NLS [=y]                                    


(3)NLS ISO 8859-1  (Latin 1; Western European Languages) 
Symbol: NLS_ISO8859_1 [=y]                                        
Type  : tristate                                                  
Prompt: NLS ISO 8859-1  (Latin 1; Western European Languages)     
  Location:                                                       
    -> File systems                                               
      -> Native language support (NLS [=y])                       
  Defined at fs/nls/Kconfig:318                                   
  Depends on: NLS [=y]                                            

(4)SCSI disk support
Symbol: BLK_DEV_SD [=y]                        
Type  : tristate                               
Prompt: SCSI disk support                      
  Location:                                    
    -> Device Drivers                          
      -> SCSI device support                   
  Defined at drivers/scsi/Kconfig:73           
  Depends on: SCSI [=y]

(5)
Symbol: USB_STORAGE [=y]                                        
Type  : tristate                                                
Prompt: USB Mass Storage support                                
  Location:                                                     
    -> Device Drivers                                           
      -> USB support (USB_SUPPORT [=y])                         
        -> Support for Host-side USB (USB [=y])                 
  Defined at drivers/usb/storage/Kconfig:8                      
  Depends on: USB_SUPPORT [=y] && USB [=y] && SCSI [=y]
```

使用方法：

１、插入Ｕ盘弹出打印信息

```
[   46.070034] usb 1-1: new high-speed USB device number 2 using dwc2
[   46.296250] usb 1-1: New USB device found, idVendor=0951, idProduct=1666
[   46.303202] usb 1-1: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[   46.310592] usb 1-1: Product: DataTraveler 3.0
[   46.315185] usb 1-1: Manufacturer: Kingston
[   46.319508] usb 1-1: SerialNumber: 60A44C413C4EB211A98A00A0
[   46.325821] usb-storage 1-1:1.0: USB Mass Storage device detected
[   46.342363] scsi host0: usb-storage 1-1:1.0
[   47.726378] scsi 0:0:0:0: Direct-Access     Kingston DataTraveler 3.0 PMAP PQ: 0 ANSI: 6
[   47.736215] sd 0:0:0:0: [sda] 30277632 512-byte logical blocks: (15.5 GB/14.4 GiB)
[   47.752538] sd 0:0:0:0: [sda] Write Protect is off
[   47.759990] sd 0:0:0:0: [sda] No Caching mode page found
[   47.765648] sd 0:0:0:0: [sda] Assuming drive cache: write through
[   47.780215]  sda: sda1
[   47.789136] sd 0:0:0:0: [sda] Attached SCSI removable disk
```

２、挂载Ｕ盘到文件系统

mount -t vfat /dev/sda1 mnt/

３、数据交换进入到/mnt即可操作完成

#### 3.1.2　USB host camera

驱动配置：

```
Symbol: USB_VIDEO_CLASS [=y]                                                                                                   
Type  : tristate                                                                                                               
Prompt: USB Video Class (UVC)                                                                                                  
  Location:                                                                                                                    
    -> Device Drivers                                                                                                          
      -> Multimedia support (MEDIA_SUPPORT [=y])                                                                               
        -> Media USB Adapters (MEDIA_USB_SUPPORT [=y])                                                                         
  Defined at drivers/media/usb/uvc/Kconfig:1                                                                                   
  Depends on: USB [=y] && MEDIA_SUPPORT [=y] && MEDIA_USB_SUPPORT [=y] && MEDIA_CAMERA_SUPPORT [=y] && VIDEO_V4L2 [=y]         
  Selects: VIDEOBUF2_VMALLOC [=n]
```

使用方法：

１、插入摄像头识别成功

```
[   56.560037] usb 1-1: new high-speed USB device number 2 using dwc2
[   56.910300] usb 1-1: New USB device found, idVendor=058f, idProduct=5608
[   56.917231] usb 1-1: New USB device strings: Mfr=3, Product=1, SerialNumber=0
[   56.924635] usb 1-1: Product: USB 2.0 Camera
[   56.929114] usb 1-1: Manufacturer: Alcor Micro, Corp.
[   56.939481] uvcvideo: Found UVC 1.00 device USB 2.0 Camera (058f:5608)
[   56.949251] input: USB 2.0 Camera as /devices/platform/ahb2/13500000.otg/usb1/1-1/1-1:1.0/input/input0
```

２、拍照测试

```
# grab -H
usage: uvcview [-d <device>] [-c <count>]
--help -H               print this message 
--print_formats -F              print video device info
--device -d             , video device, default is /dev/video0
--width -w              grab width
--height -h             grab height
--count -c              set the count to grab
--rate -r               frame sample rate(fps)
--yuv -y                use yuyv input format
--timeout -t            select timeout
--match -m              do picture match test
--perf -p               do performance test

./grab -w 640 -h 480 -d /dev/video0 -y -c 3
./grab -w 320 -h 240 -d /dev/video0 -y -c 3
```

３、生成图像

```
p-0.jpg　p-１.jpg　p-２.jpg
```

#### 3.1.3　USB  host hid mouse

驱动配置：

```
(1)USB HIDBP Mouse (simple Boot) support
Symbol: USB_MOUSE [=y]                                                         
Type  : tristate                                                               
Prompt: USB HIDBP Mouse (simple Boot) support                                  
  Location:                                                                    
    -> Device Drivers                                                          
      -> HID support                                                           
        -> USB HID support                                                     
          -> USB HID Boot Protocol drivers                                     
  Defined at drivers/hid/usbhid/Kconfig:66                                     
  Depends on: USB_HID [=n]!=y && EXPERT [=y] && USB [=y] && INPUT [=y]         

(2)Mouse interface
Symbol: INPUT_MOUSEDEV [=y]                                                          
Type  : tristate                                                                     
Prompt: Mouse interface                                                              
  Location:                                                                          
    -> Device Drivers                                                                
      -> Input device support                                                        
        -> Generic input layer (needed for keyboard, mouse, ...) (INPUT [=y])        
  Defined at drivers/input/Kconfig:95                                                
  Depends on: !UML && INPUT [=y]
```

使用方法：

１、插入鼠标设备到控制器

```
[  117.100038] usb 1-1: new low-speed USB device number 2 using dwc2
[  117.313977] usb 1-1: New USB device found, idVendor=046d, idProduct=c077
[  117.320929] usb 1-1: New USB device strings: Mfr=1, Product=2, SerialNumber=0
[  117.328305] usb 1-1: Product: USB Optical Mouse
[  117.333098] usb 1-1: Manufacturer: Logitech
[  117.338568] input: Logitech USB Optical Mouse as /devices/platform/ahb2/13500000.otg/usb1/1-1/1-1:1.0/input/input0
```

２、执行捕获事件的程序后移动鼠标触发事件

```
# cd testsuite/
# ./getevent_test 0
/dev/input/mouse0
    evdev version: 0.0.0
    name: 
    features: relative reserved unknown unknown unknown unknown unknown unknown unknown
/dev/input/mouse0: open, fd = 3
Sun Mar  1 16:38:18 2020.000000 type 0x0011; code 0x000e; value 0x00000000; Led
[  125.626285] random: nonblocking pool is initialized
Wed Dec 25 06:26:16 2019.000000 type 0x0011; code 0x000e; value 0x00000000; Led
Thu Dec 26 00:59:52 2019.000000 type 0x0011; code 0x000e; value 0x00000000; Led
Mon Dec 23 18:27:20 2019.000000 type 0x0011; code 0x000e; value 0x00000000; Led
Thu Dec 26 00:55:36 2019.000000 type 0x0011; code 0x000e; value 0x00000000; Led
Wed Dec 25 06:47:36 2019.000000 type 0x0011; code 0x000e; value 0x00000000; Led
Tue Dec 24 12:31:04 2019.000000 type 0x0011; code 0x000e; value 0x00000000; Led
Wed Dec 25 06:47:36 2019.000000 type 0x0011; code 0x000e; value 0x00000000; Led
Mon Dec 23 18:10:16 2019.000000 type 0x0011; code 0x000e; value 0x00000000; Led
....
....
....
```

### 3.2控制器作为device

#### 3.2.1 device mass storage

驱动配置：

```
Symbol: USB_CONFIGFS_MASS_STORAGE [=y]                                             
Type  : boolean                                                                    
Prompt: Mass storage                                                               
  Location:                                                                        
    -> Device Drivers                                                              
      -> USB support (USB_SUPPORT [=y])                                            
        -> USB Gadget Support (USB_GADGET [=y])                                    
          -> USB Gadget Drivers (<choice> [=y])                                    
            -> USB functions configurable through configfs (USB_CONFIGFS [=y])     
  Defined at drivers/usb/gadget/Kconfig:338                                        
  Depends on: <choice> && USB_CONFIGFS [=y] && BLOCK [=y]                          
  Selects: USB_F_MASS_STORAGE [=y]
```

使用方法：

１、制作盘符

```
dd if=/dev/zero of=fat32.img bs=1k count=2048
格式为fat
mkfs.fat fat32.img
```

２、将盘符加入

```
echo fat32.img > /sys/kernel/config/usb_gadget/demo/functions/mass_storage.msg/lun.0/file
```

３、完成后会在PC端弹出盘符并且支持热插拔

#### 3.2.2 device adb

驱动配置：

```
Symbol: USB_CONFIGFS_F_FS [=y]                                                     
Type  : boolean                                                                    
Prompt: Function filesystem (FunctionFS)                                           
  Location:                                                                        
    -> Device Drivers                                                              
      -> USB support (USB_SUPPORT [=y])                                            
        -> USB Gadget Support (USB_GADGET [=y])                                    
          -> USB Gadget Drivers (<choice> [=y])                                    
            -> USB functions configurable through configfs (USB_CONFIGFS [=y])     
  Defined at drivers/usb/gadget/Kconfig:362                                        
  Depends on: <choice> && USB_CONFIGFS [=y]                                        
  Selects: USB_F_FS [=y]
```

使用方法：

１、启动后无需任何配置直接开启adb功能

２、adb不支持热插拔，插拔后会将设备清除，再次插入时候需要重新绑定

```
echo 13500000.otg > /sys/kernel/config/usb_gadget/demo/UDC
```

#### 3.2.3 device adb + mass storage

驱动配置：

```
 Symbol: USB_CONFIGFS_MASS_STORAGE [=y]                                            
 Type  : boolean                                                                   
 Prompt: Mass storage                                                              
   Location:                                                                       
     -> Device Drivers                                                             
       -> USB support (USB_SUPPORT [=y])                                           
         -> USB Gadget Support (USB_GADGET [=y])                                   
           -> USB Gadget Drivers (<choice> [=y])                                   
             -> USB functions configurable through configfs (USB_CONFIGFS [=y])    
   Defined at drivers/usb/gadget/Kconfig:338                                       
   Depends on: <choice> && USB_CONFIGFS [=y] && BLOCK [=y]                         
   Selects: USB_F_MASS_STORAGE [=y]                                                

Symbol: USB_CONFIGFS_F_FS [=y]                                                      
Type  : boolean                                                                     
Prompt: Function filesystem (FunctionFS)                                            
  Location:                                                                         
    -> Device Drivers                                                               
      -> USB support (USB_SUPPORT [=y])                                             
        -> USB Gadget Support (USB_GADGET [=y])                                     
          -> USB Gadget Drivers (<choice> [=y])                                     
            -> USB functions configurable through configfs (USB_CONFIGFS [=y])      
  Defined at drivers/usb/gadget/Kconfig:362                                         
  Depends on: <choice> && USB_CONFIGFS [=y]                                         
  Selects: USB_F_FS [=y]
```

使用方法：

１、系统启动后直接可以使用adb

２、mass storage

```
echo fat32.img > /sys/kernel/config/usb_gadget/demo/functions/mass_storage.msg/lun.0/file
```

注意：由于使用了adb在热插拔出后会清空，再次插入后仍需执行绑定

```
echo 13500000.otg > /sys/kernel/config/usb_gadget/demo/UDC
```

#### 3.2.4 device hid

驱动配置：

```
Symbol: USB_CONFIGFS_F_HID [=y]                                                   
Type  : boolean                                                                   
Prompt: HID function                                                              
  Location:                                                                       
    -> Device Drivers                                                             
      -> USB support (USB_SUPPORT [=y])                                           
        -> USB Gadget Support (USB_GADGET [=y])                                   
          -> USB Gadget Drivers (<choice> [=y])                                   
            -> USB functions configurable through configfs (USB_CONFIGFS [=y])    
  Defined at drivers/usb/gadget/Kconfig:419                                       
  Depends on: <choice> && USB_CONFIGFS [=y]                                       
  Selects: USB_F_HID [=n]
```

使用方法：

１、对设备进行参数配置

```
#!/bin/sh
#
# Start hid....
#
case "$1" in
  start)
    mount -t configfs none /sys/kernel/config
    cd /sys/kernel/config/usb_gadget
    mkdir -p demo/strings/0x409
    echo 0x18d1 > demo/idVendor
    echo 0xd002 > demo/idProduct
    echo 0x200 > demo/bcdUSB
    echo 0x100 > demo/bcdDevice
    echo "ingenic" > demo/strings/0x409/manufacturer
    echo "composite-demo" > demo/strings/0x409/product
    echo "0123456789ABCDEF" > demo/strings/0x409/serialnumber
    mkdir -p demo/configs/c.1/strings/0x409
    echo "hid" > demo/configs/c.1/strings/0x409/configuration
    echo 120 > demo/configs/c.1/MaxPower


    mkdir -p demo/functions/hid.0
    echo 1 > demo/functions/hid.0/subclass
    echo 1 > demo/functions/hid.0/protocol
    echo 8 > demo/functions/hid.0/report_length
    echo -ne \\x5\\x1\\x9\\x6\\xa1\\x1\\x5\\x7\\x19\\xe0\\x29\\xe7\\x15\\x0\\x25\\x01\\x75\\x01\\x95\\x08\\x81\\x02\\x95\\x01\\x75\\x08\\x81\\x03\\x95\\x05\\x75\\x01\\x05\\x08\\x19\\x01\\x29\\x05\\x91\\x02\\x95\\x01\\x75\\x03\\x91\\x03\\x95\\x06\\x75\\x08\\x15\\x00\\x25\\x65\\x05\\x07\\x19\\x00\\x29\\x65\\x81\\x00\\xc0 > demo/functions/hid.0/report_desc
    ln -s demo/functions/hid.0 demo/configs/c.1
    echo 13500000.otg > /sys/kernel/config/usb_gadget/demo/UDC
    ;;
  stop)
    echo none > /sys/kernel/config/usb_gadget/demo/UDC
    sleep 1
    rm /sys/kernel/config/usb_gadget/ -rf
    rm /sys/kernel/config/usb_gadget/ -rf
    umount /sys/kernel/config
    ;;
  restart|reload)
    ;;
  *)
    echo "Usage: $0 {start|stop|restart}"
    exit 1
esac

exit $?
```

２、pc端识别设备

```
[2180880.531376] usb 2-1.5: new high-speed USB device number 115 using ehci-pci
[2180880.628285] usb 2-1.5: New USB device found, idVendor=18d1, idProduct=d002
[2180880.628292] usb 2-1.5: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[2180880.628304] usb 2-1.5: Product: composite-demo
[2180880.628307] usb 2-1.5: Manufacturer: ingenic
[2180880.628309] usb 2-1.5: SerialNumber: 0123456789ABCDEF
[2180880.635882] input: ingenic composite-demo as /devices/pci0000:00/0000:00:1d.0/usb2/2-1/2-1.5/2-1.5:1.0/0003:18D1:D002.006C/input/input126
[2180880.691853] hid-generic 0003:18D1:D002.006C: input,hidraw3: USB HID v1.01 Keyboard [ingenic composite-demo] on usb-0000:00:1d.0-1.5/input0
```

３、执行动作测试程序

```
cd testsuite/usb_test/kernel_device/usbhid_device_sample/
./usbhid_device_sample 0
```

４、测试部分结果

```
usb hid str is ABCabc AaBbCc cAbCBa CaBbAc
/;'[]<>?:"{}
uVvWwXxYyZz
, size = 28
process  A 
A is upper
process  B 
B is upper
process  C 
C is upper
process  a 
...
...
b is lower
process  A 
A is upper
process  c 
c is lower
process  

key is \n
```

#### 3.2.5 device gadget uvc

驱动配置

```
(1)USB Video Class (UVC)
Symbol: USB_VIDEO_CLASS [=y]                                                                                            
Type  : tristate                                                                                                        
Prompt: USB Video Class (UVC)                                                                                           
  Location:                                                                                                             
    -> Device Drivers                                                                                                   
      -> Multimedia support (MEDIA_SUPPORT [=y])                                                                        
        -> Media USB Adapters (MEDIA_USB_SUPPORT [=y])                                                                  
  Defined at drivers/media/usb/uvc/Kconfig:1                                                                            
  Depends on: USB [=y] && MEDIA_SUPPORT [=y] && MEDIA_USB_SUPPORT [=y] && MEDIA_CAMERA_SUPPORT [=y] && VIDEO_V4L2 [=y]  
  Selects: VIDEOBUF2_VMALLOC [=n]                                                                                       

 (2)USB Webcam function
Symbol: USB_CONFIGFS_F_UVC [=y]                                                    
Type  : boolean                                                                    
Prompt: USB Webcam function                                                        
  Location:                                                                        
    -> Device Drivers                                                              
      -> USB support (USB_SUPPORT [=y])                                            
        -> USB Gadget Support (USB_GADGET [=y])                                    
          -> USB Gadget Drivers (<choice> [=y])                                    
            -> USB functions configurable through configfs (USB_CONFIGFS [=y])     
  Defined at drivers/usb/gadget/Kconfig:429                                        
  Depends on: <choice> && USB_CONFIGFS [=y] && VIDEO_DEV [=y]                      
  Selects: VIDEOBUF2_VMALLOC [=y] && USB_F_UVC [=n]      

  (3)Enable usb dwc2 highwidth fifo
  Symbol: USB_DWC2_HIGHWIDTH_FIFO [=y]                             
Type  : boolean                                                  
Prompt: Enable usb dwc2 highwidth fifo                           
  Location:                                                      
    -> Device Drivers                                            
      -> USB support (USB_SUPPORT [=y])                          
        -> DesignWare USB2 DRD Core Support (USB_DWC2 [=y])      
  Defined at drivers/usb/dwc2/Kconfig:64                         
  Depends on: USB_SUPPORT [=y] && USB_DWC2 [=y]
```

测试方法：

１、配置相关的信息

```
#!/bin/sh
#
# Start uvc....
#
case "$1" in
  start)
        mount -t configfs none /sys/kernel/config
        cd /sys/kernel/config/usb_gadget
        mkdir -p demo/strings/0x409
        echo 0x18d1 > demo/idVendor
        echo 0xd002 > demo/idProduct
        echo 0x200 > demo/bcdUSB
        echo 0x100 > demo/bcdDevice
        echo "ingenic" > demo/strings/0x409/manufacturer
        echo "composite-demo" > demo/strings/0x409/product
        echo "0123456789ABCDEF" > demo/strings/0x409/serialnumber
        mkdir -p demo/configs/c.1/strings/0x409
        echo "uvc" > demo/configs/c.1/strings/0x409/configuration
        echo 120 > demo/configs/c.1/MaxPower


        mkdir -p demo/functions/uvc.
        echo  3072  >  demo/functions/uvc./streaming_maxpacket
        mkdir demo/functions/uvc./control/header/jz_control
        echo 256 >  demo/functions/uvc./control/header/jz_control/bcdUVC
        echo 48000000 >  demo/functions/uvc./control/header/jz_control/dwClockFrequency
        ln -s  demo/functions/uvc./control/header/jz_control   demo/functions/uvc./control/class/fs/
        ln -s  demo/functions/uvc./control/header/jz_control   demo/functions/uvc./control/class/ss/


        mkdir -p demo/functions/uvc./streaming/header/jz_streaming
        mkdir -p demo/functions/uvc./streaming/uncompressed/yuv
        mkdir -p demo/functions/uvc./streaming/uncompressed/yuv/360p

        echo 640       > demo/functions/uvc./streaming/uncompressed/yuv/360p/wWidth
        echo 360       > demo/functions/uvc./streaming/uncompressed/yuv/360p/wHeight
        echo 18432000  > demo/functions/uvc./streaming/uncompressed/yuv/360p/dwMinBitRate
        echo 55296000  > demo/functions/uvc./streaming/uncompressed/yuv/360p/dwMaxBitRate

        echo 460800  > demo/functions/uvc./streaming/uncompressed/yuv/360p/dwMaxVideoFrameBufferSize
        echo 666666  > demo/functions/uvc./streaming/uncompressed/yuv/360p/dwDefaultFrameInterval
        echo 3       > demo/functions/uvc./streaming/uncompressed/yuv/360p/dwFrameInterval
        echo 0       > demo/functions/uvc./streaming/uncompressed/yuv/360p/bmCapabilities

        ln -s demo/functions/uvc./streaming/uncompressed/yuv demo/functions/uvc./streaming/header/jz_streaming
        ln -s demo/functions/uvc./streaming/header/jz_streaming  demo/functions/uvc./streaming/class/fs/
        ln -s demo/functions/uvc./streaming/header/jz_streaming  demo/functions/uvc./streaming/class/hs/

        ln -s demo/functions/uvc. demo/configs/c.1
        echo 13500000.otg > /sys/kernel/config/usb_gadget/demo/UDC
        ;;
 stop)
        echo none > /sys/kernel/config/usb_gadget/demo/UDC
        rm /sys/kernel/config/usb_gadget/ -rf
        rm /sys/kernel/config/usb_gadget/ -rf
        umount /sys/kernel/config
        ;;
  restart|reload)
        ;;
　*)
        echo "Usage: $0 {start|stop|restart}"             
        exit 1
easc
exit $?
```

２、pc端可以看到设备信息

```
[2187560.274157] usb 2-1.5: new high-speed USB device number 19 using ehci-pci
[2187560.371029] usb 2-1.5: New USB device found, idVendor=18d1, idProduct=d002
[2187560.371033] usb 2-1.5: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[2187560.371035] usb 2-1.5: Product: composite-demo
[2187560.371037] usb 2-1.5: Manufacturer: ingenic
[2187560.371039] usb 2-1.5: SerialNumber: 0123456789ABCDEF
[2187560.394857] uvcvideo: Found UVC 1.00 device composite-demo (18d1:d002)
[2187560.403005] input: composite-demo as /devices/pci0000:00/0000:00:1d.0/usb2/2-1/2-1.5/2-1.5:1.0/input/input141
```

３、回到开发板进行测试

```
cd testsuite/
./webcam_gadget -u /dev/video0 -d -f 0 -n 2 -r 1 -s 1 -i ttt_1280x720.yuv
```

４、pc端接收

```
linux :xawtv
window :amcap
```

#### 3.2.6 device remote NDIS

驱动配置：

```
Symbol: USB_CONFIGFS_RNDIS [=y]                                                        
Type  : boolean                                                                        
Prompt: RNDIS                                                                          
  Location:                                                                            
    -> Device Drivers                                                                  
      -> USB support (USB_SUPPORT [=y])                                                
        -> USB Gadget Support (USB_GADGET [=y])                                        
          -> USB Gadget Drivers (<choice> [=y])                                        
            -> USB functions configurable through configfs (USB_CONFIGFS [=y])         
  Defined at drivers/usb/gadget/Kconfig:297                                            
  Depends on: <choice> && USB_CONFIGFS [=y] && NET [=y]                                
  Selects: USB_U_ETHER [=n] && USB_F_RNDIS [=n]
```

使用方法：

１、配置信息

```
#!/bin/sh
#
# Start rndis....
#
case "$1" in
  start)
        mount -t configfs none /sys/kernel/config
        cd /sys/kernel/config/usb_gadget
        mkdir -p demo/strings/0x409
        echo 0x18d1 > demo/idVendor
        echo 0xd002 > demo/idProduct
        echo 0x200 > demo/bcdUSB
        echo 0x100 > demo/bcdDevice
        echo "ingenic" > demo/strings/0x409/manufacturer
        echo "composite-demo" > demo/strings/0x409/product
        echo "0123456789ABCDEF" > demo/strings/0x409/serialnumber
        mkdir -p demo/configs/c.1/strings/0x409
        echo "uvc" > demo/configs/c.1/strings/0x409/configuration
        echo 120 > demo/configs/c.1/MaxPower


        mkdir -p demo/functions/rndis.
        ln -s demo/functions/rndis. demo/configs/c.1
        echo 13500000.otg > /sys/kernel/config/usb_gadget/demo/UDC
        ;;
 stop)
        echo none > /sys/kernel/config/usb_gadget/demo/UDC
        rm /sys/kernel/config/usb_gadget/ -rf
        rm /sys/kernel/config/usb_gadget/ -rf
        umount /sys/kernel/config
        ;;
esac
exit $?
```

２、pc端设备信息

```
识别信息：
[2188325.480982] usb 2-1.5: new high-speed USB device number 21 using ehci-pci
[2188325.577912] usb 2-1.5: New USB device found, idVendor=18d1, idProduct=d002
[2188325.577919] usb 2-1.5: New USB device strings: Mfr=1, Product=2, SerialNumber=3
[2188325.577930] usb 2-1.5: Product: composite-demo
[2188325.577932] usb 2-1.5: Manufacturer: ingenic
[2188325.577934] usb 2-1.5: SerialNumber: 0123456789ABCDEF
[2188325.586180] rndis_host 2-1.5:1.0 usb0: register 'rndis_host' at usb-0000:00:1d.0-1.5, RNDIS device, 52:ea:19:19:5f:69
[2188325.650205] rndis_host 2-1.5:1.0 enp0s29u1u5: renamed from usb0
[2188325.670351] IPv6: ADDRCONF(NETDEV_UP): enp0s29u1u5: link is not ready

查看网卡：
＃ifconfig -a
enp0s29u1u5 Link encap:Ethernet  HWaddr 52:ea:19:19:5f:69  
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)

＃sudo ifconfig enp0s29u1u5 192.168.4.250 up
enp0s29u1u5 Link encap:Ethernet  HWaddr 52:ea:19:19:5f:69  
          inet addr:192.168.4.250  Bcast:192.168.4.255  Mask:255.255.255.0
          inet6 addr: fe80::50ea:19ff:fe19:5f69/64 Scope:Link
          UP BROADCAST RUNNING MULTICAST  MTU:1500  Metric:1
          RX packets:0 errors:0 dropped:0 overruns:0 frame:0
          TX packets:0 errors:0 dropped:0 overruns:0 carrier:0
          collisions:0 txqueuelen:1000 
          RX bytes:0 (0.0 B)  TX bytes:0 (0.0 B)
```

３、测试结果

```
user@user-HP-Compaq-8200:~$ ping 192.168.4.250
PING 192.168.4.250 (192.168.4.250) 56(84) bytes of data.
64 bytes from 192.168.4.250: icmp_seq=1 ttl=64 time=0.034 ms
64 bytes from 192.168.4.250: icmp_seq=2 ttl=64 time=0.034 ms
64 bytes from 192.168.4.250: icmp_seq=3 ttl=64 time=0.031 ms
```

#### 3.2.7 device Serial Gadget

驱动配置：

```
Symbol: USB_G_SERIAL [=y]                                                                                            
Type  : tristate                                                                                                     
Prompt: Serial Gadget (with CDC ACM and CDC OBEX support)                                                            
  Location:                                                                                                          
    -> Device Drivers                                                                                                
      -> USB support (USB_SUPPORT [=y])                                                                              
        -> USB Gadget Support (USB_GADGET [=y])                                                                      
          -> USB Gadget Drivers (<choice> [=y])                                                                      
  Defined at drivers/usb/gadget/legacy/Kconfig:260                                                                   
  Depends on: <choice> && TTY [=y]                                                                                   
  Selects: USB_U_SERIAL [=y] && USB_F_ACM [=y] && USB_F_SERIAL [=y] && USB_F_OBEX [=y] && USB_LIBCOMPOSITE [=y]
```

使用方法：

１、pc端

```
设备信息：
[2189021.818872] usb 2-1.5: new high-speed USB device number 25 using ehci-pci
[2189021.915645] usb 2-1.5: New USB device found, idVendor=0525, idProduct=a4a7
[2189021.915651] usb 2-1.5: New USB device strings: Mfr=1, Product=2, SerialNumber=0
[2189021.915655] usb 2-1.5: Product: Gadget Serial v2.4
[2189021.915659] usb 2-1.5: Manufacturer: Linux 4.4.94+ with 13500000.otg
[2189021.922892] cdc_acm 2-1.5:2.0: ttyACM0: USB ACM device
```

２、互发信息

```
user@user-HP-Compaq-8200:~$ echo 1111111111 > /dev/ttyACM0 
# cat ttyGS0 
1111111111

# echo 2222 > ttyGS0 
user@user-HP-Compaq-8200:~$ cat /dev/ttyACM0 
2222
```



