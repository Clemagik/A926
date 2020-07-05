# boot配置


　　Linux 内核需要 U-Boot 来引导。U-Boot 是为嵌入式平台提供的开放源代码的引导程序,它提供串行口、以太网等多种下载方式,提供 NOR 和 NAND 闪存和环境变量管理等功能,支持网络协议栈、JFFS2/EXT2/FAT 文件系统,同时还支持多种设备驱动如 MMC/SD 卡、USB 设备、LCD 驱动等。

## uboot编译

在进行此步骤前，请确保已经正确配置好交叉编译环境。
针对不同开发板的配置，uboot的编译配置也不相同，在发布的uboot中，编译配置由开发板型号[BOARD_NAME]，内核镜像格式[IMAGE_FMT]和启动方式[BOOT]组成，格式如下

> [BOARD_NAME]_[IMAGE_FMT]_[BOOT]

具体配置在uboot/boards.cfg文件中，可以通过以下命令快速查看开发板支持的编译配置
```
$ cat boards.cfg | grep [BOARD_NAME]
```

根据以上方法，找到对应开发板的编译选项按照以下方式进行编译：
```
$ make distclean
$ make [BOARD_NAME]_[IMAGE_FMT]_[BOOT]
```

例如开发板halley5的编译配置如下：

> halley5_uImage_msc2    支持sd卡启动uImage的配置

> halley5_uImage_sfc_nand　支持spi nand flash启动uImage的配置

```
$ make distclean
$ make halley5_uImage_sfc_nand
```
编译完成后会在当前目录下生成u-boot-with-spl.bin文件。即最终烧录所需的uboot文件。


## uboot常用命令

打开调试串口，在uboot启动过程中，敲击任意按键，打断uboot引导镜像过程，进入uboot shell环境，uboot常用命令如下：


> “help”命令:该命令查看所有命令,其中“help command”查看具体命令的格式。

> “printenv”命令:该命令查看环境变量。

> “setenv”命令:该命令设置环境变量。

> “saveenv”命令:该命令保存环境变量。

> “bootp”命令:该命令动态获取 IP。

> “tftpboot”命令:该命令通过 TFTP 协议从网络下载文件运行。

> “bootm”命令:该命令从 memory 运行 u-boot 映像。

> “go”命令:该命令从 memory 运行应用程序。

> “boot”命令:该命令运行 bootcmd 环境变量指定的命令。

> “reset”命令:该命令复位 CPU。

> “md”命令:显示内存数据。

> “mw”命令:修改内存数据。

> “cp”命令:内存拷贝命令。


sfc nor命令：

> “sfcnor read” 从spi nor flash 中读取数据到内存。

> “sfcnor write” 从内存中写数据到spi nor flash。

> “sfcnor erase” spi nor flash擦出。



Sd卡命令：

> “mmc read” 从sd卡中读取数据。

> “mmc write” 写数据到sd卡。


## uboot通过spi flash加载内核镜像

1. 一种方法，可以在uboot运行的时候，进入uboot shell，修改bootcmd变量如下：
    ```
    halley5# set bootcmd 'sfcnand read 0x100000 0x400000 0x80600000 ;bootm 0x80600000'
    halley5# saveenv
    ```

2. 另一种方法，修改uboot源码，include/configs/[BOARD_NAME].h

    例如修改include/configs/halley2.h文件中的
    ```
    #define CONFIG_BOOTCOMMAND "sfcnand read 0x100000 0x400000 0x80600000 ;bootm 0x80600000"
    ```
注意需要编译支持spi nand flash启动的uboot。



## uboot通过tftp加载内核镜像

1. PC服务器端环境配置
    ```
    $ sudo apt-get install tftpd-hpa
    $ sudo service tftpd-hpa start      #在 /var/lib/tftpboot下的文件都可以通过 TFTP协议下载，可以通过tftp客户端程序测试tftp服务器是否可以访问。
    $ sudo apt-get install tftp-hpa			#安装客户端程序
    $tftp 127.0.0.1
    tftp> get test.txt
    ```

