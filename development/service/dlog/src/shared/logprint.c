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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>

#include <logprint.h>


typedef struct FilterInfo_t {
	char *mTag;
	log_priority mPri;
	struct FilterInfo_t *p_next;
} FilterInfo;

struct log_format_t {
	log_priority global_pri;
	FilterInfo *filters;
	log_print_format format;
};

static FilterInfo * filterinfo_new(const char *tag, log_priority pri)
{
	FilterInfo *p_ret;
	p_ret = (FilterInfo *)calloc(1, sizeof(FilterInfo));
	if (!p_ret)
		return NULL;
	p_ret->mTag = strdup(tag);
	p_ret->mPri = pri;

	return p_ret;
}

static void filterinfo_free(FilterInfo *p_info)
{
	if (p_info == NULL) {
		return;
	}

	free(p_info->mTag);
	p_info->mTag = NULL;
}

/*
 * Note: also accepts 0-9 priorities
 * returns DLOG_UNKNOWN if the character is unrecognized
 */
static log_priority filter_char_to_pri (char c)
{
	log_priority pri;

	c = tolower(c);

	if (c >= '0' && c <= '9') {
		if (c >= ('0'+DLOG_SILENT)) {
			pri = DLOG_VERBOSE;
		} else {
			pri = (log_priority)(c - '0');
		}
	} else if (c == 'v') {
		pri = DLOG_VERBOSE;
	} else if (c == 'd') {
		pri = DLOG_DEBUG;
	} else if (c == 'i') {
		pri = DLOG_INFO;
	} else if (c == 'w') {
		pri = DLOG_WARN;
	} else if (c == 'e') {
		pri = DLOG_ERROR;
	} else if (c == 'f') {
		pri = DLOG_FATAL;
	} else if (c == 's') {
		pri = DLOG_SILENT;
	} else if (c == '*') {
		pri = DLOG_DEFAULT;
	} else {
		pri = DLOG_UNKNOWN;
	}

	return pri;
}

static char filter_pri_to_char (log_priority pri)
{
	switch (pri) {
	case DLOG_VERBOSE:
		return 'V';
	case DLOG_DEBUG:
		return 'D';
	case DLOG_INFO:
		return 'I';
	case DLOG_WARN:
		return 'W';
	case DLOG_ERROR:
		return 'E';
	case DLOG_FATAL:
		return 'F';
	case DLOG_SILENT:
		return 'S';
	case DLOG_DEFAULT:
	case DLOG_UNKNOWN:
	default:
		return '?';
	}
}

static log_priority filter_pri_for_tag(log_format *p_format, const char *tag)
{
	FilterInfo *p_curFilter;

	for (p_curFilter = p_format->filters; p_curFilter != NULL; p_curFilter = p_curFilter->p_next )
	{
		if (0 == strcmp(tag, p_curFilter->mTag))
		{
			if (p_curFilter->mPri == DLOG_DEFAULT) {
				return p_format->global_pri;
			} else {
				return p_curFilter->mPri;
			}
		}
	}
	return p_format->global_pri;
}

/** for debugging */
void dump_filters(log_format *p_format)
{
	FilterInfo *p_fi;

	for (p_fi = p_format->filters ; p_fi != NULL ; p_fi = p_fi->p_next) {
		char cPri = filter_pri_to_char(p_fi->mPri);
		if (p_fi->mPri == DLOG_DEFAULT) {
			cPri = filter_pri_to_char(p_format->global_pri);
		}
		fprintf(stderr, "%s:%c\n", p_fi->mTag, cPri);
	}

	fprintf(stderr, "*:%c\n", filter_pri_to_char(p_format->global_pri));

}

/**
 * returns 1 if this log line should be printed based on its priority
 * and tag, and 0 if it should not
 */
int log_should_print_line (log_format *p_format, const char *tag, log_priority pri)
{
	return pri >= filter_pri_for_tag(p_format, tag);
}

log_format *log_format_new()
{
	log_format *p_ret;

	p_ret = calloc(1, sizeof(log_format));

	if (!p_ret)
		return NULL;
	p_ret->global_pri = DLOG_SILENT;
	p_ret->format = FORMAT_BRIEF;

	return p_ret;
}

