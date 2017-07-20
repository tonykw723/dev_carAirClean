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
#include "xtos_timer.h"

#include "xt_bsp_rtu09c.h"

#include "drv_uart.h"

#include "xt_conn.h"
#include "conn_uart.h"

#include "xt_dev.h"
#include "dev_rise.h"

#include "pro_dev_iot.h"

#include "global_para.h"
#include "pro_ap.h"
#include "xt_pro_ap.h"
#include "pro_ap_iot.h"

#include "ap.h"
#include "ap_comm.h"
#include "ap_rtu.h"
#include "ap_measure.h"
#include "misc_timer.h"

#include "config.h"
#include "drv_light.h"
#include "stm32l1xx_usart.h"
#include "key.h"
#include "do_task.h"

extern xt_conn_t gConnList[CONN_MAX];
extern xt_dev_t gDevList[DEV_MAX];
extern xt_pro_ap_t gProApList[PRO_AP_MAX];
extern rtu_para_t glbRtuPara;
ap_comm_priv_data_t gCommData = {0};

#define BUFFER_LENGTH (256+32)

//static xt_u8 connBuf[BUFFER_LENGTH];
xt_u8 *connBuf=NULL;
xt_s32 connLen = 0;

xt_u8 *odbBuf=NULL;
xt_s32 odbLen = 0;

//static xt_u8 devBuf[BUFFER_LENGTH];
xt_u8 *devBuf=NULL;
xt_s32 devLen = 0;

ap_msg_t apMsgData = {0};
//static ap_msg_t apMsgRecvData = {0};

static xt_void apCommOnSendCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	xt_u8 proApCmd = pApMsg->proAp.proApData[0];

	PrintfXTOS("apCommOnSendCmd proApCmd = %d ID=%d \n",proApCmd,pApMsg->msgInfo.proApId);

	MemMemsetXTOS(connBuf, 0, sizeof(connBuf));
	connLen = BUFFER_LENGTH;

	if (pApMsg->msgInfo.proApId < PRO_AP_MAX)
	{
		if(gProApList[pApMsg->msgInfo.proApId].xProApPack != NULL)
		{
			//PrintfXTOS("devID1=%d,connLen=%d\n",pApMsg->msgInfo.devId,connLen);
			gProApList[pApMsg->msgInfo.proApId].xProApPack(pApMsg->proAp.proApData, pApMsg->proAp.proApDataLen, connBuf, (xt_u8*)&connLen);

			if(connLen > 0)
			{
				//PrintfXTOS("devID2=%d,connLen=%d\n",pApMsg->msgInfo.devId,connLen);
				if (pApMsg->msgInfo.devId < DEV_MAX)
				{
					if(gDevList[pApMsg->msgInfo.devId].xDevSend != NULL)
					{
						if (pApMsg->msgInfo.connId < CONN_MAX)
						{
							if(pApMsg->msgInfo.connId == CONN_UART)
							{
								gDevList[pApMsg->msgInfo.devId].xDevSend(connBuf, connLen, &pApMsg->commInfo,
									gConnList[pApMsg->msgInfo.connId].xConnWrite, gConnList[pApMsg->msgInfo.connId].xConnRead);
							}
							else if(pApMsg->msgInfo.connId == CONN_MSG)
							{
								gDevList[pApMsg->msgInfo.devId].xDevSend(connBuf, connLen, &pApMsg->commInfo,
									gConnList[pApMsg->msgInfo.connId].xConnRespWrite, gConnList[pApMsg->msgInfo.connId].xConnRead);
							}
						}
						
					}
				}
			}
		}
	}
}

static ap_cmd_func_map_t mApCommCmdFunc[] = {
	{AP_COMM_CMD_SEND, apCommOnSendCmd},
};

