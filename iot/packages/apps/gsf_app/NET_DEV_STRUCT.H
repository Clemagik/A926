#ifndef __NET_DEV_STRUCT_H__
#define __NET_DEV_STRUCT_H__

//数据类型
//---------------------------------------------------------
#ifndef DATA_TYPE_DEFINE_SINGLE
#define DATA_TYPE_DEFINE_SINGLE


#if (defined(_WIN32)) // windows
#include <Winsock2.h>
#include <windows.h>
typedef  unsigned __int64 UINT64;

#ifdef NET_DEV_EXPORTS
#define NET_DEV_API extern "C"__declspec(dllexport)
#else
#define NET_DEV_API extern "C"__declspec(dllimport)
#endif
#define NET_DEV_CALL __stdcall

#elif defined(__linux__) || defined(__APPLE__) // linux
typedef  unsigned int       DWORD;
typedef  unsigned short     WORD;
typedef  unsigned short     USHORT;
typedef  unsigned char      BYTE;
typedef  char               CHAR;
typedef  int                BOOL;
typedef  unsigned int       UINT;
typedef  void*              LPVOID;
typedef  void*              HANDLE;
typedef  unsigned int*      LPDWORD;
typedef  unsigned long long UINT64;

#ifndef     TRUE
#define     TRUE  1
#endif
#ifndef     FALSE
#define     FALSE 0
#endif
#ifndef     NULL
#define     NULL 0
#endif

#define NET_DEV_API extern "C"
#define NET_DEV_CALL

typedef unsigned int   COLORKEY;
typedef unsigned int   COLORREF;

#ifndef __HWND_defined
#define __HWND_defined
  #if defined(__linux__)
    typedef unsigned int HWND;
  #else
    typedef void* HWND;
  #endif
#endif

#ifndef __HDC_defined
#define __HDC_defined
    #if defined(__linux__)
    typedef struct __DC
    {
            void*   surface;// SDL Surface
            HWND    hWnd;   // HDC window handle
    }DC;
    typedef DC* HDC;
    #else
    typedef void* HDC;
    #endif
#endif

typedef struct __InitInfo
{
  int uWidth;
  int uHeight;
}INITINFO;
#endif

#endif        //DATA_TYPE_DEFINE_SINGLE

//全局宏定义
//-------------------------------------------------------------------
#define NET_DEV_SADP_ID_LEN       64  //搜索协议设备ID长度
#define NET_DEV_DEVICE_NAME_LEN   32  //设备名字长度
#define NET_DEV_VERSION_LEN       32  //版本号字符串长度
#define NET_DEV_DATE_LEN          32  //日期字符串长度
#define NET_DEV_SERIAL_NO_LEN     32  //序列号字符串长度
#define NET_DEV_AUTHORIZECODE_LEN 32  //设备授权码(连接平台)
#define NET_DEV_HOST_LEN          128 //最大域名长度
#define NET_DEV_IPV4_ADDR_LEN     16  //IPV4地址长度
#define NET_DEV_IPV6_ADDR_LEN     48  //IPV6地址长度
#define NET_DEV_MAC_ADDR_LEN      20  //MAC地址长度
#define NET_DEV_MAX_ETH_NICS      2   //最大以太网口数
#define NET_DEV_ETH_NAME_LEN      32  //以太网名字长度
#define NET_DEV_MAX_URL_LEN       256 //最大URL长度
#define NET_DEV_MAX_PATH_LEN      260 //最大文件路径

#define NET_DEV_MAX_USER_NUM      16  //最大用户个数
#define NET_DEV_USERNAME_LEN      32  //用户名长度
#define NET_DEV_PASSWORD_LEN      16  //密码长度

#define NET_DEV_MAX_CHANN_NUM     36  //最大通道数
#define NET_DEV_CHANN_NAME_LEN    32  //设备通道名字长度
#define NET_DEV_MAX_STREAM_NUM    3   //最大码流个数

#define NET_DEV_PTZ_NAME_LEN      24  //PTZ名字长度
#define NET_DEV_MAX_PRESET_NUM    256 //最大PTZ预置点个数
#define NET_DEV_MAX_CRZ_STEP_NUM  16  //每条PTZ巡航路径最大步数
#define NET_DEV_MAX_CRZ_NUM       8   //最大巡航路径个数
#define NET_DEV_PTZ_MAX_PTN_NUM   4   //最大PTZ轨迹录制个数
#define NET_DEV_PTZ_SCHTASKS_NUM  8   //PTZ最大定时任务个数
#define NET_DEV_PTZ_MAX_SPEED     100 //PTZ最大速度

#define NET_DEV_MAX_PHY_DISK      16  //最大物理盘个数
#define NET_DEV_MAX_NET_DISK      16  //最大网络硬盘个数
#define NET_DEV_MAX_ALARM_IN_NUM  16  //最大报警输入个数
#define NET_DEV_MAX_ALARM_OUT_NUM 16  //最大报警输出个数
#define NET_DEV_ALARM_NAME_LEN    32  //告警设备名字长度
#define MAX_POSITIONVALUE_LEN     16  //位置维度长度

#define NET_DEV_MAX_LOG_PAGE_SIZE 512 //日志页大小
#define NET_DEV_MAX_REC_PAGE_SIZE 512 //录像页大小
#define NET_DEV_MAX_PIC_PAGE_SIZE 512 //图片页大小
#define NET_DEV_REC_PROPERTY_SIZE 72  //录像记录私有数据大小

#define NET_DEV_OSD_TEXT_NUM      5   //最大OSD文本个数
#define NET_DEV_OSD_TEXT_LEN      64  //最大OSD文本长度

#define NET_DEV_EMAIL_ADDR_LEN    64  //Email地址长度
#define NET_DEV_WIFI_ESSID_LEN    64  //Wifi EssID长度
#define NET_DEV_WIFI_KEY_LEN      64  //Wifi Key长度
#define NET_DEV_MAX_WIFI_AP_NUM   32  //最大Wifi Ap个数

#define NET_DEV_MAX_SEG_NUM       4   //最大时间段个数
#define NET_DEV_MAX_AREA_NUM      4   //最大区域个数

#define NET_DEV_ALARM_DATA_SIZE   64  //告警数据长度
#define NET_DEV_UPGRADE_HDR_SIZE  256 //升级包头长度

#define NET_DEV_ID_LEN            32  //设备ID长度
#define NET_DEV_PHONENUMBER_LEN   32  //电话号码长度

#define NET_DEV_MAX_DEVLIST_NUM   256 //最大设备列表个数
#define NET_DEV_P2P_DEV_UUID_LEN  32  //P2P设备UUID长度
#define NET_DEV_ALARM_DESC_LEN    32  //报警消息描述长度

//全局错误码定义
//-------------------------------------------------------------------
#define NET_DEV_ERROR_SUCCESS              0   //没有错误
#define NET_DEV_ERROR_TIMEOUT              1   //操作超时
#define NET_DEV_ERROR_FAILED               2   //操作失败

#define NET_DEV_ERROR_VERSIONNOMATCH       3   //版本不匹配
#define NET_DEV_ERROR_NOINIT               4   //没有初始化
#define NET_DEV_ERROR_OVER_MAXLINK         5   //连接个数超过最大
#define NET_DEV_ERROR_NOSUPPORT            6   //设备不支持
#define NET_DEV_ERROR_BUSY                 7   //设备忙
#define NET_DEV_ERROR_NORESOURCE           8   //资源不足

#define NET_DEV_ERROR_PASSWORD             9   //用户名密码错误
#define NET_DEV_ERROR_MAX_USERNUM          10  //设备端用户数达到最大
#define NET_DEV_ERROR_NOENOUGHPRI          11  //权限不足

#define NET_DEV_ERROR_VOICEOPENED          12  //服务器语音对讲被占用
#define NET_DEV_ERROR_SERIALPORT           13  //串口号错误
#define NET_DEV_ERROR_ALARMPORT            14  //报警端口错误
#define NET_DEV_ERROR_CHAN_NO              15  //通道号错误

#define NET_DEV_ERROR_CHAN_EXCEPTION       16  //通道处于错误状态
#define NET_DEV_ERROR_CHAN_NOTSUPPORT      17  //通道不支持该操作

#define NET_DEV_ERROR_DISK_NO              18  //硬盘号错误
#define NET_DEV_ERROR_DISK_SLEEP           19  //硬盘正在休眠,稍后继续
#define NET_DEV_ERROR_DISK_ERROR           20  //硬盘出错
#define NET_DEV_ERROR_DISK_FORMATING       21  //硬盘正在格式化


//===================================================================

//全局枚举定义
//-------------------------------------------------------------------
typedef enum _NET_DEV_TYPE
{
    NET_DEV_TYPE_DVR      = 0x00,
    NET_DEV_TYPE_NVR      = 0x01,
    NET_DEV_TYPE_IPC      = 0x02,
    NET_DEV_TYPE_DEC      = 0x03,
    NET_DEV_TYPE_IPDOME   = 0x04,
    NET_DEV_TYPE_INTEG    = 0x05,
    NET_DEV_TYPE_NETINTEG = 0x06,
    NET_DEV_TYPE_ENC      = 0x07,
    NET_DEV_TYPE_BUTT
}NET_DEV_TYPE;


typedef enum _NET_DEV_PROTOL_TYPE
{
    NET_DEV_PROTOL_ME     = 0x00,
    NET_DEV_PROTOL_ONVIF  = 0x01,
    NET_DEV_PROTOL_GB28181= 0x02,
    NET_DEV_PROTOL_ZSYH   = 0x03,  //中盛益华 平台
    NET_DEV_PROTOL_HXHT   = 0x04,  //互信互通 平台
    NET_DEV_PROTOL_HW     = 0x05,  //华为 平台
    NET_DEV_PROTOL_IST_NVR= 0x06,  //IST NVR
    NET_DEV_PROTOL_IST_PF = 0x07,  //IST 平台
    NET_DEV_PROTOL_HH     = 0x08,
    NET_DEV_PROTOL_GOOLINK= 0x09,  //浪涛 GOOLINK
    NET_DEV_PROTOL_BUTT
}NET_DEV_PROTOL_TYPE;


typedef enum _NET_DEV_TRANSPROTOL
{
    NET_DEV_TRANS_TCP=0,
    NET_DEV_TRANS_UDP
}NET_DEV_TRANSPROTOL;


typedef struct _NET_DEV_IPADDR NET_DEV_IPADDR;
struct _NET_DEV_IPADDR
{
    CHAR IpV4[NET_DEV_IPV4_ADDR_LEN];
    CHAR IpV6[NET_DEV_IPV6_ADDR_LEN];
};


typedef struct _NET_DEV_DOMAINADDR NET_DEV_DOMAINADDR;
struct _NET_DEV_DOMAINADDR
{
    CHAR Addr[NET_DEV_MAX_URL_LEN];
};


typedef struct _NET_DEV_NICCFG NET_DEV_NICCFG;
struct _NET_DEV_NICCFG
{
    NET_DEV_IPADDR IPAddr;		//设备IP
    NET_DEV_IPADDR Netmask;		//子网掩码
    NET_DEV_IPADDR Gateway;		//网关
    CHAR MACAddr[NET_DEV_MAC_ADDR_LEN];		//MAC地址
    BYTE EnableDHCP;		//是否启用DHCP
    BYTE EnableAutoDNS;		//是否允许自动获取DNS
    BYTE Mtu;		//MTU大小默认 1500
    BYTE Interface;		//网络接口 1-10MBase-T 2-10MBase-T全双工 3-100MBase-TX 4-100M全双工 5-10M/100M/1000M自适应
    NET_DEV_IPADDR DNS1;		//主DNS服务器地址
    NET_DEV_IPADDR DNS2;		//次DNS服务器地址
};


typedef struct _NET_DEV_ETHCFG NET_DEV_ETHCFG;
struct _NET_DEV_ETHCFG
{
    BYTE NICCount;		//设备支持的以太网卡个数(只读)
    BYTE Res[3];		//保留
    NET_DEV_NICCFG EthNICs[NET_DEV_MAX_ETH_NICS];
};


typedef struct _NET_DEV_WIFI_WEP NET_DEV_WIFI_WEP;
struct _NET_DEV_WIFI_WEP 	//WEP加密模式
{
    DWORD Authentication;		//认证方式 'NET_DEV_WIFI_WEP_AUTH'
    DWORD Active;
    DWORD KeyType;		//0-16进制；1-ASCII
    BYTE KeyInfo[NET_DEV_WIFI_KEY_LEN];
};


typedef struct _NET_DEV_WIFI_WPAPSK NET_DEV_WIFI_WPAPSK;
struct _NET_DEV_WIFI_WPAPSK 	//* WPA+PSK加密模式 */
{
    DWORD EncryptType;		// 加密类型NET_DEV_WIFI_WPAPSK_ET
    BYTE KeyInfo[NET_DEV_WIFI_KEY_LEN];
};


typedef struct _NET_DEV_WIFI_WPAEAP NET_DEV_WIFI_WPAEAP;
struct _NET_DEV_WIFI_WPAEAP 	//* WPA+EAP认证模式 */
{
    NET_DEV_IPADDR ServerIp;
    DWORD Port;
    BYTE KeyInfo[NET_DEV_WIFI_KEY_LEN];
};


typedef struct _NET_DEV_WIFINIC NET_DEV_WIFINIC;
struct _NET_DEV_WIFINIC
{
    DWORD EnableWifi;		//WIFI使能: 0-禁止, 1-允许
    NET_DEV_NICCFG NicCfg;		//WIFI地址结构
};


typedef enum _NET_DEV_WIFISECURITY
{
    NET_DEV_WIFIS_NONE = 0,
    NET_DEV_WIFIS_WEP,
    NET_DEV_WIFIS_WPA_PSK,
    NET_DEV_WIFIS_WPA_EAP,
    NET_DEV_WIFIS_WPA2_PSK,
    NET_DEV_WIFIS_WPA2_EAP,
}NET_DEV_WIFISECURITY;


typedef struct _NET_DEV_WIFICFG NET_DEV_WIFICFG;
struct _NET_DEV_WIFICFG
{
    /* ----------- IP配置 ----------- */
    NET_DEV_WIFINIC WifiNIC;
    /* ----------- 链接配?? ----------- */
    CHAR EssID[NET_DEV_WIFI_ESSID_LEN];		//Wifi服务号
    DWORD Security;		//加密模式 'NET_DEV_WIFISECURITY'
    union {
        NET_DEV_WIFI_WEP Wep;
        NET_DEV_WIFI_WPAPSK WpaPsk;
        NET_DEV_WIFI_WPAEAP WpaEap;
    } Key;
    BYTE WifiLinkMode;		// 0-normal(manual)   1-airlink
    BYTE Res[3];		// 保留
};


typedef struct _NET_DEV_SERVICECFG NET_DEV_SERVICECFG;
struct _NET_DEV_SERVICECFG
{
    WORD MsgPort;		//设备信令端口
    WORD RtspPort;		//设备RTSP端口
    WORD HttpPort;		//设备HTTP端口
    WORD RtmpPort;		//设备RTMP端口
};


//===================================================================

//搜索设备信息
//-------------------------------------------------------------------
typedef struct _NET_DEV_SADPINFO NET_DEV_SADPINFO;
struct _NET_DEV_SADPINFO
{
    CHAR SadpID[NET_DEV_SADP_ID_LEN];		// PU端不用填，CU端自动填充
    CHAR DevName[NET_DEV_DEVICE_NAME_LEN];
    CHAR SoftwareVer[NET_DEV_VERSION_LEN];
    CHAR HardwareVer[NET_DEV_VERSION_LEN];
    CHAR SerialNo[NET_DEV_SERIAL_NO_LEN];		// 序列号
    WORD ProtolType;		// NET_DEV_PROTOL_TYPE
    BYTE DevType;		// NET_DEV_TYPE
    BYTE EncChnCount;		// 编码通道个数
    CHAR ProductName[NET_DEV_DEVICE_NAME_LEN];		//产品型号
    NET_DEV_ETHCFG EthCfg;
    NET_DEV_WIFINIC WifiNIC;
    NET_DEV_SERVICECFG ServiceCfg;
};


//搜索设备平台信息
typedef struct _NET_DEV_SADPCMSINFO NET_DEV_SADPCMSINFO;
struct _NET_DEV_SADPCMSINFO
{
    CHAR DevName[NET_DEV_DEVICE_NAME_LEN];
    BYTE DevType;		// NET_DEV_TYPE
    BYTE EncChnCount;		// 编码通道个数
    WORD DevPort;		// 设备端口
    NET_DEV_IPADDR DevIP;		// 设备IP
    NET_DEV_IPADDR CMSIP;		// CMS IP
    WORD CmsPort;		// CMS端口
    BYTE Enable;		// 上平台开关
    BYTE Protocol;		// 传输协议类型,NET_DEV_TRANSPROTOL
    CHAR DevID[NET_DEV_ID_LEN];		// 设备平台ID
    CHAR Passwd[NET_DEV_PASSWORD_LEN];		// 注册授权码
    CHAR SerialNo[NET_DEV_SERIAL_NO_LEN];		// 序列号
    CHAR SoftwareVer[NET_DEV_VERSION_LEN];		// 软件版本
};


//搜索修改网络配置
//-------------------------------------------------------------------
typedef struct _NET_DEV_MODIFY_NET NET_DEV_MODIFY_NET;
struct _NET_DEV_MODIFY_NET
{
    CHAR SadpID[NET_DEV_SADP_ID_LEN];
    CHAR UserName[NET_DEV_USERNAME_LEN];
    CHAR Passwd[NET_DEV_PASSWORD_LEN];
    WORD ProtolType;		// NET_DEV_PROTOL_TYPE
    NET_DEV_ETHCFG EthCfg;
    NET_DEV_WIFINIC WifiNIC;
    NET_DEV_SERVICECFG ServiceCfg;
};


//===================================================================
//设备能力集信息
//-------------------------------------------------------------------

typedef enum _NET_DEV_SYS_CAP
{
    DEV_SYS_CAP_PTZ      = 0,   // 云台
    DEV_SYS_CAP_WIFI     = 1,   // WIFI
    DEV_SYS_CAP_3G       = 2,   // 3G
    DEV_SYS_CAP_IVS      = 3,   // 智能分析
    DEV_SYS_CAP_IR       = 4,   // 红外
    DEV_SYS_CAP_PIR      = 5,   // 人体红外感应
    DEV_SYS_CAP_TZ       = 6,   // 时区
}NET_DEV_SYS_CAP;


typedef enum _NET_DEV_IVS_CAP
{
    NET_DEV_IVS_BRIGHT_ABMNL_CAP        = 1,  // 亮度异常
    NET_DEV_IVS_CLARITY_ABMNL_CAP       = 2,  // 清晰度异常
    NET_DEV_IVS_NOISE_ABMNL_CAP         = 3,  // 噪声异常
    NET_DEV_IVS_COLOR_ABMNL_CAP         = 4,  // 颜色异常
    NET_DEV_IVS_SCENE_CHANGE_CAP        = 5,  // 场景变换,人为干扰
    NET_DEV_IVS_TRIP_WIRE_CAP           = 6,  // 单绊线
    NET_DEV_IVS_DOUBLE_TRIP_WIRE_CAP    = 7,  // 双绊线
    NET_DEV_IVS_PERI_METER_CAP          = 8,  // 警戒区
    NET_DEV_IVS_LOITER_CAP              = 9,  // 徘徊检测
    NET_DEV_IVS_LEFT_CAP                = 10, // 物品遗留
    NET_DEV_IVS_TAKE_CAP                = 11, // 物品丢失
}NET_DEV_IVS_CAP;


typedef enum _NET_DEV_IMA_CAP
{
    NET_DEV_IMA_SCENCE_CAP      = 0,
    NET_DEV_IMA_BRIGHT_CAP      = 1,
    NET_DEV_IMA_CONTRAST_CAP    = 2,
    NET_DEV_IMA_HUE_CAP         = 3,
    NET_DEV_IMA_SATURATION_CAP  = 4,
    NET_DEV_IMA_SHARPNESS_CAP   = 5,
    NET_DEV_IMA_MIRROR_CAP      = 6,
    NET_DEV_IMA_FLIP_CAP        = 7,
    NET_DEV_IMA_ROTATE_CAP      = 8,
    NET_DEV_IMA_ANTIFLICKER_CAP = 9,
    NET_DEV_IMA_WDR_CAP         = 10,
    NET_DEV_IMA_SHUTTER_CAP     = 11,
    NET_DEV_IMA_LDC_CAP         = 12,
    NET_DEV_IMA_ANTIFOG_CAP     = 13,
    NET_DEV_IMA_AWB_CAP         = 14,
    NET_DEV_IMA_BLC_CAP         = 15,
    NET_DEV_IMA_FREZON_CAP      = 16,
    NET_DEV_IMA_DN_CAP          = 17,
    NET_DEV_IMA_AE_MINTIME_CAP  = 18,
    NET_DEV_IMA_AE_MAXTIME_CAP  = 19,
    NET_DEV_IMA_AGAIN_CAP       = 20
}NET_DEV_IMA_CAP;


