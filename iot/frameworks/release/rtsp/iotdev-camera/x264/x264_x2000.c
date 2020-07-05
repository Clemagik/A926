#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <poll.h>

#include <linux/media.h>
#include <linux/videodev2.h>

#define X264_CSP_NV12 0
#define X264_CSP_I420 1

#include "x264_x2000.h"

static int video_fd = -1;
static int hw_inited = 0;
static int buffers_count = 2;
static unsigned int output_type = V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE;
static unsigned int capture_type = V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE;
int keyframe = 0;

struct video_buffer *video_buffers;

struct format_description formats[] = {
    {
        .description		= "nv12",
        .v4l2_format		= V4L2_PIX_FMT_NV12,
        .v4l2_buffers_count	= 2,
        .v4l2_mplane		= true,
        .planes_count		= 2,
        .x264_format = X264_CSP_NV12,
    },
    {
        .description		= "yuv420",
        .v4l2_format		= V4L2_PIX_FMT_YUV420,
        .v4l2_buffers_count	= 3,
        .v4l2_mplane		= true,
        .planes_count		= 3,
        .x264_format = X264_CSP_I420,
    },
};

static bool type_is_mplane(unsigned int type)
{
    switch (type) {
        case V4L2_BUF_TYPE_VIDEO_OUTPUT_MPLANE:
        case V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE:
            return true;

        default:
            return false;
    }
}

static void setup_format(struct v4l2_format *format, unsigned int type,
        unsigned int width, unsigned int height,
        unsigned int pixelformat)
{
    unsigned int sizeimage;

    memset(format, 0, sizeof(*format));
    format->type = type;

    //	sizeimage = type_is_output(type) ? SOURCE_SIZE_MAX : 0;
    if(pixelformat == V4L2_PIX_FMT_H264) {
        sizeimage = width * height * 2;	/*压缩后的数据不可能超过原始帧大小.*/
    } else {
        sizeimage = 0;
    }

    if (type_is_mplane(type)) {
        format->fmt.pix_mp.width = width;
        format->fmt.pix_mp.height = height;
        format->fmt.pix_mp.plane_fmt[0].sizeimage = sizeimage;
        format->fmt.pix_mp.pixelformat = pixelformat;
    } else {
        format->fmt.pix.width = width;
        format->fmt.pix.height = height;
        format->fmt.pix.sizeimage = sizeimage;
        format->fmt.pix.pixelformat = pixelformat;
    }
}

static int set_format(int video_fd, unsigned int type, unsigned int width,
		      unsigned int height, unsigned int pixelformat)
{
    struct v4l2_format format;
    int rc;

    setup_format(&format, type, width, height, pixelformat);

    rc = ioctl(video_fd, VIDIOC_S_FMT, &format);
    if (rc < 0) {
        fprintf(stderr, "Unable to set format for type %d: %s\n", type,
                strerror(errno));
        return -1;
    }

    return 0;
}

static int get_format(int video_fd, unsigned int type, unsigned int *width,
		      unsigned int *height, unsigned int *bytesperline,
		      unsigned int *sizes, unsigned int *planes_count)
{
    struct v4l2_format format;
    unsigned int count;
    unsigned int i;
    int rc;

    memset(&format, 0, sizeof(format));
    format.type = type;

    rc = ioctl(video_fd, VIDIOC_G_FMT, &format);
    if (rc < 0) {
        fprintf(stderr, "Unable to get format for type %d: %s\n", type,
                strerror(errno));
        return -1;
    }

    if (type_is_mplane(type)) {
        count = format.fmt.pix_mp.num_planes;

        if (width != NULL)
            *width = format.fmt.pix_mp.width;

        if (height != NULL)
            *height = format.fmt.pix_mp.height;

        if (planes_count != NULL)
            if (*planes_count > 0 && *planes_count < count)
                count = *planes_count;

        if (bytesperline != NULL)
            for (i = 0; i < count; i++)
                bytesperline[i] =
                    format.fmt.pix_mp.plane_fmt[i].bytesperline;

        if (sizes != NULL)
            for (i = 0; i < count; i++)
                sizes[i] = format.fmt.pix_mp.plane_fmt[i].sizeimage;

        if (planes_count != NULL)
            *planes_count = count;
    } else {
        if (width != NULL)
            *width = format.fmt.pix.width;

        if (height != NULL)
            *height = format.fmt.pix.height;

        if (bytesperline != NULL)
            bytesperline[0] = format.fmt.pix.bytesperline;

        if (sizes != NULL)
            sizes[0] = format.fmt.pix.sizeimage;

        if (planes_count != NULL)
            *planes_count = 1;
    }

    return 0;
}

