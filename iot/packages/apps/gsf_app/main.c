#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <libgen.h>
#include <getopt.h>
#include "cJSON.h"
#include <malloc.h>
#include "gsf_net_api.h"
#include "base64.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include<unistd.h>
#include "NET_DEV_PU_SDK.H"
#include "NET_DEV_STRUCT.H"
#include "gsf_data_struct.h"
#include "sadp_pu_api.h"
#include "fifolib.h"
#include "mempool.h"
#include "wrapperiotdevcamera.h"

pthread_t thread[2]; 
void * pcm_buf = NULL;
char gsf_frame_buffer[512*1024] = {0};                                          
int gsf_get_frame = 0;                                                              
int gsf_frame_size = 0;                                                              
int gsf_get_frame_already = 0;
static int g_enable_gsf = 1;
static int g_cms_port = 7000;
static char g_cms_ip[64] = "106.15.206.68";
static char g_puid[32] = "00000070061130000105";

Data_FIFO *g_AudioTlak = NULL;

extern int audio_already;
extern void *recordG711aBuf;
extern int   readLen;
extern int audioTrack_write(char *pcmBuf, int len);
void *stream_thread(void *buf);

#define VERSION_STR "1.0"

#define BUG() \
{ \
    char *________ptr = 0;*________ptr = 0; \
}


#define G711A_AUDIO_PATH "AudioEnc.g711a"
#define HEVC_VIDEO_PATH  "VideoEnc720p.h265"
#define H264_VIDEO_PATH  "VideoEnc720p.h264"
//#define PS_FILE_PATH  "1.dat"
#define PS_FILE_PATH "/mnt/sd/20200102/1.dat"

static int g_hevc_video = 0;
static char TEST_TALK_FILE[4096] = {0};
static char TEST_FILE_PATH[4096] = {0};
static char TEST_PSFILE_PATH[4096] = {0};
static int g_iFrameRate = 0;

#define FRAME_SIZE  512*1024
#define PORT        6001

#define RECORD_PROPERTY_FMT "index=%d,offset=%d,beginTime=%u, endTime=%u"
#define IMA_CAP ((1<<NET_DEV_IMA_BRIGHT_CAP) | (1<<NET_DEV_IMA_CONTRAST_CAP)\
                | (1<<NET_DEV_IMA_HUE_CAP) | (1<<NET_DEV_IMA_SATURATION_CAP)\
                | (1<<NET_DEV_IMA_SHARPNESS_CAP) | (1<<NET_DEV_IMA_SHARPNESS_CAP)\
                | (1<<NET_DEV_IMA_MIRROR_CAP) | (1<<NET_DEV_IMA_FLIP_CAP)\
                | (1<<NET_DEV_IMA_ANTIFOG_CAP) | (1<<NET_DEV_IMA_AWB_CAP))
#define IMA_RESOLUTION_0 ((1<<NET_DEV_IMG_1080P) | (1<<NET_DEV_IMG_960) | (1<<NET_DEV_IMG_720P))
#define IMA_RESOLUTION_1 ((1<<NET_DEV_IMG_D1) | (1<<NET_DEV_IMG_CIF))
#define MSG_PORT  8080
#define RTSP_PORT 554
#define HTTP_PORT 80
#define RTMP_PORT 1935
#define IPV4_ADDR "192.168.100.100"
#define IPV4_NETMASK "255.255.0.0"
#define IPV4_GATEWAY "192.168.100.1"
#define IPV4_DNS1 "8.8.8.8"
#define IPV4_DNS2 "8.8.4.4"
#define IPV4_MAC "00:11:22:33:44:55"
#define SW_VER "v1.1.1"
#define HW_VER "v1.1.0"
#define DSP_SW_VER "v1.0.1"
#define PAN_VER "v1.0.0"
#define SERIAL_NO "ipc-demo123456789"
#define DEV_NAME "ipc"
#define PRODUCT_NAME "ipc company"
#define VER_DATE_TIME __DATE__" "__TIME__

typedef int (*GSFAUDIO_DATA_PROCESS)(char *_cBuf,int _iLen,char * FileName);

typedef enum
{
    FRAME_TYPE_I = 0,
    FRAME_TYPE_P = 1,
    FRAME_TYPE_NULL       
}frame_type_t;

typedef enum
{
    DATA_TYPE_VIDEO = 0,
    DATA_TYPE_PICTURE,
    DATA_TYPE_AUDIO,
    DATA_TYPE_TEMPLE,
    DATA_TYPE_INFOR,
    DATA_TYPE_NULL       
}stream_data_type_t;

typedef enum  
{
    H264_VIDEO_ENCODER,
    H265_VIDEO_ENCODER,
    MPEG_VIDEO_ENCODER,
    JPEG_VIDEO_ENCODER,
    NUM_VIDEO_ENCODERS
} video_encoder_t;

typedef enum  
{
    G711U_AUDIO_ENCODER,
    G711A_AUDIO_ENCODER,
    G726_AUDIO_ENCODER,
    AAC_AUDIO_ENCODER,
    NUM_AUDIO_ENCODERS
} audio_encoder_t;


static NET_DEV_MEDIA_PICPARAM g_PicParam[32][2];
static int g_iWith[32][2];
static int g_iHeight[32][2];

#define  MAX_PSFRAME_SIZE  (2*1024*1024)
#define  MAX_READBUF_SIZE  (512*1024)
#define  MAX_IFRMAE_NUM  1024
GSFAUDIO_DATA_PROCESS g_AudioProcess=NULL;
typedef struct
{
	int m_iDataSize;		// ??ʵ?????ݳ???
	int m_iDataType;		// ????????  
    int m_iFrameType;       //֡????
    int m_iEnCodeType;       //????????
    int m_iWith;
    int m_iHight;
    unsigned long long m_ullTimeStap;  //ʱ????
	int m_FrameSeq; //֡????
	char m_bData[0];		// ??????
} frame_element_t;

typedef int (*STREAM_DATA_PROCESS)(void * _pMedia,frame_element_t * pFrame,void *_pContext);

#define HTTP_POST_FMT \
"POST /open-api/app/pic/accept HTTP/1.1\r\n"\
"Content-Type: application/json\r\n"\
"User-Agent: PostmanRuntime/7.26.1\r\n"\
"Accept: */*\r\n"\
"Cache-Control: no-cache\r\n"\
"Host: %s:%d\r\n"\
"Accept-Encoding: gzip, deflate, br\r\n"\
"Connection: keep-alive\r\n"\
"Content-Length: %d\r\n\r\n"\
"%s"


int gsf_http_post(const char *ipv4, int port, int timeout_ms, char *outbuf, int len)
{
	int nRet = 0;
	int nHttpVer1 = 0, nHttpVer2 = 0, nHttpStatus = 0;
	int nJpegLen = 0, nSndSize = 0, nRecvSize = 0;
	char szHttpStatus[128] = {0};
	char szPort[32] = {0};
	char *pszMsg= NULL;
	int nSock = -1;
	int iRcvLen =0;
	pszMsg = malloc(len+512);
	if(NULL==pszMsg)
	{
		nRet =  -1;
		goto leave;
	}
	sprintf(szPort, "%d", port);
	nSock = gsf_tcp_noblock_connect(NULL, NULL, ipv4, szPort, timeout_ms);
	if (nSock == -1)
	{
		printf("tcp connect(%s:%d) err %d(%s)\n", ipv4, port, errno, strerror(errno));
		nRet =  -1;
		goto leave;
	}
	nSndSize = sprintf(pszMsg, HTTP_POST_FMT, ipv4,port,len,outbuf);
	if (gsf_tcp_noblock_send(nSock, (unsigned char *)pszMsg, nSndSize, NULL, timeout_ms) != nSndSize)
	{
		printf("tcp send host(%s:%d) sock(%d) err %d(%s)\n", ipv4, port, nSock, errno, strerror(errno));
		nRet =  -1;
		goto leave;
	}
	if ((nRet = gsf_select(&nSock, 0x1, 0x1, timeout_ms)) != (0x10000 | nSock))
	{//no response data
		printf("select host(%s:%d) err %0#x\n", ipv4, port, nRet);
		nRet =  -1;
		goto leave;
	}
	
	// recv
	memset(pszMsg, 0,len+512);
	nRecvSize = len+512;
	while(iRcvLen<16)
	{
		nRecvSize = gsf_tcp_noblock_recv(nSock, (unsigned char *)pszMsg+iRcvLen, sizeof(pszMsg), 16, timeout_ms);
		if(nRecvSize<0)
		{
			break;
		}
		iRcvLen +=nRecvSize;
	}
	
	printf("http rcv nRecvSize :%d\n", nRecvSize);
	printf("http rcv %s\n", pszMsg);


	sscanf(pszMsg+strlen("HTTP/"), "%d.%d %d %s", &nHttpVer1, &nHttpVer2, &nHttpStatus, szHttpStatus);
	printf("http ver %d.%d, httpStatus %d %s\n", nHttpVer1, nHttpVer2, nHttpStatus, szHttpStatus);
	if (nHttpStatus != 200)
	{
		nRet =  -1;
		goto leave;
	}
	nRet = 0;
leave:
	if(-1!=nSock)
	{
		gsf_close_socket(&nSock);
	}
	if(pszMsg)
	{
		free(pszMsg);
		pszMsg = NULL;
	}

	return nRet;
}


int upload_picture(char* FilePath)
{
    FILE* fd = fopen(FilePath, "rb");
    if(!fd)
    {
        perror("fopen");
        exit(1);
    }

    fseek(fd, 0, SEEK_END);
    int length = ftell(fd);
    printf("the file size %d\n", length);
    fseek(fd, 0, SEEK_SET);
    char *context = malloc(length);
    if(context == NULL)
    {
        printf("malloc fail!\n");
        exit(1);
    }

    fread(context, 1, length, fd);
    char* encode = malloc(1024*1024*2);
    gsf_base64_encode(context, length, encode, 1024*1024*2);
    printf("the length %d,encode length %ld\n", length, strlen(encode));
    time_t tm;
    struct tm *utc;
 
    time(&tm);
    /* 获取 GMT 时间 */
    utc = gmtime(&tm);
    char timestring[50] = {0};
    sprintf(timestring, "%4d-%02d-%02d %02d:%02d:%02d", utc->tm_year+1970, utc->tm_mon+1, utc->tm_mday, utc->tm_hour, utc->tm_min, utc->tm_sec);
    cJSON *request = NULL;
    request = cJSON_CreateObject();
    cJSON_AddStringToObject(request, "cameraId", g_puid);
    cJSON_AddStringToObject(request, "snapTime", timestring);
    cJSON_AddStringToObject(request, "snapImage", encode);
    cJSON_AddNumberToObject(request, "longitude", 0);
    cJSON_AddNumberToObject(request, "latitude", 0);
    cJSON_AddNumberToObject(request, "intelType", 2);
    cJSON *array = cJSON_CreateArray();
    cJSON *item = cJSON_CreateObject();
    cJSON_AddStringToObject(item, "intelName", "EQplay");
    cJSON_AddStringToObject(item, "smallImage", encode);
    cJSON_AddItemToArray(array, item);
    cJSON_AddItemToObject(request, "intelInfor", array);

    char* msg = cJSON_PrintUnformatted(request);

	if(gsf_http_post("106.15.206.68",28080,10000,msg,strlen(msg))!=0)
	{
		printf("gsf_http_post\n");	
	}
	fclose(fd);
    free(context);
    free(encode);
    cJSON_free(msg);
    fd = NULL;    
}

