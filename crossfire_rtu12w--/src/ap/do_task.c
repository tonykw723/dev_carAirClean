#include <stdio.h>
#include <stdlib.h>
#include "stm32l1xx.h"
#include "xtsys_cfg.h"
#include "xtsys_type.h"
#include "xtos_task.h"
#include "xtos_timer.h"


#include "xt_bsp_rtu09c.h"
#include "discover_board.h"
#include "drv_uart.h"
#include "drv_light.h"
#include "global_para.h"
#include "do_task.h"

extern rtu_para_t glbRtuPara;
extern sensor_type sensor;

void do_sensorTask(void)
{
	static xt_u8 f_done=0;
	if(glbRtuPara.flashConfig.value.runMode==HAND_MODE)
	{
		f_done=0;
		return;
	} 
	if((sensor.PM2_5_val<=25))
	{
		if(f_done==1)
			return;
		PrintfXTOS("do_sensorTask fan speed1\n");
		FAN_SPEED1();
		glbRtuPara.runningInfo.flag_fanAdjLevel=1;
		glbRtuPara.runningInfo.flag_fanOpened=YES;
		f_done=1;
	}
	else if((sensor.PM2_5_val<=40))
	{
		if(f_done==2)
			return;
		PrintfXTOS("do_sensorTask fan speed2\n");
		FAN_SPEED2();
		glbRtuPara.runningInfo.flag_fanAdjLevel=2;
		glbRtuPara.runningInfo.flag_fanOpened=YES;
		f_done=2;
	}	
	else if((sensor.PM2_5_val<=55))
	{
		if(f_done==3)
			return;
		PrintfXTOS("do_sensorTask fan speed3\n");
		FAN_SPEED3();
		glbRtuPara.runningInfo.flag_fanAdjLevel=3;
		glbRtuPara.runningInfo.flag_fanOpened=YES;
		f_done=3;
	}
	else if(f_done!=4)
	{
		PrintfXTOS("do_sensorTask fan speed4\n");
		FAN_SPEED4();
		glbRtuPara.runningInfo.flag_fanAdjLevel=0;
		glbRtuPara.runningInfo.flag_fanOpened=YES;
		f_done=4;
	}
}

void do_timeSensorTask(void)
{
	xt_tm_t tm={0};
	xt_u8 i=0;
	task_type *p_task=NULL;
	p_task=(task_type*)&glbRtuPara.flashTask.value.whichDay1;
	//PrintfXTOS("~~~~~TTTTTTT~~~~~\n");
	//PrintfXTOS("p_task=%x,&glbRtuPara.flashTask.value.whichDay1-2-3-4=%x - %x - %x - %x\n",p_task,&glbRtuPara.flashTask.value.whichDay1,
	//	&glbRtuPara.flashTask.value.whichDay2,&glbRtuPara.flashTask.value.whichDay3,&glbRtuPara.flashTask.value.whichDay4);
	// 取时
	for(i=0;i<4;i++,p_task++)
	{
		//PrintfXTOS("i=%d,p_task =%x\n",i,p_task);
		if(XtBspGetTmTime(XtBspGetTime(),&tm) == SUCCESS_XT)
		{
			//PrintfXTOS("whichDay=%.2x,tm_wday=%0.2x\n",p_task->whichDay,tm.tm_wday);
			if(tm.tm_wday==0)
			{
				tm.tm_wday=7;
			}
			else if (tm.tm_wday>7)
				continue;
			if((p_task->whichDay&(0x01<<tm.tm_wday-1))==0)
				continue;
			//PrintfXTOS("p_task->hour=%.2d:%0.2d,tm.tm_hour=%0.2d:%0.2d\n",p_task->hour,p_task->minute,tm.tm_hour,tm.tm_min);
			if((tm.tm_hour==p_task->hour)
				&&(tm.tm_min==p_task->minute))
			{
				if(glbRtuPara.runningInfo.flag_taskStart==NO)
				{
					glbRtuPara.runningInfo.flag_taskStart=YES;
					glbRtuPara.runningInfo.taskRunTime=p_task->howmanyMinutes*60+10;
					glbRtuPara.flashConfig.value.runMode=AUTO_MODE;//定时时间到了
					glbRtuPara.runningInfo.flag_DLED_en=NO;//关闭氛围灯
					glbRtuPara.runningInfo.flag_FLED_en=YES;//开启指示灯
					glbRtuPara.runningInfo.flag_powerEn=YES;//当前状态为开启
					PrintfXTOS("~~~~~timeTask start,do %d sec\n",glbRtuPara.runningInfo.taskRunTime);
				}
			}
		}
		if(glbRtuPara.runningInfo.flag_taskStart==NO)
			continue;
		else if(glbRtuPara.runningInfo.flag_taskStart==DONE)//定时任务超时了
		{
			glbRtuPara.runningInfo.flag_taskStart=NO;
			PrintfXTOS("~~~~~timeTask timeout:close DLZ FAN POWER\n");
			drvDLZClose();	
			drvFanDim(3);
			drvPowerClose();
			glbRtuPara.runningInfo.flag_DLED_en=NO;
			glbRtuPara.runningInfo.flag_FLED_en=NO;
			glbRtuPara.runningInfo.flag_powerEn=NO;
			glbRtuPara.flashConfig.value.runMode=HAND_MODE;
			continue;
		}
		else if(glbRtuPara.runningInfo.flag_taskStart==YES)
		{
				//执行定时任务
				do_sensorTask();
		}
		
	}
}

