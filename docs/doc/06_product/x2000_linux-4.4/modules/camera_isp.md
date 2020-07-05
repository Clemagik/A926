# camera isp模块

# 一、模块简介

isp(Image Signal Processing)模块的主要作用是对前端传感器输出的信号做后期处理。主要功能有白平衡、自动曝光控制、自动对焦、自动增益等。支持mipi、dvp接口raw数据输入。NV12/NV21格式输出。

isp模块包含csi、vic、isp-core、mscaler四个子模块，其驱动框架如下图所示。

![](/assets/isp-drv.png)

# 二、内核空间

> X2000 isp 驱动目录：
>
> ```
> drivers/media/platform/ingenic-isp
>├── csi.c
>├── csi-regs.h
>├── isp.c
>├── isp-core
>│   ├── inc
>│   │   ├── system_sensor_drv.h
>│   │   ├── tiziano_core.h
>│   │   ├── tiziano_core_tuning.h
>│   │   ├── tiziano_isp.h
>│   │   └── tiziano_sys.h
>│   ├── Makefile
>│   └── src
>│       ├── clm_lut.h
>│       ├── fix_point_calc.c
>│       ├── fix_point_calc.h
>│       ├── isp-debug.h
>│       ├── jz_isp.h
>│       ├── jz_load_parameter.c
>│       ├── shapren-regs.h
>│       ├── tisp_adr.a_shipped
>│       ├── tisp_awb.a_shipped
>│       ├── tisp_ccm.a_shipped
>│       ├── tisp_defog.a_shipped
>│       ├── tiziano_adr.c
>│       ├── tiziano_adr.h
>│       ├── tiziano_ae.c
>│       ├── tiziano_ae.h
>│       ├── tiziano_af.c
>│       ├── tiziano_af.h
>│       ├── tiziano_all_params_info.c
>│       ├── tiziano_awb.c
>│       ├── tiziano_awb.h
>│       ├── tiziano_ccm.c
>│       ├── tiziano_ccm.h
>│       ├── tiziano_clm.c
>│       ├── tiziano_clm.h
>│       ├── tiziano_core.c
>│       ├── tiziano_core.h
>│       ├── tiziano_core_tuning.c
>│       ├── tiziano_defog.c
>│       ├── tiziano_defog.h
>│       ├── tiziano_dmsc.c
>│       ├── tiziano_dmsc.h
>│       ├── tiziano_dpc.c
>│       ├── tiziano_dpc.h
>│       ├── tiziano_event.c
>│       ├── tiziano_event.h
>│       ├── tiziano_gamma.c
>│       ├── tiziano_gamma.h
>│       ├── tiziano_gib.c
>│       ├── tiziano_gib.h
>│       ├── tiziano_hldc.c
>│       ├── tiziano_hldc.h
>│       ├── tiziano_lsc.c
>│       ├── tiziano_lsc.h
>│       ├── tiziano_map.h
>│       ├── tiziano_mdns.c
>│       ├── tiziano_mdns.h
>│       ├── tiziano_netlink.c
>│       ├── tiziano_netlink.h
>│       ├── tiziano_params.h
>│       ├── tiziano_params_operate.c
>│       ├── tiziano_params_operate.h
>│       ├── tiziano_sdns.c
>│       ├── tiziano_sdns.h
>│       ├── tiziano_sharpen.c
>│       ├── tiziano_sharpen.h
>│       ├── tx-isp-init-params.c
>│       ├── tx-isp-init-params-day.c
>│       ├── tx-isp-init-params-night.c
>│       ├── tzctrl
>│       └── tzctrl.c
>├── isp-drv.c
>├── isp-drv.h
>├── isp-regs.h
>├── isp-video.c
>├── isp-video-mplane.c
>├── Makefile
>├── mscaler.c
>├── mscaler-regs.h
>├── sensor.c
>├── vic.c
>└── vic-regs.h
>```


## 1. 设备树配置