static xt_void apComm_LoginAndHeartBeat()
{
	if(glbRtuPara.devPara.flag_dev_ini==YES)//3g 模块初始化成功
	{
		if(glbRtuPara.runningInfo.flag_onine==NO)
		{
			if(glbRtuPara.runningInfo.loginErrCnt<LOGIN_CNT_MAX)              // 0 1 2
			{
				if(glbRtuPara.runningInfo.flag_onlineCmdStart==YES)
				{
					//glbRtuPara.runningInfo.loginCnt++;                          // 1 2 3
					glbRtuPara.runningInfo.loginErrCnt++;                      // 1 2 3
					apMsgData.msgInfo.apCmdId = AP_RTU_CMD_COMM;
					apMsgData.msgInfo.connId = 0;
					apMsgData.msgInfo.devId = 0;
					apMsgData.msgInfo.proApId = PRO_AP_IOT;
					apMsgData.proAp.proApData[0]=accLogin;
					MemMemcpyXTOS((xt_void *)&apMsgData.commInfo.commDst, (xt_void *)glbRtuPara.flashConfig.value.rtuOrPlc_uid_own, DEV_ADDR_BYTES);
					apMsgData.commInfo.commDst = apMsgData.commInfo.commDst << 16; // uid store low 48 bit
					QueueSendXTOS(ApConfigGetMsgQ(AP_RTU), &apMsgData);
					TaskSleepMs(100);
				}
			}
			else if(glbRtuPara.runningInfo.loginErrCnt==LOGIN_CNT_MAX) //连续3次发登陆包，没有收到响应，需要重新连接
			{
				PrintfXTOS("ERR--3 times login err\n");
				//glbRtuPara.devPara.flag_dev_ini=NO;//test..
				//glbRtuPara.runningInfo.heartBeatCnt=0;        
				//glbRtuPara.runningInfo.loginCnt=0;                       
				glbRtuPara.runningInfo.loginErrCnt=0;    	
				glbRtuPara.runningInfo.flag_onine=NO;
			}
		}
		else //have logined
		{
			if(glbRtuPara.runningInfo.heartBeatErrCnt<HEART_BEAT_CNT_MAX)
			{
				if(glbRtuPara.runningInfo.flag_heartBeatCmdStart==YES)// it's time to heart beat
				{
					glbRtuPara.runningInfo.heartBeatCnt++;                          // 1 2 3
					glbRtuPara.runningInfo.heartBeatErrCnt++;                      // 1 2 3
					apMsgData.msgInfo.apCmdId = AP_RTU_CMD_COMM;
					apMsgData.msgInfo.connId = 0;
					apMsgData.msgInfo.devId = 0;
					apMsgData.msgInfo.proApId = PRO_AP_IOT;
					apMsgData.proAp.proApData[0]=accHeartBeat;
					MemMemcpyXTOS((xt_void *)&apMsgData.commInfo.commDst, (xt_void *)glbRtuPara.flashConfig.value.rtuOrPlc_uid_own, DEV_ADDR_BYTES);
					apMsgData.commInfo.commDst = apMsgData.commInfo.commDst << 16; // uid store low 48 bit

					QueueSendXTOS(ApConfigGetMsgQ(AP_RTU), &apMsgData);
					TaskSleepMs(100);
				}	
			}
			else if(glbRtuPara.runningInfo.heartBeatErrCnt==HEART_BEAT_CNT_MAX) //连续3次发登陆包，没有收到响应，需要重新连接
			{
				PrintfXTOS("ERR--3 times heartbeat err\n");
				//glbRtuPara.devPara.flag_dev_ini=NO;
				//glbRtuPara.runningInfo.loginCnt=0;     
				//glbRtuPara.runningInfo.heartBeatCnt=0;                       
				glbRtuPara.runningInfo.heartBeatErrCnt=0;    	
				glbRtuPara.runningInfo.flag_onine=NO;
			}
		}
	}
}

xt_void apComm_doReport(xt_u8 actionId,xt_u8 doFlag)
{
	PrintfXTOS("apComm_doReport\n");	
	apMsgData.msgInfo.apCmdId = AP_RTU_CMD_COMM;
	apMsgData.msgInfo.connId = 0;
	apMsgData.msgInfo.devId = 0;
	apMsgData.msgInfo.proApId = PRO_AP_IOT;
	apMsgData.proAp.proApData[0]=actionId;
	apMsgData.proAp.proApData[3]=doFlag;
	glbRtuPara.runningInfo.flag_doReport=YES;
	QueueSendXTOS(ApConfigGetMsgQ(AP_RTU), &apMsgData);
	TaskSleepMs(100);
}