static int my_printf(char* pszfmt, ...)
{
        va_list struAp;
        time_t now;

        char tmp[64];   

        int ret = 0;
        char format[512];
    
        if(pszfmt == NULL) 
        {
            return -1;
        }

		if((strlen(pszfmt)+3)>512)
		{
			return -2;
		}
		memset(format,0,sizeof(format)); 
        strcpy(format,pszfmt);
        if(format[strlen(pszfmt)-1]=='\n')
        {
            format[strlen(pszfmt)] = format[strlen(pszfmt)-2]=='\r' ?'\0':'\r';
        }
        else if(format[strlen(pszfmt)-1]=='\r')
        {
            format[strlen(pszfmt)] = format[strlen(pszfmt)-2]=='\n' ?'\0':'\n';
        }
        else
        {
            format[strlen(pszfmt)] ='\r';
            format[strlen(pszfmt)+1] ='\n';
        }

        
        #ifdef _WIN32
        now= time(0);   
        strftime( tmp, sizeof(tmp), "[%Y/%m/%d %X]",localtime(&now)); 
        printf("%s",tmp);
        #else
        struct tm tm_now;
        now=time(&now);
		localtime_r(&now, &tm_now); 
        printf("[%04d/%02d/%02d %02d:%02d:%02d]",
            tm_now.tm_year+1900,
            tm_now.tm_mon+1,
            tm_now.tm_mday,
            (tm_now.tm_hour)%24,
            tm_now.tm_min,
            tm_now.tm_sec);

        #endif
        va_start(struAp, pszfmt);
        ret = vprintf(format, struAp);
        va_end(struAp);
        return ret;
}







typedef struct _dc_talk_hdl_s_
{
	int talkHdl;
	pthread_t thId;
	int res[3];
}dc_talk_hdl_t;

typedef struct _dc_upg_hdl_s_
{
	int totalSize;
	int leftSize;
	int percent;
	int res;
}dc_upg_hdl_t;

typedef struct _dc_param_s_
{
    BYTE valid;
    BYTE exit;
    BYTE res[2];
    DWORD type;
	Data_FIFO *m_pDataFiFo;	

    NET_DEV_DATACHN *dc;
	union {
		dc_talk_hdl_t talk;
		dc_upg_hdl_t  upg;
	}hdl;
}dc_param_t;






int sadp_callback(struct _sadp_pu_notify_s *handle
            , sadp_pu_parm_t *parm)
{
    if (parm == NULL)
        return -1;

    if (parm->args == SADP_MSG_GET)
    {
        if (parm->id == SADP_MSG_ID_DEV_INFO)
        {
            if (*parm->size < sizeof(NET_DEV_SADPINFO))
                return -1;
            *parm->size = sizeof(NET_DEV_SADPINFO);
            NET_DEV_SADPINFO *sadp = (NET_DEV_SADPINFO *)parm->data;
            sadp->DevType = NET_DEV_TYPE_IPC;
			strcpy(sadp->DevName, DEV_NAME);
			strcpy(sadp->ProductName, PRODUCT_NAME);
			strcpy(sadp->SerialNo, SERIAL_NO);
			strcpy(sadp->SoftwareVer, SW_VER);
			strcpy(sadp->HardwareVer, SW_VER);
            sadp->ProtolType = (1<<NET_DEV_PROTOL_ME);//|(1<<NET_DEV_PROTOL_ONVIF);
            sadp->EncChnCount = 1;
            sadp->EthCfg.NICCount = 1;
            strcpy(sadp->EthCfg.EthNICs[0].IPAddr.IpV4, IPV4_ADDR);
			strcpy(sadp->EthCfg.EthNICs[0].Netmask.IpV4, IPV4_NETMASK);
			strcpy(sadp->EthCfg.EthNICs[0].Gateway.IpV4, IPV4_GATEWAY);
			strcpy(sadp->EthCfg.EthNICs[0].DNS1.IpV4, IPV4_DNS1);
			strcpy(sadp->EthCfg.EthNICs[0].DNS2.IpV4, IPV4_DNS2);
            sadp->EthCfg.EthNICs[0].EnableDHCP = 0;
            sadp->EthCfg.EthNICs[0].EnableAutoDNS = 0;
            sadp->EthCfg.EthNICs[0].Interface = 4;  // 100M
			strcpy(sadp->EthCfg.EthNICs[0].MACAddr, IPV4_MAC);
            sadp->ServiceCfg.MsgPort =  MSG_PORT;
            sadp->ServiceCfg.RtspPort = RTSP_PORT;
            sadp->ServiceCfg.HttpPort = HTTP_PORT;
            sadp->ServiceCfg.RtmpPort = RTMP_PORT;
            NET_DEV_SADPINFO_HTON(sadp);
        }
    }
    else if (parm->args == SADP_MSG_SET)
    {
        if (parm->id == SADP_MSG_ID_NET_CFG)
        {
            if (*parm->size < sizeof(NET_DEV_MODIFY_NET))
                return -1;
            NET_DEV_MODIFY_NET *net = (NET_DEV_MODIFY_NET *)parm->data;
            NET_DEV_MODIFY_NET_NTOH(net);
            printf("\n\nmodify net: nic[0]\n"
                "\tdhcp %d, ipv4 %s, netmask %s, gw %s\n"
                "\tdautoDns %d, ns1 %s, dns2 %s, mac[%02hhx:%02hhx:%02hhx:%02hhx:%02hhx:%02hhx]\n"
                "\tmsgPort %d, rtspPort %d, httpPort %d, rtmpPort %d\n\n"
                , net->EthCfg.EthNICs[0].EnableDHCP
                , net->EthCfg.EthNICs[0].IPAddr.IpV4
                , net->EthCfg.EthNICs[0].Netmask.IpV4
                , net->EthCfg.EthNICs[0].Gateway.IpV4
                , net->EthCfg.EthNICs[0].EnableAutoDNS
                , net->EthCfg.EthNICs[0].DNS1.IpV4
                , net->EthCfg.EthNICs[0].DNS2.IpV4
                , net->EthCfg.EthNICs[0].MACAddr[0]
                , net->EthCfg.EthNICs[0].MACAddr[1]
                , net->EthCfg.EthNICs[0].MACAddr[2]
                , net->EthCfg.EthNICs[0].MACAddr[3]
                , net->EthCfg.EthNICs[0].MACAddr[4]
                , net->EthCfg.EthNICs[0].MACAddr[5]
                , net->ServiceCfg.MsgPort
                , net->ServiceCfg.RtspPort
                , net->ServiceCfg.HttpPort
                , net->ServiceCfg.RtmpPort);
            NET_DEV_MODIFY_NET_HTON(net);
        }
    }
    return 0;
}


static int
gsf_sdk_demo_set_param(NET_DEV_PEER *who, int chn, int pid, void *in, void *out)
{
    printf("%d, %d, %s\n", chn, pid, __FUNCTION__);
    switch(pid)
    {

		case NET_DEV_PARAM_VENC_OSD:
		{
            NET_DEV_VENC_OSD *osdCfg = (NET_DEV_VENC_OSD *)in;
            printf("Set Osd: %s\n",osdCfg->Texts[0].TextData);
			if(strcmp(osdCfg->Texts[0].TextData,"????")==0)
			{
				system("rm /tmp/90\r\n");
				system("touch /tmp/0\r\n");
			}		
			else if(strcmp(osdCfg->Texts[0].TextData,"????")==0)
			{				
				system("rm /tmp/90\r\n");
				system("touch /tmp/0\r\n");
			}
			else if(strcmp(osdCfg->Texts[0].TextData,"????")==0)
			{
				system("rm /tmp/0\r\n");
				system("touch /tmp/90\r\n");
			}
            return 0;
		}

        case NET_DEV_PARAM_NET:
        {
            NET_DEV_ETHCFG *ethCfg = (NET_DEV_ETHCFG *)in;
            printf("Set EthCfg: dhcp %d, autoDNS %d\n",
                ethCfg->EthNICs[0].EnableDHCP, ethCfg->EthNICs[0].EnableAutoDNS);
            printf("Ip %s, netmask %s, gateway %s, dns1 %s, mac %02X:%02X:%02X:%02X:%02X:%02X\n",
                ethCfg->EthNICs[0].IPAddr.IpV4, ethCfg->EthNICs[0].Netmask.IpV4,
                ethCfg->EthNICs[0].Gateway.IpV4, ethCfg->EthNICs[0].DNS1.IpV4,
                ethCfg->EthNICs[0].MACAddr[0], ethCfg->EthNICs[0].MACAddr[1],
                ethCfg->EthNICs[0].MACAddr[2], ethCfg->EthNICs[0].MACAddr[3],
                ethCfg->EthNICs[0].MACAddr[4], ethCfg->EthNICs[0].MACAddr[5]);
            return 0;
        }
        case NET_DEV_PARAM_ALARM_JOINT:
        {
            NET_DEV_ALARM_JOINT *ajoint = (NET_DEV_ALARM_JOINT *)in;
            printf("Set AlarmJoint: alarmType %d, alarmNo %d\n",
                ajoint->AlarmSource.AlarmType, ajoint->AlarmSource.AlarmNo);
            switch(ajoint->AlarmSource.AlarmType)
            {
                case NET_DEV_ALARM_TYPE_MOTION:
                case NET_DEV_ALARM_TYPE_LOST:
                case NET_DEV_ALARM_TYPE_HIDE:
                case NET_DEV_ALARM_TYPE_IO:
                case NET_DEV_ALARM_TYPE_PIR:
                case NET_DEV_ALARM_TYPE_AVD:
                case NET_DEV_ALARM_TYPE_PEA_OSC:
                    if (ajoint->Process.TypeMask & (1<<NET_DEV_ALARM_HANDLE_TYPE_BEEP))
                        printf("Set Alarm Linkage Beep\n");
                    if (ajoint->Process.TypeMask & (1<<NET_DEV_ALARM_HANDLE_TYPE_RECORD))
                        printf("Set Alarm Linkage Recode chn_0 mask %d\n", ajoint->Process.RecordChannMask[0]);
                    if (ajoint->Process.TypeMask & (1<<NET_DEV_ALARM_HANDLE_TYPE_ALARMOUT))
                        printf("Set Alarm Linkage Output, output_0 mask %d\n", ajoint->Process.AlarmOutMask[0]);
                    if (ajoint->Process.TypeMask & (1<<NET_DEV_ALARM_HANDLE_TYPE_ALARMHOST))
                        printf("Set Alarm Linkage AlarmHost\n");
                    if (ajoint->Process.TypeMask & (1<<NET_DEV_ALARM_HANDLE_TYPE_SNAPPIC))
                        printf("Set Alarm Linkage snap-pic chn_0 mask %d\n", ajoint->Process.SnapChannMask[0]);
                    if (ajoint->Process.TypeMask & (1<<NET_DEV_ALARM_HANDLE_TYPE_PTZ))
                        printf("Set Alarm Linkage PTZ, chn_0 ptz action %d, no %d\n",
                            ajoint->Process.Ptzs[0].Action, ajoint->Process.Ptzs[0].No);
                    if (ajoint->Process.TypeMask & (1<<NET_DEV_ALARM_HANDLE_TYPE_EMAIL))
                        printf("Set Alarm Linkage Email\n");
                    return 0;
                default:
                    break;
            }
        }
    }

    return 0;
    //return -101;
}


