## 1.简介

> 为了使manhattan平台编译变得整洁、方便，现添加新的编译规则来规范编译环境及板级创建。

## 2.添加新的的板级

> 执行下面的命令：

```
$ mnewdevice 板级名称     #创建板级 （路径：device/板级）
```

---

* 示例一

> 本示例是在device下创建要添加的板级（myboard）

```
$ mnewdevice myboard
```

> 然后根据提示信息添加自己的配置信息：

```
******************** Attention **********************
Need to below such contents to construct a minimal device.
device/myboard/Build.mk-5-include $(CLEAR_VARS)
device/myboard/Build.mk:6:UBOOT_BUILD_CONFIG:=uboot_template_xImage_msc0
--
device/myboard/Build.mk-20-KERNEL_PATH=$(TOP_DIR)/kernel
device/myboard/Build.mk:21:KERNEL_BUILD_CONFIG:=template_kernel_defconfig
--
device/myboard/Build.mk-36-include $(CLEAR_VARS)
device/myboard/Build.mk:37:UBOOT_BUILD_CONFIG:=uboot_template_xImage_msc0
--
device/myboard/Build.mk-52-KERNEL_PATH=$(TOP_DIR)/kernel
device/myboard/Build.mk:53:KERNEL_BUILD_CONFIG:=template_kernel_defconfig
--
device/myboard/Build.mk-68-include $(CLEAR_VARS)
device/myboard/Build.mk:69:UBOOT_BUILD_CONFIG:=uboot_template_xImage_msc0
--
device/myboard/Build.mk-83-KERNEL_PATH=$(TOP_DIR)/kernel
device/myboard/Build.mk:84:KERNEL_BUILD_CONFIG:=template_kernel_defconfig
--
device/myboard/device.mk-2-TARGET_PRODUCT_BOARD:=$(TARGET_DEVICE)
device/myboard/device.mk:3:TARGET_BOARD_PLATFORM:="template_soctype"
--
device/myboard/device.mk-23-#rootfs
device/myboard/device.mk:24:BUILDROOT_CONFIG:=template_buildroot_defconfig
--
device/myboard/device.mk-31-ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),nor)
device/myboard/device.mk:32:BUILDROOT_CONFIG:=template_buildroot_defconfig
--
device/myboard/device.mk-41-ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),nand)
device/myboard/device.mk:42:BUILDROOT_CONFIG:=template_buildroot_defconfig
*****************************************************

device: [myboard] created under device/myboard

device/myboard
├── Build.mk
├── device.mk
├── myboard_base.mk
├── rootfs-overlay
│   └── empty
└── vendorsetup.sh

1 directory, 5 files
```

### 2.1板级下Build.mk构建

> * 核心编译宏：使用include引入如下宏（实为.mk文件）完成模块的编译工作。

```
BUILD_KERNEL        #编译kernel对应的规则
BUILD_UBOOT         #编译uboot对应的规则
```

> * 接口宏

```
UBOOT_BUILD_CONFIG    #编译uboot所需要的配置文件
UBOOT_TARGET_FILE     #生成的uboot的镜像文件名
LOCAL_MODULE          #模块名
KERNEL_BUILD_CONFIG   #编译kernel时make munuconfig所需要的配置文件
UBOOT_PATH            #uboot的路径
KERNEL_TARGET_IMAGE   #编译kernel的类型，如：xImage
```

* 示例一

> 本示例路径device/halley3/Build.mk

```
ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),nand)

# **** uboot ****
LOCAL_PATH :=$(my-dir)
include $(CLEAR_VARS)
UBOOT_BUILD_CONFIG:=halley3_xImage_sfc_nand
UBOOT_TARGET_FILE:=u-boot-with-spl.bin
LOCAL_MODULE := uboot
UBOOT_PATH := $(TOP_DIR)/u-boot
LOCAL_MODULE_TAGS := optional
include $(BUILD_UBOOT)

# copy above to add a new uboot config.

# **** end uboot ****

# *** kernel ****
include $(CLEAR_VARS)
KERNEL_BUILD_CONFIG:=halley3_v11_sfcnand_ubi_defconfig
#KERNEL_CONFIG_PATH:=$(DEVICE_PATH)
LOCAL_MODULE=kernel
KERNEL_IMAGE_PATH:=arch/mips/boot/compressed
KERNEL_PATH=$(TOP_DIR)/kernel
KERNEL_TARGET_IMAGE:= xImage
LOCAL_MODULE_TAGS :=optional
include $(BUILD_KERNEL)
# copy above to add a new kernel config.

# *** end kernel ****
endif # end msc
```

## 3.配置device.mk中的buildroot

> 创建好板级目录以后，会在板级目录下生成device.mk文件。在编辑板级的device.mk时，其中要指定buildroot编译路径，buildroot编译默认配置文件及文件系统类型等信息。以下对其中主要的宏进行解释。

