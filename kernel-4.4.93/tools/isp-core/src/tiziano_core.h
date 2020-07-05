#ifndef __TIZIANO_CORE_H__
#define __TIZIANO_CORE_H__
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include "isp-debug.h"

#include "tiziano_params.h"

#define TISP_VERSION_SIZE 8
#define TISP_VERSION_ID "1.03"
#define TISP_PRIV_PARAM_FLAG_SIZE	8
#define ISPSUBABS(A,B) ((A>B)?(A-B):(B-A))
#define ISPADDABS(X,X1,X2) ((X1>X2)?(X1-X):(X1+X))
#define ISPINT(X,X1,X2,Y1,Y2)    (ISPADDABS((ISPSUBABS(Y1,Y2) * ISPSUBABS(X,X1) / ISPSUBABS(X1,X2)),Y1,Y2))

extern tisp_params_t tparams;
extern tisp_params_t tparams_day;
extern tisp_params_t tparams_night;
extern int deir_en;
enum __tx_isp_private_parameters_index {
	TISP_PRIV_PARAM_BASE_INDEX,
	TISP_PRIV_PARAM_CUSTOM_INDEX,
	TISP_PRIV_PARAM_MAX_INDEX,
};

typedef struct tisp_private_parameters_header{
	char flag[TISP_PRIV_PARAM_FLAG_SIZE];
	unsigned int size;		/* the memory size of the parameter array */
	unsigned int crc;
} TXispPrivParamHeader;


typedef struct tiziano_parameters_manger {
	char version[TISP_VERSION_SIZE];
	TXispPrivParamHeader headers[TISP_PRIV_PARAM_MAX_INDEX];
	void *data;								//the base address of all data.
	unsigned int data_size;
	void *base_buf;							//the address of private0 data.
} TISPParamManage;


//extern tisp_info_t tispinfo;
#endif
