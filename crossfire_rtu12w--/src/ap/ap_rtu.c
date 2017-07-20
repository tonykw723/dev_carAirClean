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
#include "stm32l1xx.h"
#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"
#include "xtos_sem.h"
#include "xtos_timer.h"

#include "xt_bsp_rtu09c.h"
#include "discover_board.h"
#include "drv_uart.h"
#include "drv_light.h"

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
#include "displayLed.h"


#include "config.h"
#include "utils.h"


#include "version.h"

extern rtu_para_t glbRtuPara;
extern comfirm_enum_t is_sloar_module_ini;
//static ap_rtu_task_set_t *pRTUTask[RTU_ACTION_MAX_NUM];
static ap_rtu_task_in_eerom_t *pTaskInEerom;
ap_rtu_priv_data_t gRTUData = {0};

static ap_msg_t apMsgRecvData = {0};
static ap_msg_t apMsgRespData = {0};
extern sensor_type sensor;
extern xt_dev_t gDevList[DEV_MAX];


extern uint32_t STM_EVAL_FlashEepromWriteByte(uint16_t Addr,uint8_t *Buffer,uint16_t Length);
extern uint32_t STM_EVAL_FlashEepromReadByte(uint16_t Addr,uint8_t *Buffer,uint16_t Length);
#define apInnerFlashEerpomWriteByte STM_EVAL_FlashEepromWriteByte
#define apInnerFlashEerpomReadByte STM_EVAL_FlashEepromReadByte
xt_void apDimmingValueRenew(xt_u8 dim_value,xt_u8 which_lamp,xt_u8 flag_renew_now);

