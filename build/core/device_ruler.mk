ifeq ($(strip x$(TARGET_ARCH_VARIANT)),x)
TARGET_ARCH_VARIANT := mips32r2
endif
TARGET_ARCH_SPECIFIC_MAKEFILE:=$(BUILD_SYSTEM)/arch/$(TARGET_ARCH_VARIANT).mk
include $(TARGET_ARCH_SPECIFIC_MAKEFILE)
GLOBAL_DEVICE_CFLAGS := -O2
GLOBAL_DEVICE_CFLAGS += $(arch_variant_cflags)

COMPILER_LIB_PATH:=$(COMPILER_PATH)/lib/gcc/mips-linux-gnu/$(COMPILER_LIB_VERSION)/$(arch_variant_gcc_dir)

ifeq ($(strip x$(arch_variant_libc_dir)),x)
ROOT_PATH?=$(COMPILER_PATH)/mips-linux-gnu/libc
CRT_PATH?=$(COMPILER_PATH)/mips-linux-gnu/libc/usr/lib
LIBC_PATH?=$(COMPILER_PATH)/mips-linux-gnu/libc/lib
else
ROOT_PATH?=$(COMPILER_PATH)/mips-linux-gnu/libc/$(arch_variant_libc_dir)
CRT_PATH?=$(COMPILER_PATH)/mips-linux-gnu/libc/$(arch_variant_libc_dir)/usr/lib
LIBC_PATH?=$(COMPILER_PATH)/mips-linux-gnu/libc/$(arch_variant_libc_dir)/lib
endif

PRIVATE_CRT1_O_FILE:=$(CRT_PATH)/crt1.o
PRIVATE_CRTI_O_FILE:=$(CRT_PATH)/crti.o
PRIVATE_CRT_O_BEGIN_FILE:= $(COMPILER_LIB_PATH)/crtbeginS.o
PRIVATE_CRT_O_BEGINT_FILE:= $(COMPILER_LIB_PATH)/crtbeginT.o
PRIVATE_CRT_O_END_FILE:=$(COMPILER_LIB_PATH)/crtendS.o
PRIVATE_CRTN_O_FILE:=$(CRT_PATH)/crtn.o

PRIVATE_LD:=--as-needed -lgcc_s $(DEVICE_GLOBAL_LD)
PRIVATE_LD_STATIC:=--start-group -lgcc -lgcc_eh -lc --end-group $(DEVICE_GLOBAL_LD)
PRIVATE_LD_DIR:=$(COMPILER_LIB_PATH) $(LIBC_PATH) $(CRT_PATH)
PRIVATE_LD_FILE := $(arch_variant_ld_file)

define tranforms_device_o_to_exec
$(hide) $(TARGET_LD) \
--sysroot=$(ROOT_PATH) \
$(DEVICE_GLOBAL_LDFLAGS) \
-rpath-link=$(OUT_DEVICE_SHARED_DIR) \
--eh-frame-hdr -EL  \
-dynamic-linker $(PRIVATE_LD_FILE) -melf32ltsmip \
-o $@ \
$(PRIVATE_CRT1_O_FILE) \
$(PRIVATE_CRTI_O_FILE) \
$(PRIVATE_CRT_O_BEGIN_FILE) \
$(addprefix -L,$(PRIVATE_LD_DIR)) \
$(addprefix -L,$(DEVICE_GLOBAL_LD_DIR)) \
$^ $(PRIVATE_STATIC_LIBRARIES) \
$(PRIVATE_SHARED_LIBRARIES) \
$(DEVICE_GLOBAL_SHARED_LDLIBS) \
$(PRIVATE_LOCAL_LDLIBS) \
$(PRIVATE_LD) \
$(PRIVATE_CRT_O_END_FILE) \
$(PRIVATE_CRTN_O_FILE)
$(PRIVATE_CRT_O_END)
endef

define tranforms_device_o_to_static_exec
$(hide) $(TARGET_LD) \
--sysroot=$(ROOT_PATH) \
$(DEVICE_GLOBAL_LDFLAGS) \
-EL -mips32r2 -static -melf32ltsmip \
-o $@ \
$(PRIVATE_CRT1_O_FILE) \
$(PRIVATE_CRTI_O_FILE) \
$(PRIVATE_CRT_O_BEGINT_FILE) \
$(addprefix -L,$(PRIVATE_LD_DIR)) \
$(addprefix -L,$(DEVICE_GLOBAL_LD_DIR)) \
$^ $(PRIVATE_STATIC_LIBRARIES) \
$(PRIVATE_SHARED_LIBRARIES) \
$(DEVICE_GLOBAL_SHARED_LDLIBS) \
$(PRIVATE_LOCAL_LDLIBS) \
$(PRIVATE_LD_STATIC) \
$(PRIVATE_CRT_O_END_FILE) \
$(PRIVATE_CRTN_O_FILE)
$(PRIVATE_CRT_O_END)
endef

define tranforms_device_o_to_so
$(hide) $(TARGET_LD) \
--sysroot=$(ROOT_PATH) \
--eh-frame-hdr -EL $(DEVICE_GLOBAL_LDFLAGS) \
-rpath-link=$(OUT_DEVICE_SHARED_DIR) \
-shared -EL -melf32ltsmip \
-o $@ \
$(PRIVATE_CRTI_O_FILE) \
$(PRIVATE_CRT_O_BEGIN_FILE) \
$(addprefix -L,$(PRIVATE_LD_DIR)) \
$(addprefix -L,$(DEVICE_GLOBAL_LD_DIR)) \
$^ $(PRIVATE_STATIC_LIBRARIES) \
$(PRIVATE_SHARED_LIBRARIES) \
$(DEVICE_GLOBAL_SHARED_LDLIBS) \
$(PRIVATE_LOCAL_LDLIBS) \
$(PRIVATE_LOCAL_LDFLAGS) \
$(PRIVATE_LD) \
--no-as-needed \
$(PRIVATE_CRT_O_END_FILE) \
$(PRIVATE_CRTN_O_FILE)
endef
