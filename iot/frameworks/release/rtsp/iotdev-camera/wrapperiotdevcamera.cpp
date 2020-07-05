/*#############################################################
 *     File Name	: wrapperiotdevcamera.cpp
 *     Author		: JKZhao
 *     Created Time	: 2020年06月04日 星期四 09时48分32秒
 *     Description	:
 *############################################################*/

#include "CameraRecorder.h"
#include "wrapperiotdevcamera.h"

int wrapper_iotdev_camera_init()
{
	int ret = 0;
	ret = iotdev_camera_init();
	return ret;

}


int wrapper_iotdev_camera_start_by_fifo(int fd, int type)
{
	int ret = 0;
	ret = iotdev_camera_start_by_fifo(fd, type);
	return ret;
}

int wrapper_iotdev_camera_stop()
{
	printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@[%s()]:\t\n", __func__);
	iotdev_camera_stop();
	return 0;
}
