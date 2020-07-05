/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @defgroup Camera
 * @ingroup media
 * @brief 相机控制接口,包括开启关闭相机，预览，设置相机参数，获取YUV数据 
 *
 * iotdev拍照 录像都使用此接口
 * @section about 相关库和头文件
 * 使用此接口需要引用的库为libcamera_client.so 
 *
 * 使用此接口需要引用的头文件为 Camera.h CameraParameters.h
 *
 * @section opencamera 开关示例代码
 *
 * @code
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
 *   camera->stopPreview();
 *   camera->disconnect();
 * 
 * @endcode
 *
 * @section getdata 获取YUV数据 
 *
 * camera模块里提供了CameraListener类，您可以通过此类来监听录像或者拍照时的一些状态信息，也可以获取到实时的YUV数据
 * @code
 *  class myListener : public CameraListener {
 * 
 *  public:
 *   myListener();
 *   ~myListener();
 * 
 *   void notify(int32_t msgType, int32_t ext1, int32_t ext2){
 *      switch (msgType) {
 *      case CAMERA_ERROR_UNKNOWN:
 *           ALOGE("camera has a error!"); //未知错误
 * 	    break;
 * 
 *      case CAMERA_ERROR_PREVIEW:
 *           ALOGE("camera has a error!"); //预览失败	
 * 	    break;
 * 
 *      case CAMERA_ERROR_TAKEPICTURE:
 *     	    ALOGE("Err : Camera subSystem has Errors!"); //拍照失败
 *   	    break; 
 * 
 *      case CAMERA_ERROR_HAL_STORE:
 *           ALOGE("store jpeg file failed!"); //拍照时，JPEG硬编码或系统存储图片失败
 *   	    break;
 * 
 *      default:
 *   	break;
 *      }
 *   }
 * 
 *   void postData(int32_t msgType, const sp<IMemory>& dataPtr, camera_frame_metadata_t *metadata){
 *       switch (msgType) {
 *       case CAMERA_MSG_PREVIEW_FRAME: 
 *           saveYUVData(dataPtr); //dataPtr为YUV数据
 *           break;
 *       case CAMERA_MSG_COMPRESSED_IMAGE: 
 *          if(dataPtr == NULL){
 *            //拍照时硬压缩JPEG文件成功完成
 *            camera->stopPreview();
 *            camera->disconnect(); 
 *            exit(0);
 *          }else{
 * 	      saveJPEGData(dataPtr); //dataPtr为压缩后的jpeg数据
 *          }
 * 	    break;
 *       default:
 *           break;
 *       }
 *   }
 * 
 *   void postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr){
 *   	 //
 *   }
 * @endcode
 *
 * @section takepicture 拍照
 * 
 * iotdev自带了拍照应用TakePicture 。用户可以执行/usr/bin/TakePicture命令完成一次拍照 。
 * 用户需要配置版级文件device.conf来设定iotdev默认的拍照分辨率 。
 * 当然用户可以使用iotdev_SDK的Camera接口编写自己的拍照功能 。
 *
 * @subsection 拍照示例代码 
 * @code
 *   sp<Camera> camera = Camera::connect(0, "myApp", Camera::USE_CALLING_UID);	
 *   // make sure camera hardware is alive
 *   if (camera->getStatus() != NO_ERROR) {
 * 	ALOGE("Camera initialization failed");
 *       return -1;
 *   }
 *
 *   listener = new myListener();
 *   camera->setListener(listener);
 *
 *   int picWidth = 1920;
 *   int picHeight = 1072
 *   Vector<Size> supportedPSizes;
 *   CameraParameters params;
 *   params.unflatten(camera->getParameters());
 *  
 *   //设置的分辨率必须是iotdev所支持的,否则设置会无效
 *   params.getSupportedPictureSizes(supportedPSizes);
 *   for (Vector<Size>::iterator it = supportedPSizes.begin(); 
 *  	 it != supportedPSizes.end(); it++) {
 *  	if (it->width == picWidth && it->height == picHeight) {
 *  	    params.setPictureSize(picWidth,picHeight);
 *  	}
 *  }
 *  params.set("jpeg_halstore_path", "/storage/PIC_sample.jpeg"); //设置jpeg图片存储的路径
 *   res = camera->setParameters(params.flatten());
 *   if (res != NO_ERROR) {
 * 	ALOGE("Err : Set parameters failed!!");
 * 	return -1;
 *   }
 *   res = camera->startPreview(); //开始预览
 *   if (res != NO_ERROR) {
 * 	ALOGE("Err : Start preview failed!!");
 * 	return -1;	
 *   }
 *   camera->takePicture(CAMERA_MSG_COMPRESSED_IMAGE); 
 * @endcode
 * @attention camera的takePicture()是异步过程。结果会通过之前注册的listener返回。
 *
 */

#ifndef ANDROID_HARDWARE_CAMERA_H
#define ANDROID_HARDWARE_CAMERA_H

