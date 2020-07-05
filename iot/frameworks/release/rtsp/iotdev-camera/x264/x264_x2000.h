#ifndef __X264_X2000_H__
#define __X264_X2000_H__

#include <stdbool.h>

#define DEVICE_PATH "/dev/video1"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define VIDEO_MAX_PLANES 3

struct format_description {
	char *description;
	unsigned int v4l2_format;
	unsigned int v4l2_buffers_count;
	bool v4l2_mplane;
	unsigned int planes_count;
    unsigned int x264_format;
};

struct video_buffer {
	/*source*/
	void *source_map[VIDEO_MAX_PLANES];
	unsigned int source_map_lengths[VIDEO_MAX_PLANES];
	void *source_data[VIDEO_MAX_PLANES];
	unsigned int source_sizes[VIDEO_MAX_PLANES];		// unused?
	unsigned int source_offsets[VIDEO_MAX_PLANES];		// unused.
	unsigned int source_bytesperlines[VIDEO_MAX_PLANES];	// unused.
	unsigned int source_planes_count;
	unsigned int source_buffers_count;

	/*dst*/
	void *destination_map[VIDEO_MAX_PLANES];
	unsigned int destination_map_lengths[VIDEO_MAX_PLANES];
	void *destination_data[VIDEO_MAX_PLANES];
	unsigned int destination_sizes[VIDEO_MAX_PLANES];
	unsigned int destination_offsets[VIDEO_MAX_PLANES];
	unsigned int destination_bytesperlines[VIDEO_MAX_PLANES];
	unsigned int destination_planes_count;
	unsigned int destination_buffers_count;

	int export_fds[VIDEO_MAX_PLANES];
	int request_fd;
};

#endif /* __X264_X2000_H__ */
