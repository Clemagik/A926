# uart接口

# 一、模块简介

通用异步接收器/发送器（UART）串行端口。 共有十个UART：所有UART使用相同的编程模型。 每个串行端口都可以在基于中断的模式或基于DMA的模式下运行。通用异步接收器/发送器（UART）与16550行业标准兼容，可以用作符合红外数据协会（IrDA）串行红外规范1.1的慢速红外异步接口。

# 二、内核空间

**驱动位置**

```
drivers/tty/serial/ingenic_uart.h
drivers/tty/serial/ingenic_uart.c
```

## 1. 设备树配置

**设备树soc级路径arch/mips/boot/dts/ingenic/x2000-v12.dtsi**

```
               uart0: serial@0x10030000 {
                        compatible = "ingenic,8250-uart";
                        reg = <0x10030000 0x100>;
                        reg-shift = <2>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_UART0>;
                        clocks = <&extclk 0>;
                        clock-names = "uart0";
                };
                uart1: serial@0x10031000 {
                        compatible = "ingenic,8250-uart";
                        reg = <0x10031000 0x100>;
                        reg-shift = <2>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_UART1>;
                };
                uart2: serial@0x10032000 {
                        compatible = "ingenic,8250-uart";
                        reg = <0x10032000 0x100>;
                        reg-shift = <2>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_UART2>;
                };
                uart3: serial@0x10033000 {
                        compatible = "ingenic,8250-uart";
                        reg = <0x10033000 0x100>;
                        reg-shift = <2>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_UART3>;
                };

                uart4: serial@0x10034000 {
                        compatible = "ingenic,8250-uart";
                        reg = <0x10034000 0x100>;
                        reg-shift = <2>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_UART4>;
                        status = "disabled";
                };
                uart5: serial@0x10035000 {
                        compatible = "ingenic,8250-uart";
                        reg = <0x10035000 0x100>;
                        reg-shift = <2>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_UART5>;
                        status = "disabled";
                };
                uart6: serial@0x10036000 {
                        compatible = "ingenic,8250-uart";
                        reg = <0x10036000 0x100>;
                        reg-shift = <2>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_UART6>;
                        status = "disabled";
                };
                uart7: serial@0x10037000 {
                        compatible = "ingenic,8250-uart";
                        reg = <0x10037000 0x100>;
                        reg-shift = <2>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_UART7>;
                        status = "disabled";
                };

                uart8: serial@0x10038000 {
                        compatible = "ingenic,8250-uart";
                        reg = <0x10038000 0x100>;
                        reg-shift = <2>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_UART8>;
                        status = "disabled";
                };
                uart9: serial@0x10039000 {
                        compatible = "ingenic,8250-uart";
                        reg = <0x10039000 0x100>;
                        reg-shift = <2>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_UART9>;
                        status = "disabled";
                };
```

**设备树板级的设备树arch/mips/boot/dts/ingenic/zebra.dts**

```
&uart0 {
        status = "disable";
        pinctrl-names = "default";
        pinctrl-0 = <&uart0_pd>;
};

&uart2 {
        status = "okay";
        pinctrl-names = "default";
        pinctrl-0 = <&uart2_pd>;
};

&uart3 {
        status = "okay";
        pinctrl-names = "default";
        pinctrl-0 = <&uart3_pd>;
};
```

## 2. 驱动配置

```
Symbol: SERIAL_INGENIC_UART [=y]                  
Type  : tristate                                  
Prompt: ingenic serial port support               
  Location:                                       
    -> Device Drivers                             
      -> Character devices                        
        -> Serial drivers                         
  Defined at drivers/tty/serial/Kconfig:1632      
  Depends on: TTY [=y] && HAS_IOMEM [=y]          
  Selects: SERIAL_CORE [=y]
```

```
 <*> ingenic serial port support                         
 [*]   Console on ingenic soc and compatible serial port 
 [*]   ingenic baudrate add support greater than 1M      
 [*]   ingenic uart enable Magic SysRq key
```

# 三、用户空间

## 1. 设备节点

**设备节点**

```
# ls /dev/ttyS
ttyS1  ttyS2  ttyS3
```

**启动信息**

```
[    0.149670] 10032000.serial: ttyS2 at MMIO 0x10032000 (irq = 53, base_baud = 9375000) is a uart2
[    0.158607] console [ttyS2] enabled
[    0.158607] console [ttyS2] enabled
```

## 2. 测试方法

```
# cat /dev/ttyS2 &
# echo "this is serial test string" > /dev/ttyS2 
this is serial test string
```



