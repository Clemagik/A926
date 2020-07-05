#ifndef _DATA_MEMPOOL_H
#define _DATA_MEMPOOL_H

#include "BaseType.h"

#if defined(__cplusplus)
extern "C" {
#endif
#define MPOOL_SIZE_LV0	(4 * 1024)
#define MPOOL_SIZE_LV1	(8 * 1024)
#define MPOOL_SIZE_LV2	(16 * 1024)
#define MPOOL_SIZE_LV3	(32 * 1024)
#define MPOOL_SIZE_LV4	(64 * 1024)
#define MPOOL_SIZE_LV5	(128 * 1024)
#define MPOOL_SIZE_LV6	(256 * 1024)
#define MPOOL_SIZE_LV7	(512 * 1024)
#define MPOOL_SIZE_LV8	(1024 * 1024)
#define MPOOL_SIZE_LV9	(2048 * 1024)
#define MPOOL_MAX_SIZE MPOOL_SIZE_LV9
#define MPOOL_SIZE_LVS	10
#define MPOOL_GRP_SIZE	256
#define MGRP_SIZE_LV0	64
#define MGRP_SIZE_LV1	32
#define MGRP_SIZE_LV2	64
#define MGRP_SIZE_LV3	64
#define MGRP_SIZE_LV4	64
#define MGRP_SIZE_LV5	32
#define MGRP_SIZE_LV6	32
#define MGRP_SIZE_LV7	2
#define MGRP_SIZE_LV8	1
#define MGRP_SIZE_LV9	1

typedef struct
{
    void * m_pOwner;
    INT32 m_iMemSize;
    char m_bData[8];		// °üÊý¾Ý
} MemPackage;


typedef struct
{
	INT32 m_iCnt;
	INT32 m_iMallocCnt;
	INT32 m_iMaxSize;
	P2PLock m_tLock;
	MemPackage *m_ptMems[MPOOL_GRP_SIZE];
} MemGroup;

typedef struct
{
	INT32 m_iMaxMemSize;
	INT32 m_iSeq;
	INT32 m_iSizeAlloc;
	P2PLock m_tLock;
	MemGroup m_tGrps[MPOOL_SIZE_LVS];
}MemPool;




DLL_DECLARE void * MemAllloc(INT32 _iSize);

DLL_DECLARE INT32 MemFree(void * pData);



#ifdef __cplusplus
}
#endif


#endif

