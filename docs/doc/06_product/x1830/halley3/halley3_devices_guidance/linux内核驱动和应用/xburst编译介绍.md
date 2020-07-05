# Xburst 板级介绍

在发布的内核版本中，针对不同的芯片型号，会在arch/mips/xburst目录下进行添加，该目录基本介绍如下：

```
common/ #所有芯片公共部分
core/ # xburst 核心文件
Kconfig
lib/
Makefile
Platform
soc-m200/  #m200 系列板级
soc-x1000/ #x1000 系列板级
soc-x1830/ #其板级信息定义在ingenic/路径下
```

以halley3开发板为例，其板级定义在ingenic/x1830/boards/halley3

```
该目录重要文件介绍如下：

├── common
│   ├── 43438_bt_power_control.c        #蓝牙电源管理
│   ├── 43438_wlan_device.c
│   ├── 43438_wlan_power_control.c      #wifi电源管理
│   ├── i2c_bus.c                       #内核i2c设备描述
│   ├── lcd                             #不同LCD屏幕参数配置
│   │   ├── lcd-hy035jt.c
│   │   ├── lcd-kd035c10hc010a.c
│   │   ├── lcd-vs035hst31c1.c    
│   │   └── Makefile
│   ├── Makefile
│   ├── pwm_generic.c                   #pwm的定义
│   ├── rtc.c
├── halley3_v11
│   ├── board.h                         #所有使用的 GPIO 定义。
│   ├── Makefile
│   ├── pm.c
├── Kconfig
└── Makefile



其他相关信息定义在ingenic/x1830/boards/common

├── board_base.c                        #各个platform_device注册
├── board_base.h
├── Makefile
├── misc.c
├── mmc.c                               #sd控制器设备描述
├── sensor_board.c                      #camera 定义
├── sfc_bus.c
├── sfc_nor_table.h
├── speaker.c
├── spi_bus.c                           #内核spi设备描述
```