void log_format_free(log_format *p_format)
{
	FilterInfo *p_info, *p_info_old;

	p_info = p_format->filters;

	while (p_info != NULL) {
		p_info_old = p_info;
		p_info = p_info->p_next;
		filterinfo_free(p_info_old);
	}

	free(p_format);
}

void log_set_print_format(log_format *p_format,log_print_format format)
{
	p_format->format=format;
}

/**
 * Returns FORMAT_OFF on invalid string
 */
log_print_format log_format_from_string(const char * formatString)
{
	static log_print_format format;

	if (strcmp(formatString, "brief") == 0)
		format = FORMAT_BRIEF;
	else if (strcmp(formatString, "process") == 0)
		format = FORMAT_PROCESS;
	else if (strcmp(formatString, "tag") == 0)
		format = FORMAT_TAG;
	else if (strcmp(formatString, "thread") == 0)
		format = FORMAT_THREAD;
	else if (strcmp(formatString, "raw") == 0)
		format = FORMAT_RAW;
	else if (strcmp(formatString, "time") == 0)
		format = FORMAT_TIME;
	else if (strcmp(formatString, "threadtime") == 0)
		format = FORMAT_THREADTIME;
	else if (strcmp(formatString, "dump") == 0)
		format = FORMAT_DUMP;
	else if (strcmp(formatString, "long") == 0)
		format = FORMAT_LONG;
	else format = FORMAT_OFF;

	return format;
}

/**
 * filterExpression: a single filter expression
 * eg "AT:d"
 *
 * returns 0 on success and -1 on invalid expression
 *
 * Assumes single threaded execution
 */
int log_add_filter_rule(log_format *p_format,
		const char *filterExpression)
{
	size_t tagNameLength;
	log_priority pri = DLOG_DEFAULT;

	tagNameLength = strcspn(filterExpression, ":");

	if (tagNameLength == 0) {
		goto error;
	}

	if(filterExpression[tagNameLength] == ':') {
		pri = filter_char_to_pri(filterExpression[tagNameLength+1]);

		if (pri == DLOG_UNKNOWN) {
			goto error;
		}
	}

	if(0 == strncmp("*", filterExpression, tagNameLength)) {
		/* This filter expression refers to the global filter
		 * The default level for this is DEBUG if the priority
		 * is unspecified
		 */
		if (pri == DLOG_DEFAULT) {
			pri = DLOG_DEBUG;
		}

		p_format->global_pri = pri;
	} else {
		/* for filter expressions that don't refer to the global
		 * filter, the default is verbose if the priority is unspecified
		 */
		if (pri == DLOG_DEFAULT) {
			pri = DLOG_VERBOSE;
		}

		char *tagName;
		tagName = strndup(filterExpression, tagNameLength);

		FilterInfo *p_fi = filterinfo_new(tagName, pri);
		free(tagName);

		p_fi->p_next = p_format->filters;
		p_format->filters = p_fi;
	}

	return 0;
error:
	return -1;
}

/**
 * filterString: a comma/whitespace-separated set of filter expressions
 *
 * eg "AT:d *:i"
 *
 * returns 0 on success and -1 on invalid expression
 *
 * Assumes single threaded execution
 *
 */
int log_add_filter_string(log_format *p_format,
		const char *filterString)
{
	char *filterStringCopy = strdup (filterString);
	char *p_cur = filterStringCopy;
	char *p_ret;
	int err;

	while (NULL != (p_ret = strsep(&p_cur, " \t,"))) {
		/* ignore whitespace-only entries */
		if(p_ret[0] != '\0') {
			err = log_add_filter_rule(p_format, p_ret);

			if (err < 0) {
				goto error;
			}
		}
	}

	free (filterStringCopy);
	return 0;
error:
	free (filterStringCopy);
	return -1;
}

static inline char * strip_end(char *str)
{
	char *end = str + strlen(str) - 1;

	while (end >= str && isspace(*end))
		*end-- = '\0';
	return str;
}

/**
 * Splits a wire-format buffer into an LogEntry
 * entry allocated by caller. Pointers will point directly into buf
 *
 * Returns 0 on success and -1 on invalid wire format (entry will be
 * in unspecified state)
 */
