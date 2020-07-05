/*
 * linux socket网络编程常用接口
 */

//#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/ip_icmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <linux/if_packet.h>
#include "gsf_net_api.h"

//***********************套接字相关************************************//
int gsf_close_socket(int *pSock)
{
	if (pSock != NULL && *pSock > 0)
	{
//		shutdown(*pSock, 2); //ljm close 2010-07-15
		close(*pSock);
		*pSock = -1;
		return GSF_RETURN_OK;
	}
	return GSF_RETURN_FAIL;	
}

int gsf_create_sock(GSF_SOCK_TYPE iType)
{
	int fd = socket((iType/3) ? (AF_UNIX):(AF_INET),
					(iType%2) ? (SOCK_STREAM):(SOCK_DGRAM) ,0);
	return fd;
}

int gsf_set_sock_attr(int fd, int bReuseAddr, int nSndTimeO, int nRcvTimeO, int nSndBuf, int nRcvBuf)
{
    int err_ret = GSF_RETURN_OK;	    
    struct timeval sndTo, rcvTo;

	if (fd <= 0)
		return GSF_RETURN_FAIL;
	
    sndTo.tv_sec  = nSndTimeO / 1000;
    sndTo.tv_usec = (nSndTimeO % 1000) * 1000;
	
    rcvTo.tv_sec  = nRcvTimeO / 1000;
    rcvTo.tv_usec = (nRcvTimeO % 1000) * 1000;
	
    if (bReuseAddr != 0 && setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,(void *)&bReuseAddr,sizeof(int)) < 0)
        err_ret = GSF_RETURN_FAIL;
	
    if (nSndTimeO != 0 && setsockopt(fd,SOL_SOCKET,SO_SNDTIMEO,(void  *)&sndTo,sizeof(sndTo)) < 0)
		err_ret = GSF_RETURN_FAIL;
    if (nSndTimeO != 0 && setsockopt(fd,SOL_SOCKET,SO_RCVTIMEO,(void  *)&rcvTo,sizeof(rcvTo)) < 0)
		err_ret = GSF_RETURN_FAIL;
	
    if (nSndBuf != 0 && setsockopt(fd,SOL_SOCKET,SO_SNDBUF,(void  *)&nSndBuf,sizeof(nSndBuf)) < 0)
		err_ret = GSF_RETURN_FAIL;
    if (nRcvBuf != 0 && setsockopt(fd,SOL_SOCKET,SO_RCVBUF,(void  *)&nRcvBuf,sizeof(nSndBuf)) < 0)
        err_ret = GSF_RETURN_FAIL;

    return err_ret;
}

int gsf_get_sock_ip(int sock)
{
	struct sockaddr_in sockAddr;
	int addrLen = sizeof(struct sockaddr);
	
	if (0 != getsockname(sock, (struct sockaddr *)&sockAddr, (socklen_t *)&addrLen))
		return GSF_RETURN_FAIL;
	return sockAddr.sin_addr.s_addr;
}

int gsf_get_sock_port(int sock)
{
	struct sockaddr_in sockAddr;
	int addrLen = sizeof(struct sockaddr);
	
	if (0 != getsockname(sock, (struct sockaddr *)&sockAddr, (socklen_t *)&addrLen))
		return GSF_RETURN_FAIL;
	return htons(sockAddr.sin_port);
}

int gsf_set_sock_block(int nSock)
{	
    int bBlock = 0;
	if (nSock <= 0)
		return GSF_RETURN_FAIL;
	
    if (ioctl(nSock, FIONBIO, &bBlock) < 0 )
        return GSF_RETURN_FAIL;
    return GSF_RETURN_OK;
}
   
int gsf_set_sock_noblock(int nSock)
{	
    int bNoBlock = 1;
	
	if (nSock <= 0)
		return GSF_RETURN_FAIL;
	
    if (ioctl(nSock, FIONBIO, &bNoBlock) < 0 )
        return GSF_RETURN_FAIL;
    return GSF_RETURN_OK;
}

