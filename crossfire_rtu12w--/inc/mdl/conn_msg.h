/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __CONN_MSG_H_
#define __CONN_MSG_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#define CONN_MSG_QUEUE_LENGTH	(1)
#define CONN_MSG_TIMEOUT		(10)

#ifdef WIN32
xt_u32 win32msgRespThreadID;
#endif

xt_void ConnMsgInit(void);

#define ConnMsgOpen				NULL

#define ConnMsgClose			NULL

xt_s32 ConnMsgWrite				(xt_u8* pData, xt_s32 dataLen);

xt_s32 ConnMsgRead				(xt_u8* pData, xt_s32* pDataLen);

xt_s32 ConnMsgRespWrite			(xt_u8* pData, xt_s32 dataLen);

xt_s32 ConnMsgRespRead			(xt_u8* pData, xt_s32* pDataLen);

#define ConnMsgCtrl				NULL

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __CONN_MSG_H_

