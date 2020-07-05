#ifndef __gsf_data_struct_h__
#define __gsf_data_struct_h__


#define GSF_DATA_MAX_SIZE (60*1024)


typedef enum _GSF_DATA_TRANS_TYPE
{
     GSF_DATA_TRANS_TYPE_UPG = 0x0100   /* 升级ID 偏移地址 低8位必须为0x00 */
    ,GSF_DATA_TRANS_TYPE_TALK= 0x0200   /* 对讲ID 偏移地址 低8位必须为0x00 */
    ,GSF_DATA_TRANS_TYPE_DOWN= 0x0300   /* 下载ID 偏移地址 低8位必须为0x00 */
    ,GSF_DATA_TRANS_TYPE_SNAP= 0x0400   /* 抓拍ID 偏移地址 低8位必须为0x00 */
}GSF_DATA_TRANS_TYPE_E;

#define GSF_DATA_TRANS_TYPE_GET(id)    ((id)&0xff00)

typedef enum _GSF_DATA_TRANS_ID
{    
     GSF_DATA_TRANS_ID_KEEPALIVE   = 0                          /* gsf_data_fmt_t + null */
    ,GSF_DATA_TRANS_ID_UPG_REQ     = GSF_DATA_TRANS_TYPE_UPG+0  /* gsf_data_fmt_t + gsf_data_trans_upg_req */
    ,GSF_DATA_TRANS_ID_UPG_RSP                                  /* gsf_data_fmt_t + gsf_data_trans_upg_rsp */
    ,GSF_DATA_TRANS_ID_UPG_PERCENT                              /* gsf_data_fmt_t + req: null, rsp: gsf_data_upg_percent_rsp */
    ,GSF_DATA_TRANS_ID_UPG_DATA                                 /* gsf_data_fmt_t + file data */
    /*--------------------------*/
    ,GSF_DATA_TRANS_ID_TALK_REQ    = GSF_DATA_TRANS_TYPE_TALK+0 /* gsf_data_fmt_t + gsf_data_trans_talk_req */
    ,GSF_DATA_TRANS_ID_TALK_RSP                                 /* gsf_data_fmt_t + gsf_data_trans_talk_rsp */
    ,GSF_DATA_TRANS_ID_TALK_DATA                                /* gsf_data_fmt_t + gsf_data_talk_frame_t + audio data */
    /*--------------------------*/
    ,GSF_DATA_TRANS_ID_DOWN_REQ    = GSF_DATA_TRANS_TYPE_DOWN+0 /* gsf_data_fmt_t + gsf_data_trans_down_req */
    ,GSF_DATA_TRANS_ID_DOWN_RSP                                 /* gsf_data_fmt_t + gsf_data_trans_down_rsp */
    ,GSF_DATA_TRANS_ID_DOWN_SPEED                               /* gsf_data_fmt_t + req: gsf_data_trans_down_speed, rsp: null */
    ,GSF_DATA_TRANS_ID_DOWN_DATA                                /* gsf_data_fmt_t + gsf_data_packet_t + file data */
    /*--------------------------*/
    ,GSF_DATA_TRANS_ID_SNAP_REQ   = GSF_DATA_TRANS_TYPE_SNAP+0  /* gsf_data_fmt_t + gsf_data_trans_snap_req */
    ,GSF_DATA_TRANS_ID_SNAP_RSP                                 /* gsf_data_fmt_t + gsf_data_trans_snap_rsp */
    ,GSF_DATA_TRANS_ID_SNAP_DATA                                /* gsf_data_fmt_t + gsf_data_packet_t + picture data */
}GSF_DATA_TRANS_ID_E;


#define GSF_DATA_ERR_OK       0       /* OK */
/*------------------------------------------------*/
#define GSF_DATA_ERR_IO      -1       /* IO 错误  */
#define GSF_DATA_ERR_CONN    -2       /* 链接错误 */
#define GSF_DATA_ERR_KEEP    -3       /* 心跳错误 */
/*------------------------------------------------*/
#define GSF_DATA_ERR_DEV        -10   /* 设备错误 */

#define GSF_DATA_ERR_UPG_BUSY   -11   /* 设备正在升级 */
#define GSF_DATA_ERR_UPG_PKG    -12   /* 升级包格式错误 */

#define GSF_DATA_ERR_TALK_BUSY  -21   /* 设备正在对讲 */

#define GSF_DATA_ERR_DWON_FIND  -31   /* 查找不到文件 */

/*----------------------------------*/

