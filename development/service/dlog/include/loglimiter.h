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

#ifndef _LOGLIMITER_H_
#define _LOGLIMITER_H_

/* Define maximum logging speed, everything greater means unlimited.
 * Maximum possible value is INT_MAX-2 */
#define __LOG_LIMITER_LIMIT_MAX        (10*1000)

int __log_limiter_initialize(void);

void __log_limiter_destroy(void);

int __log_limiter_pass_log(const char* tag, int prio);

int __log_limiter_add_rule(const char* tag, int prio, int limit);

void __log_limiter_rules_purge(void);

#endif /* _LOGLIMITER_H_ */
