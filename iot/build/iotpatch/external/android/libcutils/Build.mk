#
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
#
LOCAL_PATH := $(my-dir)

include $(CLEAR_VARS)
COMMON_SRC_FILES := \
	android_reboot.c \
	ashmem-dev.c \
	config_utils.c \
	cpu_info.c \
	debugger.c \
	dir_hash.c \
	dlmalloc_stubs.c \
	fs.c \
	hashmap.c \
	iosched_policy.c \
	klog.c \
	load_file.c \
	memory.c \
	multiuser.c \
	native_handle.c \
	open_memstream.c \
	partition_utils.c \
	process_name.c \
	properties.c \
	qtaguid.c \
	record_stream.c \
	sched_policy.c \
	socket_inaddr_any_server.c \
	socket_local_client.c \
	socket_local_server.c \
	socket_loopback_client.c \
	socket_loopback_server.c \
	socket_network_client.c \
	sockets.c \
	strdup16to8.c \
	strdup8to16.c \
	str_parms.c \
	threads.c \
	uevent.c \
	sha1.c \
	trace.c \
#	atomic.c \
	ashmem-host.c \

LOCAL_MODULE := libcutils
LOCAL_SRC_FILES := $(COMMON_SRC_FILES)
LOCAL_C_INCLUDES := bionic_time.h
LOCAL_SHARED_LIBRARIES := libdlog.so
LOCAL_DEPANNER_MODULES := android-include dlog
LOCAL_CFLAGS += -DANDROID_SMP=0 -DHAVE_PTHREADS -DHAVE_SYS_UIO_H -DFAKE_LOG_DEVICE=1 -fPIC
LOCAL_MODULE_TAGS :=optional
include $(BUILD_STATIC_LIBRARY)

############################################################
include $(CLEAR_VARS)
COMMON_SRC_FILES := \
	android_reboot.c \
	ashmem-dev.c \
	config_utils.c \
	cpu_info.c \
	debugger.c \
	dir_hash.c \
	dlmalloc_stubs.c \
	fs.c \
	hashmap.c \
	iosched_policy.c \
	klog.c \
	load_file.c \
	memory.c \
	multiuser.c \
	native_handle.c \
	open_memstream.c \
	partition_utils.c \
	process_name.c \
	properties.c \
	qtaguid.c \
	record_stream.c \
	sched_policy.c \
	socket_inaddr_any_server.c \
	socket_local_client.c \
	socket_local_server.c \
	socket_loopback_client.c \
	socket_loopback_server.c \
	socket_network_client.c \
	sockets.c \
	strdup16to8.c \
	strdup8to16.c \
	str_parms.c \
	threads.c \
	uevent.c \
	sha1.c \
	trace.c \
#	atomic.c \
	ashmem-host.c \

LOCAL_MODULE := libcutils
LOCAL_SRC_FILES := $(COMMON_SRC_FILES)
LOCAL_C_INCLUDES := bionic_time.h
LOCAL_SHARED_LIBRARIES := libdlog.so
LOCAL_DEPANNER_MODULES := android-include dlog
LOCAL_LDLIBS := -ldlog -lresolv
LOCAL_CFLAGS += -DANDROID_SMP=0 -DHAVE_PTHREADS -DHAVE_SYS_UIO_H -DFAKE_LOG_DEVICE=1 -fPIC
LOCAL_MODULE_TAGS :=optional
include $(BUILD_SHARED_LIBRARY)
