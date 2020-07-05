#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <utils/Log.h>
#include <utils/String16.h>
#include <media/AudioRecord.h>
#include <media/AudioTrack.h>
#include <binder/IPCThreadState.h>
#include <g711.h>

extern "C" {
	int audioRecord_start();
	int audioTrack_write(char *pcmBuf, int len);
}

#define LOG_TAG "AudioRecordTest"
#define APP_NAME "com.ingenic.iotdev.audiorecorder"
#define CHECK(x) if(x==NULL) \
    { ALOGE("Null pointer!"); goto exit; }

#define SAMPLE_RATE 8000
#define SAVE_FILE   "/storage/audio_record_test.pcm"
#define G711A_SAVE_FILE   "/storage/audio_g711a_so_record.pcm"

using namespace android;

void *recordBuf         = NULL;
void *recordG711aBuf	= NULL;
int   recordBufLength   = 0;
int   readLen = 0, writeLen = 0;
int   audio_already = 0;
FILE*           recordSaveFile      = NULL;
FILE*           recordG711aSaveFile = NULL;
sp<AudioRecord> audioRecord         = NULL;
sp<AudioTrack>  audioTrack          = NULL;

void AudioRecordCallback(int event, void *user, void *info)
{
    switch(event){
    case AudioRecord::EVENT_MORE_DATA :
        ALOGI("%s EVENT_MORE_DATA.", LOG_TAG);
#if 0
        AudioRecord::Buffer *pBuf = (AudioRecord::Buffer*)info;
        ALOGI("pBuf size %d", pBuf->size);
        if(pBuf->size > 0){
            writeLen = fwrite(pBuf->raw, 1, pBuf->size, recordSaveFile);
            if(writeLen != pBuf->size){
                ALOGE("%s record save error!", LOG_TAG);
            }
        }
        pBuf->size = 0;
#endif
        break;
    case AudioRecord::EVENT_OVERRUN :
        ALOGI("%s EVENT_OVERRUN.", LOG_TAG);
        break;
    case AudioRecord::EVENT_MARKER :
        ALOGI("%s EVENT_MARKER %zu.", LOG_TAG, *((size_t*)info));
        break;
    case AudioRecord::EVENT_NEW_POS :
        ALOGI("%s EVENT_NEW_POS %zu.", LOG_TAG, *((size_t*)info));
        break;
    case AudioRecord::EVENT_NEW_IAUDIORECORD :
        ALOGI("%s EVENT_NEW_IAUDIORECORD.", LOG_TAG);
        break;
    default:
        ALOGE("err event!");
    }
}

void checkAudioRecordSetParameters(status_t st)
{
    switch(st){
    case INVALID_OPERATION :
        ALOGE("AudioRecord is already initialized or record device is already in use!");
        break;
    case BAD_VALUE :
        ALOGE("invalid parameter (channelMask, format, sampleRate...) !");
        break;
    case NO_INIT :
        ALOGE("audio server or audio hardware not initialized!");
        break;
    case PERMISSION_DENIED :
        ALOGE("recording is not allowed for the requesting process!");
        break;
    default:
        ALOGE("err status_t!");
    }
}

static void* recordThreadRun(void* args){
	int count = 0;
    while(1){
		while(audio_already){
			usleep(10);
		}
        readLen = audioRecord->read(recordBuf, recordBufLength);
        if(readLen > 0){
			{
				//G711A
				while(count < (readLen / 2)){
					((char *)recordG711aBuf)[count] = linear2alaw(((short *)recordBuf)[count]);
					count ++;
				}
				count = 0;
				audio_already = 1;
			}

        } else {
            ALOGE("%s read error: %d", LOG_TAG, readLen);
        }
    }
}

int audioTrack_write(char *pcmBuf, int len)
{
	int writeLen = 0;
	writeLen = audioTrack->write(pcmBuf, len);
	return writeLen;
}