/*
aim: for read/write eeprom,get target byte index and eeprom addr offset.
para:@*beginByte_pt  :get begin byte addr
	 @*targetByte_pt :get taget byte addr
return:taget index or eeprom addr offset	 
*/
uint16_t eeGet_targetByteBufIndexOrAddrOffset(uint8_t *beginByte_pt,uint8_t *targetByte_pt)
{
	return (uint16_t)(targetByte_pt-beginByte_pt);
}
/*
aim: for read/write eeprom,get how many byte to be writed.
para:@*beginByte_pt  :get begin byte addr
	 @*targetByte_pt :get taget byte addr
	 @self_len:get own size;
return:length of bytes from addr1 to addr2  
*/
uint16_t eeGet_bytesOfLengthToBeWrited(uint8_t *FromByte_pt,uint8_t *ToByte_pt,uint8_t self_len)
{
	return (uint16_t)(ToByte_pt-FromByte_pt+self_len);
}
xt_u8 apWriteFlashCfgPara(xt_u8 value)
{
	xt_u8 i;
	char s_pNum[10]={0};
	xt_u8 *write_temp,len=sizeof(glbRtuPara.flashConfig.value);
	write_temp=MemMallocXTOS(len);
	
	//---------非命令可设置参数---------
	//---------
	if(value==SET_DEFAULT)
	{
	glbRtuPara.flashConfig.value.linkType=1;//1-gprs ,2-ethernet  
	MemMemcpyXTOS(glbRtuPara.flashConfig.value.project_name, "nomal proj", sizeof("nomal proj"));
	//MemMemcpyXTOS(glbRtuPara.flashConfig.value.severIp, "1519yq1416.iask.in,", sizeof("1519yq1416.iask.in,"));
	//air.semsplus.com,
	MemMemcpyXTOS(glbRtuPara.flashConfig.value.severIp, "air.semsplus.com,", sizeof("air.semsplus.com,"));
	//glbRtuPara.flashConfig.value.portNum=16895;
	glbRtuPara.flashConfig.value.portNum=5887;
	//itoa(glbRtuPara.flashConfig.value.portNum, s_pNum);////
	PrintfXTOS("port:%s\n",s_pNum);////
	MemMemcpyXTOS(glbRtuPara.flashConfig.value.apn, "cmcc", sizeof("cmcc"));
	MemMemcpyXTOS(glbRtuPara.flashConfig.value.telephone, "13512345678", sizeof("13512345678"));
	glbRtuPara.flashConfig.value.pwm_type=PWM_A;
	glbRtuPara.flashConfig.value.pwm_frequence=4000;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[0]=0;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[1]=0;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[2]=0;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[3]=0;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[4]=0;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[5]=0;
	}
	//-------命令可设置参数----------------
	else if(value==RESET_PARA)
	{
		
	}

	else
	{
		
	}
	PrintfXTOS("  write rtu cfg:\n");
	glbRtuPara.flashConfig.value.have_writed=0x1234;
	//PrintfXTOS("  **write datas are:\n");
	//for(i=0;i<len;i++)
	//	PrintfXTOS("%0.2x ",glbRtuPara.flashConfig.buf[i]);
	//PrintfXTOS("\n");
	apInnerFlashEerpomWriteByte(IFLASH_RTU_CFG_START_ADDR_OFFSET,glbRtuPara.flashConfig.buf,len);
	apInnerFlashEerpomReadByte(IFLASH_RTU_CFG_START_ADDR_OFFSET,write_temp,len);
	//PrintfXTOS("  **read datas are:\n");
	//for(i=0;i<len;i++)
	//	PrintfXTOS("%0.2x ",write_temp[i]);
	//PrintfXTOS("\n");
	if(memcmp(&glbRtuPara.flashConfig.buf,write_temp,len)==0)
	{
		PrintfXTOS("  wr flash success\n");
		MemFreeXTOS(write_temp);
		return SUCCESS_XT;
	}
	else
	{
		PrintfXTOS("  wr flash failure\n");
		MemFreeXTOS(write_temp);
		return FAILURE_XT;
	}

}
xt_void apReadFlashCfgPara(void)
{	
	xt_u16 len=0;
	xt_u8 i;
	PrintfXTOS("apReadFlashCfgPara\n");
	//-------------------------------------------------------------------
	len=sizeof(glbRtuPara.flashConfig.value);
	PrintfXTOS("  size:flashConfig=%d\n",len);
	PrintfXTOS("  read rtu cfg:\n");
	apInnerFlashEerpomReadByte(IFLASH_RTU_CFG_START_ADDR_OFFSET,glbRtuPara.flashConfig.buf,len);
	//glbRtuPara.flashConfig.value.have_writed=0x1111;//test
	//PrintfXTOS("  read datas are:\n");
	//for(i=0;i<len;i++)
	//	PrintfXTOS("%0.2x ",glbRtuPara.flashConfig.buf[i]);
	//PrintfXTOS("\n");
	//test...
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[0]=0;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[1]=0;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[2]=0;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[3]=0;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[4]=0;
	glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[5]=5;
	if(glbRtuPara.flashConfig.value.have_writed==0x1234)
	{
		PrintfXTOS("  -project_name:");
		for(i=0;i<6;i++)
			PrintfXTOS("%c",glbRtuPara.flashConfig.value.project_name[i]);
		PrintfXTOS("\n  -rtu_sn:");
		for(i=0;i<4;i++)
		{
			if(glbRtuPara.flashConfig.value.rtu_sn[i]==0)
				break;
			PrintfXTOS("%c",glbRtuPara.flashConfig.value.rtu_sn[i]);
		}
		PrintfXTOS("\n  -severIp:");
		for(i=0;i<32;i++)
		{
			if(glbRtuPara.flashConfig.value.severIp[i]==0)
				break;
			PrintfXTOS("%c",glbRtuPara.flashConfig.value.severIp[i]);
		}
		PrintfXTOS("\n  -portNum:%d",glbRtuPara.flashConfig.value.portNum);
		PrintfXTOS("\n  -apn:");
		for(i=0;i<16;i++)
		{
			if(glbRtuPara.flashConfig.value.apn[i]==0)
				break;
			PrintfXTOS("%c",glbRtuPara.flashConfig.value.apn[i]);
		}
		PrintfXTOS("\n  -telephone:");
		for(i=0;i<20;i++)
		{
			if(glbRtuPara.flashConfig.value.telephone[i]==0)
				break;
			PrintfXTOS("%c",glbRtuPara.flashConfig.value.telephone[i]);
		}

		switch(glbRtuPara.flashConfig.value.pwm_type)
		{
			case PWM_A:
				PrintfXTOS("  -PWM_A\n");
				break;
			case PWM_N:
				PrintfXTOS("  -PWM_N\n");
				break;
			default:
				break;
		}		
		PrintfXTOS("  -pwm_frequence=%d\n",glbRtuPara.flashConfig.value.pwm_frequence);
		PrintfXTOS("  -group : ");
		for(i=0;i<sizeof(glbRtuPara.flashConfig.value.rtuOrPlc_group);i++)
		{
			PrintfXTOS("%0.2x ",glbRtuPara.flashConfig.value.rtuOrPlc_group[i]);
		}
		PrintfXTOS("\n");
		
		PrintfXTOS("  -own uid : ");
		for(i=0;i<sizeof(glbRtuPara.flashConfig.value.rtuOrPlc_uid_own);i++)
		{
			PrintfXTOS("%0.2x ",glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[i]);
		}
		PrintfXTOS("\n");		
		PrintfXTOS("  -sever uid : ");
		for(i=0;i<sizeof(glbRtuPara.flashConfig.value.sever_uid);i++)
		{
			PrintfXTOS("%0.2x ",glbRtuPara.flashConfig.value.sever_uid[i]);
		}
		PrintfXTOS("\n");	
		glbRtuPara.flashConfig.value.runMode=HAND_MODE;
		if(glbRtuPara.flashConfig.value.runMode==HAND_MODE)
			PrintfXTOS("HAND_MODE\n");
		else
			PrintfXTOS("AUTO_MODE\n");

	}
	else
	{
		apWriteFlashCfgPara(SET_DEFAULT);
	}			

}

xt_u8 apWriteFlashTaskpara()
{
	xt_u8 *write_temp,len=sizeof(glbRtuPara.flashTask.value);
	write_temp=MemMallocXTOS(len);
	apInnerFlashEerpomWriteByte(IFLASH_RTU_TASK_START_ADDR_OFFSET,glbRtuPara.flashTask.buf,len);
	apInnerFlashEerpomReadByte(IFLASH_RTU_CFG_START_ADDR_OFFSET,write_temp,len);
	if(memcmp(&glbRtuPara.flashConfig.buf,write_temp,len)==0)
	{
		PrintfXTOS("  wr flash success\n");
		MemFreeXTOS(write_temp);
		return SUCCESS_XT;
	}
	else
	{
		PrintfXTOS("  wr flash failure\n");
		MemFreeXTOS(write_temp);
		return FAILURE_XT;
	}	
	
}

