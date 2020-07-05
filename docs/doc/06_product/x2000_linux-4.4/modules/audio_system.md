# 一、模块简介

## 1. 硬件模块简介![](/assets/audio_hw.png)

1. audio有10个DMA模块,其中DMA0-DMA4功能是从RAM搬移数据到fifo,DMA5-DMA9功能是从fifo搬移数据到RAM中,每个DMA固定连接到DBUS中的一个FIFO上.
2. 5个BAIC\(Basic Audio Interface Controller\),BAIC0和ICODEC\(Internal CODEC\)相连支持单通道录放音
3. dmic最大支持8通道,支持语音唤醒
4. spdif\(Sony/Philips Digital Interconnect Format\)
5. DBUS可以把RFIFO和TFIFO动态连接在一起形成通路,所以通过DBUS的配置可以把DMA和其他子模块连接成数据流通路.

| 模块名 | 支持协议 | 支持通道数 | 功能 |
| :--- | :--- | :--- | :--- |
| BAIC0+ICODEC |  | 1通道\(限制于ICODEC\) | 录音/放音 |
| BAIC1 | I2S/LEFT/RIGHT/PCMA/PCMB/DSPA/DSPB | 1/2通道 | 录音/放音 |
| BAIC2 | TDM1A/TDM1B/TDM2A/TDM2B/I2S7+1/LEFT7+1/RIGHT7+1 | 2/4/8通道 | 录音 |
| BAIC3 | TDM1A/TDM1B/TDM2A/TDM2B/I2S7+1/LEFT7+1/RIGHT7+1 | 2/4/8通道 | 放音 |
| BAIC4 | I2S/LEFT/RIGHT/PCMA/PCMB/DSPA/DSPB | 1/2通道 | 录音/放音 |
| DMIC | Pulse Density Modulation | 1/2/4/6/8通道 | 录音 |
| SPDIF | BMC\(Biphase Mark Code\) | 2通道 | 录音/放音 |

## 2. 软硬件模块对应关系![](/assets/audio_soft.png)

##### amixer配置数据通路时的软硬件对应关系

| 硬件名称 | 软件名称 |
| :--- | :--- |
| BAIC0\_PLAYBACK | LO0\_MUX |
| BAIC1\_PLAYBACK | LO1\_MUX |
| SPDIF\_PLAYBACK | LO2\_MUX |
| BAIC3\_PLAYBACK | LO3\_MUX |
| BAIC4\_PLAYBACK | LO4\_MUX |
| DMA5 | LO5\_MUX |
| DMA6 | LO6\_MUX |
| DMA7 | LO7\_MUX |
| DMA8 | LO8\_MUX |
| DMA9 | LO9\_MUX |
| MIX\_IN0 | LO10\_MUX |
| MIX\_IN1 | LO11\_MUX |

##### 数据通路输入端软硬件对应关系

| 硬件名称 | 软件名称 |
| :--- | :--- |
| DMIC\_CAPTURE | LI0 |
| SPDIF\_CAPTURE | LI1 |
| BAIC0\_CAPTURE | LI2 |
| BAIC1\_CAPTURE | LI3 |
| BAIC2\_CAPTURE | LI4 |
| BAIC4\_CAPTURE | LI6 |
| MIX0\_OUT | LI7 |
| DMA0 | LI8 |
| DMA1 | LI9 |
| DMA2 | LI10 |
| DMA3 | LI11 |
| DMA4 | L12 |

# 二、内核空间

驱动代码位置：

**audio 控制器源码位置：**

```
sound/soc/ingenic/as-v2
```

**板级源码位置：**

```
sound/soc/ingenic/board
根据开发板功能编写对应的板级驱动
```

**内部codec源码位置：**

```
sound/soc/ingenic/icodec/icdc_inno.c
```

**外部codec源码位置：**

```
sound/soc/ingenic/ecodec/
```

## 1. 设备树配置

**在arch/mips/boot/dts/ingenic/x2000-v12.dtsi下配置：**

