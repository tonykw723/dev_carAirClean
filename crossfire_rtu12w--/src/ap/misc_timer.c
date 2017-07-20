/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>

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
#include "conn_msg.h"

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
#include "lib_st_aes.h"
#include "discover_board.h"
#include "drv_light.h"


#include "config.h"
#define TIMER_TASK_PERIOD 1000	//1000ms

extern xt_u8 keyPressed;
extern rtu_para_t glbRtuPara;
extern sensor_type sensor;
static os_timer_t miscTimerHandle = NULL;
#if 0
xt_u8 tbuf[256] = {0};
xt_s32 tlen = 0;
#endif

static xt_s32 apAesTestProc(xt_void)
{
	//xt_u8 tRaw[] = {0x0,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0x0,0x0,0x0};
	//xt_u8 tRaw[] = {0x68 ,0x00 ,0x00 ,0x00 ,0x00 ,0xc3 ,0x40 ,0x68 ,0xf5 ,0x04 ,0x03 ,0x01 ,0xff ,0x01 ,0xd0 ,0x16};
	xt_u8 tRaw[] = {0x68 ,0x00 ,0x00 ,0x00 ,0x00 ,0x2d ,0x79 ,0x68 ,0x01 ,0x03 ,0x43 ,0x00 ,0x03 ,0xc0 ,0x16 ,0x0};
	xt_u8 tAes[16] = {0};
	xt_u8 tDes[16] = {0};
	//xt_u32 t1, t2, t3;
	//xt_u32 i = 0;
	//xt_u32 bufLen;

	//for (i = 0; i < 256; i++)
		IotAesEncryptEcb(tRaw, sizeof(tRaw), tAes);

	//for (i = 0; i < 256; i++)
		IotAesDecryptEcb(tAes, sizeof(tAes), tDes);

	if(memcmp(tRaw, tDes, 16) != 0)
		PrintfXTOS("--- apAesTestProc ERR!!! ...\n");
	else
		PrintfXTOS("--- apAesTestProc OK!!! ...\n");
	return 0;
}
//#define TTT
//#define XXX
//#define PPP

static xt_void miscTimerCallback( os_timer_t pxTime)
{
	static xt_s32 cnt = 0;
	#ifdef TTT
	static xt_u32 sendCnt=0;
	static xt_u8 sendT[]=
		//{0x7e,0x00,0x13,0x10,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xfe,0x00,0x00,0x54,0x78,0x44,0x61,0xad};
		{0x7E,0x00,0x10,0x10,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFE,0x00,0x00,0x12,0x34,0x12,0x34,0xAD};
	xt_u8 i,crc8T,lenT;
	#endif
	#ifdef XXX
	xt_u8 yfSendBuf[]={0x20,0x01,0x00,0x21};
	xt_u8 yfRecvBuf[30],yfLen=0;
	static xt_u8 aT=0;
	#endif
	
#if 0
	xt_u8 sendbuf[] = {0x68,0,0,0,0,0x43,0x21,0x68,0x01,0x03,0x42,0x14,0x03,0x91,0x16};
	xt_u8 sendbuf1[] = {0x68,0,0,0,0,0x43,0x21,0x68,0x01,0x03,0x43,0x0,0x03,0x7e,0x16};
#endif

	PrintfXTOS("tick=%x\n",cnt++);

	XtBspWdgFeed();
	//------------------------------------
	#ifdef TTT
	sendCnt++;
	lenT=sizeof(sendT)-4;//0x0e+4;
	sendT[2]=lenT;
	sendT[lenT+3-4]=sendCnt>>24;
	sendT[lenT+3-3]=sendCnt>>16;
	sendT[lenT+3-2]=sendCnt>>8;
	sendT[lenT+3-1]=sendCnt&0xff;
	crc8T=UTDigiCrc8(&sendT[3],lenT);
	sendT[lenT+3]=crc8T;
	XtBspCtrlWrite(sendT,sizeof(sendT));	
	PrintfXTOS(" %dth send:\n",sendCnt);
	for(i=0;i<sizeof(sendT);i++)
		PrintfXTOS(" %0.2x",sendT[i]);	
	PrintfXTOS("\n");
	#endif
	//-------------------------------------
	XtBspGetTime();

	if(glbRtuPara.runningInfo.flag_onine==YES)
	{
		if(glbRtuPara.runningInfo.heartBeatPeriod>0)
			glbRtuPara.runningInfo.heartBeatPeriod--;
		else
		{
			glbRtuPara.runningInfo.flag_heartBeatCmdStart=YES;
			glbRtuPara.runningInfo.heartBeatPeriod=HEART_BEAT_PERIOD;
		}
	}
	if(glbRtuPara.devPara.flag_dev_ini==YES)//3G init success
	{
		if(glbRtuPara.runningInfo.loginPeriod>0)
			glbRtuPara.runningInfo.loginPeriod--;
		else
		{
			glbRtuPara.runningInfo.flag_onlineCmdStart=YES;
			glbRtuPara.runningInfo.loginPeriod=LOGIN_PERIOD;
		}
	}
	if(glbRtuPara.runningInfo.DLZ_timeOut>0)
	{
		glbRtuPara.runningInfo.DLZ_timeOut--;
		if(glbRtuPara.runningInfo.DLZ_timeOut==0)
		{
			drvDLZClose();
			glbRtuPara.runningInfo.flag_DLZ_Opened=NO;
		}
	}

	if(glbRtuPara.runningInfo.taskRunTime>0)
	{
		glbRtuPara.runningInfo.taskRunTime--;
		if(glbRtuPara.runningInfo.taskRunTime==0)
			glbRtuPara.runningInfo.flag_taskStart=DONE;
		PrintfXTOS("~~~~~timeTask doing ,wait %d sec\n",glbRtuPara.runningInfo.taskRunTime);
	}
		
	
#if 0
	if (cnt %5 == 0 && cnt% 10 != 0)
	{
		ConnMsgWrite(sendbuf, sizeof(sendbuf));
		while(TRUE)
		{
			ConnMsgRespRead(tbuf, &tlen);
			if(tlen > 0)
				break;
		}

		printf("miscTimerCallback resp len = %d\n", tlen);
	}
	if (cnt % 10 == 0)
	{
		ConnMsgWrite(sendbuf1, sizeof(sendbuf1));
		while(TRUE)
		{
			ConnMsgRespRead(tbuf, &tlen);
			if(tlen > 0)
				break;
		}

		printf("miscTimerCallback resp len = %d\n", tlen);
	}
#endif
	if(cnt % 2  == 0)
		XtBspOpenRunLed();
	else
		XtBspCloseRunLed();

	if(cnt % 3  == 0)
	{
		sensor_debug();
		//wcdmadTest();
	}

	//apAesTestProc();
}

xt_void MiscTimerInit(void)
{
	miscTimerHandle = TimerCreateXTOS("misc timer", TIMER_TASK_PERIOD, TRUE, 0, miscTimerCallback);

	TimerStartXTOS(miscTimerHandle);
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
