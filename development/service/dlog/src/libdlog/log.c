/*-*- Mode: C; c-basic-offset: 8; indent-tabs-mode: t -*-
 * DLOG
 * Copyright (c) 2005-2008, The Android Open Source Project
 * Copyright (c) 2012-2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
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

/*#include "config.h"*/
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <dlog.h>
#include "loglimiter.h"
#include "logconfig.h"
#ifdef HAVE_SYSTEMD_JOURNAL
#define SD_JOURNAL_SUPPRESS_LOCATION 1
#include <syslog.h>
#include <systemd/sd-journal.h>
#endif
#include <assert.h>

#define LOG_BUF_SIZE	1024

#define LOG_MAIN	"log_main"
#define LOG_RADIO	"log_radio"
#define LOG_SYSTEM	"log_system"
#define LOG_APPS	"log_apps"

#define VALUE_MAX 2
#define LOG_CONFIG_FILE "/usr/etc/dlog.conf"

#ifndef HAVE_SYSTEMD_JOURNAL
static int log_fds[(int)LOG_ID_MAX] = { -1, -1, -1, -1 };
#endif
static int (*write_to_log)(log_id_t, log_priority, const char *tag, const char *msg) = NULL;
static pthread_mutex_t log_init_lock = PTHREAD_MUTEX_INITIALIZER;
static struct log_config config;

#ifdef HAVE_SYSTEMD_JOURNAL
static inline int dlog_pri_to_journal_pri(log_priority prio)
{
	static int pri_table[DLOG_PRIO_MAX] = {
		[DLOG_UNKNOWN] = LOG_DEBUG,
		[DLOG_DEFAULT] = LOG_DEBUG,
		[DLOG_VERBOSE] = LOG_DEBUG,
		[DLOG_DEBUG] = LOG_DEBUG,
		[DLOG_INFO] = LOG_INFO,
		[DLOG_WARN] = LOG_WARNING,
		[DLOG_ERROR] = LOG_ERR,
		[DLOG_FATAL] = LOG_CRIT,
		[DLOG_SILENT] = -1,
	};

	if (prio < 0 || prio >= DLOG_PRIO_MAX)
		return DLOG_ERROR_INVALID_PARAMETER;

	return pri_table[prio];
}

static inline const char* dlog_id_to_string(log_id_t log_id)
{
	static const char* id_table[LOG_ID_MAX] = {
		[LOG_ID_MAIN]   = LOG_MAIN,
		[LOG_ID_RADIO]  = LOG_RADIO,
		[LOG_ID_SYSTEM] = LOG_SYSTEM,
		[LOG_ID_APPS]   = LOG_APPS,
	};

	if (log_id < 0 || log_id >= LOG_ID_MAX || !id_table[log_id])
		return "UNKNOWN";

	return id_table[log_id];
}

static int __write_to_log_sd_journal(log_id_t log_id, log_priority prio, const char *tag, const char *msg)
{
	pid_t tid = (pid_t)syscall(SYS_gettid);
	/* XXX: sd_journal_sendv() with manually filed iov-s might be faster */
	return sd_journal_send("MESSAGE=%s", msg,
			       "PRIORITY=%i", dlog_pri_to_journal_pri(prio),
			       "LOG_TAG=%s", tag,
			       "LOG_ID=%s", dlog_id_to_string(log_id),
			       "TID=%d", tid,
			       NULL);
}

#else
static int __write_to_log_null(log_id_t log_id, log_priority prio, const char *tag, const char *msg)
{
	return DLOG_ERROR_NOT_PERMITTED;
}

static int __write_to_log_kernel(log_id_t log_id, log_priority prio, const char *tag, const char *msg)
{
	ssize_t ret;
	int log_fd;
	struct iovec vec[3];

	if (log_id < LOG_ID_MAX)
		log_fd = log_fds[log_id];
	else
		return DLOG_ERROR_INVALID_PARAMETER;

	if (prio < DLOG_VERBOSE || prio >= DLOG_PRIO_MAX)
		return DLOG_ERROR_INVALID_PARAMETER;

	if (!tag)
		tag = "";

	if (!msg)
		return DLOG_ERROR_INVALID_PARAMETER;

	vec[0].iov_base	= (unsigned char *) &prio;
	vec[0].iov_len	= 1;
	vec[1].iov_base	= (void *) tag;
	vec[1].iov_len	= strlen(tag) + 1;
	vec[2].iov_base	= (void *) msg;
	vec[2].iov_len	= strlen(msg) + 1;

	ret = writev(log_fd, vec, 3);
	if (ret < 0)
	    ret = errno;

	return ret;
}
#endif

static void __configure(void)
{
	if (0 > __log_config_read(LOG_CONFIG_FILE, &config)) {
		config.lc_limiter = 0;
		config.lc_plog = 0;
	}

	if (config.lc_limiter) {
		if (0 > __log_limiter_initialize()) {
			config.lc_limiter = 0;
		}
	}
}

