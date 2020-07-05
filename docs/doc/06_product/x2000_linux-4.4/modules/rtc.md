# rtc

# 一、模块简介

实时时钟（RTC）单元可以在芯片主电源打开或主电源关闭但RTC电源仍然打开的情况下运行。在这种情况下，RTC电源域仅消耗几微瓦的功率。RTC包含实时，定时告警逻辑以及断电和唤醒控制逻辑。

# 二、内核空间

驱动位置

```
drivers/rtc/rtc-ingenic.c 
drivers/rtc/rtc-ingenic.h
```

## 1. 设备树配置

**1、arch/mips/boot/dts/ingenic/x2000-v12.dtsi**

```
rtc: rtc@0x10003000 {
        compatible = "ingenic,rtc";
        reg = <0x10003000 0x4c>;
        interrupt-parent = <&core_intc>;
        interrupts = <IRQ_RTC>;
        system-power-controller;
        power-on-press-ms = <1000>;
        status = "ok";
};
```

## 2. 驱动配置

**１、make menuconfig**

```
Symbol: RTC_DRV_INGENIC [=y]                  
Type  : tristate                              
Prompt: INGENIC RTC                           
  Location:                                   
    -> Device Drivers                         
      -> Real Time Clock (RTC_CLASS [=y])     
  Defined at drivers/rtc/Kconfig:142          
  Depends on: RTC_CLASS [=y]
```

**2、测试rtc另外需配置**

```
Symbol: SUSPEND_TEST [=y]                        
Type  : boolean                                  
Prompt: auto suspend test                        
  Location:                                      
    -> Machine selection                         
      -> SOC type (SOC_TYPE [=n])                
Prompt: Ingenic RTC suspend test                 
  Location:                                      
    -> Device Drivers                            
      -> Real Time Clock (RTC_CLASS [=y])        
        -> INGENIC RTC (RTC_DRV_INGENIC [=y])    
  Defined at arch/mips/xburst/Kconfig:61         
  Depends on: SOC_TYPE [=n]
```

```
Symbol: SUSPEND_ALARM_TIME [=60]                                 
Type  : integer                                                  
Prompt: suspend alarm time(second)                               
  Location:                                                      
    -> Machine selection                                         
      -> SOC type (SOC_TYPE [=n])                                
        -> auto suspend test (SUSPEND_TEST [=y])                 
Prompt: Ingenic RTC suspend alarm time Unit of second            
  Location:                                                      
    -> Device Drivers                                            
      -> Real Time Clock (RTC_CLASS [=y])                        
        -> INGENIC RTC (RTC_DRV_INGENIC [=y])                    
          -> Ingenic RTC suspend test (SUSPEND_TEST [=y])        
  Defined at arch/mips/xburst/Kconfig:65                         
  Depends on: SOC_TYPE [=n] && SUSPEND_TEST [=y]
```

# 三、用户空间

## 1. 测试方法

**１、首先需要选择在驱动配置完成如下**

```
<*>   INGENIC RTC                                                   
[*]     Ingenic RTC suspend test                                    
(60)      Ingenic RTC suspend alarm time Unit of second (NEW)    
```

**２、编译进内核启动**

**３、测试休眠唤醒－定时唤醒**

```
echo mem > sys/power/state
```

休眠60秒后自动唤醒－（测试点为定时和计时准确以及唤醒功能）