**在arch/mips/boot/dts/ingenic/x2000-v12.dtsi ahb0下配置：**

    ispcam0:
        isp-camera@0 {
        compatible = "ingenic,isp-camera";
        #address-cells = <1>;
        #size-cells = <1>;
        ranges = <>;

        port {
            isp0_ep:endpoint@0 {

            };
        };


        csi0: csi@0x10074000 {
            compatible = "ingenic,x2000-csi";
            reg = <0x10074000 0x1000>;
            interrupt-parent = <&core_intc>;
            interrupts = <IRQ_MIPI_CSI_4>;  // 4lane csi
            clocks = <&clock CLK_GATE_MIPI_CSI>;
            clock-names = "gate_csi";
        };

        vic0: vic@0x13710000 {
            compatible = "ingenic,x2000-vic";
            reg = <0x13710000 0x10000>;
            interrupt-parent = <&core_intc>;
            interrupts = <IRQ_VIC0>;
            status = "ok";
        };
        isp0: isp@0x13700000 {
            compatible = "ingenic,x2000-isp";
            reg = <0x13700000 0x2300>;
            interrupt-parent = <&core_intc>;
            interrupts = <IRQ_ISP0>;
            clocks = <&clock CLK_DIV_ISP>, <&clock CLK_GATE_ISP0>;
            clock-names = "div_isp", "gate_isp0";
            status = "ok;
            ingenic,cpm_reset = <0xb00000c4>;
            ingenic,bit_sr = <25>;
            ingenic,bit_stp = <24>;
            ingenic,bit_ack = <23>;
        };
        mscaler0: mscaler@0x13702300 {
            compatible = "ingenic,x2000-mscaler";
            reg = <0x13702300 0x400>;
            status = "ok";
        };
    };

    ispcam1: isp-camera@1 {
        compatible = "ingenic,isp-camera";
        #address-cells = <1>;
        #size-cells = <1>;
        ranges = <>;

        port {
            isp1_ep:endpoint@0 {

            };
        };

        csi1: csi@0x10073000 {
            compatible = "ingenic,x2000-csi";
            reg = <0x10073000 0x1000>;
            interrupt-parent = <&core_intc>;
            interrupts = <IRQ_MIPI_CSI2>;   // 2lane csi
            clocks = <&clock CLK_GATE_MIPI_CSI>;
            clock-names = "gate_csi";
        };

        vic1: vic@0x13810000 {
            compatible = "ingenic,x2000-vic";
            reg = <0x13810000 0x10000>;
            interrupt-parent = <&core_intc>;
            interrupts = <IRQ_VIC1>;
            status = "ok";
        };

        isp1: isp@0x13800000 {
            compatible = "ingenic,x2000-isp";
            reg = <0x13800000 0x2300>;
            interrupt-parent = <&core_intc>;
            interrupts = <IRQ_ISP1>;
            clocks = <&clock CLK_DIV_ISP>, <&clock CLK_GATE_ISP1>;
            clock-names = "div_isp", "gate_isp1";
            status = "ok";
            ingenic,cpm_reset = <0xb00000c4>;
            ingenic,bit_sr = <22>;
            ingenic,bit_stp = <21>;
            ingenic,bit_ack = <20>;
        };

        mscaler1: mscaler@0x13802300 {
            compatible = "ingenic,x2000-mscaler";
            reg = <0x13802300 0x400>;
            status = "ok";
        };
    };

**在arch/mips/boot/dts/ingenic/板级.dts下配置：**

    &i2c1 {
        status = "okay";
        clock-frequency = <100000>;
        timeout = <1000>;
        pinctrl-names = "default";
        pinctrl-0 = <&i2c1_pc>;

        /*RD_X2000_HALLEY5_CAMERA_V2.0*/
        ov4689_0:ov4689@0x21 {
                status = "ok";
                compatible = "ovti,ov4689";
                reg = <0x21>;
                pinctrl-names = "default";
                pinctrl-0 = <&cim_vic_mclk_pc>;

                port {
                        ov4689_ep0:endpoint {
                                remote-endpoint = <&isp0_ep>;
                        };
                };
        };
    };

    &i2c3 {
        status = "okay";
        clock-frequency = <100000>;
        timeout = <1000>;
        pinctrl-names = "default";
        pinctrl-0 = <&i2c3_pa>;

        /*RD_X2000_HALLEY5_CAMERA_V2.0*/
        ov4689_1:ov4689@0x21 {
                status = "ok";
                compatible = "ovti,ov4689";
                reg = <0x21>;
                pinctrl-names = "default";
                pinctrl-0 = <&cim_vic_mclk_pe_24M>;

                ingenic,rst-gpio = <&gpb 4 GPIO_ACTIVE_LOW INGENIC_GPIO_NOBIAS>;

                port {
                        ov4689_ep1:endpoint {
                                remote-endpoint = <&isp1_ep>;
                        };
                };
        };
    };


    &isp0_ep {
            remote-endpoint = <&ov4689_ep0>;
            data-lanes = <2>;
            clk-lanes = <1>;
    };

    &isp1_ep {
            remote-endpoint = <&ov4689_ep1>;
            data-lanes = <2>;
            clk-lanes = <1>;
    };

