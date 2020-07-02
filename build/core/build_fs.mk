
define build_filesystem_to_jffs2
fakeroot mkfs.jffs2 -e $(ROOTFS_JFFS2_NORFLASH_ERASESIZE) \
			-p $(ROOTFS_JFFS2_SIZE) \
			-d $(TARGET_FS_BUILD) \
			-o $@
endef
define build_filesystem_to_ubi
fakeroot mkfs.ubifs -e $(ROOTFS_UBIFS_LEBSIZE) \
			-c $(ROOTFS_UBIFS_MAXLEBCNT) \
			-m $(ROOTFS_UBIFS_MINIOSIZE) \
			-d $(TARGET_FS_BUILD) \
			-F		\
			-o $@
endef

define build_filesystem_to_ext4
fakeroot $(MKEXT4)  -N $(ROOTFS_EXT4_INODES)  \
			-b $(ROOTFS_EXT4_BLOCK) \
			-d $(TARGET_FS_BUILD) \
			-o $@
endef

define build_filesystem_to_cramfs
mkfs.cramfs $(TARGET_FS_BUILD) $@
endef
