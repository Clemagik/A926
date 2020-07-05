# Camera驱动配置

x1830 camera摄像头控制器为isp，x1830平台使用的接口为dvp和mipi，内核的sensor驱动是基于isp驱动框架编写的，应用程序可以使用isp\_demo进行测试。

## 内核编译配置

```
--- Multimedia support
        *** Multimedia core support ***        
  [*]   Cameras/video grabbers support
  [ ]   Analog TV support
  [ ]   Digital TV support
  [ ]   Remote Controller support
  [*]   Media Controller API
  [*]   V4L2 sub-device userspace API
  [ ]   Enable advanced debug functionality on V4L2 drivers
  [ ]   Enable old-style fixed minor ranges on drivers/video device
  <*>   V4L2 int device (DEPRECATED)
        *** Media drivers ***
  [*]   Media USB Adapters           /*USB camera*/
        --- Media USB Adapters
              *** Webcam devices ***
         <*>   USB Video Class (UVC)
         [*]     UVC input events device support
  [*]   V4L platform devices
         --- V4L platform devices
         [*]   TX ISP MODULE
         [*]     TX ISP LIBISP
         [*]     MCLK from CPU
  [ ]   Memory-to-memory multimedia devices  --->
  [ ]   Media test drivers  --->
         *** Supported MMC/SDIO adapters *** 
  < >   Cypress firmware helper routines
        *** Media ancillary drivers (tuners, sensors, i2c, frontends) ***
  [ ]   Autoselect ancillary drivers (tuners, sensors, i2c, frontends)
        Encoders, decoders, sensors and other helper chips  --->
        Sensors used on soc_camera driver  --->
        Sensors used on tx-isp_camera driver  ---> 
            *** tx-isp camera sensor drivers ***
            < > gc0328 camera support 
            < > sc2235 camera support
            <*> OV9712 camera support
            < > gc2375a camera support
            < > ov9732 camera support
        Customise DVB Frontends  --->
```

## 编译配置（Buildroot）

```
BR2_PACKAGE_ISP

isp demo

Symbol: BR2_PACKAGE_ISP_DEMO [=y]
Type  : boolean       
Prompt: isp demo
   Location:                                                                          
      -> ingenic packages (BR2_PACKAGE_INGENIC [=y])
   Defined at package/ingenic/isp_demo/Config.in:1
   Depends on: BR2_PACKAGE_INGENIC [=y]
   Selects: BR2_PACKAGE_LIBISP [=y] 

BR2_PACKAGE_LIBISP: 

libisp 
  Symbol: BR2_PACKAGE_LIBISP [=y]
  Type  : boolean
  Prompt: libisp
    Location:
       -> ingenic packages (BR2_PACKAGE_INGENIC [=y])
    Defined at package/ingenic/libisp/Config.in:1
    Depends on: BR2_PACKAGE_INGENIC [=y]
    Selected by: BR2_PACKAGE_ISP_DEMO [=y] && BR2_PACKAGE_INGENIC [=y]
```

## 测试应用

在x1830的camera测试应用是isp\_demo\(路径：/usr/bin/isp\_demo\),针对不同的sensor需要手动添加对应的宏到isp\_demo.c

![](/assets/isp_demo.png)

##### 开发板端执行 isp\_demo:

```
# isp_demo 
[ 3090.412447] set sensor gpio as PA-low-10bit
[ 3090.423071] ov9712 chip found @ 0x30 (i2c0)
1970-01-01 08:51:30.424856:I/IMP - isp_cim 430: SENSOR INPUT: [0] ov9712
1970-01-01 08:51:30.458456:I/IMP - isp_cim 298: Image width: 1280 height: 800 size: 1536000
1970-01-01 08:51:30.458844:I/IMP - rmem 100: CMD Line Rmem Size:34603008, Addr:0x05f00000
```

##### LCD屏显示sensor的拍摄的实时画面

![](/assets/camera.png)

