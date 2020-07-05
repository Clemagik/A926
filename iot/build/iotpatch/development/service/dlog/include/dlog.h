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

/**
 * @file	dlog.h
 * @version	0.4
 * @brief	This file is the header file of interface of dlog.
 */
#ifndef	_DLOG_H_
#define	_DLOG_H_

#include "tizen_error.h"
#include <stdarg.h>
#include <string.h>

#define DEBUG_ENABLE 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * This is the local tag used for the following simplified
 * logging macros.  You can change this preprocessor definition
 * before using the other macros to change the tag.
 */
#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

#ifndef __MODULE__
#define __MODULE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

/**
 * @addtogroup CAPI_SYSTEM_DLOG
 * @{
 *
 */
/**
 * @brief Enumeration for Dlog Error.
 * @since_tizen 2.3
 */
typedef enum {
	DLOG_ERROR_NONE = TIZEN_ERROR_NONE, /**< Successful */
	DLOG_ERROR_INVALID_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER, /**< Invalid parameter */
	DLOG_ERROR_NOT_PERMITTED = TIZEN_ERROR_NOT_PERMITTED, /**< Operation not permitted */
} dlog_error_e;
/**
 * @}
 */

/**
 * @addtogroup CAPI_SYSTEM_DLOG
 * @{
 *
 */
/**
 * @brief log priority values, in ascending priority order.
 * @since_tizen 2.3
 */
typedef enum {
	DLOG_UNKNOWN = 0, /**< Keep this always at the start */
	DLOG_DEFAULT, /**< Default */
	DLOG_VERBOSE, /**< Verbose */
	DLOG_DEBUG, /**< Debug */
	DLOG_INFO, /**< Info */
	DLOG_WARN, /**< Warning */
	DLOG_ERROR, /**< Error */
	DLOG_FATAL, /**< Fatal */
	DLOG_SILENT, /**< Silent */
	DLOG_PRIO_MAX	/**< Keep this always at the end. */
} log_priority;
/**
 * @}
 */

/*
 *  * Android log priority values, in ascending priority order.
 *   */
typedef enum android_LogPriority {
	ANDROID_LOG_UNKNOWN = 0,
	ANDROID_LOG_DEFAULT,    /* only for SetMinPriority() */
	ANDROID_LOG_VERBOSE,
    ANDROID_LOG_DEBUG,
    ANDROID_LOG_INFO,
	ANDROID_LOG_WARN,
	ANDROID_LOG_ERROR,
	ANDROID_LOG_FATAL,
	ANDROID_LOG_SILENT,     /* only for SetMinPriority(); must be last */
} android_LogPriority;



typedef enum {
	EVENT_TYPE_INT      = 0,
	EVENT_TYPE_LONG     = 1,
	EVENT_TYPE_STRING   = 2,
	EVENT_TYPE_LIST     = 3,
} AndroidEventLogType;
#define sizeof_AndroidEventLogType sizeof(typeof_AndroidEventLogType)
#define typeof_AndroidEventLogType unsigned char




/**
 * @internal
 * @brief log id
 * @since_tizen 2.3
 */

/**
 * Fixme: On the basis of TIZEN,
 * the definition of android is transplanted
 **/
typedef enum {
	LOG_ID_MIN = 0,

	LOG_ID_MAIN = 0,
	LOG_ID_RADIO = 1,
	LOG_ID_SYSTEM = 2,
	LOG_ID_APPS = 3,
    LOG_ID_EVENTS = 4,
    LOG_ID_CRASH = 5,

	LOG_ID_MAX
} log_id_t;
#define sizeof_log_id_t sizeof(typeof_log_id_t)
#define typeof_log_id_t unsigned char

static inline int __dlog_no_print(const char *fmt __attribute__((unused)), ...) { return 0; }

#define CONDITION(cond)     (__builtin_expect((cond) != 0, 0))
#define NOP(...) ({ do { __dlog_no_print(fmt, arg...); } while (0); })

