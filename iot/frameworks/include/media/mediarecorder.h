/*
 ** Copyright (C) 2008 The Android Open Source Project
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 **
 ** limitations under the License.
 */

/**
 * @defgroup media 媒体编解码模块
 */

/**
 * @defgroup MediaRecorder
 * @ingroup media
 * @brief 媒体编码模块，包含音频视频编码 
 * @section 介绍 
 * 当前支持的音频编码格式有aac amr-wb(采样率16k 单声道) amr-nb(采样率8k 单声道) OGG Vorbis。
 * 
 * 支持的视频编码格式有X264
 *
 * iotdev内置了音频编码器AudioRecord 和视频编码器VideoRecord。使用方法介绍
 * @code
 * //录制音频文件
 * ./usr/bin/AudioRecord
 * AudioRecord编码格式为AMR WB ，采样率16K 单声道 保存位置为/storage/AUDIO_xxx.3gpp
 *
 * 当然，您也可以通过iotdev提供的API来定制自己的音频编码器,以获取更多的属性设置
 * @endcode
 *
 * @code
 * //录制视频文件
 * /usr/bin/VideoRecord

 * 当然，如果您想修改更多的参数选项，或者想要过去音频PCM数据或者视频YUV数据，可以通过iotdev提供的媒体编码API来定制化自己的应用
 * @endcode
 *
 * @section about 相关库和头文件
 * 使用iotdev提供的媒体编码接口需要引用的库为libmedia.so 
 *  
 * 使用此接口需要引用的头文件为 mediarecorder.h Camera.h RefBase.h
 * @section audiodemo 录音示例代码
 * @code
 * // 录音:iotdev默认音频采样率8000 单声道
 *   int outFD = open("/storage/AUDIO_test.3gpp", O_CREAT | O_RDWR);
 *   MediaRecorder recorder = new MediaRecorder();
 * 
 *   if (recorder->initCheck() != NO_ERROR) {
 * 	ALOGE("Unable to initialize media recorder");
 * 	return -1;
 *   }
 * 
 *   recorder->setAudioSource(AUDIO_SOURCE_MIC); //iotdev暂只支持此模式
 *   recorder->setOutputFormat(OUTPUT_FORMAT_THREE_GPP); //更多保存文件格式选择请查看mediarecorder.h
 *   recorder->setOutputFile(outFD, 0, 0);
 * 
 *   recorder->setAudioEncoder(AUDIO_ENCODER_AMR_WB); //更多音频编码格式选择请查看mediarecorder.h
 *   recorder->prepare();
 *   recorder->start();
 * @endcode
 *
 * @section audioset 参数设置示例代码
 * @code
 * // 设置录音的采样率,通道数量 压缩率
 *    //aac adts when <=12200, latm when > 12200, now we use latm
 *    const char *audioBitRate = "audio-param-encoding-bitrate=13200"; 
 *    const char *audioChannel = "audio-param-number-of-channels=1";//iotdev目前最高支持2个声道
 *    const char *audioSamplingRate = "audio-param-sampling-rate=16000";//iotdev目前采样率为8k或者16k
 * 
 *    recorder->setParameters(String8(audioBitRate, strlen(audioBitRate)));
 *    recorder->setParameters(String8(audioChannel, strlen(audioChannel)));
 *    recorder->setParameters(String8(audioSamplingRate, strlen(audioSamplingRate)));
 * @endcode
 * @attention setParameters()函数必须在调用prepare()函数之前被调用
 * @attention AMR 编码格式里 只支持单声道，不支持双声道
 *
 * @section videodemo 录像示例代码
 * @code
 *
 * // 打开相机 
 *   sp<Camera> camera = Camera::connect(0, "myApp", Camera::USE_CALLING_UID);	
 *   // make sure camera hardware is alive
 *   if (camera->getStatus() != NO_ERROR) {
 * 	ALOGE("Camera initialization failed");
 *       return -1;
 *   }
 *   res = camera->startPreview(); //开始预览
 *   if (res != NO_ERROR) {
 * 	ALOGE("Err : Start preview failed!!");
 * 	return -1;	
 *   }
 * 
 *   CameraParameters  mParameters;
 *   mParameters.unflatten(mCameraDevice->getParameters());
 *   mParameters.setVideoSize(mWidth, mHeight);
 *   mParameters.setPreviewSize(mWidth, mHeight);
 *   mParameters.setPreviewFrameRate(videoFrameRate);
 *   mCameraDevice->setParameters(mParameters.flatten());
 * 
 *   //实例化MediaRecord
 *   sp<MediaRecorder> mRecorder = new MediaRecorder();
 *   if (mRecorder->initCheck() != NO_ERROR) {
 * 	ALOGE("Unable to initialize media recorder");
 * 	return false;
 *   }
 * 
 *   //配置保存文件名称和类型
 *   int fd = open("/storage/VID_test.mp4", O_CREAT | O_RDWR);
 *   mRecorder->setOutputFormat(OUTPUT_FORMAT_MPEG_4);
 *   mRecorder->setOutputFile(fd, 0, 0);
 * 
 *   //绑定camera
 *   mRecorder->setCamera(mCameraDevice->remote(), mCameraDevice->getRecordingProxy());
 * 
 *   //配置视频参数
 *   mRecorder->setVideoSource(VIDEO_SOURCE_CAMERA);
 *   mRecorder->setVideoEncoder(VIDEO_ENCODER_H264);
 *   mRecorder->setVideoFrameRate(videoFrameRate);
 *   mRecorder->setVideoSize(mWidth, mHeight);
 * 
 *   //配置音频参数，如果是无声录像，可以不配置
 *   mRecorder->setAudioSource(AUDIO_SOURCE_CAMCORDER);
 *   mRecorder->setAudioEncoder(AUDIO_ENCODER_AAC);
 *   //aac adts when <=12200, latm when > 12200, now we use latm
 *   const char *audioBitRate = "audio-param-encoding-bitrate=13200";
 *   const char *audioChannel = "audio-param-number-of-channels=1";
 *   const char *audioSamplingRate = "audio-param-sampling-rate=16000";
 *   mRecorder->setParameters(String8(audioBitRate, strlen(audioBitRate)));
 *   mRecorder->setParameters(String8(audioChannel, strlen(audioChannel)));
 *   mRecorder->setParameters(String8(audioSamplingRate, strlen(audioSamplingRate)));
 * 
 *   //启动录像
 *   mRecorder->prepare();
 *   mRecorder->start();
 *
 * @endcode
 */