static xt_void apReadFlashTask()
 {
 	xt_u16 len;
 	len=sizeof(glbRtuPara.flashTask.value);;
 	apInnerFlashEerpomReadByte(IFLASH_RTU_TASK_START_ADDR_OFFSET,glbRtuPara.flashTask.buf,len);
	if(glbRtuPara.flashTask.value.have_writed!= 0x1234){
		MemMemsetXTOS(glbRtuPara.flashTask.buf, 0, len);
		glbRtuPara.flashTask.value.have_writed= 0x1234;
		apWriteFlashTaskpara();
	}
	else
	{
		PrintfXTOS("  taskId=%d\n",glbRtuPara.flashTask.value.taskId);
		PrintfXTOS("  whichDay1=%0.2x,if =0,sunday,if=1,monday...\n",glbRtuPara.flashTask.value.whichDay1);
		PrintfXTOS("  task hour1=%d\n minutue1=%d howmanyMinutes1=%d(min)\n",glbRtuPara.flashTask.value.hour1,
		glbRtuPara.flashTask.value.minute1,glbRtuPara.flashTask.value.howmanyMinutes1);
		PrintfXTOS("  taskOn=%d,if 0-off, 1-on\n",glbRtuPara.flashTask.value.action1);

		PrintfXTOS("  whichDay2=%0.2x,if =0,sunday,if=1,monday...\n",glbRtuPara.flashTask.value.whichDay2);
		PrintfXTOS("  task hour2=%d\n minutue2=%d howmanyMinutes2=%d(min)\n",glbRtuPara.flashTask.value.hour2,
		glbRtuPara.flashTask.value.minute2,glbRtuPara.flashTask.value.howmanyMinutes2);
		PrintfXTOS("  taskOn=%d,if 0-off, 1-on\n",glbRtuPara.flashTask.value.action2);

		PrintfXTOS("  whichDay3=%0.2x,if =0,sunday,if=1,monday...\n",glbRtuPara.flashTask.value.whichDay3);
		PrintfXTOS("  task hour3=%d\n minutue3=%d howmanyMinutes3=%d(min)\n",glbRtuPara.flashTask.value.hour3,
		glbRtuPara.flashTask.value.minute3,glbRtuPara.flashTask.value.howmanyMinutes3);
		PrintfXTOS("  taskOn=%d,if 0-off, 1-on\n",glbRtuPara.flashTask.value.action3);

		PrintfXTOS("  whichDay4=%0.2x,if =0,sunday,if=1,monday...\n",glbRtuPara.flashTask.value.whichDay4);
		PrintfXTOS("  task hour4=%d\n minutue4=%d howmanyMinutes4=%d(min)\n",glbRtuPara.flashTask.value.hour4,
		glbRtuPara.flashTask.value.minute4,glbRtuPara.flashTask.value.howmanyMinutes4);
		PrintfXTOS("  taskOn=%d,if 0-off, 1-on\n",glbRtuPara.flashTask.value.action4);
	}
 }


xt_void apReadFlashPara(void)
{
	apReadFlashCfgPara();
	apReadFlashTask();
}

xt_void apParaInit(void)
{
	apReadFlashPara();
	glbRtuPara.runningInfo.flag_powerEn=NO;
	glbRtuPara.runningInfo.flag_taskStart=NO;
	glbRtuPara.runningInfo.flag_DLZ_Opened=NO;


	glbRtuPara.runningInfo.flag_DLED_en=NO;
	glbRtuPara.runningInfo.flag_FLED_en=NO;
	
}


static xt_void apRTUReqRespComm(xt_void* p, xt_u8* pRetData, xt_u8 retLen)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;

	MemMemsetXTOS(&apMsgRespData,0,sizeof(apMsgRespData));
	
	MemMemcpyXTOS(&apMsgRespData.msgInfo, &pApMsg->msgInfo, sizeof(msg_info_t));
	apMsgRespData.msgInfo.apCmdId = AP_COMM_CMD_SEND;

	MemMemcpyXTOS(&apMsgRespData.commInfo, &pApMsg->commInfo, sizeof(xt_dev_info_t));

	MemMemcpyXTOS(apMsgRespData.proAp.proApData, pRetData, retLen);
	apMsgRespData.proAp.proApDataLen = retLen;

	PrintfXTOS("apRTUReqRespComm apCmdId = 0x%x \n",pApMsg->msgInfo.apCmdId);

	if (AP_RTU_CMD_COMM == pApMsg->msgInfo.apCmdId)
		QueueSendXTOS(ApConfigGetMsgQ(AP_COMM), &apMsgRespData);

}

static xt_void apRTUOnLoginCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	
	if(glbRtuPara.runningInfo.flag_onlineCmdStart==YES)//上行包
	{
		pro_ap_login_up_t proRetSt = {0};
		pApMsg->msgInfo.proApId=PRO_AP_IOT;//模拟接收到login命令
		glbRtuPara.runningInfo.flag_onlineCmdStart=NO;	
		PrintfXTOS("apRTUOnLoginCmd up\n");
		proRetSt.mCmdId=accLogin;
		proRetSt.len=0x01;
		proRetSt.signalVal=0xff;
		apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
	}
	else//下行包
	{
		glbRtuPara.runningInfo.loginErrCnt=0;
		glbRtuPara.runningInfo.flag_onine=YES;
		PrintfXTOS("apRTUOnLoginCmd down\n");
		
	}
}


static xt_void apRTUOnHeartBeatCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	if(glbRtuPara.runningInfo.flag_heartBeatCmdStart==YES)//上行包
	{
		pro_ap_heartBeat_up_t proRetSt = {0};
		xt_tm_t tm={0};
		pApMsg->msgInfo.proApId=PRO_AP_IOT;//模拟接收到login命令
		glbRtuPara.runningInfo.flag_heartBeatCmdStart=NO;	
		PrintfXTOS("apRTUOnHeartBeatCmd up\n");
		proRetSt.mCmdId=accHeartBeat;
		proRetSt.len=0x01;
		proRetSt.signalVal=0xff;
		proRetSt.TP_val=(xt_u16)(sensor.TP_val);
		proRetSt.HM_val=(xt_u16)(sensor.HM_val);
		proRetSt.PM2_5_val=(xt_u16)(sensor.PM2_5_val);
		proRetSt.VOC_val=(xt_u16)(sensor.VOC_val);
		proRetSt.taskStatus=glbRtuPara.flashConfig.value.runMode;
		proRetSt.batteryStatus=NO;
		proRetSt.fanStatus=glbRtuPara.runningInfo.flag_fanOpened;
		proRetSt.DLZ_Status=glbRtuPara.runningInfo.flag_DLZ_Opened;	
		proRetSt.backLedStatus=glbRtuPara.runningInfo.flag_DLED;
		proRetSt.powerLedStatus=glbRtuPara.runningInfo.flag_FLED;
		if(XtBspGetTmTime(XtBspGetTime(),&tm) == SUCCESS_XT)
		{
			proRetSt.year = tm.tm_year-100;
			proRetSt.month = tm.tm_mon+1;
			proRetSt.day = tm.tm_mday;
			proRetSt.week = tm.tm_wday;
			proRetSt.hour = tm.tm_hour;
			proRetSt.minute= tm.tm_min;
			proRetSt.second= tm.tm_sec;
		}
		apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
	}
	else//下行包
	{
		glbRtuPara.runningInfo.heartBeatErrCnt=0;
		PrintfXTOS("apRTUOnHeartBeatCmd down\n");
	}	
}

