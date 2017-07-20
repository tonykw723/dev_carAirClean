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

#include "xt_bsp_rtu09c.h"

#include "drv_uart.h"

#include "conn_uart.h"

#include "xt_conn.h"
#include "xt_dev.h"

#include "pro_dev_iot.h"

#include "dev_rise.h"
#include "global_para.h"

#include "lib_st_aes.h"
extern rtu_para_t glbRtuPara;

#define BUFFER_LENGTH (256+32)

xt_u8 *sendBuf=NULL;//[BUFFER_LENGTH];
xt_s32 sendLen = 0;

static pro_pkt_parse_t	devRiseRecvData = {0};

#define DEV_RSC_USE_ENCRYPT

#ifdef DEV_RSC_USE_ENCRYPT
//static xt_u8 encryRaw[BUFFER_LENGTH] = {0};
static xt_u8 *encryBuf=NULL;//[BUFFER_LENGTH] = {0};
static xt_u32 encryLen = 0;

//static xt_u8 encrySendbuf[BUFFER_LENGTH];
static xt_u32 encrySendLen = 0;
#endif


xt_void  DevRisecommInit(void)
{
	glbRtuPara.devPara.flag_dev_ini=YES;
	if(sendBuf==NULL)
		sendBuf=MemMallocXTOS(BUFFER_LENGTH);
	#ifdef DEV_RSC_USE_ENCRYPT
	if(encryBuf==NULL)
		encryBuf=MemMallocXTOS(BUFFER_LENGTH);
	#endif

}

xt_s32	DevRisecommSend(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead)
{
	ctrl_data_in_t di = {0};
	xt_u32 j = 0;
	di.pDataBuf = pData;
	di.dataLen = (xt_u8)dataLen;

	//todo: check for groupcast...

	sendLen = BUFFER_LENGTH;
	ProDevIotPack(pDevParam->commDst, pDevParam->commCmd, &di, sendBuf, &sendLen);

	if (sendLen > 0)
	{
		if (pxWrite != NULL)
		{
			PrintfXTOS("DevRisecommSend : \n");
			PrintfXTOS("-------------DevRisecommSend : --------------\n");
			for (j = 0; j < sendLen; j++) {
				PrintfXTOS("%0.2x ",sendBuf[j]);
			}
			PrintfXTOS("\n");			
			PrintfXTOS("\n-------------------------------------------\n");
#ifdef DEV_RSC_USE_ENCRYPT
			
			//MemMemsetXTOS(encryRaw, 0, BUFFER_LENGTH);
			MemMemsetXTOS(encryBuf, 0, BUFFER_LENGTH);
			//MemMemsetXTOS(encrySendbuf, 0, BUFFER_LENGTH);
			encrySendLen = BUFFER_LENGTH;
			encryLen = 0;

			//MemMemcpyXTOS(encryRaw, (xt_void *)sendBuf, sendLen);
			encryLen = (sendLen % 16 == 0) ? (sendLen / 16):(sendLen / 16 + 1);
			encryLen = encryLen * 16;
			
			IotAesEncryptEcb(sendBuf, encryLen, encryBuf);

			di.pDataBuf = encryBuf;
			di.dataLen = encryLen;
			
			ProDevIotPack(pDevParam->commDst, pDevParam->commCmd, &di,
				sendBuf, &encrySendLen);

			PrintfXTOS("DevRisecommSend Aes: \n");
			PrintfXTOS("-------------DevRisecommSend Aes: --------------\n");
			for (j = 0; j < encrySendLen; j++) {
				PrintfXTOS("%0.2x ",sendBuf[j]);
			}
			PrintfXTOS("\n");			
			PrintfXTOS("\n-------------------------------------------\n");
			return pxWrite(sendBuf, encrySendLen);
#else
			return pxWrite(sendBuf, sendLen);
#endif
		}
	}
	return FAILURE_XT;
}