typedef enum _GSF_DATA_TRANS_DIR
{
    GSF_DATA_TRANS_DIR_NONE = 0,    /* 数据 */
    GSF_DATA_TRANS_DIR_REQ  = 1,    /* 请求消息 */
    GSF_DATA_TRANS_DIR_RSP  = 2     /* 回应消息 */
}GSF_DATA_TRANS_DIR_E;

/*数据消息头(网络字节序)*/
typedef struct _gsf_data_fmt
{
    short id;    //GSF_DATA_TRANS_ID_E  消息ID（包含数据通道类型）
    char  dir;   //GSF_DATA_TRANS_DIR_E 消息方向(请求, 响应, NONE)
    char  _rt;   //with _rt             是否携带 _rt信息 (_rt必须放置到req结构的第一个成员)
    int   seq;   //seq no;              消息序号(rsp消息直接复制req消息中的seq返回)
    int   size;  //size                 消息大小
    unsigned char  data[0];//data
}gsf_data_fmt_t;


#define GSF_DATA_FMT_HTON(fmt) do {\
    (fmt)->id  = htons((fmt)->id);\
    (fmt)->seq = htonl((fmt)->seq);\
    (fmt)->size= htonl((fmt)->size);\
}while(0)

#define GSF_DATA_FMT_NTOH(fmt) do {\
    (fmt)->id  = ntohs((fmt)->id);\
    (fmt)->seq = ntohl((fmt)->seq);\
    (fmt)->size= ntohl((fmt)->size);\
}while(0)



/*路由信息(打开数据通道的请求消息包含此信息用于转发处理, CU,PU单元不使用)*/

#define GSF_DATA_TRANS_URL_LEN     1024

typedef struct _gsf_data_rt
{
    char url[GSF_DATA_TRANS_URL_LEN]; /* trans://ip:port/s1/s2/3s/s4 */
}gsf_data_rt_t;


// 以下为业务层结构定义（使用网络字节序）
// 通道打开消息 req 中需要包含 _rt 结构;
/*-----------------------------------*/

#define GSF_DATA_UPG_FILE_HDR_LEN   256


typedef struct _gsf_data_trans_upg_req
{
    gsf_data_rt_t _rt;
    char upg_file_hdr[GSF_DATA_UPG_FILE_HDR_LEN];
    int  upg_file_size;
}gsf_data_trans_upg_req;

#define GSF_DATA_TRANS_UPG_REQ_HTON(req) do { \
    (req)->upg_file_size = htonl((req)->upg_file_size);\
}while(0)

#define GSF_DATA_TRANS_UPG_REQ_NTOH(req) do { \
    (req)->upg_file_size = ntohl((req)->upg_file_size);\
}while(0)


typedef struct _gsf_data_trans_upg_rsp
{
    int  err;
}gsf_data_trans_upg_rsp;

#define GSF_DATA_TRANS_UPG_RSP_HTON(rsp) do { \
    (rsp)->err = htonl((rsp)->err);\
}while(0)

#define GSF_DATA_TRANS_UPG_RSP_NTOH(rsp) do { \
    (rsp)->err = ntohl((rsp)->err);\
}while(0)


typedef struct _gsf_data_upg_percent_rsp
{
    int  percent;    /* <0: 错误, 1 - 100: 进度值 */
}gsf_data_upg_percent_rsp;

#define GSF_DATA_TRANS_UPG_PERCENT_RSP_HTON(rsp) do { \
    (rsp)->percent = htonl((rsp)->percent);\
}while(0)

#define GSF_DATA_TRANS_UPG_PERCENT_RSP_NTOH(rsp) do { \
    (rsp)->percent = ntohl((rsp)->percent);\
}while(0)

/*-----------------------------------*/

typedef struct _gsf_data_talk_frame
{
    unsigned int  no;    /* 帧序号 */
    unsigned int  ts;    /* 帧时间戳 ms */
    unsigned char type;  /* 帧编码类型 NET_DEV_MEDIA_ENCODETYPE */
    unsigned char res[3];/* 保留 */
    unsigned int  size;  /* 帧大小 */
    unsigned char data[];
}gsf_data_talk_frame_t;

#define GSF_DATA_TALK_FRAME_HTON(frm) do { \
    (frm)->no = htonl((frm)->no);\
    (frm)->ts = htonl((frm)->ts);\
    (frm)->size = htonl((frm)->size);\
}while(0)

#define GSF_DATA_TALK_FRAME_NTOH(frm) do { \
    (frm)->no = ntohl((frm)->no);\
    (frm)->ts = ntohl((frm)->ts);\
    (frm)->size = ntohl((frm)->size);\
}while(0)


