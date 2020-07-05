#include <utils/Log.h>
#include <binder/IPCThreadState.h>

#include "CameraRecorder.h"

extern "C" {
extern int video_engine_encode(unsigned char * src_buf, int w, int h, unsigned int format, char *gsf_frame_buffer);
extern int x2000_x264_deinit();
}

extern char gsf_frame_buffer[];

#define LOG_TAG "CameraRecorder"

#define CAMERA_NAME "com.ingenic.brillo.camera_rtsp"
#define CHECK_RES(res) { if(res < 0){ALOGE("error!");return res;} }

android::sp<android::CameraRecorder> mRecorder;

int iotdev_camera_init()
{
    int res = 0;
    mRecorder = new android::CameraRecorder();
    if(mRecorder == NULL)
        return -1;
    res = mRecorder->init();
    CHECK_RES(res);
    return 0;
}

int iotdev_camera_start_by_fifo(int fd, int type)
{
//	printf("[%s()][%s][+%d]:\t\n", __func__, __FILE__, __LINE__);
    if(fd < 0){
        ALOGE("Invaild Parameter: fd %d", fd);
        return -1;
    }
//	printf("[%s()][%s][+%d]:\t\n", __func__, __FILE__, __LINE__);
//	sleep(15);
//    mRecorder->setFd(fd);
    return mRecorder->start();
}

int iotdev_camera_stop()
{
	mRecorder->stop();
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ [%s()][+%d]:\t\n", __func__, __LINE__);
	mRecorder = NULL;
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ [%s()][+%d]:\t\n", __func__, __LINE__);
	return 0;
}

namespace android {

CameraRecorder::CameraRecorder()
{
    ProcessState::self()->startThreadPool();
}

CameraRecorder::~CameraRecorder()
{
//	  mCameraDevice->setListener(NULL);
//    mCameraDevice->stopRecording();
//    mCameraDevice = NULL;
#if 0
    mEnCodec->signalEndOfInputStream();
    mEnCodec->stop();
    mEnCodec->release();
    mEnCodec = NULL;
#else
//    x2000_x264_deinit();
//	printf("\n\n@@@@@@@@@@@@@@@@@@@@@@@@ Camera recorder_release [+%d]\n", __LINE__);
#endif
    mFd = -1;
}

int CameraRecorder::stop()
{
	printf("\n\n@@@@@@@@@@@@@@@@@@@@@@@@ CameraRecorder->stop() [+%d]\n", __LINE__);
	mCameraDevice->setListener(NULL);
	printf("\n\n@@@@@@@@@@@@@@@@@@@@@@@@ CameraRecorder->stop() [+%d]\n", __LINE__);
    mCameraDevice->stopRecording();
	printf("\n\n@@@@@@@@@@@@@@@@@@@@@@@@ CameraRecorder->stop() [+%d]\n", __LINE__);
    mCameraDevice = NULL;
	printf("\n\n@@@@@@@@@@@@@@@@@@@@@@@@ CameraRecorder->stop() [+%d]\n", __LINE__);
#if 0
    mEnCodec->signalEndOfInputStream();
    mEnCodec->stop();
    mEnCodec->release();
    mEnCodec = NULL;
#else
    x2000_x264_deinit();
	printf("\n\n@@@@@@@@@@@@@@@@@@@@@@@@ CameraRecorder->stop() [+%d]\n", __LINE__);
#endif
    mFd = -1;
	return 0;
}

void CameraRecorder::notify(int32_t msgType, int32_t ext1, int32_t ext2)
{}

void CameraRecorder::postData(int32_t msgType, const sp<IMemory>& dataPtr, camera_frame_metadata_t *metadata)
{}

//#include <stdio.h>
//#include <fcntl.h>
//#include <time.h>
#include <sys/ioctl.h>
void CameraRecorder::postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr)
{
    //ALOGI("postDataTimestamp");
    //printf("postDataTimestamp\n");
    static ssize_t offset;
    static size_t  size;
    static size_t  index = 0;
    static int res = 0;
    static uint32_t flags = MediaCodec::BUFFER_FLAG_SYNCFRAME;
    static sp<IMemoryHeap> heap = NULL;
    heap = dataPtr->getMemory(&offset, &size);
    static uint8_t* heapBase;
    heapBase = (uint8_t*)heap->base();
    static sp<ABuffer> inputBuf = NULL, outputBuf = NULL;

    if(heapBase != NULL) {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(heapBase + offset);
#if 0
        res = mEnCodec->dequeueInputBuffer(&index, 0);
        inputBuf = mInputBuffers.editItemAt(index);
        if(inputBuf->capacity() < size){
            ALOGE("inputBuf too small, %d/%d", inputBuf->capacity(), size);
            return;
        } else {
            memcpy(inputBuf->data(), data, size);
        }
        if(inputBuf->capacity() > size)
            memset(inputBuf->data()+size, 0, inputBuf->capacity()-size-1);
        //ALOGI("mEnCodec %d %d %lu", index, size, timestamp);
        res = mEnCodec->queueInputBuffer(index, 0, size, timestamp, flags);
        inputBuf = NULL;
    }
    if(res == OK && mFd >= 0){
        res = mEnCodec->dequeueOutputBuffer(&index, (size_t*)&offset, &size, &timestamp, &flags);
        outputBuf = mOutputBuffers.itemAt(index);
        //ALOGI("outputBuf size %d", outputBuf->size());
        //static int h264Fd = open("/storage/t.h264", O_WRONLY | O_CREAT | O_TRUNC);
        //mFd = h264Fd;

        static int read_bytes = 0;
        ioctl(mFd, FIONREAD, &read_bytes);
        //printf("read_bytes %d\n", read_bytes);
        if(read_bytes > 10000) {
            printf("out of buffer %d\n", read_bytes);
            mEnCodec->releaseOutputBuffer(index);
            outputBuf = NULL;
            return;
        }
        write(mFd, (void*)outputBuf->data(), outputBuf->size());
        mEnCodec->releaseOutputBuffer(index);
        outputBuf = NULL;
    } else {
        ALOGE("outputBuf failed: %d/%d", res, mFd);
#else
//    res = video_engine_encode((unsigned char*)data, 1280, 720, 0, mFd);
    res = video_engine_encode((unsigned char*)data, 1280, 720, 0, gsf_frame_buffer);
    if(res < 0)
        ALOGE("video_engine_encode error!");
#endif
    }

#if 0
static time_t rawtime;
static struct tm * timeinfo;
time ( &rawtime );
timeinfo = localtime ( &rawtime );
printf ( "%s", asctime (timeinfo) );
#endif
    //printf("outputBuf size %d\n", outputBuf->size());
}

