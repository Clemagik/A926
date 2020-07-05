
# x1500 yak


## 1. make all build

### config for yak

device/halley2/device.mk
>
UBOOT_BUILD_CONFIG := yak_xImage_nor_spl_boot
KERNEL_BUILD_CONFIG := yak_linux_defconfig
KERNEL_TARGET_IMAGE :=xImage
KERNEL_IMAGE_PATH:= arch/mips/boot/compressed


### build command

```
$ source build/envsetup.sh
$ lunch halley2_norflash-eng
$ make

### Target images:
X1000_SDK_V8.0-20180316$ ll out/product/halley2/image/
total 14124
-rw-r--r-- 1 lgwang sw1 12255232 11月  8 08:52 system.jffs2
-rw-r--r-- 1 lgwang sw1   194200 11月  8 08:52 u-boot-with-spl.bin
-rw-r--r-- 1 lgwang sw1  2007104 11月  8 08:52 xImage
```

## 2. rebuild

### rebuild u-boot

```
$ cd u-boot/
$ make distclean; make yak_xImage_nor_spl_boot
### Target is: u-boot-with-spl.bin
```

### rebuild kernel

```
$ cd ../kernel
$ make yak_linux_defconfig
$ make xImage
### target is: arch/mips/boot/compressed/xImage
```
