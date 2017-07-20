/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>
#include <math.h>  

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

#include "TH_am23xx.h"
#include "VOC_zp01.h"
#include "displayLed.h"

#include "config.h"
#define GET_POWER_DEBUG_INFO
extern AD_type  ad_cvs;

sensor_type sensor;
xt_s32 v100;
ap_measure_priv_data_t gMeasureData = {0};
#define idx_PM2_5 0
//#define idx_VOC 0

//========================================================
float pm25_1equation[11][2]=
{
	{0.6,-0.57},
	{0.18,-0.15},
	{0.14,-0.26},
	{0.2,-0.2},
	{0.12,-0.02},

	{0.16,-0.12},
	{0.14,-0.06},
	{0.26,-0.45},
	{0.5,-1.29},
	{1,-3.09},
	{34,-123.54},
};

xt_u16 pm25_xVal[11][2]=
{
	{0,95},
	{95,150},
	{150,175},
	{175,200},
	{200,225},

	{225,275},
	{275,300},
	{300,325},
	{325,350},
	{350,365},
	{365,366},
};





void sensor_debug(void)
{
	xt_u8 i,j;
	xt_tm_t tm={0};
	
	// ȡʱ
	if(XtBspGetTmTime(XtBspGetTime(),&tm) == SUCCESS_XT)
	{
		PrintfXTOS(">>>date&time:20%d-%d-%d w%d %d:%d:%d\n",tm.tm_year-100,tm.tm_mon+1,tm.tm_mday,tm.tm_wday,tm.tm_hour,tm.tm_min,tm.tm_sec);		
	}
	#ifdef GET_POWER_DEBUG_INFO
	PrintfXTOS("^^^^^^^^^^^sensor_value^^^^^^^^^^^^\r\n");
	for(i=0;i<CHANNLE;i++)
	{
		PrintfXTOS("----channle=%d,sample =32:----\r\n",i);
		for(j=1;j<10+1;j++)
			PrintfXTOS(" %d",ad_cvs.buf[i][j]);
		PrintfXTOS("\r\n");
		for(j=11;j<20+1;j++)
			PrintfXTOS(" %d",ad_cvs.buf[i][j]);
		PrintfXTOS("\r\n");
		for(j=21;j<SAMPLE+1;j++)
			printf(" %d",ad_cvs.buf[i][j]);
		PrintfXTOS("\r\n");		
	}	
	PrintfXTOS("-----------------------------\r\n");
	PrintfXTOS("v100=%d\n",v100);
	PrintfXTOS("\r\nTP_val=%d,HM_val=%d,PM2_5_val=%0.4f,VOC_val=%x\r\n",sensor.TP_val,sensor.HM_val,sensor.PM2_5_val,sensor.VOC_val);
	
	printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\r\n");
	#endif
}

void calculateTPandHM(void)
{
	TH_getVal((char *)&sensor.HM_val,(unsigned char*)&sensor.TP_val);
}

void calculatePM2_5(void)
{
	xt_u8 i,j=0;
	xt_u32 sum=0;
	xt_u32 average;
	
	for(i=1;i<SAMPLE+1;i++)
		sum+=ad_cvs.buf[idx_PM2_5][i];
	average=sum/SAMPLE;
	PrintfXTOS("PM2_5-average1=%d\n",average);
	sum=0;
	for(i=1;i<SAMPLE+1;i++)
	{
		if(ad_cvs.buf[idx_PM2_5][i]>=average)
		{
			sum+=ad_cvs.buf[idx_PM2_5][i];
			j++;
		}
	}
	average=sum/j;
	PrintfXTOS("PM2_5-average2=%d\n",average);
	v100=average*2*5*100/4096;
	
	for(i=0;i<11;i++)
	{
		if((v100>=pm25_xVal[i][0])&&(v100<=pm25_xVal[i][1]))
			break;
	}
	if(v100>pm25_xVal[10][1])
	{
		PrintfXTOS("PM2_5-v100=%d>366,max data\n",v100);
		sensor.PM2_5_val=0.8;
	}
	else
		sensor.PM2_5_val=v100*pm25_1equation[i][0]/100+pm25_1equation[i][1];
	if(sensor.PM2_5_val<0)
		sensor.PM2_5_val=0;
	sensor.PM2_5_val=sensor.PM2_5_val*1000;
	PrintfXTOS("PM2_5-v100=%d,calculatePM2_5=%f\n",v100,sensor.PM2_5_val);
}

void calculateVOC(void)
{
	sensor.VOC_val=VOC_getVal();
	PrintfXTOS("sensor.VOC_val=%x\n",sensor.VOC_val);
}

void get_sensor_value(void)
{
	
	calculatePM2_5();
	calculateTPandHM();
	calculateVOC();
	if((sensor.PM2_5_val<=25))
	{
		FLED_green();
		DLED_green();
	}
	else if((sensor.PM2_5_val<=50))
	{
		FLED_blue();
		DLED_blue();
	}
	else if((sensor.PM2_5_val<=75))
	{
		FLED_yellow();
		DLED_yellow();
	}
	else
	{
		FLED_red();
		DLED_red();
	}

}


static xt_void apMeasureCalc()
{
	ad_cvs.sample = 0;
	
	XtBspAdcTimerStart();

	while(ad_cvs.sample < SAMPLE_NUM)
	{
		TaskSleepMs(1);
	};
	LXXPWMCfg_forPM2_5(0);//close PWM
	get_sensor_value();
	//sensor_debug();
}

static xt_void apMeasureTask(xt_void *pvParameters)
{
	while(TRUE)
	{
#if 0
		PrintfXTOS("--- apMeasureTask \n");
		//XtBspCtrlWrite("--- apMeasureTask \n",strlen("--- apMeasureTask \n"));
		TaskSleepMs(1000);
#else
		KEY_getVal();
		KEY_doAction();
		apMeasureCalc();
		TaskSleepMs(1000);
		//FLED_test();
#endif
	}	
}

static xt_void apMeasureInit(void)
{
	PrintfXTOS("apMeasureInit... \n");
}

ap_t* ApMeasureGetInstance(void)
{
	if(gMeasureData.instance.ApInit== NULL)
	{
		gMeasureData.instance.ApInit = apMeasureInit;
		gMeasureData.instance.ApTask = apMeasureTask;
	}
	return (ap_t*)&(gMeasureData.instance);
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