typedef struct _NET_DEV_CH_CAPABILITIES NET_DEV_CH_CAPABILITIES;
struct _NET_DEV_CH_CAPABILITIES
{
    BYTE VideoEncType;		// 支持的视频编码集 NET_DEV_VENC_TYPE
    BYTE AudioEncType;		// 支持的音频编码集 NET_DEV_AENC_TYPE
    BYTE OverlayCount;		// 视频覆盖区域个数
    BYTE HideAlarmCount;		// 遮挡报警区域个数
    BYTE MotionAlarmCount;		// 移动侦测报警个数
    BYTE OsdCount;		// OSD个数
    BYTE LostAlarm;		// 是否支持视频丢失报警
    BYTE StreamCount;		// 码流个数
    DWORD Resolution[NET_DEV_MAX_STREAM_NUM];		//支持的分辨率集 掩码 NET_DEV_IMAGERESOLUTION
    DWORD FPS[NET_DEV_MAX_STREAM_NUM];		//支持的最大帧率集 数值表示支持的最大帧率值
    DWORD Ima;		//图像参数能力集NET_DEV_IMA_CAP
};


typedef struct _NET_DEV_CAPABILITIES NET_DEV_CAPABILITIES;
struct _NET_DEV_CAPABILITIES
{
    /*------- system -----*/
    CHAR ProductName[NET_DEV_DEVICE_NAME_LEN];		//产品型号
    BYTE DevType;		// 设备类型 NET_DEV_TYPE
    BYTE EncChnCount;		// 编码通道个数
    BYTE DecChnCount;		// 解码通道个数
    BYTE AudioInCount;		// 语音输入个数
    BYTE AudioOutCount;		// 语音输出个数
    BYTE AlarmInCount;		// 报警输入个数
    BYTE AlarmOutCount;		// 报警输出个数
    BYTE Rs232Count;		// RS232串口个数
    BYTE Rs485Count;		// RS485串口个数
    BYTE EthCount;		// 网络口个数
    BYTE DiskCount;		// 硬盘口个数
    BYTE VoutCount;		// 视频输出口的个数
    BYTE UsbCount;		// USB口的个数
    BYTE CruisesPerChn;		// 每个通道最大支持的巡航个数（<= NET_DEV_MAX_CRZ_NUM ）
    BYTE PatternPerChn;		// 每个通道最大支持的花样扫描条数 (<= NET_DEV_PTZ_MAX_PTN_NUM)
    BYTE ScreenCount;		// 输出屏幕个数
    BYTE MixEncChn;		// 混合编码通道使能
    DWORD SystemCap;		// 系统能力 NET_DEV_SYS_CAP
    DWORD ProtolCap;		// 协议能力 NET_DEV_PROTOL_TYPE
    DWORD InProtolCap;		// NVR接入的IPC协议能力 NET_DEV_PROTOL_TYPE
    DWORD IvsCap;		// 智能分析能力 NET_DEV_IVS_CAP
    DWORD Res1[2];		// 保留
    /*--------------------*/
    NET_DEV_CH_CAPABILITIES ChannelCap;		//通道能力
};


//===================================================================

typedef enum _NET_DEV_REC_POLICY
{
    NET_DEV_REC_PLOLICY_STOP = 0,
    NET_DEV_REC_PLOLICY_AUTO = 1
}NET_DEV_REC_POLICY;


typedef enum _NET_DEV_LANGUAGE
{
    NET_DEV_LANGUAGE_CHINESE = 0,
    NET_DEV_LANGUAGE_ENGLISH = 1
}NET_DEV_LANGUAGE;


typedef enum _NET_DEV_VIDEOMODE
{
    NET_DEV_VIDEOMODE_PAL = 0,
    NET_DEV_VIDEOMODE_NTSC= 1
}NET_DEV_VIDEOMODE;


typedef enum _NET_DEV_VIDEOOUT
{
    NET_DEV_VIDEOOUT_CVBS = 0,
    NET_DEV_VIDEOOUT_VGA  = 1,
    NET_DEV_VIDEOOUT_HDMI = 2
}NET_DEV_VIDEOOUT;


typedef enum _NET_DEV_VOUT_RESOLUTION
{
    NET_DEV_VOUT_1024x768_60 = 0,  // 1024x768    60HZ
    NET_DEV_VOUT_1280x1024_60, // 1280x1024   60HZ
    NET_DEV_VOUT_1440x900_60,  // 1440x900    60HZ
    NET_DEV_VOUT_720P_50,      // 1280x720    50HZ
    NET_DEV_VOUT_720P_60,      // 1280x720    60HZ
    NET_DEV_VOUT_1080P_50,     // 1920x1080   50HZ
    NET_DEV_VOUT_1080P_60,     // 1920x1080   60HZ
    NET_DEV_VOUT_BUTT
}NET_DEV_VOUT_RESOLUTION;


typedef enum _NET_DEV_BOOTGUIDE
{
    NET_DEV_BOOTGUIDE_NO  = 0,
    NET_DEV_BOOTGUIDE_YES = 1
}NET_DEV_BOOTGUIDE;


//设备基本信息
//-------------------------------------------------------------------
typedef struct _NET_DEV_NVRCFG NET_DEV_NVRCFG;
struct _NET_DEV_NVRCFG
{
    DWORD DevIRId;		// 设备IRId, 用于遥控器
    BYTE StandbyTime;		// 菜单待机时间 1~60分钟  0: 表示不待机
    BYTE BootGuide;		// 开机向导 NET_DEV_BOOTGUIDE
    BYTE MainVideoOut;		// 输出设备  NET_DEV_VIDEOOUT
    BYTE MainResolution;		// (主屏)分辨率 NET_DEV_VOUT_RESOLUTION
    BYTE AuxEnable;		// (辅屏)是否开启
    BYTE AuxResolution;		// (辅屏)分辨率 NET_DEV_VOUT_RESOLUTION
    BYTE Res[2];
};


typedef struct _NET_DEV_BASECFG NET_DEV_BASECFG;
struct _NET_DEV_BASECFG
{
    CHAR DevName[NET_DEV_DEVICE_NAME_LEN];		// 设备名称
    BYTE Language;		// 语言, NET_DEV_LANGUAGE
    BYTE VideoMode;		// 视频制式,NET_DEV_VIDEOMODE
    BYTE RecordRecycle;		// 是否循环录像, NET_DEV_REC_POLICY
    BYTE Res[1];
    NET_DEV_NVRCFG NvrCfg;
};


//设备版本信息
//-------------------------------------------------------------------

typedef struct _NET_DEV_VERSION NET_DEV_VERSION;
struct _NET_DEV_VERSION
{
    DWORD DevType;		// 设备类型： NET_DEV_TYPE
    CHAR ProductName[NET_DEV_DEVICE_NAME_LEN];		//产品型号
    CHAR SerialNo[NET_DEV_SERIAL_NO_LEN];		// 序列号
    CHAR SoftwareVer[NET_DEV_VERSION_LEN];		// 软件版本号
    CHAR SoftwareDate[NET_DEV_DATE_LEN];		// 软件生成日期
    CHAR DspSoftwareVer[NET_DEV_VERSION_LEN];		// DSP软件版本
    CHAR DspSoftwareDate[NET_DEV_DATE_LEN];		// DSP软件生成日期
    CHAR PanelVer[NET_DEV_VERSION_LEN];		// 前面板版本
    CHAR HardwareVer[NET_DEV_VERSION_LEN];		// 硬件版本
};


//===================================================================
//设备服务配置
//-------------------------------------------------------------------

typedef struct _NET_DEV_ALARMHOST_CFG NET_DEV_ALARMHOST_CFG;
struct _NET_DEV_ALARMHOST_CFG
{
    DWORD Enable;		//报警上传使能开关
    NET_DEV_DOMAINADDR Host;		//报警主机IP
    WORD Port;		//报警主机端口
    WORD Type;		//报警协议类型
    DWORD Protol;		//传输协议类型 NET_DEV_TRANSPROTOL
};


typedef struct _NET_DEV_CMS_CFG NET_DEV_CMS_CFG;
struct _NET_DEV_CMS_CFG
{
    DWORD Enable;		//是否开启
    CHAR PuId[NET_DEV_ID_LEN];		//设备ID
    CHAR PuAC[NET_DEV_AUTHORIZECODE_LEN];		//设备授权码
    NET_DEV_DOMAINADDR CmsIP;		//服务器IP
    NET_DEV_DOMAINADDR MdsIP;		//流媒体IP
    WORD CmsPort;		//服务器端口
    WORD MdsPort;		//流媒体端口
    WORD TransProtol;		//接入协议，NET_DEV_TRANSPROTOL
    WORD State;		//连接状态，1-CMS已连接  0-未连接
};


//wsx平台配置
//-------------------------------------------------------------------
typedef struct _NET_DEV_WSX_CFG NET_DEV_WSX_CFG;
struct _NET_DEV_WSX_CFG
{
    CHAR  PuId[64];
    CHAR  PuName[64];
    CHAR  PuPassword[64];
    DWORD ControlPort;
    DWORD SipPort;
    DWORD RtspPort;
    CHAR  Dns[16];
    DWORD RegSvrPort;
    CHAR  RegSvrIp[64];

    DWORD AlarmSvrPort;
    CHAR  AlarmSvrIp[16];
    CHAR  AlarmURL[128];
    DWORD EnableHb;
    DWORD HbPeriod;
    DWORD HbSvrPort;
    CHAR  HbSvrIp[64];

    DWORD EnableSipReg;
    DWORD SipRegSvrPort;
    CHAR  SipRegSvrIp[64];

    DWORD EnableSipHeartbeat;
    DWORD SipHbPeriod;
    DWORD SipHbSvrPort;
    CHAR  SipHbSvrIp[64];
    BYTE  EnableFlag; /* 0:不启用;  1:启用. */
    BYTE  ConType;        /* 1:tcp 0:udp */
    BYTE  OnLine;     /* 1:在线  0:不在线 */
    BYTE  SmoothFlag;/* 平滑标记  0:不平滑  1:平滑 */
    BYTE  PtzTime;
    BYTE  VideoConfCh;
    BYTE  AudioConfCh;  /* 视频会议音频通道，取值0x00 ~ MaxChannel ，取值为MaxChannel表示对讲通道*/
    BYTE  Used[1];
    BYTE    OffRecord;      /* 设备掉线录像 0:不启用;  1:启用. */
    BYTE    OffUpload;      /* 设备掉线录像上传 0:不启用;  1:启用. */
};

typedef struct _NET_DEV_UPNP_PORT NET_DEV_UPNP_PORT;
struct _NET_DEV_UPNP_PORT
{
    WORD Enable;		//该端口是否使能映射
    WORD ExtPort;		//映射的外部端口号
};


typedef struct _NET_DEV_UPNP_PORT_STATE NET_DEV_UPNP_PORT_STATE;
struct _NET_DEV_UPNP_PORT_STATE
{
    DWORD dwStatus;		//端口映射状态：0- 未生效；1- 未生效：映射源端口与目的端口需一致；2- 未生效：映射端口号已被使用；3- 生效
    WORD InternalPort;		//映射前的端口
    WORD ExternalPort;		//映射后的端口
};


typedef struct _NET_DEV_UPNP_CFG NET_DEV_UPNP_CFG;
struct _NET_DEV_UPNP_CFG
{
    WORD EnableUpnp;		//UPNP功能是否启用
    WORD EnableNat;		//UPNP端口映射（NAT）功能是否启用
    NET_DEV_IPADDR IpAddr;		//NAT路由器LAN IP地址
    WORD NatMode;		//0:固定映射 1:自动映射
    WORD RefreshTime;
    NET_DEV_UPNP_PORT ExpectMsgPort;
    NET_DEV_UPNP_PORT ExpectRtspPort;
    NET_DEV_UPNP_PORT ExpectHttpPort;
    NET_DEV_UPNP_PORT ExpectRtmpPort;
};


typedef struct _NET_DEV_UPNP_STATE NET_DEV_UPNP_STATE;
struct _NET_DEV_UPNP_STATE
{
    NET_DEV_DOMAINADDR NatInternalIp;		//NAT路由器LAN IP地址
    NET_DEV_DOMAINADDR NatExternalIp;		//映射后的外部地址
    NET_DEV_UPNP_PORT_STATE ResultMsgPort;
    NET_DEV_UPNP_PORT_STATE ResultRtspPort;
    NET_DEV_UPNP_PORT_STATE ResultHttpPort;
    NET_DEV_UPNP_PORT_STATE ResultRtmpPort;
};


typedef struct _NET_DEV_FTPUPLOAD_CFG NET_DEV_FTPUPLOAD_CFG;
struct _NET_DEV_FTPUPLOAD_CFG
{
    NET_DEV_DOMAINADDR IpAddr;		//服务器地址
    WORD Port;		//服务器端口
    CHAR UserName[NET_DEV_USERNAME_LEN];		//登陆用户名
    CHAR PassWord[NET_DEV_PASSWORD_LEN];		//登陆密码
    BYTE DirLevel;		//0- 不使用目录结构，直接保存在根目录；1- 使用1级目录；2- 使用2级目录
    BYTE TopDirMode;		//一级目录：0x1- 使用设备名，0x2- 使用设备号，0x3- 使用设备ip地址，0x4- 使用监测点，0x5- 使用时间(年月)，0x6- 使用自定义
    BYTE SubDirMode;		//二级目录：0x1- 使用通道名，0x2- 使用通道号，0x3- 使用时间(年月日)，0x4- 使用车道号，0x5- 使用自定义
    BYTE EnableAnony;		//启用匿名：0- 否，1- 是
    CHAR TopCustomDir[NET_DEV_MAX_PATH_LEN];		//自定义一级目录
    CHAR SubCustomDir[NET_DEV_MAX_PATH_LEN];		//自定义二级目录
};


typedef struct _NET_DEV_SMTP_CFG NET_DEV_SMTP_CFG;
struct _NET_DEV_SMTP_CFG
{
    NET_DEV_DOMAINADDR IpAddr;		//smtp 服务器地址
    WORD Port;		//smtp 服务器端口
    WORD SSLEnable;		//是否启用SSL
    NET_DEV_DOMAINADDR FromAddr;		//发送邮件地址
    CHAR FromUserName[NET_DEV_USERNAME_LEN];		//发送邮件账号
    CHAR FromPassWord[NET_DEV_PASSWORD_LEN];		//发送邮件密码
    NET_DEV_DOMAINADDR ToAddr1;		//接受邮件地址1
    NET_DEV_DOMAINADDR ToAddr2;		//接受邮件地址2
    NET_DEV_DOMAINADDR ToAddr3;		//接受邮件地址3
};


typedef enum _NET_DEV_DDNS_TYPE
{
    NET_DEV_DDNS_3322 = 1,
    NET_DEV_DDNS_DYNDNS,
}NET_DEV_DDNS_TYPE;


typedef struct _NET_DEV_DDNS_CFG NET_DEV_DDNS_CFG;
struct _NET_DEV_DDNS_CFG
{
    CHAR Account[NET_DEV_USERNAME_LEN];		//DDNS 注册名
    CHAR UserName[NET_DEV_USERNAME_LEN];		//用户名称
    CHAR PassWord[NET_DEV_PASSWORD_LEN];		//用户密码
    WORD Enable;		//DDNS开关
    WORD Type;		//DDNS类型 NET_DEV_DDNS_TYPE
    WORD Port;		//DDNS服务器端口
    WORD UpdateInterval;		//更新时间
};


typedef struct _NET_DEV_GB28181_CFG NET_DEV_GB28181_CFG;
struct _NET_DEV_GB28181_CFG
{
    CHAR DeviceId[NET_DEV_ID_LEN];		// 设备ID
    CHAR DevicePwd[NET_DEV_PASSWORD_LEN];		// 设备密码
    CHAR ServerId[NET_DEV_ID_LEN];		// 服务器ID
    NET_DEV_DOMAINADDR ServerIpAddr;		// 服务器地址
    WORD ServerPort;		// 服务器端口
    WORD LocalMsgPort;		// 本地消息端口
};


//设备网络配置
//-------------------------------------------------------------------

typedef enum _NET_DEV_NIC_IDX
{
    NET_DEV_NIC_IDX_ETH0   = 0,
    NET_DEV_NIC_IDX_ETH1   = 1,
    NET_DEV_NIC_IDX_ETH2   = 2,
    NET_DEV_NIC_IDX_ETH3   = 3,
    NET_DEV_NIC_IDX_WIFI0  = 4,
    NET_DEV_NIC_IDX_BUTT
}NET_DEV_NIC_IDX;


typedef enum _NET_DEV_NIC_STATE
{
    NET_DEV_NIC_DISCONNECTED = 0,        //断开
    NET_DEV_NIC_CONNECTING,              //正在连接
    NET_DEV_NIC_CONNECTED                //已连接
}NET_DEV_NIC_STATE;


typedef struct _NET_DEV_NETSTATES NET_DEV_NETSTATES;
struct _NET_DEV_NETSTATES
{
    BYTE NicState[NET_DEV_NIC_IDX_BUTT];		//NET_DEV_NIC_STATE
    BYTE PPPOEState;		//NET_DEV_NIC_STATE
    BYTE DIALState;		//NET_DEV_NIC_STATE
    BYTE Res[2];
};


typedef enum _NET_DEV_WIFI_WEP_AUTH
{
    NET_DEV_WIFI_WEP_OPEN   = 0,
    NET_DEV_WIFI_WEP_SHARED
}NET_DEV_WIFI_WEP_AUTH;


typedef enum _NET_DEV_WIFI_WPAPSK_ET
{
    NET_DEV_WIFI_WPAPSK_AES = 0,
    NET_DEV_WIFI_WPAPSK_TKIP
}NET_DEV_WIFI_WPAPSK_ET;


typedef struct _NET_DEV_WIFI_AP NET_DEV_WIFI_AP;
struct _NET_DEV_WIFI_AP
{
    CHAR EssId[NET_DEV_WIFI_ESSID_LEN];		//服务区别号
    DWORD Security;		//加密模式 NET_DEV_WIFISECURITY
    DWORD Authentication;		//认证模式 NET_DEV_WIFI_WEP_AUTH
    DWORD EncryptType;		//加密类型 'NET_DEV_WIFI_WPAPSK_ET', 只对应于加密类型为WPAPSK/WPA2PSK的情况
    DWORD Quality;		//信号质量 0-100
    DWORD BitRate;		//传输速率
};


typedef struct _NET_DEV_WIFI_APSET NET_DEV_WIFI_APSET;
struct _NET_DEV_WIFI_APSET
{
    DWORD Count;		// 个数
    NET_DEV_WIFI_AP APs[NET_DEV_MAX_WIFI_AP_NUM];
};


typedef struct _NET_DEV_WIFI_WPS NET_DEV_WIFI_WPS;
struct _NET_DEV_WIFI_WPS
{
    BYTE ConnectType;		//WPS连接方式：0- PBC，1- AP PIN
    BYTE Res[3];
    BYTE PIN[8];
    CHAR EssId[NET_DEV_WIFI_ESSID_LEN];		//服务区别号
};


//pppoe参数配置 ==========================> EthNet
typedef struct _NET_DEV_PPPOECFG NET_DEV_PPPOECFG;
struct _NET_DEV_PPPOECFG
{
    DWORD EnablePPPOE;
    DWORD NICUsed;		//拨号使用的网卡序号,NET_DEV_NIC_IDX
    NET_DEV_IPADDR IPAddr;		//IP地址
    CHAR UserName[NET_DEV_USERNAME_LEN];		//PPP拨号账号
    CHAR Passwd[NET_DEV_PASSWORD_LEN];		//PPP拨号密码
};


//ppp参数配置(子结构) ================> RS232
typedef struct _NET_DEV_PPPCFG NET_DEV_PPPCFG;
struct _NET_DEV_PPPCFG
{
    NET_DEV_IPADDR RemoteIP;		//远端IP地址
    NET_DEV_IPADDR LocalIP;		//本地IP地址
    NET_DEV_IPADDR LocalNetmask;		//本地IP地址掩码
    CHAR UserName[NET_DEV_USERNAME_LEN];		//用户名
    CHAR PassWord[NET_DEV_PASSWORD_LEN];		//密码
    BYTE PPPMode;		//PPP模式, 0－主动，1－被动
    BYTE Redial;		//是否回拨 ：0-否,1-是
    BYTE RedialMode;		//回拨模式,0-由拨入者指定,1-预置回拨号码
    BYTE DataEncrypt;		//数据加密,0-否,1-是
    DWORD Mtu;		//MTU
    CHAR TelNumber[NET_DEV_PHONENUMBER_LEN];		//电话号码
};


//3g拨号参数配置(子结构)========> 3G
typedef struct _NET_DEV_PPPDPARAM NET_DEV_PPPDPARAM;
struct _NET_DEV_PPPDPARAM
{
    CHAR DialNum[NET_DEV_USERNAME_LEN];		// dial number, eg. "#777"
    CHAR UserName[NET_DEV_USERNAME_LEN];		// user name, eg. "card"
    CHAR Password[NET_DEV_PASSWORD_LEN];		// password, eg. "card"
    CHAR Apn[NET_DEV_USERNAME_LEN];		// access point name, eg. "cmnet"
    NET_DEV_IPADDR LocalIp;		// local IP address
    NET_DEV_IPADDR RemoteIp;		// remote IP address
    WORD Mtu;		// max transfer unit of ppp interface
    BYTE VerifyProtocal;		// IPCP verify protocal: 0: auto; 1: CHAP; 2: PAP
    BYTE Res[25];
};


