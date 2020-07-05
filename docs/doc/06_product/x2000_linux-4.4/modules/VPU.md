# vpu模块

## 1.VPU控制器简介
x2000将vpu分为felix和helix两部分：
1. felix是h264解码,包括:流解析器、运动补偿、反量化、IDCT和De-block engines的功能。
2. helix是h264编码、JPEG压缩和解压缩。

## 2.内核空间

## 2.1.驱动相关的源码：

```
drivers/media/platform/ingenic-vcodec
├── felix
│   ├── felix_drv.c
│   ├── felix_drv.h
│   ├── felix_ops.c
│   ├── felix_ops.h
│   ├── libh264
├── helix
│   ├── api
│   ├── default_sliceinfo.c
│   ├── h264e.c
│   ├── h264e.h
│   ├── h264enc
│   ├── helix_buf.h
│   ├── helix_drv.c
│   ├── helix_drv.h
│   ├── helix_ops.c
│   ├── helix_ops.h
│   ├── jpgd.c
│   ├── jpgd.h
│   ├── jpge
│   ├── jpge.c
│   ├── jpge.h
│   ├── Makefile
│   └── README
```

## 2.2 设备树配置

```
&felix {
        status = "okay";
};

&helix {
        status = "okay";
};
```

### 2.3 驱动配置

```
--- Multimedia support
    *** Multimedia core support ***
[*]   Cameras/video grabbers support
[*]   Memory-to-memory multimedia devices  --->
    --- Memory-to-memory multimedia devices
    <*>   V4L2 driver for ingenic Video Codec /*选择vpu驱动*/
    < >   SuperH VEU mem2mem video processing driver
[ ]   Autoselect ancillary drivers (tuners, sensors, i2c, frontends)
```

## 3.用户空间

## 3.1.驱动加载成功
```c
helix-venc 13200000.helix: encoder(helix) registered as /dev/video1 /*helix加载成功*/
felix-vdec 13300000.felix: h264decoder(felix) registered as /dev/video2 /*felix加载成功*/
```

## 3.2.设备节点

```c
/dev/video1    /*helix的设备节点*/
/dev/video2    /*felix的设备节点*/
```

## 3.3.测试方法

1. 测试h264解码

* 源码路径
```c
packages/example/App/v4l2-h264dec
```

* 测试方法
```c
v4l2_h264dec -t nv12/nv21/tile420 -v /dev/video2 -f video.mp4.dump.h264 -w 1280 -h 720

注意：
    -v 指定felix的video节点
    -f 指定要解码的码流
    -w 指定图片width
    -h 指定图片height
```
* 标准：h264解码后lcd显示的图像完整无数据丢失

2. 测试h264编码

* 源码路径
```c
packages/example/App/v4l2-h264enc
```

* 测试方法
```c
v4l2_h264enc -t nv12 -v /dev/video1 -f video-1280x720_nv12.yuv -w 1280 -h 720

注意：
    -v 指定felix的video节点
    -f 指定要编码的文件
    -w 指定图片width
    -h 指定图片height
```
* 标准：执行完成后查看生成的output.h264文件是否正常

3. 测试jpeg解码

* 源码路径
```c
packages/example/App/v4l2-jpegdec
```

* 测试方法
```c
v4l2_jpegdec -v /dev/video1 -f test.jpg
```
* 标准：执行完成后查看生成output.raw文件是否正常

4. 测试jpeg编码

* 源码路径
```c
packages/example/App/v4l2-jpegenc
```

* 测试方法
```c
v4l2_jpegenc -v /dev/video1 -f video-1280x720_nv12.yuv
```
* 标准：执行完成后查看生成的output.jpg文件是否正常