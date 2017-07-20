/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __XT_CONN_H_
#define __XT_CONN_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

typedef xt_s32	(*pxConnWrite)(xt_u8*, xt_s32);

typedef xt_s32	(*pxConnRead)(xt_u8*, xt_s32*);

typedef struct
{
	xt_u8		connId;

	xt_void		(*xConnInit)(void);

	xt_void		(*xConnOpen)(void);

	xt_void		(*xConnClose)(void);

	//connection write
	xt_s32		(*xConnWrite)(xt_u8* pData ,xt_s32 dataLen);

	//connection read
	xt_s32		(*xConnRead)(xt_u8* pData, xt_s32* pDataLen);

	//virtual connection write
	xt_s32		(*xConnRespWrite)(xt_u8* pData ,xt_s32 dataLen);

	//virtual connection read
	xt_s32		(*xConnRespRead)(xt_u8* pData, xt_s32* pDataLen);

	xt_void		(*xConnCtrl)(void);
} xt_conn_t;

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XT_CONN_H_