typedef enum _GSF_DATA_TRANS_TALK_TYPE
{
    GSF_DATA_TRANS_TALK_TYPE_DUAL = 0,  /* 双向语音     */
    GSF_DATA_TRANS_TALK_TYPE_BC   = 1   /* CU单向广播PU */
}GSF_DATA_TRANS_TALK_TYPE_E;


typedef struct _gsf_data_trans_talk_req
{
    gsf_data_rt_t _rt;
    int  ch;        // 多路设备时： -1：设备， 0 - 255： 设备通道
    int  type;      // GSF_DATA_TRANS_TALK_TYPE_E
}gsf_data_trans_talk_req;

#define GSF_DATA_TRANS_TALK_REQ_HTON(req) do { \
    (req)->ch = htonl((req)->ch);\
    (req)->type = htonl((req)->type);\
}while(0)

#define GSF_DATA_TRANS_TALK_REQ_NTOH(req) do { \
    (req)->ch = ntohl((req)->ch);\
    (req)->type = ntohl((req)->type);\
}while(0)

typedef struct _gsf_data_trans_talk_rsp
{
    int  err;
}gsf_data_trans_talk_rsp;

#define GSF_DATA_TRANS_TALK_RSP_HTON(rsp) do { \
    (rsp)->err = htonl((rsp)->err);\
}while(0)

#define GSF_DATA_TRANS_TALK_RSP_NTOH(rsp) do { \
    (rsp)->err = ntohl((rsp)->err);\
}while(0)

/*-----------------------------------*/

typedef enum _GSF_DATA_PACKET_TYPE
{
    GSF_DATA_PACKET_TYPE_NVR = 1,   /* NVR存储数据包 */
    GSF_DATA_PACKET_TYPE_CMS = 2,   /* CMS存储数据包 */
    GSF_DATA_PACKET_TYPE_JPEG= 3,   /* 标准JPEG图片  gsf_data_trans_pic */ 
	GSF_DATA_PACKET_TYPE_IPC = 4	/* IPC存储数据包 */
}GSF_DATA_PACKET_TYPE_E;

typedef enum _GSF_DATA_PACKET_EOF
{
    GSF_DATA_PACKET_EOF_NONE = 0,   /* 不是结束包 */
    GSF_DATA_PACKET_EOF_FILE = 1,   /* 单个文件结束标记 */   
    GSF_DATA_PACKET_EOF_ALL  = 2    /* 所有文件结束标记 */
}GSF_DATA_PACKET_EOF_E;


typedef struct _gsf_data_packet
{
    unsigned int  no;     /* 包序号   */
    unsigned char type;   /* 包类型 GSF_DATA_PACKET_TYPE_E */
    unsigned char eof;    /* 结束包标记 GSF_DATA_PACKET_EOF_E */
    unsigned char res[2]; /* 保留     */
    unsigned int  size;   /* 包大小   */
    unsigned char data[];
}gsf_data_packet_t;

#define GSF_DATA_PACKET_HTON(pkt) do { \
    (pkt)->no = htonl((pkt)->no);\
    (pkt)->size = htonl((pkt)->size);\
}while(0)

#define GSF_DATA_PACKET_NTOH(pkt) do { \
    (pkt)->no = ntohl((pkt)->no);\
    (pkt)->size = ntohl((pkt)->size);\
}while(0)


typedef enum _GSF_DATA_TRANS_DOWN_TYPE
{
    GSF_DATA_TRANS_DOWN_REC1 = 1,   /* property */
    GSF_DATA_TRANS_DOWN_REC2 = 2,   /* begin_time , end_time */
    GSF_DATA_TRANS_DOWN_PIC1 = 3,   /* property */
    GSF_DATA_TRANS_DOWN_PIC2 = 4,   /* begin_time , end_time */
}GSF_DATA_TRANS_DOWN_TYPE;


typedef struct _gsf_data_trans_down_req
{
    gsf_data_rt_t _rt;
    int down_type;               /* GSF_DATA_TRANS_DOWN_TYPE */
    char property[64];           /* 文件句柄（下载单个文件时使用） */
    //以下保留暂不实现;
    //---------------------------------------
    int ch;                      /* 通道号 */
    int st_ch;                   /* 码流类型 */
    int rec_type;                /* 文件类型 */
    int begin_time;              /* 开始时间 */
    int end_time;                /* 结束时间 */
}gsf_data_trans_down_req;