int log_process_log_buffer(struct logger_entry *buf, log_entry *entry)
{
	int i, start = -1, end = -1;

	if (buf->len < 3) {
		fprintf(stderr, "Entry too small\n");
		return -1;
	}

	entry->tv_sec = buf->sec;
	entry->tv_nsec = buf->nsec;
	entry->pid = buf->pid;
	entry->tid = buf->tid;

	entry->priority = buf->msg[0];
	if (entry->priority < 0 || entry->priority > DLOG_SILENT) {
		fprintf(stderr, "Wrong priority message\n");
		return -1;
	}

	entry->tag = buf->msg + 1;
	if (!strlen(entry->tag)) {
		fprintf(stderr, "No tag message\n");
		return -1;
	}

	for (i = 0; i < buf->len; i++) {
		if (buf->msg[i] == '\0') {
			if (start == -1) {
				start = i + 1;
			} else {
				end = i;
				break;
			}
		}
	}
	if (start == -1) {
		fprintf(stderr, "Malformed log message\n");
		return -1;
	}
	if (end == -1) {
		end = buf->len - 1;
		buf->msg[end] = '\0';
	}

	entry->message = buf->msg + start;
	entry->messageLen = end - start;

	return 0;
}

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
		const log_entry *entry,
		size_t *p_outLength)
{
#if defined(HAVE_LOCALTIME_R)
	struct tm tmBuf;
#endif
	struct tm* ptm;
	char timeBuf[32], tzBuf[16];
	char prefixBuf[128], suffixBuf[128];
	char priChar;
	int prefixSuffixIsHeaderFooter = 0;
	char * ret = NULL;

	priChar = filter_pri_to_char(entry->priority);

	/*
	 * Get the current date/time in pretty form
	 *
	 * It's often useful when examining a log with "less" to jump to
	 * a specific point in the file by searching for the date/time stamp.
	 * For this reason it's very annoying to have regexp meta characters
	 * in the time stamp.  Don't use forward slashes, parenthesis,
	 * brackets, asterisks, or other special chars here.
	 */
#if defined(HAVE_LOCALTIME_R)
	ptm = localtime_r(&(entry->tv_sec), &tmBuf);
#else
	ptm = localtime(&(entry->tv_sec));
#endif
	strftime(timeBuf, sizeof(timeBuf), "%m-%d %H:%M:%S", ptm);
	strftime(tzBuf, sizeof(tzBuf), "%z", ptm);

	/*
	 * Construct a buffer containing the log header and log message.
	 */
	size_t prefixLen, suffixLen;

	switch (p_format->format) {
	case FORMAT_TAG:
		prefixLen = snprintf(prefixBuf, sizeof(prefixBuf),
				"%c/%-8s: ", priChar, entry->tag);
		strcpy(suffixBuf, "\n"); suffixLen = 1;
		break;
	case FORMAT_PROCESS:
		prefixLen = snprintf(prefixBuf, sizeof(prefixBuf),
				"%c(%5d) ", priChar, (int)entry->pid);
		suffixLen = snprintf(suffixBuf, sizeof(suffixBuf),
				"  (%s)\n", entry->tag);
		break;
	case FORMAT_THREAD:
		prefixLen = snprintf(prefixBuf, sizeof(prefixBuf),
				"%c(%5d:%5d) ", priChar, (int)entry->pid, (int)entry->tid);
		strcpy(suffixBuf, "\n");
		suffixLen = 1;
		break;
	case FORMAT_RAW:
		prefixBuf[0] = 0;
		prefixLen = 0;
		strcpy(suffixBuf, "\n");
		suffixLen = 1;
		break;
	case FORMAT_TIME:
		prefixLen = snprintf(prefixBuf, sizeof(prefixBuf),
				"%s.%03ld%s %c/%-8s(%5d): ", timeBuf, entry->tv_nsec / 1000000,
				tzBuf, priChar, entry->tag, (int)entry->pid);
		strcpy(suffixBuf, "\n");
		suffixLen = 1;
		break;
	case FORMAT_THREADTIME:
		prefixLen = snprintf(prefixBuf, sizeof(prefixBuf),
				"%s.%03ld%s %5d %5d %c %-8s: ", timeBuf, entry->tv_nsec / 1000000,
				tzBuf, (int)entry->pid, (int)entry->tid, priChar, entry->tag);
		strcpy(suffixBuf, "\n");
		suffixLen = 1;
		break;
	case FORMAT_DUMP:
		prefixLen = snprintf(prefixBuf, sizeof(prefixBuf),
				"%s.%03ld%s %5d %5d %c %-8s: ", timeBuf,
				entry->tv_nsec / 1000000, tzBuf, (int)entry->pid,
			   	(int)entry->tid, priChar, entry->tag);
		strcpy(suffixBuf, "\n");
		suffixLen = 1;
		break;
	case FORMAT_LONG:
		prefixLen = snprintf(prefixBuf, sizeof(prefixBuf),
				"[ %s.%03ld %5d:%5d %c/%-8s ]\n",
				timeBuf, entry->tv_nsec / 1000000, (int)entry->pid,
				(int)entry->tid, priChar, entry->tag);
		strcpy(suffixBuf, "\n\n");
		suffixLen = 2;
		prefixSuffixIsHeaderFooter = 1;
		break;
	case FORMAT_BRIEF:
	default:
		prefixLen = snprintf(prefixBuf, sizeof(prefixBuf),
				"%c/%-8s(%5d): ", priChar, entry->tag, (int)entry->pid);
		strcpy(suffixBuf, "\n");
		suffixLen = 1;
		break;
	}
	/* snprintf has a weird return value.   It returns what would have been
	 * written given a large enough buffer.  In the case that the prefix is
	 * longer then our buffer(128), it messes up the calculations below
	 * possibly causing heap corruption.  To avoid this we double check and
	 * set the length at the maximum (size minus null byte)
	 */
	if(prefixLen >= sizeof(prefixBuf))
		prefixLen = sizeof(prefixBuf) - 1;
	if(suffixLen >= sizeof(suffixBuf))
		suffixLen = sizeof(suffixBuf) - 1;

	/* the following code is tragically unreadable */

	size_t numLines;
	char *p;
	size_t bufferSize;
	const char *pm;

	if (prefixSuffixIsHeaderFooter) {
		/* we're just wrapping message with a header/footer */
		numLines = 1;
	} else {
		pm = entry->message;
		numLines = 0;

		/* The line-end finding here must match the line-end finding
		 * in for ( ... numLines...) loop below
		 */
		while (pm < (entry->message + entry->messageLen)) {
			if (*pm++ == '\n') numLines++;
		}
		/* plus one line for anything not newline-terminated at the end */
		if (pm > entry->message && *(pm-1) != '\n') numLines++;
	}

	/* this is an upper bound--newlines in message may be counted
	 * extraneously
	 */
	bufferSize = (numLines * (prefixLen + suffixLen)) + entry->messageLen + 1;

	if (defaultBufferSize >= bufferSize) {
		ret = defaultBuffer;
	} else {
		ret = (char *)malloc(bufferSize);

		if (ret == NULL) {
			return ret;
		}
	}

	ret[0] = '\0';       /* to start strcat off */

	p = ret;
	pm = entry->message;

	if (prefixSuffixIsHeaderFooter) {
		strcat(p, prefixBuf);
		p += prefixLen;
		strncat(p, entry->message, entry->messageLen);
		p += entry->messageLen;
		strcat(p, suffixBuf);
		p += suffixLen;
	} else {
		while (pm < (entry->message + entry->messageLen)) {
			const char *lineStart;
			size_t lineLen;

			lineStart = pm;

			/* Find the next end-of-line in message */
			while (pm < (entry->message + entry->messageLen)
					&& *pm != '\n') pm++;
			lineLen = pm - lineStart;

			strcat(p, prefixBuf);
			p += prefixLen;
			strncat(p, lineStart, lineLen);
			p += lineLen;
			strcat(p, suffixBuf);
			p += suffixLen;

			if (*pm == '\n')
				pm++;
		}
	}

	if (p_outLength != NULL) {
		*p_outLength = p - ret;
	}

	return ret;
}

