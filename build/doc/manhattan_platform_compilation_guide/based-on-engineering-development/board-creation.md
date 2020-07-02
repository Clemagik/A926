## 4.1 **创建板级**

### 4.1.1 **创建板级目录**

> 进入工程device目录，然后创建以板级名称命名的目录，以phoenix为例。

```
$ mkdir phoenix
```

### 4.1.2 **创建板级目录下的存储介质目录**

> 在创建完板级目录以后，需要在板级目录下创建下存储介质目录，如phoenix用norflash的介质，就创建一个目录叫norflash。

```
$ mkdir norflash
```

### 4.1.3 **创建不同的model**

> 进入板级存储介质目录下，新建model1目录和model2目录（至少要有model1目录），并且要分别在model1目录和model2目录下新建一个文件为vendorsetup.sh。

```
$ mkdir norflash
$ vim vendorsetup.sh
```

_**注：其中model1表示的模式为normal模式，在命名的时候也可以根据需要命名为其他名称，但是要与device.mk中的MODEL指定的名称保持一致。model2表示的模式为ota模式，名称也可以根据需要命名**_

### 4.1.4 **编辑板级device.mk**

> 创建好板级目录以后，要在板级目录下新建并编辑device.mk文件。在编辑板级的device.mk时，其中要指定uboot，kernel编译路径，kernel编译默认配置文件，kernel生成的镜像名，uboot编译生成的镜像名以及文件系统类型等信息。以下对其中主要的宏进行解释。

```
UBOOT_BUILD_DIR                #需要编译的uboot目录
KERNEL_BUILD_DIR               #需要编译的kernel目录
KERNEL_COFIG_FILE              #编译kernel时make munuconfig所需要的配置文件
MODEL                          #指定的默认编译model

×编译多线程配置：
MAKE_JLEVEL := 4               #配置编译时用几个进程

×硬件配置：
TARGET_BOOTLOADER_BOARD_NAME   #需要编译的kernel目录
TARGET_PRODUCT_BOARD
TARGET_BOARD_PLATFORM          #该板级属于的平台，如phoenix为X1000
TARGET_BOARD_ARCH              #体系结构，mips

×设备目录文件配置：
UBOOT_BUILD_CONFIG             #编译uboot需要的配置文件名
KERNEL_BUILD_CONFIG            #编译kernel需要的配置文件名
KERNEL_TARGET_IMAGE            #编译kernel的类型，如uImage
KERNEL_IMAGE_PATH              #生成的kernel镜像存放目录
UBOOT_TARGET_FILE              #生成的uboot镜像文件名
FILE_SYSTEM_TYPE               #文件系统类型（目前只支持jffs2，ubi，ext4这三种文件系统类型）
ROOTFS_JFFS2_NORFLASH_ERASESIZE:= 0x8000     #文件系统flash擦除大小
ROOTFS_JFFS2_SIZE:=  0xc80000                #文件系统大小
MODEL                                        #文件系统模式（model1或者 model2）
```

* 示例一：

  本示例为工程device/phoenix/device.mk

```
PRODUCT:= product
UBOOT_BUILD_DIR := $(TOPDIR)u-boot
KERNEL_BUILD_DIR := $(TOPDIR)kernel
KERNEL_COFIG_FILE := $(KERNEL_BUILD_DIR)/.config
MODEL := model1

################### make -j config  #######################
MAKE_JLEVEL := 4

############### hardware config #########################
TARGET_BOOTLOADER_BOARD_NAME:=$(TARGET_DEVICE)
TARGET_PRODUCT_BOARD:=$(TARGET_DEVICE)
TARGET_BOARD_PLATFORM:= "x1000"
TARGET_BOARD_ARCH:="mips"

################  target_device config ###################
ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),norflash)
#nor flash config
UBOOT_BUILD_CONFIG := phoenix_v10_uImage_sfc_nor
KERNEL_BUILD_CONFIG := phoenix_linux_defconfig
KERNEL_TARGET_IMAGE :=uImage
KERNEL_IMAGE_PATH:=arch/mips/boot

UBOOT_TARGET_FILE:= u-boot-with-spl.bin
FILE_SYSTEM_TYPE:=jffs2
ROOTFS_JFFS2_NORFLASH_ERASESIZE:= 0x8000
ROOTFS_JFFS2_SIZE:=  0xc80000

ifeq ($(strip $(TARGET_EXT_SUPPORT)),ota) #OTA
OTA:=y
MODEL:=model2
FILE_SYSTEM_TYPE:=cramfs

UBOOT_BUILD_CONFIG := phoenix_v10_xImage_sfc_nor
KERNEL_IMAGE_PATH := arch/mips/boot/compressed
KERNEL_TARGET_IMAGE := xImage
endif #OTA
endif #norflash

ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),spinand)
#nand flash config
UBOOT_BUILD_CONFIG :=  phoenix_v10_uImage_sfc_nand
KERNEL_BUILD_CONFIG := phoenix_linux_sfcnand_ubi_defconfig
KERNEL_TARGET_IMAGE :=uImage
KERNEL_IMAGE_PATH:=arch/mips/boot

UBOOT_TARGET_FILE:= u-boot-with-spl.bin

FILE_SYSTEM_TYPE:=ubi
ROOTFS_UBIFS_LEBSIZE   := 0x1f000
ROOTFS_UBIFS_MAXLEBCNT := 2048
ROOTFS_UBIFS_MINIOSIZE := 0x800

endif #spinand
```

### 4.1.5 **新建并编辑板级\_base.mk文件**

> 板级\_base.mk文件中包含的模块是板级所有编译模式（eng，user，userdebug）都需要编译的基本包模块。并且LOCAL\_MODULE\_TAGS为optional的模块添加到板级\_base.mk才会被编译（[LOCAL\_MODULE\_TAGS详情请参考4.2.1 Lunch选项详解](#_Lunch选项详解)）。
>
> 在device/板级/目录，如phoenix\_base.mk。把需要添加的编译到产品文件系统的模块名写到变量PRODUCT\_PACKAGES中。以phoenix为例：

* 示例一：

  本示例为工程device/phoenix/phoenix\_base.mk

```
RUNTESTDEMO_UTILS := RuntestScript AmicScript WificonnectScript bash CleanfileScript \
                DmicScript TfcardcopyScript
CAMERA_UTILS := CameraScript cimutils
PRODUCT_MODULES +=  $(RUNTESTDEMO_UTILS)\
            $(CAMERA_UTILS)\
```

**注：该文件中添加的模块必须是LOCAL\_MODULE\_TAGS为optional的模块，否则即使在PRODUCT\_MODULES宏中的模块也不会被编译。**

