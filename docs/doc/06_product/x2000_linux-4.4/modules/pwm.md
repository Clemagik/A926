# pwm模块

# 一、模块简介

Pulse width modulation\(pwm\)模块设计分为16个独立的通道，可以互不影响的进行独立工作，同时又分为两种工作模式，为cpu模式和DMA模式。cpu模式通过使用输入的时钟源来产生方波，而DMA模式的方波产生是通过fifo中的的数据来决定周期以及占空比，另外DMA模式必须按照4word进行对齐。

![](/assets/pwm.png)

# 二、内核空间

* 驱动位置

```
drivers/pwm/pwm-ingenic-v2.c
```

## 1. 设备树配置

* 设备树代码位置

**１、arch/mips/boot/dts/ingenic/x2000-v12-pinctrl.dtsi**

```
定义相关的引脚以及function例如：
pwm_pin: pwm-pin{
                pwm0_pc: pwm0_pc {
                        ingenic,pinmux = <&gpc 0 0>;
                        ingenic,pinmux-funcsel = <PINCTL_FUNCTION0>;
                };
....
....
....
               pwm15_pc: pwm15_pc {
                        ingenic,pinmux = <&gpc 15 15>;
                        ingenic,pinmux-funcsel = <PINCTL_FUNCTION0>;
                };
```

**２、arch/mips/boot/dts/ingenic/x2000-v12.dtsi**

```
pwm: pwm@0x134c0000 {

        compatible = "ingenic,x2000-pwm";
        reg = <0x134c0000 0x10000>;
        interrupt-parent = <&core_intc>;
        interrupts = <IRQ_PWM>;
        pinctrl-names = "default","pwm15_pc";
        pinctrl-0 = <&pwm0_pc> ,<&pwm15_pc>;
        status = "ok";
};

默认使用的是pin0和pin15通道，如有不同的需求可以增加或是减少注册的pin。
```

## 2. 驱动配置

```
１、打开菜单配置　make menuconfig

Symbol: PWM_INGENIC_V2 [=y]                                                                                                                                                                         │  
   Type  : tristate                                                                                                                                                                                    │  
   Prompt: Ingenic PWM V2 support                                                                                                                                                                      │  
     Location:                                                                                                                                                                                         │  
       -> Device Drivers                                                                                                                                                                               │  
         -> Pulse-Width Modulation (PWM) Support (PWM [=y])                                                                                                                                            │  
     Defined at drivers/pwm/Kconfig:202                                                                                                                                                                │  
     Depends on: PWM [=y] && MACH_XBURST2 [=y]
```

# 三、用户空间

## 1. 设备节点

**１、进入编译后的内核系统**

```
cd sys/devices/platform/ahb2/134c0000.pwm/

 channels         enable           power            uevent
 config           free             pwm
 driver           modalias         request
 driver_override  of_node          subsystem
```

## 2. 测试方法

**1、使用相关的文件主要有request,config,enable,free以及channels**

芯片设计提供16个pwm通道编号为0~15;

16个通道使用的gpioc00~15,FUNCTION0;

\(1\)使用之前要明确使用哪个通道之后去申请这个通道　如：

```
echo 15 > request //申请15通道，前提是相应的dts中要注册了
```

\(2\)此时可以看到该通道已被申请

```
# cat channels
ch: 00 unrequested
ch: 01 unrequested
ch: 02 unrequested
ch: 03 unrequested
ch: 04 unrequested
ch: 05 unrequested
ch: 06 unrequested
ch: 07 unrequested
ch: 08 unrequested
ch: 09 unrequested
ch: 10 unrequested
ch: 11 unrequested
ch: 12 unrequested
ch: 13 unrequested
ch: 14 unrequested
ch: 15 requested
```



\(3\)可以进行配置占空比

```
echo 0 100000 200000 > config　　//第一个参数模式选择（０：cpu　1:DMA）二三参数是占空和周期，单位是ns;
current_id = 15, period_ns = 200000 , duty_ns = 100000 mode = COMMON_MODE
period=4760
duty=2380 level_init=1 low=2380
pwm_config finish
```

\(4\)使能该通道

```
echo 1 > enable
input enable value 1,current_id 15
```

\(5\)验证就可以去量取对应的pin引脚的脉冲（根据设计的电路原理图）。

\(6\)关闭该通道

```
# echo 15 > free
# cat channels
ch: 00 unrequested
ch: 01 unrequested
ch: 02 unrequested
ch: 03 unrequested
ch: 04 unrequested
ch: 05 unrequested
ch: 06 unrequested
ch: 07 unrequested
ch: 08 unrequested
ch: 09 unrequested
ch: 10 unrequested
ch: 11 unrequested
ch: 12 unrequested
ch: 13 unrequested
ch: 14 unrequested
ch: 15 unrequested
```

　　到此基本的使用流程就结束了，以上了流程可以对每一个通道使用，request后就要配置之后使能，如果没有使能最好先将其free。

注意：当request一个通道后拿到的是该通道的id，以至于当前的操作全部基于此id。