// Macro inner work---------------------------------------------------------------
#undef LOG_
#ifdef DEBUG_ENABLE
#define LOG_(id, prio, tag, fmt, arg...) \
	({ do { \
		__dlog_print(id, prio, tag, "%s: %s(%d) > " fmt, __MODULE__, __func__, __LINE__, ##arg); \
	} while (0); })
#else
#define LOG_(id, prio, tag, fmt, arg...) \
	({ do { \
		if ((int)prio != DLOG_DEBUG) { \
			__dlog_print(id, prio, tag, "%s: %s(%d) > " fmt, __MODULE__, __func__, __LINE__, ##arg); \
		} \
	} while (0); })
#endif

#undef SECURE_LOG_
#ifdef DEBUG_ENABLE
#define SECURE_LOG_(id, prio, tag, fmt, arg...) \
	({ do { \
		__dlog_print(id, prio, tag, "%s: %s(%d) > [SECURE_LOG] " fmt, __MODULE__, __func__, __LINE__, ##arg); \
	} while (0); })
#else
#define SECURE_LOG_(id, prio, tag, fmt, arg...) NOP(fmt, ##arg)
#endif

#define LOG_ASSERT(id, prio, tag, fmt, arg...) \
	({ do { \
		__dlog_print_assert(id, prio, tag, "%s: %s(%d) > " fmt, __MODULE__, __func__, __LINE__, ##arg); \
	} while (0); })
// ---------------------------------------------------------------------
/**
 * @internal
 * @brief For Secure Log.
 * @remarks Normally we strip Secure log from release builds.
 * Please use this macros.
 */
/**
 * @internal
 * @brief For Application and etc.
 * @details Simplified macro to send a main log message using the current LOG_TAG.
 * Example:
 *  SECURE_LOGD("app debug %d", num);
 *  SECURE_LOGE("app error %d", num);
 */
#define SECURE_LOGD(format, arg...) SECURE_LOG_(LOG_ID_MAIN, DLOG_DEBUG, LOG_TAG, format, ##arg)
#define SECURE_LOGI(format, arg...) SECURE_LOG_(LOG_ID_MAIN, DLOG_INFO, LOG_TAG, format, ##arg)
#define SECURE_LOGW(format, arg...) SECURE_LOG_(LOG_ID_MAIN, DLOG_WARN, LOG_TAG, format, ##arg)
#define SECURE_LOGE(format, arg...) SECURE_LOG_(LOG_ID_MAIN, DLOG_ERROR, LOG_TAG, format, ##arg)
#define SECURE_LOGF(format, arg...) SECURE_LOG_(LOG_ID_MAIN, DLOG_FATAL, LOG_TAG, format, ##arg)
#define SECURE_LOGV(format, arg...) SECURE_LOG_(LOG_ID_MAIN, DLOG_VERBOSE, LOG_TAG, format, ##arg)
/**
 * @internal
 * @brief For Framework and system etc.
 * @details Simplified macro to send a system log message using the current LOG_TAG.
 * Example:
 *  SECURE_SLOGD("system debug %d", num);
 *  SECURE_SLOGE("system error %d", num);
 */
#define SECURE_SLOGD(format, arg...) SECURE_LOG_(LOG_ID_SYSTEM, DLOG_DEBUG, LOG_TAG, format, ##arg)
#define SECURE_SLOGI(format, arg...) SECURE_LOG_(LOG_ID_SYSTEM, DLOG_INFO, LOG_TAG, format, ##arg)
#define SECURE_SLOGW(format, arg...) SECURE_LOG_(LOG_ID_SYSTEM, DLOG_WARN, LOG_TAG, format, ##arg)
#define SECURE_SLOGE(format, arg...) SECURE_LOG_(LOG_ID_SYSTEM, DLOG_ERROR, LOG_TAG, format, ##arg)
#define SECURE_SLOGF(format, arg...) SECURE_LOG_(LOG_ID_SYSTEM, DLOG_FATAL, LOG_TAG, format, ##arg)
#define SECURE_SLOGV(format, arg...) SECURE_LOG_(LOG_ID_SYSTEM, DLOG_VERBOSE, LOG_TAG, format, ##arg)
/**
 * @internal
 * @brief For Modem and radio etc.
 * @details Simplified macro to send a radio log message using the current LOG_TAG.
 * Example:
 *  SECURE_RLOGD("radio debug %d", num);
 *  SECURE_RLOGE("radio error %d", num);
 */
#define SECURE_RLOGD(format, arg...) SECURE_LOG_(LOG_ID_RADIO, DLOG_DEBUG, LOG_TAG, format, ##arg)
#define SECURE_RLOGI(format, arg...) SECURE_LOG_(LOG_ID_RADIO, DLOG_INFO, LOG_TAG, format, ##arg)
#define SECURE_RLOGW(format, arg...) SECURE_LOG_(LOG_ID_RADIO, DLOG_WARN, LOG_TAG, format, ##arg)
#define SECURE_RLOGE(format, arg...) SECURE_LOG_(LOG_ID_RADIO, DLOG_ERROR, LOG_TAG, format, ##arg)
#define SECURE_RLOGF(format, arg...) SECURE_LOG_(LOG_ID_RADIO, DLOG_FATAL, LOG_TAG, format, ##arg)
#define SECURE_RLOGV(format, arg...) SECURE_LOG_(LOG_ID_RADIO, DLOG_VERBOSE, LOG_TAG, format, ##arg)
/**
 * @internal
 * @brief For Tizen OSP Application macro.
 */
#define SECURE_ALOGD(format, arg...) SECURE_LOG_(LOG_ID_APPS, DLOG_DEBUG, LOG_TAG, format, ##arg)
#define SECURE_ALOGI(format, arg...) SECURE_LOG_(LOG_ID_APPS, DLOG_INFO, LOG_TAG, format, ##arg)
#define SECURE_ALOGW(format, arg...) SECURE_LOG_(LOG_ID_APPS, DLOG_WARN, LOG_TAG, format, ##arg)
#define SECURE_ALOGE(format, arg...) SECURE_LOG_(LOG_ID_APPS, DLOG_ERROR, LOG_TAG, format, ##arg)
#define SECURE_ALOGF(format, arg...) SECURE_LOG_(LOG_ID_APPS, DLOG_FATAL, LOG_TAG, format, ##arg)
#define SECURE_ALOGV(format, arg...) SECURE_LOG_(LOG_ID_APPS, DLOG_VERBOSE, LOG_TAG, format, ##arg)
/**
 * @internal
 * @details If you want use redefined macro.
 * You can use this macro.
 * This macro need priority and tag arguments.
 */
#define SECURE_LOG(priority, tag, format, arg...) SECURE_LOG_(LOG_ID_MAIN, D##priority, tag, format, ##arg)
#define SECURE_SLOG(priority, tag, format, arg...) SECURE_LOG_(LOG_ID_SYSTEM, D##priority, tag, format, ##arg)
#define SECURE_RLOG(priority, tag, format, arg...) SECURE_LOG_(LOG_ID_RADIO, D##priority, tag, format, ##arg)
#define SECURE_ALOG(priority, tag, format, arg...) SECURE_LOG_(LOG_ID_APPS, D##priority, tag, format, ##arg)

/**
 * @internal
 * @brief For Application and etc.
 * @details Simplified macro to send a main log message using the current LOG_TAG.
 * Example:
 *  LOGD("app debug %d", num);
 *  LOGE("app error %d", num);
 */
#ifdef DEBUG_ENABLE
#define LOGD(format, arg...) LOG_(LOG_ID_MAIN, DLOG_DEBUG, LOG_TAG, format, ##arg)
#else
#define LOGD(format, arg...) NOP(format, ##arg)
#endif
#define LOGI(format, arg...) LOG_(LOG_ID_MAIN, DLOG_INFO, LOG_TAG, format, ##arg)
#define LOGW(format, arg...) LOG_(LOG_ID_MAIN, DLOG_WARN, LOG_TAG, format, ##arg)
#define LOGE(format, arg...) LOG_(LOG_ID_MAIN, DLOG_ERROR, LOG_TAG, format, ##arg)
#define LOGF(format, arg...) LOG_(LOG_ID_MAIN, DLOG_FATAL, LOG_TAG, format, ##arg)
#define LOGV(format, arg...) LOG_(LOG_ID_MAIN, DLOG_VERBOSE, LOG_TAG, format, ##arg)
/**
 * @internal
 * @brief For Framework and system etc.
 * @details Simplified macro to send a system log message using the current LOG_TAG.
 * Example:
 *  SLOGD("system debug %d", num);
 *  SLOGE("system error %d", num);
 */
#ifdef DEBUG_ENABLE
#define SLOGD(format, arg...) LOG_(LOG_ID_SYSTEM, DLOG_DEBUG, LOG_TAG, format, ##arg)
#else
#define SLOGD(format, arg...) NOP(format, ##arg)
#endif
#define SLOGI(format, arg...) LOG_(LOG_ID_SYSTEM, DLOG_INFO, LOG_TAG, format, ##arg)
#define SLOGW(format, arg...) LOG_(LOG_ID_SYSTEM, DLOG_WARN, LOG_TAG, format, ##arg)
#define SLOGE(format, arg...) LOG_(LOG_ID_SYSTEM, DLOG_ERROR, LOG_TAG, format, ##arg)
#define SLOGF(format, arg...) LOG_(LOG_ID_SYSTEM, DLOG_FATAL, LOG_TAG, format, ##arg)
#define SLOGV(format, arg...) LOG_(LOG_ID_SYSTEM, DLOG_VERBOSE, LOG_TAG, format, ##arg)
/**
 * @internal
 * @brief For Modem and radio etc.
 * @details Simplified macro to send a radio log message using the current LOG_TAG.
 * Example:
 *  RLOGD("radio debug %d", num);
 *  RLOGE("radio error %d", num);
 */
#ifdef DEBUG_ENABLE
#define RLOGD(format, arg...) LOG_(LOG_ID_RADIO, DLOG_DEBUG, LOG_TAG, format, ##arg)
#else
#define RLOGD(format, arg...) NOP(format, ##arg)
#endif
#define RLOGI(format, arg...) LOG_(LOG_ID_RADIO, DLOG_INFO, LOG_TAG, format, ##arg)
#define RLOGW(format, arg...) LOG_(LOG_ID_RADIO, DLOG_WARN, LOG_TAG, format, ##arg)
#define RLOGE(format, arg...) LOG_(LOG_ID_RADIO, DLOG_ERROR, LOG_TAG, format, ##arg)
#define RLOGF(format, arg...) LOG_(LOG_ID_RADIO, DLOG_FATAL, LOG_TAG, format, ##arg)
#define RLOGV(format, arg...) LOG_(LOG_ID_RADIO, DLOG_VERBOSE, LOG_TAG, format, ##arg)
/**
 * @internal
 * @brief For Tizen OSP Application macro.
 */
#ifdef DEBUG_ENABLE
#define ALOGD(format, arg...) LOG_(LOG_ID_APPS, DLOG_DEBUG, LOG_TAG, format, ##arg)
#else
#define ALOGD(format, arg...) NOP(format, ##arg)
#endif
#define ALOGI(format, arg...) LOG_(LOG_ID_APPS, DLOG_INFO, LOG_TAG, format, ##arg)
#define ALOGW(format, arg...) LOG_(LOG_ID_APPS, DLOG_WARN, LOG_TAG, format, ##arg)
#define ALOGE(format, arg...) LOG_(LOG_ID_APPS, DLOG_ERROR, LOG_TAG, format, ##arg)
#define ALOGF(format, arg...) LOG_(LOG_ID_APPS, DLOG_FATAL, LOG_TAG, format, ##arg)
//#define ALOGV(format, arg...) LOG_(LOG_ID_APPS, DLOG_VERBOSE, LOG_TAG, format, ##arg)
#define ALOGV(format, arg...) LOG_(LOG_ID_APPS, DLOG_INFO, LOG_TAG, format, ##arg)
/**
 * @internal
 * @brief Basic log message macro
 * @details This macro allows you to specify a priority and a tag
 * if you want to use this macro directly, you must add this messages for unity of messages.
 * (LOG(prio, tag, "%s: %s(%d) > " format, __MODULE__, __func__, __LINE__, ##arg))
 *
 * Example:
 *  #define MYMACRO(prio, tag, format, arg...) \
 *          LOG(prio, tag, format, ##arg)
 *
 *  MYMACRO(LOG_DEBUG, MYTAG, "test mymacro %d", num);
 *
 */
#ifndef LOG
#define LOG(priority, tag, format, arg...) LOG_(LOG_ID_MAIN, D##priority, tag, format, ##arg)
#endif
#define SLOG(priority, tag, format, arg...) LOG_(LOG_ID_SYSTEM, D##priority, tag, format, ##arg)
#define RLOG(priority, tag, format, arg...) LOG_(LOG_ID_RADIO, D##priority, tag, format, ##arg)
#define ALOG(priority, tag, format, arg...) LOG_(LOG_ID_APPS, D##priority, tag, format, ##arg)
/**
 * @internal
 */
#define LOG_VA(priority, tag, fmt, args) \
	vprint_log(D##priority, tag, fmt, args)
#define ALOG_VA(priority, tag, fmt, args) \
	vprint_apps_log(D##priority, tag, fmt, args)
#define RLOG_VA(priority, tag, fmt, args) \
	vprint_radio_log(D##priority, tag, fmt, args)
#define SLOG_VA(priority, tag, fmt, args) \
	vprint_system_log(D##priority, tag, fmt, args)
#define print_apps_log(prio, tag, fmt...) \
	__dlog_print(LOG_ID_APPS, prio, tag, fmt)
#define vprint_apps_log(prio, tag, fmt...) \
	__dlog_vprint(LOG_ID_APPS, prio, tag, fmt)
#define print_log(prio, tag, fmt...) \
	__dlog_print(LOG_ID_MAIN, prio, tag, fmt)
#define vprint_log(prio, tag, fmt...) \
	__dlog_vprint(LOG_ID_MAIN, prio, tag, fmt)
#define print_radio_log(prio, tag, fmt...)\
	__dlog_print(LOG_ID_RADIO, prio, tag, fmt)
#define vprint_radio_log(prio, tag, fmt...) \
	__dlog_vprint(LOG_ID_RADIO, prio, tag, fmt)
#define print_system_log(prio, tag, fmt...)\
	__dlog_print(LOG_ID_SYSTEM, prio, tag, fmt)
#define vprint_system_log(prio, tag, fmt...) \
	__dlog_vprint(LOG_ID_SYSTEM, prio, tag, fmt)

// ---------------------------------------------------------------------
// Don't use below macro no more!! It will be removed -- Verbose and Fatal priority macro will be removed --
#define COMPATIBILITY_ON
#ifdef COMPATIBILITY_ON
/**
 * @internal
 * @breif Conditional Macro.
 * @remarks Don't use this macro. It's just compatibility.
 * It will be deprecated.
 */
#define LOGD_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			LOGD(format, ##arg); \
		} \
	} while (0); })
#define LOGI_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			LOGI(format, ##arg); \
		} \
	} while (0); })
