# 1.SFC uboot相关配置

## 1.1.源码位置

* spl

```c
u-boot/common/spl/
├── spl_sfc_nand_v2.c
└── spl_sfc_nor_v2.c

u-boot/tools/ingenic-tools/nand_device/ /*各厂商的参数配置文件和一些通用接口*/
```

* uboot

```C
u-boot/drivers/mtd/devices/jz_sfc_v2/
├── jz_sfc_common.c
├── jz_sfc_nand.c
├── jz_sfc_nor.c
├── jz_sfc_ops.c
└── nand_device/    /*各厂商的参数配置文件和一些通用接口*/
```

## 1.2.配置gpio function

* x2000 SFC共支持两组GPIO:

```c
        PD1.8V:
            SFC0：PD17-22 FUNCTION1
            SFC1：PD23-26 FUNCTION1
        PE3.3V:
            SFC0：PE16-21 FUNCTION0
```

注: *PD组只可以单独配置SFC0或者同时配置SFC0、SFC1为8线，SFC1不可单独使用。*

* spl和uboot配置SFC的gpio function，需要修改板级配置文件:

```c
例如:　u-boot/include/configs/halley5.h

修改宏:
    /* sfc gpio */
    /* #define CONFIG_JZ_SFC_PD_4BIT */
    /* #define CONFIG_JZ_SFC_PD_8BIT */
    /* #define CONFIG_JZ_SFC_PD_8BIT_PULL */
    #define CONFIG_JZ_SFC_PE
```

## 1.3.SFC频率

* spl和uboot配置SFC的频率，需要修改板级配置文件:

```c
例如:　u-boot/include/configs/halley5.h

修改宏:
    /*
     * NOR: SFC外部时钟频率　＝　CONFIG_SFC_NOR_RATE/4
     * NAND: SFC外部时钟频率　＝　CONFIG_SFC_NAND_RATE/4
     **/
    #define CONFIG_SFC_NOR_RATE     296000000　　/* value <= 296000000(sfc 74Mhz)*/
    #define CONFIG_SFC_NAND_RATE    296000000　　/* value <= 296000000(sfc 74Mhz)*/
```

## 1.4.配置单线四线

* spl配置单线四线，需要修改板级配置文件

```c
例如:　u-boot/include/configs/halley5.h

修改宏:
    #define CONFIG_SFC_QUAD  /*定义则配置为４线*/
```

* uboot NOR:　配置单线四线，需要配置cloner烧录工具

> `Boot quad`勾选后，NOR Flash uboot启动为四线，否则为单线。

![ ](/assets/cloner-quad-boot.png "cloner select quad boot.")

* uboot NAND:　配置单线四线，需要修改各厂商参数配置文件源码

```c
例如:　u-boot/drivers/mtd/devices/jz_sfc_v2/nand_device/dosilicon_nand.c

修改基础参数:
            [1] = {
        /*DS35Q2GAXXX*/
                .pagesize = 2 * 1024,
                .blocksize = 2 * 1024 * 64,
                .oobsize = 64,
                .flashsize = 2 * 1024 * 64 * 2048,

                .tHOLD  = THOLD,
                .tSETUP = TSETUP,
                .tSHSL_R = TSHSL_R,
                .tSHSL_W = TSHSL_W,

                .tRD = 90,
                .tPP = TPP,
                .tBE = TBE,

                .plane_select = 1,
                .ecc_max = 0x4,
                //.need_quad = 1,  /*配置为４线模式*/
                .need_quad = 0,  /*配置为单线模式*/
        },

```

## 1.5.NAND配置BBP和PPB

* BPP: bytes per page，需要修改配置文件
* PPB: pages per block，需要修改配置文件

```c
例如:　u-boot/include/configs/halley5.h

修改宏:
/* sfc nand config */
#define CONFIG_SPI_NAND_BPP (2048 +64)      /*Bytes Per Page*/
#define CONFIG_SPI_NAND_PPB (64)            /*Page Per Block*/
```

## 1.6.flash中参数位置

* spl和uboot默认将参数烧录到flash的固定位置，启动时去指定位置读参数

```c
例如:　u-boot/include/configs/halley5.h

修改宏:
#define CONFIG_SPIFLASH_PART_OFFSET 0x5800　　/*参数的偏移地址*/

```

# 2.添加一款新的flash参数

## 2.1. 添加NOR的spl和uboot参数(同kenrel)

* 通过cloner烧录工具添加spi nor flash参数:

  1. 打开cloner中的`Config`，在`INFO`菜单下，选择`Board`为“x2000_v12_sfc_nor_lpddr3_linux.cfg”。
  2. 在`SFC`菜单下，选择二级菜单`norinfo`，选择`ADD`。
  3. 在弹出的对话框中按照spi nor flash的参数填上保存即可。

![ ](/assets/cloner_nor_add_params.png "Add nor params.")

## 2.2. 添加NAND的spl和uboot参数

* 添加NAND spl参数

```c
相关代码位置:
u-boot/tools/ingenic-tools/nand_device/
├── ato_nand.c
├── dosilicon_nand.c
├── foresee_nand.c
├── gd_nand.c
├── mxic_nand.c
├── sfc_params.c
├── tc_nand.c
├── winbond_nand.c
├── xtx_mid0b_nand.c
├── xtx_nand.c
└── zetta_nand.c

参照已有的配置添加新型号的flash参数，例如:
u-boot/tools/ingenic-tools/nand_device/dosilicon_nand.c
```

* 添加NAND uboot参数（同kernel）

```c
相关代码位置:
u-boot/drivers/mtd/devices/jz_sfc_v2/nand_device/
├── ato_nand.c
├── dosilicon_nand.c
├── foresee_nand.c
├── gd_nand.c
├── mxic_nand.c
├── nand_common.c
├── xtx_mid0b_nand.c
├── xtx_nand.c
└── zetta_nand.c

参照已有的配置添加新型号的flash参数，例如:
u-boot/drivers/mtd/devices/jz_sfc_v2/nand_device/dosilicon_nand.c
```

# 3.通过spl传参数为bootrom升频

* 在boards.cfg中添加SPL_PARAMS_FIXER配置

```c
例如:
zebra_uImage_sfc_nor   mips    xburst2 zebra   ingenic x2000_v12   zebra:SPL_SFC_NOR,ENV_IS_IN_SFC,MTD_SFCNOR,SPL_PARAMS_FIXER
```

* 可以通过配置PLL，来配置bootrom频率，源码位置如下:

```c
u-boot/tools/ingenic-tools/spl_params_fixer_x2000_v12.c
```
