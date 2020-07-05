#ifndef _GSF_NET_API_H
#define _GSF_NET_API_H

#include <sys/time.h>
#include <sys/socket.h>

/*
* Copyright (c) 2010
* All rights reserved.
* 
* filename:gsf_net_api.h
* 
* func:本文件实现了linux下 socket封装
* 
* author : gsf
* createdate: 2010-10-10
*
* modify list: 
* 
*/

#ifdef __cplusplus
extern "C" {
#endif

#define LISTENQ 1024
#define GSF_MAX_BLOCK_SEND_TIME 3
#define GSF_MAX_BLOCK_RECV_TIME 3

#define GSF_DEFAULT_RECV_TIMEOUT 500
#define GSF_DEFAULT_SEND_TIMEOUT 1000

#define GSF_DEFAULT_LOCAL_TIME	2

#define GSF_RETURN_FAIL -1
#define GSF_RETURN_OK    0

typedef enum _GSF_SOCK_TYPE_
{
    GSF_TCP_SOCK         = 1,
    GSF_UDP_SOCK        ,
    GSF_LOCAL_TCP_SOCK  ,
    GSF_LOCAL_UDP_SOCK  ,
}GSF_SOCK_TYPE;
	
//////////////////////////////////////////////////////////////////////////
//
//socket 操作函数
//
//////////////////////////////////////////////////////////////////////////

//=================================================================
//函数: gsf_create_sock 
//
//功能:创建一个socket
//
//iType :创建的socket的类型 1:tcp 2:udp 3:本地通讯tcp sock
//		  4.本地通讯udp sock 使用af_unix协议族
//
//返回值：成功返回socket ,出错返回GSF_RETURN_FAIL
//==================================================================
int gsf_create_sock(GSF_SOCK_TYPE iType);

//=================================================================
//函数: gsf_close_socket 
//
//功能:关闭指定的socket
//
//GSFd : 所要关闭的socket 	
//返回值：超时返回GSF_RETURN_OK,出错返回GSF_RETURN_FAIL
//=================================================================
int gsf_close_socket(int *fd);

//=================================================================
//函数: gsf_get_sock_ip 
//
//功能: 取得socket绑定ip
//
//sock:套接字
//
//返回值：成功返回ip ,出错返回GSF_RETURN_FAIL
//=================================================================
int gsf_get_sock_ip(int sock);

//=================================================================
//函数: gsf_get_sock_port 
//
//功能:取得socket绑定port
//
//sock:套接字
//
//返回值：成功返回port ,出错返回GSF_RETURN_FAIL
//=================================================================
int gsf_get_sock_port(int sock);

//=================================================================
//函数: gsf_tcp_noblock_connect 
//
//功能:tcp非阻塞方式连接服务端
//
//参数:
//localHost:本地端的域名或者ip地址
//localServ:本地端的服务名称或者是端口
//
//dstHost: 服务端的域名或者ip地址
//dstServ: 服务端的服务名称或者是端口
//timeout_ms: 连接服务器超时时间(单位毫秒)
//
//返回值：连接成功返回连接使用的socket, 否则返回GSF_RETURN_FAIL
//=================================================================	
int gsf_tcp_noblock_connect(const char *localHost, const char *localServ, 
					const char *dstHost, const char *dstServ, int timeout_ms);

//=================================================================
//函数: gsf_tcp_noblock_recv 
//
//功能:tcp非阻塞socket接收函数
//
//参数:
//  sockfd:接收的socket
//  rcvBuf:接收的缓冲区
//  bufSize:接收的缓冲区大小
//  rcvSize:制定接收的字节数, 填0不指定接收的字节数
//  timeOut:接收超时时间(单位毫秒)		
//
//返回值:成功返回接收到的字节数, 否则返回GSF_RETURN_FAIL
//=================================================================
int gsf_tcp_noblock_recv(int sockfd, unsigned char *rcvBuf, int bufSize, int rcvSize, 
						int timeOut);
//=================================================================
//函数: gsf_tcp_noblock_send 
//
//功能:tcp非阻塞方式发送
//
//参数:
// hSock:所要发送数据的socket
// pbuf:所要发送的数据
// size: 所要发送的数据的字节数
// pBlock: 判读网路是不是断开 (连续超过timeout毫秒次不成功则认为网络断开)
// timeOut: 发送超时时间(单位毫秒)
//
//返回值:成功返回发送的字符数, 否则返回GSF_RETURN_FAIL
//=================================================================
int gsf_tcp_noblock_send(int hSock, unsigned char *pbuf, int size, int *pBlock, int timeOut);

//=================================================================
//函数: gsf_tcp_noblock_mtu_send 
//
//功能:tcp非阻塞方式发送每次发送mtu字节数据
//
//参数:
// hSock:所要发送数据的socket
// pbuf: 所要发送的数据
// size: 所要发送的数据的字节数
// mtu:  每次发送的最大字节数
//返回值:成功返回发送的字符数, 否则返回GSF_RETURN_FAIL
//=================================================================
int gsf_tcp_noblock_mtu_send(int hSock, unsigned char *pbuf,int size, int mtu);

//=================================================================
//函数: gsf_select 
//
//功能:监听fd_array数组中的socket
//
//fd_array: socket 数组
//fd_num:   socket 的个数
//fd_type:  bit 1 为 1 代表监听socket 是否可读
//	 	  bit 2 为 1 代表监听socket 是否可写
//		  bit 3 为 1 代表监听socket 是否有异常
//	
//返回值：超时返回GSF_RETURN_OK,出错返回GSF_RETURN_FAIL，否则
//		如果有可读的返回该socket值|0x10000
//		有可写返回该socket值|0x20000
//		有异常返回该socket值|0x40000
//=================================================================
int gsf_select(int *fd_array, int fd_num, int fd_type, int time_out);

//=================================================================
//函数: gsf_ip_n2a 
//
//功能:整数IP 转成 点分十进制 IP
//
//ip    输入: 整数IP(网络序)
//ourIp 输出: 点分十进制 IP缓存
//len   输入: 点分十进制 IP缓存大小
//	
//返回值:成功返回GSF_RETURN_OK,否则返回GSF_RETURN_FAIL
//=================================================================	
unsigned long gsf_ip_n2a(unsigned long ip, char *ourIp, int len);

//=================================================================
//函数: gsf_ip_a2n 
//
//功能:点分十进制 IP(或者域名) 转成 整数IP  
//
//ip    输入: 点分十进制 IP缓存
//	
//返回值:成功返回整数IP(网络序),否则返回GSF_RETURN_FAIL
//=================================================================	
unsigned long gsf_ip_a2n(char *szIp);

//=================================================================
//函数: gsf_Ip_N2A 
//
//功能:gsf_itoa 跟windows 上itoa一样 数字转成字符串
//
//n:数值
//
//s:字串
//
//返回值:无
//=================================================================
void   gsf_itoa(int   n,   char   s[]);

/*
SO_LINGER

   此选项指定函数close对面向连接的协议如何操作（如TCP）。
   缺省close操作是立即返回，如果有数据残留在套接口缓冲区中
   则系统将试着将这些数据发送给对方。
SO_LINGER选项用来改变此缺省设置。使用如下结构：
struct linger {
     int l_onoff; // 0 = off, nozero = on 
     int l_linger; // linger time 
};

有下列三种情况：

l_onoff为0，则该选项关闭，l_linger的值被忽略，等于缺省情况，close立即返回； 

l_onoff为非0，l_linger为0，则套接口关闭时TCP夭折连接，
TCP将丢弃保留在套接口发送缓冲区中的任何数据并发送一个RST给对方，
而不是通常的四分组终止序列，这避免了TIME_WAIT状态； 

l_onoff 为非0，l_linger为非0，当套接口关闭时内核将拖延一段时间（由l_linger决定）。
如果套接口缓冲区中仍残留数据，进程将处于睡眠状态，
直 到（a）所有数据发送完且被对方确认，
之后进行正常的终止序列（描述字访问计数为0）或（b）延迟时间到。
此种情况下，应用程序检查close的返回值是非常重要的，
如果在数据发送完并被确认前时间到，
close将返回EWOULDBLOCK错误且套接口发送缓冲区中的任何数据都丢失。
close的成功返回仅告诉我们发送的数据（和FIN）已由对方TCP确认，
它并不能告诉我们对方应用进程是否已读了数据。
如果套接口设为非阻塞的，它将不等待close完 成。 


l_linger的单位依赖于实现，4.4BSD假设其单位是时钟滴答（百分之一秒），
但Posix.1g规定单位为秒。

*/

unsigned short cal_chksum(unsigned short *addr, int len);


#ifdef __cplusplus
}
#endif 

#endif