typedef struct _NET_DEV_QOSPARAM NET_DEV_QOSPARAM;
struct _NET_DEV_QOSPARAM
{
    WORD MaxBitUL;		// the maximum number of kbits/s in up-link traffic
    WORD MaxBitDL;		// the maximum number of kbits/s in down-link traffic
    BYTE TrafficClass;		// 0: conversationnal; 1: streaming; 2: interactive;3: background; 4: subscribed value(default)
    BYTE Res[7];
};


typedef struct _NET_DEV_DIALPARAM NET_DEV_DIALPARAM;
struct _NET_DEV_DIALPARAM
{
    BYTE Enable3G;		// 0: disable; 1: enable 3g
    BYTE DialMethod;		// dial methold: 0: auto dial; 1: manual dial;
    BYTE SwitchMethod;		// switch methold: 0: auto switch;1：3G优先，2: manual switch to 2G; 3: manual switch to 3G;
    BYTE EnaAlarmInDial;		// 使能报警输入触发拨号; 0: disable; 1: enable
    BYTE Res1[10];		//
    WORD OffLineTime;		// auto off line when no connection exist,单位：分钟，范围：5~65535分钟
    NET_DEV_PPPDPARAM PppdParam;
    NET_DEV_QOSPARAM QosParam;
    CHAR UimNumber[NET_DEV_PHONENUMBER_LEN];
    BYTE Res2[24];
};


//===================================================================
//设备时间配置
//-------------------------------------------------------------------

typedef enum _NET_DEV_WEEKDAY
{
    NET_DEV_SUNDAY=0,
    NET_DEV_MONDAY,
    NET_DEV_TUESDAY,
    NET_DEV_WEDNESDAY,
    NET_DEV_THURSDAY,
    NET_DEV_FRIDAY,
    NET_DEV_SATURDAY,
    NET_DEV_EVERYDAY,
    NET_DEV_WEEKDAY_BUTT
}NET_DEV_WEEKDAY;

typedef struct _NET_DEV_TIME_EX NET_DEV_TIME_EX;
struct _NET_DEV_TIME_EX
{
    DWORD Zone;		//时区
    WORD Year;		//年
    BYTE Month;		//月
    BYTE Date;		//日
    BYTE Hour;		//时
    BYTE Minute;		//分
    BYTE Second;		//秒
    BYTE Day;		//星期
    DWORD MilliSec;		//毫秒
};


//时间段(子结构)
typedef struct _NET_DEV_TIMESEG NET_DEV_TIMESEG;
struct _NET_DEV_TIMESEG
{
    DWORD Enable;		//是否生效
    BYTE StartHour;		//开始时间
    BYTE StartMin;
    BYTE EndHour;		//结束时间
    BYTE EndMin;
};


typedef struct _NET_DEV_DAY NET_DEV_DAY;
struct _NET_DEV_DAY
{
    DWORD Count;
    DWORD DayId;		//NET_DEV_WEEKDAY
    DWORD AllDayFlag;		//是否全天录像 1: 是， 0: 不是;
    NET_DEV_TIMESEG Segs[NET_DEV_MAX_SEG_NUM];
};


typedef struct _NET_DEV_WEEK NET_DEV_WEEK;
struct _NET_DEV_WEEK
{
    DWORD Count;
    NET_DEV_DAY Days[NET_DEV_WEEKDAY_BUTT];
};


typedef struct _NET_DEV_NTPCFG NET_DEV_NTPCFG;
struct _NET_DEV_NTPCFG
{
    DWORD EnableNTP;		//是否开启: 0：不开启，1：NTP对时，2：CMS对时
    DWORD EnableDST;		//夏令时使能；1：启用，0：不启用
    NET_DEV_DOMAINADDR NTPServer;		//NTP服务器
    DWORD TimeZone;		//时区
    DWORD UpdateInterval;		//对时间隔
};


//===================================================================
//编码参数
//-------------------------------------------------------------------

typedef enum _NET_DEV_STREAM_TYPE
{
    NET_DEV_STREAM_MAIN = 0,
    NET_DEV_STREAM_SECOND,
    NET_DEV_STREAM_THIRD
}NET_DEV_STREAM_TYPE;


typedef enum _NET_DEV_STREAM_MIX_TYPE
{
    NET_DEV_STREAM_MIX_VIDEO = 0,
    NET_DEV_STREAM_MIX_AV
}NET_DEV_STREAM_MIX_TYPE;


typedef enum _NET_DEV_VENC_TYPE
{
    NET_DEV_VENC_H264   = 0x00,
    NET_DEV_VENC_MJPEG  = 0x01,
    NET_DEV_VENC_JPEG   = 0x02,
    NET_DEV_VENC_MPEG4  = 0x03,
    NET_DEV_VENC_H265   = 0x04,
    NET_DEV_VENC_BUTT
}NET_DEV_VENC_TYPE;


typedef enum _NET_DEV_AENC_TYPE
{
    NET_DEV_AENC_G711A = 0x00,
    NET_DEV_AENC_G711U = 0x01,
    NET_DEV_AENC_G726  = 0x02,
    NET_DEV_AENC_AAC   = 0x03,
    NET_DEV_AENC_BUTT
}NET_DEV_AENC_TYPE;


typedef enum _NET_DEV_AUDIOINPUTMODE
{
    NET_DEV_AIM_MIC = 0,
    NET_DEV_AIM_LINE
}NET_DEV_AUDIOINPUTMODE;


typedef enum _NET_DEV_VIDEOFORMAT
{
    NET_DEV_VF_PAL = 0,
    NET_DEV_VF_NTSC
}NET_DEV_VIDEOFORMAT;


typedef enum _NET_DEV_VENC_BITRATE_TYPE
{
    NET_DEV_BT_ABR = 0,
    NET_DEV_BT_CBR,
    NET_DEV_BT_VBR,
    NET_DEV_BT_FIXQP,
    NET_DEV_BT_BUTT
}NET_DEV_VENC_BITRATE_TYPE;


typedef enum _NET_DEV_ENCODELEVEL
{
    NET_DEV_EL_BASELINE = 0,
    NET_DEV_EL_MAIN,
    NET_DEV_EL_HIGH
}NET_DEV_ENCODELEVEL;


typedef enum _NET_DEV_IMAGERESOLUTION
{
    NET_DEV_IMG_QCIF        = 0,    // 176*144    n制176*120
    NET_DEV_IMG_CIF         = 1,    // 352*288    n制352*240
    NET_DEV_IMG_HD1         = 2,    // 704*288    n制704*240
    NET_DEV_IMG_D1          = 3,    // 704*576    n制704*480
    NET_DEV_IMG_QQVGA       = 4,    // 160*112
    NET_DEV_IMG_QVGA        = 5,    // 320*240
    NET_DEV_IMG_VGA         = 6,    // 640*480
    NET_DEV_IMG_SVGA        = 7,    // 800*600
    NET_DEV_IMG_XGA         = 8,    // 1024*768
    NET_DEV_IMG_WXGA        = 9,    // 1280*800
    NET_DEV_IMG_SXGA        = 10,   // 1280*1024
    NET_DEV_IMG_UXGA        = 11,   // 1600*1200
    NET_DEV_IMG_720P        = 12,   // 1280*720
    NET_DEV_IMG_960         = 13,   // 1280*960
    NET_DEV_IMG_1080P       = 14,   // 1920*1080
    NET_DEV_IMG_3Mega       = 15,   // 2048*1536
    NET_DEV_IMG_4Mega       = 16,   // 2560x1440
    NET_DEV_IMG_5Mega       = 17,   // 2592*1944
    NET_DEV_IMG_UltraHD     = 18,   // 3840*2160 4k ultra hd
    NET_DEV_IMG_BUTT
}NET_DEV_IMAGERESOLUTION;


typedef struct _NET_DEV_ENCODEPARAM NET_DEV_ENCODEPARAM;
struct _NET_DEV_ENCODEPARAM
{
    DWORD EncodeLevel;		//编码等级 'NET_DEV_ENCODELEVEL'
    DWORD StreamMixType;		//码流模式NET_DEV_STREAM_MIX_TYPE
    DWORD FrameRate;		//帧率 数值型 常用值：5，10, 15, 20, 25, 30
    DWORD IsFrameRatePreferred;		//是否帧率优先 1: 是, 0: 不是
    DWORD IframeInterval;		//I帧间隔 数值型 常用值: 25，50，75，100
    DWORD VideoEncodeType;		//视频编码类型, NET_DEV_VENC_TYPE
    DWORD VideoFormat;		//制式 'NET_DEV_VIDEOFORMAT'
    DWORD ImageResolution;		//图像分辨率 'NET_DEV_IMAGERESOLUTION'
    DWORD QPValue;		//质量  数值型 常用值：25，50
    DWORD BitRateType;		//位率类型 'NET_DEV_VENC_BITRATE_TYPE'
    DWORD BitRate;		//位率 128k ~ 8000k
    DWORD AudioInputMode;		//音频输入类型 'NET_DEV_AUDIOINPUTMODE'
    DWORD AudioEncodeType;		//音频编码类型 'NET_DEV_AENC_TYPE'
};


typedef struct _NET_DEV_ENCODECFG NET_DEV_ENCODECFG;
struct _NET_DEV_ENCODECFG
{
    NET_DEV_ENCODEPARAM Main;		//主码流    编码参数
    NET_DEV_ENCODEPARAM Second;		//子码流    编码参数
    NET_DEV_ENCODEPARAM Third;		//第三码流  编码参数
};


//===================================================================
//视频效果参数
//-------------------------------------------------------------------

typedef struct _NET_DEV_VIDEO_EFFECT NET_DEV_VIDEO_EFFECT;
struct _NET_DEV_VIDEO_EFFECT
{
    DWORD Contrast;		//对比度
    DWORD Bright;		//亮度
    DWORD Hue;		//灰度
    DWORD Saturation;		//饱和度
    DWORD Sharpness;		//锐度
};


typedef struct _NET_DEV_VIDEO_INPUT_EFFECT NET_DEV_VIDEO_INPUT_EFFECT;
struct _NET_DEV_VIDEO_INPUT_EFFECT
{
    DWORD EffectMode;		//模式,0:用户定义, 1: 默认值
    NET_DEV_VIDEO_EFFECT CustomizeMode;		//自定义模式参数
};


//===================================================================
//外设
//-------------------------------------------------------------------

typedef struct _NET_DEV_SERIAL_CFG NET_DEV_SERIAL_CFG;
struct _NET_DEV_SERIAL_CFG
{
    WORD SerialNo;		//串口号,从0开始
    BYTE WorkMode;		//工作模式: 0 －透明通道, 1 - PTZ 模式(使用PTZ中的串口参数), 3 - 报警盒模式
    BYTE AlarmBox;		//报警盒协议类型
    DWORD BaudRate;		//串口波特率 115200, 57600, 38400, 19200, 9600, 4800, 2400, 1800, 1200, 600
    BYTE DataBit;		//数据位 5, 6, 7, 8
    BYTE StopBit;		//停止位 1, 2
    BYTE Verify;		//校验位 0:无，1:奇校验 2:偶校验
    BYTE FlowControl;		//流控 0－无，1－软流控, 2-硬流控
};


//===================================================================
//云台
//-------------------------------------------------------------------

typedef struct _NET_DEV_PTZ_PRESET NET_DEV_PTZ_PRESET;
struct _NET_DEV_PTZ_PRESET
{
    DWORD Valid;		//是否有效
    DWORD No;		//预置位号
    CHAR Name[NET_DEV_PTZ_NAME_LEN];
};


typedef struct _NET_DEV_PTZ_CRUISEPOINT NET_DEV_PTZ_CRUISEPOINT;
struct _NET_DEV_PTZ_CRUISEPOINT
{
    NET_DEV_PTZ_PRESET Preset;		//预置点
    WORD Speed;		//移动速度
    WORD Dwell;		//停留时间
};


typedef struct _NET_DEV_PTZ_CRUISE NET_DEV_PTZ_CRUISE;
struct _NET_DEV_PTZ_CRUISE
{
    DWORD Valid;		//是否有效
    DWORD No;		//巡航号
    CHAR Name[NET_DEV_PTZ_NAME_LEN];
};


typedef struct _NET_DEV_PTZ_PATTERN NET_DEV_PTZ_PATTERN;
struct _NET_DEV_PTZ_PATTERN
{
    DWORD Valid;		//是否有效
    DWORD No;		//轨迹号
    CHAR Name[NET_DEV_PTZ_NAME_LEN];
};


typedef struct _NET_DEV_PTZ_3D NET_DEV_PTZ_3D;
struct _NET_DEV_PTZ_3D
{
    DWORD XOffset;
    DWORD YOffset;
    DWORD Amplify;
};


typedef enum _NET_DEV_PTZ_ACTION
{
    NET_DEV_PTZ_ID_STOP        = 0,     //停止
    NET_DEV_PTZ_ID_AUTO        = 1,     //自动
    NET_DEV_PTZ_ID_LEFT        = 2,     //左
    NET_DEV_PTZ_ID_RIGHT       = 3,     //右
    NET_DEV_PTZ_ID_UP          = 4,     //上
    NET_DEV_PTZ_ID_DOWN        = 5,     //下
    NET_DEV_PTZ_ID_LEFT_UP     = 6,     //左上
    NET_DEV_PTZ_ID_LEFT_DOWN   = 7,     //左下
    NET_DEV_PTZ_ID_RIGHT_UP    = 8,     //右上
    NET_DEV_PTZ_ID_RIGHT_DOWN  = 9,     //右下
    NET_DEV_PTZ_ID_SET_PP      = 10,    //设置预置点
    NET_DEV_PTZ_ID_USE_PP      = 11,    //调用预置点
    NET_DEV_PTZ_ID_DEL_PP      = 12,    //删除预置点
    NET_DEV_PTZ_ID_ADD_ZOOM    = 13,    //焦距放大
    NET_DEV_PTZ_ID_SUB_ZOOM    = 14,    //焦距缩小
    NET_DEV_PTZ_ID_ADD_FOCUS   = 15,    //远焦点
    NET_DEV_PTZ_ID_SUB_FOCUS   = 16,    //近焦点
    NET_DEV_PTZ_ID_ADD_IRIS    = 17,    //光圈+
    NET_DEV_PTZ_ID_SUB_IRIS    = 18,    //光圈-
    NET_DEV_PTZ_ID_TURN_ON     = 19,    //开启灯光
    NET_DEV_PTZ_ID_TURN_OFF    = 20,    //关闭灯光
    NET_DEV_PTZ_ID_WIPERS_ON   = 21,    //开启雨刷
    NET_DEV_PTZ_ID_WIPERS_OFF  = 22,    //关闭雨刷
    NET_DEV_PTZ_ID_START_CRZ   = 23,    //开始巡航
    NET_DEV_PTZ_ID_STOP_CRZ    = 24,    //停止巡航
    NET_DEV_PTZ_ID_3D          = 25,    //3D控球
    NET_DEV_PTZ_ID_LEFT_LIMIT  = 26,    //设置左限位
    NET_DEV_PTZ_ID_RIGHT_LIMIT = 27,    //设置右限位
    NET_DEV_PTZ_ID_RANDOM      = 28,    //随机扫描
    NET_DEV_PTZ_ID_RUN_PTN     = 29,    //运行轨迹
    NET_DEV_PTZ_ID_START_PTN   = 30,    //开始轨迹录制
    NET_DEV_PTZ_ID_STOP_PTN    = 31,    //停止轨迹录制
}NET_DEV_PTZ_ACTION;


typedef struct _NET_DEV_PTZ_CONTROL NET_DEV_PTZ_CONTROL;
struct _NET_DEV_PTZ_CONTROL
{
    DWORD Action;		// NET_DEV_PTZ_ACTION
    DWORD Flag;		// 方向操作启停标志  1: 开始 0: 停止;
    DWORD Speed;		// 调用速度 0 - 100
    union {
        NET_DEV_PTZ_PRESET Preset;
        NET_DEV_PTZ_CRUISE Cruise;
        NET_DEV_PTZ_PATTERN Pattern;
        NET_DEV_PTZ_3D D3D;
    } Param;
};


//===================================================================
//云台配置
//-------------------------------------------------------------------

typedef enum _NET_DEV_PTZ_PROTOL
{
    NET_DEV_PTZ_PROTOL_PELCOD = 0,
    NET_DEV_PTZ_PROTOL_PELCOP = 1,
    NET_DEV_PTZ_PROTOL_BUTT
}NET_DEV_PTZ_PROTOL;


typedef struct _NET_DEV_PTZ_CFG NET_DEV_PTZ_CFG;
struct _NET_DEV_PTZ_CFG
{
    WORD SerialNo;		// 保留
    WORD PtzAddr;		// 云台地址
    DWORD Protol;		// NET_DEV_PTZ_PROTOL
    DWORD BaudRate;		// 波特率   请参考 NET_DEV_RS232_CFG 定义解释
    BYTE DataBit;		// 数据位   请参考 NET_DEV_RS232_CFG 定义解释
    BYTE StopBit;		// 停止位   请参考 NET_DEV_RS232_CFG 定义解释
    BYTE Verify;		// 校验     请参考 NET_DEV_RS232_CFG 定义解释
    BYTE FlowControl;		// 流控     请参考 NET_DEV_RS232_CFG 定义解释
};


typedef struct _NET_DEV_PTZ_PRESETSET NET_DEV_PTZ_PRESETSET;
struct _NET_DEV_PTZ_PRESETSET
{
    DWORD PresetCount;		//预置点数量
    NET_DEV_PTZ_PRESET Presets[NET_DEV_MAX_PRESET_NUM];		//预置点集合
};


//云台轨迹集合
typedef struct _NET_DEV_PTZ_PATTERNSET NET_DEV_PTZ_PATTERNSET;
struct _NET_DEV_PTZ_PATTERNSET
{
    DWORD PatternCount;
    NET_DEV_PTZ_PATTERN Patterns[NET_DEV_PTZ_MAX_PTN_NUM];
};


//巡航路径 NET_DEV_PTZ_CRUISEWAY
//一条巡航路径的详细信息
//-----------------------------------------------------
typedef struct _NET_DEV_PTZ_CRUISEWAY NET_DEV_PTZ_CRUISEWAY;
struct _NET_DEV_PTZ_CRUISEWAY
{
    NET_DEV_PTZ_CRUISE Cruise;
    DWORD StepCount;		//巡航点数量
    NET_DEV_PTZ_CRUISEPOINT Steps[NET_DEV_MAX_CRZ_STEP_NUM];		//巡航点集合(路径)
};


//巡航路径集合 NET_DEV_PTZ_CRUISEWAYSET
//只包含巡航名称，巡航号，是否开启，不包含巡航点信息
//------------------------------------------------------
typedef struct _NET_DEV_PTZ_CRUISEWAYSET NET_DEV_PTZ_CRUISEWAYSET;
struct _NET_DEV_PTZ_CRUISEWAYSET
{
    DWORD CruiseCount;		//巡航路径数量
    NET_DEV_PTZ_CRUISE Cruises[NET_DEV_MAX_CRZ_NUM];		//巡航路径集合
};


typedef enum _NET_DEV_PTZ_JOIN_ACTION_TYPE
{
    PTZ_ACTION_NONE        = 0,
    PTZ_ACTION_CALL_PRESET = 1,
    PTZ_ACTION_RUN_CRUISE  = 2,
    PTZ_ACTION_AUTO_SCAN   = 3,
    PTZ_ACTION_RANDOM_SCAN = 4,
    PTZ_ACTION_RUN_PTN     = 5
}NET_DEV_PTZ_JOIN_ACTION_TYPE;


//云台守望参数结构体
//守望等待时间：守望配置后执行动作的等待时间。
//守望动作的巡航扫描、预置点需要先配置。
//支持的巡航路径编号、预置点编号由能力集获取。
//-----------------------------------------
typedef struct _NET_DEV_PTZ_PARKACTION_CFG NET_DEV_PTZ_PARKACTION_CFG;
struct _NET_DEV_PTZ_PARKACTION_CFG
{
    BYTE Enable;		//是否启用：0-不启用，1-启用
    BYTE Res[3];		//保留
    DWORD ParkTime;		//守望等待时间，单位：s
    WORD Action;		//守望动作类型： NET_DEV_PTZ_JOIN_ACTION_TYPE
    WORD No;		// 巡航号、预置点号
};


//云台定时任务结构体
//在时间段内超过等待时间执行任务
//---------------------------------------------
typedef struct _NET_DEV_PTZ_SCHEDTASK NET_DEV_PTZ_SCHEDTASK;
struct _NET_DEV_PTZ_SCHEDTASK
{
    NET_DEV_TIMESEG Seg;		//时间段
    WORD Action;		//NET_DEV_PTZ_JOIN_ACTION_TYPE
    WORD No;		//巡航号、预置点号
};


typedef struct _NET_DEV_PTZ_TIME_TASK NET_DEV_PTZ_TIME_TASK;
struct _NET_DEV_PTZ_TIME_TASK
{
    BYTE Enable;		//是否启用：0-不启用，1-启用
    BYTE Res[3];		//保留
    DWORD ParkTime;		//闲置等待时间，单位：s
    DWORD TaskCount;		//任务个数
    NET_DEV_PTZ_SCHEDTASK Tasks[NET_DEV_PTZ_SCHTASKS_NUM];		//任务
};


//===================================================================
//通道
//-------------------------------------------------------------------

typedef struct _NET_DEV_RECT NET_DEV_RECT;
struct _NET_DEV_RECT
{
    WORD Left;
    WORD Top;
    WORD Right;
    WORD Bottom;
};


