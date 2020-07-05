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
    /* ������Ϣ���*/
    void (*Print) (int Level, const char *Message);
    /* �����豸���� */
    int  (*SetP) (NET_DEV_PEER *Peer, int Chnl, int PID, void *In, void *Out);
    /* ��ȡ�豸���� */
    int  (*GetP) (NET_DEV_PEER *Peer, int Chnl, int PID, void *In, void *Out);
    /* POST������Ϣ */
    int  (*PMsg) (NET_DEV_PEER *Peer, int Chnl, int PID, void *In, void *Out);
    /* ��ȡʵʱ��ý����� */
    int  (*LProbe) (int Chnl, int Level, void *MediaParamIn);             /* ��ȡý�����,���ܱ���ε��� */
    /* ��ʵʱ�� */
    int  (*LOpen) (NET_DEV_MEDIA *Media, int Chnl, int Level);
    /* ��ʼ����ʵʱ�� */
    int  (*LPlay) (NET_DEV_MEDIA *Media);                                  /* �˺����ʹ�� NET_DEV_SDK_MEDIA_LPUSH()����ʵʱ�� */
    /* ����ʱ���� */
    int  (*LCtrl) (NET_DEV_MEDIA *Media, unsigned Cmd, void *Data);        /* ���ƣ�Cmd=1 I֡���� */
    /* �ر�ʵʱ�� */
    void (*LClose) (NET_DEV_MEDIA *Media);
    /* ��ȡ��ʷ��ý����� */
    int  (*HProbe) (int Chnl, int Level, unsigned StartTime, unsigned EndTime,
        unsigned RecType, char *Property, void *MediaParamIn);            /* ��ȡý�����,���ܱ���ε��� */
    /* ����ʷ�� */
    int  (*HOpen) (NET_DEV_MEDIA *Media, int Chnl, int Level,
        unsigned StartTime, unsigned EndTime, unsigned RecType, char *Property);
    /* ��ʼ������ʷ�� */
    int  (*HPlay) (NET_DEV_MEDIA *Media);
    /* ��ͣ��ʷ�� */
    int  (*HPause) (NET_DEV_MEDIA *Media);
    /* ��λ����λ�� */
    int  (*HLseek) (NET_DEV_MEDIA *Media, unsigned Timestamp);
    /* �ر���ʷ�� */
    void (*HClose) (NET_DEV_MEDIA *Media);
    /* ��ȡ��ʷ������֡ */
    int  (*HPull) (NET_DEV_MEDIA *Media, void **PFrame);
    /* ������͸��ͨ�� */
    int  (*DCOpen) (NET_DEV_DATACHN *Dc);
    /* ����͸��ͨ������ */
    int  (*DCRecv) (NET_DEV_DATACHN *Dc, void *Ddata, int Size);
    /* �ر�����͸��ͨ�� */
    void (*DCClose) (NET_DEV_DATACHN *Dc);
};

/* ��ʼ��SDK */
int NET_DEV_SDK_INIT(int CmdPort, int DataPort,  NET_DEV_SDK_FUNCS *Ops);

/* ����SDK */
int NET_DEV_SDK_START( void );

/* ����IVS(ƽ̨)���� */
int NET_DEV_SDK_START_IVS_SERVICE(NET_DEV_IVS_CONFIG *Conf);

/* ֹͣIVS(ƽ̨)���� */
void NET_DEV_SDK_STOP_IVS_SERVICE( void );

/* ��ȡSDK�汾�� */
int NET_DEV_SDK_GET_VERSION( void );

/* �ϴ��澯 */
void NET_DEV_SDK_SUBMIT_ALARM(void *Alarm);

/* �����û��Զ������� */
void NET_DEV_SDK_MEDIA_SET_UDATA(NET_DEV_MEDIA *Media, void *Data);

/* ��ȡ�û��Զ������� */
void *NET_DEV_SDK_MEDIA_GET_UDATA(NET_DEV_MEDIA *Media);

/* ����ʵʱ�� */
int NET_DEV_SDK_MEDIA_LPUSH(NET_DEV_MEDIA *Media, void *frm);

/* ������ͨ��SDK */
void NET_DEV_SDK_MEDIA_KILL(NET_DEV_MEDIA *Media);

/* ������������ü��� */
void NET_DEV_SDK_MEDIA_REF(NET_DEV_MEDIA *Media);

/* ������������ü��� */
void NET_DEV_SDK_MEDIA_UNREF(NET_DEV_MEDIA *Media);

/* ����һ֡�Ŀռ䣬������ʷ��pull */
void *NET_DEV_SDK_FRAME_ALLOC(int DataSize, int NalCount);

/* �ͷ�һ֡�Ŀռ� */
void NET_DEV_SDK_FRAME_FREE(void *PFrame);

/* �����û��Զ������� */
void NET_DEV_SDK_DC_SET_UDATA(NET_DEV_DATACHN *Dc, void *Data);

/* ��ȡ�û��Զ������� */
void *NET_DEV_SDK_DC_GET_UDATA(NET_DEV_DATACHN *Dc);

/* ����DC��������ü��� */
void NET_DEV_SDK_DC_REF(NET_DEV_DATACHN *Dc);

/* ����DC��������ü��� */
void NET_DEV_SDK_DC_UNREF(NET_DEV_DATACHN *Dc);

/* ��͸��ͨ���������� */
int NET_DEV_SDK_DC_SEND(NET_DEV_DATACHN *Dc, void *Data, int Size);

/* ͨ������ͨ��SDK */
void NET_DEV_SDK_DC_KILL(NET_DEV_DATACHN *Dc);

#endif /* __NET_DEV_PU_SDK_H__ */