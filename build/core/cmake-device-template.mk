CMAKE_PREFLAGS=-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -Os $(arch_variant_cflags)

$(CMAKE_TEMPLATE):$(BUILD_SYSTEM)/template.cmake
	mkdir -p $(dir $@)
	cp $< $@
	sed -e 's#CMAKE_PROGRAM_PATH_TEMPLATE#$(TOP_DIR)/$(OUT_DEVICE_BINRARY_DIR)#' \
		-e 's#CMAKE_SYSROOT_TEMPLATE#$(TOP_DIR)/$(SYSROOT)#' \
		-e 's#CMAKE_C_COMPILER_TEMPLATE#$(COMPILER_PATH)/bin/$(TARGET_GCC)#' \
		-e 's#CMAKE_CXX_COMPILER_TEMPLATE#$(COMPILER_PATH)/bin/$(TARGET_GXX)#' \
		-e 's#CMAKE_ASM_COMPILER_TEMPLATE#$(COMPILER_PATH)/bin/$(TARGET_ASM)#' \
		-e 's#CMAKE_AR_TEMPLATE#mips-linux-gnu-ar#' \
		-e 's#CMAKE_RANLIB_TEMPLATE#mips-linux-gnu-ran#' \
	    -e 's#CMAKE_PREFLAGS#$(CMAKE_PREFLAGS)#' 	\
	$< > $@