static int
gsf_sdk_demo_get_param(NET_DEV_PEER *who, int chn, int pid, void *in, void *out)
{
    int ret = 0;
    //if(NET_DEV_PARAM_ALARM_JOINT == chn)
    //    ret = -1;
    printf("%d, %d, %s, ret:%d\n", chn, pid, __FUNCTION__, ret);
    switch(pid)
    {
        case NET_DEV_PARAM_VERSION:
        {
            NET_DEV_VERSION *ver = (NET_DEV_VERSION *)out;
            ver->DevType = NET_DEV_TYPE_IPC;
			strcpy(ver->ProductName, PRODUCT_NAME);
			strcpy(ver->SerialNo, SERIAL_NO);
			strcpy(ver->SoftwareVer, SW_VER);
            strcpy(ver->SoftwareDate, VER_DATE_TIME);
			strcpy(ver->DspSoftwareVer, DSP_SW_VER);
            strcpy(ver->DspSoftwareDate, VER_DATE_TIME);
			strcpy(ver->PanelVer, PAN_VER);
			strcpy(ver->HardwareVer, HW_VER);
            return 0;
        }
        case NET_DEV_PARAM_SERVICECFG:
        {
            NET_DEV_SERVICECFG *serv = (NET_DEV_SERVICECFG *)out;
            serv->MsgPort = MSG_PORT;
            serv->RtspPort = RTSP_PORT;
            serv->HttpPort = HTTP_PORT;
            serv->RtmpPort = RTMP_PORT;
            return 0;
        }
        case NET_DEV_PARAM_ALARM_JOINT:
        {
            NET_DEV_PARAM_QUERY *query = (NET_DEV_PARAM_QUERY *)in;
            NET_DEV_ALARM_JOINT *ajoint = (NET_DEV_ALARM_JOINT *)out;
            if (query == NULL || query->ParamId != pid)
            {
                printf("Invalid input param %p\n", in);
                return (-1);
            }
            ajoint->AlarmSource.AlarmType = query->Param.AlarmSource.AlarmType;
            ajoint->AlarmSource.AlarmNo = query->Param.AlarmSource.AlarmNo;
            ajoint->Process.TypeMask = 0;
            switch(ajoint->AlarmSource.AlarmType)
            {
                case NET_DEV_ALARM_TYPE_MOTION:
                case NET_DEV_ALARM_TYPE_LOST:
                case NET_DEV_ALARM_TYPE_HIDE:
                case NET_DEV_ALARM_TYPE_IO:
                case NET_DEV_ALARM_TYPE_PIR:
                case NET_DEV_ALARM_TYPE_AVD:
                case NET_DEV_ALARM_TYPE_PEA_OSC:
                    ajoint->Process.TypeMask |= (1<<NET_DEV_ALARM_HANDLE_TYPE_RECORD);
                    ajoint->Process.RecordChannMask[0] = 1;

                    ajoint->Process.TypeMask |= (1<<NET_DEV_ALARM_HANDLE_TYPE_ALARMOUT);
                    ajoint->Process.AlarmOutMask[0] = 1;

                    ajoint->Process.TypeMask |= (1<<NET_DEV_ALARM_HANDLE_TYPE_PTZ);
                    ajoint->Process.Ptzs[0].Action = PTZ_ACTION_CALL_PRESET;
                    ajoint->Process.Ptzs[0].No = 5;
                    return 0;
                default:
                    break;
            }

            return 0;
        }
    }
    //return 0;
    return -100;
}

static int gsf_sdk_demo_get_sys_time(NET_DEV_TIME_EX *time_ex)
{
    time_t t_time = 0;
    struct tm *ptm = NULL;
    time(&t_time);
    if ((ptm = gmtime(&t_time)) != NULL)
    {
        time_ex->Zone = 0;
        time_ex->Year = ptm->tm_year + 1900;
        time_ex->Month = (ptm->tm_mon % 12) + 1;
        time_ex->Date = ptm->tm_mday;
        time_ex->Hour = ptm->tm_hour;
        time_ex->Minute = ptm->tm_min;
        time_ex->Second = ptm->tm_sec;
        time_ex->Day = ptm->tm_wday;
    }
    return 0;
}


int g_iStreeStat = 0;
static int
gsf_sdk_demo_post_msg(NET_DEV_PEER *who, int chn, int pid, void *in, void *out)
{
	int i=0;
    printf("=>RECEIVE POST MSG(), %d, %d, %s\n", chn, pid, __FUNCTION__);
    switch(pid)
    {
        case NET_DEV_MSG_LOGIN:
        {
            NET_DEV_LOGIN_REQ *req = (NET_DEV_LOGIN_REQ *)in;
            NET_DEV_LOGIN_RSP *rsp = (NET_DEV_LOGIN_RSP *)out;
            if (req == NULL)
            {
                printf("login req param invalid %p\n", req);
                return (-1);
            }
            if (strcmp(req->User.UserName, "admin") || strcmp(req->User.PassWord, "12345"))
            {
                printf("login req user or password err\n");
                return (-2);
            }
            memset(rsp, 0, sizeof(NET_DEV_LOGIN_RSP));
            strncpy(rsp->UserInfo.UserName, req->User.UserName, NET_DEV_USERNAME_LEN - 1);
            strncpy(rsp->UserInfo.PassWord, req->User.PassWord, NET_DEV_PASSWORD_LEN - 1);
            rsp->UserInfo.LocalRight = 0xFFFFFFFF;
            rsp->UserInfo.RemoteRight = 0xFFFFFFFF;
            return 0;
        }
        case NET_DEV_MSG_CAPABILITIES:
        {
            NET_DEV_CAPABILITIES *pCap = (NET_DEV_CAPABILITIES *)out;
            memset(out, 0, sizeof(NET_DEV_CAPABILITIES));
            strcpy((char *)pCap->ProductName, "ipc-company");
            pCap->DevType = NET_DEV_TYPE_IPC;
            pCap->EncChnCount = 1;
            pCap->DecChnCount = 0;
            pCap->AudioInCount = 1;
            pCap->AudioOutCount = 1;
            pCap->AlarmInCount = 1;
            pCap->AlarmOutCount = 1;
            pCap->Rs232Count = 1;
            pCap->Rs485Count = 1;
            pCap->EthCount = 1;
            pCap->DiskCount = 1;
            pCap->CruisesPerChn = NET_DEV_MAX_CRZ_NUM;
            pCap->PatternPerChn = NET_DEV_PTZ_MAX_PTN_NUM;
            pCap->SystemCap = (1<<DEV_SYS_CAP_PTZ) | (1<<DEV_SYS_CAP_IR);
            pCap->ProtolCap = (1<<NET_DEV_PROTOL_ME) | (1<<NET_DEV_PROTOL_ONVIF);
            pCap->InProtolCap = 0;
            pCap->IvsCap = 0;
            pCap->ChannelCap.OverlayCount = NET_DEV_MAX_AREA_NUM;
            pCap->ChannelCap.MotionAlarmCount = NET_DEV_MAX_AREA_NUM;
            pCap->ChannelCap.HideAlarmCount = NET_DEV_MAX_AREA_NUM;
            pCap->ChannelCap.OsdCount = NET_DEV_OSD_TEXT_NUM;
            pCap->ChannelCap.LostAlarm = 0;
            pCap->ChannelCap.StreamCount = 2;
            pCap->ChannelCap.VideoEncType = (1<<NET_DEV_VENC_H264) | (1<<NET_DEV_VENC_JPEG);
            pCap->ChannelCap.AudioEncType = (1<<NET_DEV_AENC_G711A);
            pCap->ChannelCap.Ima = IMA_CAP;
            pCap->ChannelCap.Resolution[0] = IMA_RESOLUTION_0;
            pCap->ChannelCap.Resolution[1] = IMA_RESOLUTION_1;
            pCap->ChannelCap.FPS[0] = 25;
            pCap->ChannelCap.FPS[1] = 25;
            return 0;
        }
        case NET_DEV_MSG_MEDIA_URL:
        {
            NET_DEV_MEDIA_URL *req = (NET_DEV_MEDIA_URL *)in;
            NET_DEV_MEDIA_URL *rsp = (NET_DEV_MEDIA_URL *)out;
            if (req == NULL || rsp == NULL)
            {
                printf("Invalid Param Input %p, Output %p\n", in, out);
                return -1;
            }
            rsp->StreamType = req->StreamType;
            rsp->MediaType  = req->MediaType;

            if (req->IpAddr.IpV4[0])
                memcpy(&rsp->IpAddr, &req->IpAddr, sizeof(NET_DEV_IPADDR));
            else
            {
                strcpy(rsp->IpAddr.IpV4, IPV4_ADDR);
            }
            sprintf(rsp->Url, "rtsp://%s:%d/%d/%d/%d", rsp->IpAddr.IpV4, RTSP_PORT, 0, 0, 0);
            return 0;
        }

        case NET_DEV_MSG_KEEPALIVE:
        {
            NET_DEV_KEEPALIVE_RSP *rsp = (NET_DEV_KEEPALIVE_RSP *)out;
            gsf_sdk_demo_get_sys_time(&rsp->TimeEx);
            return 0;
        }
        #if 1
        case NET_DEV_MSG_RECORD_QUERY:
        {
            int i = 0, sessionId = 0;
            NET_DEV_RECORD_QUERY_REQ *req = (NET_DEV_RECORD_QUERY_REQ *)in;
            NET_DEV_RECORD_QUERY_RSP *rsp = (NET_DEV_RECORD_QUERY_RSP *)out;
            if (req == NULL || rsp == NULL)
            {
                printf("Invalid Param Input %p, Output %p\n", in, out);
                return (-1);
            }
            printf("record query condition:\n");
            printf("dtype %d, recType %d, beginNo %d, reqCount %d, sessId %d, beginTime[%u], endTime[%u]\n",
                req->Cond.DetailType, req->Cond.RecordType,req->BeginNo, req->ReqCount,req->SessionId,
                req->Cond.BeginTime,
                req->Cond.EndTime);
            // fill query result
            if (req->SessionId > 0) // use valid sessionId's result set
            {
                printf("use for valid sessionId %d\n", req->SessionId);
            }
            else
            {
                // query record and get result, use sessionId
                sessionId = 1;
                printf("query record success, sessionId %d\n", sessionId);
            }
            rsp->SessionId = sessionId;
            rsp->TotalCount = 110;
            rsp->RspCount = 0;
            for (i = req->BeginNo; i < req->BeginNo + req->ReqCount; i++)
            {
                rsp->Items[rsp->RspCount].RecordType = NET_DEV_RECORD_TIMER;
                rsp->Items[rsp->RspCount].RecordSize = 128*1024*1024;
                memcpy(&rsp->Items[rsp->RspCount].BeginTime, &req->Cond.BeginTime, sizeof(req->Cond.BeginTime));
                memcpy(&rsp->Items[rsp->RspCount].EndTime, &req->Cond.EndTime, sizeof(req->Cond.EndTime));
                sprintf((char *)rsp->Items[rsp->RspCount].Property, RECORD_PROPERTY_FMT, \
                    rsp->RspCount, rsp->RspCount*rsp->Items[rsp->RspCount].RecordSize,
                    rsp->Items[rsp->RspCount].BeginTime,
                    rsp->Items[rsp->RspCount].EndTime);
                rsp->RspCount++;
                if (rsp->RspCount >= 100)//to avoid result-data overflow, value 100 for indication only
                    break;
            }
            return 0;
        }
        #endif

        case NET_DEV_MSG_PF_CMSTIME:
        {
            NET_DEV_PF_CMSTIME *cst = (NET_DEV_PF_CMSTIME*)in;
            printf("====> SET CMS TIME: %u\n", cst->UTCTime);
            return 0;
        }

        case NET_DEV_MSG_PF_GETCHNLSTATE:
        {
            NET_DEV_PF_CHNLSTATE *chnl_st = (NET_DEV_PF_CHNLSTATE*)out;
            chnl_st->CH00TO31 = 1;
            chnl_st->CH32TO63 = 10;
            printf("====> GET CHNL ST\n");
            return 0;
        }
		case NET_DEV_MSG_PTZ_CONTROL:
		{
			NET_DEV_PTZ_CONTROL *pNetDevPtzControl = (NET_DEV_PTZ_CONTROL*)in;;
            printf("====> NET_DEV_MSG_PTZ_CONTROL Action %d Flag:%d\n",pNetDevPtzControl->Action,pNetDevPtzControl->Flag);

			if(pNetDevPtzControl->Action==NET_DEV_PTZ_ID_SET_PP)
			{


				
			}

			//????Ԥ??λ
			else if(pNetDevPtzControl->Action==50)
			{
				int temp = 0;
				wrapper_iotdev_camera_stop();
				sleep(1);
				pthread_cancel(thread[0]);
                printf("#################Begin take photo!\n");
                system("TakePicture");
                usleep(100 * 1000);
                printf("#################End take photo!\n");
                upload_picture("/storage/PIC_teeTimeteeTime1.jpg");
				sleep(1);
				if((temp = pthread_create(&thread[0], NULL, stream_thread, NULL)) != 0){    
					printf("stream thread create failed!\n");                               
					return -1;                                                              
				} 

            }

			//?ӻ?????
			else if(pNetDevPtzControl->Action==50)
			{
	
			}
		}

    }
    return 0;
}

