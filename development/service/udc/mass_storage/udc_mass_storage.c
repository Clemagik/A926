#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <linux/netlink.h>

#define SLEEP_TIME		1
#define EVENT_PATH_LENGTH	100
#define USB_DEVICE_STATUS	"/sys/class/android_usb/f_mass_storage/device/state"

typedef struct {
	struct nlmsghdr hdr;
	char buf[EVENT_PATH_LENGTH];
} NETLINK_DATA;

int netlink_init() {

	struct sockaddr_nl nladdr;
	int sz = 64 * 1024;
	int on = 1;
	int mSock = 0;

	memset(&nladdr, 0, sizeof(nladdr));
	nladdr.nl_family = AF_NETLINK;
	nladdr.nl_pid = getpid();
	nladdr.nl_groups = 0xffffffff;

	if ((mSock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT)) < 0) {
	        perror("Unable to create uevent socket");
		    return -1;
	}

	if (setsockopt(mSock, SOL_SOCKET, SO_RCVBUFFORCE, &sz, sizeof(sz)) < 0) {
	        perror("Unable to set uevent socket SO_RCVBUFFORCE option");
		    goto fail;
	}

	if (setsockopt(mSock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)) < 0) {
	        perror("Unable to set uevent socket SO_PASSCRED option");
		    goto fail;
	}

	if (bind(mSock, (struct sockaddr *) &nladdr, sizeof(nladdr)) < 0) {
	        perror("Unable to bind uevent socket");
		    goto fail;
	}

	return mSock;

fail:
	close(mSock);
	return -1;
}

int main() {

	int socket_id = 0, usb_fd = 0;
	int ret = 0;
	fd_set inputs, testfds;
	struct timeval timeout = {SLEEP_TIME, 0};
	NETLINK_DATA event_buf;
	struct sockaddr_nl kerneladdr;

	memset(&kerneladdr, 0, sizeof(kerneladdr));
	kerneladdr.nl_family = AF_NETLINK;
	kerneladdr.nl_pid = 0;
	kerneladdr.nl_groups = 0;

	if(ret = access(USB_DEVICE_STATUS, F_OK)) {
		perror(USB_DEVICE_STATUS);
		return ret;
	}

	if((usb_fd = open(USB_DEVICE_STATUS, O_RDONLY)) < 0) {
		perror("open usb device status file error");
		return -1;
	}

	if((socket_id = netlink_init()) < 0) {
		printf("netlink init error\n");
		ret = -1;
		goto out;
	}

	FD_ZERO(&inputs);
	FD_SET(socket_id, &inputs);
	while(1) {
		memset(&event_buf, 0, sizeof(event_buf));
		FD_ZERO(&testfds);
		testfds = inputs;
		timeout.tv_sec = SLEEP_TIME;
		ret = select(socket_id + 1, &testfds, NULL, NULL, &timeout);
		switch(ret) {
		    case -1 :
			    perror("select error");
		    case  0 :
			    continue;
		    default :
			if(FD_ISSET(socket_id, &testfds)) {
				int ker_addr_size = sizeof(kerneladdr);
				ret = recvfrom(socket_id, &event_buf, sizeof(event_buf), 0, (struct sockaddr *)&kerneladdr, &ker_addr_size);
				if(ret <= sizeof(struct nlmsghdr)) {
					perror("recv from event error");
					continue;
				}
			}
		}
		if(!strcmp(event_buf.buf, "virtual/android_usb/android0")) {
			char buf[30] = {0};
			char *data = NULL;
			while(1) {
				if((ret = read(usb_fd, buf, sizeof(buf))) < 0) {
					perror("read usb device status error");
					continue;
				}
				if(ret == 0) {
					lseek(usb_fd, 0, SEEK_SET);
					continue;
				}
				buf[ret] = 0;
				if(data = strchr(buf, '\n'))
					*data = 0;
				if(!strcmp(buf, "CONFIGURED") || !strcmp(buf, "CONNECTED")) {
					printf("usb tigger configure or connect event\n");
					ret = 0;
					goto out;
				} else {
					if(!strcmp(buf, "DISCONNECTED")) {
					    printf("usb tigger disconnect event\n");
					    ret = 1;
					    goto out;
					} else {
						memset(buf, 0, sizeof(buf));
						lseek(usb_fd, 0, SEEK_SET);
						continue;
					}
				}
			}
		}
	}

out:
	close(socket_id);
fail:
	close(usb_fd);
	return ret;

}
