#include <stdio.h>
#include <stdlib.h>
#include "stm32l1xx.h"
#include "xtsys_cfg.h"
#include "xtsys_type.h"
#include "xtos_task.h"

#include "xt_bsp_rtu09c.h"
#include "discover_board.h"
#include "drv_uart.h"
#include "drv_light.h"
#include "global_para.h"
#include "key.h"
#include "pro_ap.h"
#if 1
static char cntPower=0,cntMode=0,cntFanAdj=0,cntDLZ=0;
static char flagPowerPressed=0,flagModePressed=0,flagFanAdjPressed=0,flagDLZPressed=0;
key_t key;
extern rtu_para_t glbRtuPara;

void KEY_GPIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_AHBPeriphClockCmd(GPIO_KEY_POWER_CLK|GPIO_KEY_MODE_CLK|GPIO_KEY_FAN_ADJ_CLK|GPIO_KEY_DLZ_CLK, ENABLE);
  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_KEY_POWER_PIN|GPIO_KEY_MODE_PIN|GPIO_KEY_FAN_ADJ_PIN|GPIO_KEY_DLZ_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIO_KEY_PORT, &GPIO_InitStructure);

  
}

char  KEY_cheak(char *flag,char *cnt,unsigned char(*fun)(void))
{
	if(*flag==0)
	{
		if((*fun)()==0)
		{
			(*cnt)++;
			if(*cnt>2)
			{
				*flag=1;//key pressed
				*cnt=0;
			}
		}
		else
			*cnt=0;
	}
	else
	{
		if((*fun)())//wait release
		{
			(*cnt)++;
			if(*cnt>2)
			{
				*flag=0;
				*cnt=0;
				return 1;
			}
		}
		else
			*cnt=0;
	}
	return 0;
}

#if 1
unsigned char KEY_readPower(void)
{
	return GPIO_ReadInputDataBit(GPIO_KEY_POWER_PORT, GPIO_KEY_POWER_PIN);
}

unsigned char KEY_readMode(void)
{
	return GPIO_ReadInputDataBit(GPIO_KEY_MODE_PORT, GPIO_KEY_MODE_PIN);
}

unsigned char KEY_readFanAdj(void)
{
	 return GPIO_ReadInputDataBit(GPIO_KEY_FAN_ADJ_PORT, GPIO_KEY_FAN_ADJ_PIN);
}


unsigned char KEY_readDLZ(void)
{
	return GPIO_ReadInputDataBit(GPIO_KEY_DLZ_PORT, GPIO_KEY_DLZ_PIN);
}
#endif 


void KEY_getVal(void)
{
	KEY_WritePower(1);
	KEY_WriteMode(1);
	KEY_WriteFanAdj(1);
	KEY_WriteDLZ(1);
	if(KEY_cheak(&flagPowerPressed,&cntPower,&KEY_readPower))
		key.bitPower=1;
	if(KEY_cheak(&flagModePressed,&cntMode,&KEY_readMode))
		key.bitMode=1;		
	if(KEY_cheak(&flagFanAdjPressed,&cntFanAdj,&KEY_readFanAdj))
		key.bitFanAdj=1;	
	if(KEY_cheak(&flagDLZPressed,&cntDLZ,&KEY_readDLZ))
		key.bitDLZ=1;	
	TaskSleepMs(10);
	
}


