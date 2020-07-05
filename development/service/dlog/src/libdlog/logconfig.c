/*
 * DLOG
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logconfig.h"
#include "loglimiter.h"

/* Functions possible return value */
#define RET_ERROR                      (-1)
#define RET_SUCCESS                    0

#define CONFIG_LINE_MAX_LEN            256
#define CONFIG_OPTION_MAX_LEN          64

/* Shortcut macros */
#define isCOMMENT(c)                   ('#' == c)
#define isNEWLINE(c)                   ('\n' == c || '\r' == c)

/* Dlog options definition */
#define LOG_PLATFORM_STRING            "PlatformLogs"
#define LOG_LIMITER_STRING             "LogLimiter"

/* Define options possible values */
#define ALLOW_STRING                   "allow"
#define DENY_STRING                    "deny"
#define ON_STRING                      "on"
#define OFF_STRING                     "off"
#define isYES(c)                       (c == '1')
#define isNO(c)                        (c == '0')


static int log_config_multiplex_opt(char* opt_str, char* val_str, int prio,
                                    struct log_config* config)
{
	int value = 0;

	/* There are only two ways to interpret the lines, so make here a short circuit */
	if (0 < prio) { /* Is it a rule or an option  ? */
		/* For the filtering rule ... */

		if (!strncasecmp(ALLOW_STRING, val_str, sizeof(ALLOW_STRING))) {
			value = __LOG_LIMITER_LIMIT_MAX + 1;
		} else if (!strncasecmp(DENY_STRING, val_str,
		                                     sizeof(DENY_STRING))) {
			value = 0;
		} else {
			char* endptr = NULL;

			value = strtoul(val_str, &endptr, 0);
			if (*endptr != '\0') {
				return RET_ERROR;
			}
		}

		return __log_limiter_add_rule(opt_str, prio, value);

	} else { /* It's an option then */
		if (isYES(*val_str)) {
			value = 1;
		} else if (isNO(*val_str)) {
			value = 0;
		} else if (!strncasecmp(ON_STRING, val_str,
		                        sizeof(ON_STRING))) {
			value = 1;
		} else if (!strncasecmp(OFF_STRING, val_str,
		                        sizeof(OFF_STRING))) {
			value = 0;
		} else {
			return RET_ERROR;
		}

		if (!strncasecmp(LOG_PLATFORM_STRING, opt_str,
		                 sizeof(LOG_PLATFORM_STRING))) {
			config->lc_plog = value;
		} else if (!strncasecmp(LOG_LIMITER_STRING, opt_str,
		                       sizeof(LOG_LIMITER_STRING))) {
			config->lc_limiter = value;
		} else {
			return RET_ERROR;
		}
	}

	return RET_SUCCESS;
}

/* Function returns 0 for success or -1 when error occurred */
int __log_config_read(const char* config_file, struct log_config* config)
{
	FILE* fconfig = NULL;
	char buf[CONFIG_LINE_MAX_LEN];
	char opt[CONFIG_OPTION_MAX_LEN];
	char opt_value[CONFIG_OPTION_MAX_LEN];
	int prio = (-1);
	int ret = 0;

	/* Check input */
	if (NULL == config_file || NULL == config) {
		return RET_ERROR;
	}

	if (NULL == (fconfig = fopen(config_file, "r"))) {
		return RET_ERROR;
	}

	while (1) {
		memset(buf, 0, CONFIG_LINE_MAX_LEN);
		errno = 0;
		if (NULL == fgets(buf, CONFIG_LINE_MAX_LEN, fconfig)) {
			if (!errno) {
				break;
			}
			goto bailout;
		}

		/* We ignore comments and blank lines */
		if (isCOMMENT(*buf) || isNEWLINE(*buf)) {
			continue;
		}

		memset(opt, 0, sizeof(opt));
		memset(opt_value, 0, sizeof(opt_value));
		prio = (-1);
		/* Read configure line, sscanf() should return two tokens,
		 * even for tag filtering rule */
		ret = sscanf(buf, "%[A-z0-9-]\t%[A-z0-9]",
		                  opt, opt_value);
		if (ret != 2) { /* The line is malformed ? */
			char c = 0;
			/* This could be rule with space inside TAG */
			ret = sscanf(buf, "\"%[]A-z0-9*\x20_+:;/-]\"\t|\t%c\t%[A-z0-9]",
			                  opt, &c, opt_value);
			if (ret != 3) {
				goto bailout;
			}
			prio = (int)c;
		}


		if (0 > log_config_multiplex_opt(opt, opt_value, prio, config)) {
			goto bailout;
		}
	}

	fclose(fconfig);
	return RET_SUCCESS;

bailout:
	/* These actions should warranty that
	   we cleanly handle initialization errors */
	fclose(fconfig);
	/* Clean rules list to prevent log limiter initialization,
	   if configuration error occured. */
	__log_limiter_rules_purge();

	return RET_ERROR;
}