* 需根据camera板以及所连接的sensor进行配置，上例为RD_X2000_HALLEY5_CAMERA_V2.0 链接ov4689 sensor

## 2. 驱动配置

**make menuconfig， 然后配置：**

通用配置:

    Symbol: VIDEO_INGENIC_ISP [=y]
    Type  : tristate
    Prompt: V4L2 Driver for ingenic isp
        Location:
            -> Device Drivers
                -> Multimedia support (MEDIA_SUPPORT [=y])
                     -> V4L platform devices (V4L_PLATFORM_DRIVERS [=y])Kconfig:117
        Depends on: MEDIA_SUPPORT [=y] && V4L_PLATFORM_DRIVERS [=y] && VIDEO_DEV [=y] && VIDEO_V4L2 [=y]
        Selects: VIDEOBUF2_DMA_CONTIG [=y]

        Symbol: VIDEO_V4L2_SUBDEV_API [=y]
        Type  : boolean
        Prompt: V4L2 sub-device userspace API
        Location:
            -> Device Drivers
                -> Multimedia support (MEDIA_SUPPORT [=y])
        Defined at drivers/media/Kconfig:117
        Depends on: MEDIA_SUPPORT [=y] && VIDEO_DEV [=y] && MEDIA_CONTROLLER [=y]


扩展配置，根据不同的sensor选择驱动

    Symbol: INGENIC_ISP_CAMERA_OV4689 [=y]
    Type  : tristate
    Prompt: ov4689 camera support
        Location:
            -> Device Drivers
                -> Multimedia support (MEDIA_SUPPORT [=y])
                    -> Sensors used on ingenic-isp camera driver
    Defined at drivers/media/i2c/ingenic-isp/Kconfig:4
    Depends on: MEDIA_SUPPORT [=y] && VIDEO_V4L2 [=y] && VIDEO_INGENIC_ISP [=y] && I2C [=y]


# 三、用户空间

## 1. 设备节点

驱动加载成功，生成的相关设备节点
>/dev/video0 ISP0 channel0设备节点

>/dev/video1 ISP1 channel0设备节点

>/sys/devices/platform/ahb0/ahb0:isp-camera@0/10074000.csi csi0文件节点

>/sys/devices/platform/ahb0/ahb0:isp-camera@0/13710000.vic vic0文件节点

>/sys/devices/platform/ahb0/ahb0:isp-camera@0/13700000.isp isp0文件节点

>/sys/devices/platform/ahb0/ahb0:isp-camera@0/13702300.mscaler mscaler0 文件节点

>/sys/devices/platform/ahb0/ahb0:isp-camera@1/10073000.csi csi1文件节点

>/sys/devices/platform/ahb0/ahb0:isp-camera@1/13810000.vic vic1文件节点

>/sys/devices/platform/ahb0/ahb0:isp-camera@1/13800000.isp isp1文件节点

>/sys/devices/platform/ahb0/ahb0:isp-camera@1/13802300.mscaler mscaler1 文件节点

* 若注册其他video_device,/dev下设备节点编号可能发生变化;

## 2. 应用程序

测试应用为v4l2-ctl 源码位置:buildroot/output/build/libv4l-1.12.2/utils/v4l2-ctl

## 3. 测试方法

### 1.v4l2-ctl
v4l2-ctl -v width=640,height=480,pixelformat="NV12" --stream-mmap=3 --stream-to="test-ch0.yuv" -d /dev/
video0

测试isp通路并保存图像数据到文件，其中，-d为相应的mscaler通道对应的节点 --stream-to=图像文件名

### 2.ffmpeg
ffmpeg -pix_fmt nv12 -s 640*480 -i /dev/video0 -f fbdev /dev/fb0

配合LCD屏测试isp通路并预览，其中， -i为相应的mscaler通道对应的节点 fbdev为显示屏对应的节点



