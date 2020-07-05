# Copyright (C) 2008 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

LOCAL_PATH:= $(call my-dir)

COMMONSOURCES:= \
	src/mi/init.c \
	src/ptrace/_UPT_elf.c \
	src/ptrace/_UPT_accessors.c \
	src/ptrace/_UPT_access_fpreg.c \
	src/ptrace/_UPT_access_mem.c \
	src/ptrace/_UPT_access_reg.c \
	src/ptrace/_UPT_create.c \
	src/ptrace/_UPT_destroy.c \
	src/ptrace/_UPT_find_proc_info.c \
	src/ptrace/_UPT_get_dyn_info_list_addr.c \
	src/ptrace/_UPT_put_unwind_info.c \
	src/ptrace/_UPT_get_proc_name.c \
	src/ptrace/_UPT_reg_offset.c \
	src/ptrace/_UPT_resume.c \
	src/mips/is_fpreg.c \
	src/mips/regname.c \
	src/mips/Gcreate_addr_space.c \
	src/mips/Gget_proc_info.c \
	src/mips/Gget_save_loc.c \
	src/mips/Gglobal.c \
	src/mips/Ginit.c \
	src/mips/Ginit_local.c \
	src/mips/Ginit_remote.c \
	src/mips/Gregs.c \
	src/mips/Gresume.c \
	src/mips/Gstep.c \
	src/mips/Lcreate_addr_space.c \
	src/mi/flush_cache.c \
	src/mi/mempool.c \
	src/mi/strerror.c \
	src/mi/backtrace.c \
	src/mi/dyn-cancel.c \
	src/mi/dyn-info-list.c \
	src/mi/dyn-register.c \
	src/mi/map.c \
	src/mi/Lmap.c \
	src/mi/Ldyn-extract.c \
	src/mi/Lfind_dynamic_proc_info.c \
	src/mi/Lget_proc_info_by_ip.c \
	src/mi/Lget_proc_name.c \
	src/mi/Lput_dynamic_unwind_info.c \
	src/mi/Ldestroy_addr_space.c \
	src/mi/Lget_reg.c \
	src/mi/Lset_reg.c \
	src/mi/Lget_fpreg.c \
	src/mi/Lset_fpreg.c \
	src/mi/Lset_caching_policy.c \
	src/mi/Gdyn-extract.c \
	src/mi/Gdyn-remote.c \
	src/mi/Gfind_dynamic_proc_info.c \
	src/mi/Gget_accessors.c \
	src/mi/Gget_proc_info_by_ip.c \
	src/mi/Gget_proc_name.c \
	src/mi/Gput_dynamic_unwind_info.c \
	src/mi/Gdestroy_addr_space.c \
	src/mi/Gget_reg.c \
	src/mi/Gset_reg.c \
	src/mi/Gget_fpreg.c \
	src/mi/Gset_fpreg.c \
	src/mi/Gset_caching_policy.c \
	src/dwarf/Lexpr.c \
	src/dwarf/Lfde.c \
	src/dwarf/Lparser.c \
	src/dwarf/Lpe.c \
	src/dwarf/Lstep_dwarf.c \
	src/dwarf/Lfind_proc_info-lsb.c \
	src/dwarf/Lfind_unwind_table.c \
	src/dwarf/Gexpr.c \
	src/dwarf/Gfde.c \
	src/dwarf/Gfind_proc_info-lsb.c \
	src/dwarf/Gfind_unwind_table.c \
	src/dwarf/Gparser.c \
	src/dwarf/Gpe.c \
	src/dwarf/Gstep_dwarf.c \
	src/dwarf/global.c \
	src/os-common.c \
	src/os-linux.c \
	src/Los-common.c \
	src/mips/getcontext-android.S

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= $(COMMONSOURCES)
LOCAL_MODULE:= libunwind
LOCAL_MODULE_TAGS := optional
LOCAL_SHARED_LIBRARIES := libdlog.so
LOCAL_DEPANNER_MODULES := dlog libunwind-headers
LOCAL_MODULE_GEN_SHARED_FILES:=libunwind.so
LOCAL_C_INCLUDES := include src
LOCAL_CPP_INCLUDES := include src
LOCAL_CFLAGS += -Wa,-mips32r2 -O2 -G 0 -Wall -shared -fPIC -D_GNU_SOURCE=1 -DHAVE_CONFIG_H -DHAVE_SYS_PTRACE_H
LOCAL_LDLIBS += -lc -lstdc++ -lpthread -lrt -ldlog
include $(BUILD_SHARED_LIBRARY)

#===================================================
# copy headers
# #
include $(CLEAR_VARS)

LOCAL_MODULE := libunwind-headers
LOCAL_MODULE_TAGS :=optional
LOCAL_MODULE_PATH :=$(TARGET_INSTALL_PATH)/android
LOCAL_COPY_FILES := include/libunwind.h:include/libunwind.h \
	include/compiler.h:include/compiler.h \
	include/config.h:include/config.h \
	include/libunwind-mips.h:include/libunwind-mips.h \
	include/libunwind-dynamic.h:include/libunwind-dynamic.h \
	include/tdep/libunwind_i.h:include/tdep/libunwind_i.h  \
	include/libunwind-common.h:include/libunwind-common.h \
	include/libunwind-ptrace.h:include/libunwind-ptrace.h

include $(BUILD_MULTI_PREBUILT)