static int create_buffers(int video_fd, unsigned int type,
			  unsigned int buffers_count, unsigned int *index_base)
{
	struct v4l2_create_buffers buffers;
	int rc;

	memset(&buffers, 0, sizeof(buffers));
	buffers.format.type = type;
	buffers.memory = V4L2_MEMORY_MMAP;
	buffers.count = buffers_count;

	rc = ioctl(video_fd, VIDIOC_G_FMT, &buffers.format);
	if (rc < 0) {
		fprintf(stderr, "Unable to get format for type %d: %s\n", type,
			strerror(errno));
		return -1;
	}

	rc = ioctl(video_fd, VIDIOC_CREATE_BUFS, &buffers);
	if (rc < 0) {
		fprintf(stderr, "Unable to create buffer for type %d: %s\n",
			type, strerror(errno));
		return -1;
	}

	if (index_base != NULL)
		*index_base = buffers.index;

	return 0;
}

static int query_buffer(int video_fd, unsigned int type, unsigned int index,
			unsigned int *lengths, unsigned int *offsets,
			unsigned int buffers_count)
{
	struct v4l2_plane planes[buffers_count];
	struct v4l2_buffer buffer;
	unsigned int i;
	int rc;

	memset(planes, 0, sizeof(planes));
	memset(&buffer, 0, sizeof(buffer));

	buffer.type = type;
	buffer.memory = V4L2_MEMORY_MMAP;
	buffer.index = index;
	buffer.length = buffers_count;
	buffer.m.planes = planes;

	rc = ioctl(video_fd, VIDIOC_QUERYBUF, &buffer);
	if (rc < 0) {
		fprintf(stderr, "Unable to query buffer: %s\n",
			strerror(errno));
		return -1;
	}

	if (type_is_mplane(type)) {
		if (lengths != NULL)
			for (i = 0; i < buffer.length; i++)
				lengths[i] = buffer.m.planes[i].length;

		if (offsets != NULL)
			for (i = 0; i < buffer.length; i++)
				offsets[i] = buffer.m.planes[i].m.mem_offset;
	} else {
		if (lengths != NULL)
			lengths[0] = buffer.length;

		if (offsets != NULL)
			offsets[0] = buffer.m.offset;
	}

	return 0;
}

static int queue_buffer(int video_fd, unsigned int type,
			uint64_t ts, unsigned int index, unsigned int* sizes,
			unsigned int buffers_count)
{
	struct v4l2_plane planes[buffers_count];
	struct v4l2_buffer buffer;
	unsigned int i;
	int rc;

    /*fprintf(stderr, "\033[33m(l:%d, f:%s, F: %s) %d %s\033[0m\n", __LINE__, __func__, __FILE__, 0, "");*/
	memset(planes, 0, sizeof(planes));
	memset(&buffer, 0, sizeof(buffer));

	buffer.type     = type;
	buffer.memory   = V4L2_MEMORY_MMAP;
	buffer.index    = index;
	buffer.length   = buffers_count;
	buffer.m.planes = planes;
    /* fprintf(stderr, "\033[33m buffer.type = %d | buffer.memory = %d | buffer.index = %d | buffer.length = %d | buffer.m.planes = %x |\033[0m\n", */
    /*         buffer.type, buffer.memory, buffer.index, buffer.length, buffer.m.planes); */

	for (i = 0; i < buffers_count; i++)
		if (type_is_mplane(type))
			buffer.m.planes[i].bytesused = sizes[i];
		else
			buffer.bytesused = sizes[0];

	buffer.timestamp.tv_usec = ts / 1000;
	buffer.timestamp.tv_sec = ts / 1000000000ULL;

    /*fprintf(stderr, "\033[33m(l:%d, f:%s, F: %s) %d %s\033[0m\n", __LINE__, __func__, __FILE__, 0, "");*/
	rc = ioctl(video_fd, VIDIOC_QBUF, &buffer);
	if (rc < 0) {
		fprintf(stderr, "Unable to queue buffer: %s\n",
			strerror(errno));
		return -1;
	}
    /*fprintf(stderr, "\033[33m(l:%d, f:%s, F: %s) %d %s\033[0m\n", __LINE__, __func__, __FILE__, 0, "");*/

	return 0;
}