typedef struct _NET_DEV_AREA NET_DEV_AREA;
struct _NET_DEV_AREA
{
    DWORD MaxWidth;		//最大宽度
    DWORD MaxHeight;		//最大高度
    WORD Count;		//数据中区域个??
    WORD RectMask;		//按位标示区域是否有效
    NET_DEV_RECT Rects[NET_DEV_MAX_AREA_NUM];
};


typedef struct _NET_DEV_VENC_OSD_TEXT NET_DEV_VENC_OSD_TEXT;
struct _NET_DEV_VENC_OSD_TEXT
{
    DWORD TextEnable;	//是否显示文本；1：显示，0：隐藏
    DWORD TextX;		//文本坐标X
    DWORD TextY;		//文本坐标Y
    DWORD TextW;		//文本宽度(获取有效)
    DWORD TextH;		//文本高度(获取有效)
    CHAR TextData[NET_DEV_OSD_TEXT_LEN];
};


typedef struct _NET_DEV_VENC_OSD NET_DEV_VENC_OSD;
struct _NET_DEV_VENC_OSD
{
    DWORD BitRateEnable;		//是否显示码流信息；1：显示，0：隐藏
    DWORD DisplayAttr;		//osd 反色；1：开启，0：关闭
    DWORD DisplayColor;		//osd显示颜色
    DWORD MaxWidth;		//最大宽度
    DWORD MaxHeight;		//最大高度
    DWORD TimeEnable;		//是否显示时间；1：显示，0：隐藏
    DWORD TimeX;		//时间坐标X
    DWORD TimeY;		//时间坐标Y
    DWORD TimeW;		//时间宽度(获取有效)
    DWORD TimeH;		//时间高度(获取有??)
    NET_DEV_VENC_OSD_TEXT Texts[NET_DEV_OSD_TEXT_NUM];
};


typedef struct _NET_DEV_VENC_OVERLAY NET_DEV_VENC_OVERLAY;
struct _NET_DEV_VENC_OVERLAY
{
    DWORD Enable;		//是否启用遮挡
    DWORD Color;		//遮挡时使用的颜色
    NET_DEV_AREA Area;		//遮挡区域,数目为0即遮挡无效
};


typedef struct _NET_DEV_RECORD NET_DEV_RECORD;
struct _NET_DEV_RECORD
{
    NET_DEV_WEEK Week;		//定时录像时间段
    WORD EnableWeek;		//是否开启定时录像 0-禁止 1-开启
    WORD StreamType;		//码流类型: NET_DEV_STREAM_TYPE
    WORD PreRecord;		//通道预录时长，单位：秒
    WORD AutoCover;		//通道录像是否自动覆盖 0: 磁盘满停止, 1: 自动覆盖  //???? Tom
    WORD RecordTime;		//录象延时时间 单位：秒
    WORD Redundancy;		//是否冗余录像,重要数据双备份：0/1
    WORD AudioRec;		//录像时复合流编码时是否记录音频数据：国外有此法规
    WORD LockDuration;		//录像锁定时长，单位小时 0表示不锁定，0xffff表示永久锁定，录像段的时长大于锁定的持续时长的录像，将不会锁定
    WORD RecordBackup;		//0:录像不存档 1：录像存档
    WORD SVCLevel;		//SVC抽帧类型：0-不抽，1-抽二分之一 2-抽四分之三
};


typedef struct _NET_DEV_SNAPPIC_TIMERPARAM NET_DEV_SNAPPIC_TIMERPARAM;
struct _NET_DEV_SNAPPIC_TIMERPARAM
{
    BYTE Enable;
    BYTE PicType;		//
    BYTE Resolution;		//NET_DEV_IMAGERESOLUTION
    BYTE Quality;
    BYTE Interval;		//抓拍间隔
    BYTE Ftp;		//定时抓拍ftp 上传
    BYTE Res[2];
};


typedef struct _NET_DEV_SNAPPIC_ALARMPARAM NET_DEV_SNAPPIC_ALARMPARAM;
struct _NET_DEV_SNAPPIC_ALARMPARAM
{
    BYTE Enable;
    BYTE PicType;
    BYTE Resolution;		//NET_DEV_IMAGERESOLUTION
    BYTE Quality;
    BYTE Interval;		//抓拍间隔
    BYTE Times;		//抓拍张数
    BYTE Ftp;		//
    BYTE Email;
};


typedef struct _NET_DEV_SNAPPIC NET_DEV_SNAPPIC;
struct _NET_DEV_SNAPPIC
{
    NET_DEV_WEEK Week;		//定时抓拍时间段
    NET_DEV_SNAPPIC_TIMERPARAM TimerParam;		//定时抓拍参数
    NET_DEV_SNAPPIC_ALARMPARAM AlarmParam;		//告警联动抓拍参数
};


//==============================================
//告警配置
//--------------------------------------------

typedef enum _NET_DEV_ALARM_TYPE
{
    NET_DEV_ALARM_TYPE_MOTION = 0,
    NET_DEV_ALARM_TYPE_LOST   = 1,
    NET_DEV_ALARM_TYPE_HIDE   = 2,
    NET_DEV_ALARM_TYPE_IO     = 3,
    NET_DEV_ALARM_TYPE_PIR    = 4,
    NET_DEV_ALARM_TYPE_AVD    = 5,
    NET_DEV_ALARM_TYPE_PEA_OSC= 6,
    NET_DEV_ALARM_TYPE_BUTT
}NET_DEV_ALARM_TYPE;


typedef enum _NET_DEV_EXCEPTION_TYPE
{
    NET_DEV_EXCEPTION_TYPE_HDD_NO     = 0,
    NET_DEV_EXCEPTION_TYPE_HDD_FULL   = 1,
    NET_DEV_EXCEPTION_TYPE_HDD_ERR    = 2,
    NET_DEV_EXCEPTION_TYPE_NET_BROKEN = 3,
    NET_DEV_EXCEPTION_TYPE_IP_CONFLICT= 4,
    NET_DEV_EXCEPTION_TYPE_BUTT
}NET_DEV_EXCEPTION_TYPE;


typedef struct _NET_DEV_MOTION_ALARM NET_DEV_MOTION_ALARM;
struct _NET_DEV_MOTION_ALARM
{
    DWORD Enable;
    DWORD Sensitive;
    DWORD AlarmInterval;
    NET_DEV_AREA Area;
    NET_DEV_WEEK Week;
};


typedef struct _NET_DEV_LOST_ALARM NET_DEV_LOST_ALARM;
struct _NET_DEV_LOST_ALARM
{
    DWORD Enable;
    DWORD AlarmInterval;		//报警间隔 单位 s
    NET_DEV_WEEK Week;
};


typedef struct _NET_DEV_HIDE_ALARM NET_DEV_HIDE_ALARM;
struct _NET_DEV_HIDE_ALARM
{
    DWORD Enable;
    DWORD AlarmInterval;		//报警间隔 单位 s
    DWORD Sensitive;		//灵敏度字段，范围 0-4，值越低，越灵敏
    NET_DEV_AREA Area;
    NET_DEV_WEEK Week;
};


typedef struct _NET_DEV_ALARMIN_ALARM NET_DEV_ALARMIN_ALARM;
struct _NET_DEV_ALARMIN_ALARM
{
    CHAR Name[NET_DEV_ALARM_NAME_LEN];
    WORD AlarmInNo;		//告警输入号, 从 0 开始
    WORD Enable;		//是否开启
    DWORD InputType;		//报警输入类型，1：常开(默认)，0：常闭
    DWORD AlarmInterval;		//报警间隔 单位 s
    NET_DEV_WEEK Week;
};


typedef struct _NET_DEV_ALARMOUTCFG NET_DEV_ALARMOUTCFG;
struct _NET_DEV_ALARMOUTCFG
{
    CHAR Name[NET_DEV_ALARM_NAME_LEN];		// 名称
    WORD AlarmOutNo;		// 告警输出号, 从0开始
    WORD Enable;		// 是否开启
    DWORD OutDelay;		// 输出保持时间(-1为无限，手动关闭)
    NET_DEV_WEEK Week;
};


typedef struct _NET_DEV_PIR_ALARM NET_DEV_PIR_ALARM;
struct _NET_DEV_PIR_ALARM
{
    DWORD Enable;
    DWORD AlarmInterval;		//报警间隔 单位 s
    NET_DEV_WEEK Week;
};


//告警处理方式，按移位掩码
typedef enum _NET_DEV_ALARM_HANDLE_TYPE
{
    NET_DEV_ALARM_HANDLE_TYPE_BEEP       = 0,   //声音警告
    NET_DEV_ALARM_HANDLE_TYPE_RECORD     = 1,   //联动录像
    NET_DEV_ALARM_HANDLE_TYPE_ALARMOUT   = 2,   //联动输出
    NET_DEV_ALARM_HANDLE_TYPE_ALARMHOST  = 3,   //发送到报警主机
    NET_DEV_ALARM_HANDLE_TYPE_SNAPPIC    = 4,   //联动抓拍
    NET_DEV_ALARM_HANDLE_TYPE_PTZ        = 5,   //联动ptz
    NET_DEV_ALARM_HANDLE_TYPE_EMAIL      = 6,   //联动Email
}NET_DEV_ALARM_HANDLE_TYPE;


typedef struct _NET_DEV_ALARM_PTZ_ACTION NET_DEV_ALARM_PTZ_ACTION;
struct _NET_DEV_ALARM_PTZ_ACTION
{
    BYTE Action;		// NET_DEV_PTZ_JOIN_ACTION_TYPE
    BYTE No;		//Action 为preset 则为preset no, 为 Action 则为 cruise no, 为自动扫描则无效
    BYTE Res[2];
};


typedef struct _NET_DEV_ALARM_HANDLE NET_DEV_ALARM_HANDLE;
struct _NET_DEV_ALARM_HANDLE
{
    DWORD TypeMask;		//NET_DEV_ALARM_HANDLE_TYPE
    BYTE AlarmOutMask[NET_DEV_MAX_ALARM_OUT_NUM];		//联动报警输出通道
    BYTE RecordChannMask[NET_DEV_MAX_CHANN_NUM];		//联动录像通道
    BYTE SnapChannMask[NET_DEV_MAX_CHANN_NUM];		//联动抓拍通道
    #if 0 //此部分移除到 “抓盘参数”，“录像参数”
    array int08 SnapFtp[NET_DEV_MAX_CHANN_NUM];         //联动通道抓拍FTP上传
    array int08 RecFtp[NET_DEV_MAX_CHANN_NUM];          //联动通道录像FTP上传
    array int08 Email[NET_DEV_MAX_CHANN_NUM];           //联动通道Email发送
    #endif
    NET_DEV_ALARM_PTZ_ACTION Ptzs[NET_DEV_MAX_CHANN_NUM];		//联动云台动作
};


typedef struct _NET_DEV_ALARM_SRC NET_DEV_ALARM_SRC;
struct _NET_DEV_ALARM_SRC
{
    WORD AlarmType;		//NET_DEV_ALARM_TYPE
    WORD AlarmNo;		//告警输入号, 从 0 开始, 视频告警为通道号, IO告警为 IO输入号
};


typedef struct _NET_DEV_ALARM_JOINT NET_DEV_ALARM_JOINT;
struct _NET_DEV_ALARM_JOINT
{
    NET_DEV_ALARM_SRC AlarmSource;
    NET_DEV_ALARM_HANDLE Process;
};


typedef struct _NET_DEV_EXCEPTION NET_DEV_EXCEPTION;
struct _NET_DEV_EXCEPTION
{
    NET_DEV_ALARM_HANDLE Handles[NET_DEV_EXCEPTION_TYPE_BUTT];
};


//===================================================================
//磁盘
//-----------------------------------------------

typedef enum _NET_DEV_DISK_STATE
{
    NET_DEV_DISK_USING = 0,     /* 正在使用   */
    NET_DEV_DISK_MOUNTED,       /* 已挂载     */
    NET_DEV_DISK_UNMOUNT,       /* 未挂载     */
    NET_DEV_DISK_FORMATTING     /* 正在格式化 */
}NET_DEV_DISK_STATE;


typedef enum _NET_DEV_DISK_TYPE
{
    NET_DEV_DISK_SATA=0,
    NET_DEV_DISK_USB,
    NET_DEV_DISK_ISCSI,
    NET_DEV_DISK_NFS,
    NET_DEV_DISK_SD
}NET_DEV_DISK_TYPE;


typedef enum _NET_DEV_FILESYSTEM_TYPE
{
    NET_DEV_FS_FAT16=0,
    NET_DEV_FS_FAT32,
    NET_DEV_FS_NTFS,
    NET_DEV_FS_EXT,
    NET_DEV_FS_EXT3,
    NET_DEV_FS_EXT4
}NET_DEV_FILESYSTEM_TYPE;


typedef struct _NET_DEV_PHY_DISK_INFO NET_DEV_PHY_DISK_INFO;
struct _NET_DEV_PHY_DISK_INFO
{
    DWORD DiskNo;		//磁盘号
    DWORD TotalSize;		//磁盘容量 单位M
    DWORD FreeSize;		//剩余容量 单位M
    DWORD IsBackup;		//是否备份盘
    DWORD State;		//NET_DEV_DISK_STATE
    DWORD DiskType;		//NET_DEV_DISK_TYPE
    DWORD FileSystemType;		//NET_DEV_FILESYSTEM_TYPE
};


typedef struct _NET_DEV_PHY_DISK NET_DEV_PHY_DISK;
struct _NET_DEV_PHY_DISK
{
    DWORD DiskCount;
    NET_DEV_PHY_DISK_INFO PhyDisks[NET_DEV_MAX_PHY_DISK];
};


typedef struct _NET_DEV_NAS_MOUNT_PARAM NET_DEV_NAS_MOUNT_PARAM;
struct _NET_DEV_NAS_MOUNT_PARAM
{
    BYTE MountType;		//0～保留,1~NFS, 2~ SMB/CIFS
    CHAR UserName[NET_DEV_USERNAME_LEN];		// 用户名(SMB/CIFS时生效)
    CHAR PassWord[NET_DEV_PASSWORD_LEN];		// 密码
};


typedef struct _NET_DEV_NET_SINGLE_DISK NET_DEV_NET_SINGLE_DISK;
struct _NET_DEV_NET_SINGLE_DISK
{
    BYTE NetDiskType;		// 网络硬盘类型, 0-NFS(NAS),1-iSCSI
    NET_DEV_IPADDR NetDiskAddr;		// 网络硬盘地址
    CHAR Directory[NET_DEV_MAX_PATH_LEN];		// PATHNAME_LEN = 256
    WORD IscsiPort;		// iscsi端口，现在为默认
    NET_DEV_NAS_MOUNT_PARAM MountParam;		// NAS挂载方式 (byNetDiskType为NFS(NAS)时生效)
};


//网络硬盘结构配置
typedef struct _NET_DEV_NET_DISKCFG NET_DEV_NET_DISKCFG;
struct _NET_DEV_NET_DISKCFG
{
    DWORD DiskCount;
    NET_DEV_NET_SINGLE_DISK SingleDisks[NET_DEV_MAX_NET_DISK];
};


//录像查询
//-----------------------------------------------

typedef enum _NET_DEV_RECORD_TYPE
{
    NET_DEV_RECORD_TIMER      = 1,
    NET_DEV_RECORD_ALARM      = 2,
    NET_DEV_RECORD_MOTION     = 3,
    NET_DEV_RECORD_MANUAL     = 4,
    NET_DEV_RECORD_LOST       = 5,
    NET_DEV_RECORD_HIDE       = 6,
    NET_DEV_RECORD_NET_FAULT  = 7,
    NET_DEV_RECORD_ALL        = 0xffffffff
}NET_DEV_RECORD_TYPE;


typedef struct _NET_DEV_RECORD_ITEM NET_DEV_RECORD_ITEM;
struct _NET_DEV_RECORD_ITEM
{
    DWORD RecordType;		// 掩码 NET_DEV_RECORD_TYPE
    DWORD RecordSize;
    DWORD BeginTime;
    DWORD EndTime;
    CHAR Property[NET_DEV_REC_PROPERTY_SIZE];
};


typedef struct _NET_DEV_RECORD_OUTLINE_ITEM NET_DEV_RECORD_OUTLINE_ITEM;
struct _NET_DEV_RECORD_OUTLINE_ITEM
{
    DWORD Time;
    BYTE RecordType;
};


typedef enum _NET_DEV_RECORD_DETAIL_TYPE
{
    NET_DEV_RECORD_DETAIL_ITEM    = 0,
    NET_DEV_RECORD_DETAIL_OUTLINE
}NET_DEV_RECORD_DETAIL_TYPE;


//录像查询条件
typedef struct _NET_DEV_RECORD_COND NET_DEV_RECORD_COND;
struct _NET_DEV_RECORD_COND
{
    DWORD DetailType;		//NET_DEV_RECORD_DETAIL_TYPE
    DWORD RecordType;		//掩码 NET_DEV_RECORD_TYPE
    DWORD BeginTime;
    DWORD EndTime;
};


//录像查询请求
typedef struct _NET_DEV_RECORD_QUERY_REQ NET_DEV_RECORD_QUERY_REQ;
struct _NET_DEV_RECORD_QUERY_REQ
{
    NET_DEV_RECORD_COND Cond;
    DWORD BeginNo;
    DWORD ReqCount;
    DWORD SessionId;
};


//录像查询回应
typedef struct _NET_DEV_RECORD_QUERY_RSP NET_DEV_RECORD_QUERY_RSP;
struct _NET_DEV_RECORD_QUERY_RSP
{
    DWORD SessionId;
    DWORD TotalCount;
    DWORD RspCount;
    NET_DEV_RECORD_ITEM Items[NET_DEV_MAX_REC_PAGE_SIZE];
};


//媒体
//--------------------------------------------------

typedef enum _NET_DEV_MEDIA_URL_TYPE
{
    NET_DEV_MEDIA_URL_REALTIME_AV  = 0,        //实时音视频流
    NET_DEV_MEDIA_URL_HISTORY_AV,              //历史音视频（点播）
    NET_DEV_MEDIA_URL_HISTORY_AV_DOWNLOAD,     //历史音视频（下载）
    NET_DEV_MEDIA_URL_REALTIME_PICTURE,        //实时抓拍图片
    NET_DEV_MEDIA_URL_HISTORY_PICTURE          //历史图片
}NET_DEV_MEDIA_URL_TYPE;


typedef struct _NET_DEV_MEDIA_URL NET_DEV_MEDIA_URL;
struct _NET_DEV_MEDIA_URL
{
    DWORD StreamType;		//码流类型
    DWORD MediaType;		//NET_DEV_MEDIA_URL_TYPE
    NET_DEV_IPADDR IpAddr;		//服务器IP
    CHAR Url[NET_DEV_MAX_URL_LEN];		//返回的URL
    NET_DEV_IPADDR ClientIpAddr;		//客户端IP
};


//告警信息

typedef enum _NET_DEV_ALARM_MAJOR_TYPE
{
    NET_DEV_ALARM_MAJOR_ALARM     = 0,    //告警 AlarmSubType = NET_DEV_ALARM_TYPE
    NET_DEV_ALARM_MAJOR_EXCEPTION,        //异常 AlarmSubType = NET_DEV_EXCEPTION_TYPE
    NET_DEV_ALARM_MAJOR_STATE,            //状态 AlarmSubType = NET_DEV_STATE_IDX
    NET_DEV_ALARM_MAJOR_EXT,              //扩展 AlarmSubType = NET_DEV_EXT_TYPE
}NET_DEV_ALARM_MAJOR_TYPE;

//--------------------------------------------------
typedef struct _NET_DEV_ALARM_ITEM NET_DEV_ALARM_ITEM;
struct _NET_DEV_ALARM_ITEM
{
    DWORD Channel;		//通道号
    DWORD AlarmType;		//主类型 NET_DEV_ALARM_MAJOR_TYPE
    DWORD AlarmSubType;		//子类型 NET_DEV_ALARM_TYPE, NET_DEV_EXCEPTION_TYPE
    DWORD Action;		//0：开始告警，1：结束告警
    DWORD Time;
    BYTE Data[NET_DEV_ALARM_DATA_SIZE];		//SubType=AVD/PEAOSC:NET_DEV_ALARM_DESC;SubType=STATE_CHANNNEL:NET_DEV_CHANNSTATE;
};


//告警信息二级描述
//--------------------------------------------------
typedef struct _NET_DEV_ALARM_DESC NET_DEV_ALARM_DESC;
struct _NET_DEV_ALARM_DESC
{
    DWORD Type;		// AVD-->NET_DEV_AVD_TYPE, PEAOSC-->NET_DEV_PEAOSC_EVENT_TYPE
    DWORD Id;		// 保留
    DWORD Rule;		// 保留
    CHAR Desc[NET_DEV_ALARM_DESC_LEN];
};


//用户信息
//---------------------------------------

