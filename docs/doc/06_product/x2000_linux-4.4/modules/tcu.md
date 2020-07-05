# tcu模块

# 一、模块简介

定时计数单元\(TCU\)芯片设计包含８个tcu通道分别是0~7标号，每个通道可以有三个时钟输入源pwm\(gpio0,gpio1\),extal，可以使用其中的任意组合进行功能的使用。

按照spec描述可以分为７种功能模式分别是：

**常规模式**: 计数器在时钟的上升沿或是下降沿进行计数，也可以上升下降同时采集计数.

**门模式**: 门为０时，计数器计数.

**正交模式**: 计数器由于正交输入而计数.

**方向模式**: 由输入信号决定计数器的递增还是递减.

**pos 模式**: 由于上升沿或是下降沿，计数器开始从０开始计数.

**捕获模式**: 计数器对周期进行计数并输出高电平时间和周期时间

**filter function**\(example for ch0\)

| TCU通道 | GPIO0 | GPIO1 |
| :--- | :--- | :--- |
| ch0 | pwm0 | pwm1 |
| ch1 | pwm2 | pwm3 |
| ch2 | pwm4 | pwm5 |
| ch3 | pwm6 | pwm7 |
| ch4 | pwm8 | pwm9 |
| ch5 | pwm10 | pwm11 |
| ch6 | pwm12 | pwm13 |
| ch7 | pwm14 | pwm15 |

# 二、内核空间

驱动位置

```
drivers/mfd/ingenic-tcu.c
```

## 1. 设备树配置

**１、arch/mips/boot/dts/ingenic/x2000-v12.dtsi**

```
tcu: tcu@0x10002000 {

    compatible = "ingenic,tcu";
    reg = <0x10002000 0x1000>;
    interrupt-parent = <&core_intc>;
    interrupt-names = "default";
    interrupts = <IRQ_TCU0>;
    status = "ok";

};
```

**2、arch/mips/boot/dts/ingenic/zebra.dts**

```
&pwm {
        status = "okay";
        pinctrl-names = "default","pwm15_pc";
        pinctrl-0 = <&pwm0_pc> ,<&pwm15_pc>; 
};
```

## 2. 驱动配置

**1、make menuconfig**

**2、选择对应的驱动配置**

```
Symbol: MFD_INGENIC_TCU [=y]                                              
Type  : boolean                                                           
Prompt: Ingenic tcu driver                                                
  Location:                                                               
    -> Device Drivers                                                     
      -> Multifunction device drivers                                     
  Defined at drivers/mfd/Kconfig:417                                      
  Depends on: HAS_IOMEM [=y] && (MACH_XBURST [=n] || MACH_XBURST2 [=y])   
  Selects: MFD_CORE [=y] && GENERIC_IRQ_CHIP [=y]
```

# 三、用户空间

## 1. 设备节点

**1、系统启动后进入节点**

```
cd sys/devices/platform/apb/10002000.tcu/
disable          enable           power
driver           modalias         subsystem
driver_override  of_node          uevent
```

## 2. 测试方法

**1、使用主要用到的是disable和enable**

**2、驱动中有一个配置的函数默认为drivers/mfd/ingenic-tcu.c**