int gsf_tcp_noblock_connect(const char *localHost, const char *localServ, const char *dstHost, const char *dstServ, int timeout_ms)
{
    int     sockfd, n;
    struct addrinfo gsfnts, *dstRes, *localRes, *resSave;
    int	    error = 1;
    int	    error_len = sizeof(int);
    fd_set  fset;
    int     ret = 0;
	int     opt = 0;
	
    struct timeval to = {0};
	
	 dstRes = resSave = localRes = NULL;
	
    if (dstHost == NULL || dstServ == NULL)
		return GSF_RETURN_FAIL;	
	
    bzero(&gsfnts, sizeof (struct addrinfo));
    gsfnts.ai_family = AF_UNSPEC;
    gsfnts.ai_socktype = SOCK_STREAM;
	
    to.tv_sec = timeout_ms / 1000;
    to.tv_usec= (timeout_ms % 1000)*1000;	
    if ( (n = getaddrinfo (dstHost, dstServ, &gsfnts, &dstRes)) != 0)
        return GSF_RETURN_FAIL;
    resSave = dstRes;
	
    do {
		sockfd = socket (dstRes->ai_family, dstRes->ai_socktype, dstRes->ai_protocol);
		if (sockfd < 0)
		{
			sleep(3);     
			continue;            /*ignore this one */
		}   
		
		gsf_set_sock_noblock(sockfd);
		opt = 1;
		setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char *)&opt,sizeof(opt));
		
		if (localServ)
		{
			if ( (n = getaddrinfo (localHost, localServ, &gsfnts, &localRes)) != 0)
			{
				if (resSave)
				{
					freeaddrinfo (resSave);
					resSave = NULL;
				}
				close(sockfd); 
				return GSF_RETURN_FAIL;
			}
			gsf_set_sock_attr(sockfd, 1, 0, 0, 0, 0);  //set the sock reuser_addr attribute
			
			if (bind(sockfd, localRes->ai_addr, localRes->ai_addrlen) == 0)
				break;               /* success */		
		}
		ret = connect (sockfd, dstRes->ai_addr, dstRes->ai_addrlen);
		if (ret == 0)
			break;	
		if (ret < 0 && errno == EINPROGRESS)
		{
			FD_ZERO(&fset);
			FD_SET(sockfd,&fset);
			
			if ((ret = select(sockfd + 1, NULL, &fset, NULL, &to)) > 0)
			{
				if (FD_ISSET(sockfd, &fset) > 0)
				{
					if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *)&error, (socklen_t *)&error_len) < 0)
						error = 1;
				}
			}
			if (error == 0)
				break;
		}
		
		close(sockfd);          /* ignore this one */
    } while ( (dstRes = dstRes->ai_next) != NULL);
	if (dstRes == NULL)             /* errno set from final connect() */
        sockfd =  GSF_RETURN_FAIL;

	if (resSave)
	{
		freeaddrinfo (resSave);
		resSave = NULL;
	}
	if (localRes)
	{
		freeaddrinfo (localRes);
		resSave = NULL;
	}

    return (sockfd);
}


int gsf_tcp_noblock_recv(int sockfd, unsigned char *rcvBuf, int bufSize, int rcvSize, int timeOut)
{
    int		ret;
    unsigned long	dwRecved = 0;
    unsigned long	nTryTime = 0;
    int             nSize    = rcvSize;
	
	if (sockfd <= 0 || rcvBuf == NULL || bufSize <= 0)
		return -1;
	
    if (rcvSize <= 0 || bufSize < rcvSize)
        nSize = bufSize;	           
	
    while(dwRecved < nSize)
    {
		ret = recv(sockfd, rcvBuf + dwRecved, nSize - dwRecved, 0);
		if(0 == ret)
			return -1;
		else if(ret < 1)
		{
			if(ECONNRESET == errno)
				return -1;
			else if(EWOULDBLOCK == errno  || errno == EINTR || errno == EAGAIN)
			{
				if(nTryTime++ < GSF_DEFAULT_RECV_TIMEOUT)
				{				
					usleep(10000);
					continue;
				}
				else
					break;
			}
			
			return -1;
		}
		nTryTime = 0;
		dwRecved += ret;
		if (rcvSize <= 0) //没有指定收数据的长度,则收一次就返回收到的长度
            break;
    }
    return dwRecved;
}

