#ifndef __RINGQUEUE_H_
#define __RINGQUEUE_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

typedef struct _ringqueue_t
{
	xt_u16		qin;
	xt_u16		qout;
	xt_u8*		buffaddr;
	xt_u16		buffsize;
}ringqueue_t,*pringqueue_t;

 ringqueue_t* RQueueNew(xt_u8* buf,xt_u16 size);

 xt_s32 RQueueWriteInByte(ringqueue_t* pQ, xt_u8 pData);
 

 xt_s32 RQueueReadOutByte(ringqueue_t* pQ,xt_u8 *pData);


 xt_s32 RQueueIsEmpty(ringqueue_t* pQ);
 xt_s32 RQueueIsFull(ringqueue_t* pQ);

 xt_s32 RQueueFree(ringqueue_t* pQ);
 


#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __RINGQUEUE_H_
 
