# adc接口

# 一、模块简介

A / D是CMOS低功耗10位触摸屏SAR（逐次逼近寄存器）模数转换器，它使用3.3V电源供电，它是针对40nm CMOS工艺的嵌入式高分辨率ADC而开发的，已广泛应用于便携式电子设备，高端家庭娱乐中心，通信系统等。

 SAR A / D控制器专用于控制A / D在两种不同模式下工作：正常操作模式和重复采样，采样数据可以通过CPU传输到内存中。

# 二、内核空间

**驱动位置**

```
drivers/mfd/ingenic_adc_aux.c
drivers/mfd/ingenic_adc_v13.c
```

## 1. 设备树配置

**设备树代码位置arch/mips/boot/dts/ingenic/x2000-v12.dtsi**

```
sadc: sadc@10070000 {
                        compatible = "ingenic,sadc";
                        reg = <0x10070000 0x32>;
                        interrupt-parent = <&core_intc>;
                        interrupts = <IRQ_SDAC>;
                        status = "disable";
                };
```

## 2. 驱动配置

```
Symbol: MFD_INGENIC_SADC_V13 [=y]                                        
Type  : tristate                                                         
Prompt: Support for the Ingenic X2000_V12 SADC core                      
  Location:                                                              
    -> Device Drivers                                                    
      -> Multifunction device drivers                                    
  Defined at drivers/mfd/Kconfig:417                                     
  Depends on: HAS_IOMEM [=y] && (SOC_X2000_V12 [=y] || MACH_XBURST2 [=y])
  Selects: MFD_CORE [=n]
```

```
 Symbol: MFD_INGENIC_SADC_AUX [=y]                                          
 Type  : tristate                                                           
 Prompt: Support for the Ingenic X2000_V12 SADC AUX                         
   Location:                                                                
     -> Device Drivers                                                      
       -> Multifunction device drivers                                      
   Defined at drivers/mfd/Kconfig:425                                       
   Depends on: HAS_IOMEM [=y] && (SOC_X2000_V12 [=y] || MACH_XBURST2 [=y])  
   Selects: MFD_CORE [=n]
```

# 三、用户空间

## 1. 设备节点

```
/dev/ingenic_adc_aux_0
/dev/ingenic_adc_aux_1
/dev/ingenic_adc_aux_2
/dev/ingenic_adc_aux_3
/dev/ingenic_adc_aux_4
/dev/ingenic_adc_aux_5
```

测试方法

直接读取节点的值即可