int gsf_tcp_noblock_send(int hSock, unsigned char *pbuf,int size, int *pBlock, int timeOut)
{
	int  block = 0;	
	int  alllen = size;
	int  sended = 0;
	
	if (hSock < 0 || pbuf == NULL || size <= 0)
		return 0;

	if (pBlock != NULL)
		*pBlock = 0;
	
	while(alllen > 0)
	{	
		sended = send(hSock,pbuf,alllen,0);
		if(0 == sended)
		{
			return GSF_RETURN_FAIL;
		}
		else if(sended < 1)
		{
			if(block > timeOut || timeOut == 0)
			{
				return GSF_RETURN_FAIL;
			}
			if(errno == EWOULDBLOCK || errno == EINTR || errno == EAGAIN)
			{			
				if(block++ < timeOut)
				{
					usleep(1000);
					continue;
				}
				else
				{
					if (pBlock != NULL)	
						*pBlock = 1;
					break;
				}
			}
			return GSF_RETURN_FAIL;
		}
		else
		{	
			if (pBlock != NULL)	
				*pBlock = 0;
			pbuf += sended;
			alllen -= sended;
		}
	}
		  
	if(alllen > 0)
		return GSF_RETURN_FAIL;
	return size;
}
	
int gsf_tcp_noblock_mtu_send(int hSock,unsigned char *pbuf,int size, int mtu)
{
	int ret       = size;
	int sendLen   = 0;
	int nBlock    = 0;
	
	if (hSock <= 0 || pbuf == NULL || size <= 0)
		return GSF_RETURN_FAIL;

	
	while(size > 0)
	{
		sendLen = size > mtu ? mtu : size;
		if (sendLen != gsf_tcp_noblock_send(hSock, pbuf, sendLen, &nBlock, 3000))
			return GSF_RETURN_FAIL;
	
		pbuf += sendLen;
		size -= sendLen;
	}

	return ret;
}
	 		 
int gsf_tcp_noblock_accept(int fd, struct sockaddr *sa, int *salenptr, struct timeval * to)
{
	int nready = 0;
	int maxfd = fd+1;
	fd_set rset;

	if (fd <= 0 || to == NULL)
		return GSF_RETURN_FAIL;
	
	FD_ZERO(&rset);
	FD_SET(fd, &rset);
	
	nready = select(maxfd, &rset, NULL, NULL, to);
	if (nready > 0 && FD_ISSET(fd, &rset))
	{
		return gsf_tcp_block_accept(fd, sa, salenptr);
	}
	
	return GSF_RETURN_FAIL;
}

int gsf_tcp_block_accept(int fd, struct sockaddr *sa, int *salenptr)
{
	int n = GSF_RETURN_FAIL;
//	char szIp[32] = {0};
again:
	if ( (n = accept(fd, sa, (socklen_t *)salenptr)) < 0) 
	{
		if (errno == ECONNABORTED)
			goto again;
	}
	//if (sa != NULL)
	//	GSF_SYSLOG(GSF_LOG_DEBUG,"RECV REMOTE CONNECT %s\n", 
	//	inet_ntop(AF_INET, &((struct   sockaddr_in*)sa)->sin_addr, szIp, sizeof(szIp)));
	return (n);
}

