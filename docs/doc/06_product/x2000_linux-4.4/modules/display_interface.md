# 一、模块简介

略

# 二、内核空间

驱动代码位置：

```
drivers/video/fbdev/ingenic/x2000_v12
├── displays                    /*添加屏设备目录*/
│   ├── Kconfig
│   ├── Makefile
│   └── panel-y88249.c
├── dpu_reg.h
├── ingenicfb.c
├── ingenicfb.h
 ```

## 1. 设备树配置

#### 在arch/mips/boot/dts/ingenic/板级.dts下配置：   

```
&dpu {
	status = "okay";
	port {
		dpu_out_ep: endpoint {
			remote-endpoint = <&panel_y88249_ep>;
			/*remote-endpoint = <&panel_kd050hdfia019_ep>;*/
		};
	};
};
display-dbi {
        compatible = "simple-bus";
        #interrupt-cells = <1>;
        #address-cells = <1>;
        #size-cells = <1>;
        ranges = <>; 
        panel_y88249@0 {
                compatible = "ingenic,y88249";
                status = "okay";                /*默认配置*/
                pinctrl-names = "default";
                pinctrl-0 = <&tft_lcd_pb>;
                ingenic,vdd-en-gpio = <&gpc 12 GPIO_ACTIVE_LOW INGENIC_GPIO_NOBIAS>;
                ingenic,rst-gpio = <&gpc 1 GPIO_ACTIVE_LOW INGENIC_GPIO_NOBIAS>;
                ingenic,pwm-gpio = <&gpc 15 GPIO_ACTIVE_LOW INGENIC_GPIO_NOBIAS>;
                port {
                        panel_y88249_ep: endpoint {
                                remote-endpoint = <&dpu_out_ep>;
                        };
                };
        };
};
```

## 2. 驱动配置

#### 在电脑端输入 make menuconfig， 然后配置：

```
Frame buffer Devices  --->
        <*> Support for frame buffer devices  ---> 
        <*> Ingenic Framebuffer Driver  ----
        (9) Vsync skip ratio[0..9]
        (3) how many frames support  /*配置驱动支持的frame数*/
        (4) how many layers support  /*配置驱动支持的layer数*/
        [*] fb test for displaying color bar
        <*> Ingenic Framebuffer Driver for X2000_v12  --->
                --- Ingenic Framebuffer Driver for X2000_v12
                -*-   Supported lcd panels  --->
                        <*>   lcd panel y88249
-*- Backlight & LCD device support  --->
        -*-   Lowlevel LCD controls 
        -*-   Lowlevel Backlight controls
```

# 三、用户空间

## 1. 设备节点
```
/dev/fb0
```
## 2. 应用程序
```
manhatton工程
packages/example/Sample/dpu
```
## 3. 测试方法
测试应用为testsuite/dpu，应用中包含：剪辑、全局透明、四层叠加和缩放。

```
在测试前需要确定kernel配置支持的最大layer数，并使用-l选项配置应用程序
# ./dpu -l 2                            /*　使用-l配置最大支持２个layer　*/
# ./dpu -l 2 -n 1000                    /*　使用-n配置刷新多少帧后接收应用程序　*/
# ./dpu -l 2 -n 1000 -o 1               /*　使用-o配置打开的layer数，要求等于小于kernel配置的最大layer数 */
# ./dpu -l 2 -n 1000 -o 1 -F rgb565     /*　使用-F 配置１，２层显示的格式，支持rgb565,nv12,yuv422 */
```

