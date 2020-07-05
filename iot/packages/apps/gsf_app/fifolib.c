#include "fifolib.h"


Data_FIFO *Make_FIFO(int _iSize,DATA_RELEASE _Release) 
{
	Data_FIFO *ptFifo = (Data_FIFO *)malloc(sizeof(Data_FIFO) + _iSize * sizeof(void *));

	if (ptFifo != NULL)
	{
		memset(ptFifo, 0, sizeof(*ptFifo));
		ptFifo->m_iMaxCnt= _iSize;		
		ptFifo->m_iByteSzie = 0;
		ptFifo->m_ReleaseFun= _Release;
		InitP2PLock(ptFifo->m_Lock);
	}
	return ptFifo;
}

void FIFOClear(Data_FIFO *_ptFifo)
{
    INT32 iIndx = 0;
    INT32 i=0;
    void * pData = NULL;
	if (_ptFifo != NULL)
	{
        LockP2PLock(_ptFifo->m_Lock);
		for (i = _ptFifo->m_iBase; i < _ptFifo->m_iTop; i++)
		{
			iIndx = i % _ptFifo->m_iMaxCnt;
			pData = _ptFifo->m_pData[iIndx];
            if(pData&&_ptFifo->m_ReleaseFun)
            {
                _ptFifo->m_ReleaseFun(pData);
            }
		}
		_ptFifo->m_iBase = 0;
		_ptFifo->m_iTop = 0;		
		_ptFifo->m_iByteSzie = 0;

        UnLockP2PLock(_ptFifo->m_Lock);
		
	}
}




DLL_DECLARE INT32  PushDataToFiFo(void *_pData, Data_FIFO *ptFifo)
{
    int iCnt = 0;
    int iRet=-1;
    int iIndx = 0;
    void * pOld = NULL;
	if (_pData != NULL && ptFifo != NULL)
	{
        LockP2PLock(ptFifo->m_Lock);
    insert:
		iCnt = ptFifo->m_iTop - ptFifo->m_iBase;
		if(iCnt < ptFifo->m_iMaxCnt)
		{
			iIndx = (ptFifo->m_iTop++) % ptFifo->m_iMaxCnt;
			ptFifo->m_pData[iIndx] = _pData;
			iRet = 0;
		}
		else
		{
            pOld = ptFifo->m_pData[(ptFifo->m_iBase++) % ptFifo->m_iMaxCnt];
			if (ptFifo->m_iBase == ptFifo->m_iTop)
			{
				ptFifo->m_iBase = 0;
				ptFifo->m_iTop = 0;
			}
            if(pOld&&ptFifo->m_ReleaseFun)
            {
                ptFifo->m_ReleaseFun(pOld);
            }
            goto insert;
		}
        UnLockP2PLock(ptFifo->m_Lock);
	}
	return iRet;
}



DLL_DECLARE void *GetDataFromFiFo(Data_FIFO *_ptFifo)
{
	void *ptPkg = NULL;
	if (_ptFifo != NULL)
	{
        LockP2PLock(_ptFifo->m_Lock);
		if (_ptFifo->m_iTop > _ptFifo->m_iBase)
		{
			ptPkg = _ptFifo->m_pData[(_ptFifo->m_iBase++) % _ptFifo->m_iMaxCnt];
			if (_ptFifo->m_iBase == _ptFifo->m_iTop)
			{
				_ptFifo->m_iBase = 0;
				_ptFifo->m_iTop = 0;
			}
		}      
        UnLockP2PLock(_ptFifo->m_Lock);
	}
	return ptPkg;
}

DLL_DECLARE int GetFifoByteNum( Data_FIFO *_ptFifo)
{
	if (_ptFifo)
	{
		return _ptFifo->m_iByteSzie;
	}
	else
	{
		return 0;
	}
}