```
BUILDROOT_CONFIG:=    #编译buildroot需要的配置文件名
BUILDROOT_PATH  :=    #buildroot的文件名
ROOTFS_OVERLAY_DIR：=  #rootfs-overlay的路径
FILE_SYSTEM_TYPE:=    #文件系统类型（目前只支持jffs2，ubi，ext4这三种文件系统类型）
```

---

* 示例一

> 本示例的路径为device/halley3/device.mk

```
ifeq ($(strip $(TARGET_STORAGE_MEDIUM)),nand)
BUILDROOT_CONFIG:=halley3_v11_defconfig
#BUILDROOT_CONFIG_PATH:=$(DEVICE_PATH)
BUILDROOT_PATH:=buildroot
ROOTFS_OVERLAY_DIR:=$(DEVICE_PATH)/rootfs-overlay
FILE_SYSTEM_TYPE:=ubifs

endif #end nand
```

## 4.工程编译

> 在新的编译规则下，我们不需要再进入对应的文件（eg：kernel）中更改编译信息，从而提高编译的效率。

---

* 示例一

> 本示例是针对kernel的编译配置，在顶层目录执行下面命令进行编译配置：

```
$ make kernel-menuconfig   #根据用户需求添加或取消自己的编译配置
$ make kernel              #编译kernel
```

* 示例二

> 本示例是针对buildroot的编译配置，在顶层目录执行下面命令进行编译配置：

```
$ make buildroot-menuconfig #根据据用户需求添加或取消自己的应用
$ make buildroot            #编译buildroot
```

## 5.添加Cmake模块

> 在添加Cmake模块时，首先要确保模块能够正常编译通过，再添加到platform上，在添加放置Cmake功能模块时，建议放置到packages目录下，并参照packages目录下各个的Build.mk文件编写所添模块的Build.mk文件，对所添模块进行指导性编译，如下几个宏对Cmake的编译提供支持。

---

> * 核心编译宏：使用include引入如下宏（实为.mk文件）完成模块的编译工作。

`BUILD_CMAKE_DEVICE    #负责Cmake模块的编译，参见示例一`

> * 接口宏：

```
CMAKE_PATH                      #模块的路径
LOCAL_MODULE                    #模块名
LOCAL_MODULE_TAGS               #模块所属开发模式
LOCAL_DEPANNER_MODULES          #依赖的模块
```

* 示例一

> 本示例为工程packages/kunpeng/network/Build.mk

```
LOCAL_PATH := $(my-dir)

include $(CLEAR_VARS)
CMAKE_PATH=$(LOCAL_PATH)/utils/debug_syslog
LOCAL_MODULE:=debug_syslog
LOCAL_MODULE_TAGS :=optional
include $(BUILD_CMAKE_DEVICE)

include $(CLEAR_VARS)
CMAKE_PATH=$(LOCAL_PATH)/network-manager-app
LOCAL_MODULE:=network_manager
CMAKE_CONF_OPTS:=-DBUILD_libNetworkConnectionManager=ON -DBUILD_NetworkConnectionManagerDemo=ON
LOCAL_DEPANNER_MODULES=debug_syslog
LOCAL_MODULE_TAGS :=optional
include $(BUILD_CMAKE_DEVICE)

include $(CLEAR_VARS)
CMAKE_PATH=$(LOCAL_PATH)/utils
LOCAL_MODULE:=network_utils
LOCAL_DEPANNER_MODULES=network_manager
LOCAL_MODULE_TAGS :=optional
include $(BUILD_CMAKE_DEVICE)

include $(CLEAR_VARS)
CMAKE_PATH=$(LOCAL_PATH)/utils/event_manager
LOCAL_MODULE:=event_manager
LOCAL_MODULE_TAGS :=optional
include $(BUILD_CMAKE_DEVICE)

include $(CLEAR_VARS)
CMAKE_PATH=$(LOCAL_PATH)/bluetooth
LOCAL_MODULE:=bluetooth
LOCAL_DEPANNER_MODULES=event_manager network_manager
LOCAL_MODULE_TAGS :=optional
include $(BUILD_CMAKE_DEVICE)
```

## 6.如何添加buildroot应用

> 以在buildroot中添加自己的isp\_demo应用为例，在halley3\_v11\_defconfig的项目下，加入isp\_demo及依赖的库libisp到package，将isp\_demo代码的源码编译生成到rootfs根文件系统中。
>
> 1.在buildroot/ingenic/下添加isp\_demo源码及依赖的库文件libisp

```
$ mkdir isp_demo
$ ls isp_demo      #以下是添加的源文件及相应的Makefile
  image_convert.c  image_convert.h  isp_demo.c  Makefile

$ mkdir libisp
$ ls libisp        #以下是isp_demo依赖的库文件
  include  libisp_520.so  libisp.so
```

> 2.在buildroot/package/ingenic下创建isp\_demo文件夹

```
$ mkdir isp_demo
```

> 3.在isp\_demo文件夹下创建如下的文件

```
$ touch Config.mk
$ touch isp_demo.mk
```

**注意：文件名很关键，isp\_demo.mk要小写，不能乱起名字，因为buildroot框架有一套根据命名规则。**