#ifndef ANDROID_MEDIARECORDER_H
#define ANDROID_MEDIARECORDER_H

#include <utils/Log.h>
#include <utils/threads.h>
#include <utils/List.h>
#include <utils/Errors.h>
#include <media/IMediaRecorderClient.h>
#include <media/IMediaDeathNotifier.h>

namespace android {

class Surface;
class IMediaRecorder;
class ICamera;
class ICameraRecordingProxy;
class IGraphicBufferProducer;
struct PersistentSurface;
class Surface;

typedef void (*media_completion_f)(status_t status, void *cookie);

enum video_source {
    VIDEO_SOURCE_DEFAULT = 0,
    VIDEO_SOURCE_CAMERA = 1,
    VIDEO_SOURCE_SURFACE = 2,

    VIDEO_SOURCE_LIST_END  // must be last - used to validate audio source type
};

//Please update media/java/android/media/MediaRecorder.java if the following is updated.
enum output_format {
    OUTPUT_FORMAT_DEFAULT = 0,
    OUTPUT_FORMAT_THREE_GPP = 1,
    OUTPUT_FORMAT_MPEG_4 = 2,


    OUTPUT_FORMAT_AUDIO_ONLY_START = 3, // Used in validating the output format.  Should be the
                                        //  at the start of the audio only output formats.

    /* These are audio only file formats */
    OUTPUT_FORMAT_RAW_AMR = 3, //to be backward compatible
    OUTPUT_FORMAT_AMR_NB = 3,
    OUTPUT_FORMAT_AMR_WB = 4,
    OUTPUT_FORMAT_AAC_ADIF = 5,
    OUTPUT_FORMAT_AAC_ADTS = 6,