#define G711A_FRAME_SIZE 160
void *gsf_sdk_demo_dc_talk_task(void *arg)
{
    dc_param_t *param = (dc_param_t *)arg;
    if (param == NULL)
        return NULL;

	param->m_pDataFiFo=  Make_FIFO(25,MemFree);
	if(NULL==param->m_pDataFiFo)
	{ 	       
		printf("gsf_sdk_demo_dc_talk_task  MediaDataFIFO_MakeFifo err %s\n");
        return NULL;
	}	
	g_AudioTlak = param->m_pDataFiFo;
	
    int ts = time(NULL);
    int nLen = 0;
    int seq = 0;
    char data[512] = {0};
	frame_element_t * pMyFrame =NULL;

    printf("start talk task thrd proc\n");
    while(param->exit == 0)
    {
        gsf_data_fmt_t *fmt = (gsf_data_fmt_t *)data;
        gsf_data_talk_frame_t *pFrame = (gsf_data_talk_frame_t *)fmt->data;
        int prefix_size = sizeof(gsf_data_fmt_t)+sizeof(gsf_data_talk_frame_t);
		#if 0
        if ((nLen = fread(pFrame->data, 1, G711A_FRAME_SIZE, fp)) != G711A_FRAME_SIZE)
        {
            printf("fread talk data file %s err %s\n", TEST_TALK_FILE, strerror(errno));
            fseek(fp, 0, SEEK_SET);
            continue;
        }
        else if (feof(fp))
        {
            //printf("test talk data end\n");
            fseek(fp, 0, SEEK_SET);
            continue;
        }
		#endif
		if(param->m_pDataFiFo)
		{
			pMyFrame = GetDataFromFiFo(param->m_pDataFiFo);
		}
		if(NULL==pMyFrame)
		{	
			usleep(18000);            
			continue;
		}
		nLen = pMyFrame->m_iDataSize;
		//printf("start talk task thrd proc  nLen:%d\n",nLen);

		memcpy(pFrame->data,pMyFrame->m_bData,nLen);
		
        fmt->id  = GSF_DATA_TRANS_ID_TALK_DATA;
        fmt->dir = GSF_DATA_TRANS_DIR_NONE;
        fmt->_rt = 0;
        fmt->seq = seq;
        fmt->size=sizeof(gsf_data_talk_frame_t) + nLen;
        pFrame->no = seq++;
        pFrame->type = NET_DEV_MEDIA_ENC_TYPE_G711A;
        pFrame->ts = ts;
        pFrame->size = nLen;
        // nLen use for send
        nLen =  fmt->size + sizeof(gsf_data_fmt_t);
        GSF_DATA_FMT_HTON(fmt);
        GSF_DATA_TALK_FRAME_HTON(pFrame);
        if (NET_DEV_SDK_DC_SEND(param->dc, fmt, nLen) != 0)
        {
            printf("send talk data err\n");
        }
			
		if(pMyFrame)
		{			
			MemFree(pMyFrame);
			pMyFrame=NULL;
		}	
        usleep(10);
    };
	if(param->m_pDataFiFo)
	{
		g_AudioTlak = NULL;
		FIFOClear(param->m_pDataFiFo);
		free(param->m_pDataFiFo);
		param->m_pDataFiFo=NULL;
	}	


	if(g_AudioProcess)
			g_AudioProcess(NULL,0,NULL);

	
    printf("talk task thrd exit\n");
    return NULL;
}

int gsf_sdk_demo_dc_talk_proc(dc_param_t *param, gsf_data_fmt_t *pData)
{
    int err = -1;
    if (pData->id == GSF_DATA_TRANS_ID_TALK_REQ)
    {
        printf("talk req...\n");
        gsf_data_trans_talk_req *req = (gsf_data_trans_talk_req *)(&pData->data[0]);
        // talk process task
        if (req->type == GSF_DATA_TRANS_TALK_TYPE_DUAL)
        {
            if ((err = pthread_create(&param->hdl.talk.thId, NULL, gsf_sdk_demo_dc_talk_task, param)) < 0)
            {
                printf("Talk Thread ch %d Create err %d\n", req->ch, err);
                return err;
            }
        }
        param->valid = 1;
        // response
        pData->id = GSF_DATA_TRANS_ID_TALK_RSP;
        pData->dir = GSF_DATA_TRANS_DIR_RSP;
        pData->size = sizeof(gsf_data_trans_talk_rsp);
        ((gsf_data_trans_talk_rsp *)pData->data)->err = 0;
        GSF_DATA_FMT_HTON(pData);
        GSF_DATA_TRANS_TALK_RSP_HTON((gsf_data_trans_talk_rsp *)pData->data);
        if ((err = NET_DEV_SDK_DC_SEND(param->dc, pData, sizeof(gsf_data_fmt_t) + sizeof(gsf_data_trans_talk_rsp))) != 0)
        {
            printf("talk req => send resp err %d\n", err);
            return err;
        }
        return 0;
    }
    if (pData->id == GSF_DATA_TRANS_ID_TALK_DATA)
    {
        // decode audio and send to output
        gsf_data_talk_frame_t *frm = (gsf_data_talk_frame_t *)pData->data;
        GSF_DATA_TALK_FRAME_NTOH(frm);
        if (frm->size != G711A_FRAME_SIZE)
        {
            printf("invalid g711a audio data size %d\n", frm->size);
            return (-1);
        }
		#if 1
		if(g_AudioProcess)
			g_AudioProcess(frm->data,frm->size,NULL);
		#endif
        return 0;
    }

    printf("Invalid talk id %d !!\n", pData->id);
    return err;
}