static int dequeue_buffer(int video_fd, unsigned int type,
			  unsigned int *index, unsigned int buffers_count,
			  bool *error, unsigned int *bytesused)
{
	struct v4l2_plane planes[buffers_count];
	struct v4l2_buffer buffer;
	int rc;

	memset(planes, 0, sizeof(planes));
	memset(&buffer, 0, sizeof(buffer));

	/*对于dequeue_buffer 只需要指定 type*/
	buffer.type = type;
	buffer.memory = V4L2_MEMORY_MMAP;
	buffer.length = buffers_count;
	buffer.m.planes = planes;

	rc = ioctl(video_fd, VIDIOC_DQBUF, &buffer);
	if (rc < 0) {
		fprintf(stderr, "Unable to dequeue buffer: %s, type: %d\n",
			strerror(errno), type);
		return -1;
	}

    keyframe = buffer.flags & V4L2_BUF_FLAG_KEYFRAME;
    /* fprintf(stderr, "\033[33m keyframe = %d |\033[0m\n", keyframe); */

	if(bytesused) {
		*bytesused = buffer.m.planes[0].bytesused;
	}

	*index = buffer.index;

	if (error != NULL)
		*error = !!(buffer.flags & V4L2_BUF_FLAG_ERROR);

	return 0;
}

static int set_stream(int video_fd, unsigned int type, bool enable)
{
	enum v4l2_buf_type buf_type = type;
	int rc;

	rc = ioctl(video_fd, enable ? VIDIOC_STREAMON : VIDIOC_STREAMOFF,
		   &buf_type);
	if (rc < 0) {
		fprintf(stderr, "Unable to %sable stream: %s\n",
			enable ? "en" : "dis", strerror(errno));
		return -1;
	}

	return 0;
}