    OUTPUT_FORMAT_AUDIO_ONLY_END = 7, // Used in validating the output format.  Should be the
                                      //  at the end of the audio only output formats.

    /* Stream over a socket, limited to a single stream */
    OUTPUT_FORMAT_RTP_AVP = 7,

    /* H.264/AAC data encapsulated in MPEG2/TS */
    OUTPUT_FORMAT_MPEG2TS = 8,

    /* VP8/VORBIS data in a WEBM container */
    OUTPUT_FORMAT_WEBM = 9,

    OUTPUT_FORMAT_LIST_END // must be last - used to validate format type
};

enum audio_encoder {
    AUDIO_ENCODER_DEFAULT = 0,
    AUDIO_ENCODER_AMR_NB = 1,
    AUDIO_ENCODER_AMR_WB = 2,
    AUDIO_ENCODER_AAC = 3,
    AUDIO_ENCODER_HE_AAC = 4,
    AUDIO_ENCODER_AAC_ELD = 5,
    AUDIO_ENCODER_VORBIS = 6,

    AUDIO_ENCODER_LIST_END // must be the last - used to validate the audio encoder type
};

enum video_encoder {
    VIDEO_ENCODER_DEFAULT = 0,
    VIDEO_ENCODER_H263 = 1,
    VIDEO_ENCODER_H264 = 2,
    VIDEO_ENCODER_MPEG_4_SP = 3,
    VIDEO_ENCODER_VP8 = 4,

    VIDEO_ENCODER_LIST_END // must be the last - used to validate the video encoder type
};

/*
 * The state machine of the media_recorder.
 */
enum media_recorder_states {
    // Error state.
    MEDIA_RECORDER_ERROR                 =      0,

    // Recorder was just created.
    MEDIA_RECORDER_IDLE                  = 1 << 0,

    // Recorder has been initialized.
    MEDIA_RECORDER_INITIALIZED           = 1 << 1,

    // Configuration of the recorder has been completed.
    MEDIA_RECORDER_DATASOURCE_CONFIGURED = 1 << 2,

    // Recorder is ready to start.
    MEDIA_RECORDER_PREPARED              = 1 << 3,

    // Recording is in progress.
    MEDIA_RECORDER_RECORDING             = 1 << 4,
};

// The "msg" code passed to the listener in notify.
enum media_recorder_event_type {
    MEDIA_RECORDER_EVENT_LIST_START               = 1,
    MEDIA_RECORDER_EVENT_ERROR                    = 1,
    MEDIA_RECORDER_EVENT_INFO                     = 2,
    MEDIA_RECORDER_EVENT_LIST_END                 = 99,

    // Track related event types
    MEDIA_RECORDER_TRACK_EVENT_LIST_START         = 100,
    MEDIA_RECORDER_TRACK_EVENT_ERROR              = 100,
    MEDIA_RECORDER_TRACK_EVENT_INFO               = 101,
    MEDIA_RECORDER_TRACK_EVENT_LIST_END           = 1000,
};

/*
 * The (part of) "what" code passed to the listener in notify.
 * When the error or info type is track specific, the what has
 * the following layout:
 * the left-most 16-bit is meant for error or info type.
 * the right-most 4-bit is meant for track id.
 * the rest is reserved.
 *
 * | track id | reserved |     error or info type     |
 * 31         28         16                           0
 *
 */
enum media_recorder_error_type {
    MEDIA_RECORDER_ERROR_UNKNOWN                   = 1,

    // Track related error type
    MEDIA_RECORDER_TRACK_ERROR_LIST_START          = 100,
    MEDIA_RECORDER_TRACK_ERROR_GENERAL             = 100,
    MEDIA_RECORDER_ERROR_VIDEO_NO_SYNC_FRAME       = 200,
    MEDIA_RECORDER_TRACK_ERROR_LIST_END            = 1000,
};

// The codes are distributed as follow:
//   0xx: Reserved
//   8xx: General info/warning
//
enum media_recorder_info_type {
    MEDIA_RECORDER_INFO_UNKNOWN                   = 1,