static xt_void apDevCommInit(void)
{
	xt_u16 j = 0;
	if(glbRtuPara.devPara.flag_dev_ini==YES)
		return;
	for (j = 0; j < sizeof(gDevList)/sizeof(xt_dev_t); j++)
	{
		gDevList[j].xDevInit();
	}
}
static xt_void apCommTask(xt_void *pvParameters)
{
	//static xt_s32 ccnt = 0;
	xt_s32 ret = 0;
	xt_u16 i = 0, j = 0, k = 0,m=0;
	xt_u16 msgCmd = 0;
	
	while(TRUE)
	{
		
		for (i = 0; i < sizeof(gConnList)/sizeof(xt_conn_t); i++)
		{
			MemMemsetXTOS(connBuf, 0, sizeof(connBuf));
			connLen = 0;

			if(gConnList[i].xConnRead != NULL)
				gConnList[i].xConnRead(connBuf,&connLen);
			
			if (connLen > 0)
			{
				MDBG("conn [%d]: recv raw data = %d \n $$$$$$original data:$$$$$\r\n" ,i, connLen);
				for(m=0;m<connLen;m++)
				{
					MDBG(" %0.2x" , connBuf[m]);
				}
				MDBG(" \n-------------\n " , connBuf[m]);

				for (j = 0; j < sizeof(gDevList)/sizeof(xt_dev_t); j++)
				{
					MemMemsetXTOS(&apMsgData,0,sizeof(apMsgData));
					MemMemsetXTOS(devBuf,0,sizeof(devBuf));
					devLen = 0;

					if(gDevList[j].xDevRecv != NULL)
					{
						while (gDevList[j].xDevRecv(connBuf, connLen, devBuf, &devLen, &apMsgData.commInfo)== SUCCESS_XT)
						{
							connLen=0;
							for (k = 0; k < sizeof(gProApList)/sizeof(xt_pro_ap_t); k++)
							{
								MemMemsetXTOS(&apMsgData.proAp, 0, sizeof(pro_ap_info_t));
								MemMemsetXTOS(&apMsgData.msgInfo, 0, sizeof(msg_info_t));

								if(gProApList[k].xProApParse != NULL)
								{
									gProApList[k].xProApParse(devBuf, devLen, apMsgData.proAp.proApData, &apMsgData.proAp.proApDataLen);

									if (apMsgData.proAp.proApDataLen > 0)
									{
										apMsgData.msgInfo.apCmdId = AP_RTU_CMD_COMM;
										apMsgData.msgInfo.connId = i;
										apMsgData.msgInfo.devId = j;
										apMsgData.msgInfo.proApId = k;

										QueueSendXTOS(ApConfigGetMsgQ(AP_RTU), &apMsgData);
									}

								}

							}

							MemMemsetXTOS(&apMsgData.commInfo, 0, sizeof(xt_dev_info_t));
						}

					}
					
				}

			}
			
		}

		ret = QueueReceiveXTOS(gCommData.instance.apTaskMsgQId, &apMsgData, AP_COMM_MSG_TIMEOUT);
		if(ret == SUCCESS_XT)
		{
			PrintfXTOS("apComm recv msg, apCmd = 0x%x \n", apMsgData.msgInfo.apCmdId);

			for (i = 0; i < sizeof(mApCommCmdFunc)/sizeof(ap_cmd_func_map_t); i++)
			{
				if(mApCommCmdFunc[i].cmd == apMsgData.msgInfo.apCmdId)
				{
					if(NULL != mApCommCmdFunc[i].processFunc) {
						mApCommCmdFunc[i].processFunc(&apMsgData);
					}
					else
						LOG_PRINTF(LOG_LEVEL_WARNING, "apComm: unrealize cId=0x%x \n", msgCmd);
					break;
				}
				else
				{
					if(i + 1 == sizeof(mApCommCmdFunc)/sizeof(ap_cmd_func_map_t))
					{
						LOG_PRINTF(LOG_LEVEL_WARNING, "apComm: unsupport cId=0x%x \n", msgCmd);
					}
				}

			}

		}


		apDevCommInit();
		apComm_LoginAndHeartBeat();
		KEY_getVal();
		KEY_doAction();
		odb_doTask(odbBuf,&odbLen);
		do_timeSensorTask();
	}

}

static xt_void apCommInit(void)
{
	XtBspCommInit(115200,USART_Parity_No);
	connBuf=MemMallocXTOS(BUFFER_LENGTH);
	odbBuf=MemMallocXTOS(BUFFER_LENGTH);
	devBuf=MemMallocXTOS(BUFFER_LENGTH);
	if(connBuf==NULL)
		PrintfXTOS("connBuf malloc err... \n");
	if(odbBuf==NULL)
		PrintfXTOS("odbBuf malloc err... \n");
	if(devBuf==NULL)
		PrintfXTOS("devBuf malloc err... \n");	

	PrintfXTOS("apCommInit... \n");
}

ap_t* ApCommGetInstance(void)
{
	if(gCommData.instance.ApInit== NULL)
	{
		gCommData.instance.ApInit = apCommInit;
		gCommData.instance.ApTask = apCommTask;
	}
	return (ap_t*)&(gCommData.instance);
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