int gsf_sdk_demo_dc_upg_proc(dc_param_t *param, gsf_data_fmt_t *pData)
{
    int err = -1;
    if (pData->id == GSF_DATA_TRANS_ID_UPG_REQ)
    {
        printf("upg req...\n");
        gsf_data_trans_upg_req *req = (gsf_data_trans_upg_req *)(&pData->data[0]);
		GSF_DATA_TRANS_UPG_REQ_NTOH(req);
        param->valid = 1;
		param->hdl.upg.totalSize = param->hdl.upg.leftSize = req->upg_file_size;
		param->hdl.upg.percent = 0;
        // response
        pData->id = GSF_DATA_TRANS_ID_UPG_RSP;
        pData->dir = GSF_DATA_TRANS_DIR_RSP;
        pData->size = sizeof(gsf_data_trans_upg_rsp);
        ((gsf_data_trans_upg_rsp *)pData->data)->err = 0;
        GSF_DATA_FMT_HTON(pData);
        GSF_DATA_TRANS_UPG_RSP_HTON((gsf_data_trans_upg_rsp *)pData->data);
        if ((err = NET_DEV_SDK_DC_SEND(param->dc, pData, sizeof(gsf_data_fmt_t) + sizeof(gsf_data_trans_upg_rsp))) != 0)
        {
            printf("ERROR: => send upg req resp err %d\n", err);
            return err;
        }
        return 0;
    }
    else if (pData->id == GSF_DATA_TRANS_ID_UPG_DATA)
    {
        
		param->hdl.upg.leftSize -= pData->size;
		param->hdl.upg.percent = 100-100*(param->hdl.upg.leftSize*1.0)/(param->hdl.upg.totalSize*1.0);
		if (param->hdl.upg.leftSize <= 0)
			param->hdl.upg.percent = 100;
		printf("----debug-->upgrade debug data size %d ok, left %d, percent %d <---debug---\n", 
			pData->size, param->hdl.upg.leftSize, param->hdl.upg.percent);
        return 0;
    }
	else if (pData->id == GSF_DATA_TRANS_ID_UPG_PERCENT)
	{
		pData->id = GSF_DATA_TRANS_ID_UPG_PERCENT;
		pData->dir = GSF_DATA_TRANS_DIR_RSP;
		pData->_rt = 0;
		pData->size = sizeof(gsf_data_upg_percent_rsp);
		((gsf_data_upg_percent_rsp *)pData->data)->percent = param->hdl.upg.percent;
		GSF_DATA_FMT_HTON(pData);
	    GSF_DATA_TRANS_UPG_PERCENT_RSP_HTON((gsf_data_upg_percent_rsp *)pData->data);
		if ((err = NET_DEV_SDK_DC_SEND(param->dc, pData, sizeof(gsf_data_fmt_t) + sizeof(gsf_data_upg_percent_rsp))) != 0)
			printf("ERROR: => upgrade send percent %d err %d\n", param->hdl.upg.percent, err);
		return err;
	}
	
    printf("Invalid upg id %0#x !!\n", pData->id);
    return err;
}


static int
gsf_sdk_demo_dc_open (NET_DEV_DATACHN *Dc)
{
    printf("DC : %p open ...\n", Dc);

    dc_param_t *dc_param = (dc_param_t *)calloc(sizeof(dc_param_t), 1);
    if (dc_param == NULL)
        return -1;
    dc_param->dc = Dc;
    NET_DEV_SDK_DC_SET_UDATA(Dc, dc_param);
    return 0;
}


static int
gsf_sdk_demo_dc_recv (NET_DEV_DATACHN *Dc, void *Data, int Size)
{
    int err = -1;
    //printf("DC : %p recv: %p, size:%d\n", Dc, Data, Size);
    dc_param_t *dc_param = (dc_param_t *)NET_DEV_SDK_DC_GET_UDATA(Dc);
    if (dc_param == NULL)
        return err;
    gsf_data_fmt_t *pData = (gsf_data_fmt_t *)Data;
    GSF_DATA_FMT_NTOH(pData);
    if (pData->id == GSF_DATA_TRANS_ID_KEEPALIVE)
    {
        GSF_DATA_FMT_HTON(pData);
        if ((err = NET_DEV_SDK_DC_SEND(Dc, pData, sizeof(gsf_data_fmt_t))) != 0)
            printf("Error: send keepalive resp err %d\n", err);
        return err;
    }
    err = -1;
    dc_param->type = GSF_DATA_TRANS_TYPE_GET(pData->id);
    switch(GSF_DATA_TRANS_TYPE_GET(pData->id))
    {
        case GSF_DATA_TRANS_TYPE_UPG:
            err = gsf_sdk_demo_dc_upg_proc(dc_param, pData);    // upg process
            break;

        case GSF_DATA_TRANS_TYPE_TALK:
            err = gsf_sdk_demo_dc_talk_proc(dc_param, pData);
            break;

        case GSF_DATA_TRANS_TYPE_DOWN:
            err = 0;    // refer to talk: send capture audio case
            break;

        case GSF_DATA_TRANS_TYPE_SNAP:
            err = 0;    // refer to talk: send capture audio case
            break;

        default:
            printf("Error: Unexpected type %d\n", GSF_DATA_TRANS_TYPE_GET(pData->id));
            break;
    }

    return err;
}


static void
gsf_sdk_demo_dc_close (NET_DEV_DATACHN *Dc)
{
    printf("DEBUG: => %s : DC %p closed ...\n", __FUNCTION__, Dc);
    dc_param_t *param = (dc_param_t *)NET_DEV_SDK_DC_GET_UDATA(Dc);
    if (param == NULL)
        return;
    param->exit = 1;
    switch(param->type)
    {
        case GSF_DATA_TRANS_TYPE_TALK:
            if (param->valid)
            {
                param->valid = 0;
                pthread_join(param->hdl.talk.thId, NULL);
                printf("close talk finish\n");
            }
            break;

        default:
            break;
    }
    free(param);
    param = NULL;
    NET_DEV_SDK_DC_SET_UDATA(Dc, NULL);
}


enum {
    ST_OPEN,
    ST_PLAY,
    ST_CLOSE,
};


typedef struct _gsf_hs_param
{
    int fd;
    int frm;
    int ts;
    int state;
    NET_DEV_MEDIA *m;
    pthread_mutex_t mutex;
}gsf_hs_param_t;

//历史流接口测试
static int
gsf_sdk_demo_hs_stream_probe(int channel, int level,
    unsigned start_time, unsigned end_time, unsigned rec_type, 
    char *property, void *_sdp)
{
    NET_DEV_MEDIA_PARAM *sdp = (NET_DEV_MEDIA_PARAM*)_sdp;
    char frame[512*1024];
    int frame_type, frame_size = 0;

    int fd = file_open(TEST_FILE_PATH);
    if (fd < 0)
    {
        printf("htest probe open failed!\n");
        return -1;
    }

    sdp->BeginTime = 0;
    sdp->EndTime   = 100000;
    frame_size = file_read(fd, frame, sizeof(frame), &frame_type);
    sdp->Mask =  MEDIA_TYPE_VIDEO | MEDIA_TYPE_AUDIO | MEDIA_TYPE_METADATA;
    sdp->Video.EncodeType = (g_hevc_video)?NET_DEV_MEDIA_ENC_TYPE_H265:NET_DEV_MEDIA_ENC_TYPE_H264;
    sdp->Video.BitRate = 8000;
    sdp->Video.SamplesPerSec = NET_DEV_MEDIA_SAMPLE_RATE_90K;
    sdp->Video.FrameRate = (g_hevc_video)?60:25;
  //  nalu_hevc_get_sps_pps(frame, frame_size, &sdp->Video.PicParam);
    file_close(fd);

    return 0;
}


static int
gsf_sdk_demo_hs_stream_open(NET_DEV_MEDIA *m, int channel,
    int level, unsigned start_time, unsigned end_time, 
    unsigned rec_type, char *property)
{
    printf("hs_stream_open => time: %u => %u\n", start_time, end_time);
    gsf_hs_param_t *hs_param = calloc(1, sizeof(gsf_hs_param_t));
    hs_param->fd = file_open(TEST_FILE_PATH);
    if (hs_param->fd < 0)
    {
        return -1;
    }
    hs_param->state = ST_OPEN;
    pthread_mutex_init(&hs_param->mutex, NULL);
    NET_DEV_SDK_MEDIA_SET_UDATA(m, hs_param);
    return 0;
}


static int
gsf_sdk_demo_hs_stream_play(NET_DEV_MEDIA *m)
{
    gsf_hs_param_t *hs_param = NET_DEV_SDK_MEDIA_GET_UDATA(m);
    pthread_mutex_lock(&hs_param->mutex);
    hs_param->state = ST_PLAY;
    pthread_mutex_unlock(&hs_param->mutex);
    return 0;
}


static void
gsf_sdk_demo_hs_stream_close(NET_DEV_MEDIA *m)
{
    gsf_hs_param_t *hs_param = NET_DEV_SDK_MEDIA_GET_UDATA(m);
    if (hs_param->fd)
        file_close(hs_param->fd);
    pthread_mutex_destroy(&hs_param->mutex);
    free(hs_param);
    NET_DEV_SDK_MEDIA_SET_UDATA(m, NULL);
}


static unsigned utc_time_count = 0;


static int
gsf_sdk_demo_hs_stream_pause(NET_DEV_MEDIA *m)
{
    printf("gsf_sdk_demo_hs_stream_pause.\n");
    return 0;
}


static int
gsf_sdk_demo_hs_stream_lseek(NET_DEV_MEDIA *m, unsigned ts)
{
    printf("gsf_sdk_demo_hs_stream_lseek. ts:%d\n", ts);

    utc_time_count = ts;
    return 0;
}


static int
gsf_sdk_demo_hs_stream_pull(NET_DEV_MEDIA *m, void **_frm)
{
#define FRAME_SIZE 512*1024
    NET_DEV_MEDIA_FRAME **frm = (NET_DEV_MEDIA_FRAME **)_frm;
    gsf_hs_param_t *hs_param = NET_DEV_SDK_MEDIA_GET_UDATA(m);
    int frame_size = 0;
    int ft;
    NET_DEV_MEDIA_FRAME *fr;
    NET_DEV_MEDIA_NALDESC nd;
    nd.NalNum = 0;

    fr = (NET_DEV_MEDIA_FRAME*)NET_DEV_SDK_FRAME_ALLOC(FRAME_SIZE, 0);
    if (!fr)
        return -1;

    for (;;)
    {
        frame_size = file_read(hs_param->fd, (char*)fr->Data, FRAME_SIZE, &ft);
        if(frame_size < 0)
        {
            printf("###############file_read() failed %d, %s!\n", frame_size, strerror(errno));
            continue;
        }

        break;
    }

    fr->Hdr.No = hs_param->frm++;
    fr->Hdr.EncodeType = (g_hevc_video)?NET_DEV_MEDIA_ENC_TYPE_H265:NET_DEV_MEDIA_ENC_TYPE_H264;
    fr->Hdr.Timestamp = (g_hevc_video)?(hs_param->ts += 16):(hs_param->ts += 40); //ms
    fr->Hdr.Type = ft;
    fr->Hdr.Video.Width = 0;
    fr->Hdr.Video.Height = 0;
    fr->Hdr.Video.UTCTime = utc_time_count++;//time(NULL);
    fr->Hdr.Size = frame_size;
    *frm = fr;
    return 0;
}