```
/*This is a simple configuration test demo*/
static void ingenic_tcu_config_attr(int id,enum tcu_mode_sel mode_sel)
{
        int i = 0;
        g_tcu_chn[id].mode_sel = mode_sel;
        g_tcu_chn[id].irq_type = FULL_IRQ_MODE;
        g_tcu_chn[id].full_num = 0xffff;
        g_tcu_chn[id].half_num = 0x5000;
        g_tcu_chn[id].prescale = TCU_PRESCALE_1;

        g_tcu_chn[id].count_value = 0;
        g_tcu_chn[id].shutdown_mode = 0;
        g_tcu_chn[id].en_flag = 1;

        /*In order to facilitate testing,
         * it has no practical significance.*/
        switch(g_tcu_chn[id].mode_sel){
                case GENERAL_MODE:
                        /*Enable external clock to use rising edge counting , result TCNT != 0*/
                        g_tcu_chn[id].clk_ext = TCU_CLKSRC_EXT;
                        g_tcu_chn[id].sig_ext = SIG_POS_EN;
                        break;
                case GATE_MODE:
                        /*gate signal hold on 0,counter start when control signal is 1,result TCNT == 0*/
                        g_tcu_chn[id].gate_sel = GATE_SEL_HZ;
                        g_tcu_chn[id].gate_pola = GATE_POLA_HIGH;
                        g_tcu_chn[id].clk_ext = TCU_CLKSRC_EXT;
                        g_tcu_chn[id].sig_ext = SIG_POS_EN;
                        break;
                case DIRECTION_MODE:
                        /*use gpio0 with direction signa. counter sub when control signal is 1.result TCNT add and sub */
                        g_tcu_chn[id].clk_gpio0 = TCU_CLKSRC_GPIO0;
                        g_tcu_chn[id].dir_sel = DIR_SEL_GPIO0;
                        g_tcu_chn[id].dir_pola = DIR_POLA_HIGH;
                        g_tcu_chn[id].clk_ext = TCU_CLKSRC_EXT;
                        g_tcu_chn[id].sig_ext = SIG_POS_EN;
                        break;
                case POS_MODE:
                        /*TCNT is cleared on the rising edge of gpio0.*/
                        g_tcu_chn[id].clk_ext = TCU_CLKSRC_EXT;
                        g_tcu_chn[id].sig_ext = SIG_POS_EN;
                        /*gpio0 pos clear count*/
                        g_tcu_chn[id].clk_gpio0 = TCU_CLKSRC_GPIO0;
                        g_tcu_chn[id].sig_gpio0 = SIG_POS_EN;
                        g_tcu_chn[id].capture_sel = CAPTURE_GPIO0;
                        break;
                case CAPTURE_MODE:
                        /*Use ext_clk to capture the duty cycle of gpio0, result CAP_VAL register have value*/
                        g_tcu_chn[id].clk_ext = TCU_CLKSRC_EXT;
                        g_tcu_chn[id].sig_ext = SIG_POS_EN;
                        g_tcu_chn[id].clk_gpio0 = TCU_CLKSRC_GPIO0;
                        g_tcu_chn[id].capture_sel = CAPTURE_GPIO0;
                        g_tcu_chn[id].capture_num = 0xa0;
                        break;
                case FILTER_MODE:
                        /*for easy test set max*/
                        g_tcu_chn[id].clk_gpio0 = TCU_CLKSRC_GPIO0;
                        g_tcu_chn[id].sig_gpio0 = SIG_POS_EN;
                        g_tcu_chn[id].fil_a_num = 0x3ff;
                        g_tcu_chn[id].fil_b_num = 0x3ff;
                        break;
                default:
                         break;
        }

        ingenic_tcu_config_chn(&g_tcu_chn[id]);
        ingenic_tcu_enable_counter(&g_tcu_chn[id]);
        ingenic_tcu_start_counter(&g_tcu_chn[id]);

        for(i = 0; i < 20 ;i++)
                sws_pr_debug(&g_tcu_chn[id]);
}

```

说明：具体应用场景还没有，系统接口没有留太多，此函数的配置结合spec能够进行快速的验证功能,后续会留更多的系统配置接口。

在使用DIRECTION\_MODE、POS\_MODE、CAPTURE\_MODE、FILTER\_MODE模式之前请先配置对应tcu通道的pwm0\(gpio0\)。

在使用QUADRATURE\_MODE模式的时候请先配置对应tcu通道的pwm0\(gpio0\)和pwm1\(gpio1\)为正交信号。

**3、使用过程就是把对应的通道写入enable该通道就开始工作**

```
# cat enable 

mode_sel :
0:GENERAL_MODE 1:GATE_MODE 2:DIRECTION_MODE 3:QUADRATURE_MODE
4:POS_MODE 5:CAPTURE_MODE 6:FILTER_MODE

####################example####################
## echo channel_id mode_sel > enable 
## echo channel_id > disable 

channel: 00 disable
channel: 01 disable
channel: 02 disable
channel: 03 disable
channel: 04 disable
channel: 05 disable
channel: 06 disable
channel: 07 disable

# echo 0 0 > enable 
[  164.485676] mode_id = 0 
...
...
[  165.896941] ----------------------------------------count N0.20----------------------------------------------------
[  165.896941] 
[  165.912304] -stop-----addr-b000201c-value-00000000-----------
[  165.918236] -mask-----addr-b0002030-value-00ff80fe-----------
[  165.924181] -enable---addr-b0002010-value-00000001-----------
[  165.930120] -flag-----addr-b0002020-value-00010000-----------
[  165.936048] -Control--addr-b000204c-value-00010004-----------
[  165.941990] -full-----addr-b0002040-value-0000ffff-----------
[  165.947925] -half-   -addr-b0002044-value-00005000-----------
[  165.953874] -TCNT-----addr-b0002048-value-0000b5bf-----------
[  165.959811] -CAP reg_base-------value-00000000-----------
[  165.965399] -CAP_VAL register---value-00000000-----------



```

**4、把对应的通道写入disable就停止计数工作了**

```
# echo 0 > disable
# cat disable
channel: 00 disable
channel: 01 disable
channel: 02 disable
channel: 03 disable
channel: 04 disable
channel: 05 disable
channel: 06 disable
channel: 07 disable
```

说明：tcu使用的功能模式比较多，以至于没有留有太多的接口，但是内部逻辑都已经写好了，只需要按照spec中的不同模式的使用方式对ingenic\_tcu\_config\_attr函数赋值就好了，每一类的宏也定义好了，能够方便的配置使用,不同的使用场景请修改相应的配置。

