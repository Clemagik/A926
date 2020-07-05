# i2c接口

# 一、模块简介

SMB总线是两线串行接口，由串行数据线（SDA）和串行时钟（SCL）组成，这些电线在连接到总线的设备之间传送信息。每个设备都有一个唯一的地址，并且可以根据设备的功能充当“发送器”或“接收器”，在执行数据传输时，设备也可以视为主机或从机。主设备是初始化/终止总线上的数据传输并生成时钟信号以允许该传输的设备。在这段时间内，任何寻址的设备都被视为从设备，SMB控制器是软件控制的，它充当主机或从机，但是，不支持同时作为主机和从机运行。

X2000 的 i2c 控制器为 SMB，SMB 支持的接口有 i2c,支持 100 Kb/s 和 400 Kb/s，I2C 接口可连接至 pmu, camera，通过 i2c 接口进行配置。

# 二、内核空间

驱动位置

```
drivers/i2c/busses/i2c-ingenic.c
```

## 1. 设备树配置

**设备树位置arch/mips/boot/dts/ingenic/x2000-v12.dtsi**

```
               i2c0: i2c@0x10050000 {
                        compatible = "ingenic,x2000-i2c";
                        reg = <0x10050000 0x1000>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_I2C0>;
                        #address-cells = <1>;
                        #size-cells = <0>;
                        status = "okay";
                };

                i2c1: i2c@0x10051000 {
                        compatible = "ingenic,x2000-i2c";
                        reg = <0x10051000 0x1000>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_I2C1>;
                        #address-cells = <1>;
                        #size-cells = <0>;
                        status = "disabled";
                };

                i2c2: i2c@0x10052000 {
                        compatible = "ingenic,x2000-i2c";
                        reg = <0x10052000 0x1000>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_I2C2>;
                        #address-cells = <1>;
                        #size-cells = <0>;
                        status = "disabled";
                };

                i2c3: i2c@0x10053000 {
                        compatible = "ingenic,x2000-i2c";
                        reg = <0x10053000 0x1000>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_I2C3>;
                        #address-cells = <1>;
                        #size-cells = <0>;
                        status = "disabled";
                };

                i2c4: i2c@0x10054000 {
                        compatible = "ingenic,x2000-i2c";
                        reg = <0x10054000 0x1000>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_I2C4>;
                        #address-cells = <1>;
                        #size-cells = <0>;
                        status = "okay";
                };

                i2c5: i2c@0x10055000 {
                        compatible = "ingenic,x2000-i2c";
                        reg = <0x10055000 0x1000>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_I2C5>;
                        #address-cells = <1>;
                        #size-cells = <0>;
                        status = "disabled";
                };
```

## 2. 驱动配置

```
Symbol: I2C_INGENIC [=y]                                                
Type  : boolean                                                         
Prompt: Ingenic SoC based on Xburst arch's I2C controler Driver support 
  Location:                                                             
    -> Device Drivers                                                   
      -> I2C support                                                    
        -> I2C support (I2C [=y])                                       
          -> I2C Hardware Bus support                                   
  Defined at drivers/i2c/busses/Kconfig:996                             
  Depends on: I2C [=y] && HAS_IOMEM [=y]
```

```
 [*] Ingenic SoC based on Xburst arch's I2C controler Driver support      
 [ ]   controler i2c no restart mode (NEW)                                
 (64)  INGENIC I2C Controller FIFO length (NEW)                           
 [ ]   enable or disable Ingenic Soc's I2C driver debug info (NEW)
```

设备节点导出到用户空间的/dev下

```
 Symbol: I2C_CHARDEV [=y]              
 Type  : tristate                      
 Prompt: I2C device interface          
   Location:                           
     -> Device Drivers                 
       -> I2C support                  
 (1)     -> I2C support (I2C [=y])     
   Defined at drivers/i2c/Kconfig:49   
   Depends on: I2C [=y]
```

# 三、用户空间

## 1. 设备节点

```
# ls /dev/i2c-
i2c-0  i2c-2  i2c-3  i2c-4
```

## 2. 测试方法

查看I2C总线

```
# i2cdetect -l
i2c-0   i2c             i2c0                                    I2C adapter
i2c-2   i2c             i2c2                                    I2C adapter
i2c-3   i2c             i2c3                                    I2C adapter
i2c-4   i2c             i2c4                                    I2C adapter
```