#include <utils/Timers.h>
#include <gui/IGraphicBufferProducer.h>
#include <system/camera.h>
#include <camera/ICameraClient.h>
#include <camera/ICameraRecordingProxy.h>
#include <camera/ICameraRecordingProxyListener.h>
#include <camera/ICameraService.h>
#include <camera/ICamera.h>
#include <camera/CameraBase.h>

namespace android {

class Surface;
class String8;
class String16;

// ref-counted object for callbacks
class CameraListener: virtual public RefBase
{
public:
    virtual void notify(int32_t msgType, int32_t ext1, int32_t ext2) = 0;
    virtual void postData(int32_t msgType, const sp<IMemory>& dataPtr,
                          camera_frame_metadata_t *metadata) = 0;
    virtual void postDataTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr) = 0;
};

class Camera;

template <>
struct CameraTraits<Camera>
{
    typedef CameraListener        TCamListener;
    typedef ICamera               TCamUser;
    typedef ICameraClient         TCamCallbacks;
    typedef status_t (ICameraService::*TCamConnectService)(const sp<ICameraClient>&,
                                                           int, const String16&, int,
                                                           /*out*/
                                                           sp<ICamera>&);
    static TCamConnectService     fnConnectService;
};


class Camera :
    public CameraBase<Camera>,
    public BnCameraClient
{
public:
    enum {
        USE_CALLING_UID = ICameraService::USE_CALLING_UID
    };

            // construct a camera client from an existing remote
    static  sp<Camera>  create(const sp<ICamera>& camera);
    static  sp<Camera>  connect(int cameraId,
                                const String16& clientPackageName,
                                int clientUid);

    static  status_t  connectLegacy(int cameraId, int halVersion,
                                     const String16& clientPackageName,
                                     int clientUid, sp<Camera>& camera);

            virtual     ~Camera();

            status_t    reconnect();
            status_t    lock();
            status_t    unlock();

            // pass the buffered IGraphicBufferProducer to the camera service
            status_t    setPreviewTarget(const sp<IGraphicBufferProducer>& bufferProducer);

            // start preview mode, must call setPreviewTarget first
            status_t    startPreview();

            // stop preview mode
            void        stopPreview();

            // get preview state
            bool        previewEnabled();

            // start recording mode, must call setPreviewTarget first
            status_t    startRecording();

            // stop recording mode
            void        stopRecording();

            // get recording state
            bool        recordingEnabled();

            // release a recording frame
            void        releaseRecordingFrame(const sp<IMemory>& mem);

            // autoFocus - status returned from callback
            status_t    autoFocus();

            // cancel auto focus
            status_t    cancelAutoFocus();

            // take a picture - picture returned from callback
            status_t    takePicture(int msgType);

            // set preview/capture parameters - key/value pairs
            status_t    setParameters(const String8& params);

            // get preview/capture parameters - key/value pairs
            String8     getParameters() const;

            // send command to camera driver
            status_t    sendCommand(int32_t cmd, int32_t arg1, int32_t arg2);

            // tell camera hal to store meta data or real YUV in video buffers.
            status_t    storeMetaDataInBuffers(bool enabled);

            void        setListener(const sp<CameraListener>& listener);
            void        setRecordingProxyListener(const sp<ICameraRecordingProxyListener>& listener);

            // Configure preview callbacks to app. Only one of the older
            // callbacks or the callback surface can be active at the same time;
            // enabling one will disable the other if active. Flags can be
            // disabled by calling it with CAMERA_FRAME_CALLBACK_FLAG_NOOP, and
            // Target by calling it with a NULL interface.
            void        setPreviewCallbackFlags(int preview_callback_flag);
            status_t    setPreviewCallbackTarget(
                    const sp<IGraphicBufferProducer>& callbackProducer);

            sp<ICameraRecordingProxy> getRecordingProxy();

    // ICameraClient interface
    virtual void        notifyCallback(int32_t msgType, int32_t ext, int32_t ext2);
    virtual void        dataCallback(int32_t msgType, const sp<IMemory>& dataPtr,
                                     camera_frame_metadata_t *metadata);
    virtual void        dataCallbackTimestamp(nsecs_t timestamp, int32_t msgType, const sp<IMemory>& dataPtr);

    class RecordingProxy : public BnCameraRecordingProxy
    {
    public:
        RecordingProxy(const sp<Camera>& camera);

        // ICameraRecordingProxy interface
        virtual status_t startRecording(const sp<ICameraRecordingProxyListener>& listener);
        virtual void stopRecording();
        virtual void releaseRecordingFrame(const sp<IMemory>& mem);

    private:
        sp<Camera>         mCamera;
    };

protected:
                        Camera(int cameraId);
                        Camera(const Camera&);
                        Camera& operator=(const Camera);

    sp<ICameraRecordingProxyListener>  mRecordingProxyListener;

    friend class        CameraBase;
};

}; // namespace android

#endif
