/*
 * Copyright 2011, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CUTILS_ANDROID_REBOOT_H__
#define __CUTILS_ANDROID_REBOOT_H__

__BEGIN_DECLS

/* Commands */
#define ANDROID_RB_RESTART  0xDEAD0001
#define ANDROID_RB_POWEROFF 0xDEAD0002
#define ANDROID_RB_RESTART2 0xDEAD0003

#define LINUX_REBOOT_MAGIC1 0xfee1dead
#define LINUX_REBOOT_MAGIC2 672274793
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define LINUX_REBOOT_MAGIC2A 85072278
#define LINUX_REBOOT_MAGIC2B 369367448
#define LINUX_REBOOT_MAGIC2C 537993216
#define LINUX_REBOOT_CMD_RESTART 0x01234567
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define LINUX_REBOOT_CMD_HALT 0xCDEF0123
#define LINUX_REBOOT_CMD_CAD_ON 0x89ABCDEF
#define LINUX_REBOOT_CMD_CAD_OFF 0x00000000
#define LINUX_REBOOT_CMD_POWER_OFF 0x4321FEDC
/* WARNING: DO NOT EDIT, AUTO-GENERATED CODE - SEE TOP FOR INSTRUCTIONS */
#define LINUX_REBOOT_CMD_RESTART2 0xA1B2C3D4
#define LINUX_REBOOT_CMD_SW_SUSPEND 0xD000FCE2
#define LINUX_REBOOT_CMD_KEXEC 0x45584543


/* Flags */
#define ANDROID_RB_FLAG_NO_SYNC       0x1
#define ANDROID_RB_FLAG_NO_REMOUNT_RO 0x2

int android_reboot(int cmd, int flags, char *arg);

__END_DECLS

#endif /* __CUTILS_ANDROID_REBOOT_H__ */