#define GSF_DATA_TRANS_DOWN_REQ_HTON(req) do { \
    (req)->down_type = htonl((req)->down_type);\
    (req)->ch   = htonl((req)->ch);\
    (req)->st_ch= htonl((req)->st_ch);\
    (req)->rec_type = htonl((req)->rec_type);\
    (req)->begin_time = htonl((req)->begin_time);\
    (req)->end_time = htonl((req)->end_time);\
}while(0)

#define GSF_DATA_TRANS_DOWN_REQ_NTOH(req) do { \
    (req)->down_type = ntohl((req)->down_type);\
    (req)->ch   = ntohl((req)->ch);\
    (req)->st_ch= ntohl((req)->st_ch);\
    (req)->rec_type = ntohl((req)->rec_type);\
    (req)->begin_time = ntohl((req)->begin_time);\
    (req)->end_time = ntohl((req)->end_time);\
}while(0)

typedef struct _gsf_data_trans_down_rsp
{
    int err;
    //以下保留暂不实现;
    //---------------------------------------
    int item_count;     /* 文件个数       */
    int total_size;     /* 估算总大小单位k */
    int begin_time;     /* 实际开始时间    */
    int end_time;       /* 实际结束时间    */
}gsf_data_trans_down_rsp;

#define GSF_DATA_TRANS_DOWN_RSP_HTON(rsp) do { \
    (rsp)->err = htonl((rsp)->err);\
    (rsp)->item_count = htonl((rsp)->item_count);\
    (rsp)->total_size = htonl((rsp)->total_size);\
    (rsp)->begin_time = htonl((rsp)->begin_time);\
    (rsp)->end_time = htonl((rsp)->end_time);\
}while(0)

#define GSF_DATA_TRANS_DOWN_RSP_NTOH(rsp) do { \
    (rsp)->err = ntohl((rsp)->err);\
    (rsp)->item_count = ntohl((rsp)->item_count);\
    (rsp)->total_size = ntohl((rsp)->total_size);\
    (rsp)->begin_time = ntohl((rsp)->begin_time);\
    (rsp)->end_time = ntohl((rsp)->end_time);\
}while(0)


typedef struct _gsf_data_trans_down_speed
{
    int speed;  /* 1, 2, 4, 8 */
}gsf_data_trans_down_speed;

#define GSF_DATA_TRANS_DOWN_SPEED_HTON(req) do { \
    (req)->speed = htonl((req)->speed);\
}while(0)

#define GSF_DATA_TRANS_DOWN_SPEED_NTOH(req) do { \
    (req)->speed = ntohl((req)->speed);\
}while(0)

/*-------------------------------------*/

typedef struct _gsf_data_trans_snap_req
{
    gsf_data_rt_t _rt;
    int  ch;        /* 通道号 */
    int  st_ch;     /* 子码流号 */
    int  quality;   /* 质量 */
}gsf_data_trans_snap_req;

#define GSF_DATA_TRANS_SNAP_REQ_HTON(req) do { \
    (req)->ch      = htonl((req)->ch);\
    (req)->st_ch   = htonl((req)->st_ch);\
    (req)->quality = htonl((req)->quality);\
}while(0)

#define GSF_DATA_TRANS_SNAP_REQ_NTOH(req) do { \
    (req)->ch      = ntohl((req)->ch);\
    (req)->st_ch   = ntohl((req)->st_ch);\
    (req)->quality = ntohl((req)->quality);\
}while(0)


typedef struct _gsf_data_trans_snap_rsp
{
    int  err;           /* 错误码 */
}gsf_data_trans_snap_rsp;

#define GSF_DATA_TRANS_SNAP_RSP_HTON(rsp) do { \
    (rsp)->err = htonl((rsp)->err);\
}while(0)

#define GSF_DATA_TRANS_SNAP_RSP_NTOH(rsp) do { \
    (rsp)->err = ntohl((rsp)->err);\
}while(0)

typedef struct _gsf_data_trans_pic
{
    int magic;      /* 0x55aa */
    int size;       /* size */
    char data[0];
}gsf_data_trans_pic;

#define GSF_DATA_TRANS_SNAP_PIC_HTON(pic) do { \
    (pic)->magic  = htonl((pic)->magic);\
    (pic)->size   = htonl((pic)->size);\
}while(0)

#define GSF_DATA_TRANS_SNAP_PIC_NTOH(pic) do { \
    (pic)->magic  = ntohl((pic)->magic);\
    (pic)->size   = ntohl((pic)->size);\
}while(0)


#endif //__gsf_data_struct_h__