# Variables we check:
#     HOST_BUILD_TYPE = { release debug }
#     TARGET_BUILD_TYPE = { release debug }
# and we output a bunch of variables, see the case statement at
# the bottom for the full list
#     OUT_DIR is also set to "out" if it's not already set.
#         this allows you to set it to somewhere else if you like

# Set up version information.

# ---------------------------------------------------------------
# If you update the build system such that the environment setup
# or buildspec.mk need to be updated, increment this number, and
# people who haven't re-run those will have to do so before they
# can build.  Make sure to also update the corresponding value in
# buildspec.mk.default and envsetup.sh.

# ---------------------------------------------------------------
# The product defaults to generic on hardware
# NOTE: This will be overridden in product_config.mk if make
# was invoked with a PRODUCT-xxx-yyy goal.
ifeq ($(TARGET_PRODUCT),)
TARGET_PRODUCT := full
endif

# the variant -- the set of files that are included for a build
ifeq ($(strip $(TARGET_BUILD_VARIANT)),)
TARGET_BUILD_VARIANT := eng
endif

# ---------------------------------------------------------------
# Set up configuration for host machine.  We don't do cross-
# compiles except for arm/mips, so the HOST is whatever we are
# running on

HOST_OS := $(strip $(HOST_OS))
ifndef HOST_OS
# On all modern variants of Windows (including Cygwin and Wine)
# the OS environment variable is defined to 'Windows_NT'
#
# The value of PROCESSOR_ARCHITECTURE will be x86 or AMD64
#
ifeq ($(OS),Windows_NT)
	HOST_OS := windows
else

UNAME := $(shell uname -sm)

# HOST_OS
ifneq (,$(findstring Linux,$(UNAME)))
	HOST_OS := linux
endif
ifneq (,$(findstring Darwin,$(UNAME)))
	HOST_OS := darwin
endif
ifneq (,$(findstring Macintosh,$(UNAME)))
	HOST_OS := darwin
endif
ifneq (,$(findstring CYGWIN,$(UNAME)))
	HOST_OS := windows
endif
endif
endif
# BUILD_OS is the real host doing the build.
BUILD_OS := $(HOST_OS)

# Under Linux, if USE_MINGW is set, we change HOST_OS to Windows to build the
# Windows SDK. Only a subset of tools and SDK will manage to build properly.
ifeq ($(HOST_OS),linux)
ifneq ($(USE_MINGW),)
	HOST_OS := windows
endif
endif

ifeq ($(HOST_OS),)
$(error Unable to determine HOST_OS from uname -sm: $(UNAME)!)
endif

HOST_OS_BASE := $(HOST_OS)
HOST_ARCH := $(strip $(HOST_ARCH))

ifeq ($(HOST_OS_BASE),windows)
	HOST_ARCH := $(PROCESSOR_ARCHITECTURE)
ifeq ($(HOST_ARCH),AMD64)
	HOST_ARCH := x86
endif
endif

# HOST_ARCH
ifneq (,$(findstring 86,$(UNAME)))
	HOST_ARCH := x86
endif

ifneq (,$(findstring Power,$(UNAME)))
	HOST_ARCH := ppc
endif

BUILD_ARCH := $(HOST_ARCH)

ifeq ($(HOST_ARCH),)
$(error Unable to determine HOST_ARCH from uname -sm: $(UNAME)!)
endif

# the host build defaults to release, and it must be release or debug
ifeq ($(HOST_BUILD_TYPE),)
HOST_BUILD_TYPE := release
endif

ifneq ($(HOST_BUILD_TYPE),release)
ifneq ($(HOST_BUILD_TYPE),debug)
$(error HOST_BUILD_TYPE must be either release or debug, not '$(HOST_BUILD_TYPE)')
endif
endif

# This is the standard way to name a directory containing prebuilt host
# objects. E.g., prebuilt/$(HOST_PREBUILT_TAG)/cc
ifeq ($(HOST_OS),windows)
  HOST_PREBUILT_TAG := windows
else
  HOST_PREBUILT_TAG := $(HOST_OS)-$(HOST_ARCH)
endif

# TARGET_COPY_OUT_* are all relative to the staging directory, ie PRODUCT_OUT.
# Define them here so they can be used in product config files.
TARGET_COPY_OUT_SYSTEM := system

# Read the product specs so we an get TARGET_DEVICE and other
# variables that we need in order to locate the output files.

build_variant := $(filter-out eng user userdebug,$(TARGET_BUILD_VARIANT))
ifneq ($(build_variant)-$(words $(TARGET_BUILD_VARIANT)),-1)
$(warning bad TARGET_BUILD_VARIANT: $(TARGET_BUILD_VARIANT))
$(error must be empty or one of: eng user userdebug)
endif

