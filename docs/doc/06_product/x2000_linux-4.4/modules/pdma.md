# pdma接口

# 一、模块简介

可编程DMA控制器（PDMAC）专门用于在片上外设（MSC，AIC，UART等），外部存储器和存储器映射的外部设备之间智能传输数据，支持多达32个独立的DMA通道。

# 二、内核空间

* 驱动位置

```
drivers/dma/ingenic$ tree .
.
├── ingenic_dma.c
├── ingenic_dma.h
└── Makefile
```

## 1. 设备树配置

* 设备树代码位置

**1、soc 级的代码位置arch/mips/boot/dts/ingenic/x2000-v12.dtsi**

```
pdma: dma@13420000 {
                        compatible = "ingenic,x2000-pdma";
                        reg = <0x13420000 0x10000>;
                        interrupt-parent = <&core_intc>;
                        interrupt-names = "pdma", "pdmad";
                        interrupts = <IRQ_PDMA>, <IRQ_PDMAD>;
                        #dma-channels = <32>;
                        #dma-cells = <1>;
                        ingenic,reserved-chs = <0x3>;
                };
```

**2、板级配置 halley5\_v20 为例子arch/mips/boot/dts/ingenic/halley5\_v20.dts**

```
pdma {
        status = "okay";
};
```

## 2. 驱动配置

**1、控制器驱动**

```
Symbol: INGENIC_PDMAC [=y]                                                                                                                            
Type  : boolean                                                                                                                                       
Prompt: Ingenic programmable dma controller (Of Driver)                                                                                               
  Location:                                                                                                                                           
    -> Device Drivers                                                                                                                                 
      -> DMA Engine support (DMADEVICES [=y])                                                                                                         
  Defined at drivers/dma/Kconfig:280                                                                                                                  
  Depends on: DMADEVICES [=y] && (MACH_XBURST [=n] || MACH_XBURST2 [=y])                                                                              
  Selects: DMA_VIRTUAL_CHANNELS [=y] && DMA_ENGINE [=y]                                                                                               
  Selected by: SND_ASOC_PDMA [=n] && SOUND [=n] && !M68K && !UML && SND [=n] && SND_SOC [=n] && SND_ASOC_INGENIC [=n] && SND_ASOC_INGENIC_AS_V1 [=n]
```

**2、测试驱动**

```
 Symbol: DMATEST [=y]                             
 Type  : tristate                                 
 Prompt: DMA Test client                          
   Location:                                      
     -> Device Drivers                            
       -> DMA Engine support (DMADEVICES [=y])    
   Defined at drivers/dma/Kconfig:561             
   Depends on: DMADEVICES [=y] && DMA_ENGINE [=y]
```

# 三、用户空间

## 1. 设备节点

**1、控制器节点**

```
# cd /sys/devices/platform/ahb2/13420000.dma/dma/
# ls
dma0chan0   dma0chan14  dma0chan2   dma0chan25  dma0chan30  dma0chan8
dma0chan1   dma0chan15  dma0chan20  dma0chan26  dma0chan31  dma0chan9
dma0chan10  dma0chan16  dma0chan21  dma0chan27  dma0chan4
dma0chan11  dma0chan17  dma0chan22  dma0chan28  dma0chan5
dma0chan12  dma0chan18  dma0chan23  dma0chan29  dma0chan6
dma0chan13  dma0chan19  dma0chan24  dma0chan3   dma0chan7
```

**2、测试程序的节点**

```
# cd /sys/module/dmatest/parameters/
# ls
channel           noverify          threads_per_chan  xor_sources
device            pq_sources        timeout
iterations        run               verbose
max_channels      test_buf_size     wait
```

## 2. 测试方法

**1、进入到测试程序的节点**

```
# echo dma0chan31 > /sys/module/dmatest/parameters/channel
# echo 2000 > /sys/module/dmatest/parameters/timeout
# echo 1 > /sys/module/dmatest/parameters/iterations
# echo 1 > /sys/module/dmatest/parameters/run
```

**2、测试结果**

```
[  148.309057] dmatest: Started 1 threads using dma0chan31
[  148.309825] dmatest: dma0chan31-copy: summary 1 tests, 0 failures 1355 iops 10840 KB/s (0)
```