2. 开发板端配置

    进入uboot环境，根据网络情况设置参数，将需要的uImage放入上述服务器文件夹下执行以下命令将uImage下载到内存0x80800000位置，并启动tftp服务，根据实际的ip情况，设置uboot各个环境变量如下：
    ```
    $set ipaddr 192.168.4.145
    $set serverip 192.168.4.146
    $tftp 0x80800000 uImage
    $bootm 0x80800000
    ```


## uboot通过fastboot命令加载内核镜像
Fastboot是使用USB端口把uImage下载到内存并启动kernel命令，只用于调试kernel镜像不支持android fastboot 其他功能。

使用方法：

1. PC端使用以下命令安装fastboot应用程序
    ```
    $ sudo apt-get install android-tools-fastboot
    ```

2. 进入开发板调试串口，在uboot启动过程中，敲击任意按键，打断uboot引导镜像过程，进入uboot shell模式，输入#fastboot命令
    ```
    #fastboot
    ```
3. 在PC端存放kernel镜像文件（uImage）的路径下
    ```
    $ sudo fastboot boot uImage
    ```

4. 命令执行成功后，即可下载kernel镜像（uImage）到内存并启动kernel

>


## uboot挂载文件系统

在进行开发时，可以将文件系统存放在不同的介质中，比如nfs网络文件系统，spi nor flash jffs2文件系统， SD卡 ext4文件系统等等。针对不同的文件系统，uboot需要向内核传递参数，即设置bootargs环境变量，在内核启动的时候，会根据参数去挂载相应的文件系统。

### 挂载Jffs2文件系统

```
#set bootargs 'console=ttyS3,115200 mem=128M@0x0 ip=off init=/linuxrc rootfstype=jffs2 root=/dev/mtdblock2 rw '
#saveenv

```

### 挂载ubi文件系统
```
#set bootargs 'console=ttyS3,115200 mem=128M@0x0 ip=off init=/linuxrc ubi.mtd=2 root=ubi0:rootfs ubi.mtd=3 rootfstype=ubifs rw'
#saveenv
```

### 挂载ext4文件系统
```
#set bootargs 'console=ttyS3,115200 mem=128M@0x0 rootfstype=ext4 root=/dev/mmcblk0p7 rootdelay=3 rw'
#saveenv
```

### 挂载网络文件系统
1. pc端配置

    挂载网络文件系统，需要在PC端安装NFS服务器，并且已经将其导出。以ubuntu为例，设置如下：
    ```
    安装nfs服务器：
    $ sudo apt-get install nfs-kernel-server

    导出/home/user/nfs_root文件夹
    $ sudo vi /etc/exports
    添加以下行：
        /home/user/nfs_root	*(rw,insecure,no_root_squash,no_subtree_check)

    启动nfs服务
    $ sudo /etc/init.d/nfs-kernel-server restart

    ```
    测试nfs服
    ```
    mount 192.168.4.146:/home/user/nfs_root/ /mnt
    如果mnt下的文件和/home/user/nfs_root目录下文件一致说明nfs服务器安装成功
    ```
2. 开发板端配置
    具体ip地址以实际开发环境为准。注意内核需要选中nfs网络文件系统支持，才可以通过网络挂载文件系统。
    ```
    #set bootargs 'console=ttyS3,115200 mem=128M@0x0 ip=192.168.4.145:192.168.4.1:192.168.4.1:255.255.255.0 nfsroot=192.168.4.146:/home/nfsroot/fpga/user/nfs_root rw'
    #saveenv
    ```

> 注意：使用kernel3.10 camera功能时，需要修改bootargs的mem配置为： mem=96M@0x0 rmem=32M@0x6000000


## log打印级别loglevel

log打印共有8个级别，从0~7依次递增，0为最低级别，7为最高级别，相关描述文件在以下路径：kernel/Documentation/kernel-parameters.txt


![](/assets/loglevel.png)

在板级头文件定义中有如下的信息（以halley5开发板为例）：

板级文件路径：u-boot/include/configs/halley5.h
```
#define BOOTARGS_COMMON "console=ttyS3,115200 mem=128M@0x0 loglevel=7"

```