static void __dlog_init(void)
{
	pthread_mutex_lock(&log_init_lock);
	/* configuration */
	__configure();
#ifdef HAVE_SYSTEMD_JOURNAL
	write_to_log = __write_to_log_sd_journal;
#else
	/* open device */
	log_fds[LOG_ID_MAIN] = open("/dev/"LOG_MAIN, O_WRONLY);
	log_fds[LOG_ID_SYSTEM] = open("/dev/"LOG_SYSTEM, O_WRONLY);
	log_fds[LOG_ID_RADIO] = open("/dev/"LOG_RADIO, O_WRONLY);
	log_fds[LOG_ID_APPS] = open("/dev/"LOG_APPS, O_WRONLY);
	if (log_fds[LOG_ID_MAIN] < 0) {
		write_to_log = __write_to_log_null;
	} else {
		write_to_log = __write_to_log_kernel;
	}
	if (log_fds[LOG_ID_RADIO] < 0)
		log_fds[LOG_ID_RADIO] = log_fds[LOG_ID_MAIN];
	if (log_fds[LOG_ID_SYSTEM] < 0)
		log_fds[LOG_ID_SYSTEM] = log_fds[LOG_ID_MAIN];
	if (log_fds[LOG_ID_APPS] < 0)
		log_fds[LOG_ID_APPS] = log_fds[LOG_ID_MAIN];
#endif
	pthread_mutex_unlock(&log_init_lock);
}

void __dlog_fatal_assert(int prio)
{
#ifdef FATAL_ON
	assert(!(prio == DLOG_FATAL));
#endif
}

static int dlog_should_log(log_id_t log_id, const char* tag, int prio)
{
	int should_log;

	if (!tag)
		return DLOG_ERROR_INVALID_PARAMETER;

	if (log_id < 0 || LOG_ID_MAX <= log_id)
		return DLOG_ERROR_INVALID_PARAMETER;

	if (log_id != LOG_ID_APPS && !config.lc_plog)
		return DLOG_ERROR_NOT_PERMITTED;

	if (config.lc_limiter) {
		should_log = __log_limiter_pass_log(tag, prio);

		if (!should_log) {
			return DLOG_ERROR_NOT_PERMITTED;
		} else if (should_log < 0) {
			write_to_log(log_id, prio, tag,
			             "Your log has been blocked due to limit of log lines per minute.");
			return DLOG_ERROR_NOT_PERMITTED;
		}
	}

	return DLOG_ERROR_NONE;
}

int __dlog_vprint(log_id_t log_id, int prio, const char *tag, const char *fmt, va_list ap)
{
	int ret;
	char buf[LOG_BUF_SIZE];

	if (write_to_log == NULL)
		__dlog_init();

	ret = dlog_should_log(log_id, tag, prio);

	if (ret < 0)
		return ret;

	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
	ret = write_to_log(log_id, prio, tag, buf);
#ifdef FATAL_ON
	__dlog_fatal_assert(prio);
#endif
	return ret;
}

int __dlog_vprint_assert(log_id_t log_id, int prio, const char *tag, const char *fmt, va_list ap)
{
	int ret;
	char buf[LOG_BUF_SIZE];

	if (write_to_log == NULL)
		__dlog_init();

	ret = dlog_should_log(log_id, tag, prio);

	if (ret < 0)
		return ret;

	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
	ret = write_to_log(log_id, prio, tag, buf);

	__dlog_fatal_assert(prio);
	return ret;
}

int __dlog_print(log_id_t log_id, int prio, const char *tag, const char *fmt, ...)
{
	int ret;
	va_list ap;
	char buf[LOG_BUF_SIZE];

	if (write_to_log == NULL)
		__dlog_init();

	ret = dlog_should_log(log_id, tag, prio);

	if (ret < 0)
		return ret;

	va_start(ap, fmt);
	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
	va_end(ap);

	ret = write_to_log(log_id, prio, tag, buf);
#ifdef FATAL_ON
	__dlog_fatal_assert(prio);
#endif
	return ret;
}

int __dlog_print_assert(log_id_t log_id, int prio, const char *tag, const char *fmt, ...)
{
	int ret;
	va_list ap;
	char buf[LOG_BUF_SIZE];

	if (write_to_log == NULL)
		__dlog_init();

	ret = dlog_should_log(log_id, tag, prio);

	if (ret < 0)
		return ret;

	va_start(ap, fmt);
	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
	va_end(ap);

	ret = write_to_log(log_id, prio, tag, buf);

	__dlog_fatal_assert(prio);
	return ret;
}

int dlog_vprint(log_priority prio, const char *tag, const char *fmt, va_list ap)
{
	char buf[LOG_BUF_SIZE];

	if (write_to_log == NULL)
		__dlog_init();

	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);

	return write_to_log(LOG_ID_APPS, prio, tag, buf);
}

int dlog_print(log_priority prio, const char *tag, const char *fmt, ...)
{
	va_list ap;
	char buf[LOG_BUF_SIZE];

	if (write_to_log == NULL)
		__dlog_init();

	va_start(ap, fmt);
	vsnprintf(buf, LOG_BUF_SIZE, fmt, ap);
	va_end(ap);

	return write_to_log(LOG_ID_APPS, prio, tag, buf);
}

void __attribute__((destructor)) __dlog_fini(void)
{
	__log_limiter_destroy();
}
