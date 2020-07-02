arch_variant_cflags := -mips32r2 -mfp64

ifeq ($(strip $(TOOLCHAINS_DIR)),mips-gcc720-glibc226)
arch_variant_gcc_dir :=
arch_variant_libc_dir :=
else
#For 5.2.0
arch_variant_gcc_dir := fp64
arch_variant_libc_dir := mfp64
endif

arch_variant_ld_file := /lib/ld-linux-mipsn8.so.1