#define LOGW_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			LOGW(format, ##arg); \
		} \
	} while (0); })
#define LOGE_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			LOGE(format, ##arg); \
		} \
	} while (0); })
#define LOGF_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			LOGF(format, ##arg); \
		} \
	} while (0); })
#define ALOGD_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			ALOGD(format, ##arg); \
		} \
	} while (0); })
#define ALOGI_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			ALOGI(format, ##arg); \
		} \
	} while (0); })
#define ALOGW_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			ALOGW(format, ##arg); \
		} \
	} while (0); })
#define ALOGV_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			ALOGV(format, ##arg); \
		} \
	} while (0); })
#define ALOGE_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			ALOGE(format, ##arg); \
		} \
	} while (0); })
#define ALOGF_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			ALOGF(format, ##arg); \
		} \
	} while (0); })
#define SLOGD_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			SLOGD(format, ##arg); \
		} \
	} while (0); })
#define SLOGI_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			SLOGI(format, ##arg); \
		} \
	} while (0); })
#define SLOGW_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			SLOGW(format, ##arg); \
		} \
	} while (0); })
#define SLOGE_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			SLOGE(format, ##arg); \
		} \
	} while (0); })
#define SLOGF_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			SLOGF(format, ##arg); \
		} \
	} while (0); })
