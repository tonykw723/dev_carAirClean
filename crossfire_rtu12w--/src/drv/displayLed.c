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
#include "displayLed.h"

extern rtu_para_t glbRtuPara;
void DLED_GPIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_AHBPeriphClockCmd(GPIO_LED_BLUE_CLK|GPIO_LED_RED_CLK|GPIO_LED_RED_CLK, ENABLE|GPIO_FLAG_LED_BLUE_CLK);
  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_LED_BLUE_PIN|GPIO_LED_RED_PIN|GPIO_LED_GREEN_PIN|GPIO_FLAG_LED_BLUE_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIO_LED_PORT, &GPIO_InitStructure);
  //DLED_WriteLedEn(1);
  
}

void FLED_GPIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  RCC_AHBPeriphClockCmd(GPIO_FLAG_LED_RED_CLK, ENABLE);
  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_FLAG_LED_RED_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIO_FLAG_LED_RED_PORT, &GPIO_InitStructure);


    /* Enable the GPIO_LED Clock */
    RCC_AHBPeriphClockCmd(GPIO_FLAG_LED_GREEN_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_FLAG_LED_GREEN_PIN;
  /* Configure the GPIO_LED pin */
  GPIO_Init(GPIO_FLAG_LED_GREEN_PORT, &GPIO_InitStructure);

  
  
    /* Enable the GPIO_LED Clock */
    RCC_AHBPeriphClockCmd(GPIO_FLAG_LED_BLUE_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_FLAG_LED_BLUE_PIN;
  /* Configure the GPIO_LED pin */
  GPIO_Init(GPIO_FLAG_LED_BLUE_PORT, &GPIO_InitStructure);
}

//---------·ÕÎ§µÆ
void DLED_close(void)
{
	   DLED_WriteBlue(0);
	   DLED_WriteRed(0);
	   DLED_WriteGreen(0);
	   glbRtuPara.runningInfo.flag_DLED=NO;
}

void DLED_green(void)
{
	   if(	glbRtuPara.runningInfo.flag_DLED_en)
	   {
		   DLED_WriteBlue(0);
		   DLED_WriteRed(0);
		   DLED_WriteGreen(1);
		   glbRtuPara.runningInfo.flag_DLED=YES;
	   }
	   else
	   	DLED_close();
}

void DLED_blue(void)
{
	   if(	glbRtuPara.runningInfo.flag_DLED_en)
	   {
		   DLED_WriteBlue(1);
		   DLED_WriteRed(0);
		   DLED_WriteGreen(0);
		   glbRtuPara.runningInfo.flag_DLED=YES;
	   }
	   else
	   	DLED_close();
}

void DLED_yellow(void)
{
	   if(	glbRtuPara.runningInfo.flag_DLED_en)
	   {
		   DLED_WriteBlue(0);
		   DLED_WriteRed(1);
		   DLED_WriteGreen(1);
	   }
	   else
	   	DLED_close();
}

void DLED_red(void)
{
	   if(	glbRtuPara.runningInfo.flag_DLED_en)
	   {
		   DLED_WriteBlue(0);
		   DLED_WriteRed(1);
		   DLED_WriteGreen(0);
		   glbRtuPara.runningInfo.flag_DLED=YES;
	   }
	   else
	   	DLED_close();
}


//--------Ö¸Ê¾µÆ
void FLED_close(void)
{
	   FLED_WriteBlue(0);
	   FLED_WriteRed(0);
	   FLED_WriteGreen(0);
	   glbRtuPara.runningInfo.flag_FLED=NO;
}

void FLED_green(void)
{
	   if(	glbRtuPara.runningInfo.flag_FLED_en)
	   {
		   FLED_WriteBlue(0);
		   FLED_WriteRed(0);
		   FLED_WriteGreen(1);
		   glbRtuPara.runningInfo.flag_FLED=YES;
	   }
	   else
	   	FLED_close();
}

void FLED_blue(void)
{
	   if(	glbRtuPara.runningInfo.flag_FLED_en)
	   {
		   FLED_WriteBlue(1);
		   FLED_WriteRed(0);
		   FLED_WriteGreen(0);
		   glbRtuPara.runningInfo.flag_FLED=YES;
	   }
	   else
	   	FLED_close();
}

void FLED_yellow(void)
{
	   if(	glbRtuPara.runningInfo.flag_FLED_en)
	   {
		   FLED_WriteBlue(0);
		   FLED_WriteRed(1);
		   FLED_WriteGreen(1);
	   }
	   else
	   	FLED_close();
}

void FLED_red(void)
{
	   if(	glbRtuPara.runningInfo.flag_FLED_en)
	   {
		   FLED_WriteBlue(0);
		   FLED_WriteRed(1);
		   FLED_WriteGreen(0);
		   glbRtuPara.runningInfo.flag_FLED=YES;
	   }
	   else
	   	FLED_close();
}

void DLED_test(void)
{
#if 1
	static char flag=1;
	switch(flag)
	{
		case 0:
			   DLED_WriteBlue(0);
			   DLED_WriteRed(0);
			   DLED_WriteGreen(0);
			   flag=1;
			break;
		case 1:
			   DLED_WriteBlue(0);
			   DLED_WriteRed(0);
			   DLED_WriteGreen(0);
			   flag=2;
			break;
		case 2:
			   DLED_WriteBlue(1);
			   DLED_WriteRed(0);
			   DLED_WriteGreen(0);
			   flag=3;
			break;
		case 3:
			   DLED_WriteBlue(0);
			   DLED_WriteRed(1);
			   DLED_WriteGreen(0);
			   flag=4;
			break;
		case 4:
			   DLED_WriteBlue(0);
			   DLED_WriteRed(0);
			   DLED_WriteGreen(1);
			   flag=5;
			break;
		case 5://×ÏÉ«
			   DLED_WriteBlue(1);
			   DLED_WriteRed(1);
			   DLED_WriteGreen(0);
			   flag=6;
			break;
		case 6://»Æ
			   DLED_WriteBlue(0);
			   DLED_WriteRed(1);
			   DLED_WriteGreen(1);
			   flag=7;
			break;
		case 7://ÌìÀ¶
			   DLED_WriteBlue(1);
			   DLED_WriteRed(0);
			   DLED_WriteGreen(1);
			   flag=8;
			break;
		case 8://°×
			   DLED_WriteBlue(1);
			   DLED_WriteRed(1);
			   DLED_WriteGreen(1);
			   flag=0;
			break;
	}
#else
	   FLED_WriteBlue(0);
	   FLED_WriteRed(1);
	   FLED_WriteGreen(0);
#endif
	
}

void FLED_test(void)
{
#if 0
	static char flag=1;
	switch(flag)
	{
		case 0:
			   FLED_WriteBlue(0);
			   FLED_WriteRed(0);
			   FLED_WriteGreen(0);
			   flag=1;
			break;
		case 1:
			   FLED_WriteBlue(0);
			   FLED_WriteRed(0);
			   FLED_WriteGreen(0);
			   flag=2;
			break;
		case 2:
			   FLED_WriteBlue(1);
			   FLED_WriteRed(0);
			   FLED_WriteGreen(0);
			   flag=3;
			break;
		case 3:
			   DLED_WriteBlue(0);
			   DLED_WriteRed(1);
			   DLED_WriteGreen(0);
			   flag=4;
			break;
		case 4:
			   FLED_WriteBlue(0);
			   FLED_WriteRed(0);
			   FLED_WriteGreen(1);
			   flag=5;
			break;
		case 5://×ÏÉ«
			   FLED_WriteBlue(1);
			   FLED_WriteRed(1);
			   FLED_WriteGreen(0);
			   flag=6;
			break;
		case 6://»Æ
			   FLED_WriteBlue(0);
			   FLED_WriteRed(1);
			   FLED_WriteGreen(1);
			   flag=7;
			break;
		case 7://ÌìÀ¶
			   FLED_WriteBlue(1);
			   FLED_WriteRed(0);
			   FLED_WriteGreen(1);
			   flag=8;
			break;
		case 8://°×
			   FLED_WriteBlue(1);
			   FLED_WriteRed(1);
			   FLED_WriteGreen(1);
			   flag=0;
			break;
	}
#else
	   FLED_WriteBlue(0);
	   FLED_WriteRed(1);
	   FLED_WriteGreen(0);
#endif
	
}
 