static xt_void apRTUOnDoAllCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_doAll_down_t* pProCmd = (pro_ap_doAll_down_t*)pApMsg->proAp.proApData;
	pro_ap_doAll_up_t proRetSt = {0};
	PrintfXTOS("apRTUOnDoAllCmd\n");	
	if(glbRtuPara.runningInfo.flag_doReport==YES)
	{
		PrintfXTOS("report\n");	
		glbRtuPara.runningInfo.flag_doReport=NO;
		proRetSt.respId=accDoAll;
		proRetSt.mCmdId=accDoAll;
		proRetSt.doFlag=pProCmd->doFlag;
		proRetSt.isSuccess=YES;
	}
	else
	{
		proRetSt.respId=pProCmd->mCmdId;
		proRetSt.mCmdId=pProCmd->mCmdId;
		proRetSt.isSuccess=YES;
		proRetSt.doFlag=pProCmd->doFlag;
		if(pProCmd->doFlag==0)//close all
		{
			drvTurnOff();
			glbRtuPara.runningInfo.flag_taskStart=NO;
			glbRtuPara.runningInfo.flag_taskStart_backup=glbRtuPara.runningInfo.flag_taskStart;
			glbRtuPara.flashConfig.value.runMode=HAND_MODE;
			glbRtuPara.runningInfo.flag_fanAdjLevel=0;
		}
		else
		{
			drvTurnOn();
			glbRtuPara.runningInfo.flag_taskStart=DONE;
			glbRtuPara.runningInfo.flag_taskStart=glbRtuPara.runningInfo.flag_taskStart_backup;
			glbRtuPara.flashConfig.value.runMode=AUTO_MODE;
		}
	}
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnOpenFanCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_openFan_down_t* pProCmd = (pro_ap_openFan_down_t*)pApMsg->proAp.proApData;
	pro_ap_openFan_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnOpenFanCmd\n");	
	if(glbRtuPara.runningInfo.flag_doReport==YES)
	{
		PrintfXTOS("report\n");	
		glbRtuPara.runningInfo.flag_doReport=NO;
		proRetSt.respId=accOpenFan;
		proRetSt.mCmdId=accOpenFan;
 		proRetSt.isSuccess=YES;
	}
	else
	{
		proRetSt.respId=pProCmd->mCmdId;
		proRetSt.mCmdId=pProCmd->mCmdId;
		if(glbRtuPara.runningInfo.flag_powerEn==YES)
		{
			proRetSt.isSuccess=YES;
			if(pProCmd->PWM_val<=5)
				glbRtuPara.runningInfo.flag_fanOpened=NO;
			else
				glbRtuPara.runningInfo.flag_fanOpened=YES;
			glbRtuPara.runningInfo.flag_fanAdjLevel=pProCmd->PWM_val/25;
			drvFanDim(pProCmd->PWM_val);
		}
		else
			proRetSt.isSuccess=NO;
		
	}
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnCloseFanCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_closeFan_down_t* pProCmd = (pro_ap_closeFan_down_t*)pApMsg->proAp.proApData;
	pro_ap_closeFan_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnCloseFanCmd\n");	
	proRetSt.respId=pProCmd->mCmdId;
	proRetSt.mCmdId=pProCmd->mCmdId;
	proRetSt.isSuccess=YES;
	glbRtuPara.runningInfo.flag_fanOpened=NO;
	glbRtuPara.runningInfo.flag_fanAdjLevel=0;
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnOpenDLZCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_openDLZ_down_t* pProCmd = (pro_ap_openDLZ_down_t*)pApMsg->proAp.proApData;
	pro_ap_openDLZ_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnOpenDLZCmd\n");	
	if(glbRtuPara.runningInfo.flag_doReport==YES)
	{
		PrintfXTOS("report\n");	
		glbRtuPara.runningInfo.flag_doReport=NO;
		proRetSt.respId=accOpenDLZ;
		proRetSt.mCmdId=accOpenDLZ;
 		proRetSt.isSuccess=YES;
	}
	else
	{
		proRetSt.respId=pProCmd->mCmdId;
		proRetSt.mCmdId=pProCmd->mCmdId;
		if(glbRtuPara.runningInfo.flag_powerEn==YES)
		{
			proRetSt.isSuccess=YES;
			drvDLZOpen();
		}
		else
			proRetSt.isSuccess=NO;
	}
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnCloseDLZCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_closeDLZ_down_t* pProCmd = (pro_ap_closeDLZ_down_t*)pApMsg->proAp.proApData;
	pro_ap_closeDLZ_up_t proRetSt = {0};
	PrintfXTOS("apRTUOnOpenDLZCmd\n");	
	if(glbRtuPara.runningInfo.flag_doReport==YES)
	{
		PrintfXTOS("report\n");	
		glbRtuPara.runningInfo.flag_doReport=NO;
		proRetSt.respId=accCloseDLZ;
		proRetSt.mCmdId=accCloseDLZ;
 		proRetSt.isSuccess=YES;
	}
	else
	{	
 		proRetSt.respId=pProCmd->mCmdId;
		proRetSt.mCmdId=pProCmd->mCmdId;
		if(glbRtuPara.runningInfo.flag_powerEn==YES)
		{
			proRetSt.isSuccess=YES;
			drvDLZClose();
		}
		else
			proRetSt.isSuccess=NO;
	}
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnOpenLEDCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_openLED_down_t* pProCmd = (pro_ap_openLED_down_t*)pApMsg->proAp.proApData;
	pro_ap_openLED_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnOpenLEDCmd\n");	
	proRetSt.respId=pProCmd->mCmdId;
	proRetSt.mCmdId=pProCmd->mCmdId;
	proRetSt.flagLED=pProCmd->flagLED;
	if(glbRtuPara.runningInfo.flag_powerEn==YES)
	{
		proRetSt.isSuccess=YES;
		if(pProCmd->flagLED==0)//氛围灯
		{
			
			glbRtuPara.runningInfo.flag_DLED_en=YES;
		}
		else//指示灯
		{
			glbRtuPara.runningInfo.flag_FLED_en=YES;
			//FLED_green();
		}
	}
	else
		proRetSt.isSuccess=NO;
	//------------to do open led----------
	//DLED_WriteLedEn(1);
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnCloseLEDCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_closeLED_down_t* pProCmd = (pro_ap_closeLED_down_t*)pApMsg->proAp.proApData;
	pro_ap_closeLED_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnCloseLEDCmd\n");
	proRetSt.respId=pProCmd->mCmdId;
	proRetSt.mCmdId=pProCmd->mCmdId;
	
	proRetSt.flagLED=pProCmd->flagLED;
	if(glbRtuPara.runningInfo.flag_powerEn==YES)
	{
		proRetSt.isSuccess=YES;
		if(pProCmd->flagLED==0)
		{
			glbRtuPara.runningInfo.flag_DLED_en=NO;
			DLED_close();
		}
		else
		{
			glbRtuPara.runningInfo.flag_FLED_en=NO;
			FLED_close();
		}
	}
	else
	{
		proRetSt.isSuccess=NO;
	}
	//------------to do ServerParaGet----------
	//DLED_WriteLedEn(0);
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnQuerySensorCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_querySensor_down_t* pProCmd = (pro_ap_querySensor_down_t*)pApMsg->proAp.proApData;
	pro_ap_querySensor_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnQuerySensorCmd\n");	
	PrintfXTOS("\r\n-->TP_val=%d,HM_val=%d,PM2_5_val=%0.4f,VOC_val=%x\r\n",sensor.TP_val,sensor.HM_val,sensor.PM2_5_val,sensor.VOC_val);
	proRetSt.mCmdId=pProCmd->mCmdId;
	proRetSt.sCmd=pProCmd->sCmd;
	proRetSt.TP_val=(xt_u16)(sensor.TP_val);
	proRetSt.HM_val=(xt_u16)(sensor.HM_val);
	proRetSt.PM2_5_val=(xt_u16)(sensor.PM2_5_val);
	proRetSt.VOC_val=(xt_u16)(sensor.VOC_val);
	proRetSt.taskStatus=glbRtuPara.flashConfig.value.runMode;
	proRetSt.batteryStatus=NO;
	proRetSt.fanStatus=glbRtuPara.runningInfo.flag_fanOpened;
	proRetSt.DLZ_Status=glbRtuPara.runningInfo.flag_DLZ_Opened;	
	proRetSt.backLedStatus=glbRtuPara.runningInfo.flag_DLED;
	proRetSt.powerLedStatus=glbRtuPara.runningInfo.flag_FLED;
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnQueryStatusCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_queryStatus_down_t* pProCmd = (pro_ap_queryStatus_down_t*)pApMsg->proAp.proApData;
	pro_ap_queryStatus_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnQueryStatusCmd\n");	
	proRetSt.mCmdId=pProCmd->mCmdId;
	proRetSt.sCmd=pProCmd->sCmd;
	proRetSt.taskStatus=glbRtuPara.flashConfig.value.runMode;
	proRetSt.batteryStatus=NO;
	proRetSt.fanStatus=glbRtuPara.runningInfo.flag_fanOpened;
	proRetSt.DLZ_Status=glbRtuPara.runningInfo.flag_DLZ_Opened;
	
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnResetCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_reset_down_t* pProCmd = (pro_ap_reset_down_t*)pApMsg->proAp.proApData;
	pro_ap_reset_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnResetCmd\n");	
	proRetSt.respId=pProCmd->mCmdId;
	proRetSt.isSuccess=YES;
	//------------to do reset-----------
	//1.reset sever ip para
	glbRtuPara.flashConfig.value.linkType=0;
	MemMemsetXTOS(glbRtuPara.flashConfig.value.severIp,0, sizeof(glbRtuPara.flashConfig.value.severIp));
	glbRtuPara.flashConfig.value.portNum=0;
	MemMemsetXTOS(glbRtuPara.flashConfig.value.apn, 0, sizeof(glbRtuPara.flashConfig.value.apn));
	MemMemsetXTOS( glbRtuPara.flashConfig.value.telephone,0, sizeof(glbRtuPara.flashConfig.value.telephone));
	//2.wr flash data
	proRetSt.isSuccess=apWriteFlashCfgPara(WRITE_PARA);
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}


