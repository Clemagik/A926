#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <signal.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/netlink.h>

#include "tiziano_params.h"
#include "tiziano_params_operate.h"

#define NETLINK_USER    22
#define USER_MSG        (NETLINK_USER + 1)
#define MSG_LEN         (1024*64)
#define MAX_PLOAD       (1024*64)

typedef struct {
	char *name;
	char *str_help;
} help_info;

enum {
	TZCTRL_CMD_ID_ARRAY_DUMP_ID,
	TZCTRL_CMD_ID_ARRAY_GET,
	TZCTRL_CMD_ID_ARRAY_SET,
	TZCTRL_CMD_ID_AE_ARRAY_GET,
	TZCTRL_CMD_ID_AWB_ARRAY_GET,
	TZCTRL_CMD_ID_END
};

static struct option long_options[] = {
	[TZCTRL_CMD_ID_ARRAY_DUMP_ID] = {"array_dump_id", no_argument, 0, 0},
	[TZCTRL_CMD_ID_ARRAY_GET] = {"array_get", required_argument, 0, 0},
	[TZCTRL_CMD_ID_ARRAY_SET] = {"array_set", required_argument, 0, 0},
	[TZCTRL_CMD_ID_AE_ARRAY_GET] = {"ae_array_get", no_argument, 0, 0},
	[TZCTRL_CMD_ID_END] = {0, 0, 0, 0}
};

static help_info hinfo[] = {
	[TZCTRL_CMD_ID_ARRAY_DUMP_ID] = {"array_dump_id", "dump array id"},
	[TZCTRL_CMD_ID_ARRAY_GET] = {"array_get", "get array data, <id>"},
	[TZCTRL_CMD_ID_ARRAY_SET] = {"array_set", "set array data, <id>:<datafile>"},
	[TZCTRL_CMD_ID_AE_ARRAY_GET] = {"ae_array_get", "get ae array data"},
	[TZCTRL_CMD_ID_AWB_ARRAY_GET] = {"awb_array_set", "get awb array data"},
	[TZCTRL_CMD_ID_END] = {0, 0}
};

void help(int argc, char **argv)
{
	int cnt = 0;
	int i = 0;
	printf("usage: %s --option [args]\n", argv[0]);
	for (i = 0; i < TZCTRL_CMD_ID_END; i++) {
		printf("\t%-20s\t\t%s\n", hinfo[i].name, hinfo[i].str_help);
	}
}

int32_t tzctrl_array_dump_id(void);
int tzctrl_ae_param_dump(void);
int32_t tzctrl_param_get_cnt(uint32_t id);
int32_t tzctrl_param_set(uint32_t id, uint32_t *ptr, uint32_t cnt);
int32_t tzctrl_param_dump(uint32_t id);

int tzctrl_parse_arg(int argc, char **argv)
{
	int c;
	if (1 == argc) {
		help(argc, argv);
		return -1;
	}

	while (1) {
		int option_index = 0;
		c = getopt_long(argc, argv, "", long_options, &option_index);
		/* end */
		if ((-1 == c)||('?' == c)) {
			break;
		}
		switch (option_index) {
		case TZCTRL_CMD_ID_ARRAY_DUMP_ID:
		{
			tzctrl_array_dump_id();
		}
		break;
		case TZCTRL_CMD_ID_ARRAY_GET:
		{
			uint32_t id = 0;
			int32_t ret = 0;
			//printf("optarg = %s\n", optarg);
			ret = sscanf(optarg, "%d", &id);
			if (1 != ret) {
				printf("err: param id err \n");
			} else {
				tzctrl_param_dump(id);
			}
		}
		break;
		case TZCTRL_CMD_ID_ARRAY_SET:
		{
			int32_t id = 0;
			int32_t ret = 0;
			int32_t i = 0;
			int32_t cnt = 0;
			int32_t *ptr = NULL;
			int8_t file[50];
			FILE *fp;
			char num[50];
			//printf("optarg = %s\n", optarg);
			ret = sscanf(optarg, "%d:%s", &id, file);
			if (2 != ret) {
				printf("err: param id err \n");
				return -1;
			}
			fp = fopen(file, "r");
			if (NULL == fp) {
				printf("err: file open err \n");
				return -1;
			}
			cnt = tzctrl_param_get_cnt(id);
			ptr = malloc(cnt*sizeof(uint32_t));
			if (NULL == ptr) {
				printf("err: malloc error \n");
				return -1;
			}
			for (i = 0; i < cnt; i++) {
				char *end = NULL;
				uint32_t x = 0;
				ret = fscanf(fp, "%s", num);
				if (1 != ret) {
					printf("err: param id err \n");
					return -1;
				}
				x = strtol(num, &end, 0);
				if ((num+strlen(num)) != end) {
					printf("err: data error \n");
					return -1;
				}
				ptr[i] = x;
				//printf("%s-0x%x %p-%p, %d\n", num,x, num, end, strlen(num));
			}
			tzctrl_param_set(id, ptr, cnt);
			free(ptr);
		}
		break;
		case TZCTRL_CMD_ID_AE_ARRAY_GET:
			tzctrl_ae_param_dump();
			break;
		default:
			printf("getopt_long returned c %d\n", c);
			break;
		}
	}
	return 0;
}

