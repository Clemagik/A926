#ifdef __cplusplus
extern "C" {
#endif

	int wrapper_iotdev_camera_init();
	int wrapper_iotdev_camera_start_by_fifo(int fd, int type);
	int wrapper_iotdev_camera_stop();

#ifdef __cplusplus
}
#endif