static xt_void apRTUOnRestartCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_restart_down_t* pProCmd = (pro_ap_restart_down_t*)pApMsg->proAp.proApData;
	pro_ap_restart_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnRestartCmd\n");	
	proRetSt.respId=pProCmd->mCmdId;
	proRetSt.isSuccess=YES;
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
	TaskSleepMs(2000);
	PrintfXTOS("############restart mcu now###########\n");
	//------------to do restart----------
	//NVIC_SystemReset();
}

static xt_void apRTUOnTimmingCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_timming_down_t* pProCmd = (pro_ap_timming_down_t*)pApMsg->proAp.proApData;
	pro_ap_timming_up_t proRetSt = {0};

	xt_tm_t tm={0};

	// 校时
	tm.tm_year = pProCmd->year;
	tm.tm_wday = pProCmd->week;
	tm.tm_mon = pProCmd->month;
	tm.tm_mday = pProCmd->day;
	tm.tm_hour = pProCmd->hour;
	tm.tm_min = pProCmd->minute;
	tm.tm_sec = pProCmd->second;
	if(XtBspSettime(tm) == SUCCESS_XT){
		proRetSt.isSuccess = YES;
		glbRtuPara.lastTime = XtBspGetTime();
	}else
		proRetSt.isSuccess = ERR;
	
	PrintfXTOS("apRTUOnTimmingCmd\n");	
	proRetSt.respId=pProCmd->mCmdId;
	//------------to do timming----------
	proRetSt.mCmdId=pProCmd->mCmdId;
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnTimeGetCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_timeGet_down_t* pProCmd = (pro_ap_timeGet_down_t*)pApMsg->proAp.proApData;
	pro_ap_timeGet_up_t proRetSt = {0};
	xt_tm_t tm={0};
	
	PrintfXTOS("apRTUOnTimeGetCmd\n");	
	proRetSt.mCmdId=pProCmd->mCmdId;
	//------------to do timeGet----------
	// 取时
	if(XtBspGetTmTime(XtBspGetTime(),&tm) == SUCCESS_XT)
	{
		proRetSt.year = tm.tm_year-100;
		proRetSt.month = tm.tm_mon+1;
		proRetSt.day = tm.tm_mday;
		proRetSt.week = tm.tm_wday;
		proRetSt.hour = tm.tm_hour;
		proRetSt.minute= tm.tm_min;
		proRetSt.second= tm.tm_sec;
		
	}
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnServerParaSetCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_serverParaSet_down_t* pProCmd = (pro_ap_serverParaSet_down_t*)pApMsg->proAp.proApData;
	pro_ap_serverParaSet_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnServerParaSetCmd\n");	
	proRetSt.mCmdId=pProCmd->mCmdId;
	//------------to do ServerParaSet----------
	glbRtuPara.flashConfig.value.linkType=pProCmd->linkType;
	MemMemcpyXTOS(glbRtuPara.flashConfig.value.severIp, pProCmd->severIp, sizeof(pProCmd->severIp));
	glbRtuPara.flashConfig.value.portNum=pProCmd->portNum;
	MemMemcpyXTOS(glbRtuPara.flashConfig.value.apn, pProCmd->apn, sizeof(pProCmd->apn));
	MemMemcpyXTOS(glbRtuPara.flashConfig.value.telephone, pProCmd->telephone, sizeof(pProCmd->telephone));
	proRetSt.isSuccess=apWriteFlashCfgPara(WRITE_PARA);
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}


static xt_void apRTUOnServerParaGetCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_serverParaGet_down_t* pProCmd = (pro_ap_serverParaGet_down_t*)pApMsg->proAp.proApData;
	pro_ap_serverParaGet_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnServerParaGetCmd\n");	
	MemMemsetXTOS((xt_u8*)&proRetSt.mCmdId, 0, sizeof(pro_ap_serverParaGet_up_t));
	proRetSt.mCmdId=pProCmd->mCmdId;
	//------------to do ServerParaGet----------	
	proRetSt.linkType=glbRtuPara.flashConfig.value.linkType;
	MemMemcpyXTOS(proRetSt.severIp,glbRtuPara.flashConfig.value.severIp, sizeof(proRetSt.severIp));
	proRetSt.portNum=glbRtuPara.flashConfig.value.portNum;
	MemMemcpyXTOS(proRetSt.apn,glbRtuPara.flashConfig.value.apn,  sizeof(proRetSt.apn));
	MemMemcpyXTOS( proRetSt.telephone,glbRtuPara.flashConfig.value.telephone, sizeof(proRetSt.telephone));
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnVersionCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_version_down_t* pProCmd = (pro_ap_version_down_t*)pApMsg->proAp.proApData;
	pro_ap_version_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnVersionCmd\n");	
	proRetSt.mCmdId=pProCmd->mCmdId;
	//------------to do ServerParaGet----------
	proRetSt.version=atoi (RTU_VERSION);
	
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnRunModeSetCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_runModeSet_down_t* pProCmd = (pro_ap_runModeSet_down_t*)pApMsg->proAp.proApData;
	pro_ap_runModeSet_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnRunModeSetCmd\n");	
	proRetSt.respId=pProCmd->mCmdId;
	proRetSt.mCmdId=pProCmd->mCmdId;
	//------------to do RunModeSet----------
	glbRtuPara.flashConfig.value.runMode=pProCmd->runMode;
	proRetSt.isSuccess=YES;
	//proRetSt.isSuccess=apWriteFlashCfgPara(WRITE_PARA);
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnTimeTaskSetCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_timeTaskSet_down_t* pProCmd = (pro_ap_timeTaskSet_down_t*)pApMsg->proAp.proApData;
	pro_ap_timeTaskSet_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnTimeTaskSetCmd\n");	
	proRetSt.respId=pProCmd->mCmdId;
	proRetSt.mCmdId=pProCmd->mCmdId;
	//------------to do RunModeSet----------
	MemMemcpyXTOS(&glbRtuPara.flashTask.value.taskId, &pProCmd->taskId, sizeof(pro_ap_timeTaskSet_down_t)-2);
	proRetSt.isSuccess=apWriteFlashTaskpara();
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}

static xt_void apRTUOnTimeTaskGetCmd(xt_void* p)
{
	ap_msg_t* pApMsg = (ap_msg_t*)p;
	pro_ap_timeTaskGet_down_t* pProCmd = (pro_ap_timeTaskGet_down_t*)pApMsg->proAp.proApData;
	pro_ap_timeTaskGet_up_t proRetSt = {0};
	
	PrintfXTOS("apRTUOnTimeTaskGetCmd\n");	
	proRetSt.respId=pProCmd->mCmdId;
	proRetSt.mCmdId=pProCmd->mCmdId;
	//------------to do RunModeSet----------
	MemMemcpyXTOS(&proRetSt.taskId, &glbRtuPara.flashTask.value.taskId, sizeof(pro_ap_timeTaskGet_up_t)-4);
	proRetSt.isSuccess=YES;
	//--------------------------------
	apRTUReqRespComm(p, (xt_u8*)&proRetSt, sizeof(proRetSt));
}






static ap_cmd_func_map_t mApRTUCmdFunc[] = {
	{accLogin,	apRTUOnLoginCmd},
	{accHeartBeat,apRTUOnHeartBeatCmd},
	{accRespond,NULL},
	{accOpenFan,	apRTUOnOpenFanCmd},
	{accCloseFan,	apRTUOnCloseFanCmd},
	{accOpenDLZ,	apRTUOnOpenDLZCmd},
	{accCloseDLZ,apRTUOnCloseDLZCmd},
	{accQuerySensor,apRTUOnQuerySensorCmd},
	{accQueryStatus,apRTUOnQueryStatusCmd},
	{accReset,apRTUOnResetCmd},
	{accRestart,apRTUOnRestartCmd},
	{accTimming,apRTUOnTimmingCmd},
	{accTimeGet,apRTUOnTimeGetCmd},
	{accServerParaSet,apRTUOnServerParaSetCmd},
	{accServerParaGet,apRTUOnServerParaGetCmd},
	{accVersion,apRTUOnVersionCmd},
	{accRunModeSet,apRTUOnRunModeSetCmd},
	{accOpenLED,apRTUOnOpenLEDCmd},
	{accCloseLED,apRTUOnCloseLEDCmd},
	{accDoAll,apRTUOnDoAllCmd},
	{accTimeTaskSet,apRTUOnTimeTaskSetCmd},
	{accTimeTaskGet,apRTUOnTimeTaskGetCmd},
};