int gsf_ping(char *ips, int timeout, int max_times)
{
    struct timeval timeo;
    int sockfd;
    struct sockaddr_in addr;
    struct sockaddr_in from;
    
    struct timeval *tval;
    struct ip 		 *iph;
    struct icmp 	 *icmp;

    unsigned char sendpacket[128];
    unsigned char recvpacket[128];
    char from_ip[32];
    int n;
    int ping_times = 0;
    int ret = 0;
    pid_t pid;
    int maxfds = 0;
    fd_set readfds;
    
    if (ips == NULL || strcmp(ips, "") == 0)
		return 0;
    bzero(&addr,sizeof(addr));
    addr.sin_family = AF_INET;
	inet_pton(AF_INET, ips, &addr.sin_addr);  

    sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0)
    {
		//printf("error: %s -> sockfd invalid %s\n", __func__, strerror(errno));
		return GSF_RETURN_FAIL;
    }
    
    timeo.tv_sec = timeout / 1000000;
    timeo.tv_usec = (timeout % 1000);
    
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(timeo)) == -1)
    {
		//printf("error: %s -> setsockopt err %s\n", __func__, strerror(errno));
    	close(sockfd);
		//sleep(5);
        return GSF_RETURN_FAIL;
    }
    
    memset(sendpacket, 0, sizeof(sendpacket));
    
    pid=getpid();
    int packsize = 0;

    icmp=(struct icmp*)sendpacket;
    icmp->icmp_type=ICMP_ECHO;
    icmp->icmp_code=0;
    icmp->icmp_cksum=0;
    icmp->icmp_seq=0;
    icmp->icmp_id=pid;
    packsize=8+56;
    tval= (struct timeval *)icmp->icmp_data;
    gettimeofday(tval,NULL);
    icmp->icmp_cksum=cal_chksum((unsigned short *)icmp,packsize);

    n = sendto(sockfd, (char *)&sendpacket, packsize, 0, (struct sockaddr *)&addr, sizeof(addr));
    if (n < 1)
    {
		//printf("error: %s -> sendto err %s\n", __func__, strerror(errno));
		close(sockfd);
		//sleep(5);
        return GSF_RETURN_FAIL;
    }
	
    while(ping_times++ < max_times)
    {	
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        maxfds = sockfd + 1;
        n = select(maxfds, &readfds, NULL, NULL, &timeo);
        if (n <= 0)
        {
			//printf("error: %s -> select %s %s\n", __func__, (n==0)?"timeout":"err", (n==0)?"":strerror(errno));
            ret = GSF_RETURN_FAIL;
            continue;
        }
    
        memset(recvpacket, 0, sizeof(recvpacket));
        int fromlen = sizeof(from);
        n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, 
					(struct sockaddr *)&from, (socklen_t *)&fromlen);
        if (n < 1) 
        {
			//printf("error: %s -> recvfrom err %s\n", __func__, strerror(errno));
			ret = GSF_RETURN_FAIL;
            continue;
        }
        
		inet_ntop(AF_INET, &from.sin_addr, from_ip, sizeof(from_ip));
        if (strcmp(from_ip,ips) != 0)
        {
			//printf("error: %s -> inet_ntop err %s\n", __func__, strerror(errno));
			ret = GSF_RETURN_FAIL;
            continue;
        }
        
        iph = (struct ip *)recvpacket;
    
        icmp=(struct icmp *)(recvpacket + (iph->ip_hl<<2));

        if (icmp->icmp_type == ICMP_ECHOREPLY && icmp->icmp_id == pid)
        {
            ret = GSF_RETURN_OK;
            break;
        }  
        else
        {
			//printf("error: %s -> icmp_type %d(%d) or icmp_id %d(%d) err\n", __func__, 
			//	icmp->icmp_type, ICMP_ECHOREPLY, icmp->icmp_id, pid);
            ret = GSF_RETURN_FAIL;
            continue;
        }
    }
 
    close(sockfd);
    return ret;
}
 
int gsf_get_host_ip(int af, char *host)
{
    unsigned long dwIp = 0;
    int           ret  = 0;
    struct addrinfo gsfnts, *res;
    
    if(host == NULL)
		return GSF_RETURN_FAIL;
    if(strcmp(host,"255.255.255.255") == 0 || strcmp(host, "") == 0)
        return GSF_RETURN_FAIL;
	
    bzero (&gsfnts, sizeof (struct addrinfo));
    gsfnts.ai_flags  = AI_CANONNAME;   /* always return canonical name */
    gsfnts.ai_family = AF_UNSPEC;   /* AF_UNSPEC, AF_INET, AF_INET6, etc. */
    gsfnts.ai_socktype = 0;  /* 0, SOCK_STREAM, SOCK_DGRAM, etc. */
    if ( (ret = getaddrinfo(host, NULL, &gsfnts, &res)) != 0)
        return GSF_RETURN_FAIL;
    dwIp =  ((struct sockaddr_in *)(res->ai_addr))->sin_addr.s_addr;	
	
    freeaddrinfo(res);

    return dwIp;
}
	
