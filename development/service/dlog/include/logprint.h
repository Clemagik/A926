/*
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

#ifndef _LOGPRINT_H
#define _LOGPRINT_H

#include <time.h>
#include <pthread.h>

#include <logger.h>
#include <dlog.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FORMAT_OFF = 0,
    FORMAT_BRIEF,
    FORMAT_PROCESS,
    FORMAT_TAG,
    FORMAT_THREAD,
    FORMAT_RAW,
    FORMAT_TIME,
    FORMAT_THREADTIME,
    FORMAT_DUMP,
    FORMAT_LONG,
} log_print_format;

typedef struct log_format_t log_format;

typedef struct log_entry_t {
    time_t tv_sec;
    long tv_nsec;
    log_priority priority;
    pid_t pid;
    pthread_t tid;
    const char * tag;
    size_t messageLen;
    const char * message;
} log_entry;

log_format *log_format_new();

void log_format_free(log_format *p_format);

void log_set_print_format(log_format *p_format, 
        log_print_format format);

/**
 * Returns FORMAT_OFF on invalid string
 */
log_print_format log_format_from_string(const char *s);

/** 
 * filterExpression: a single filter expression
 * eg "AT:d"
 *
 * returns 0 on success and -1 on invalid expression
 *
 * Assumes single threaded execution
 *
 */

int log_add_filter_rule(log_format *p_format, 
        const char *filterExpression);


/** 
 * filterString: a whitespace-separated set of filter expressions 
 * eg "AT:d *:i"
 *
 * returns 0 on success and -1 on invalid expression
 *
 * Assumes single threaded execution
 *
 */

int log_add_filter_string(log_format *p_format,
        const char *filterString);


/** 
 * returns 1 if this log line should be printed based on its priority
 * and tag, and 0 if it should not
 */
int log_should_print_line (
        log_format *p_format, const char *tag, log_priority pri);


/**
 * Splits a wire-format buffer into an log_entry
 * entry allocated by caller. Pointers will point directly into buf
 *
 * Returns 0 on success and -1 on invalid wire format (entry will be
 * in unspecified state)
 */
int log_process_log_buffer(struct logger_entry *buf,
                                 log_entry *entry);

/**
 * Formats a log message into a buffer
 *
 * Uses defaultBuffer if it can, otherwise malloc()'s a new buffer
 * If return value != defaultBuffer, caller must call free()
 * Returns NULL on malloc error
 */

char *log_format_log_line (    
    log_format *p_format,
    char *defaultBuffer,
    size_t defaultBufferSize,
    const log_entry *p_line,
    size_t *p_outLength);


/**
 * Either print or do not print log line, based on filter
 *
 * Assumes single threaded execution
 *
 */
int log_print_log_line(
    log_format *p_format,
    int fd,
    const log_entry *entry);

/**
 * logprint test furction
 *
 */
void logprint_run_tests(void);

#ifdef __cplusplus
}
#endif

#endif /*_LOGPRINT_H*/