static int
gsf_sdk_demo_ls_stream_probe(int Chnl, int Level, void *MediaParamIn)
{
    NET_DEV_MEDIA_PARAM *sdp = (NET_DEV_MEDIA_PARAM*)MediaParamIn;
    char frame[512*1024];
    int frame_type, frame_size = 0;

    int fd = file_open(TEST_FILE_PATH);
    if (fd < 0)
    {
        printf("htest probe open failed!\n");
        return -1;
    }
    sdp->BeginTime = 0;
    sdp->EndTime   = 100000;
    frame_size = file_read(fd, frame, sizeof(frame), &frame_type);
    sdp->Mask =  MEDIA_TYPE_VIDEO;
    sdp->Video.EncodeType = (g_hevc_video)?NET_DEV_MEDIA_ENC_TYPE_H265:NET_DEV_MEDIA_ENC_TYPE_H264;
    sdp->Video.BitRate = 8000;
    sdp->Video.SamplesPerSec = NET_DEV_MEDIA_SAMPLE_RATE_90K;
    sdp->Video.FrameRate = (g_hevc_video)?60:25;
   // nalu_hevc_get_sps_pps(frame, frame_size, &sdp->Video.PicParam);
    file_close(fd);

    return 0;
}






static int
gsf_sdk_demo_ls_ps_stream_probe(int Chnl, int Level, void *MediaParamIn)
{
    NET_DEV_MEDIA_PARAM *sdp = (NET_DEV_MEDIA_PARAM*)MediaParamIn;

	unsigned char  cTempHead[16]={0};
    FILE *m_Fp = NULL;
	int ret = 0;
	int iHead =0;
	unsigned short  u16Templen =0;
	
    sdp->BeginTime = 0;
    sdp->EndTime   = 100000;
    sdp->Mask =  MEDIA_TYPE_VIDEO|MEDIA_TYPE_AUDIO;
    sdp->Video.EncodeType = NET_DEV_MEDIA_ENC_TYPE_H264;
    sdp->Video.BitRate = 8000;
    sdp->Video.SamplesPerSec = NET_DEV_MEDIA_SAMPLE_RATE_08K;
    sdp->Video.FrameRate = g_iFrameRate;
	memcpy(&sdp->Video.PicParam,&g_PicParam[Chnl][Level],sizeof(NET_DEV_MEDIA_PICPARAM));

    sdp->Audio.EncodeType = NET_DEV_MEDIA_ENC_TYPE_G711A;
    sdp->Audio.BitRate = 8000;
    sdp->Audio.SamplesPerSec = NET_DEV_MEDIA_SAMPLE_RATE_08K;
    sdp->Audio.BitsPerSample = 16;
    sdp->Audio.ChannelNum = 2;
	sdp->Meta.Size = 0;
	printf("gsf_sdk_demo_ls_ps_stream_probe end...\n");
    return 0;
}



typedef struct _GsfLiveMedia GsfLiveMedia;
struct _GsfLiveMedia
{
    NET_DEV_MEDIA *m;
    volatile int state;
    volatile int m_iStat;
	int m_iChan;
	int m_iStream;
	unsigned short m_nSeq;
	unsigned short m_nVSeq;
	unsigned short m_nASeq;
	Data_FIFO *m_pDataFiFo;	
	volatile int m_iBind;
	STREAM_DATA_PROCESS m_FrameProcess;
    pthread_t thread_id;
    pthread_mutex_t mutex;
    GsfLiveMedia *next;
};

static Data_FIFO * g_pDataFiFo[32][2];
static void *TskMedia(void * pArgs) 
{
	int i=0;
	int index=0;
    int ret = 0;
	frame_element_t * pFrame =NULL;
	GsfLiveMedia *pMedia = (GsfLiveMedia *)pArgs;
	pMedia->m_iStat=  pMedia->m == NULL ? 1 :  2;
    while(pMedia->m_iStat)
    {
    	//暂停
		if(pMedia->m_iStat==2)
		{
			usleep(50*1000);
			continue;
		}
		if(pMedia->m_pDataFiFo)
		{
			pFrame = GetDataFromFiFo(pMedia->m_pDataFiFo);
		}
		if(pFrame)
		{	
			//printf("TskMedia pFrame\n");
			if(pMedia->m_FrameProcess)
			{
				pMedia->m_FrameProcess(pMedia,pFrame,pMedia->m);
			}
            MemFree(pFrame);      
			pFrame =NULL;	
		}
		else
		{
			usleep(10*1000);
		}
    }
	
	if(pFrame)
	{			
  		MemFree(pFrame);   
		pFrame=NULL;
	}		
	pMedia->m_FrameProcess =NULL;
	if(pMedia->m_pDataFiFo)
	{
		if(pMedia->m_iBind)
		{
			g_pDataFiFo[pMedia->m_iChan][pMedia->m_iStream] = NULL;
			pMedia->m_iBind = 0;
		}
		FIFOClear(pMedia->m_pDataFiFo);
		free(pMedia->m_pDataFiFo);
		pMedia->m_pDataFiFo=NULL;
	}		
	
	free(pMedia);
	pMedia = NULL;
    return NULL;
}



void * CreatMedia(int Chnl, int Level,STREAM_DATA_PROCESS ProcessFrame,NET_DEV_MEDIA *m)
{
	HANDLE m_pthread;
	int ret = 0;
	unsigned long long u16Templen =0;
	char cTmp[256]={0};	
	int iHead =0;
	int i =0;
	GsfLiveMedia *pMedia = NULL;
	if(g_pDataFiFo[Chnl][Level])
	{		
		goto leave;
	}

	pMedia = (GsfLiveMedia *)malloc(sizeof(GsfLiveMedia));
	if(NULL==pMedia)
	{	
		goto leave;
	}	
	
	memset(pMedia,0,sizeof(GsfLiveMedia));
	pMedia->m_pDataFiFo=  Make_FIFO(25,MemFree);
	if(NULL==pMedia->m_pDataFiFo)
	{ 	
		goto errorleave;
	}	

	pMedia->m_iChan=Chnl;
	pMedia->m_iStream=Level;
	
	g_pDataFiFo[pMedia->m_iChan][pMedia->m_iStream] = pMedia->m_pDataFiFo;
	pMedia->m_iBind=1;

	printf("CreatMedia OK\n");

    if(pthread_create(&pMedia->thread_id, NULL,TskMedia, pMedia))
    {		
    	goto errorleave;
    }
    pMedia->m = m;
	pMedia->m_FrameProcess=ProcessFrame;
	return pMedia;
errorleave:
	if(pMedia)
	{			
		pMedia->m_FrameProcess =NULL;
		if(pMedia->m_pDataFiFo)
		{
			if(pMedia->m_iBind)
			{
				g_pDataFiFo[pMedia->m_iChan][pMedia->m_iStream] = NULL; 
				pMedia->m_iBind = 0;
			}
			FIFOClear(pMedia->m_pDataFiFo);
			free(pMedia->m_pDataFiFo);
			pMedia->m_pDataFiFo=NULL;
		}		
	
		free(pMedia);
		pMedia = NULL;
	}
leave:
	return pMedia;
}





static __inline__ void
gsf_sdk_demo_set_media_state(GsfLiveMedia *media, int state)
{
    pthread_mutex_lock(&media->mutex);
    media->state = state;
    pthread_mutex_unlock(&media->mutex);
}


static __inline__ int
gsf_sdk_demo_get_media_state(GsfLiveMedia *media)
{
    int state;
    pthread_mutex_lock(&media->mutex);
    state = media->state;
    pthread_mutex_unlock(&media->mutex);
    return state;
}


static __inline__ void
gsf_sdk_demo_free_media(GsfLiveMedia *media)
{
    NET_DEV_SDK_MEDIA_UNREF(media->m);
    pthread_mutex_destroy(&media->mutex);
    free(media);
    printf("=====> ### live stream '%p' closed.\n", media);
}


static __inline__ int
gsf_sdk_demo_ls_read_file(int fd, NET_DEV_MEDIA_FRAME *fr,
    int *pno, int *ts)
{
    int frame_size;
    int ft;
    for (;;)
    {

        frame_size = file_read(fd, (char*)fr->Data, FRAME_SIZE, &ft);
        if(frame_size < 0)
        {
            continue;
        }

        break;
    }
	printf("gsf_sdk_demo_ls_read_file %02x %02x %02x %02x %02x\n",fr->Data[0],fr->Data[1],fr->Data[2],fr->Data[3],fr->Data[4]);

    fr->Hdr.No = (*pno)++;
    fr->Hdr.EncodeType = (g_hevc_video)?NET_DEV_MEDIA_ENC_TYPE_H265:NET_DEV_MEDIA_ENC_TYPE_H264;
    fr->Hdr.Timestamp =  (g_hevc_video)?(*ts += 16):(*ts += 40); //ms
    fr->Hdr.Type = ft;
    fr->Hdr.Video.Width = 0;
    fr->Hdr.Video.Height = 0;
    fr->Hdr.Video.UTCTime = time(NULL);
    fr->Hdr.Size = frame_size;
    return 0;
}




static void *
gsf_sdk_demo_ls_media_thread(void *data)
{
    int state, fd, sleep_ms, frame_no = 0, ts = 0;
    GsfLiveMedia *media = (GsfLiveMedia*)data;
    NET_DEV_MEDIA_FRAME frame_head;
    char frame_data[FRAME_SIZE];
    NET_DEV_MEDIA_NALDESC nd;
    nd.NalNum = 0;

    pthread_detach(pthread_self());
    fd = file_open(TEST_FILE_PATH);
    if (fd < 0)
    {
        NET_DEV_SDK_MEDIA_KILL(media->m);
        goto failed;
    }

    for (;;)
    {
        state = gsf_sdk_demo_get_media_state(media);
        switch (state)
        {
        case ST_OPEN:
            usleep(10*1000);
            break;

        case ST_PLAY:
            frame_head.Data = frame_data;
            frame_head.NalDesc = &nd;
            gsf_sdk_demo_ls_read_file(fd, &frame_head, &frame_no, &ts);
            NET_DEV_SDK_MEDIA_LPUSH(media->m, &frame_head);
            sleep_ms = (g_hevc_video)?16:40;
            usleep(sleep_ms *1000);
            break;

        case ST_CLOSE:
            goto failed;
            break;

        default:
            BUG();
            break;
        }
    }

failed:
    gsf_sdk_demo_free_media(media);
    return NULL;
}


static GsfLiveMedia *
gsf_sdk_demo_new_media(NET_DEV_MEDIA *m)
{
    int err;
    GsfLiveMedia *media = (GsfLiveMedia*)malloc(sizeof(*media));
    NET_DEV_SDK_MEDIA_REF(m);
    media->m = m;
    media->state = ST_OPEN;
    pthread_mutex_init(&media->mutex, NULL);

    err = pthread_create(&media->thread_id, NULL,
        gsf_sdk_demo_ls_media_thread, media);
    if (err)
    {
        gsf_sdk_demo_free_media(media);
        return NULL;
    }

    return media;
}


/* 打开实时流 */
static int
gsf_sdk_demo_ls_stream_open(NET_DEV_MEDIA *m, int Chnl, int Level)
{
    GsfLiveMedia *media = gsf_sdk_demo_new_media(m);
    if (media)
    {
        NET_DEV_SDK_MEDIA_SET_UDATA(m, media);
    }
	printf("gsf_sdk_demo_ls_stream_open end...\n");

    return 0;
}