> 4.在package/Config\_in中添加如下内容：

```
source "package/ingenic/isp_demo/Config.in"
```

> 5.在package/ingenic/isp\_demo/Config.in的内容如下：

```
config BR2_PACKAGE_ISP_DEMO
        bool "isp demo"
        select BR2_PACKAGE_LIBISP
        help
           isp demo
```

**注意：Packages应用一定要以BR2\_PACKAGE\_作为开头，以isp\_demo大写来填充这个变量，这样才能被buildroot命名框架识别、展开，才能通过make menuconfig配置。**

> 6.在package/ingenic/isp\_demo/isp\_demo.mk的内容如下：
>
> * 以下是对应变量的说明（以isp\_demo为例）：
>
> > 1. 所有的变量都是以ISP\_DEMO\_开头的, 这样buildroot才能通过命名框架去解析；
> > 2. 以\_VERSION结尾的变量是下载isp\_demo源码的版本号；
> > 3. \_SITE\_METHOD结尾的变量是isp\_demo变量的下载方法；
> > 4. \_SIZE结尾的变量是isp\_demo的下载地址；
> > 5. \_DEPENDENCIES结尾的变量是isp\_demo依赖的库；
> > 6. 以\_CMDS结尾的变量是构建过程中会被buildroot执行的命令；
> > 7. \_BUILD\_CMDS结尾的函数，在构建过程函数，一般是给isp\_demo传递编译和链接选项，调用源代码中的Makefile执行编译；
> >
> > 8. \_INSTALL\_TARGET\_CMDS结尾的函数, 就是isp\_demo编译完成后，自动安装的执行，一般让buildroot编译出来的库或bin文件安装到指定的目录下；
> >
> > 9. $\(eval $\(generic-package\)\)是把整个isp\_demo.mk构建脚本，通过buildroot下的Makefile，生成isp\_demo的构建目标。
>
> * 以下是isp\_demo.mk示例：

```
################################################################################
#
# isp demo
#
################################################################################
ISP_DEMO_VERSION =

ISP_DEMO_SOURCE = isp_demo         
ISP_DEMO_SITE_METHOD = local
ISP_DEMO_SITE = ingenic/isp_demo

ISP_DEMO_INSTALL_STAGING = YES 
ISP_DEMO_INSTALL_TARGET = YES 
ISP_DEMO_DEPENDENCIES = libisp

define ISP_DEMO_CONFIGURE_CMDS
        $(TARGET_CONFIGURE_OPTS) $(TARGET_CONFIGURE_ARGS) $(MAKE) -C $(@D) configure_env
endef

define ISP_DEMO_BUILD_CMDS
    $(TARGET_MAKE_ENV) $(MAKE) -C $(@D)  all
endef

define ISP_DEMO_INSTALL_TARGET_CMDS
        $(TARGET_MAKE_ENV) $(MAKE) -C $(@D) DESTDIR=$(TARGET_DIR) install
endef

$(eval $(generic-package))
```

> 7.同理在package/ingenic/下添加libisp
>
> * 以下是package/Config\_in中添加内容：

```
source "package/ingenic/libisp/Config.in"
```

> * 以下是package/ingenic/libisp/Config.in的内容：

```
config BR2_PACKAGE_LIBISP
        bool "libisp"

        help
          libisp

config BR2_PACKAGE_LIBISP_520
        bool "Select a library file"
        depends on BR2_PACKAGE_LIBISP
        help
          select libisp_520.so
```

> * 以下是libisp.mk的内容：

```
################################################################################
#
# libisp
#
################################################################################
LIBISP_VERSION =
LIBISP_SOURCE = libisp
LIBISP_SITE_METHOD = local
LIBISP_SITE = ingenic/libisp
LIBISP_INSTALL_STAGING = YES 
LIBISP_INSTALL_TARGET = YES 

ifeq ($(BR2_PACKAGE_LIBISP_520),y)
define LIBISP_INSTALL_STAGING_CMDS
        cp $(@D)/libisp_520.so $(STAGING_DIR)/usr/lib/libisp.so -vfr
        cp $(@D)/include/* $(STAGING_DIR)/usr/include/ -vfr
endef

define LIBISP_INSTALL_TARGET_CMDS
        cp $(@D)/libisp_520.so $(TARGET_DIR)/usr/lib/libisp.so -vfr
endef
else
define LIBISP_INSTALL_STAGING_CMDS
        cp $(@D)/libisp.so $(STAGING_DIR)/usr/lib/ -vfr
        cp $(@D)/include/* $(STAGING_DIR)/usr/include/ -vfr
endef

define LIBISP_INSTALL_TARGET_CMDS
        cp $(@D)/libisp.so $(TARGET_DIR)/usr/lib/ -vfr
endef
endif

$(eval $(generic-package))
```

> 8.通过make menuconfig在ingenic packages选项下选择要编译isp\_demo应用及依赖的库libisp

![](/assets/isp_demo.png)

