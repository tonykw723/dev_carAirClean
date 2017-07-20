#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include "xtsys_type.h"
#include "ringqueue.h"
#include "xtos_mem.h"

static void ringPlusIn(ringqueue_t* pQ);

 ringqueue_t* RQueueNew(xt_u8* buf,xt_u16 size)
{
	ringqueue_t * pNode = (ringqueue_t*)MemMallocXTOS(sizeof(ringqueue_t));
	
	if (NULL == pNode) 
		return NULL;

	pNode->qin= 0;
	pNode->qout= 0;
	pNode->buffaddr= buf;
	pNode->buffsize = size;
	return pNode;
}

xt_s32 RQueueWriteInByte(ringqueue_t* pQ, xt_u8 pData)
{
	if(RQueueIsFull(pQ)==TRUE)
		return FALSE;

	pQ->buffaddr[pQ->qin++]=pData;

	if(pQ->qin>=pQ->buffsize)
		pQ->qin = 0 ;
	return TRUE;

}



xt_s32 RQueueReadOutByte(ringqueue_t* pQ,xt_u8 *pData)
{
	if (RQueueIsEmpty(pQ)==TRUE)
		return FALSE;

	*pData=pQ->buffaddr[pQ->qout++];

	if(pQ->qout>=pQ->buffsize)
		pQ->qout = 0 ;
	return TRUE;
}


xt_s32 RQueueIsEmpty(ringqueue_t* pQ)
{
	return (pQ->qin == pQ->qout);
}

xt_s32 RQueueIsFull(ringqueue_t* pQ)
{
	return (((pQ->qin+1)%pQ->buffsize)== pQ->qout);
}

xt_s32 RQueueFree(ringqueue_t* pQ)
{

	MemFreeXTOS(pQ);
	return TRUE;
}


#ifdef __cplusplus
}
#endif // End for __cplusplus

