
#include <dlog.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Send a simple string to the log.
 */
int __android_log_buf_write(int bufID, int prio, const char *tag, const char *text);
int __android_log_buf_print(int bufID, int prio, const char *tag, const char *fmt, ...)
#if defined(__GNUC__)
    __attribute__((__format__(printf, 4, 5)))
#endif
    ;

#ifndef LOG_PRI
#define LOG_PRI(priority, tag, ...) \
    android_printLog(priority, tag, __VA_ARGS__)
#endif


/****************log event*********/
#define android_btWriteLog(tag, type, payload, len) \
    __android_log_btwrite(tag, type, payload, len)

#ifndef LOG_EVENT_INT
#define LOG_EVENT_INT(_tag, _value) {                                       \
        int intBuf = _value;                                                \
        (void) android_btWriteLog(_tag, EVENT_TYPE_INT, &intBuf,            \
            sizeof(intBuf));                                                \
    }
#endif
#ifndef LOG_EVENT_LONG
#define LOG_EVENT_LONG(_tag, _value) {                                      \
        long long longBuf = _value;                                         \
        (void) android_btWriteLog(_tag, EVENT_TYPE_LONG, &longBuf,          \
            sizeof(longBuf));                                               \
    }
#endif
//#ifndef LOG_EVENT_STRING
//#define LOG_EVENT_STRING(_tag, _value)                                      \
        (void) __android_log_bswrite(_tag, _value);
//#endif


#ifdef __cplusplus
}
#endif