static int x2000_x264_init(int w, int h, unsigned int format)
{
	struct video_buffer *buffer;
	unsigned int source_map_offsets[VIDEO_MAX_PLANES];
	unsigned int destination_map_offsets[VIDEO_MAX_PLANES];
	unsigned int i, j;
	int rc;
    struct format_description *selected_format = NULL;

	video_buffers = malloc(buffers_count * sizeof(struct video_buffer));
	memset(video_buffers, 0, buffers_count * sizeof(struct video_buffer));

    video_fd = open(DEVICE_PATH, O_RDWR | O_NONBLOCK, 0);
    if (video_fd < 0) {
        fprintf(stderr, "Unable to open video node: %s\n",
                strerror(errno));
        goto error;
    }

    // in buffer.
    for(i = 0; i < ARRAY_SIZE(formats); i++) {
        if(format == formats[i].x264_format) {
            selected_format = &formats[i];
            fprintf(stderr, "i ========== %d\n", i);
            break;
        }
    }
    if (selected_format == NULL) {
        fprintf(stderr,
                "Unable to find any supported destination format\n");
        goto error;
    }

    rc = set_format(video_fd, output_type, w, h, selected_format->v4l2_format);
    if (rc < 0) {
        fprintf(stderr, "Unable to set source format\n");
        goto error;
    }

    // create in buffers.
    rc = create_buffers(video_fd, output_type, buffers_count, NULL);
    if (rc < 0) {
        fprintf(stderr, "Unable to create source buffers\n");
        goto error;
    }

    for (i = 0; i < buffers_count; i++) {
        buffer = &(video_buffers[i]);

        rc = query_buffer(video_fd, output_type, i, buffer->source_map_lengths,
                source_map_offsets, selected_format->planes_count);
        if (rc < 0) {
            fprintf(stderr, "Unable to request source buffer\n");
            goto error;
        }

        fprintf(stderr, "\033[33m selected_format->v4l2_buffers_count = %d |\033[0m\n", selected_format->v4l2_buffers_count);
        for(j = 0; j < selected_format->v4l2_buffers_count; j++) {
            buffer->source_map[j] = mmap(NULL,
                    buffer->source_map_lengths[j],
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    video_fd,
                    source_map_offsets[j]);

            if (buffer->source_map[j] == MAP_FAILED) {
                fprintf(stderr, "Unable to map source buffer\n");
                goto error;
            }

            buffer->source_data[j] = buffer->source_map[j];
            buffer->source_sizes[j] = buffer->source_map_lengths[j];

            fprintf(stderr, "buffer[%d]@%p, buffer->source_map[%d]: %x, buffer->source_sizes[%d]: %x\n", i, buffer, j, buffer->source_map[j], j, buffer->source_sizes[j]);
        }

        buffer->source_planes_count = selected_format->planes_count;
        buffer->source_buffers_count = buffer->source_planes_count;
    }


    // out buffer.
    rc = set_format(video_fd, capture_type, w, h, V4L2_PIX_FMT_H264);
    if (rc < 0) {
        fprintf(stderr, "Unable to set destination format\n");
        goto error;
    }

    /* create out Buffer. */
    rc = create_buffers(video_fd, capture_type, buffers_count, NULL);
    if (rc < 0) {
        fprintf(stderr, "Unable to create destination buffers\n");
        goto error;
    }

    for (i = 0; i < buffers_count; i++) {
        buffer = &(video_buffers[i]);

        rc = query_buffer(video_fd, capture_type, i,
                buffer->destination_map_lengths,
                destination_map_offsets, 1);
        if (rc < 0) {
            fprintf(stderr,
                    "Unable to request destination buffer\n");
            goto error;
        }

        buffer->destination_map[0] = mmap(NULL,
                buffer->destination_map_lengths[0],
                PROT_READ | PROT_WRITE,
                MAP_SHARED,
                video_fd,
                destination_map_offsets[0]);
        if(buffer->destination_map[0] == MAP_FAILED) {
            fprintf(stderr,
                    "Unable to map destination buffer\n");
            goto error;
        }

        buffer->destination_data[0] = buffer->destination_map[0];
        buffer->destination_sizes[0] = buffer->destination_map_lengths[0];

        fprintf(stderr, "buffer[%d]@%p, buffer->destination_map[%d]: %x, buffer->destination_sizes[%d]: %x\n", i, buffer, 0, buffer->destination_map[0], 0, buffer->destination_sizes[0]);
    }

    rc = set_stream(video_fd, output_type, true);
    if (rc < 0) {
        fprintf(stderr, "Unable to enable source stream\n");
        goto error;
    }

    rc = set_stream(video_fd, capture_type, true);
    if (rc < 0) {
        fprintf(stderr, "Unable to enable destination stream\n");
        goto error;
    }

    printf("\033[33m(l:%d, f:%s, F: %s) %d %s\033[0m\n", __LINE__, __func__, __FILE__, 0, "init done");
    return 0;

error:
    return -1;
}