/* 开始播放实时流 */
static int
gsf_sdk_demo_ls_stream_play(NET_DEV_MEDIA *m)
{
    GsfLiveMedia *media = NET_DEV_SDK_MEDIA_GET_UDATA(m);
    if (media)
    {
        gsf_sdk_demo_set_media_state(media, ST_PLAY);
    }	
	printf("gsf_sdk_demo_ls_stream_play end...\n");

    return 0;
}


/* 播放时控制 */
static int
gsf_sdk_demo_ls_stream_ctrl(NET_DEV_MEDIA *m, unsigned Cmd, void *Data)
{
    return 0;
}

/* 关闭实时流 */
static void
gsf_sdk_demo_ls_stream_close(NET_DEV_MEDIA *m)
{
    GsfLiveMedia *media = NET_DEV_SDK_MEDIA_GET_UDATA(m);
    if (media)
    {
        gsf_sdk_demo_set_media_state(media, ST_CLOSE);
    }
	printf("gsf_sdk_demo_ls_stream_close end...\n");

}


int MediaFrameCall(void * _pMedia,frame_element_t * pFrame,void *_pContext)
{
	
    NET_DEV_MEDIA_FRAME frame_head;
    NET_DEV_MEDIA_NALDESC nd;
	WORD EncodeType =0;	
	WORD Type =0;
	nd.NalNum = 0;
    static char cTemplet[128]={0};
	int iTemplet = 0;
    static int  sleep_ms, frame_no = 0, ts = 0;
	char cTemPlet[1024]={0};
	int iTemPlet = 160;
	GsfLiveMedia *pMedia =(GsfLiveMedia *) _pMedia;

	if(pFrame->m_iDataType==DATA_TYPE_VIDEO)
	{   
		EncodeType = pFrame->m_iEnCodeType == H264_VIDEO_ENCODER ? NET_DEV_MEDIA_ENC_TYPE_H264 : NET_DEV_MEDIA_ENC_TYPE_H265;
		if( pFrame->m_iFrameType==FRAME_TYPE_I)
		{
			Type = NET_DEV_MEDIA_FRAME_TYPE_I;
		}
		else
		{
			Type = NET_DEV_MEDIA_FRAME_TYPE_P;
		}

	    nd.NalNum = 0;
	    frame_head.Hdr.No = pMedia->m_nVSeq++;
	    frame_head.Hdr.EncodeType = EncodeType;
	    frame_head.Hdr.Timestamp =   CTimerRdmsc(); //ms
	    frame_head.Hdr.Type = Type; //0x01--I 帧
	    frame_head.Hdr.Video.Width = pFrame->m_iWith;
	    frame_head.Hdr.Video.Height =  pFrame->m_iHight;
	    frame_head.Hdr.Video.UTCTime = time(NULL);
		frame_head.Hdr.Size =  pFrame->m_iDataSize;
		frame_head.Data =  pFrame->m_bData;
		frame_head.NalDesc = &nd;
		
	}
	else if(pFrame->m_iDataType ==DATA_TYPE_AUDIO)
	{
		EncodeType = NET_DEV_MEDIA_ENC_TYPE_G711A;			
		Type = NET_DEV_MEDIA_FRAME_TYPE_A;   
		nd.NalNum = 0;
	    frame_head.Hdr.No = pMedia->m_nASeq++;
	    frame_head.Hdr.EncodeType = EncodeType;
	    frame_head.Hdr.Timestamp =  CTimerRdmsc(); //ms
	    frame_head.Hdr.Type = Type; //0x01--I 帧
	    frame_head.Hdr.Audio.SamplesPerSec = NET_DEV_MEDIA_SAMPLE_RATE_08K;
	    frame_head.Hdr.Audio.BitsPerSample = 16;
	    frame_head.Hdr.Audio.ChannelNum = 2;
		frame_head.Hdr.Size =  pFrame->m_iDataSize;
		frame_head.Data =  pFrame->m_bData;
		frame_head.NalDesc = &nd;
		iTemPlet = pFrame->m_iDataSize;
	}	

	else if(pFrame->m_iDataType ==DATA_TYPE_TEMPLE)
	{		
		memcpy(cTemPlet,pFrame->m_bData,pFrame->m_iDataSize);
		EncodeType = NET_DEV_MEDIA_ENC_TYPE_G711A;
		Type = NET_DEV_MEDIA_FRAME_TYPE_A;
		nd.NalNum = 0;
	    frame_head.Hdr.No = pMedia->m_nASeq++;
	    frame_head.Hdr.EncodeType = EncodeType;
	    frame_head.Hdr.Timestamp =  CTimerRdmsc(); //ms
	    frame_head.Hdr.Type = Type; //0x01--I 帧
	    frame_head.Hdr.Audio.SamplesPerSec = NET_DEV_MEDIA_SAMPLE_RATE_08K;
	    frame_head.Hdr.Audio.BitsPerSample = 16;
	    frame_head.Hdr.Audio.ChannelNum = 2;
		frame_head.Hdr.Size =  iTemPlet;
		frame_head.Data =  cTemPlet;
		frame_head.NalDesc = &nd;	
	}	
	NET_DEV_SDK_MEDIA_LPUSH(_pContext, &frame_head);
	return 0;
}


/* 打开实时流 */
static int
gsf_sdk_demo_ls_ps_stream_open(NET_DEV_MEDIA *m, int Chnl, int Level)
{
	void *media =  CreatMedia(Chnl,Level,MediaFrameCall,m);
    if (media)
    {
        NET_DEV_SDK_MEDIA_SET_UDATA(m, media);
    }
    return 0;
}
static int
gsf_sdk_demo_ls_ps_stream_play(NET_DEV_MEDIA *m)
{
    GsfLiveMedia *media = NET_DEV_SDK_MEDIA_GET_UDATA(m);
    if (media)
    {
        media->m_iStat = 1;
    }
    return 0;
}

/* 播放时控制 */
static int
gsf_sdk_demo_ls_ps_stream_ctrl(NET_DEV_MEDIA *m, unsigned Cmd, void *Data)
{
    return 0;
}

/* 关闭实时流 */
static void
gsf_sdk_demo_ls_ps_stream_close(NET_DEV_MEDIA *m)
{
    GsfLiveMedia *media = NET_DEV_SDK_MEDIA_GET_UDATA(m);
    if (media)
    {
        media->m_iStat = 0;
    }
}


static NET_DEV_SDK_FUNCS g_sdk_ops =
{
    /* 参数获取/设置, 控制命令 */
    .SetP   = gsf_sdk_demo_set_param,
    .GetP   = gsf_sdk_demo_get_param,
    .PMsg   = gsf_sdk_demo_post_msg,

    /* 实时流 */
	#if 0
    .LProbe  = gsf_sdk_demo_ls_stream_probe,
    .LOpen   = gsf_sdk_demo_ls_stream_open,
    .LPlay   = gsf_sdk_demo_ls_stream_play,
    .LCtrl   = gsf_sdk_demo_ls_stream_ctrl,
    .LClose  = gsf_sdk_demo_ls_stream_close,
    #else
    .LProbe  = gsf_sdk_demo_ls_ps_stream_probe,
    .LOpen   = gsf_sdk_demo_ls_ps_stream_open,
    .LPlay   = gsf_sdk_demo_ls_ps_stream_play,
    .LCtrl   = gsf_sdk_demo_ls_ps_stream_ctrl,
    .LClose  = gsf_sdk_demo_ls_ps_stream_close,
    #endif
    /* 历史流 */
    .HProbe  = gsf_sdk_demo_hs_stream_probe,
    .HOpen   = gsf_sdk_demo_hs_stream_open,
    .HPlay   = gsf_sdk_demo_hs_stream_play,
    .HPause  = gsf_sdk_demo_hs_stream_pause,
    .HLseek  = gsf_sdk_demo_hs_stream_lseek,
    .HClose  = gsf_sdk_demo_hs_stream_close,
    .HPull   = gsf_sdk_demo_hs_stream_pull,

    /* 元数据通道(图片/对讲等) */
    .DCOpen  = gsf_sdk_demo_dc_open,
    .DCRecv  = gsf_sdk_demo_dc_recv,
    .DCClose = gsf_sdk_demo_dc_close
};


static struct option g_opt_longs[] =
{
    {"cms_enable", 0, NULL, 'E'},
    {"cms_addr", 1, NULL, 'A'},
    {"cms_port", 1, NULL, 'P'},
    {"dev_id", 1, NULL, 'I'},
    {"dir", 1, NULL, 'D'},
    {"hevc", 0, NULL, "h"},
    {"version", 0, NULL, 'V'},
    {"help", 0, NULL, 'H'},
    {NULL, 0, 0, 0}
};


static __inline__ void
gsf_sdk_demo_print_version(char *name)
{
    printf(
        "%s version %s\n",
        basename(name),
        VERSION_STR
    );
}


static __inline__ void
gsf_sdk_demo_print_usage(char *name)
{
    printf(
        "%s version %s\n"
        "  --cms_enable|-E               enable @gsf\n"
        "  --cms_addr|-A 192.168.100.100 cms ip\n"
        "  --cms_port|-P 7000            cms port\n"
        "  --dev_id|-I 123...            dev id\n"
        "  --dir|-D Path                 data dir\n"
        "  --hevc|-h                     hevc video\n"
        "  --version|-V                  print version\n"
        "  --help|-H                     print help manual\n"
        ,
        basename(name),
        VERSION_STR
    );
}

int gsf_init(char *pCmsIp,char *pUid,int iCmsPort,GSFAUDIO_DATA_PROCESS AudioProcess,int iFrameRate)
{
    int err, opt;
    NET_DEV_IVS_CONFIG ivs_param;
	if(pCmsIp)
		strncpy(g_cms_ip, pCmsIp, sizeof(g_cms_ip) - 1);
	if(pUid)
		strncpy(g_puid, pUid, sizeof(g_puid) - 1);
	if(iCmsPort!=-1)
		g_cms_port = iCmsPort;
	dbgmsg("###########gsf_init  start#######\n");
	
    strcat(TEST_TALK_FILE, G711A_AUDIO_PATH);
    strcat(TEST_FILE_PATH, (g_hevc_video)?HEVC_VIDEO_PATH:H264_VIDEO_PATH);
    strcat(TEST_PSFILE_PATH, PS_FILE_PATH);

    signal(SIGPIPE, SIG_IGN);
	g_iFrameRate = iFrameRate;
    NET_DEV_SDK_INIT(MSG_PORT, RTSP_PORT, &g_sdk_ops);
    NET_DEV_SDK_START();
    if (g_enable_gsf)
    {
        strncpy(ivs_param.Ip, g_cms_ip, sizeof(ivs_param.Ip) - 1);
        ivs_param.Ip[sizeof(ivs_param.Ip) - 1] = 0;
        ivs_param.Port = g_cms_port;
        strcpy(ivs_param.DevID, g_puid);
        strcpy(ivs_param.Passwd, "");
        printf("now start ivs service...\n");
        NET_DEV_SDK_START_IVS_SERVICE(&ivs_param);
    }
	
	g_AudioProcess = AudioProcess;

    return 0;
}