typedef struct {
	int skfd;
	struct sockaddr_nl  local;
	struct nlmsghdr *nlh;
} nl_handle;

void *tzctrl_nl_init(void)
{
	int skfd;
	struct sockaddr_nl  local;
	struct nlmsghdr *nlh = NULL;
	nl_handle *h = malloc(sizeof(nl_handle));
	if (NULL == h) {
		printf("malloc error...%s\n", strerror(errno));
		return NULL;
	}

	skfd = socket(AF_NETLINK, SOCK_RAW, USER_MSG);
	if(skfd == -1)
	{
		printf("%s,%d: create socket error...%s\n",
		       __func__, __LINE__, strerror(errno));
		return NULL;
	}
	memset(&local, 0, sizeof(local));
	local.nl_family = AF_NETLINK;
	local.nl_pid = 50;
	local.nl_groups = 0;
	if(bind(skfd, (struct sockaddr *)&local, sizeof(local)) != 0)
	{
		printf("bind() error\n");
		close(skfd);
		return NULL;
	}
	nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PLOAD));
	if (NULL == nlh) {
		printf("malloc error...%s\n", strerror(errno));
		return NULL;
	}
	h->skfd = skfd;
	h->local = local;
	h->nlh = nlh;
	return h;
}

int tzctrl_nl_send_msg(void *h, void *buf, int len)
{
	struct sockaddr_nl  local, dest_addr;
	nl_handle *handle = h;
	socklen_t dest_addrlen = sizeof(dest_addr);
	int skfd;
	struct nlmsghdr *nlh = handle->nlh;
	int ret;

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0;
	dest_addr.nl_groups = 0;

	memset(nlh, 0, sizeof(struct nlmsghdr));
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PLOAD);
	nlh->nlmsg_flags = 0;
	nlh->nlmsg_type = 0;
	nlh->nlmsg_seq = 0;
	nlh->nlmsg_pid = handle->local.nl_pid; //self port
	memcpy(NLMSG_DATA(nlh), buf, len);
	ret = sendto(handle->skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_nl));
	if(!ret) {
		printf("sendto error1\n");
		close(handle->skfd);
	}
	return ret;
}

int tzctrl_nl_recv_msg(void *h, void *buf, int len)
{
	int ret = 0;
	nl_handle *handle = h;
	struct sockaddr_nl  dest_addr;
	struct nlmsghdr *nlh = handle->nlh;
	socklen_t dest_addrlen = sizeof(dest_addr);
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0;
	dest_addr.nl_groups = 0;
	//printf("wait kernel msg!\n");
	ret = recvfrom(handle->skfd, nlh, len, 0, (struct sockaddr *)&dest_addr, &dest_addrlen);
	if (-1 == ret) {
		printf("recv form kernel error\n");
		return ret;
	}
	int datalen=ret-NLMSG_HDRLEN;
	memcpy(buf, NLMSG_DATA(nlh), datalen);
	return datalen;
}

tisp_param_op_msg_t *_msg_alloc(int payload)
{
	return malloc(sizeof(tisp_param_op_msg_t)+payload);
}

void _msg_free(tisp_param_op_msg_t *msg)
{
	free(msg);
}

extern tisp_param_info_t  all_params_info[TISP_PARAM_TOTAL_SIZE];

int32_t tzctrl_param_get_cnt(uint32_t id)
{
	return all_params_info[id].cols*all_params_info[id].rows;
}

int32_t tzctrl_param_set(uint32_t id, uint32_t *ptr, uint32_t cnt)