void KEY_doAction(void)
{
	if(key.bitPower)
	{
		key.bitPower=0;
		PrintfXTOS("key power press to do...\n");
		if(glbRtuPara.runningInfo.flag_powerEn==NO)
		{

			glbRtuPara.runningInfo.flag_taskStart=DONE;
			glbRtuPara.runningInfo.flag_taskStart=glbRtuPara.runningInfo.flag_taskStart_backup;
			glbRtuPara.runningInfo.flag_DLED_en=NO;//关闭氛围灯
			glbRtuPara.runningInfo.flag_FLED_en=YES;//开启指示灯
			glbRtuPara.runningInfo.flag_powerEn=YES;//当前状态为开启
			drvDLZOpen();
			glbRtuPara.flashConfig.value.runMode=AUTO_MODE;//进入传感器任务
			apComm_doReport(accDoAll,1);
		}
		else
		{
			//goto idle status
			drvFanDim( 0);
			glbRtuPara.runningInfo.flag_fanOpened=NO;
			drvDLZClose();			
			glbRtuPara.runningInfo.flag_taskStart=NO;
			glbRtuPara.runningInfo.flag_taskStart_backup=glbRtuPara.runningInfo.flag_taskStart;
			glbRtuPara.runningInfo.flag_DLED_en=NO;
			glbRtuPara.runningInfo.flag_FLED_en=NO;
			glbRtuPara.runningInfo.flag_powerEn=NO;
			glbRtuPara.flashConfig.value.runMode=HAND_MODE;//退出传感器任务，直到定时任务生效
			glbRtuPara.runningInfo.flag_fanAdjLevel=0;
		}
	}
	else if(key.bitMode)
	{
		if(glbRtuPara.runningInfo.flag_powerEn==NO)
			return;
		key.bitMode=0;
		PrintfXTOS("key run mode press to do...\n");
		//if(glbRtuPara.flashConfig.value.runMode==HAND_MODE)
		//{
			glbRtuPara.flashConfig.value.runMode=AUTO_MODE;
			glbRtuPara.runningInfo.flag_DLED_en=NO;
			glbRtuPara.runningInfo.flag_FLED_en=YES;
		//}
		//else
		//	glbRtuPara.flashConfig.value.runMode=HAND_MODE;
		/*if(apWriteFlashCfgPara(WRITE_PARA)==YES)
			PrintfXTOS("press key to set run mode success\n");
		else
			PrintfXTOS("press key to set run mode fail\n");*/
	}
	else if(key.bitFanAdj)
	{
		if(glbRtuPara.runningInfo.flag_powerEn==NO)
			return;
		key.bitFanAdj=0;
		PrintfXTOS("key fanAdj press to do...\n");
		glbRtuPara.flashConfig.value.runMode=HAND_MODE;
		if(glbRtuPara.runningInfo.flag_fanAdjLevel==0)
		{
			drvFanDim( 20);
			apComm_doReport(accOpenFan,1);
			glbRtuPara.runningInfo.flag_fanAdjLevel=1;
			glbRtuPara.runningInfo.flag_fanOpened=YES;
			
		}
		else if(glbRtuPara.runningInfo.flag_fanAdjLevel==1)
		{
			drvFanDim(40);
			apComm_doReport(accOpenFan,1);
			glbRtuPara.runningInfo.flag_fanAdjLevel=2;
			glbRtuPara.runningInfo.flag_fanOpened=YES;
		}
		else if(glbRtuPara.runningInfo.flag_fanAdjLevel==2)
		{
			drvFanDim(60);
			apComm_doReport(accOpenFan,1);
			glbRtuPara.runningInfo.flag_fanAdjLevel=3;
			glbRtuPara.runningInfo.flag_fanOpened=YES;
		}
		else if(glbRtuPara.runningInfo.flag_fanAdjLevel==3)
		{
			drvFanDim(80);
			apComm_doReport(accOpenFan,1);
			glbRtuPara.runningInfo.flag_fanAdjLevel=0;
			glbRtuPara.runningInfo.flag_fanOpened=YES;
		}
	}
	else if(key.bitDLZ)
	{
		if(glbRtuPara.runningInfo.flag_powerEn==NO)
			return;
		key.bitDLZ=0;
		PrintfXTOS("key DLZ press to do...\n");
		glbRtuPara.flashConfig.value.runMode=HAND_MODE;
		if(glbRtuPara.runningInfo.flag_DLZ_Opened==NO)
		{
			drvDLZOpen();
			apComm_doReport(accOpenDLZ,1);
		}
		else
		{
		
			drvDLZClose();
			apComm_doReport(accCloseDLZ,1);
		}
	}
}
 

#endif