int video_engine_encode(unsigned char * src_buf, int w, int h, unsigned int format, int fd)
{
    bool source_error, destination_error;
    int rc;
    int ret;
    static unsigned int index = 0;
    static int flag = 1;

    if (hw_inited == 0) {
        printf("\033[33m(l:%d, f:%s, F: %s) %d %s\033[0m\n", __LINE__, __func__, __FILE__, 0, "frist init");
        x2000_x264_init(w, h, format);
        hw_inited = 1;
    }

    memcpy(video_buffers[index].source_data[0], src_buf, w * h);
    memcpy(video_buffers[index].source_data[1], src_buf + w * h, w * h / 2);

    /* fprintf(stderr, "\033[33m video_buffers[%d].source_sizes = %x | video_buffers[%d].source_planes_count = %d |\033[0m\n", index, video_buffers[index].source_sizes, index, video_buffers[index].source_planes_count); */

    rc = queue_buffer(video_fd, output_type, 0, index, video_buffers[index].source_sizes,
            video_buffers[index].source_planes_count);
    if(rc < 0) {
        fprintf(stderr, "Unable to queue source buffer\n");
        return -1;
    }

    rc = queue_buffer(video_fd, capture_type, 0, index, video_buffers[index].destination_sizes, 1);
    if (rc < 0) {
        fprintf(stderr, "Unable to queue destination buffer\n");
        return -1;
    }

    int out_index = 0;
    rc = dequeue_buffer(video_fd, output_type, &out_index, video_buffers[index].source_buffers_count,
            &source_error, NULL);
    if (rc < 0) {
        fprintf(stderr, "Unable to dequeue source buffer\n");
        return -1;
    }

    int cap_index = 0;
    unsigned int bytesused = 0;
    rc = dequeue_buffer(video_fd, capture_type, &cap_index, 1, &destination_error, &bytesused);
    if(rc < 0) {
        fprintf(stderr, "Unable to dequeue destination buffer\n");
        return -1;
    }

#if 0
    fprintf(stderr, "\033[33m bytesused = %d |size_limit = %d \033[0m\n", bytesused, size_limit);

    if(bytesused > size_limit){
        fprintf(stderr, "\033[31mout of size limit\033[0m\n");
        return -1;
    };
#endif

    /*memcpy(dst_buf, video_buffers[index].destination_data[0], bytesused);*/
    write(fd, video_buffers[index].destination_data[0], bytesused);

#if 0
    if (flag) {
        FILE *fd = fopen("/tmp/in", "wb");
        fwrite(src_buf, 1, w * h *3/2, fd);
        fclose(fd);

        fd = fopen("/tmp/out", "wb");
        fwrite(video_buffers[index].destination_data[0], 1, bytesused, fd);
        fclose(fd);

        /* flag = 0; */
    }
    if (flag) {
        FILE *fd = fopen("/tmp/out", "ab");
        fseek(fd, 0, SEEK_END);
        fwrite(video_buffers[index].destination_data[0], 1, bytesused, fd);
        fclose(fd);
    }
#endif

    index ++;
    index %= buffers_count;

    return 0;
}