int gsf_select(int *fd_array, int fd_num, int fd_type, int time_out)
{
	int maxfd = 0;
	int index = 0;
	int ret   = 0;
	
	fd_set read_fd, *p_read_fd;
	fd_set write_fd, *p_write_fd;
	fd_set except_fd, *p_except_fd;
	struct timeval timeO, *p_time_out;

	if (fd_array == NULL || fd_num < 0)
		return GSF_RETURN_FAIL;
	p_time_out = NULL;
	if (time_out > 0)
	{
    	timeO.tv_sec = time_out / 1000;
    	timeO.tv_usec= (time_out % 1000)*1000;	
		p_time_out = &timeO;
	}

	p_read_fd = p_write_fd = p_except_fd = NULL;
	
	if (fd_type & 0x1)
	{
		p_read_fd = &read_fd;
		FD_ZERO(p_read_fd);
	}
	
	if (fd_type & 0x2)
	{
		p_write_fd = &write_fd;
		FD_ZERO(p_write_fd);
	}
	
	if (fd_type & 0x4)
	{
		p_except_fd = &except_fd;
		FD_ZERO(p_except_fd);
	}
	
	for (index = 0; index < fd_num; ++index)
	{
		if(fd_array[index] <= 0)
			continue;
		maxfd = maxfd > fd_array[index] ? maxfd : fd_array[index];

		if (p_read_fd)
			FD_SET(fd_array[index], p_read_fd);
		if (p_write_fd)
			FD_SET(fd_array[index], p_write_fd);
		if (p_except_fd)
			FD_SET(fd_array[index], p_except_fd);
	}
	if (maxfd <= 0)
		return GSF_RETURN_FAIL;
	
	maxfd += 1;

	while (1)
	{
		ret = select(maxfd, p_read_fd, p_write_fd, p_except_fd, p_time_out);
		if (ret < 0 && errno == EINTR)
		{
		    continue;
		}    
		else if (ret < 0)
			return GSF_RETURN_FAIL;
		else if (ret == 0)
			return GSF_RETURN_OK;
		else
		{ //rongp  同一时间只处理了一个connect
			for (index = 0; index < fd_num; ++index)
			{
				if (fd_array[index] <= 0)  //socket error
					continue;
				
				if (p_read_fd)
				{
					if (FD_ISSET(fd_array[index], p_read_fd))
						return fd_array[index] | 0x10000;
				}
				else if (p_write_fd)
				{
					if (FD_ISSET(fd_array[index], p_write_fd))
						return fd_array[index] | 0x20000;
				}
				else if (p_except_fd)
				{
					if (FD_ISSET(fd_array[index], p_except_fd))
						return fd_array[index] | 0x40000;;
				}			
			}
			return GSF_RETURN_OK;
		}
	}	
	return GSF_RETURN_FAIL;	
}

unsigned long gsf_ip_n2a(unsigned long ip, char *ourIp, int len)
{
	if(!ourIp)
		return GSF_RETURN_FAIL;
	
	memset(ourIp, 0, len);
	ip = htonl(ip);
	inet_ntop(AF_INET, &ip, ourIp, len);	
	return GSF_RETURN_OK;
}

unsigned long gsf_ip_a2n(char *pstrIP)
{
	int err;
	unsigned long ret;
    struct sockaddr_in *sinp;
	struct addrinfo	gsfnts, *result, *aip;

	if (!pstrIP)
		return -1;

    if(1 == inet_pton(AF_INET, pstrIP, &ret))
    	return htonl(ret);

	memset(&gsfnts, 0, sizeof(gsfnts));
	gsfnts.ai_family = AF_INET;

	if ((err = getaddrinfo(pstrIP, 0, &gsfnts, &result)) != 0)
	{
		printf("getaddrinfo error: %s\n", gai_strerror(err));
		return -1;
	}

	ret = 0xffffffff;
	for (aip = result; aip != NULL; aip = aip->ai_next)
	{
		if (aip->ai_family == AF_INET)
		{
			sinp = (struct sockaddr_in *)aip->ai_addr;
			ret = sinp->sin_addr.s_addr;
			break;
		}
	}
	
	freeaddrinfo(result);
	return htonl(ret);
}

void  reverse(char   s[])   
{   
	int   c,   i,   j;   

	for   (i   =   0,   j   =   strlen(s)-1;   i   <   j;   i++,   j--)
	{   
		c   =   s[i];   
		s[i]   =   s[j];   
		s[j]   =   c;   
	}   
} 

void   gsf_itoa(int   n,   char   s[])   
{   
    int   i,   sign;   

    if ((sign = n) < 0)     /*   record   sign   */   
    	n = -n;                     /*   make   n   positive   */   
    i = 0;   
    do{             /*   generate   digits   in   reverse   order   */   
    	s[i++] = n % 10 + '0';     /*   get   next   digit   */   
    }while ((n /= 10) > 0);         /*   delete   it   */   

	if (sign < 0)   
       s[i++] = '-';   
    s[i] = '\0';   
    reverse(s);   
}
unsigned short cal_chksum(unsigned short *addr, int len)
{
	int leave = len;
	int sum   = 0;
	unsigned short *w=addr;
	unsigned short answer=0;
    
	while(leave > 1)
	{
		sum += *w++;
		leave -= 2;
	}
     
	if( leave == 1)
	{       
		*(unsigned char *)(&answer) = *(unsigned char *)w;
		sum += answer;
	}
    
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
    
	return answer;
}
