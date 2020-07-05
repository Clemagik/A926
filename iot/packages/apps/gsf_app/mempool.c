#include "mempool.h"


#define MEMPOOL_MAGIC_NUM 0x55aa5aa5

static MemPool * g_pMemPool=NULL;

static int g_iGrpSizes[] = {0, MPOOL_SIZE_LV0, MPOOL_SIZE_LV1, MPOOL_SIZE_LV2,MPOOL_SIZE_LV3,
	MPOOL_SIZE_LV4, MPOOL_SIZE_LV5, MPOOL_SIZE_LV6, MPOOL_SIZE_LV7,MPOOL_SIZE_LV8, MPOOL_SIZE_LV9};


#define MP_DEFAULT_SIZE		(6 * 1024 * 1024) 
//数据池创建
static MemPool *MemPoolCreate(INT32 _iMaxMemSize)	
{
    INT32 i = 0;
    MemGroup *ptGrp = NULL;
	MemPool *ptPool = (MemPool *)malloc(sizeof(MemPool));
	int m_iGrpSize[] = {MGRP_SIZE_LV0, MGRP_SIZE_LV1, MGRP_SIZE_LV2, MGRP_SIZE_LV3,
		MGRP_SIZE_LV4, MGRP_SIZE_LV5, MGRP_SIZE_LV6, MGRP_SIZE_LV7, MGRP_SIZE_LV8, MGRP_SIZE_LV9};

	if (ptPool != NULL)
	{
		memset(ptPool, 0, sizeof(*ptPool));
		ptPool->m_iSeq = 1;
		InitP2PLock(ptPool->m_tLock);

		for (i = 0; i < MPOOL_SIZE_LVS; i++)
		{
            ptGrp = &ptPool->m_tGrps[i];
			ptGrp->m_iMaxSize = m_iGrpSize[i];
			InitP2PLock(ptGrp->m_tLock);
		}
		//内存池最大多少
		ptPool->m_iMaxMemSize = (_iMaxMemSize <= 0) ? MP_DEFAULT_SIZE : _iMaxMemSize;
	}
	return ptPool;
}

static void MemGroupDestroy(MemGroup *_ptGrp)
{
	INT32 i = 0;
	while (i < _ptGrp->m_iCnt)
	{
		free(_ptGrp->m_ptMems[i++]);
	}
}

void MemPoolDestroy(MemPool *_ptPool)	//数据池销毁
{
    INT32 i;
	if (_ptPool != NULL)
	{

		for (i = 0; i < MPOOL_SIZE_LVS; i++)
		{
			MemGroupDestroy(_ptPool->m_tGrps + i);
		}

		free(_ptPool);
	}
}

// 待优化
 static int MemSizeToGroup(int _iSize, int *_piNewSize)
{
	int i;
	for (i = 0; i < MPOOL_SIZE_LVS; i++)
	{
		if (_iSize > g_iGrpSizes[i] && _iSize <= g_iGrpSizes[i + 1])
		{
			*_piNewSize = g_iGrpSizes[i + 1];
			return i;
		}
	}
	return -1;
}


 static MemPackage *MemPackageAlloc(INT32 _iSize)
{
	MemPackage *ptPkg = (MemPackage *)malloc(sizeof(MemPackage) + _iSize);
	if (ptPkg != NULL)
	{
		ptPkg->m_iMemSize = _iSize;
	}
	return ptPkg;
}

static MemPackage *MemPackageCreate(INT32 _iSize, MemPool *_ptPool)  //数据包创建
{
	INT32 iSize;
	MemPackage *ptPkg = NULL;
	MemGroup *ptGrp =NULL;
	INT32 iGid;
	if (_ptPool != NULL)
	{
		iGid = MemSizeToGroup(_iSize, &iSize);
		if (iGid >= 0)
		{
			ptGrp = &_ptPool->m_tGrps[iGid];
			LockP2PLock(ptGrp->m_tLock);
			if (ptGrp->m_iCnt > 0)
			{
				ptPkg = ptGrp->m_ptMems[--ptGrp->m_iCnt];
				UnLockP2PLock(ptGrp->m_tLock);
			}
			else
			{
				UnLockP2PLock(ptGrp->m_tLock);
				ptPkg = MemPackageAlloc(iSize);
				if (ptPkg)
				{
					ptGrp->m_iMallocCnt++;
				} 
			}
	
			if (ptPkg)
			{
				ptPkg->m_pOwner = _ptPool;
			}
		}
		else
		{
	        ptPkg = MemPackageAlloc(iSize);
		    if (ptPkg)
		    {		
			    ptPkg->m_pOwner = _ptPool;
		    }  
		}
	}

	return ptPkg;
}


static void MemPackageFree(MemPackage *_ptPkg) 
{	
	INT32 flg = 0;
    INT32 iGid;
    INT32 iSize;
    MemGroup *ptGrp;
    MemPool *ptPool = NULL;
	if (_ptPkg != NULL)
	{
	    ptPool = (MemPool *)_ptPkg->m_pOwner;	
		iGid = MemSizeToGroup(_ptPkg->m_iMemSize, &iSize);
		if (iGid < 0)
		{
		    flg = 1;
			goto EXIT;
		}
		ptGrp = ptPool->m_tGrps + iGid;

        LockP2PLock(ptGrp->m_tLock);
		if (ptGrp->m_iCnt >= ptGrp->m_iMaxSize || ptGrp->m_iCnt >= MPOOL_GRP_SIZE)
		{
			UnLockP2PLock(ptGrp->m_tLock);
			flg = 1;
			LockP2PLock(ptPool->m_tLock);
			ptPool->m_iSizeAlloc -= _ptPkg->m_iMemSize;
			UnLockP2PLock(ptPool->m_tLock);
		}
		else
		{
			ptGrp->m_ptMems[ptGrp->m_iCnt++] = _ptPkg;
			UnLockP2PLock(ptGrp->m_tLock);
		}
EXIT:		
		if (flg)
		{
			free(_ptPkg);
		}
	}	
}


void * MemAllloc(INT32 _iSize)
{
    MemPackage * pMemPkg = NULL;
    if(NULL==g_pMemPool)
    {
        g_pMemPool = MemPoolCreate(0);
    }
    pMemPkg = g_pMemPool ? MemPackageCreate(_iSize,g_pMemPool) : NULL;
    return pMemPkg ? &pMemPkg->m_bData : NULL;
}

INT32  MemFree(void * pData)
{
    MemPackage * pMemPkg = NULL;
    if(pData)
    {
        pMemPkg =(MemPackage *)((char *)pData-sizeof(INT32)-sizeof(void*));
        MemPackageFree(pMemPkg);
    }
    return 0;
}