```
as:as {
        compatible = "simple-bus";
        #address-cells = <1>;
        #size-cells = <1>;
        ranges = <>;

        /*as-platform 节点内属性不需要更改*/
        as_platform: as-platform {
                compatible = "ingenic,as-platform";
                reg = <0x134d0000 0x114>, <0x134d1000 0x100>;
                reg-names = "dma", "fifo";
                ingenic,fifo-size = <4096>;
                interrupt-parent = <&core_intc>;
                interrupts = <IRQ_AUDIO>;
                ingenic,fth_quirk;
        };
        /*as-vir-fe 节点是虚拟设备，可以删除*/
        as_virtual_fe: as-virtual-fe {
                compatible = "ingenic,as-vir-fe";
                reg = <0x00000000 0x0>;
                ingenic,cap-dai-bm = <0xc>;
                ingenic,num-dais = <4>;
        };
        /*as-fmtcov 节点内属性不需要更改*/
        as_fmtcov: as-fmtcov {
                compatible = "ingenic,as-fmtcov";
                reg = <0x134d2000 0x28>;
        };
        as_fe_dsp: as-dsp {
                compatible = "ingenic,as-dsp";
                reg = <0x134d4000 0x30>;
                /*li-port成员是有效的DBUS transmit设备号，与LI*相对应，不需要更改*/
                ingenic,li-port = <0 1 2 3 4 6 7 8 9 10 11 12>;
                /*lo-port成员是有效的DBUS receive设备号，与LO*_MUX相对应，不需要更改*/
                ingenic,lo-port = <0 1 2 3 4 5 6 7 8 9 10 11>;
                /*对应10个dma设备，置１位是Capture DMA, 低５位清零位　Playback DMA，不需要更改*/
                ingenic,cap-dai-bm = <0x3e0>;
                ／*10 个ＤＭＡ*／
                ingenic,num-dais = <10>;
        };
        as_be_baic: as-baic {
                compatible = "ingenic,as-baic";
                reg = <0x134d5000 0x5000>;
                ／*5个baic设备, 根据实际开发板实际使用情况定义，可以定义不使用*／
                ingenic,num-dais = <5>;
                /*使用的baic设备号，根据实际开发板实际使用情况定义，可以定义不使用*/
                ingenic,dai-array = <0>, <1>, <2>, <3>, <4>;
                ／*定义每个baic设备支持的功能，定义要与dai-arry设备号相对应*／
                ingenic,dai-mode = <BAIC_3AND(BAIC_PCM_MODE, BAIC_DSP_MODE, BAIC_I2S_MODE)>,
                        <BAIC_3AND(BAIC_PCM_MODE, BAIC_DSP_MODE, BAIC_I2S_MODE)>,
                        <BAIC_4AND(BAIC_I2S_MODE, BAIC_TDM1_MODE, BAIC_TDM2_MODE, BAIC_NO_REPLAY)>,
                        <BAIC_4AND(BAIC_I2S_MODE, BAIC_TDM1_MODE, BAIC_TDM2_MODE, BAIC_NO_RECORD)>,
            <BAIC_3AND(BAIC_PCM_MODE, BAIC_DSP_MODE, BAIC_I2S_MODE)>;
                ／*定义每个baic设备gpio引脚数量，定义要与dai-arry设备号相对应*／
                ingenic,data-pin-num = <1>, <1>, <4>, <4>, <1>;
                ／*定义每个baic设备是否接收和发送的时钟分离，定义要与dai-arry相对应*／
                ingenic,clk-split = <1>, <1>, <1>, <1>, <0>;
                ／*定义每个baic设备接收使用的cpm时钟，定义要与dai-arry设备号相对应*／
                ingenic,clk-rname = "div_i2s0","div_i2s0","div_i2s2","no_clk","no_clk";
                ／*定义每个baic设备接发送用的cpm时钟，定义要与dai-arry设备号相对应*／
                ingenic,clk-tname = "div_i2s1","div_i2s1","no_clk","div_i2s3","mux_pcm";
                ／*定义baic4设备的时钟源父设备，根据实际情况需要合理分配*／
                ingenic,pcm-clk-parent = "div_i2s2";
                ／*定义gpio_a组baic2和baic3 gpio功能引脚的电压，根据实际使用调整电压值*／
                ingenic,baic23-interface-vol = <BAIC23_INTERFACE_VOL_3V3>;
        };
        as_dmic: as-dmic {
                compatible = "ingenic,as-dmic";
                reg = <0x134da000 0x10>;
                ／*dmic 时钟源*／
                ingenic,clk-name = "mux_dmic";
                ／*dmic 时钟源父设备*／
                ingenic,clk-parent = "div_i2s3";
        };

        as_aux_mixer: as-mixer {
                compatible = "ingenic,as-mixer";
                reg = <0x134dc000 0x8>;
                ingenic,num-mixers = <1>;
        };
        as_spdif: as-spdif {
                compatible = "ingenic,as-spdif";
                reg = <0x134db000 0x14>, <0x134db100 0x14>;
                reg-names = "out", "in";
                ／*spdif 时钟源*／
                ingenic,clk-name = "mux_spdif";
                ／*spdif 时钟源父设备*／
                ingenic,clk-parent = "div_i2s2";
        };
};
```