#define RLOGD_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			RLOGD(format, ##arg); \
		} \
	} while (0); })
#define RLOGI_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			RLOGI(format, ##arg); \
		} \
	} while (0); })
#define RLOGW_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			RLOGW(format, ##arg); \
		} \
	} while (0); })
#define RLOGE_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			RLOGE(format, ##arg); \
		} \
	} while (0); })
#define RLOGF_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			RLOGF(format, ##arg); \
		} \
	} while (0); })
#define LOG_ON() ({ do { } while (0); })
#define SECLOG(...) ({ do { } while (0); })
#endif

/*
 * Log a fatal error.  If the given condition fails, this stops program
 * execution like a normal assertion, but also generating the given message.
 * It is NOT stripped from release builds.  Note that the condition test
 * is -inverted- from the normal assert() semantics.
 */
/*#ifndef LOG_ALWAYS_FATAL_IF
#define LOG_ALWAYS_FATAL_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			__dlog_print_assert(LOG_ID_APPS, \
			DLOG_FATAL, LOG_TAG, "%s: %s(%d) > " \
			format, __MODULE__, __func__, __LINE__, ##arg); \
		} \
	} while (0); })
#endif

#ifndef LOG_ALWAYS_FATAL
#define LOG_ALWAYS_FATAL(format, arg...) \
	({ do { \
		__dlog_print_assert(LOG_ID_APPS, \
		 DLOG_FATAL, LOG_TAG, "%s: %s(%d) > " \
		 format, __MODULE__, __func__, __LINE__, ##arg); \
	 } while (0); })
#endif*/

