/**
 * @defgroup iotdev-camera
 * @ingroup rtsp
 * @brief Rtsp服务相关的视频数据采集，应用于 m200 / x2000 平台.
 * @section description 说明
 * <pre>
 * iotdev-camera v1.0 (2020.05.15)
 *
 *      Rtsp iotdev-camera 组件提供视频数据.
 *
 *      支持的视频源有: H.264.
 *
 *      支持的视频源获取方式:
 *
 *          1. FIFO (/tmp/camera_h264_fifo).
 *
 *      -----------------------------------------------------
 *
 *      数据流:
 *
 *      1.
 *          CameraDevice(yuyv/i420...) ->
 *
 *          MediaCodec(h264) ->
 *
 *          FIFO
 *
 *      -----------------------------------------------------
 *
 *      TODO:
 *
 *          1. 支持视频源 MP4.
 *
 *          2. 支持视频源获取方式 Stream Buffer 内存读取.
 * </pre>
 */
#ifndef IOTDEV_FRAMEWORKS_BASE_CORE_RTSP_CAMERA_RECORDER_H
#define IOTDEV_FRAMEWORKS_BASE_CORE_RTSP_CAMERA_RECORDER_H

#include <utils/RefBase.h>
#include <camera/Camera.h>
#include <camera/CameraParameters.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/MediaDefs.h>
#include <media/openmax/OMX_IVCommon.h>
#include <media/openmax/OMX_Video.h>
#include <gui/Surface.h>
#include <media/ICrypto.h>

extern "C" {

/**
 * 初始化 Rtsp Camera 组件.
 *
 * @return 0 success, -1 fail.
 */
int iotdev_camera_init();

/**
 * 启动 Rtsp Camera 组件.
 *
 * @param fd FIFO 文件描述符.
 * @param type Camera 运行模式, unused.
 *
 * @return 0 success, -1 fail.
 */
int iotdev_camera_start_by_fifo(int fd, int type);

int iotdev_camera_stop();
}; // extern "C"

namespace android {

struct ABuffer;
struct AMessage;
struct ICrypto;
struct Surface;

class CameraRecorder : 
    public CameraListener {

public:

    CameraRecorder();
    virtual ~CameraRecorder();

    void notify(int32_t msgType, int32_t ext1, int32_t ext2);
    void postData(int32_t msgType, const sp<IMemory>& dataPtr, camera_frame_metadata_t *metadata);
    void postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr);

    int  init();
    void setFd(int fd);
    int  start();
    int  stop();

private:

    sp<Camera>          mCameraDevice;
    sp<MediaCodec>      mEnCodec;
    Vector<sp<ABuffer>> mInputBuffers;
    Vector<sp<ABuffer>> mOutputBuffers;

    int mFd;

};

}; // namespace android

#endif /* IOTDEV_FRAMEWORKS_BASE_CORE_RTSP_CAMERA_RECORDER_H */