typedef enum _NET_DEV_USER_LOCALRIGHT
{
    NET_DEV_USER_LR_PTZ_CTL = 0,/*本地控制云台*/
    NET_DEV_USER_LR_REC_MANUL,  /*本地手动录象*/
    NET_DEV_USER_LR_REC_PLAY,   /*本地回放*/
    NET_DEV_USER_LR_PARAM_SET,  /*本地设置参数*/
    NET_DEV_USER_LR_PARAM_GET,  /*本地查看参数 */
    NET_DEV_USER_LR_ADVANCED,   /*本地高级操作(升级，格式化)*/
    NET_DEV_USER_LR_LOG_STATE,  /*本地查看状态、日志*/
    NET_DEV_USER_LR_CHANNEL,    /*本地管理模拟和IP camera */
    NET_DEV_USER_LR_BACKUP,     /*本地备份 */
    NET_DEV_USER_LR_POWEROFF,   /*本地关机/重启 */
}NET_DEV_USER_LOCALRIGHT;

typedef enum _NET_DEV_USER_REMOTERIGHT
{
    NET_DEV_USER_RR_PTZ_CTL = 0,/*远程控制云台*/
    NET_DEV_USER_RR_REC_MANUL,  /*远程手动录象*/
    NET_DEV_USER_RR_REC_PLAY,   /*远程回放 */
    NET_DEV_USER_RR_PARAM_SET,  /*远程设置参数*/
    NET_DEV_USER_RR_PARAM_GET,  /*远程查看参数 */
    NET_DEV_USER_RR_ADVANCED,   /*远程高级操作(升级，格式化)*/
    NET_DEV_USER_RR_LOG_STATE,  /*远程查看状态、日志*/
    NET_DEV_USER_RR_REAL_TALK,  /*远程发起语音对讲*/
    NET_DEV_USER_RR_REAL_PLAY,  /*远程预览*/
    NET_DEV_USER_RR_ALARMOUT,   /*远程请求报警上传、报警输出*/
    NET_DEV_USER_RR_VIDEOOUT,   /*远程控制，本地输出*/
    NET_DEV_USER_RR_RS232_TRNS, /*远程控制串口*/
    NET_DEV_USER_RR_CHANNEL,    /*远程管理模拟和IP camera */
    NET_DEV_USER_RR_POWEROFF,   /*远程关机/重启 */
}NET_DEV_USER_REMOTERIGHT;


typedef struct _NET_DEV_USER NET_DEV_USER;
struct _NET_DEV_USER
{
    CHAR UserName[NET_DEV_USERNAME_LEN];		//用户名
    CHAR PassWord[NET_DEV_PASSWORD_LEN];		//密码
};


typedef struct _NET_DEV_USER_INFO NET_DEV_USER_INFO;
struct _NET_DEV_USER_INFO
{
    CHAR UserName[NET_DEV_USERNAME_LEN];		//用户名
    CHAR PassWord[NET_DEV_PASSWORD_LEN];		//密码
    DWORD LocalRight;		//本地操作权限掩码 NET_DEV_USER_LOCALRIGHT
    DWORD RemoteRight;		//远程操作权限掩码 NET_DEV_USER_REMOTERIGHT
    DWORD Reserve;		//保留 按通道划分权限 ????
};


typedef struct _NET_DEV_USERSET NET_DEV_USERSET;
struct _NET_DEV_USERSET
{
    DWORD UserCount;
    NET_DEV_USER_INFO UserInfo[NET_DEV_MAX_USER_NUM];
};


typedef struct _NET_DEV_USER_MODIFY NET_DEV_USER_MODIFY;
struct _NET_DEV_USER_MODIFY
{
    CHAR OldUserName[NET_DEV_USERNAME_LEN];		// 旧用户名
    CHAR OldPassWord[NET_DEV_PASSWORD_LEN];		// 旧用户名
    CHAR NewUserName[NET_DEV_USERNAME_LEN];		// 新用户名
    CHAR NewPassWord[NET_DEV_PASSWORD_LEN];		// 新密码
    DWORD LocalRight;		// 本地操作权限掩码 ???
    DWORD RemoteRight;		// 远程操作权限掩码 ???
    DWORD Reserve;
};


//升级
//------------------------------------------

typedef struct _NET_DEV_UPGRADE NET_DEV_UPGRADE;
struct _NET_DEV_UPGRADE
{
    DWORD Percent;
    DWORD DataLen;
    DWORD FileLen;
    DWORD SessionId;
    NET_DEV_IPADDR IpAddr;
    BYTE Data[NET_DEV_UPGRADE_HDR_SIZE];
};


typedef struct _NET_DEV_UPGRADE_PROGRESS NET_DEV_UPGRADE_PROGRESS;
struct _NET_DEV_UPGRADE_PROGRESS
{
    DWORD Percent;
};


// NVR CHANNEL
//-----------------------------------------------

typedef enum _NET_DEV_CHANNEL_TYPE
{
    NET_DEV_CHANNEL_LOCAL,             //通道类型: 本地
    NET_DEV_CHANNEL_NET                //通道类型: 网络
}NET_DEV_CHANNEL_TYPE;


typedef struct _NET_DEV_REMOTE_CHANNEL NET_DEV_REMOTE_CHANNEL;
struct _NET_DEV_REMOTE_CHANNEL
{
    DWORD ChannNo;		//通道号（0-255）
    DWORD TransProtol;		//音视频传输协议 NET_DEV_TRANSPROTOL
};


typedef struct _NET_DEV_REMOTE_DEVICE NET_DEV_REMOTE_DEVICE;
struct _NET_DEV_REMOTE_DEVICE
{
    CHAR UserName[NET_DEV_USERNAME_LEN];		//用户名称
    CHAR PassWord[NET_DEV_PASSWORD_LEN];		//用户密码
    NET_DEV_IPADDR IpAddr;		//IP地址
    DWORD Port;		//端口
    DWORD ChannNum;		//总通道数
    BYTE DevType;		//设备类型  NET_DEV_TYPE
    BYTE ProtolType;		//设备协议类型 NET_DEV_PROTOL_TYPE
    BYTE FromNIC;		//NVR搜索设备时，标记此设备通过哪个网口接入; NET_DEV_NIC_IDX
    BYTE Res;		//保留
};


typedef struct _NET_DEV_REMOTE_RTSP NET_DEV_REMOTE_RTSP;
struct _NET_DEV_REMOTE_RTSP
{
    CHAR RtspUrl0[NET_DEV_MAX_URL_LEN];		//RTSP URL0
    CHAR RtspUrl1[NET_DEV_MAX_URL_LEN];		//RTSP URL1
};


typedef struct _NET_DEV_NVR_CHANNEL NET_DEV_NVR_CHANNEL;
struct _NET_DEV_NVR_CHANNEL
{
    DWORD No;		//通道号（0-255）
    DWORD Type;		//通道类型，NET_DEV_CHANNEL_TYPE
    DWORD Enable;		//通道使能（0: 禁止、1: 开启）
    CHAR Name[NET_DEV_CHANN_NAME_LEN];		//通道名称
    NET_DEV_REMOTE_CHANNEL RemoteChann;		//远程设备通道信息
    DWORD RemoteType;		//连接方式：0-RemoteDevice, 1-RemoteRtsp
    union {
        NET_DEV_REMOTE_DEVICE RemoteDevice;		//远程设备信息
        NET_DEV_REMOTE_RTSP RemoteRtsp;		//远程RTSP设备
    } Remote;
};


// 摄像机参数
//-----------------------------------------------

//日夜切换参数
//-------------------------------------------------------------------
typedef struct _NET_DEV_DAYNIGHT NET_DEV_DAYNIGHT;
struct _NET_DEV_DAYNIGHT
{
    BYTE DayNightFilterType;		//0-白天，1-夜晚，2-自动，3-定时，4-报警输入触发
    BYTE FilterLevel;		//自动切换时 0 低电平切换 1 高电平切换
    BYTE AlarmTrigState;		//报警输入触发 报警输入触发状态：0-白天，1-夜晚
    BYTE Res;
    NET_DEV_TIMESEG Segs[NET_DEV_MAX_SEG_NUM];		//定时切换时间段
};


typedef struct _NET_DEV_CAMERAPARAM NET_DEV_CAMERAPARAM;
struct _NET_DEV_CAMERAPARAM
{
    DWORD Mirror;		//镜像(图像左右调换) 1 镜像 0 不镜像
    DWORD Flip;		//翻转(图像上下调换) 1 翻转 0 不翻转
    DWORD ScenceMode;		//场景模式
    DWORD AntiFlicker;		//抗闪烁 0-关闭 1-50Hz 2-60 Hz
    DWORD WdrMode;		//宽动态模式 0自动   1 打开 2 关闭
    DWORD WdrStreng;		//0-255为宽动态强度
    DWORD DeNoiseLevel;		//降噪等级 0--关闭 1--低 2--中 3--高
    DWORD LdcStreng;		//畸变校正强度(0-255)：0 不启用    1-255 畸变校正强度值
    DWORD AntiFog;		//去雾：0 不启用    1 开启
    DWORD Rotate;		//旋转：0 无    1 旋转90度    2 旋转270度
    DWORD Shutter;		//电子快门 0-9  0：自动快门 1：1/25S， 2：1/50S， 3：1/100S， 4: 1/200S 5：1/500S， 6：1/1000S， 7：1/2000S， 8：1/5000S， 9：1/10000S
    DWORD AwbMode;		//白平衡模式 0 自动1 室内 2室外 3自动跟踪 4 手动
    DWORD AwbRed;		//白平衡红色
    DWORD AwbBlue;		//白平衡蓝色
    DWORD Blc;		//背光补偿0 关闭 1 打开
    DWORD Frezon;		//视频冻结0 关闭 1 打开
    DWORD AEMinTime;		//自动曝光最小曝光时间,自动快门时有效：0：不限,1：1/25S，2：1/50S，3：1/100S，4: 1/200S，5：1/500S，6：1/1000S，7：1/2000S，8：1/5000S，9：1/10000S
    DWORD AEMaxTime;		//自动曝光最大曝光时间,同AEMinTime,且应>=AEMinTime
    DWORD AGain;		//自动增益(0-255)
};


typedef struct _NET_DEV_CAMERACFG NET_DEV_CAMERACFG;
struct _NET_DEV_CAMERACFG
{
    DWORD CameraMode;		// 0: 用户自定义， 1：默认值
    NET_DEV_CAMERAPARAM CameraParam;		//用户自定义
};


typedef struct _NET_DEV_ROI NET_DEV_ROI;
struct _NET_DEV_ROI
{
    DWORD Enable;		//1:开启    0: 不开启
    DWORD AbsQp;		//0 相对质量 1 绝对质量
    DWORD QpVal;		//质量值 0-51
    NET_DEV_AREA Area;		//区域
};


//控制
//------------------------------------------------------------

typedef enum _NET_DEV_CONTROL_TYPE
{
    NET_DEV_SHUTDOWN_DEVICE         = 0x01,        //设备关机
    NET_DEV_RESTART_DEVICE          = 0x02,        //设备重启
    NET_DEV_RESTORE_DEFAULT         = 0x03,        //恢复出厂默认
    NET_DEV_DETECT_DEAL_PIX         = 0x04,        //坏点检测
    NET_DEV_DETECT_IRIS             = 0x05,        //光圈校正
    NET_DEV_FORCE_IFRAME            = 0x06,        //强制I帧
    NET_DEV_OPEN_IO                 = 0x07,        //打开IO
    NET_DEV_CLOSE_IO                = 0X08,        //关闭IO
}NET_DEV_CONTROL_TYPE;


typedef enum _NET_DEV_PIX_CAL_STATE
{
    NET_DEV_PIX_CAL_START   = 0x0,        //坏点检测开始
    NET_DEV_PIX_CAL_END     = 0x1,        //坏点检测结束(成功)
    NET_DEV_PIX_CAL_FAIL    = 0x2,        //坏点检测失败
    NET_DEV_PIX_CAL_TIMEOUT = 0x3,        //坏点检测超时
}NET_DEV_PIX_CAL_STATE;


typedef enum _NET_DEV_IRIS_CAL_STATE
{
    NET_DEV_IRIS_CAL_START  = 0x0,      //光圈校正开始
    NET_DEV_IRIS_CAL_END    = 0x1,      //光圈校正结束(成功)
    NET_DEV_IRIS_CAL_FAIL   = 0x2,      //光圈校正失败
    NET_DEV_IRIS_CAL_TIMEOUT= 0x3,      //光圈校正超时
}NET_DEV_IRIS_CAL_STATE;


typedef struct _NET_DEV_CONTROL NET_DEV_CONTROL;
struct _NET_DEV_CONTROL
{
    DWORD Command;		//控制命令类型    //NET_DEV_CONTROL_TYPE
    DWORD Param;		//控制命令参数    //???
};


//状态
//------------------------------------------------------------

typedef enum _NET_DEV_STATE_IDX
{
    NET_DEV_STATE_NETWORK   = 0,    //NET_DEV_NETSTATES
    NET_DEV_STATE_UPNP      = 1,    //NET_DEV_UPNP_STATE
    NET_DEV_STATE_DISK      = 2,    //NET_DEV_DISKSTATES
    NET_DEV_STATE_PIX_CAL   = 3,    //NET_DEV_PIX_CAL_STATE
    NET_DEV_STATE_IRIS_CAL  = 4,    //NET_DEV_IRIS_CAL_STATE
    NET_DEV_STATE_CHANNNEL  = 5,    //NET_DEV_CHANNSTATE
    NET_DEV_STATE_WORKS     = 6,    //NET_DEV_WORKSTATE
}NET_DEV_STATE_IDX;

typedef struct _NET_DEV_DISKSTATE NET_DEV_DISKSTATE;
struct _NET_DEV_DISKSTATE
{
    DWORD Volume;		//硬盘容量  M
    DWORD FreeSpace;		//硬盘剩余空间 M
    DWORD State;		//硬盘状态 0－正在使用；1－未挂载；2－已挂载未使用, 3 -休眠
};


typedef struct _NET_DEV_DISKSTATES NET_DEV_DISKSTATES;
struct _NET_DEV_DISKSTATES
{
    NET_DEV_DISKSTATE States[NET_DEV_MAX_NET_DISK];		//硬盘状态
};


typedef struct _NET_DEV_CHANNSTATE NET_DEV_CHANNSTATE;
struct _NET_DEV_CHANNSTATE
{
    BYTE RecordState;		//通道是否在录像：0－不录像；1－录像
    BYTE SignalState;		//连接的信号状态：0－正常，  1－信号丢失
    BYTE HardwareState;		//通道硬件状态：  0－正常，  1－异常（例如DSP异常）
    BYTE Res;
    DWORD BitRate[NET_DEV_MAX_STREAM_NUM];		//实际码率
};


typedef struct _NET_DEV_CHANNSTATES NET_DEV_CHANNSTATES;
struct _NET_DEV_CHANNSTATES
{
    NET_DEV_CHANNSTATE States[NET_DEV_MAX_CHANN_NUM];		//通道状态
};


typedef struct _NET_DEV_WORKSTATE NET_DEV_WORKSTATE;
struct _NET_DEV_WORKSTATE
{
    DWORD DeviceState;		// 设备的状态：0－正常；1－CPU占用率太高，超过85%；
    NET_DEV_DISKSTATES DiskState;		// 磁盘状态
    NET_DEV_NETSTATES NetState;		// 网络状态
    NET_DEV_CHANNSTATES ChannState;		// 通道状态
    BYTE AlarmInState[NET_DEV_MAX_ALARM_IN_NUM];		//报警输入口的状态：0-没有报警；1-有报警
    BYTE AlarmOutState[NET_DEV_MAX_ALARM_OUT_NUM];		//报警输出口的状态：0-没有输出，1-有报警输出
    DWORD LocalDisplayState;		//本地显示状态：0-正常，1-不正常
    BYTE AudioChannState[NET_DEV_MAX_CHANN_NUM];		//表示语音通道的状态：0-未使用，1-使用中，0xff无效
};


//扩展告警
//------------------------------------------------------------
typedef enum _NET_DEV_EXT_TYPE
{
    NET_DEV_EXT_POSITION   = 0,    //设备位置信息
}NET_DEV_EXT_TYPE;

typedef struct _NET_DEV_EXT_POSITIONINFO NET_DEV_EXT_POSITIONINFO;
struct _NET_DEV_EXT_POSITIONINFO
{
    DWORD UTCTime;		//上报时的UTC时(网络字节序)
    CHAR X[MAX_POSITIONVALUE_LEN];		//X
    CHAR Y[MAX_POSITIONVALUE_LEN];		//Y
    CHAR Z[MAX_POSITIONVALUE_LEN];		//Z
};


//日志
//----------------------------------------------------------------

//日志操作类型
typedef enum _NET_DEV_LOG_OPERATE
{
    NET_DEV_LOG_OP_EMPTY = 1,     // 清空(全部)
    NET_DEV_LOG_OP_DEL,           // 删除(按时间段)
}NET_DEV_LOG_OPERATE;


//日志类型定义（ 最大支持32种）
typedef enum _NET_DEV_LOG_MAJOR_TYPE
{
    NET_DEV_LOG_MAJOR_SYSTEM     = 0x0001,      //系统日志
    NET_DEV_LOG_MAJOR_ALARM      = 0x0002,      //告警日志
    NET_DEV_LOG_MAJOR_OPERATE    = 0x0003,      //操作日志
    NET_DEV_LOG_MAJOR_NETWORK    = 0x0004,      //网络日志
    NET_DEV_LOG_MAJOR_PARAM      = 0x0005,      //参数日志
    NET_DEV_LOG_MAJOR_EXCEPTION  = 0x0006,      //异常日志
    NET_DEV_LOG_MAJOR_ALL        = 0xffffffff   //所有日志
}NET_DEV_LOG_MAJOR_TYPE;


//系统日志子类型
typedef enum _NET_DEV_LOG_MINOR_SYS_TYPE
{
    NET_DEV_LOG_MINOR_SYS_STARTUP = 0x0001,   //开机
    NET_DEV_LOG_MINOR_SYS_SHUTDOWN,           //关机
    NET_DEV_LOG_MINOR_SYS_REBOOT,             //重启
    NET_DEV_LOG_MINOR_SYS_ALL     = 0xffff    //所有系统日志
}NET_DEV_LOG_MINOR_SYS_TYPE;


//告警日志子类型
typedef enum _NET_DEV_LOG_MINOR_ALARM_TYPE
{
    NET_DEV_LOG_MINOR_ALARM_MD_START = 0x0001,//移动侦测告警发生
    NET_DEV_LOG_MINOR_ALARM_MD_STOP,          //移动侦测告警停止
    NET_DEV_LOG_MINOR_ALARM_VL_START,         //视频丢失告警发生
    NET_DEV_LOG_MINOR_ALARM_VL_STOP,          //视频丢失告警停止
    NET_DEV_LOG_MINOR_ALARM_OD_START,         //视频遮挡报警发生
    NET_DEV_LOG_MINOR_ALARM_OD_STOP,          //视频遮挡报警停止
    NET_DEV_LOG_MINOR_ALARM_DI_START ,        //输入告警发生
    NET_DEV_LOG_MINOR_ALARM_DI_STOP,          //输入告警停止
    NET_DEV_LOG_MINOR_ALARM_PIR_START,    //PIR告警发生
    NET_DEV_LOG_MINOR_ALARM_PIR_STOP,         //PIR告警停止
    NET_DEV_LOG_MINOR_ALARM_ALL      = 0xffff //所有告警日志
}NET_DEV_LOG_MINOR_ALARM_TYPE;


//操作日志子类型

typedef enum _NET_DEV_LOG_MINOR_OPERATE_TYPE
{
    NET_DEV_LOG_MINOR_OPERATE_LOGIN = 0x0001,    // 登陆
    NET_DEV_LOG_MINOR_OPERATE_LOGOUT,            // 注销
    NET_DEV_LOG_MINOR_OPERATE_USER_ADD,          // 用户管理-增加
    NET_DEV_LOG_MINOR_OPERATE_USER_DEL,          // 用户管理-删除
    NET_DEV_LOG_MINOR_OPERATE_USER_MODI,         // 用户管理-修改
    NET_DEV_LOG_MINOR_OPERATE_SETTIME,           // 设置系统时间
    NET_DEV_LOG_MINOR_OPERATE_FORMAT_DISK,       // 格式化硬盘
    NET_DEV_LOG_MINOR_OPERATE_DEFAULT,           // 恢复默认
    NET_DEV_LOG_MINOR_OPERATE_UPGRADE,           // 升级
    NET_DEV_LOG_MINOR_OPERATE_PLAYBACK,          // 回放
    NET_DEV_LOG_MINOR_OPERATE_PTZ,               // 云台控制
    NET_DEV_LOG_MINOR_OPERATE_BACKUP,            // 备份
    NET_DEV_LOG_MINOR_OPERATE_RECORD_START,      // 启动录像
    NET_DEV_LOG_MINOR_OPERATE_RECORD_STOP,       // 停止录像
    NET_DEV_LOG_MINOR_OPERATE_CLR_ALARM,         // 清除报警
    NET_DEV_LOG_MINOR_OPERATE_TALKBACK_START,    // 对讲开始
    NET_DEV_LOG_MINOR_OPERATE_TALKBACK_STOP,     // 对讲结束
    NET_DEV_LOG_MINOR_OPERATE_LOG_DEL,           // 删除日志
    NET_DEV_LOG_MINOR_OPERATE_LOG_EMPTY,         // 清空日志
    NET_DEV_LOG_MINOR_OPERATE_LOG_BACKUP,         // 备份日志
    NET_DEV_LOG_MINOR_OPERATE_MANUAL_RECORD_START,// 手动启动录像
    NET_DEV_LOG_MINOR_OPERATE_MANUAL_RECORD_STOP, // 手动停止录像
    NET_DEV_LOG_MINOR_OPERATE_FORMAT_DISK_U,      // 格式化U盘
    NET_DEV_LOG_MINOR_OPERATE_CTRL_ALARM_OUT,     // 手动控制报警输出
    NET_DEV_LOG_MINOR_OPERATE_DETECT_DEAL_PIX,    // 坏点检测
    NET_DEV_LOG_MINOR_OPERATE_DETECT_IRIS,        // 光圈校正
    NET_DEV_LOG_MINOR_OPERATE_MINOR_ALL  = 0xffff // 所有操作日志
}NET_DEV_LOG_MINOR_OPERATE_TYPE;