{
	int32_t ret = 0;
	int32_t i = 0;
	int32_t len = 0;
	tisp_param_op_msg_t *msg_send = _msg_alloc(16*1024);
	tisp_param_op_msg_t msg_recv;
	void *handle = tzctrl_nl_init();
	if (NULL == handle) {
		printf("%s,%d: tzctrl_nl_init error\n", __func__, __LINE__);
		return -1;
	}
	msg_send->msg_type = TISP_PARAM_OP_TYPE_ARRAY_SET;

	i = id;
	{
		int j,k;
		int rows,cols;
		uint32_t *buf;
		msg_send->msg_data1 = i;
		rows = all_params_info[i].rows;
		cols = all_params_info[i].cols;
		buf = (uint32_t *)msg_send->msg_buf;
		memcpy(buf, ptr, cnt*sizeof(uint32_t));

		printf("%s:\n", all_params_info[i].name);
		for (j = 0; j < rows; j++) {
			for (k = 0; k < cols; k++) {
				printf("0x%x ", buf[j*k+k]);
			}
			printf("\n");
		}
		tzctrl_nl_send_msg(handle, msg_send, sizeof(tisp_param_op_msg_t)+cnt*sizeof(uint32_t));
		tzctrl_nl_recv_msg(handle, &msg_recv, (sizeof(tisp_param_op_msg_t)));
	}
}

int32_t tzctrl_array_dump_id(void)
{
	int32_t i = 0;
	for (i = 0; i < TISP_PARAM_TOTAL_SIZE; i++) {
		printf("%d: %s\n", i, all_params_info[i].name);
	}
	return 0;
}

int32_t tzctrl_param_dump(uint32_t id)
{
	int32_t ret = 0;
	int32_t i = 0;
	int32_t len = 0;
	tisp_param_op_msg_t msg_send;
	tisp_param_op_msg_t *msg_recv = _msg_alloc(16*1024);
	void *handle = tzctrl_nl_init();
	if (NULL == handle) {
		printf("%s,%d: tzctrl_nl_init error\n", __func__, __LINE__);
		return -1;
	}
	msg_send.msg_type = TISP_PARAM_OP_TYPE_ARRAY_GET;

	i = id;
	{
		int j,k;
		int rows,cols;
		uint32_t *buf;
		msg_send.msg_data1 = i;
		tzctrl_nl_send_msg(handle, &msg_send, sizeof(tisp_param_op_msg_t));
		len = tzctrl_nl_recv_msg(handle, msg_recv, (sizeof(tisp_param_op_msg_t)+16*1024));
		rows = all_params_info[i].rows;
		cols = all_params_info[i].cols;
		buf = (uint32_t *)msg_recv->msg_buf;
		len-=sizeof(tisp_param_op_msg_t);

		printf("%s:\n", all_params_info[i].name);
		//printf("len = %d, rows = %d, cols = %d\n", len, rows, cols);
		for (j = 0; j < rows; j++) {
			for (k = 0; k < cols; k++) {
				printf("0x%x ", buf[j*k+k]);
			}
			printf("\n");
		}
	}
}

int32_t tzctrl_ae_param_dump(void)
{
	int32_t ret = 0;
	int32_t i = 0;
	int32_t len = 0;
	tisp_param_op_msg_t msg_send;
	tisp_param_op_msg_t *msg_recv = _msg_alloc(16*1024);
	void *handle = tzctrl_nl_init();
	if (NULL == handle) {
		printf("%s,%d: tzctrl_nl_init error\n", __func__, __LINE__);
		return -1;
	}
	msg_send.msg_type = TISP_PARAM_OP_TYPE_ARRAY_GET;

	for (i=TISP_PARAM_AE_START; i<=TISP_PARAM_AE_END; i++) {
		int32_t j,k;
		int32_t rows,cols;
		uint32_t *buf;
		msg_send.msg_data1 = i;
		tzctrl_nl_send_msg(handle, &msg_send, sizeof(tisp_param_op_msg_t));
		len = tzctrl_nl_recv_msg(handle, msg_recv, (sizeof(tisp_param_op_msg_t)+16*1024));
		rows = all_params_info[i].rows;
		cols = all_params_info[i].cols;
		buf = (uint32_t *)msg_recv->msg_buf;
		len-=sizeof(tisp_param_op_msg_t);

		printf("%s:\n", all_params_info[i].name);
		//printf("len = %d, rows = %d, cols = %d\n", len, rows, cols);
		for (j = 0; j < rows; j++) {
			for (k = 0; k < cols; k++) {
				printf("0x%x ", buf[j*k+k]);
			}
			printf("\n");
		}
	}
}

int main(int argc, char **argv)
{
	int ret = 0;
	ret = tzctrl_parse_arg(argc, argv);
	return ret;
	return 0;

}