xt_s32	DevRisecommRecv(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam)
{
	static xt_u8 copyFlag = TRUE; 
	packet_head_t* pHead = NULL;
	xt_u8* pSubCmdOffset = NULL;
	xt_u8* pModBuf = NULL;
	xt_u32 modLen = 0;
	//memset((xt_u8 *)&devRiseRecvData,0,sizeof(devRiseRecvData));
	if (copyFlag == TRUE)
	{
		if (devRiseRecvData.chkOff > 0)
			MemMemcpyXTOS(devRiseRecvData.chkBuf, devRiseRecvData.chkBuf + devRiseRecvData.chkOff,
			devRiseRecvData.chkLen);
				
		if(devRiseRecvData.chkLen + dataLen <= PRO_PKT_MAX_SIZE)
		{
			MemMemcpyXTOS(devRiseRecvData.chkBuf + devRiseRecvData.chkLen, pData, dataLen);
			devRiseRecvData.chkLen += dataLen;
		}
		
		MDBG("DevRisecommOnRecv: recv raw data = %d \n", dataLen);

		copyFlag = FALSE;
	}

	if (ProDevIotParse(&devRiseRecvData) == SUCCESS_XT) {
		if (devRiseRecvData.pktLen == 0)
		{
			copyFlag = TRUE;
			return FAILURE_XT;
		}
#ifdef DEV_RSC_USE_ENCRYPT
		//MemMemsetXTOS(encryRaw, 0, BUFFER_LENGTH);
		MemMemsetXTOS(encryBuf, 0, BUFFER_LENGTH);
		encryLen = 0;

		pHead = (packet_head_t*)devRiseRecvData.pktBuf;

		if (pHead->cmd == CMD_UNICAST || pHead->cmd == CMD_BROADCAST || pHead->cmd == CMD_GRPCAST)
		{
			//MemMemcpyXTOS(encryRaw, (xt_void *)(devRiseRecvData.pktBuf + sizeof(packet_head_t)), pHead->len);
			encryLen = (pHead->len % 16 == 0) ? 
				(pHead->len / 16):(pHead->len / 16 + 1);
			encryLen = encryLen * 16;
			if(pHead->len < 16)
			{
				devRiseRecvData.pktLen = 0;
				return FAILURE_XT;
			}

			IotAesDecryptEcb(devRiseRecvData.pktBuf + sizeof(packet_head_t), encryLen, encryBuf);

			pHead = (packet_head_t*)encryBuf;

			if(UTCheckProData((packet_head_t*)encryBuf) != SUCCESS_XT){
				devRiseRecvData.pktLen = 0;
				return FAILURE_XT;
			}
			pModBuf = encryBuf;
			modLen = pHead->len + PACK_MIN_SIZE;
		}
		else
		{
			pModBuf = devRiseRecvData.pktBuf;
			modLen = devRiseRecvData.pktLen;
		}
		
#else
		pModBuf = devRiseRecvData.pktBuf;
		modLen = devRiseRecvData.pktLen;
#endif

		MDBG("DevRisecommOnRecv: recv packSize = %d \n", modLen);

		pHead = (packet_head_t*)pModBuf;

		pDevParam->commCmd = pHead->cmd;

		MemMemcpyXTOS((xt_void *)&pDevParam->commDst, (xt_void *)pHead->devaddr, DEV_ADDR_BYTES);
		pDevParam->commDst = pDevParam->commDst << 16; // uid store low 48 bit

		pSubCmdOffset = pModBuf + PACK_OFFSET_DATA;

#ifdef DEV_RSC_USE_ENCRYPT
		if (pHead->cmd == CMD_UNICAST || pHead->cmd == CMD_BROADCAST)
		{
			pDevParam->commGroup = 0;

			MemMemcpyXTOS(pRetData, pSubCmdOffset, modLen - PACK_MIN_SIZE);
			*pRetDataLen = modLen - PACK_MIN_SIZE;
		}
		else if(pHead->cmd == CMD_GRPCAST)
		{
			pDevParam->commGroup = *(pSubCmdOffset);
			pSubCmdOffset ++;

			MemMemcpyXTOS(pRetData, pSubCmdOffset, modLen - 1 - PACK_MIN_SIZE);
			*pRetDataLen = modLen - 1 - PACK_MIN_SIZE;
		}
#else
		pDevParam->commGroup = 0;

		MemMemcpyXTOS(pRetData, pSubCmdOffset, modLen - PACK_MIN_SIZE);
		*pRetDataLen = modLen - PACK_MIN_SIZE;
#endif

		//todo: check for groupcast...

		devRiseRecvData.pktLen = 0;
		return SUCCESS_XT; 
	}
	else
	{
		copyFlag = TRUE;
		return FAILURE_XT;
	}
	
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
