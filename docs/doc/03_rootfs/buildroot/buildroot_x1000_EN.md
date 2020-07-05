# rootfs for x1000

## version
* X1000 SDK version： ingenic-linux-kernel3.10.14-x1000-v8.0-20180316 
* buildroot version: **buildroot-2017.02.** 
> download buildroot-2017.02.tar.bz2 from:	https://buildroot.org/downloads/ 

## build step:
```
	$ tar xjf buildroot-2017.02.tar.bz2 
	$ cp -afr buildroot_ingenic_x1000_patch/* buildroot-2017.02/ 
	$ cd buildroot-2017.02 
	$ make ingenic_x1000_defconfig 
	$ make  # Waiting... 
```

* When build finish, The target rootfs is: 
```
buildroot-2017.02$ ll output/images/ 
-rw-r--r-- 1 lgwang sw1 20776960  5月  4 17:36 rootfs.tar 
-rw-r--r-- 1 lgwang sw1  7751296  5月  4 17:36 rootfs.tar.gz 
```

* make jffs2 fs image: 
```
	$ mkfs.jffs2 -e 0x8000 -p 0xc80000 -d output/target/ -o output/images/system.jffs2 
```
---

* Attached: ingenic x1000 patch files: 
```
$ tree buildroot_ingenic_x1000_patch/ 
buildroot_ingenic_x1000_patch/ 
|-- configs 
|   `-- ingenic_x1000_defconfig 
`-- system 
    `-- ingenic 
        `-- common 
            `-- etc 
                |-- init.d 
                |   `-- rcS 
                `-- inittab 
6 directories, 3 files 
```