//参数日志子类型

typedef enum _NET_DEV_LOG_MINOR_PARAM_TYPE
{
    NET_DEV_LOG_MINOR_PARAM_NETWORK = 0x0001,       // 网络参数配置
    NET_DEV_LOG_MINOR_PARAM_UART,                   // 串口参数配置
    NET_DEV_LOG_MINOR_PARAM_PTZ,                    // 云台参数配置
    NET_DEV_LOG_MINOR_PARAM_CHAN,                   // 通道参数配置
    NET_DEV_LOG_MINOR_PARAM_VENC,                   // 编码参数配置
    NET_DEV_LOG_MINOR_PARAM_TV,                     // TV参数配置
    NET_DEV_LOG_MINOR_PARAM_PIC_ATTR,               // 图像属性配置
    NET_DEV_LOG_MINOR_PARAM_RECORD,                 // 录像参数配置
    NET_DEV_LOG_MINOR_PARAM_MOTION,                 // 移动侦测配置
    NET_DEV_LOG_MINOR_PARAM_VIDEOLOST,              // 视频丢失配置
    NET_DEV_LOG_MINOR_PARAM_ALARMIN,                // 输入告警配置
    NET_DEV_LOG_MINOR_PARAM_NETFAULT,               // 网络故障检测配置
    NET_DEV_LOG_MINOR_PARAM_POLL,                   // 轮巡配置
    NET_DEV_LOG_MINOR_PARAM_PREVIEW,                // 预览配置
    NET_DEV_LOG_MINOR_PARAM_OVERLAY,                // 遮挡区域配置
    NET_DEV_LOG_MINOR_PARAM_OSD,                    // OSD配置
    NET_DEV_LOG_MINOR_PARAM_BASE,                   // 设备基本参数配置
    NET_DEV_LOG_MINOR_PARAM_OD,                     // 视频遮挡配置
    NET_DEV_LOG_MINOR_PARAM_ROI,                    // 感兴趣区域配置
    NET_DEV_LOG_MINOR_PARAM_FTP,                    // FTP 配置
    NET_DEV_LOG_MINOR_PARAM_UPNP,                   // UPNP 配置
    NET_DEV_LOG_MINOR_PARAM_SMTP,                   // SMTP 配置
    NET_DEV_LOG_MINOR_PARAM_NTP,                    // NTP  配置
    NET_DEV_LOG_MINOR_PARAM_DDNS,                   // DDNS 配置
    NET_DEV_LOG_MINOR_PARAM_ALARMUP,                // 报警上传配置
    NET_DEV_LOG_MINOR_PARAM_PIR,                    // PIR 配置
    NET_DEV_LOG_MINOR_PRARM_ALL    = 0xffff         // 所有参数日志
}NET_DEV_LOG_MINOR_PARAM_TYPE;


//异常日志子类型

typedef enum _NET_DEV_LOG_MINOR_EXCEPTION_TYPE
{
    NET_DEV_LOG_MINOR_EXCEPTION_HD_NO   = 0x0001,       //无硬盘
    NET_DEV_LOG_MINOR_EXCEPTION_HD_FULL,                //硬盘满
    NET_DEV_LOG_MINOR_EXCEPTION_HD_ERROR,               //硬盘错误
    NET_DEV_LOG_MINOR_EXCEPTION_HD_NORMAL,              //磁盘正常
    NET_DEV_LOG_MINOR_EXCEPTION_NET_BROKEN,             //网络断开
    NET_DEV_LOG_MINOR_EXCEPTION_NET_NORMAL,             //网络正常
    NET_DEV_LOG_MINOR_EXCEPTION_IP_CONFLICT,            //IP冲突
    NET_DEV_LOG_MINOR_EXCEPTION_IP_NORMAL,              //IP正常
    NET_DEV_LOG_MINOR_EXCEPTION_ALL     = 0xffff        //所有异常日志
}NET_DEV_LOG_MINOR_EXCEPTION_TYPE;


//日志记录定义
typedef struct _NET_DEV_LOG_ITEM NET_DEV_LOG_ITEM;
struct _NET_DEV_LOG_ITEM
{
    DWORD Time;
    DWORD MajorType;		// 掩码 NET_DEV_LOG_MAJOR_TYPE
    DWORD MinorType;		// MINOR_TYPE
    DWORD Args;		// 如果 MINOR_TYPE 和通道号相关，args表示通道号;如果 MINOR_TYPE 和IO告警相关，args表示IO告警输入号)
    DWORD Res;
    CHAR UserName[NET_DEV_USERNAME_LEN];		//操作用户名
    NET_DEV_IPADDR IpAddr;		//操作IP地址
};


//日志查询条件
typedef struct _NET_DEV_LOG_COND NET_DEV_LOG_COND;
struct _NET_DEV_LOG_COND
{
    DWORD BeginTime;		//开始时间
    DWORD EndTime;		//结束时间
    DWORD MajorType;		//掩码  NET_DEV_LOG_MAJOR_TYPE
    CHAR UserName[NET_DEV_USERNAME_LEN];		//操作用户 不指定为空
    NET_DEV_IPADDR IpAddr;		//用户IP  不指定为0
};


//日志查询请求
typedef struct _NET_DEV_LOG_QUERY_REQ NET_DEV_LOG_QUERY_REQ;
struct _NET_DEV_LOG_QUERY_REQ
{
    NET_DEV_LOG_COND Cond;
    DWORD BeginNo;
    DWORD ReqCount;
    DWORD SessionId;
};


//日志查询回应
typedef struct _NET_DEV_LOG_QUERY_RSP NET_DEV_LOG_QUERY_RSP;
struct _NET_DEV_LOG_QUERY_RSP
{
    DWORD SessionId;
    DWORD TotalCount;
    DWORD RspCount;
    NET_DEV_LOG_ITEM Items[NET_DEV_MAX_LOG_PAGE_SIZE];
};


//图片查询条件
typedef struct _NET_DEV_PIC_COND NET_DEV_PIC_COND;
struct _NET_DEV_PIC_COND
{
    DWORD BeginTime;		//开始时间
    DWORD EndTime;		//结束时间
    DWORD PictureType;		//掩码 NET_DEV_RECORD_TYPE
};


//图片查询请求
typedef struct _NET_DEV_PIC_QUERY_REQ NET_DEV_PIC_QUERY_REQ;
struct _NET_DEV_PIC_QUERY_REQ
{
    NET_DEV_PIC_COND Cond;
    DWORD BeginNo;
    DWORD ReqCount;
    DWORD SessionId;
};


//图片记录定义
typedef struct _NET_DEV_PIC_ITEM NET_DEV_PIC_ITEM;
struct _NET_DEV_PIC_ITEM
{
    DWORD Time;		//图片生成时间
    DWORD PictureType;		//掩码 NET_DEV_RECORD_TYPE
    DWORD PictureSize;		//图片大小
    CHAR Property[NET_DEV_REC_PROPERTY_SIZE];
};


//图片查询回应
typedef struct _NET_DEV_PIC_QUERY_RSP NET_DEV_PIC_QUERY_RSP;
struct _NET_DEV_PIC_QUERY_RSP
{
    DWORD SessionId;
    DWORD TotalCount;
    DWORD RspCount;
    NET_DEV_PIC_ITEM Items[NET_DEV_MAX_PIC_PAGE_SIZE];
};


//------------------- IVS 结构定义 -------------------
//--------------------IVS 宏定义 ---------------------
#define NET_DEV_MAX_AVD_RULE_NUM              5   //AVD最大策略个数
#define NET_DEV_MAX_IVS_NAME_LEN              32  //智能分析规则名称长度
#define NET_DEV_MAX_POLYGON_POINT_NUM         8   //多边形最大顶点个数

#define NET_DEV_MAX_PEAOSC_RULE_NUM           8   //PEAOSC最大策略个数
#define NET_DEV_MAX_PEAOSC_TARGET_TRACK_NUM   40  //PEAOSC目标最大轨迹点个数
#define NET_DEV_MAX_PEAOSC_TARGET_NUM         64  //PEAOSC目标个数
#define NET_DEV_MAX_PEAOSC_EVENT_NUM          128 //PEAOSC事件个数

#define NET_DEV_MAX_CPC_RULE_NUM              1   //CPC策略最大个数
#define NET_DEV_MAX_CPC_RULE_POINT_NUM        4   //CPC策略最大点数
#define NET_DEV_MAX_CPC_TARGET_POINT_NUM      1   //CPC目标最大点数
#define NET_DEV_MAX_CPC_TARGET_NUM            15  //CPC目标最大个数
#define NET_DEV_MAX_CPC_EVENT_NUM             1   //CPC最大事件个数


//--------------------AVD CFG ---------------------
typedef enum _NET_DEV_AVD_TYPE
{
  NET_DEV_AVD_TYPE_BRIGHT   = 0,    // 亮度异常检测
  NET_DEV_AVD_TYPE_CLARITY  = 1,    // 清晰度异常检测
  NET_DEV_AVD_TYPE_NOISE    = 2,    // 噪声异常检测
  NET_DEV_AVD_TYPE_COLOR    = 3,    // 偏色检测
  NET_DEV_AVD_TYPE_SCENE    = 4     // 场景变换检测
}NET_DEV_AVD_TYPE;

typedef struct _NET_DEV_AVD_RULE NET_DEV_AVD_RULE;
struct _NET_DEV_AVD_RULE
{
    BYTE Enable;		// 0 不检测 1 检测
    BYTE Level;		// 灵敏度0-4 值越大灵敏度越高
    BYTE AlarmTimes;		// 异常报警时间，（0-20秒），数值越大需要的报警时间越长( 预留参数，暂不支持)
    BYTE Res[29];
};


typedef struct _NET_DEV_AVD_CFG NET_DEV_AVD_CFG;
struct _NET_DEV_AVD_CFG
{
    BYTE Enable;
    BYTE Res[3];
    NET_DEV_WEEK Week;
    NET_DEV_AVD_RULE Rules[NET_DEV_MAX_AVD_RULE_NUM];		//视频诊断规则，见 NET_DEV_AVD_TYPE
};


//--------------------PEA/OSC CFG ---------------------
// 场景类型
typedef enum _NET_DEV_SCENE_TYPE
{
  NET_DEV_SCENE_TYPE_INDOOR  = 0,     // 室内场景
  NET_DEV_SCENE_TYPE_OUTDOOR = 1      // 室外场景
}NET_DEV_SCENE_TYPE;

//穿越方向
typedef enum _NET_DEV_CROSS_TYPE
{
  NET_DEV_CROSS_TYPE_BOTH = 0,    //双向
  NET_DEV_CROSS_TYPE_L2R  = 1,    //垂直线为从左到右，水平线为从上到下
  NET_DEV_CROSS_TYPE_R2L  = 2     //垂直线为从右到左, 水平线为从下到上
}NET_DEV_CROSS_TYPE;

// 规则类型定义:
typedef enum _NET_DEV_PEAOSC_RULE_TYPE
{
  NET_DEV_PEAOSC_RULE_TYPE_TRIPWIRE      = 0,       //单绊线-->   NET_DEV_TRIPWIRE_RULE
  NET_DEV_PEAOSC_RULE_TYPE_MUTITRIPWIRE  = 1,       //双绊线,暂不支持
  NET_DEV_PEAOSC_RULE_TYPE_PERIMETER     = 2,       //警戒区-->   NET_DEV_PERIMETER_RULE
  NET_DEV_PEAOSC_RULE_TYPE_LOITER        = 3,       //徘徊,暂不支持
  NET_DEV_PEAOSC_RULE_TYPE_LEFT          = 4,       //物品遗留检测-->NET_DEV_LEFT_RULE
  NET_DEV_PEAOSC_RULE_TYPE_TAKE          = 5        //物品丢失检测--> NET_DEV_TAKE_RULE
}NET_DEV_PEAOSC_RULE_TYPE;

// 事件类型定义
typedef enum _NET_DEV_PEAOSC_EVENT_TYPE
{
  NET_DEV_PEAOSC_EVENT_TYPE_UNKNOWN       = 0,    //未知事件
  NET_DEV_PEAOSC_EVENT_TYPE_TRIPWIRE      = 1,    //单绊线
  NET_DEV_PEAOSC_EVENT_TYPE_MUTITRIPWIRE  = 2,    //双绊线
  NET_DEV_PEAOSC_EVENT_TYPE_PERIMETER     = 3,    //警戒区报警
  NET_DEV_PEAOSC_EVENT_TYPE_LOITER        = 4,    //徘徊
  NET_DEV_PEAOSC_EVENT_TYPE_LEFT          = 5,    //物品遗留
  NET_DEV_PEAOSC_EVENT_TYPE_TAKE          = 6     //物品丢失
}NET_DEV_PEAOSC_EVENT_TYPE;

typedef enum _NET_DEV_PEAOSC_TARGET_FILTER_TYPE
{
  NET_DEV_PEAOSC_TARGET_FILTER_TYPE_PEOPLE    = 0,// 人
  NET_DEV_PEAOSC_TARGET_FILTER_TYPE_CARE      = 1 // 汽车
}NET_DEV_PEAOSC_TARGET_FILTER_TYPE;

// 点
typedef struct _NET_DEV_POINT NET_DEV_POINT;
struct _NET_DEV_POINT
{
    WORD X;
    WORD Y;
};


// 线
typedef struct _NET_DEV_LINE NET_DEV_LINE;
struct _NET_DEV_LINE
{
    NET_DEV_POINT Start;
    NET_DEV_POINT End;
};


// 多边形
typedef struct _NET_DEV_POLYGON NET_DEV_POLYGON;
struct _NET_DEV_POLYGON
{
    DWORD Count;		//多边形顶点数
    NET_DEV_POINT Points[NET_DEV_MAX_POLYGON_POINT_NUM];		//多边形点坐标
};


//单绊线参数
typedef struct _NET_DEV_TRIPWIRE_RULE NET_DEV_TRIPWIRE_RULE;
struct _NET_DEV_TRIPWIRE_RULE
{
    NET_DEV_LINE Line;
    DWORD Cross;		// NET_DEV_CROSS_TYPE
};


//周界防范
typedef struct _NET_DEV_PERIMETER_RULE NET_DEV_PERIMETER_RULE;
struct _NET_DEV_PERIMETER_RULE
{
    NET_DEV_POLYGON Polygon;		//多边形区域
    DWORD Mode;		//周界模式 0 入侵 1 进入 2 离开
};


// 物品遗留
typedef struct _NET_DEV_LEFT_RULE NET_DEV_LEFT_RULE;
struct _NET_DEV_LEFT_RULE
{
    NET_DEV_POLYGON Polygon;		//多边形区域
};


// 物品丢失
typedef struct _NET_DEV_TAKE_RULE NET_DEV_TAKE_RULE;
struct _NET_DEV_TAKE_RULE
{
    NET_DEV_POLYGON Polygon;		//多边形区域
};


// 规则参数定义
typedef struct _NET_DEV_PEAOSC_RULE_PARAM NET_DEV_PEAOSC_RULE_PARAM;
struct _NET_DEV_PEAOSC_RULE_PARAM
{
    DWORD RuleType;		//NET_DEV_PEAOSC_RULE_TYPE
    union {
        NET_DEV_TRIPWIRE_RULE TripWire;
        NET_DEV_PERIMETER_RULE Perimeter;
        NET_DEV_LEFT_RULE Left;
        NET_DEV_TAKE_RULE Take;
    } Rule;
};


//目标过滤器参数结构体( 暂不支持，预留)
typedef struct _NET_DEV_PEAOSC_TARGET_FILTER NET_DEV_PEAOSC_TARGET_FILTER;
struct _NET_DEV_PEAOSC_TARGET_FILTER
{
    BYTE Enable;		// 使能目标过滤
    BYTE Res[3];		// 保留
    DWORD Fliter;		// 目前，bit0: 人, bit1: 车
};


//时间过滤参数( 目前仅物品遗留、物品丢失规则支持最小时间限制，
//最大时间限制均不支持, 定义仅作预留使用)
typedef struct _NET_DEV_PEAOSC_TIME_FILTER NET_DEV_PEAOSC_TIME_FILTER;
struct _NET_DEV_PEAOSC_TIME_FILTER
{
    BYTE Enable;		//使能过滤
    BYTE MinTime;		//最小时间
    BYTE MaxTime;		//最大时间
    BYTE Res;
};


//尺寸大小限制参数
typedef struct _NET_DEV_PEAOSC_SIZE_FILTER NET_DEV_PEAOSC_SIZE_FILTER;
struct _NET_DEV_PEAOSC_SIZE_FILTER
{
    DWORD MinSize;		//最小尺寸
    DWORD MaxSize;		//最大尺寸
};


//尺寸宽高限制参数
typedef struct _NET_DEV_PEAOSC_WH_FILTER NET_DEV_PEAOSC_WH_FILTER;
struct _NET_DEV_PEAOSC_WH_FILTER
{
    WORD MinWidth;		//最小宽度
    WORD MaxWidth;		//最大宽度
    WORD MinHeight;		//最小高度
    WORD MaxHeight;		//最大高度
};


//尺寸过滤器参数结构体( 目前仅物品遗留、物品丢失规则支持u8FilterType = 0 的
//限制参数, 其他定义仅作预留使用)
typedef struct _NET_DEV_PEAOSC_FILTER_RULE NET_DEV_PEAOSC_FILTER_RULE;
struct _NET_DEV_PEAOSC_FILTER_RULE
{
    BYTE Enable;		// 使能尺寸过滤
    BYTE FilterType;		// 限制类型，0: 限制总体尺寸，1: 限制宽高
    BYTE Res[2];
    union {
        NET_DEV_PEAOSC_SIZE_FILTER FilterSize;
        NET_DEV_PEAOSC_WH_FILTER FilterWH;
    } Filter;
};


// 规则限制定义
typedef struct _NET_DEV_PEAOSC_RULE_LIMIT NET_DEV_PEAOSC_RULE_LIMIT;
struct _NET_DEV_PEAOSC_RULE_LIMIT
{
    NET_DEV_PEAOSC_TARGET_FILTER TargetFilter;
    NET_DEV_PEAOSC_TIME_FILTER TimeFilter;
    NET_DEV_PEAOSC_FILTER_RULE SizeFilter;
};


// 规则异常处理定义
typedef struct _NET_DEV_PEAOSC_RULE_EXCEPTION_HANDLE NET_DEV_PEAOSC_RULE_EXCEPTION_HANDLE;
struct _NET_DEV_PEAOSC_RULE_EXCEPTION_HANDLE
{
    BYTE SubmitAlarm;		// 客户端/平台是否上报告警NET_DEV_ALARM_TYPE_PEA_OSC
    BYTE Res[3];
    NET_DEV_ALARM_JOINT Joint;		// 事件联动
};


// 检测规则
typedef struct _NET_DEV_PEAOSC_RULE NET_DEV_PEAOSC_RULE;
struct _NET_DEV_PEAOSC_RULE
{
    BYTE Enable;		//使能规则
    BYTE Level;		//警戒级别0 ~3 无警报 低级警报 中级警报 高级警报，预留定义
    BYTE Res[2];		// 预留
    CHAR Name[NET_DEV_MAX_IVS_NAME_LEN];		//规则名称
    NET_DEV_PEAOSC_RULE_PARAM Param;		// 规则参数定义
    NET_DEV_PEAOSC_RULE_LIMIT Limit;		// 规则限制定义
};


// PEA/OSC 检测配置项
typedef struct _NET_DEV_PEAOSC_CFG NET_DEV_PEAOSC_CFG;
struct _NET_DEV_PEAOSC_CFG
{
    BYTE Enable;		// pea,osc 使能
    BYTE Scene;		// 场景类型NET_DEV_SCENE_TYPE ，预留定义，目前设置无效果
    BYTE Level;		// 目标输出灵敏度级别0~2 高中低，预留定义，目前设置无效果
    BYTE Res;		// 预留
    NET_DEV_WEEK Week;
    WORD MaxWidth;		//规则定义参考宽度
    WORD MaxHeight;		//规则定义参考高度
    DWORD RuleCount;
    NET_DEV_PEAOSC_RULE Rules[NET_DEV_MAX_PEAOSC_RULE_NUM];		// 检测规则
};


//-------------------CPC CFG ---------------------
typedef struct _NET_DEV_CPC_RULE NET_DEV_CPC_RULE;
struct _NET_DEV_CPC_RULE
{
    BYTE Enable;		// 是否启用规则
    BYTE Res[3];		// reserved
    DWORD PointCount;
    NET_DEV_POINT Points[NET_DEV_MAX_CPC_RULE_POINT_NUM];
};


