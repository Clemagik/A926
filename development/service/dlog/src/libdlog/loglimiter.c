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

#include <stddef.h>
#include <limits.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/time.h>
#include <time.h>

/* Included for priorities level */
#include <dlog.h>
#include "loglimiter.h"

/* Defines maximal meaningful tag length */
#define TAG_REASONABLE_LEN        32

/* Some random big odd number to make hash more diverse */
#define HASH_MAGIC_THINGY         5237231

#define TIME_FRAME                60

struct rule{
	/* TODO: List element handle, the list could be embedded
	         into structure some day, like kernel lists */
	struct rule* prev;

	unsigned hash;
	int prio;
	int limit;
	int hit;
	time_t start;
	char tag[TAG_REASONABLE_LEN];
};

typedef int (*hash_cmp_func_t)(struct rule*, struct rule*);
typedef int (*hash_match_func_t)(struct rule*, unsigned, const char*, int);

struct hashmap {
	hash_cmp_func_t cmp;
	hash_match_func_t match;
	int size;
	void* bucket[];
};

struct hashmap* rules_hashmap = NULL;

/* Keep rules table as single-linked list */
struct rule* rules_table = NULL;

#define HASHMAP_MASK(hm)          ((int)(hm->size - 1))
#define HASHMAP_LINEAR_PROBE_LEAP 1

static void rules_destroy(struct rule* rlist)
{
	struct rule* r;

	if (NULL == rlist) {
		return;
	}

	while ((r = rlist)) {
		rlist = rlist->prev;
		free(r);
	}
}

void __log_limiter_rules_purge(void)
{
	rules_destroy(rules_table);
	rules_table = NULL;
}

static int rule_compare(struct rule* r1, struct rule* r2)
{
	if (r1->hash == r2->hash) {
		if (r1->prio == r2->prio) {
			return strncmp(r1->tag, r2->tag, strlen(r2->tag));
		} else {
			return (r1->prio > r2->prio ? 1 : (-1));
		}
	}

	return (r1->hash > r2->hash ? 1 : (-1));
}

static int rule_match(struct rule* r1, unsigned key, const char* s, int prio)
{
	if (r1->hash == key) {
		if (r1->prio == prio) {
			return strncmp(r1->tag, s, strlen(s));
		} else {
			return (r1->prio > prio ? 1 : (-1));
		}
	}

	return (r1->hash > key ? 1 : (-1));
}

/* Translate fancy priority notation into common denominator */
static int util_prio_to_char(int prio)
{
	static const char pri_table[DLOG_PRIO_MAX] = {
		[DLOG_VERBOSE] = 'V',
		[DLOG_DEBUG] = 'D',
		[DLOG_INFO] = 'I',
		[DLOG_WARN] = 'W',
		[DLOG_ERROR] = 'E',
		[DLOG_FATAL] = 'F',
		[DLOG_SILENT] = 'S',
	};

	if (DLOG_PRIO_MAX > prio && prio >= 0) {
		return pri_table[prio];
	} else {
		switch (prio) {
			case 'V': case 'v': case '1':
			case 'D': case 'd': case '2':
			case 'I': case 'i': case '3':
			case 'W': case 'w': case '4':
			case 'E': case 'e': case '5':
			case 'F': case 'f': case '6':
			case 'S': case 's': case '7':
			case '*':
				return prio;

			default:
				;;
		}
	}

	return '?';
}

/* The key is built from TAG and priority by DJB algorithm (Dan Bernstein).
 * Key is only 31 bits long. Negative values are keep to hold NULL bucket */
static unsigned util_hash_key(const char* s, int c)
{
	unsigned hash = (unsigned)c;

	hash = ((hash << 5) + hash) + c;

	if (!s || !s[0]) {
		goto finish;
	}

	while ('\0' != (c = *s++)) {
		hash = ((hash << 5) + hash) + c;
	}

finish:
	/* Makes the hash more diverse */
	hash *= HASH_MAGIC_THINGY;

	return hash;
}

/* Create hashmap, it's internal interface. */
static struct hashmap* hashmap_create(int size, hash_cmp_func_t cmp_func,
				      hash_match_func_t match_func)
{
	struct hashmap* hm = NULL;
	/* please keep hashmap fill ratio around 50% */
	int internal_size = size << 1;

	if (!cmp_func || !match_func || !size) {
		return NULL;
	}


	/* Round up the lines counter to next power of two. */
	internal_size--;
	internal_size |= internal_size >> 1;
	internal_size |= internal_size >> 2;
	internal_size |= internal_size >> 4;
	internal_size |= internal_size >> 8;
	internal_size |= internal_size >> 16;
	internal_size++;

	hm = malloc(sizeof(struct hashmap) + internal_size * sizeof(void*));
	if (!hm) {
		return NULL;
	}
	/* Initialize hash field to correct value */
	memset((void*)hm, 0, sizeof(struct hashmap) + internal_size * sizeof(void*));

	hm->size = internal_size;
	hm->cmp = cmp_func;
	hm->match = match_func;

	return hm;
}