# ---------------------------------------------------------------
# Set up configuration for target machine.
# The following must be set:
# 		TARGET_OS = { linux }
# 		TARGET_ARCH = { arm | x86 | mips }

TARGET_OS := linux
# TARGET_ARCH should be set by BoardConfig.mk and will be checked later

# the target build type defaults to release
ifneq ($(TARGET_BUILD_TYPE),debug)
TARGET_BUILD_TYPE := release
endif

# ---------------------------------------------------------------
# figure out the output directories

ifeq (,$(strip $(OUT_DIR)))
OUT_DIR := out
endif

PRODUCT := $(strip $(PRODUCT))

ifeq ($(PRINT_BUILD_CONFIG),)
PRINT_BUILD_CONFIG := true
endif

DEVICE_COMPILER.c := $(TARGET_GCC)
DEVICE_COMPILER.cpp := $(TARGET_GXX)
DEVICE_COMPILER.cc := $(TARGET_GXX)
DEVICE_COMPILER.S := $(TARGET_GCC)
HOST_COMPILER.c := $(HOST_GCC)
HOST_COMPILER.cpp := $(HOST_GXX)
HOST_COMPILER.cc := $(HOST_GXX)
HOST_COMPILER.S := $(HOST_GXX)

ALL_INLCUDES:=include

ifeq ($(strip x$(SDK_BUILD)),x)
MODULES_ALL:=$(shell ls $(TOPDIR))
endif
OUT_DIR:=out
ALL_GEN_FILE_TYPE:=STATIC SHARED BINRARY

HARDWARE_INC :=$(TOP_DIR)/hardware/init/include

TARGET_INSTALL_PATH:=$(OUT_DIR)/$(PRODUCT)/$(TARGET_DEVICE)
TARGET_FS_BUILD := $(TARGET_INSTALL_PATH)/system
TARGET_OTA_FS_BUILD := $(TARGET_INSTALL_PATH)/systemOTA
TARGET_TESTSUIT_DIR:=testsuite
TARGET_MODEL_PATH := $(TOPDIR)/device/$(TARGET_DEVICE)/$(TARGET_STORAGE_MEDIUM)/$(MODEL)
TARGET_FS_SRC_PATH := $(TARGET_MODEL_PATH)/system
TARGET_COMMON_PATH:= $(TOPDIR)/device/common
TARGET_COMMON_FS_PATH:=$(TARGET_COMMON_PATH)/system

TARGET_SYSROOT:=$(TARGET_INSTALL_PATH)/sysroot
SYSROOT:=$(TARGET_INSTALL_PATH)/sysroot
CMAKE_TEMPLATE=$(OUT_HOST_BINRARY_DIR)/cmake/toolchain.cmake

OUT_IMAGE_DIR:=$(OUT_DIR)/$(PRODUCT)/$(TARGET_DEVICE)/image

OUT_DEVICE_BINRARY_DIR:=$(TARGET_FS_BUILD)/usr/bin
OUT_DEVICE_LOCAL_DIR:= $(TARGET_FS_BUILD)/usr/local

ifeq ($(strip $(USE_LEGACY_BUILD_SYSTEM)),YES)
ifeq ($(strip x$(SDK_BUILD)),x)
OUT_DEVICE_OBJ_DIR:=$(TARGET_INSTALL_PATH)/obj
OUT_DEVICE_INCLUDE_DIR:=$(TARGET_INSTALL_PATH)/include
OUT_DEVICE_SHARED_DIR:=$(TARGET_FS_BUILD)/usr/lib
OUT_DEVICE_STATIC_DIR:=$(TARGET_INSTALL_PATH)/static/lib
OUT_DEVICE_ANDROID_INCLUDE_DIR:=$(TARGET_INSTALL_PATH)/android/include
else
OUT_DEVICE_OBJ_DIR:=$(TOPDIR)/obj
OUT_DEVICE_INCLUDE_DIR:=$(TOPDIR)/platform/usr/include
OUT_DEVICE_SHARED_DIR:=$(TOPDIR)/platform/usr/lib
OUT_DEVICE_STATIC_DIR:=$(TOP_DIR)/platform/usr/lib
DEVICE_GLOBAL_LDFLAGS:=-L$(TOPDIR)/platform/lib
endif

else

