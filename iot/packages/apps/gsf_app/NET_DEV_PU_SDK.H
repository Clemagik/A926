#ifndef __NET_DEV_PU_SDK_H__
#define __NET_DEV_PU_SDK_H__

#define NET_PEER_NAME_LEN   32
#define NET_PEER_IP_LEN     16
#define NET_HOST_NAME_LEN   128
#define NET_DEV_IVS_ID_LEN  32
#define NET_DEV_IVS_PASSWD  32

#define NET_DEV_SDK_VER_OEMNO(v) (((v)&0xff000000) >> 24)
#define NET_DEV_SDK_VER_MAJOR(v) (((v)&0x00ff0000) >> 16)
#define NET_DEV_SDK_VER_MINOR(v) (((v)&0x0000ff00) >> 8)
#define NET_DEV_SDK_VER_DEVEL(v) (((v)&0x000000ff) >> 0)

typedef struct _NET_DEV_PEER NET_DEV_PEER;
struct _NET_DEV_PEER
{
    char Name[NET_PEER_NAME_LEN];
    char Peer[NET_PEER_IP_LEN];
    char Local[NET_PEER_IP_LEN];
};

typedef struct _NET_DEV_IVS_CONFIG NET_DEV_IVS_CONFIG;
struct _NET_DEV_IVS_CONFIG
{
    char Ip[NET_HOST_NAME_LEN];   /* IVS-CMS */
    int  Port;
    char DevID[NET_DEV_IVS_ID_LEN];
    char Passwd[NET_DEV_IVS_PASSWD];
};

typedef struct _NET_DEV_MEDIA NET_DEV_MEDIA;
typedef struct _NET_DEV_DATACHN NET_DEV_DATACHN;

typedef struct _NET_DEV_SDK_FUNCS NET_DEV_SDK_FUNCS;
struct _NET_DEV_SDK_FUNCS
{
    /* 调试信息输出*/
    void (*Print) (int Level, const char *Message);
    /* 设置设备参数 */
    int  (*SetP) (NET_DEV_PEER *Peer, int Chnl, int PID, void *In, void *Out);
    /* 获取设备参数 */
    int  (*GetP) (NET_DEV_PEER *Peer, int Chnl, int PID, void *In, void *Out);
    /* POST控制消息 */
    int  (*PMsg) (NET_DEV_PEER *Peer, int Chnl, int PID, void *In, void *Out);
    /* 获取实时流媒体参数 */
    int  (*LProbe) (int Chnl, int Level, void *MediaParamIn);             /* 获取媒体参数,可能被多次调用 */
    /* 打开实时流 */
    int  (*LOpen) (NET_DEV_MEDIA *Media, int Chnl, int Level);
    /* 开始播放实时流 */
    int  (*LPlay) (NET_DEV_MEDIA *Media);                                  /* 此后可以使用 NET_DEV_SDK_MEDIA_LPUSH()发送实时流 */
    /* 播放时控制 */
    int  (*LCtrl) (NET_DEV_MEDIA *Media, unsigned Cmd, void *Data);        /* 控制：Cmd=1 I帧请求 */
    /* 关闭实时流 */
    void (*LClose) (NET_DEV_MEDIA *Media);
    /* 获取历史流媒体参数 */
    int  (*HProbe) (int Chnl, int Level, unsigned StartTime, unsigned EndTime,
        unsigned RecType, char *Property, void *MediaParamIn);            /* 获取媒体参数,可能被多次调用 */
    /* 打开历史流 */
    int  (*HOpen) (NET_DEV_MEDIA *Media, int Chnl, int Level,
        unsigned StartTime, unsigned EndTime, unsigned RecType, char *Property);
    /* 开始播放历史流 */
    int  (*HPlay) (NET_DEV_MEDIA *Media);
    /* 暂停历史流 */
    int  (*HPause) (NET_DEV_MEDIA *Media);
    /* 定位播放位置 */
    int  (*HLseek) (NET_DEV_MEDIA *Media, unsigned Timestamp);
    /* 关闭历史流 */
    void (*HClose) (NET_DEV_MEDIA *Media);
    /* 获取历史流数据帧 */
    int  (*HPull) (NET_DEV_MEDIA *Media, void **PFrame);
    /* 打开数据透传通道 */
    int  (*DCOpen) (NET_DEV_DATACHN *Dc);
    /* 接收透传通道数据 */
    int  (*DCRecv) (NET_DEV_DATACHN *Dc, void *Ddata, int Size);
    /* 关闭数据透传通道 */
    void (*DCClose) (NET_DEV_DATACHN *Dc);
};

/* 初始化SDK */
int NET_DEV_SDK_INIT(int CmdPort, int DataPort,  NET_DEV_SDK_FUNCS *Ops);

/* 启动SDK */
int NET_DEV_SDK_START( void );

/* 启动IVS(平台)服务 */
int NET_DEV_SDK_START_IVS_SERVICE(NET_DEV_IVS_CONFIG *Conf);

/* 停止IVS(平台)服务 */
void NET_DEV_SDK_STOP_IVS_SERVICE( void );

/* 获取SDK版本号 */
int NET_DEV_SDK_GET_VERSION( void );

/* 上传告警 */
void NET_DEV_SDK_SUBMIT_ALARM(void *Alarm);

/* 设置用户自定义数据 */
void NET_DEV_SDK_MEDIA_SET_UDATA(NET_DEV_MEDIA *Media, void *Data);

/* 获取用户自定义数据 */
void *NET_DEV_SDK_MEDIA_GET_UDATA(NET_DEV_MEDIA *Media);

/* 推送实时流 */
int NET_DEV_SDK_MEDIA_LPUSH(NET_DEV_MEDIA *Media, void *frm);

/* 流故障通告SDK */
void NET_DEV_SDK_MEDIA_KILL(NET_DEV_MEDIA *Media);

/* 增加流句柄引用计数 */
void NET_DEV_SDK_MEDIA_REF(NET_DEV_MEDIA *Media);

/* 减少流句柄引用计数 */
void NET_DEV_SDK_MEDIA_UNREF(NET_DEV_MEDIA *Media);

/* 分配一帧的空间，用于历史流pull */
void *NET_DEV_SDK_FRAME_ALLOC(int DataSize, int NalCount);

/* 释放一帧的空间 */
void NET_DEV_SDK_FRAME_FREE(void *PFrame);

/* 设置用户自定义数据 */
void NET_DEV_SDK_DC_SET_UDATA(NET_DEV_DATACHN *Dc, void *Data);

/* 获取用户自定义数据 */
void *NET_DEV_SDK_DC_GET_UDATA(NET_DEV_DATACHN *Dc);

/* 增加DC句柄的引用计数 */
void NET_DEV_SDK_DC_REF(NET_DEV_DATACHN *Dc);

/* 减少DC句柄的引用计数 */
void NET_DEV_SDK_DC_UNREF(NET_DEV_DATACHN *Dc);

/* 向透传通道发送数据 */
int NET_DEV_SDK_DC_SEND(NET_DEV_DATACHN *Dc, void *Data, int Size);

/* 通道故障通告SDK */
void NET_DEV_SDK_DC_KILL(NET_DEV_DATACHN *Dc);

#endif /* __NET_DEV_PU_SDK_H__ */
