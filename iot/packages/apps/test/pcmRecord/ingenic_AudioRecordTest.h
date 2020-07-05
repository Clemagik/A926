#ifndef __INGENIC_AUDIORECORDTEST_H__
#define __INGENIC_AUDIORECORDTEST_H__
extern "C" {
	int audioRecord_start();
	int audioTrack_write(char *pcmBuf, int len);
};
#endif
