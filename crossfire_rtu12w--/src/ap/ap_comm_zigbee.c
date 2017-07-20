/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: tony <zhiwen.tang@iotcomm.com>                     */
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
#include "ap_comm_zigbee.h"
#include "ap_rtu.h"
#include "ap_measure.h"
#include "misc_timer.h"

#include "config.h"

extern xt_u8 *connBuf;
extern xt_s32 connLen;
extern xt_u8 *devBuf;
extern xt_s32 devLen;
//extern xt_u8 sendBuf[BUFFER_LENGTH];
//extern xt_s32 sendLen;
extern rtu_para_t glbRtuPara;
extern xt_dev_t gDevList[DEV_MAX];
extern xt_conn_t gConnList[CONN_MAX];
extern ap_msg_t apMsgData ;

xt_s32 apCommZigbee_writeAndRead(xt_u8 *dataBuf,xt_s32 len)
{
	xt_dev_info_t	commInfo;
	xt_u8 i;
	commInfo.commDst=0;
	for(i=0;i<6;i++)
	{
		commInfo.commDst<<=8;
		commInfo.commDst|=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[5-i];
	}
	commInfo.commDst<<=16;
	commInfo.commCmd=CMD_REPORT;
	gDevList[DEV_ZIGBEE].xDevSend(dataBuf, len, &commInfo,
		gConnList[CONN_UART].xConnWrite, gConnList[CONN_UART].xConnRead);
	TaskSleepMs(2000);
	gConnList[CONN_UART].xConnRead(connBuf,&connLen);
	
	if(connLen>0)
	{
		return gDevList[CONN_UART].xDevRecv(connBuf, connLen, devBuf, &devLen,&commInfo);
	}
	return FAILURE_XT;
}

xt_s32	ApCommZigbee_joinNetProcess(void )
{
	xt_s32 pLen = 0;
	xt_s32 i;
	xt_u8 dataBuf[5];
	pro_ap_iot_joinNetRequest_in_t *pData=(pro_ap_iot_joinNetRequest_in_t *)devBuf;
	static xt_u8 retryNum=0;	
	static xt_u8 step=0;
	static xt_u8 randomComFreq=1;
	static xt_u8 oldComFreq;
	
	if(glbRtuPara.runningInfo.flag_joinNet_finished)
	{
		return SUCCESS_XT;
	}

	switch(step)
	{
		case 0:
			if(glbRtuPara.flashConfig.value.flag_have_joined)//����һ���ŵ����г�������
			{
				randomComFreq=glbRtuPara.flashConfig.value.comFreq;
				oldComFreq=glbRtuPara.flashConfig.value.comFreq;
				gDevList[DEV_ZIGBEE].xDevInit();
				retryNum=8;
				step=2;
			}
			else
			{
				step=1;
			}
			break;
		case 1://-����Ŀ�ʼ�ŵ�
		{
			xt_u16 seed;
			glbRtuPara.runningInfo.flag_zigbee_channle=0;
			srand(seed);                    /*ʹ�������һ��*/
			randomComFreq=rand();                          /*���������*/
			//randomComFreq=15;////TEMP
			step=2;
		}
			break;
		case 2://-�л���ָ�����ŵ�
			randomComFreq=randomComFreq%16;
			if(randomComFreq==0)
			{
				randomComFreq+=2;//����ͨ��0��ͨ��1��ͨ��1���ֶ��޸��á�
			}
			glbRtuPara.runningInfo.flag_zigbee_channle|= (1<<randomComFreq);
			glbRtuPara.devPara.flag_dev_ini=NO;
			glbRtuPara.flashConfig.value.comFreq=randomComFreq;
			retryNum=2;	
			step=3;
			break;
		case 3://-����ע��
		
			if(glbRtuPara.devPara.flag_dev_ini==NO)
			{
				gDevList[DEV_ZIGBEE].xDevInit();
				break;
			}
			
			i=0;
			dataBuf[i++] = ccJoinNetRequest;
			dataBuf[i++] = randomComFreq; //Ƶ���ŵ�
			if((apCommZigbee_writeAndRead(dataBuf,i)==SUCCESS_XT)&&(retryNum>0))
			{
				if((pData->cmd==ccJoinNetRequest)&&(pData->is_ok==YES)
					&&(pData->comFreq==randomComFreq))
				{
					step=4;
				}
			}
			else if (retryNum==0)
			{
				if(glbRtuPara.runningInfo.flag_zigbee_channle==0xfffc)//�����ŵ�,��ѭ��һ��δ�����ɹ�
				{
					step=5;
				}
				else
				{
					step=2;
					randomComFreq++;
				}
			}
			else
				retryNum--;
			break;
		case 4://-д��flash
			if(oldComFreq!=randomComFreq)
			{
				//glbRtuPara.flashConfig.value.comFreq=randomComFreq;
				oldComFreq=randomComFreq;
				apWriteFlashCfgDefaultPara(OTHER);
			}
			glbRtuPara.runningInfo.flag_joinNet_finished=YES;//���������
			glbRtuPara.flashConfig.value.flag_have_joined=YES;			
			step=0;
			PrintfXTOS("join net success haha...!!!...\n");
			return SUCCESS_XT;
		case 5://-��ʱ����
			PrintfXTOS("sent to hand ctrllor\n");
			//randomComFreq=1;//�̶��ֶ��޸�Ƶ���ŵ�Ϊͨ��1
			//gDevList[DEV_ZIGBEE].xDevInit();
			//-��ʱ������������ʾ�һ�Ϊ�������磬���ø����Ҽ���˭?
			
			//-��ʱδ�յ��ֳ��ն˵Ļظ����˳���
			glbRtuPara.runningInfo.flag_zigbee_channle=0;
			step=1;
			break;
	}
	return SUCCESS_XT;

}

