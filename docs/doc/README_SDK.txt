
Documents:
	* docs/doc/***

ReleaseNote:
	* docs/doc/ReleaseNote_X1000.txt



================================================================================

## OS requires:
   * Linux OS: ubuntu 14.04 64bit
   * install depended packages:
        $ source build/envsetup.sh
        $ autoenvsetup            ### install packages

## build all:
   * select product:
        $ source build/envsetup.sh
        $ lunch                     ### product 'halley2_nor-eng' (setting path: 'device/halley2/')

>Lunch menu... pick a combo:
	1. halley2_nand-eng
	2. halley2_nor-eng

   * make
        $ make

   * target iamges: out/product/halley2/image/
        system.*
        uboot
        kernel


--------------------------------------------------------------------------------

## build uboot kernel buidroot:
   * build uboot
	$ cd <Manhatton_Project>
        $ make uboot  ### u-boot/boards.cfg
      Target image: 'out/product/halley2/obj/uboot-intermediate/u-boot-with-spl.bin or out/product/halley2/image/uboot'

   * build kernel
        $ cd <Manhatton_Project>
        $ make kernel     ### 'default configuration:halley2_linux_nor_defconfig from kernel/arch/mips/configs/'
      Target image: 'out/product/halley2/obj/kernel-intermediate/arch/mips/boot/uImage or out/product/halley2/image/kernel'

/*******************************************************************/
Tips:
	If your Halley2 Development Kit Version is 'RD_X1000_Halley2_V2.0' use default configuration 'halley2_linux_nor_defconfig'.
	If your Halley2 Development Kit Version is 'RD_X1000_Halley2_V4.1' please open the following configurations in 'device/halley2/Build.mk':


		53 # KERNEL_BUILD_CONFIG:=halley2_linux_nor_defconfig
		54 KERNEL_CONFIG_PATH:=$(DEVICE_PATH)
		55 KERNEL_BUILD_CONFIG =halley2_linux_coreboardv41_nor_defconfig

/*******************************************************************/

   * build buildroot
	$ cd <Manhatton_Project>
	$ make buildroot ### or make rootfs.
	Target image:out/product/halley2/obj/buildroot-intermediate/images/rootfs.*