int audioRecord_start()
{
//    ProcessState::self()->startThreadPool();
    /***/
    status_t                st              = OK;
    size_t                  minFrameCount   = 0;
    uint32_t                sampleRate      = SAMPLE_RATE;
    audio_format_t          audioFormat     = AUDIO_FORMAT_PCM_16_BIT;
    audio_channel_mask_t    channelMask     = AUDIO_CHANNEL_IN_MONO;
    audio_source_t          inputSource     = AUDIO_SOURCE_MIC;

    uint32_t        updatePeriod        = sampleRate / 10; // \
            0; // cancels new position notification callback.

    int   frameSize         = 0;
    /***/
    ALOGI("===========================================");
    ALOGI("%s start!", LOG_TAG);

    ALOGI("%s create.", LOG_TAG);
    audioRecord = new AudioRecord(String16(APP_NAME));
    CHECK(audioRecord);

    ALOGI("%s getMinFrameCount.", LOG_TAG);
    st = AudioRecord::getMinFrameCount(&minFrameCount, sampleRate, audioFormat, channelMask);
    if(st != NO_ERROR){
        ALOGE("%s getMinFrameCount failed!", LOG_TAG);
        return -1;
    }
    ALOGI("%s: minFrameCount = %zu, sampleRate = %lu, audioFormat = %d, channelMask = %d", 
                    LOG_TAG, minFrameCount, sampleRate, audioFormat, channelMask);

    ALOGI("%s set Parameters.", LOG_TAG);
#if 1
    st = audioRecord->set(inputSource, sampleRate, audioFormat,
                    channelMask, 0, AudioRecordCallback,
                    NULL, 0, false, AUDIO_SESSION_ALLOCATE,
                    AudioRecord::TRANSFER_SYNC);
#else
    st = audioRecord->set(inputSource, sampleRate, audioFormat,
                    channelMask, 0, NULL);
#endif
    if(st != NO_ERROR){
        checkAudioRecordSetParameters(st);
        return -1;
    }
    ALOGI("%s successful intialization!", LOG_TAG);

    ALOGI("%s initCheck.", LOG_TAG);
    st = audioRecord->initCheck();
    if(st != NO_ERROR){
        ALOGE("%s initCheck error: %d !", LOG_TAG, st);
        return -1;
    }

#if 1
    audioTrack = new AudioTrack();
    st = audioTrack->set(AUDIO_STREAM_MUSIC, 
            8000, AUDIO_FORMAT_PCM_16_BIT, AUDIO_CHANNEL_OUT_MONO,
            0, AUDIO_OUTPUT_FLAG_PRIMARY, NULL, NULL, 0, 0, false,
            AUDIO_SESSION_ALLOCATE, AudioTrack::TRANSFER_SYNC);
    checkAudioRecordSetParameters(st);
    st = audioTrack->initCheck();
    st = audioTrack->start();
	char *tmpAudio = (char*)calloc(8192,1);
	for(int i=0; i<10; i++)
		audioTrack->write(tmpAudio, 8192);
#endif


#if 1
    ALOGI("%s malloc recordBuf.", LOG_TAG);

	frameSize = 1 * 2;

    recordBufLength = minFrameCount * frameSize;
    ALOGI("recordBufLength %d", recordBufLength);
    recordBuf = calloc(recordBufLength, 1);
    recordG711aBuf = calloc(recordBufLength/2, 1);
    CHECK(recordBuf);
    CHECK(recordG711aBuf);

    pthread_t pth = -1;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int ret = pthread_create(&pth, &attr, recordThreadRun, NULL);
    if(ret < 0){
        ALOGE("%s recordThreadRun failed!", LOG_TAG);
        goto exit;
    }
#endif

    ALOGI("%s record start.", LOG_TAG);
    st = audioRecord->start();
    if(st != NO_ERROR){
        ALOGE("%s record error %d !", LOG_TAG, st);
        goto exit;
    }
    ALOGI("%s record ok!", LOG_TAG);
    while(1)sleep(~0l);

    ALOGI("%s end!", LOG_TAG);
    ALOGI("===========================================");
exit:
    if(audioRecord != NULL){
        audioRecord->stop();
        //delete audioRecord; // strong pointer not need.
        audioRecord = NULL;
    }
    if(recordSaveFile){
        fflush(recordSaveFile);
        fclose(recordSaveFile);
        recordSaveFile = NULL;
    }
#if 1
    if(recordBuf){
        free(recordBuf);
        recordBuf = NULL;
    }
#endif
    return 0;
}