typedef struct _NET_DEV_CPC_RULE_CFG NET_DEV_CPC_RULE_CFG;
struct _NET_DEV_CPC_RULE_CFG
{
    BYTE Level;		// Algorithm Sensitivity
    BYTE CountMax;		// Maximum targets couted in one frame
    BYTE Res[2];		// reserved
    WORD MinSize;		// Minimum diamater of target heads
    WORD MaxSize;		// Maximum diamater of target heads
    DWORD RuleCount;
    NET_DEV_CPC_RULE Rules[NET_DEV_MAX_CPC_RULE_NUM];
};


typedef struct _NET_DEV_CPC_CFG NET_DEV_CPC_CFG;
struct _NET_DEV_CPC_CFG
{
    BYTE Enable;
    BYTE Res[3];
    NET_DEV_WEEK Week;
    WORD MaxWidth;		//规则定义参考宽度
    WORD MaxHeight;		//规则定义参考高度
    NET_DEV_CPC_RULE_CFG Rule;
};


//------------------- IVS 帧数据结构 ---------------------

//------------------- PEA/OSC 帧结构 ---------------------

// 分析结果规则参数定义
typedef struct _NET_DEV_PEAOSC_RESULT_RULE
{
    BYTE Index;     // 实际规则号( 0 ~ NET_DEV_MAX_PEAOSC_RULE_NUM - 1)
    BYTE Res[3];
    NET_DEV_PEAOSC_RULE_PARAM Param;
}NET_DEV_PEAOSC_RESULT_RULE;

// 事件信息定义
typedef struct _NET_DEV_PEAOSC_EVENT
{
    DWORD Type;       //事件类型  NET_DEV_PEAOSC_EVENT_TYPE
    DWORD Id;         //事件标识
    BYTE Index;       //事件触发的规则编号, 值为NET_DEV_PEAOSC_RESULT_RULE.Index, 即参数配置里的第几条规则
    BYTE Level;       //事件警报级别
    BYTE Res[2];
    DWORD TargetId;   //事件触发目标ID
    NET_DEV_RECT Rect;//事件触发目标所在区域
}NET_DEV_PEAOSC_EVENT;

// 目标信息定义
typedef struct _NET_DEV_PEAOSC_TARGET
{
    DWORD Size;           // 该目标信息总长度，包括Size本身
    DWORD Id;             // 目标ID
    DWORD Event;          //目标触发的事件类型 NET_DEV_PEAOSC_EVENT_TYPE
    BYTE Index;          //目标 触发的规则编号, 值为NET_DEV_PEAOSC_RESULT_RULE.Index, 即参数配置里的第几条规则,未触发规则为0xff
    BYTE Type;           // 目标类型人，车等 0XFF: 所有类型/未知类型,1 人2 车
    BYTE Track;          // 是否有轨迹信息
    BYTE TrackNum;       // 轨迹点个数, Track为1时有效
    BYTE Res[4];         // 预留
    NET_DEV_RECT Rect;    // 目标所在区域
    WORD MoveSpeed;      // 移动速率（coord /s）-1 未知
    WORD MoveDirection;  // 移动方向（0~359度）-1 未知
    NET_DEV_POINT Points[NET_DEV_MAX_PEAOSC_TARGET_TRACK_NUM]; // 实际个数可变，Track为1时有效，由TrackNnum值指定
}NET_DEV_PEAOSC_TARGET;


//-------------------CPC 帧结构 ---------------------


typedef struct _NET_DEV_CPC_RESULT_RULE
{
    BYTE Type;    //reserved
    BYTE Index;   //reserved
    BYTE Res[2];
    NET_DEV_CPC_RULE Rule;
}NET_DEV_CPC_RESULT_RULE;

//Reserved
typedef struct _NET_DEV_CPC_EVENT
{
    BYTE Res[32];
}NET_DEV_CPC_EVENT;

typedef enum _NET_DEV_CPC_CROSS_TYPE
{
  NET_DEV_CPC_CROSS_TYPE_ENTER = 0,   // 进入
  NET_DEV_CPC_CROSS_TYPE_LEAVE = 1    // 离开
}NET_DEV_CPC_CROSS_TYPE;


typedef struct _NET_DEV_CPC_TARGET
{
    DWORD Size;             //
    DWORD TargetSize;       // 目标（人）头（方形框）尺寸, 介于最小最大之间
    BYTE Cross;               // NET_DEV_CPC_CROSS_TYPE
    BYTE PointNum;            //
    BYTE Res[2];              // res
    NET_DEV_POINT Points[NET_DEV_MAX_CPC_TARGET_POINT_NUM]; // target's center point
}NET_DEV_CPC_TARGET;

//------------------- IVS END. ---------------------

typedef enum _NET_DEV_VO_INTF_SYNC
{
    NET_DEV_VO_INTF_1024x768_60   = 0x00,
    NET_DEV_VO_INTF_720P_50       = 0x01,
    NET_DEV_VO_INTF_1280x1024_60  = 0x02,
    NET_DEV_VO_INTF_1080P_50      = 0x03,
    NET_DEV_VO_INTF_720P_60       = 0x04,
    NET_DEV_VO_INTF_1080P_60      = 0x05,
    NET_DEV_VO_INTF_1440x900_60   = 0x06,
    NET_DEV_VO_INTF_1366x768_60   = 0x07,
    NET_DEV_VO_INTF_1280x800_60   = 0x08,
    NET_DEV_VO_INTF_800x600_60    = 0x09,
    NET_DEV_VO_INTF_BUTT
}NET_DEV_VO_INTF_SYNC;

typedef struct _NET_DEV_VO_CFG NET_DEV_VO_CFG;
struct _NET_DEV_VO_CFG
{
    DWORD IntfSync;		//NET_DEV_VO_INTF_SYNC
};


//===================================================================
//参数查询时使用, 参数设置时不需要(已定义到对应的结构中)
//---------------------------------------------------
typedef struct _NET_DEV_PARAM_QUERY NET_DEV_PARAM_QUERY;
struct _NET_DEV_PARAM_QUERY
{
    DWORD ParamId;		//NET_DEV_PARAM_ID
    union {
        DWORD CruiseNo;		//巡航号 当  ParamId == NET_DEV_PARAM_CRUISEWAY
        DWORD SerialNo;		//串口号 当  ParamId == NET_DEV_PARAM_SERIAL_CFG
        DWORD AlarmInNo;		//告警输入号 ParamId == NET_DEV_PARAM_ALARMIN_ALARM
        DWORD AlarmOutNo;		//告警输出号 ParamId == NET_DEV_PARAM_ALARMOUTCFG
        NET_DEV_ALARM_SRC AlarmSource;		//告警联动源 ParamId == NET_DEV_PARAM_ALARM_JOINT
    } Param;
};


//参数ID定义
//----------------------------------------------------

typedef enum _NET_DEV_PARAM_ID
{
    NET_DEV_PARAM_VERSION             = 1,  // NET_DEV_VERSION            设备出厂信息 v1
    NET_DEV_PARAM_BASECFG             = 2,  // NET_DEV_BASECFG            设备基础信息 v1
    NET_DEV_PARAM_TIME                = 3,  // NET_DEV_TIME_EX            系统时间信息 v1
    NET_DEV_PARAM_NET                 = 4,  // NET_DEV_ETHCFG             网络信息     v1
    NET_DEV_PARAM_SERVICECFG          = 5,  // NET_DEV_SERVICECFG         网络服务端口 v1
    NET_DEV_PARAM_USER_QUERY          = 6,  // NET_DEV_USERSET            用户信息查询 v1
    NET_DEV_PARAM_MODIFY_USER_CFG     = 7,  // NET_DEV_USER_MODIFY        修改用户信息 v1
    NET_DEV_PARAM_USER_ADD            = 8,  // NET_DEV_USER_INFO          增加用户(带权限)   v1
    NET_DEV_PARAM_USER_DEL            = 9,  // NET_DEV_USER               删除用户(用户名称) v1
    NET_DEV_PARAM_VIDEOIN_EFFECT      = 10, // NET_DEV_VIDEO_INPUT_EFFECT 视频输入参数 v1           CHN
    NET_DEV_PARAM_NVR_CHANNEL         = 11, // NET_DEV_NVR_CHANNEL        设备通道配置 v1 nvr       CHN
    NET_DEV_PARAM_CAMERA_CFG          = 12, // NET_DEV_CAMERACFG          图像参数     v1           CHN
    NET_DEV_PARAM_ENCODECFG           = 13, // NET_DEV_ENCODECFG          编码参数     v1           CHN
    NET_DEV_PARAM_VENC_OVERLAY        = 14, // NET_DEV_VENC_OVERLAY       遮挡参数                  CHN
    NET_DEV_PARAM_VENC_OSD            = 15, // NET_DEV_VENC_OSD           OSD参数                   CHN
    NET_DEV_PARAM_VENC_ROI            = 16, // NET_DEV_ROI                感兴趣区域编码            CHN
    NET_DEV_PARAM_MOTION_ALARM        = 17, // NET_DEV_MOTION_ALARM       移动告警                  CHN
    NET_DEV_PARAM_LOST_ALARM          = 18, // NET_DEV_LOST_ALARM         丢失告警                  CHN
    NET_DEV_PARAM_HIDE_ALARM          = 19, // NET_DEV_HIDE_ALARM         遮挡告警                  CHN
    NET_DEV_PARAM_ALARMIN_ALARM       = 20, // NET_DEV_ALARMIN_ALARM      告警输入
    NET_DEV_PARAM_ALARMOUTCFG         = 21, // NET_DEV_ALARMOUTCFG        告警输出
    NET_DEV_PARAM_PIR_ALARM           = 22, // NET_DEV_PIR_ALARM          PIR 告警                  CHN
    NET_DEV_PARAM_ALARM_JOINT         = 23, // NET_DEV_ALARM_JOINT        联动配置
    NET_DEV_PARAM_EXCEPTION           = 24, // NET_DEV_EXCEPTION          设备异常处理
    NET_DEV_PARAM_RECORD              = 25, // NET_DEV_RECORD             录像参数        v1        CHN
    NET_DEV_PARAM_SNAPPIC             = 26, // NET_DEV_SNAPPIC            抓拍参数                  CHN
    NET_DEV_PARAM_PHY_DISK            = 27, // NET_DEV_PHY_DISK           磁盘信息        v1
    NET_DEV_PARAM_PRESETSET_QUERY     = 28, // NET_DEV_PTZ_PRESETSET      查询预置点集合   v1       CHN
    NET_DEV_PARAM_CRUISEWAYSET_QUERY  = 29, // NET_DEV_PTZ_CRUISEWAYSET   查询巡航路径集合 v1       CHN
    NET_DEV_PARAM_CRUISEWAY           = 30, // NET_DEV_PTZ_CRUISEWAY      配置巡航路径     v1       CHN
    NET_DEV_PARAM_PTZ_PARKACTION_CFG  = 31, // NET_DEV_PTZ_PARKACTION_CFG 云台守望配置              CHN
    NET_DEV_PARAM_PTZ_TIME_TASK       = 32, // NET_DEV_PTZ_TIME_TASK      云台定时任务配置          CHN
    NET_DEV_PARAM_PTZ_CFG             = 33, // NET_DEV_PTZ_CFG            云台信息         v1       CHN
    NET_DEV_PARAM_SERIAL_CFG          = 34, // NET_DEV_SERIAL_CFG         串口参数
    NET_DEV_PARAM_DAYNIGHT            = 35, // NET_DEV_DAYNIGHT           日夜模式参数              CHN
    NET_DEV_PARAM_WIFI_APSET          = 36, // NET_DEV_WIFI_APSET         wifi 搜索
    NET_DEV_PARAM_WIFICFG             = 37, // NET_DEV_WIFICFG            wifi 配置
    NET_DEV_PARAM_PPPOECFG            = 38, // NET_DEV_PPPOECFG           PPPOE拨号配置
    NET_DEV_PARAM_PPPCFG              = 39, // NET_DEV_PPPCFG             PPP拨号配置
    NET_DEV_PARAM_DIALPARAM           = 40, // NET_DEV_DIALPARAM          3G拨号参数
    NET_DEV_PARAM_NTPCFG              = 41, // NET_DEV_NTPCFG             NTP 信息
    NET_DEV_PARAM_FTPUPLOAD_CFG       = 42, // NET_DEV_FTPUPLOAD_CFG      ftp 参数
    NET_DEV_PARAM_SMTP_CFG            = 43, // NET_DEV_SMTP_CFG           smtp 参数
    NET_DEV_PARAM_UPNP_CFG            = 44, // NET_DEV_UPNP_CFG           upnp 参数
    NET_DEV_PARAM_DDNS_CFG            = 45, // NET_DEV_DDNS_CFG           动态域名服务
    NET_DEV_PARAM_NET_DISK_CFG        = 46, // NET_DEV_NET_DISKCFG        网络磁盘管理
    NET_DEV_PARAM_ALARMHOST_CFG       = 47, // NET_DEV_ALARMHOST_CFG      报警上传配置
    NET_DEV_PARAM_CMS_CFG             = 48, // NET_DEV_CMS_CFG            平台信息
    NET_DEV_PARAM_GB28181_CFG         = 49, // NET_DEV_GB28181_CFG        GB28181平台
    NET_DEV_PARAM_PATTERNSET_QUERY    = 50, // NET_DEV_PTZ_PATTERNSET     查询PTZ轨迹集合   v2     CHN
    NET_DEV_PARAM_AVD_CFG             = 51, // NET_DEV_AVD_CFG            IVS视频诊断配置          CHN
    NET_DEV_PARAM_PEAOSC_CFG          = 52, // NET_DEV_PEAOSC_CFG         IVS周界配置              CHN
    NET_DEV_PARAM_CPC_CFG             = 53, // NET_DEV_CPC_CFG            IVS目标检测              CHN
    NET_DEV_PARAM_VO_CFG              = 54, // NET_DEV_VO_CFG             VO参数配置              CHN
    NET_DEV_PARAM_MAX
}NET_DEV_PARAM_ID;


//消息定义
//----------------------------------------------------

#define NET_DEV_MAX_PIC_SIZE (512*1024)

typedef struct _NET_DEV_LOGIN_REQ NET_DEV_LOGIN_REQ;
struct _NET_DEV_LOGIN_REQ
{
    NET_DEV_USER User;		// 用户名，密码
    DWORD KeepAliveTime;		// 心跳周期 单位 s
};


typedef struct _NET_DEV_LOGIN_RSP NET_DEV_LOGIN_RSP;
struct _NET_DEV_LOGIN_RSP
{
    NET_DEV_USER_INFO UserInfo;		// 用户信息(包括权限)
};


typedef struct _NET_DEV_ALARMSUBSCRIBE NET_DEV_ALARMSUBSCRIBE;
struct _NET_DEV_ALARMSUBSCRIBE
{
    DWORD Subscribe;		// 是否订阅告警信息
};


typedef struct _NET_DEV_MANUL_SNAPPIC_REQ NET_DEV_MANUL_SNAPPIC_REQ;
struct _NET_DEV_MANUL_SNAPPIC_REQ
{
    BYTE StreamType;
    BYTE Quality;
    BYTE Res[2];
};


typedef struct _NET_DEV_MANUL_SNAPPIC_RSP NET_DEV_MANUL_SNAPPIC_RSP;
struct _NET_DEV_MANUL_SNAPPIC_RSP
{
    BYTE Data[NET_DEV_MAX_PIC_SIZE];
};


typedef struct _NET_DEV_STATE_QUERY_REQ NET_DEV_STATE_QUERY_REQ;
struct _NET_DEV_STATE_QUERY_REQ
{
    DWORD StateType;		//NET_DEV_STATE_IDX
};


typedef struct _NET_DEV_STATE_QUERY_RSP NET_DEV_STATE_QUERY_RSP;
struct _NET_DEV_STATE_QUERY_RSP
{
    DWORD StateType;		//NET_DEV_STATE_IDX
    union {
        DWORD PixCalState;		//NET_DEV_PIX_CAL_STATE
        DWORD IrisCalState;		//NET_DEV_IRIS_CAL_STATE
        NET_DEV_NETSTATES NetState;
        NET_DEV_UPNP_STATE UpnpState;
        NET_DEV_DISKSTATES DiskState;
        NET_DEV_CHANNSTATES ChannState;
        NET_DEV_WORKSTATE WorkState;
    } States;
};


typedef struct _NET_DEV_FORMATDISK_REQ NET_DEV_FORMATDISK_REQ;
struct _NET_DEV_FORMATDISK_REQ
{
    DWORD DiskNo;		// 磁盘号
    DWORD Operate;		// 0: 执行格式化， 1：查询格式化进度
};


typedef struct _NET_DEV_FORMATDISK_RSP NET_DEV_FORMATDISK_RSP;
struct _NET_DEV_FORMATDISK_RSP
{
    DWORD DiskNo;		// 磁盘号
    DWORD Progress;		// 格式化进度(0 - 100), -1:表示出错
};


typedef struct _NET_DEV_KEEPALIVE_RSP NET_DEV_KEEPALIVE_RSP;
struct _NET_DEV_KEEPALIVE_RSP
{
    NET_DEV_TIME_EX TimeEx;		// 系统时间
};


typedef struct _NET_DEV_NVR_DEVLIST_REQ NET_DEV_NVR_DEVLIST_REQ;
struct _NET_DEV_NVR_DEVLIST_REQ
{
    DWORD ProtolType;		//设备协议类型 NET_DEV_PROTOL_TYPE
};


typedef struct _NET_DEV_NVR_DEVLIST_RSP NET_DEV_NVR_DEVLIST_RSP;
struct _NET_DEV_NVR_DEVLIST_RSP
{
    DWORD DeviceCount;		//返回的设备个数
    NET_DEV_REMOTE_DEVICE Devices[NET_DEV_MAX_DEVLIST_NUM];		//设备列表
};


typedef struct _NET_DEV_PF_REGISTER_REQ NET_DEV_PF_REGISTER_REQ;
struct _NET_DEV_PF_REGISTER_REQ 	// 注册平台
{
    CHAR DevID[NET_DEV_ID_LEN];
    CHAR Passwd[NET_DEV_PASSWORD_LEN];
    CHAR Ingress[NET_DEV_HOST_LEN];
    DWORD OEMID;
};


typedef struct _NET_DEV_PF_REGISTER_RSP NET_DEV_PF_REGISTER_RSP;
struct _NET_DEV_PF_REGISTER_RSP
{
    DWORD KeepAlive;
};


typedef struct _NET_DEV_PF_GETMDS_REQ NET_DEV_PF_GETMDS_REQ;
struct _NET_DEV_PF_GETMDS_REQ
{
    CHAR DevID[NET_DEV_ID_LEN];
    CHAR Ingress[NET_DEV_HOST_LEN];
};


typedef struct _NET_DEV_PF_MDSIP NET_DEV_PF_MDSIP;
struct _NET_DEV_PF_MDSIP
{
    NET_DEV_IPADDR Ip;
    DWORD Port;
};


typedef struct _NET_DEV_PF_KEEPALIVE_RSP NET_DEV_PF_KEEPALIVE_RSP;
struct _NET_DEV_PF_KEEPALIVE_RSP
{
    DWORD UTCTime;
    DWORD KeepAlive;
};


typedef struct _NET_DEV_PF_CMSTIME NET_DEV_PF_CMSTIME;
struct _NET_DEV_PF_CMSTIME
{
    DWORD UTCTime;
};


typedef struct _NET_DEV_PF_CHNLSTATE NET_DEV_PF_CHNLSTATE;
struct _NET_DEV_PF_CHNLSTATE
{
    DWORD CH00TO31;		//音视频
    DWORD CH32TO63;		//音视频
};


//----------------------------------------------------

#define NET_DEV_DEC_MAX_MODE_NUM        32
#define NET_DEV_DEC_MAX_DIV_NUM         64
#define NET_DEV_DEC_DESCRIBE_LEN        256
#define NET_DEV_DEC_NAME_LEN            64

typedef enum _NET_DEV_DEC_DIVID
{
    NET_DEV_DEC_DIV1  = 0x01,
    NET_DEV_DEC_DIV4  = 0x02,
    NET_DEV_DEC_DIV6  = 0x03,
    NET_DEV_DEC_DIV9  = 0x04,
    NET_DEV_DEC_DIV16 = 0x05,
    NET_DEV_DEC_DIV25 = 0x06,
    NET_DEV_DEC_DIV36 = 0x07,
    NET_DEV_DEC_DIV64 = 0x08
}NET_DEV_DEC_DIVID;

typedef enum _NET_DEV_DEC_ACTION
{
    NET_DEV_DEC_STEP  = 0x01,
    NET_DEV_DEC_TOUR  = 0x02,
    NET_DEV_DEC_GROUP = 0x03
}NET_DEV_DEC_ACTION;

typedef struct _NET_DEV_DEC_DIVMODE NET_DEV_DEC_DIVMODE;
struct _NET_DEV_DEC_DIVMODE
{
    DWORD DivId;		//分屏模式ID NET_DEV_DEC_DIVID
    CHAR Name[NET_DEV_DEC_NAME_LEN];
    CHAR Desc[NET_DEV_DEC_DESCRIBE_LEN];
};


typedef struct _NET_DEV_DEC_DIVMODE_QUERY_REQ NET_DEV_DEC_DIVMODE_QUERY_REQ;
struct _NET_DEV_DEC_DIVMODE_QUERY_REQ
{
    DWORD BeginNo;
    DWORD ReqCount;
    DWORD SessionId;
};


