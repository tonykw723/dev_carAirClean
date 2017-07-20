/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>
#include <string.h>

#include "xtsys_type.h"
#include "xtsys_cfg.h"

#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"
#include "xtos_sem.h"

#include "xt_bsp_rtu09c.h"

#include "xt_conn.h"
#include "conn_msg.h"

#include "xt_dev.h"
#include "dev_rise.h"

#include "pro_dev_iot.h"

#include "global_para.h"
#include "pro_ap.h"
#include "xt_pro_ap.h"
#include "pro_ap_iot.h"
#include "pro_ap_hz.h"

#include "ap.h"

static os_queue_t msgHandle = NULL;
static os_queue_t msgRespHandle = NULL;
static os_mutex_t connMsgMutex = NULL;

static pro_ap_info_t msgData = {0};

xt_void ConnMsgInit()
{
	if (msgHandle == NULL)
		msgHandle = QueueCreateXTOS(CONN_MSG_QUEUE_LENGTH, sizeof(pro_ap_info_t));
	if (msgRespHandle == NULL)
		msgRespHandle = QueueCreateXTOS(CONN_MSG_QUEUE_LENGTH, sizeof(pro_ap_info_t));
	if (connMsgMutex == NULL)
		MutexCreateXTOS(connMsgMutex);
}

xt_s32 ConnMsgWrite(xt_u8* pData, xt_s32 dataLen)
{
	xt_s32 ret = FAILURE_XT;
	xt_s32 rVal = 0;

	MutexLockXTOS(connMsgMutex);
	MemMemcpyXTOS(msgData.proApData, pData, dataLen);
	msgData.proApDataLen = dataLen;
	ret = QueueSendXTOS(msgHandle, &msgData);
	if(ret == SUCCESS_XT)
		rVal = dataLen;
	else
		rVal = 0;
	MutexUnlockXTOS(connMsgMutex);
	return rVal;
}

xt_s32 ConnMsgRead(xt_u8* pData, xt_s32* pDataLen)
{
	xt_s32 ret = FAILURE_XT;
	xt_s32 rVal = 0;

	MutexLockXTOS(connMsgMutex);
	ret = QueueReceiveXTOS(msgHandle, &msgData, CONN_MSG_TIMEOUT);
	if(ret == SUCCESS_XT)
	{
		MemMemcpyXTOS(pData, msgData.proApData, msgData.proApDataLen);
		*pDataLen = msgData.proApDataLen;
		rVal = msgData.proApDataLen;
	}
	else
	{
		*pDataLen = 0;
		rVal = 0;
	}
	MutexUnlockXTOS(connMsgMutex);

	return rVal;
}

xt_s32 ConnMsgRespWrite(xt_u8* pData, xt_s32 dataLen)
{
	xt_s32 ret = FAILURE_XT;
	xt_s32 rVal = 0;

	MutexLockXTOS(connMsgMutex);
	MemMemcpyXTOS(msgData.proApData, pData, dataLen);
	msgData.proApDataLen = dataLen;
	ret = QueueSendXTOS(msgRespHandle, &msgData);
	if(ret == SUCCESS_XT)
		rVal = dataLen;
	else
		rVal = 0;
	MutexUnlockXTOS(connMsgMutex);

	return rVal;
}

xt_s32 ConnMsgRespRead(xt_u8* pData, xt_s32* pDataLen)
{
	xt_s32 ret = FAILURE_XT;
	xt_s32 rVal = 0;

	MutexLockXTOS(connMsgMutex);
	ret = QueueReceiveXTOS(msgRespHandle, &msgData, CONN_MSG_TIMEOUT);
	if(ret == SUCCESS_XT)
	{
		MemMemcpyXTOS(pData, msgData.proApData, msgData.proApDataLen);
		*pDataLen = msgData.proApDataLen;
		rVal = msgData.proApDataLen;
	}
	else
	{
		*pDataLen = 0;
		rVal = 0;
	}
	MutexUnlockXTOS(connMsgMutex);

	return rVal;
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