/**
 * Either print or do not print log line, based on filter
 *
 * Returns count bytes written
 */

int log_print_log_line(
		log_format *p_format,
		int fd,
		const log_entry *entry)
{
	int ret;
	char defaultBuffer[512];
	char *outBuffer = NULL;
	size_t totalLen;

	outBuffer = log_format_log_line(p_format,
			defaultBuffer, sizeof(defaultBuffer), entry, &totalLen);

	if (!outBuffer)
		return -1;

	do {
		ret = write(fd, outBuffer, totalLen);
	} while (ret < 0 && errno == EINTR);

	if (ret < 0) {
		fprintf(stderr, "+++ LOG: write failed (errno=%d)\n", errno);
		ret = 0;
		goto done;
	}

	if (((size_t)ret) < totalLen) {
		fprintf(stderr, "+++ LOG: write partial (%d of %d)\n", ret,
				(int)totalLen);
		goto done;
	}

done:
	if (outBuffer != defaultBuffer) {
		free(outBuffer);
	}

	return ret;
}



void logprint_run_tests()
{
	int err;
	const char *tag;
	log_format *p_format;

	p_format = log_format_new();

	fprintf(stderr, "running tests\n");

	tag = "random";

	log_add_filter_rule(p_format,"*:i");

	assert (DLOG_INFO == filter_pri_for_tag(p_format, "random"));
	assert(log_should_print_line(p_format, tag, DLOG_DEBUG) == 0);
	log_add_filter_rule(p_format, "*");
	assert (DLOG_DEBUG == filter_pri_for_tag(p_format, "random"));
	assert(log_should_print_line(p_format, tag, DLOG_DEBUG) > 0);
	log_add_filter_rule(p_format, "*:v");
	assert (DLOG_VERBOSE == filter_pri_for_tag(p_format, "random"));
	assert(log_should_print_line(p_format, tag, DLOG_DEBUG) > 0);
	log_add_filter_rule(p_format, "*:i");
	assert (DLOG_INFO == filter_pri_for_tag(p_format, "random"));
	assert(log_should_print_line(p_format, tag, DLOG_DEBUG) == 0);

	log_add_filter_rule(p_format, "random");
	assert (DLOG_VERBOSE == filter_pri_for_tag(p_format, "random"));
	assert(log_should_print_line(p_format, tag, DLOG_DEBUG) > 0);
	log_add_filter_rule(p_format, "random:v");
	assert (DLOG_VERBOSE == filter_pri_for_tag(p_format, "random"));
	assert(log_should_print_line(p_format, tag, DLOG_DEBUG) > 0);
	log_add_filter_rule(p_format, "random:d");
	assert (DLOG_DEBUG == filter_pri_for_tag(p_format, "random"));
	assert(log_should_print_line(p_format, tag, DLOG_DEBUG) > 0);
	log_add_filter_rule(p_format, "random:w");
	assert (DLOG_WARN == filter_pri_for_tag(p_format, "random"));
	assert(log_should_print_line(p_format, tag, DLOG_DEBUG) == 0);

	log_add_filter_rule(p_format, "crap:*");
	assert (DLOG_VERBOSE== filter_pri_for_tag(p_format, "crap"));
	assert(log_should_print_line(p_format, "crap", DLOG_VERBOSE) > 0);

	/* invalid expression */
	err = log_add_filter_rule(p_format, "random:z");
	assert (err < 0);
	assert (DLOG_WARN == filter_pri_for_tag(p_format, "random"));
	assert(log_should_print_line(p_format, tag, DLOG_DEBUG) == 0);

	/* Issue #550946 */
	err = log_add_filter_string(p_format, " ");
	assert(err == 0);
	assert(DLOG_WARN == filter_pri_for_tag(p_format, "random"));

	/* note trailing space */
	err = log_add_filter_string(p_format, "*:s random:d ");
	assert(err == 0);
	assert(DLOG_DEBUG == filter_pri_for_tag(p_format, "random"));

	err = log_add_filter_string(p_format, "*:s random:z");
	assert(err < 0);

	log_format_free(p_format);

	fprintf(stderr, "tests complete\n");
}