int CameraRecorder::init()
{
    int pictureW = 1280, pictureH = 720;

    int mNumberOfCameras = Camera::getNumberOfCameras();
    if(mNumberOfCameras == 0){
        ALOGE("can not get camera device!");
        return -1;
    }
    String16 clientName(CAMERA_NAME, strlen(CAMERA_NAME));
    struct CameraInfo cameraInfo;
    for(int i=0; i<mNumberOfCameras; i++){
        Camera::getCameraInfo(i, &cameraInfo);
        if(cameraInfo.facing == CAMERA_FACING_BACK)
            mCameraDevice = Camera::connect(i, clientName, Camera::USE_CALLING_UID);
        if(mCameraDevice != NULL)
            break;
    }
    if(mCameraDevice == NULL){
        ALOGE("Fail to connect to camera service!");
        return -1;
    }

    // make sure camera hardware is alive
    if (mCameraDevice->getStatus() != NO_ERROR){
        ALOGE("Camera initialization failed!");
        return -1;
    }

    // CameraListener.
    mCameraDevice->setListener(this);

    CameraParameters mParameters;
    mParameters.unflatten(mCameraDevice->getParameters());
    mParameters.setPreviewSize(pictureW, pictureH);
    mParameters.setPreviewFrameRate(30); // fps
    mCameraDevice->setParameters(mParameters.flatten());

    int res = 0;
#if 0
    sp<ALooper> mLooper = new ALooper;
    mLooper->setName("MediaCodec_looper");
    mLooper->start(false, true, PRIORITY_FOREGROUND);

    mEnCodec = MediaCodec::CreateByType(mLooper, "video/avc", true, &res);
    if(res != OK || mEnCodec == NULL){
        ALOGE("mEnCodec create failed!");
        return -1;
    }

    // mOutputFormat
    sp<AMessage> format = new AMessage;
    format->setString("mime", MEDIA_MIMETYPE_VIDEO_AVC);
    format->setInt32("width", pictureW);
    format->setInt32("height", pictureH);
    format->setInt32("stride", pictureW);
    format->setInt32("slice-height", pictureH);
    format->setInt32("color-format", OMX_COLOR_FormatYUV420Planar);
    format->setInt32("bitrate", 5000000);
    format->setInt32("bitrate-mode", OMX_Video_ControlRateConstant);
    format->setInt32("frame-rate", 30);
    format->setInt32("i-frame-interval", 15);
    format->setInt32("intra-refresh-mode", OMX_VIDEO_IntraRefreshCyclic);
    format->setInt32("prepend-sps-pps-to-idr-frames", 1);

    res = mEnCodec->configure(format, NULL, NULL, MediaCodec::CONFIGURE_FLAG_ENCODE);
#endif

    return res;
}

inline void CameraRecorder::setFd(int fd)
{
    mFd = fd;
}

int CameraRecorder::start()
{
    int res = 0;
//	printf("[%s()][%s][+%d]:\t\n", __func__, __FILE__, __LINE__);
    res = mCameraDevice->startRecording();
    CHECK_RES(res);

#if 0
    res = mEnCodec->start();
    CHECK_RES(res);

    res = mEnCodec->getInputBuffers(&mInputBuffers);
    CHECK_RES(res);
    res = mEnCodec->getOutputBuffers(&mOutputBuffers);
    CHECK_RES(res);
#endif

    return res;
}

} // namespace android
