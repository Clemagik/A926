# rootfs

## readonly rootfs

 * REMOUNT_ROOTFS_RW
`/bin/mount -o remount,rw /`
 * REMOUNT_ROOTFS_RO
 `/bin/mount -o remount,ro /`
 * mount as readonly on bootup:
``` diff
 ### /etc/init.d/rcS
+/bin/mount -o remount,rw / # REMOUNT_ROOTFS_RW
+/bin/mount -o remount,ro / # REMOUNT_ROOTFS_RO
```
 * Add rcS to prbuilt.
X1000 SDK "device/halley2/rootfs_overlay/etc/init.d/rcS"
``` shell
/bin/mknod -m 660 /dev/null c 1 3
/bin/mknod -m 660 /dev/console c 5 1
/bin/mount -t proc proc /proc
/bin/mount -o remount,rw / # REMOUNT_ROOTFS_RW
/bin/mount -a
/bin/mkdir -p /dev/pts
/bin/mount -t devpts devpts /dev/pts
/bin/mkdir  /dev/shm
/bin/mount -t tmpfs tmpfs /dev/shm
/bin/hostname -F /etc/hostname
/bin/mount -t tmpfs tmpfs /var
/bin/mount -t tmpfs tmpfs /run
/bin/mount -o remount,ro / # REMOUNT_ROOTFS_RO
```

## mount data partition
mount data partition
>
device/halley2/rootfs_overlay/etc/init.d/S21mount
device/halley2/rootfs_overlay/usr/bin/mount_data_partition.sh
device/halley2/rootfs_overlay/usr/bin/jffs2mount.sh
device/halley2/rootfs_overlay/usr/bin/ubimount.sh


## spi flash partition
### config file
 * cloner-2.5.4/configs/x1000/x1000_sfc_nor_16mb.cfg

###  partition
 * x1000_sfc_nor_16mb.cfg
``` ini
[spiPartition]
Partition0="uboot,0x0,0x40000,0,MTD_MODE"
Partition1="kernel,0x40000,0x300000,0,MTD_MODE"
Partition2="rootfs,0x360000,0xba0000,0,MTD_MODE"
Partition3="data,0xf00000,-1,0,MTD_MODE"
count=4
```

### download offset
Make sure download offset match partition. For example, the data image download offset should be 0xf00000, same to Partition3="data,0xf00000,-1,0,MTD_MODE"
```
[policy3]
attribute=
enabled=0
label=data
offset=0xf00000
ops="12,6,0"
type=0
```