**在arch/mips/boot/dts/ingenic/板级.dts下配置：**

```
&as_be_baic {
        ／*配置使用的gpio*／
        pinctrl-names = "default";
        pinctrl-0 = <&baic1_tmclk_pc>, <&baic1_rmclk_pc>, <&baic1_pc>, <&baic2_mclk_pa>, <&baic2_pa>,
                <&baic3_mclk_pa>, <&baic3_pa>, <&baic4_pd>;
        ingenic,baic23-interface-vol = <BAIC23_INTERFACE_VOL_3V3>;
};

&as_spdif {
        ／*配置使用的gpio*／
        pinctrl-names = "default";
        pinctrl-0 = <&spdif_pc>;
};

&as_dmic {
        ／*配置gpio,支持通道数不同配置不同*／
        pinctrl-names = "default";
        pinctrl-0 = <&dmic_pc>;
};

&icodec {
        ／*定义　baic0+icodec外接喇叭的控制gpio*／
        ingenic,spken-gpio = <&gpb 2 GPIO_ACTIVE_HIGH INGENIC_GPIO_NOBIAS>;
};
／*板级设备*／
sound {
        compatible = "ingenic,x2000-sound";
        ingenic,model = "halley5_v20";
};
```

## 2. 驱动配置

**make menuconfig， 然后配置：**

```
Symbol: SND_ASOC_INGENIC [=y]
    Type  : tristate
    Prompt: ASoC support for Ingenic
    Location:
    -> Device Drivers
    -> Sound card support (SOUND [=y])
     -> Advanced Linux Sound Architecture (SND [=y])
       -> ALSA for SoC audio support (SND_SOC [=y])
    Defined at sound/soc/ingenic/Kconfig:1
    Depends on: SOUND [=y] && !M68K && !UML && SND [=y] && (MACH_XBURST [=n] || MACH_XBURST2 [=y]) && SND_SOC [=y]

／*AudioSystem Version 2 For Ingenic SOCs　支持X2000_V12*／
Prompt: Audio Version:
  Location:
    -> Device Drivers
      -> Sound card support (SOUND [=y])
        -> Advanced Linux Sound Architecture (SND [=y])
          -> ALSA for SoC audio support (SND_SOC [=y])
            -> ASoC support for Ingenic (SND_ASOC_INGENIC [=y])
  Defined at sound/soc/ingenic/Kconfig:15
  Depends on: SOUND [=y] && !M68K && !UML && SND [=y] && SND_SOC [=y] && SND_ASOC_INGENIC [=y]
  Selected by: SOUND [=y] && !M68K && !UML && SND [=y] && SND_SOC [=y] && SND_ASOC_INGENIC [=y] && m

/* 选择板级　*/
Prompt: SOC x2000 v12 codec Type select
  Location:
    -> Device Drivers
      -> Sound card support (SOUND [=y])
        -> Advanced Linux Sound Architecture (SND [=y])
          -> ALSA for SoC audio support (SND_SOC [=y])
            -> ASoC support for Ingenic (SND_ASOC_INGENIC [=y])
              -> Ingenic Board Type Select
  Defined at sound/soc/ingenic/Kconfig:125
  Depends on: SOUND [=y] && !M68K && !UML && SND [=y] && SND_SOC [=y] && SND_ASOC_INGENIC [=y] && SOC_X2000_V12 [=y] && \
SND_ASOC_INGENIC_AS_V2 [=y]
  Selected by: SOUND [=y] && !M68K && !UML && SND [=y] && SND_SOC [=y] && SND_ASOC_INGENIC [=y] && SOC_X2000_V12 [=y] && \
SND_ASOC_INGENIC_AS_V2 [=y] && m
```

# 三、用户空间
## 1. 设备节点
```
/dev/snd/
controlC0 pcmC0D0p pcmC0D1p pcmC0D2p pcmC0D3p pcmC0D4p pcmC0D5c pcmC0D6c pcmC0D7c pcmC0D8c pcmC0D9c timer
10个pcmC0D*设备对应硬件的10个DMA设备
```
## 2. 应用程序
```
manhatton工程中的buildroot编译
alsa-utils/
alsa-lib/
```

## 3. 操作方法

#### 1.1 baic0+icode录放音

###### 1、baic0+icodec录音，amixer命令配置BAIC0\_CAPTURE\(LI2\)和DM6\(LO6\_MUX\)数据通路连接,arecord命令录音