static xt_void apRTUTask(xt_void *pvParameters)
{
	xt_s32 ret = 0;
	xt_s32 i = 0;
	xt_u16 msgCmd = 0;
	xt_time_t t,lastT=0;

	while(TRUE)
	{
#if 1
		
		ret = QueueReceiveXTOS( gRTUData.instance.apTaskMsgQId, &apMsgRecvData, AP_RTU_MSG_TIMEOUT );	
		if(ret == SUCCESS_XT)
		{
			msgCmd = apMsgRecvData.proAp.proApData[0];
			PrintfXTOS("apRTU recv msg, apCmd = 0x%x msgCmd = 0x%x \n",apMsgRecvData.msgInfo.apCmdId, msgCmd);

			if (apMsgRecvData.msgInfo.apCmdId == AP_RTU_CMD_COMM || apMsgRecvData.msgInfo.apCmdId == AP_RTU_CMD_485)
			{
				for (i = 0; i < sizeof(mApRTUCmdFunc)/sizeof(ap_cmd_func_map_t); i++)
				{
					if(mApRTUCmdFunc[i].cmd== msgCmd)
					{
						if(NULL != mApRTUCmdFunc[i].processFunc) {
							mApRTUCmdFunc[i].processFunc(&apMsgRecvData);
						}
						else
							LOG_PRINTF(LOG_LEVEL_WARNING, "apRTU: unrealize cId=0x%x \n", msgCmd);
						break;
					}
					else
					{
						if(i + 1 == sizeof(mApRTUCmdFunc)/sizeof(ap_cmd_func_map_t))
						{
							LOG_PRINTF(LOG_LEVEL_WARNING, "apRTU: unsupport cId=0x%x \n", msgCmd);
						}
					}
				}
			}

		}
#else
		TaskSleepMs(500);
		XtBspToggleRunLed();
#endif
		KEY_getVal();
		KEY_doAction();
		//apRTUOnDimStepByStep();

	}	
}

static xt_void apRTUInit(void)
{
#if 0
	//xt_u8 i;
	PrintfXTOS("apRTUInit... \n");
//	for(i=0;i<RTU_ACTION_MAX_NUM;i++){
		 //pRTUTask[i] = (ap_rtu_task_set_t *)(glbRtuPara.flashTask.buf+sizeof(ap_rtu_task_set_t)*i);
		 pTaskInEerom = (ap_rtu_task_in_eerom_t*)glbRtuPara.flashTask.buf;
//	}

	if(SCB->VTOR == APP_START_ADDR)
		apRTULocal = RTU_UPDATE_LOCAL_PRIV;
	else if(SCB->VTOR == (APP_START_ADDR + APP_MAX_SIZE))
		apRTULocal = RTU_UPDATE_LOCAL_NEXT;

	PrintfXTOS("===== apRTUInit local = %d =====\n", apRTULocal);

	apRTUVersion = atoi(RTU_VERSION);
	PrintfXTOS("===== apRTUInit ver = %d =====\n", apRTUVersion);

	PrintfXTOS("===== apRTUInit DEVID = 0x%x  REVID = 0x%x =====\n", DBGMCU_GetDEVID(), DBGMCU_GetREVID());

	apParaInit();
	//apRTUTaskAction(accOpen,0x14);
	glbRtuPara.lastTime = XtBspGetTime();
#endif
	PrintfXTOS("apRTUInit... \n");
	if(RCC_GetFlagStatus(RCC_FLAG_OBLRST))
		PrintfXTOS("RCC_FLAG_OBLRST... \n");
	if(RCC_GetFlagStatus(RCC_FLAG_PINRST))
		PrintfXTOS("RCC_FLAG_PINRST... \n");
	if(RCC_GetFlagStatus(RCC_FLAG_PORRST))
		PrintfXTOS("RCC_FLAG_PORRST... \n");
	if(RCC_GetFlagStatus(RCC_FLAG_SFTRST))
		PrintfXTOS("RCC_FLAG_SFTRST... \n");
	if(RCC_GetFlagStatus(RCC_FLAG_IWDGRST))
		PrintfXTOS("RCC_FLAG_IWDGRST... \n");
	if(RCC_GetFlagStatus(RCC_FLAG_WWDGRST))
		PrintfXTOS("RCC_FLAG_WWDGRST... \n");	
	if(RCC_GetFlagStatus(RCC_FLAG_LPWRRST))
		PrintfXTOS("RCC_FLAG_LPWRRST... \n");	
	apParaInit();
	//drvLightDim(XT_BSP_PWM_0, 70);
}

ap_t* ApRTUGetInstance(void)
{
	if(gRTUData.instance.ApInit== NULL)
	{
		gRTUData.instance.ApInit = apRTUInit;
		gRTUData.instance.ApTask = apRTUTask;
	}
	return (ap_t*)&(gRTUData.instance);
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
