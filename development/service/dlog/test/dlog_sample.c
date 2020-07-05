/*
 * Use dlogutil to print your LOG information.
 *
 * Print *LOGV: < dlogutil *:V >
 * Print RLOG* and SECURE_RLOG*: < dlogutil -b radio > / < dlogutil -b radio *:V >
 * Clear log: < dlogutil -c > / < dlogutil -b radio -c >
 *
 * Run < dlogutil --help > to know more how to use dlogutil.
 *
 * Note: 1. Must define LOG_TAG before include <dlog.h>.
 *       2. Must define DEBUG_ENABLE before #include <dlog.h> if you want to print *LOGD.
 */

#define LOG_TAG "test_dlog" /* Used to identify that the LOG information belongs to the current module */
#define DEBUG_ENABLE
#include <dlog.h>
#include <sys/uio.h>
#include <string.h>

/*
 * Run This demo, and run dlogutil to view the LOG information.
 */

int main(int argc,char *argv[])
{
	int test_int = 0x1234;
	char *test_str = "dlog-test";
	char *buf;

	ALOGV("ALOGV ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	ALOGD("ALOGD ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	ALOGI("ALOGI ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	ALOGW("ALOGW ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	ALOGE("ALOGE ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	ALOGF("ALOGF ------ test_str: %s, test_int: 0x%x", test_str, test_int);

	RLOGV("RLOGV ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	RLOGD("RLOGD ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	RLOGI("RLOGI ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	RLOGW("RLOGW ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	RLOGE("RLOGE ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	RLOGF("RLOGF ------ test_str: %s, test_int: 0x%x", test_str, test_int);

	SLOGV("SLOGV ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SLOGD("SLOGD ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SLOGI("SLOGI ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SLOGW("SLOGW ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SLOGE("SLOGE ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SLOGF("SLOGF ------ test_str: %s, test_int: 0x%x", test_str, test_int);

	SECURE_ALOGV("SECURE_ALOGV ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SECURE_ALOGD("SECURE_ALOGD ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SECURE_ALOGI("SECURE_ALOGI ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SECURE_ALOGW("SECURE_ALOGW ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SECURE_ALOGE("SECURE_ALOGE ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SECURE_ALOGF("SECURE_ALOGF ------ test_str: %s, test_int: 0x%x", test_str, test_int);

	SECURE_RLOGV("SECURE_RLOGV ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SECURE_RLOGD("SECURE_RLOGD ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SECURE_RLOGI("SECURE_RLOGI ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SECURE_RLOGW("SECURE_RLOGW ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SECURE_RLOGE("SECURE_RLOGE ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	SECURE_RLOGF("SECURE_RLOGF ------ test_str: %s, test_int: 0x%x", test_str, test_int);

	LOGV("LOGV ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	LOGD("LOGD ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	LOGI("LOGI ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	LOGW("LOGW ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	LOGE("LOGE ------ test_str: %s, test_int: 0x%x", test_str, test_int);
	LOGF("LOGF ------ test_str: %s, test_int: 0x%x", test_str, test_int);

	ALOG_ASSERT(buf, "ALOG_ASSERT ------ test_str: %s, test_int: 0x%x", test_str, test_int);

	return 0;
}