typedef struct _NET_DEV_DEC_DIVMODE_QUERY_RSP NET_DEV_DEC_DIVMODE_QUERY_RSP;
struct _NET_DEV_DEC_DIVMODE_QUERY_RSP
{
    DWORD SessionId;
    DWORD TotalCount;
    DWORD RspCount;
    NET_DEV_DEC_DIVMODE Items[NET_DEV_DEC_MAX_MODE_NUM];
};


typedef struct _NET_DEV_DEC_DIVINFO NET_DEV_DEC_DIVINFO;
struct _NET_DEV_DEC_DIVINFO
{
    DWORD No;		//分隔序号
    DWORD Action;		//动作类型: NET_DEV_DEC_ACTION
    CHAR ActionName[NET_DEV_DEC_NAME_LEN];		//巡回/群组名称
    DWORD Result;		//0:成功, !0:失败
    CHAR Encoder[NET_DEV_DEC_NAME_LEN];		//编码器名称
    DWORD EncChann;		//编码器通道
    DWORD Level;		//码流类型
    CHAR Url[NET_DEV_MAX_URL_LEN];		//编码器URL（为NULL则在当前分割窗口显示黑屏）
    DWORD Flag;		//清分割标志（0：不清除分割1：清除分割）
};


typedef struct _NET_DEV_DEC_FULLSCREENINFO NET_DEV_DEC_FULLSCREENINFO;
struct _NET_DEV_DEC_FULLSCREENINFO
{
    CHAR Encoder[NET_DEV_DEC_NAME_LEN];		//编码器名称
    DWORD EncChnn;		//编码器通道
    DWORD Level;		//码流类型
    DWORD Action;		//动作类型: NET_DEV_DEC_ACTION
    CHAR ActionName[NET_DEV_DEC_NAME_LEN];		//巡回/群组名称
    DWORD Result;		//0:成功, !0:失败
};


typedef struct _NET_DEV_DEC_SCREENSTATE NET_DEV_DEC_SCREENSTATE;
struct _NET_DEV_DEC_SCREENSTATE
{
    DWORD DivId;		//分屏模式ID NET_DEV_DEC_DIVID
    DWORD DivCount;		//分割个数
    NET_DEV_DEC_DIVINFO DivInfo[NET_DEV_DEC_MAX_DIV_NUM];
    DWORD IsFullScreen;		//是否全屏
    NET_DEV_DEC_FULLSCREENINFO FullScreenInfo;		//
};


typedef struct _NET_DEV_DEC_SCREEN NET_DEV_DEC_SCREEN;
struct _NET_DEV_DEC_SCREEN
{
    DWORD DivId;		//分屏模式ID NET_DEV_DEC_DIVID
    DWORD DispChann;		//显示通道号
};


typedef struct _NET_DEV_DEC_DIV NET_DEV_DEC_DIV;
struct _NET_DEV_DEC_DIV
{
    DWORD DivNo;		//分割序号 -1: 表示清除所有分割
    DWORD DispChann;		//显示通道号
};


typedef struct _NET_DEV_DEC_TVWPLAY NET_DEV_DEC_TVWPLAY;
struct _NET_DEV_DEC_TVWPLAY
{
    DWORD Action;		//动作类型: NET_DEV_DEC_ACTION
    CHAR Name[NET_DEV_DEC_NAME_LEN];		//巡回或群组的名称（单步没有名称）
    DWORD Step;		//步序号（单步填1）
    DWORD DispChann;		//显示通道编号
    DWORD DivId;		//分屏模式ID NET_DEV_DEC_DIVID
    DWORD KeepOther;		//是否保持其它分割的运行状态, 0-不保持, 黑屏, 1-保持
    DWORD DivCount;		//本次下发的编码器个数
    NET_DEV_DEC_DIVINFO DivInfo[NET_DEV_DEC_MAX_DIV_NUM];
};


typedef enum _NET_DEV_DEC_BIND_TYPE
{
    NET_DEV_DEC_BIND_COM    = 0x01,   // 串口
    NET_DEV_DEC_BIND_AUDIO  = 0x02,   // 音频
}NET_DEV_DEC_BIND_TYPE;

typedef struct _NET_DEV_DEC_BIND NET_DEV_DEC_BIND;
struct _NET_DEV_DEC_BIND
{
    DWORD Type;		// 数据类型 NET_DEV_DEC_BIND_TYPE
    BYTE Enable;		// 1: 使能, 0: 关闭
    BYTE Res[3];		// 保留
    WORD Channel;		// 远程数据通道号 默认 0
    WORD Port;		// 远程设备端口 默认 554
    NET_DEV_IPADDR IPAddr;		// 远程设备IP
};


typedef struct _NET_DEV_DEC_BIND_Q NET_DEV_DEC_BIND_Q;
struct _NET_DEV_DEC_BIND_Q
{
    DWORD Type;		// 数据类型 NET_DEV_DEC_BIND_TYPE
};


//消息ID定义
//----------------------------------------------------

typedef enum _NET_DEV_MSG_ID
{
    /*-------- ID ---------------------------  REQ -----------------------  RSP  ---------------------- NOTE  ----*/
    NET_DEV_MSG_LOGIN           = 1000,     // NET_DEV_LOGIN_REQ            NET_DEV_LOGIN_RSP           用户登录
    NET_DEV_MSG_LOGOUT          = 1001,     // ----                         ----                        用户注销
    NET_DEV_MSG_CAPABILITIES    = 1002,     // ----                         NET_DEV_CAPABILITIES        获取能力集
    NET_DEV_MSG_ALARMSUBSCRIBE  = 1003,     // NET_DEV_ALARMSUBSCRIBE       ----                        订阅告警信息
    NET_DEV_MSG_PTZ_CONTROL     = 1004,     // NET_DEV_PTZ_CONTROL          ----                        云台控制      CHN
    NET_DEV_MSG_MANUL_SNAPPIC   = 1005,     // NET_DEV_MANUL_SNAPPIC_REQ    NET_DEV_MANUL_SNAPPIC_RSP   手动抓拍(DC)  CHN
    NET_DEV_MSG_RECORD_QUERY    = 1006,     // NET_DEV_RECORD_QUERY_REQ     NET_DEV_RECORD_QUERY_RSP    录像查询      CHN
    NET_DEV_MSG_PIC_QUERY       = 1007,     // NET_DEV_PIC_QUERY_REQ        NET_DEV_PIC_QUERY_RSP       图片查询      CHN
    NET_DEV_MSG_LOG_QUERY       = 1008,     // NET_DEV_LOG_QUERY_REQ        NET_DEV_LOG_QUERY_RSP       日志查询
    NET_DEV_MSG_DEV_CONTROL     = 1009,     // NET_DEV_CONTROL              ----                        设备控制
    NET_DEV_MSG_DEV_STATE       = 1010,     // NET_DEV_STATE_QUERY_REQ      NET_DEV_STATE_QUERY_RSP     设备状态
    NET_DEV_MSG_FORMATDISK      = 1011,     // NET_DEV_FORMATDISK_REQ       NET_DEV_FORMATDISK_RSP      磁盘格式化
    NET_DEV_MSG_ALARMSUBMIT     = 1012,     // NET_DEV_ALARM_ITEM           ----                        告警上报信息
    NET_DEV_MSG_MEDIA_URL       = 1013,     // NET_DEV_MEDIA_URL            NET_DEV_MEDIA_URL           获取媒体URL   CHN
    NET_DEV_MSG_KEEPALIVE       = 1014,     // ----                         NET_DEV_KEEPALIVE_RSP       心跳
    NET_DEV_MSG_NVR_DEVLIST     = 1015,     // NET_DEV_NVR_DEVLIST_REQ      NET_DEV_NVR_DEVLIST_RSP     获取设备列表
    /*---------------  decoder -------------*/
    NET_DEV_MSG_DEC_DIVMODE_QUERY  = 1080,  //NET_DEV_DEC_DIVMODE_QUERY_REQ NET_DEV_DEC_DIVMODE_QUERY_RSP 获取分屏模式集合
    NET_DEV_MSG_DEC_SCREEN_STATE   = 1081,  // ----                         NET_DEV_DEC_SCREENSTATE       获取屏幕状态
    NET_DEV_MSG_DEC_SCREEN_SETDIV  = 1082,  //NET_DEV_DEC_SCREEN            ----                          改变分屏模式
    NET_DEV_MSG_DEC_SCREEN_ENTERFULL=1083,  //NET_DEV_DEC_DIV               ----                          进入全屏显示
    NET_DEV_MSG_DEC_SCREEN_EXITFULL= 1084,  //NET_DEV_DEC_DIV               ----                          退出全屏显示
    NET_DEV_MSG_DEC_TVW_PLAY       = 1085,  //NET_DEV_DEC_TVWPLAY           NET_DEV_DEC_TVWPLAY           电视墙播放
    NET_DEV_MSG_DEC_DIV_CLEAR      = 1086,  //NET_DEV_DEC_DIV               ----                          清除分割显示
    NET_DEV_MSG_DEC_BIND_SET       = 1087,  //NET_DEV_DEC_BIND              ----                          绑定数据源配置
    NET_DEV_MSG_DEC_BIND_STATE     = 1088,  //NET_DEV_DEC_BIND_Q            NET_DEV_DEC_BIND              绑定数据源状态

    //@{Pf msg}
    NET_DEV_MSG_PF_REGISTER     = 1100,     // NET_DEV_PF_REGISTER_REQ      NET_DEV_PF_REGISTER_RSP     注册平台(SDK内部使用)
    NET_DEV_MSG_PF_GETMDS       = 1101,     // NET_DEV_PF_GETMDS_REQ        NET_DEV_PF_MDSIP            获取MDS地址信息(SDK内部使用)
    NET_DEV_MSG_PF_KEEPALIVE    = 1102,     // NET_DEV_PF_CHNLSTATE         NET_DEV_PF_KEEPALIVE_RSP    心跳(SDK内部使用)
    NET_DEV_MSG_PF_MDSUPDATE    = 1103,     // ----                         ----                        MDS信息变更(SDK内部使用)
    NET_DEV_MSG_PF_CMSTIME      = 1104,     // NET_DEV_PF_CMSTIME           ----                        CMS时间
    NET_DEV_MSG_PF_GETCHNLSTATE = 1105      // ----                         NET_DEV_PF_CHNLSTATE        获取通道在线状态
}NET_DEV_MSG_ID;


//媒体结构定义
//----------------------------------------------------

/*媒体类型*/
typedef enum _NET_DEV_MEDIA_TYPE
{
    MEDIA_TYPE_VIDEO    = 0x01,
    MEDIA_TYPE_AUDIO    = 0x02,
    MEDIA_TYPE_METADATA = 0x04
}NET_DEV_MEDIA_TYPE;

/*编码类型*/
typedef enum _NET_DEV_MEDIA_ENCODETYPE
{
   NET_DEV_MEDIA_ENC_TYPE_H264,
   NET_DEV_MEDIA_ENC_TYPE_MPEG4,
   NET_DEV_MEDIA_ENC_TYPE_G711A,
   NET_DEV_MEDIA_ENC_TYPE_G711U,
   NET_DEV_MEDIA_ENC_TYPE_JPEG,
   NET_DEV_MEDIA_ENC_TYPE_METADATA,
   NET_DEV_MEDIA_ENC_TYPE_H265,
   NET_DEV_MEDIA_ENC_TYPE_RESERVED1,
   NET_DEV_MEDIA_ENC_TYPE_RESERVED2,
   NET_DEV_MEDIA_ENC_TYPE_RESERVED3,
   NET_DEV_MEDIA_ENC_TYPE_AAC,
}NET_DEV_MEDIA_ENCODETYPE;

/*采样率*/
typedef enum _NET_DEV_MEDIA_SAMPLERATE
{
    NET_DEV_MEDIA_SAMPLE_RATE_08K = 8,
    NET_DEV_MEDIA_SAMPLE_RATE_90K = 90
}NET_DEV_MEDIA_SAMPLERATE;

/*帧类型*/
typedef enum _NET_DEV_MEDIA_FRAMETYPE
{
    NET_DEV_MEDIA_FRAME_TYPE_I   = 0x01,   /*视频I帧*/
    NET_DEV_MEDIA_FRAME_TYPE_P   = 0x02,   /*视频P帧*/
    NET_DEV_MEDIA_FRAME_TYPE_B   = 0x03,   /*视频B帧*/
    NET_DEV_MEDIA_FRAME_TYPE_A   = 0x04,   /*音频帧*/
    NET_DEV_MEDIA_FRAME_TYPE_M   = 0x05,   /*Meta帧*/
    NET_DEV_MEDIA_FRAME_TYPE_EXT = 0xfe,   /*Ext帧(MSS)*/
    NET_DEV_MEDIA_FRAME_TYPE_EOF = 0xff    /*结束帧*/
}NET_DEV_MEDIA_FRAMETYPE;

typedef enum _NET_DEV_META_FRAMETYPE
{
    NET_DEV_META_FRAME_IVS_PEA = 0x01,    /*IVS PEA&OSC*/
    NET_DEV_META_FRAME_IVS_CPC = 0x02,    /*IVS CPC*/
    NET_DEV_META_FRAME_AGW_DATA = 0x10,   /*AGW DATA*/
    NET_DEV_META_FRAME_DEV_POSITION = 0x20, /*DEV POSITION*/
}NET_DEV_META_FRAMETYPE;

/*Meta扩展信息*/
typedef struct _NET_DEV_MEDIA_MFRAMEEXT NET_DEV_MEDIA_MFRAMEEXT;
struct _NET_DEV_MEDIA_MFRAMEEXT
{
    WORD Type;                          /* Meta帧类型 NET_DEV_META_FRAMETYPE */
    WORD Res;                           /* 保留 */
    DWORD UTCTime;                      /* UTC时间 */
};
/*视频扩展信息*/
typedef struct _NET_DEV_MEDIA_VFRAMEEXT NET_DEV_MEDIA_VFRAMEEXT;
struct _NET_DEV_MEDIA_VFRAMEEXT
{
    WORD  Width;                        /*视频宽度*/
    WORD  Height;                       /*视频高度*/
    DWORD UTCTime;
};

/*音频扩展信息*/
typedef struct _NET_DEV_MEDIA_AFRAMEEXT NET_DEV_MEDIA_AFRAMEEXT;
struct _NET_DEV_MEDIA_AFRAMEEXT
{
    BYTE SamplesPerSec;                 /*采样率 NET_DEV_MEDIA_SAMPLERATE */
    BYTE BitsPerSample;                 /*位宽 16 */
    BYTE ChannelNum;                    /*音频通道数目, 如 1，2 */
    BYTE Reserved;                      /*保留*/
};


/* IVS帧扩展信息 */
typedef struct _NET_DEV_MEDIA_IVS_HDR
{
  BYTE Mask;          // 目前使用bit0: 有效帧标识，为0 表示清除所有的规则/目标
  BYTE RuleNum;       // 包含规则个数(Mask & 0x1 == 1时有效) <= NET_DEV_MAX_PEAOSC_RULE_NUM/NET_DEV_MAX_CPC_RULE_NUM
  BYTE EventNum;      // 包含事件个数(Mask & 0x1 == 1时有效) <= NET_DEV_MAX_PEAOSC_EVENT_NUM/NET_DEV_MAX_CPC_EVENT_NUM
  BYTE TargetNum;     // 包含目标个数(Mask & 0x1 == 1时有效) <= NET_DEV_MAX_PEAOSC_TARGET_NUM/NET_DEV_MAX_CPC_TARGET_NUM
  WORD MaxWidth;      // 参考宽度
  WORD MaxHeight;     // 参考高度
}NET_DEV_MEDIA_IVS_HDR;

/*视频帧NAL描述*/
typedef struct _NET_DEV_MEDIA_NALDESC NET_DEV_MEDIA_NALDESC;
struct _NET_DEV_MEDIA_NALDESC

{
    DWORD NalNum;                       /*NAL个数*/
    struct _NAL
    {
        DWORD NalOffset;                /*NAL地址*/
        DWORD NalSize;                  /*NAL大小*/
    }Nal[0];
};

/* IVS帧数据格式 [----NET_DEV_MEDIA_IVS_HDR---- | ----RULE*N----- | ----EVENT*N----- | ----TARGET*N-----] */
/*             ^                              ^                                                         */
/*             hdr                            data                                                      */

#define NET_DEV_MEDIA_PEAOSC_RULE(hdr, data) ((NET_DEV_PEAOSC_RESULT_RULE*)(data))
#define NET_DEV_MEDIA_PEAOSC_EVENT(hdr, data) ((NET_DEV_PEAOSC_EVENT*)(NET_DEV_MEDIA_PEAOSC_RULE(hdr, data)+(hdr)->RuleNum))
#define NET_DEV_MEDIA_PEAOSC_TARGET(hdr, data) ((NET_DEV_PEAOSC_TARGET*)(NET_DEV_MEDIA_PEAOSC_EVENT(hdr, data)+(hdr)->EventNum))

#define NET_DEV_MEDIA_CPC_RULE(hdr, data) ((NET_DEV_CPC_RESULT_RULE*)(data))
#define NET_DEV_MEDIA_CPC_EVENT(hdr, data) ((NET_DEV_CPC_EVENT*)(NET_DEV_MEDIA_CPC_RULE(hdr, data)+(hdr)->RuleNum))
#define NET_DEV_MEDIA_CPC_TARGET(hdr, data) ((NET_DEV_CPC_TARGET*)(NET_DEV_MEDIA_CPC_EVENT(hdr, data)+(hdr)->EventNum))

/*音/视频帧头*/
typedef struct _NET_DEV_MEDIA_FRAMEHEADER NET_DEV_MEDIA_FRAMEHEADER;
struct _NET_DEV_MEDIA_FRAMEHEADER
{
    WORD  No;                           /*帧序号*/
    WORD  EncodeType;                   /*编码类型*/
    DWORD Timestamp;                    /*时间戳 MS*/
    BYTE Type;                          /*帧类型*/
    BYTE RefIdc;                        /*参考指示,00:非参考图像,大于00值指示相对传输优先级*/
    BYTE Res[2];                        /*保留*/
    union
    {
        NET_DEV_MEDIA_VFRAMEEXT Video;  /*视频扩展*/
        NET_DEV_MEDIA_AFRAMEEXT Audio;  /*音频扩展*/
        NET_DEV_MEDIA_MFRAMEEXT Meta;   /*Meta扩展*/
    };
    DWORD Size;                         /*帧数据长度*/
};

/*帧描述*/
typedef struct _NET_DEV_MEDIA_FRAME NET_DEV_MEDIA_FRAME;
struct _NET_DEV_MEDIA_FRAME
{
    NET_DEV_MEDIA_FRAMEHEADER Hdr;    /*帧头*/
    BYTE *Data;                       /*帧数据*/
    NET_DEV_MEDIA_NALDESC *NalDesc;   /*视频帧NAL描述*/
};

/*图像参数*/
typedef struct _NET_DEV_MEDIA_PICPARAM NET_DEV_MEDIA_PICPARAM;
struct _NET_DEV_MEDIA_PICPARAM
{
    WORD SPSSize;                     /*sps数据长度*/
    WORD PPSSize;                     /*pps数据长度*/
    BYTE SPS[128];                    /*不含起始码*/
    BYTE PPS[128];                    /*不含起始码*/
};

/*视频信息*/
typedef struct _NET_DEV_MEDIA_VIDEOPARAM NET_DEV_MEDIA_VIDEOPARAM;
struct _NET_DEV_MEDIA_VIDEOPARAM
{
    WORD EncodeType;                  /*编码类型*/
    WORD BitRate;                     /*码率*/
    WORD SamplesPerSec;               /*采样率*/
    WORD FrameRate;                   /*帧率*/
    NET_DEV_MEDIA_PICPARAM PicParam;  /*图像参数*/
};

/*音频信息*/
typedef struct _NET_DEV_MEDIA_AUDIOPARAM NET_DEV_MEDIA_AUDIOPARAM;
struct _NET_DEV_MEDIA_AUDIOPARAM
{
    WORD EncodeType;                /*编码类型*/
    WORD BitRate;                   /*码率*/
    WORD SamplesPerSec;             /*采样率*/
    WORD BitsPerSample;             /*位宽*/
    WORD FrameRate;                 /*帧率*/
    WORD ChannelNum;                /*音频通道数目, 如 1，2 */
};

/*Meta信息*/
typedef struct _NET_DEV_MEDIA_METAPARAM NET_DEV_MEDIA_METAPARAM;
struct _NET_DEV_MEDIA_METAPARAM
{
    WORD Size;                         /*Meta私有数据长度, 0:无*/
    BYTE Data[128];                    /*Meta私有数据内容*/
};

/*媒体参数*/
typedef struct _NET_DEV_MEDIA_PARAM NET_DEV_MEDIA_PARAM;
struct _NET_DEV_MEDIA_PARAM
{
    DWORD Mask;                      /*媒体类型掩码*/
    DWORD BeginTime;                 /*媒体开始时间*/
    DWORD EndTime;                   /*媒体结束时间*/
    NET_DEV_MEDIA_VIDEOPARAM Video;  /*音频媒体信息*/
    NET_DEV_MEDIA_AUDIOPARAM Audio;  /*视频媒体信息*/
    NET_DEV_MEDIA_METAPARAM  Meta;   /*Meta媒体信息*/
};


#endif //__NET_DEV_STRUCT_H__