ifeq ($(strip x$(SDK_BUILD)),x)
OUT_DEVICE_OBJ_DIR:=$(TARGET_INSTALL_PATH)/obj
#OUT_DEVICE_INCLUDE_DIR:=$(TARGET_INSTALL_PATH)/include
OUT_DEVICE_SHARED_DIR:=$(TARGET_FS_BUILD)/usr/lib
#OUT_DEVICE_STATIC_DIR:=$(TARGET_INSTALL_PATH)/static/lib
#OUT_DEVICE_ANDROID_INCLUDE_DIR:=$(TARGET_INSTALL_PATH)/android/include

OUT_DEVICE_INCLUDE_DIR:=$(SYSROOT)/usr/include
#OUT_DEVICE_SHARED_DIR:=$(SYSROOT)/usr/lib
OUT_DEVICE_STATIC_DIR:=$(SYSROOT)/usr/lib
OUT_DEVICE_ANDROID_INCLUDE_DIR:=$(SYSROOT)/usr/include
OUT_DEVICE_SHARED_SYSROOT_DIR:=$(SYSROOT)/usr/lib

else
OUT_DEVICE_OBJ_DIR:=$(TOPDIR)/obj
OUT_DEVICE_INCLUDE_DIR:=$(TOPDIR)/platform/usr/include
#OUT_DEVICE_SHARED_DIR:=$(TOPDIR)/platform/usr/lib
OUT_DEVICE_STATIC_DIR:=$(TOP_DIR)/platform/usr/lib
OUT_DEVICE_SHARED_SYSROOT_DIR:=$(TOP_DIR)/usr/lib
DEVICE_GLOBAL_LDFLAGS:=-L$(TOPDIR)/platform/lib
endif


endif
OUT_HOST_DIR:=$(OUT_DIR)/host
OUT_HOST_BINRARY_DIR:=$(OUT_HOST_DIR)/tools
OUT_HOST_STATIC_DIR:=$(OUT_HOST_DIR)/static/lib
OUT_HOST_SHARED_DIR:=$(OUT_HOST_DIR)/tools
OUT_HOST_OBJ_DIR:=$(OUT_HOST_DIR)/obj
OUT_HOST_INCLUDE_DIR:=$(OUT_HOST_DIR)/include

ABS_HOST_INCLUDE_DIR:=$(TOP_DIR)/$(OUT_HOST_INCLUDE_DIR)
ABS_HOST_SHARED_DIR:=$(TOP_DIR)/$(OUT_HOST_SHARED_DIR)
ABS_HOST_STATIC_DIR:=$(TOP_DIR)/$(OUT_HOST_STATIC_DIR)

ABS_DEVICE_INCLUDE_DIR:=$(TOP_DIR)/$(OUT_DEVICE_INCLUDE_DIR)
ABS_DEVICE_SHARED_DIR:=$(TOP_DIR)/$(OUT_DEVICE_SHARED_DIR)
ABS_DEVICE_STATIC_DIR:=$(TOP_DIR)/$(OUT_DEVICE_STATIC_DIR)
ABS_DEVICE_ANDROID_INCLUDE_DIR:=$(TOP_DIR)/$(OUT_DEVICE_INCLUDE_DIR)

HOST_GLOBAL_LD_DIR:=$(OUT_HOST_SHARED_DIR)
HOST_GLOBAL_SHARED_LDLIBS:=
HOST_GLOBAL_LD:=

DEVICE_GLOBAL_LD_DIR:=$(OUT_DEVICE_SHARED_DIR)
DEVICE_GLOBAL_SHARED_LDLIBS:=
DEVICE_GLOBAL_LD:=

MKJFFS2:=$(OUT_HOST_BINRARY_DIR)/mkfs.jffs2
MKUBIFS:=$(OUT_HOST_BINRARY_DIR)/mkfs.ubifs
MKCRAMFS:=$(OUT_HOST_BINRARY_DIR)/mkcramfs
MKEXT4:=$(OUT_HOST_BINRARY_DIR)/genext2fs.sh

DEVELOPMENT_TOOLS_PATH:=$(TOP_DIR)/development/tools

export LD_LIBRARY_PATH=$(ABS_HOST_SHARED_DIR)
export HOST_BINRARY_DIR=$(OUT_HOST_BINRARY_DIR)
export MAKE_JLEVEL?=1


ifeq ($(strip $(USE_LEGACY_BUILD_SYSTEM)),NO)
# 使用新的编译系统, ROOT_PATH都指向buildroot的SYSROOT.
ROOT_PATH:=$(TOP_DIR)/$(SYSROOT)
CRT_PATH:=$(TOP_DIR)/$(SYSROOT)/usr/lib
LIBC_PATH:=$(TOP_DIR)/$(SYSROOT)/lib
HOST_GLOBAL_LD_DIR:=
endif

PYTHON_TOOL=$(TOP_DIR)/$(OUT_HOST_DIR)/usr/bin/python