    MEDIA_RECORDER_INFO_MAX_DURATION_REACHED      = 800,
    MEDIA_RECORDER_INFO_MAX_FILESIZE_REACHED      = 801,

    // All track related informtional events start here
    MEDIA_RECORDER_TRACK_INFO_LIST_START           = 1000,
    MEDIA_RECORDER_TRACK_INFO_COMPLETION_STATUS    = 1000,
    MEDIA_RECORDER_TRACK_INFO_PROGRESS_IN_TIME     = 1001,
    MEDIA_RECORDER_TRACK_INFO_TYPE                 = 1002,
    MEDIA_RECORDER_TRACK_INFO_DURATION_MS          = 1003,

    // The time to measure the max chunk duration
    MEDIA_RECORDER_TRACK_INFO_MAX_CHUNK_DUR_MS     = 1004,

    MEDIA_RECORDER_TRACK_INFO_ENCODED_FRAMES       = 1005,

    // The time to measure how well the audio and video
    // track data is interleaved.
    MEDIA_RECORDER_TRACK_INTER_CHUNK_TIME_MS       = 1006,

    // The time to measure system response. Note that
    // the delay does not include the intentional delay
    // we use to eliminate the recording sound.
    MEDIA_RECORDER_TRACK_INFO_INITIAL_DELAY_MS     = 1007,

    // The time used to compensate for initial A/V sync.
    MEDIA_RECORDER_TRACK_INFO_START_OFFSET_MS      = 1008,

    // Total number of bytes of the media data.
    MEDIA_RECORDER_TRACK_INFO_DATA_KBYTES          = 1009,

    MEDIA_RECORDER_TRACK_INFO_LIST_END             = 2000,
};

// ----------------------------------------------------------------------------
// ref-counted object for callbacks
class MediaRecorderListener: virtual public RefBase
{
public:
    virtual void notify(int msg, int ext1, int ext2) = 0;
};

class MediaRecorder : public BnMediaRecorderClient,
                      public virtual IMediaDeathNotifier
{
public:
    MediaRecorder(const String16& opPackageName);
    ~MediaRecorder();

    void        died();
    status_t    initCheck();
    status_t    setCamera(const sp<ICamera>& camera, const sp<ICameraRecordingProxy>& proxy);
    status_t    setPreviewSurface(const sp<IGraphicBufferProducer>& surface);
    status_t    setVideoSource(int vs);
    status_t    setAudioSource(int as);
    status_t    setOutputFormat(int of);
    status_t    setVideoEncoder(int ve);
    status_t    setAudioEncoder(int ae);
    status_t    setOutputFile(int fd, int64_t offset, int64_t length);
    status_t    setVideoSize(int width, int height);
    status_t    setVideoFrameRate(int frames_per_second);
    status_t    setParameters(const String8& params);
    status_t    setListener(const sp<MediaRecorderListener>& listener);
    status_t    setClientName(const String16& clientName);
    status_t    prepare();
    status_t    getMaxAmplitude(int* max);
    status_t    start();
    status_t    stop();
    status_t    reset();
    status_t    init();
    status_t    close();
    status_t    release();
    void        notify(int msg, int ext1, int ext2);
    status_t    setInputSurface(const sp<PersistentSurface>& surface);
    sp<IGraphicBufferProducer>     querySurfaceMediaSourceFromMediaServer();

private:
    void                    doCleanUp();
    status_t                doReset();

    sp<IMediaRecorder>          mMediaRecorder;
    sp<MediaRecorderListener>   mListener;

    // Reference to IGraphicBufferProducer
    // for encoding GL Frames. That is useful only when the
    // video source is set to VIDEO_SOURCE_GRALLOC_BUFFER
    sp<IGraphicBufferProducer>  mSurfaceMediaSource;

    media_recorder_states       mCurrentState;
    bool                        mIsAudioSourceSet;
    bool                        mIsVideoSourceSet;
    bool                        mIsAudioEncoderSet;
    bool                        mIsVideoEncoderSet;
    bool                        mIsOutputFileSet;
    Mutex                       mLock;
    Mutex                       mNotifyLock;
};

};  // namespace android

#endif // ANDROID_MEDIARECORDER_H