int gsf_postion(float x ,float y)
{
    NET_DEV_ALARM_ITEM alarm;
    memset(&alarm, 0, sizeof(alarm));
    alarm.AlarmType = NET_DEV_ALARM_MAJOR_EXT;
    alarm.AlarmSubType = NET_DEV_EXT_POSITION;

    NET_DEV_EXT_POSITIONINFO *pos = (NET_DEV_EXT_POSITIONINFO*)alarm.Data;
    pos->UTCTime = htonl(time(NULL));
    snprintf(pos->X, sizeof(pos->X), "%f", x);
    snprintf(pos->Y, sizeof(pos->Y), "%f", y);
    snprintf(pos->Z, sizeof(pos->Z), "0");
    NET_DEV_SDK_SUBMIT_ALARM(&alarm);

}

int gsf_alarm(int iStat)
{
    NET_DEV_ALARM_ITEM alarm;
    memset(&alarm, 0, sizeof(alarm));
    alarm.AlarmType = NET_DEV_ALARM_MAJOR_ALARM;
    alarm.AlarmSubType = NET_DEV_ALARM_TYPE_IO;

    NET_DEV_ALARM_DESC *pos = (NET_DEV_ALARM_DESC*)alarm.Data;
    pos->Type= NET_DEV_PEAOSC_EVENT_TYPE_UNKNOWN;
    pos->Id= 0;
    pos->Rule= 0;
	strcpy(pos->Desc,"SOS");
    NET_DEV_SDK_SUBMIT_ALARM(&alarm);

}


int gsf_push(int ichan,int istream,char *pData,int iLen,int iWith,int iHight,int iDataType)
{
	static int iSps[32][2];
	frame_element_t *pFrame = NULL;
	int iFrameType = FRAME_TYPE_P;
	if(iDataType==DATA_TYPE_VIDEO)
		iFrameType = (pData[4]&0x1f)==0x07 ?  FRAME_TYPE_I : FRAME_TYPE_P;
	g_iWith[ichan][istream] = iWith;
	g_iHeight[ichan][istream] = iHight;
	if((iSps[ichan][istream]==0)&&(iFrameType==FRAME_TYPE_I))
	{
		if(nalu_hevc_get_sps_pps(pData,iLen,&g_PicParam[ichan][istream])==0)
		{
			iSps[ichan][istream] = 1;
		}
	}

    pFrame = (frame_element_t *)MemAllloc(iLen+sizeof(frame_element_t)+128);
    if(pFrame)
    {   
    	pFrame->m_iDataSize = 0;
		memcpy(pFrame->m_bData,pData,iLen);
		pFrame->m_iDataSize = iLen;
        pFrame->m_iDataType=iDataType;     
        pFrame->m_iFrameType=iFrameType;   
		pFrame->m_iEnCodeType=H264_VIDEO_ENCODER;      
        pFrame->m_iWith =iWith;
        pFrame->m_iHight=iHight;    
        pFrame->m_ullTimeStap = CTimerRdmsc();   
		if(g_pDataFiFo[ichan][istream])
			PushDataToFiFo(pFrame,g_pDataFiFo[ichan][istream]);
		else
			MemFree(pFrame);
	}
	
}



#define  MAX_FRAME_SIZE  (512*1024)
#define  MAX_SIZE  (512*1024)
static CHAR g_cSubFilebuf[MAX_SIZE]; 
static INT32  g_nSubCurfilebufsize = 0;
static CHAR g_SubFrameBuf[MAX_FRAME_SIZE]; 
static INT32  g_SubFrameLen = 0; 

typedef struct nalu_s
{
	unsigned char  nalu_type;
	unsigned char  sz_startcode;
	char  *nalubuf;
	int   nalusize;
}nalu_t;



static int h264_get_nalu(nalu_t *nalu,char *srcbuf,int srcsize)
{
	int  index=0;
	int  i=0;
	
	while(i < srcsize - 3)
	{
		if(srcbuf[i+2] <= 1) 
		{
			if(srcbuf[i] == 0 && srcbuf[i+1] == 0 && srcbuf[i+2] == 1)
			{
				if(i > 0 && srcbuf[i-1] == 0)
				{
					nalu->sz_startcode = 4;
					nalu->nalubuf = &srcbuf[i-1];
					index = i-1;
				}
				else
				{
					nalu->sz_startcode = 3;
					nalu->nalubuf = &srcbuf[i];
					index = i;
				}
				break;
			}
			i++;
		}
		else
		{
			i += 3;
		}
	}

	if(i >= srcsize-3)
	{
        dbgmsg("##########%s  i:%d srcsize:%d\n", __FUNCTION__,i,srcsize);  
		return  -1;
	}

	i += nalu->sz_startcode; //??????һ??NALU??ʼ??
	while(i < srcsize - 3)
	{
		if(srcbuf[i+2] <= 1) 
		{
			if(srcbuf[i] == 0 && srcbuf[i+1] == 0 && srcbuf[i+2] == 1)
			{
				if(srcbuf[i-1] == 0)
				{
					nalu->nalusize = i-1 - index;
				}
				else
				{
					nalu->nalusize = i - index;
				}
				break;
			}
			i++;
		}
		else
		{
			i += 3;
		}
	}

	if(i >= srcsize-3)
	{
		nalu->nalusize = srcsize - index;
	}
	nalu->nalu_type = nalu->nalubuf[nalu->sz_startcode];

	return  0;
}



INT32 PthreadStream(void *_uThis) 
{
    INT32 ret = 0;
    INT32 MainSq = 0;
    INT32 SubSq = 0;
    INT32 MainLen = 0;
    INT32 SubLen = 0;
    FILE *FpSub = NULL;
	nalu_t nalu;
	INT32 nType =0;
	INT32 nCount =0;
	open_lab:
    FpSub = fopen("sub.h264", "rb");
    while(1)
    {
		if(FpSub==NULL)
		{
			XSleep(1,40*1000);
			goto open_lab;
		}
        loop:
        ret = fread(g_cSubFilebuf+g_nSubCurfilebufsize,1,MAX_SIZE - g_nSubCurfilebufsize,FpSub);
	    if(ret<=0)
	    {
            
	        fseek(FpSub,0,0);
	        goto loop;
	    }    
	    else
	    {
	       g_nSubCurfilebufsize+= ret;
	    }
	    if(h264_get_nalu(&nalu,g_cSubFilebuf,g_nSubCurfilebufsize) ==0)
		{
	        memcpy(g_SubFrameBuf+SubLen,nalu.nalubuf,nalu.nalusize);
            SubLen+=nalu.nalusize;
            
         //   dbgmsg("##########%s nalu.nalu_type %02x\n", __FUNCTION__,nalu.nalu_type);  
		    if((nalu.nalu_type & 0x1f) != 0x07 && (nalu.nalu_type & 0x1f) != 0x08  &&(nalu.nalu_type & 0x1f)!=0x06)
	        {
	            nType = (nalu.nalu_type & 0x1f) == 0x05 ? 0 : 1;
            	gsf_push(0,0,g_SubFrameBuf,SubLen,720,576,DATA_TYPE_VIDEO);
            	SubLen = 0;
            }
            g_nSubCurfilebufsize-= nalu.nalusize;
            memmove(g_cSubFilebuf, nalu.nalubuf + nalu.nalusize, g_nSubCurfilebufsize);
		}      
		else
        {
            dbgmsg("##########%s g_pSubMedia error \n", __FUNCTION__);  
        }
		XSleep(0,40*1000);
    }
    return 0;
}


void *stream_thread(void *buf)
{
	pthread_detach(pthread_self());
	wrapper_iotdev_camera_init();
	wrapper_iotdev_camera_start_by_fifo(1, 1);
	while(1){
		memset(gsf_frame_buffer, 0x0, 512*1024);                                       
		gsf_get_frame = 1;
		while(!gsf_get_frame_already){                                                 
			usleep(10);                                                                
		}
        gsf_push(0, 0, gsf_frame_buffer, gsf_frame_size, 1280, 720, DATA_TYPE_VIDEO);
        gsf_frame_size = 0;
        gsf_get_frame_already = 0;
    }
}


void *audio_thread(void *buf)
{
	int audio_push_count = 0, count = 0;
	FILE *fd = NULL;
	int ret = 0;

	while(1){
		while(!audio_already){
			usleep(10);
		}
		if(audio_already){
			audio_push_count = readLen / 160 / 2;

			if(readLen % 160){
				while(count < (audio_push_count + 1)){
					if(count == audio_push_count){
						gsf_push(0, 0, (((char *)recordG711aBuf) + (count * 160)), readLen % 160 / 2, 1280, 720, DATA_TYPE_AUDIO);
					}else{
						gsf_push(0, 0, (((char *)recordG711aBuf) + (count * 160)), 160, 1280, 720, DATA_TYPE_AUDIO);
					}
					count++;
				}
			}else{
				while(count < audio_push_count){
					gsf_push(0, 0, (((char *)recordG711aBuf) + (count * 160)), 160, 1280, 720, DATA_TYPE_AUDIO);
					count++;
				}
			}
		}
		audio_already = 0;
		audio_push_count = 0;
		count = 0;
	}
}

int gsf_audio_cb(char *_cBuf,int _iLen,char * FileName){
	int count = 0, ret = 0;
	while(count < _iLen){
		((short *)pcm_buf)[count] = alaw2linear((unsigned char *)_cBuf[count]); 
		count++;
	}
	ret = audioTrack_write(pcm_buf, _iLen * 2);
	memset(pcm_buf, 0x0, 160 * 2);
	return ret;
}

INT32 main(INT32 argc, char **argv) 
{
	pcm_buf = calloc(1, 8192);
	gsf_init(NULL,NULL,-1,gsf_audio_cb,25);
//	PthreadStream(NULL);
	int temp;
    memset(&thread, 0, sizeof(thread));
    
    if((temp = pthread_create(&thread[0], NULL, stream_thread, NULL)) != 0){
	    printf("stream thread create failed!\n");
		return -1;
	}

    if((temp = pthread_create(&thread[1], NULL, audio_thread, NULL)) != 0){
	    printf("audio thread create failed!\n");
		return -1;
	}
	audioRecord_start();
	//wrapper_iotdev_camera_stop();
	while(1)
	{
		XSleep(10,0);
	}
}





//:~ End