#ifndef LOG_ALWAYS_FATAL_IF
#define LOG_ALWAYS_FATAL_IF(cond, format, arg...) \
	({ do { \
		if (CONDITION(cond)) { \
			LOG_ASSERT(LOG_ID_APPS, DLOG_FATAL, LOG_TAG, format, ##arg); \
		} \
	} while (0); })
#endif

#ifndef LOG_ALWAYS_FATAL
#define LOG_ALWAYS_FATAL(format, arg...) \
	({ do { \
			LOG_ASSERT(LOG_ID_APPS, DLOG_FATAL, LOG_TAG, format, ##arg); \
	 } while (0); })
#endif

/*
 * Versions of LOG_ALWAYS_FATAL_IF and LOG_ALWAYS_FATAL that
 * are stripped out of release builds.
 */
/*#if LOG_NDEBUG

#ifndef LOG_FATAL_IF
#define LOG_FATAL_IF(cond, format, arg...) ((void)0)
#endif
#ifndef LOG_FATAL
#define LOG_FATAL(format, arg...) ((void)0)
#endif

#else*/

#ifndef LOG_FATAL_IF
#define LOG_FATAL_IF(cond, format, arg...) LOG_ALWAYS_FATAL_IF(cond, format, ##arg)
#endif
#ifndef LOG_FATAL
#define LOG_FATAL(format, arg...) LOG_ALWAYS_FATAL(format, ##arg)
#endif

/*#endif*/

/*
 * Assertion that generates a log message when the assertion fails.
 * Stripped out of release builds.  Uses the current LOG_TAG.
 */
#ifndef ALOG_ASSERT
//#define ALOG_ASSERT(cond, format, arg...) LOG_FATAL_IF(!(cond), format, ##arg)
#define ALOG_ASSERT(cond) LOG_FATAL_IF(!(cond), "Assertion failed: " #cond)
#define ALOG_ASSERT_T(cond, format, arg...) LOG_FATAL_IF(!(cond), format, ##arg)
#define ALOG_ASSERT_I(cond) LOG_FATAL_IF((cond), "Assertion failed: " #cond)
#define ALOG_ASSERT_N(cond, format, arg...) LOG_FATAL_IF((cond), format, ##arg)
#endif

// ---------------------------------------------------------------------

/**
 * @addtogroup CAPI_SYSTEM_DLOG
 * @{
 */
/**
 * @brief     Send log with priority and tag.
 * @details   for application
 * @since_tizen 2.3
 * @param[in] prio log_priority
 * @param[in] tag tag
 * @param[in] fmt format string
 * @return On success, the function returns the number of bytes written.
 *         On error, a negative errno-style error code
 * @retval #DLOG_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DLOG_ERROR_NOT_PERMITTED Operation not permitted
 * @pre       none
 * @post      none
 * @see       dlog_vprint
 * @code
#include<dlog.h>
int main(void)
{
    int integer = 21;
    char string[] = "test dlog";

	dlog_print(DLOG_INFO, "USR_TAG", "test dlog");
	dlog_print(DLOG_INFO, "USR_TAG", "%s, %d", string, integer);
    return 0;
}
 * @endcode
 */
int dlog_print(log_priority prio, const char *tag, const char *fmt, ...);

/**
 * @brief     Send log with priority, tag and va_list.
 * @details   for application
 * @since_tizen 2.3
 * @param[in] prio log_priority
 * @param[in] tag tag
 * @param[in] fmt format string
 * @param[in] ap va_list
 * @return On success, the function returns the number of bytes written.
 *         On error, a negative errno-style error code
 * @retval #DLOG_ERROR_INVALID_PARAMETER Invalid parameter
 * @retval #DLOG_ERROR_NOT_PERMITTED Operation not permitted
 * @pre       none
 * @post      none
 * @see       dlog_print
 * @code
#include<dlog.h>
void my_debug_print(char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    dlog_vprint(DLOG_INFO, "USR_TAG", format, ap);
    va_end(ap);
}

int main(void)
{
    my_debug_print("%s", "test dlog");
    my_debug_print("%s, %d", "test dlog", 21);
    return 0;
}
 * @endcode
 */
int dlog_vprint(log_priority prio, const char *tag, const char *fmt, va_list ap);
/**
 * @}
 */


/*
 * The stuff in the rest of this file should not be used directly.
 */
/**
 * @internal
 * @brief     Send log.
 * @details   Use LOG(), SLOG(), RLOG() family
 *            not to use __dlog_print() directly
 * @remarks   Must not use this API directly. use macros instead.
 * @param[in] log_id log device id
 * @param[in] prio priority
 * @param[in] tag tag
 * @param[in] fmt format string
 * @return    Operation result
 * @retval    0>= Success
 * @retval    -1  Error
 * @pre       none
 * @post      none
 * @see       __dlog_print
 * @code
    #define LOG_TAG USR_TAG
    #include<dlog.h>
     __dlog_print(LOG_ID_MAIN, DLOG_INFO, USR_TAG, "you must not use this API directly");
 * @endcode
 */
int __dlog_print(log_id_t log_id, int prio, const char *tag, const char *fmt, ...);

/**
 * @internal
 * @brief     Send log with va_list.
 * @details   Use LOG_VA(), SLOG_VA(), RLOG_VA() family
              not to use __dlog_vprint() directly
 * @remarks   Must not use this API directly. use macros instead.
 * @param[in] log_id log device id
 * @param[in] prio priority
 * @param[in] tag tag
 * @param[in] fmt format string
 * @param[in] ap va_list
 * @return    Operation result
 * @retval    0>= Success
 * @retval    -1  Error
 * @pre       none
 * @post      none
 * @see       __dlog_vprint
 * @code
    #define LOG_TAG USR_TAG
    #include<dlog.h>
     __dlog_vprint(LOG_ID_MAIN, DLOG_INFO, USR_TAG, "you must not use this API directly", ap);
 * @endcode
  */
int __dlog_vprint(log_id_t log_id, int prio, const char *tag, const char *fmt, va_list ap);

/**
 * @internal
 * @brief     Send log and assert.
 * @details   Use LOG(), SLOG(), RLOG() family
 *            not to use __dlog_print() directly
 * @remarks   Must not use this API directly. use macros instead.
 * @param[in] log_id log device id
 * @param[in] prio priority
 * @param[in] tag tag
 * @param[in] fmt format string
 * @return    Operation result
 * @retval    0>= Success
 * @retval    -1  Error
 * @pre       none
 * @post      none
 * @see       __dlog_print
 * @code
    #define LOG_TAG USR_TAG
    #include<dlog.h>
     __dlog_print(LOG_ID_MAIN, DLOG_INFO, USR_TAG, "you must not use this API directly");
 * @endcode
 */
int __dlog_print_assert(log_id_t log_id, int prio, const char *tag, const char *fmt, ...);

/**
 * @internal
 * @brief     Send log with va_list and assert.
 * @details   Use LOG_VA(), SLOG_VA(), RLOG_VA() family
              not to use __dlog_vprint() directly
 * @remarks   Must not use this API directly. use macros instead.
 * @param[in] log_id log device id
 * @param[in] prio priority
 * @param[in] tag tag
 * @param[in] fmt format string
 * @param[in] ap va_list
 * @return    Operation result
 * @retval    0>= Success
 * @retval    -1  Error
 * @pre       none
 * @post      none
 * @see       __dlog_vprint
 * @code
    #define LOG_TAG USR_TAG
    #include<dlog.h>
     __dlog_vprint(LOG_ID_MAIN, DLOG_INFO, USR_TAG, "you must not use this API directly", ap);
 * @endcode
  */
int __dlog_vprint_assert(log_id_t log_id, int prio, const char *tag, const char *fmt, va_list ap);
#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* _DLOG_H_*/