static void hashmap_destroy(struct hashmap* hm)
{
	if (hm) {
		hm->size = 0;
		free(hm);
	}
}

static void hashmap_add(struct hashmap* hm, struct rule* r)
{
	unsigned b = (r->hash & HASHMAP_MASK(hm));

	while (hm->bucket[b]) {
		if (!hm->cmp(r, (struct rule*)hm->bucket[b])) {
			break;
		}
		b = (b + 1) & HASHMAP_MASK(hm);
	}

	hm->bucket[b] = r;
}

static struct rule* hashmap_search(struct hashmap* hm, unsigned key,
				   const char* tag, int prio)
{
	unsigned b = (key & HASHMAP_MASK(hm));
	unsigned b0 = b;

	while (hm->bucket[b]) {
		if (!hm->match(hm->bucket[b], key, tag, prio)) {
			break;
		}

		b = (b + 1) & HASHMAP_MASK(hm);

		if (b0 == b) {
			return NULL;
		}
	}

	if (!hm->bucket[b]) {
		return NULL;
	}

	return hm->bucket[b];
}

/* Must be always executed after __log_config_read() */
int __log_limiter_initialize(void)
{
	int hm_size;
	struct rule* rlist = NULL;

	/* logconfig.c module had to initialize this correctly */
	if (NULL == rules_table) {
		return (-1);
	}

	/* Count rules in the table */
	hm_size = 0;
	rlist = rules_table;
	while (rlist) {
		hm_size++;
		rlist = rlist->prev;
	}

	/* Allocate hashmap */
	rules_hashmap = (struct hashmap*) hashmap_create(hm_size,
	                                                 &rule_compare,
	                                                 &rule_match);
	if (NULL == rules_hashmap || !rules_hashmap->size) {
		goto bailout;
	}

	/* Add rule to hashmap */
	rlist = rules_table;
	while (rlist) {
		hashmap_add(rules_hashmap, rlist);
		rlist = rlist->prev;
	}

	return 0;

bailout:
	hashmap_destroy(rules_hashmap);
	rules_destroy(rules_table);
	rules_table = NULL;
	rules_hashmap = NULL;

	return (-1);
}

void __log_limiter_destroy(void)
{
	hashmap_destroy(rules_hashmap);
	rules_destroy(rules_table);
	rules_table = NULL;
	rules_hashmap = NULL;
}

int __log_limiter_add_rule(const char* tag, int prio, int limit)
{
	struct rule* r;

	if (!tag) {
		return (-1);
	}

	r = (struct rule*) malloc(sizeof(struct rule));
	if (NULL == r) {
		return (-1);
	}
	memset(r, 0, sizeof(struct rule));

	snprintf(r->tag, TAG_REASONABLE_LEN, "%s", tag);
	r->prio = util_prio_to_char(prio);
	r->hash = util_hash_key(tag, r->prio);
	r->limit = limit;
	r->start = time(NULL);
	r->hit = 0;

	r->prev = rules_table;
	rules_table = r;

	return 0;
}


/* Function implement logic needed to decide,
   whenever message is written to log or not.

   Possible return values are:
	0  - to indicate that message is deny to write into log.
	(-1) - to indicate that limit of the messages is reached.
	1  - to indicate that message is allowed to write into log.
*/
int __log_limiter_pass_log(const char* tag, int prio)
{
	unsigned key = 0;
	struct rule* r = NULL;
	time_t now = 0;

	key = util_hash_key(tag, util_prio_to_char(prio));
	r = hashmap_search(rules_hashmap, key, tag, util_prio_to_char(prio));

	if (!r) {
		/* Rule not found, let's check general rule TAG:* */
		key = util_hash_key(tag, '*');
		r = hashmap_search(rules_hashmap, key, tag, '*');
		if (!r) {
			/* Rule TAG:* not found,
			   let check general rule *:priority */
			key = util_hash_key("*", util_prio_to_char(prio));
			r = hashmap_search(rules_hashmap, key, "*",
			                               util_prio_to_char(prio));
			if (!r) {
				/* All known paths were exhausted,
				   use global rule *:* */
				key = util_hash_key("*", '*');
				r = hashmap_search(rules_hashmap, key, "*", '*');

				/* *:* is not defined, so pass message through */
				if (!r) {
					return 1;
				}
			}
		}
	}

	if (!r->limit) {
		return 0;
	} else if (__LOG_LIMITER_LIMIT_MAX < r->limit) {
		return 1;
	}

	/* Decide, if it should go through or stop */
	now = time(NULL);

	if (0 > now) {
		return 1;
	}

	if (now - r->start <= TIME_FRAME) {
		if (r->hit >= 0) {
			if (r->hit < r->limit) {
				r->hit++;
				return 1;
			}
			r->hit = INT_MIN+1;
			return (-1);
		} else {
			r->hit++;
			return 0;
		}

	} else {
		r->start = now;
		r->hit = 0;
		return 1;
	}

	/* If everything failed, then pass message through */
	return 1;
}
