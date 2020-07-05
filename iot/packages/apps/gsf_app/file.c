#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "file.h"

/*--------------- 720P数据帧格式 begin ---------------------*/
typedef 	unsigned char BYTE;
typedef 	unsigned short WORD;
typedef 	unsigned int  DWORD;
typedef		unsigned long long DWORD64;

typedef struct _HI_VIDEO_INFO_S
{
	WORD				wImageWidth;	//图像宽度
	WORD				wImageHeight;	//图像高度
	BYTE				byEncodeType;	//视频编码格式0-h264 1-mjpeg 2-jpeg
	BYTE				byFrameRate;	//帧率(保留)
	BYTE				byPal;			//制式		0-n制1-pal制
	BYTE				byRes[1];
}HI_VIDEO_INFO_S,*LPHI_VIDEO_INFO_S;

typedef struct _HI_AUDIO_INFO_S
{
	BYTE		byAudioSamples;			//采样率	0--8k 1--16k 2-32k
	BYTE		byEncodeType;			//音频编码格式0--pcm 1-g711a 2-g711u 3--g726
	BYTE		byAudioChannels;		//通道数		暂只支持1	
	BYTE		byAudioBits;			//位数			16bit
	BYTE		byRes[4];
}HI_AUDIO_INFO_S, *LPHI_AUDIO_INFO_S;

typedef struct _HI_FRAME_HEAD_S
{
	WORD		wFrameFlag;				//0x3448 magic data
	BYTE		wFrameType;				//I-0x7 p--0x8 b--0xb A--0xa
	BYTE		byRsvd[1];				//保留
	DWORD		dwFrameNo;				//帧号
	DWORD		dwTime;					//系统时间
	DWORD		dwFrameSize;			//数据流长度
	DWORD64		dw64TimeStamp;			//时间戳 
	union
	{
		HI_VIDEO_INFO_S	unVideoInfo;
		HI_AUDIO_INFO_S	unAudioInfo;
	};
}HI_FRAME_HEAD_S, *LPHI_FRAME_HEAD_S;

/*--------------- 720P数据帧格式 end ---------------------*/

int file_open(char *file_name)
{
    int fd;
    if((fd = open(file_name, O_RDONLY)) < 0)
    {
        printf("err: open file %s\n", file_name);
    }
    return fd;
}

int file_close(int fd)
{
    if(fd) close(fd);
    return 0;
}

int file_read(int fd, char *buf, int size, int *fr_type)
{

    HI_FRAME_HEAD_S *fr_head = (HI_FRAME_HEAD_S*)buf;

    int ret = -1;
    int fr_size = 0;

    ret = read(fd, fr_head, sizeof(HI_FRAME_HEAD_S));
	if(ret != sizeof(HI_FRAME_HEAD_S))
	{
		if(lseek(fd, 0, SEEK_SET) < 0)
        {
            printf("err: lseek fd:%d\n", fd);
        }
        return (-1);
	}

    if((fr_head->dwFrameSize) && (fr_head->dwFrameSize < size))
    {
        fr_size = fr_head->dwFrameSize;
        //I-0x7 p--0x8 b--0xb A--0xa
        *fr_type = (fr_head->wFrameType == 0x7)?0x01:((fr_head->wFrameType == 0x8)?0x02:0x03);     
        //printf("read: fd:%d, type:%d, size:%d\n", fd, *fr_type, fr_size);
        
        if(read(fd, buf + 0 /*sizeof(HI_FRAME_HEAD_S)*/, fr_size) != fr_size)
        {
            printf("err: read fd:%d, size:%d\n", fd, fr_size);
            if(lseek(fd, 0, SEEK_SET) < 0)
            {
                printf("err: lseek fd:%d\n", fd);
            }
            return (-1);
        }
        else
        {
            return (fr_size);
        }
    }
    return -1;
}







