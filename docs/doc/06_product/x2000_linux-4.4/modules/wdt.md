# watchdog

# 一、模块简介

看门狗定时器用于在处理器受噪声和系统错误等故障干扰时恢复处理器，看门狗定时器可以产生复位信号。看门狗使用的是rtc提供的时钟源，通过软件可以分频为1,4,16,64,256和1024，并且拥有16bit的计数寄存器，除此之外还支持半中断处理。

# 二、内核空间

驱动代码位置：

```
drivers/watchdog/ingenic_wtd.c
```

## 1. 设备树配置

**１、在arch/mips/boot/dts/ingenic/x2000-v12.dtsi下配置：**

```
watchdog: watchdog@0x10002000 {
        compatible = "ingenic,watchdog";
        reg = <0x10002000 0x40>;
        interrupt-parent = <&core_intc>;
        interrupts = <IRQ_TCU0>;
        status = "ok";  /*发布版本默认状态*/
}
```

## 2. 驱动配置

**1、 make menuconfig， 然后配置：**

```
--- Watchdog Timer Support
-*-   WatchDog Timer Driver Core
<*>   Ingenic ingenic SoC hardware watchdog
```

# 三、用户空间

## 1. 设备节点

**1、驱动注册成功后出现设备节点**

```
/dev/watchdog
```

## 2. 应用程序

发布的SDK中busybox会默认配置watchdog测试应用，。

## 3. 测试方法

以下为测试应用的说明：

```
 watchdog [-t N[ms]] [-T N[ms]] [-F] DEV

    Periodically write to watchdog device DEV

    Options:

            -T N    Reboot after N seconds if not reset (default 60)
            -t N    Reset every N seconds (default 30)
            -F      Run in foreground
    Use 500ms to specify period in milliseconds
```

执行以下命令测试watchdog：

```
watchdog -T 3000 /dev/watchdog    /*3 seconds重启*/
```



