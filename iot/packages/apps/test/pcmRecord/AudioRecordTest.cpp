#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <utils/Log.h>
#include <utils/String16.h>
#include <media/AudioRecord.h>
#include <media/AudioTrack.h>
#include <binder/IPCThreadState.h>

#define LOG_TAG "AudioRecordTest"
#define APP_NAME "com.ingenic.iotdev.audiorecorder"
#define CHECK(x) if(x==NULL) \
    { ALOGE("Null pointer!"); goto exit; }

#define SAMPLE_RATE 8000
#define SAVE_FILE   "/storage/audio_record_test.pcm"

using namespace android;

void *recordBuf         = NULL;
int   recordBufLength   = 0;
int   readLen = 0, writeLen = 0;
FILE*           recordSaveFile      = NULL;
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
    while(1){
        readLen = audioRecord->read(recordBuf, recordBufLength);
        if(readLen > 0){
            writeLen = audioTrack->write(recordBuf, recordBufLength);
            ALOGI("%s record %d bytes.", LOG_TAG, readLen);
            writeLen = fwrite(recordBuf, 1, readLen, recordSaveFile);
            if(writeLen != readLen){
                ALOGE("%s record save error!", LOG_TAG);
            }
        } else {
            ALOGE("%s read error: %d", LOG_TAG, readLen);
            sleep(2);
        }
    }
}

int main(int argc, char* argv[])
{
//    ProcessState::self()->startThreadPool();
    /***/
    status_t                st              = OK;
    size_t                  minFrameCount   = 0;
    uint32_t                sampleRate      = SAMPLE_RATE;
    audio_format_t          audioFormat     = AUDIO_FORMAT_PCM_16_BIT;
    audio_channel_mask_t    channelMask     = AUDIO_CHANNEL_IN_MONO;
    audio_source_t          inputSource     = AUDIO_SOURCE_MIC;

    int             recordTime         = 0;
    uint32_t        updatePeriod        = sampleRate / 10; // \
            0; // cancels new position notification callback.

#if 1
    int   frameSize         = 0;
#endif

#if 0
    st = AudioSystem::setDeviceConnectionState(
            AUDIO_DEVICE_IN_BLUETOOTH_A2DP, AUDIO_POLICY_DEVICE_STATE_AVAILABLE,
            NULL, NULL);
#endif

    /***/
    ALOGI("===========================================");
    ALOGI("%s start!", LOG_TAG);
    if(argc < 2){
        recordTime = 20;
    } else {
        recordTime = atoi(argv[1]);
    }
    ALOGI("%s recordTime %d.", LOG_TAG, recordTime);
    if(recordTime <= 0) return -1;

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

#if 0
    ALOGI("%s setPositionUpdatePeriod.", LOG_TAG);
    st = audioRecord->setPositionUpdatePeriod(updatePeriod);
    if(st != NO_ERROR){
        ALOGE("%s setPositionUpdatePeriod failed!", LOG_TAG);
        return -1;
    }
#endif

    ALOGI("%s openRecordSaveFile.", LOG_TAG);
    recordSaveFile = fopen(SAVE_FILE, "w");
    CHECK(recordSaveFile);

#if 1
    audioTrack = new AudioTrack();
    st = audioTrack->set(AUDIO_STREAM_MUSIC, 
            8000, AUDIO_FORMAT_PCM_16_BIT, AUDIO_CHANNEL_OUT_STEREO,
            0, AUDIO_OUTPUT_FLAG_PRIMARY, NULL, NULL, 0, 0, false,
            AUDIO_SESSION_ALLOCATE, AudioTrack::TRANSFER_SYNC);
    checkAudioRecordSetParameters(st);
    st = audioTrack->initCheck();
    st = audioTrack->start();
#endif

#if 1
    ALOGI("%s malloc recordBuf.", LOG_TAG);
#if 0
    if(channelMask == AUDIO_CHANNEL_IN_STEREO && audioFormat == AUDIO_FORMAT_PCM_16_BIT){
        frameSize = 2 * 2;
    } else {
        ALOGE("frameSize must be set!");
        goto exit;
    }
#else
	frameSize = 1 * 2;
#endif
    recordBufLength = minFrameCount * frameSize;
    ALOGI("recordBufLength %d", recordBufLength);
    recordBuf = calloc(recordBufLength, 1);
    CHECK(recordBuf);

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
    //sleep(recordTime);
    while(1)sleep(~0l);
#if 0
    while(recordTime--){
        usleep(256 * 1000);
        ALOGI("%s recordTime %d.", LOG_TAG, recordTime);
        readLen = audioRecord->read(recordBuf, recordBufLength);
        ALOGW("%s record %d bytes.", LOG_TAG, readLen);
        if(readLen > 0){
            writeLen = fwrite(recordBuf, 1, readLen, recordSaveFile);
            if(writeLen != readLen){
                ALOGE("%s record save error!", LOG_TAG);
            }
        }
    }
#endif

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