int video_engine_encode_frist(unsigned char * src_buf, int w, int h, unsigned int format, unsigned char * dst_buf, unsigned int * size, unsigned int size_limit)
{
    bool source_error, destination_error;
    int rc;
    int ret;
    static unsigned int index = 0;
    static int flag = 1;

    if (hw_inited == 0) {
        printf("\033[33m(l:%d, f:%s, F: %s) %d %s\033[0m\n", __LINE__, __func__, __FILE__, 0, "frist init");
        x2000_x264_init(w, h, format);
        hw_inited = 1;
    }

    memcpy(video_buffers[0].source_data[0], src_buf, w * h);
    memcpy(video_buffers[0].source_data[1], src_buf + w * h, w * h / 2);

    /* fprintf(stderr, "\033[33m video_buffers[%d].source_sizes = %x | video_buffers[%d].source_planes_count = %d |\033[0m\n", index, video_buffers[index].source_sizes, index, video_buffers[index].source_planes_count); */
    fprintf(stderr, "\033[31m%d\033[0m\n", __LINE__);

    rc = queue_buffer(video_fd, output_type, 0, 0, video_buffers[0].source_sizes,
            video_buffers[0].source_planes_count);
    if(rc < 0) {
        fprintf(stderr, "Unable to queue source buffer\n");
        return -1;
    }

    rc = queue_buffer(video_fd, output_type, 0, 1, video_buffers[1].source_sizes,
            video_buffers[1].source_planes_count);
    if(rc < 0) {
        fprintf(stderr, "Unable to queue source buffer\n");
        return -1;
    }

    fprintf(stderr, "\033[31m%d\033[0m\n", __LINE__);
    rc = queue_buffer(video_fd, capture_type, 0, 0, video_buffers[0].destination_sizes, 1);
    if (rc < 0) {
        fprintf(stderr, "Unable to queue destination buffer\n");
        return -1;
    }

    fprintf(stderr, "\033[31m%d\033[0m\n", __LINE__);
    rc = queue_buffer(video_fd, capture_type, 0, 1, video_buffers[1].destination_sizes, 1);
    if (rc < 0) {
        fprintf(stderr, "Unable to queue destination buffer\n");
        return -1;
    }

    fprintf(stderr, "\033[31m%d\033[0m\n", __LINE__);
    int out_index = 0;
    rc = dequeue_buffer(video_fd, output_type, &out_index, video_buffers[0].source_buffers_count,
            &source_error, NULL);
    if (rc < 0) {
        fprintf(stderr, "Unable to dequeue source buffer\n");
        return -1;
    }

    fprintf(stderr, "\033[31m%d\033[0m\n", __LINE__);
    int cap_index = 0;
    unsigned int bytesused = 0;
    rc = dequeue_buffer(video_fd, capture_type, &cap_index, 1, &destination_error, &bytesused);
    if(rc < 0) {
        fprintf(stderr, "Unable to dequeue destination buffer\n");
        return -1;
    }

    fprintf(stderr, "\033[31m%d\033[0m\n", __LINE__);
    fprintf(stderr, "\033[33m bytesused = %d |size_limit = %d \033[0m\n", bytesused, size_limit);

    if(bytesused > size_limit){
        fprintf(stderr, "\033[31mout of size limit\033[0m\n");
        return -1;
    };

    memcpy(dst_buf, video_buffers[index].destination_data[0], bytesused);
    *size = bytesused;

    cap_index = 1;
    bytesused = 0;
    rc = dequeue_buffer(video_fd, capture_type, &cap_index, 1, &destination_error, &bytesused);
    if(rc < 0) {
        fprintf(stderr, "Unable to dequeue destination buffer\n");
        return -1;
    }

    if (flag) {
        FILE *fd = fopen("/tmp/out", "ab");
        fseek(fd, 0, SEEK_END);
        fwrite(video_buffers[index].destination_data[0], 1, bytesused, fd);
        fclose(fd);
    }

    index ++;
    index %= buffers_count;

    return 0;
}

static int video_engine_stop(void)
{
    unsigned int i, j;
    int rc;

    rc = set_stream(video_fd, output_type, false);
    if (rc < 0) {
        fprintf(stderr, "Unable to enable source stream\n");
        return -1;
    }

    rc = set_stream(video_fd, capture_type, false);
    if (rc < 0) {
        fprintf(stderr, "Unable to enable destination stream\n");
        return -1;
    }

    for (i = 0; i < buffers_count; i++) {

        for(j = 0; j < video_buffers[i].source_buffers_count; j++) {
            if(video_buffers[i].source_map[j] == NULL)
                break;

            munmap(video_buffers[i].source_map[j],
                    video_buffers[i].source_map_lengths[j]);
        }

        if (video_buffers[i].destination_map[0] == NULL)
            break;

        munmap(video_buffers[i].destination_map[0],
                video_buffers[i].destination_map_lengths[0]);
    }

    free(video_buffers);

    return 0;
error:
    return -1;
}


int x2000_x264_deinit()
{
	int rc;
    if (hw_inited) {
        rc = video_engine_stop();
        if (rc < 0) {
            fprintf(stderr, "Unable to stop video engine\n");
            goto error;
        }
    }
    return 0;
error:
    return -1;
}