```
amixer cset name='LO6_MUX' LI2
arecord -D hw:0,6 -c 1 -f S32_LE -r 16000  -d 5 baic0_16000-32-1.wav
arecord -D hw:0,6 -c 1 -f S16_LE -r 48000  -d 5 baic0_48000-16-1.wav
```

###### 2、baic0+icodec放音，amixer命令配置BAIC0\_PLAYBACK\(LO0\_MUX \)和DMA0\(LI8\)数据通路连接,aplay命令放音

```
amixer cset name='LO0_MUX' LI8
aplay -D hw:0,0 16000-32-1.wav
```

###### 3、调整icodec录放音增益

```
amixer  cset name='ICODEC MICL GAIN' 31
amixer  cset name='ICODEC HPOUTL GAIN' 31
```

#### 1.2 baic1录放音

###### **1、baic1录音，amixer命令配置BAIC1\_CAPTURE\(LI3\)和DMA7\(LO7\_MUX\)数据通路连接,arecord命令录音**

```
amixer cset name='LO7_MUX' LI3
arecord -D hw:0,7 -c 2 -f S32_LE -r 48000  -d 10 baic1_48000-32-2.wav
arecord -D hw:0,7 -c 2 -f S16_LE -r 16000  -d 10 baic1_16000-16-2.wav
```

###### **2、baic1放音，amixer命令配置BAIC1\_PLAYBACK\(LO1\_MUX \)和DMA1\(LI9\)数据通路连接,aplay命令放音**

```
amixer cset name='LO1_MUX' LI9
aplay -D hw:0,1 48000-16-2.wav
```

#### 1.3 baic2录音

###### **1、baic2录音，amixer命令配置BAIC2\_CAPTURE\(LI4\)和DMA8\(LO8\_MUX\)数据通路连接,arecord命令录音**

```
amixer cset name='LO8_MUX' LI4
arecord -D hw:0,8 -r 48000 -f S32_LE -c 8 -d 8 baic2_48000-32-8.wav
```

#### 1.4 baic3放音

###### **1、baic3放音，amixer命令配置BAIC3\_PLAYBACK\(LO3\_MUX \)和DMA2\(LI10\)数据通路连接,aplay命令放音**

```
amixer cset name='LO3_MUX' LI10
aplay -D hw:0,2 48000-32-8.wav
```

#### 1.5 baic4录放音

略

#### 1.6 dmic录音

###### **1、dmic录音，amixer命令配置DMIC\_CAPTURE\(LI0\)和DMA5\(LO5\_MUX\)数据通路连接,**_**dmic只能和DMA5连接成数据通路**_**，arecord命令录音**

```
amixer  cset name=LO5_MUX LI0
arecord -D hw:0,5 -c 2 -f S16_LE -r 16000  -d 10 dmic_16000-16-2.wav
arecord -D hw:0,5 -c 4 -f S16_LE -r 48000  -d 10 dmic_48000-16-4.wav
arecord -D hw:0,5 -c 6 -f S16_LE -r 16000  -d 10 dmic_16000-16-6.wav
arecord -D hw:0,5 -c 8 -f S16_LE -r 16000  -d 10 dmic_16000-16-8.wav
```

###### **2、调整dmic录音增益**

```
amixer  cset name='DMIC Volume' 31
```

#### 1.7 spdif录放音

###### **1、spdif放音，amixer命令配置SPDIF\_PLAYBACK\(LO2\_MUX \)和DMA4\(LI12\)数据通路连接,aplay命令放音**

```
amixer cset name='LO2_MUX' LI12
aplay -D hw:0,4 48000-16-2.wav
```

###### **2、spdif录音，amixer命令配置SPDIF\_CAPTURE\(LI1\)和DMA9\(LO8\_MUX\)数据通路连接,arecord命令录音**

```
amixer cset name='LO9_MUX' LI1
arecord -D hw:0,9 -c 2 -f S16_LE -r 48000 -d 10 spdif_48000-16-2.wav
```

## 4. 使用注意事项

1、录音位宽为24bit时，录音数据在内存用32bit存放，有效数据存放在低24bit，使用上位机播放时需要预处理，如果使用x2000 audio设备播放不需要预处理。

2、spdif 录音数据由音频数据和音频信息组成，播放前需要预处理，spdif具体格式请阅读PM手册。

## 5. 开发板支持的功能

| 子设备 | baic0+icodec | baic1 | baic2 | baic3 | baic4 | dmic | spdif |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| zebra | 支持 | 支持 | 支持 | **支持** | 支持 | 支持 | 支持 |
| hallley5\_v20 | 支持 |  |  |  |  | 支持 |  |



