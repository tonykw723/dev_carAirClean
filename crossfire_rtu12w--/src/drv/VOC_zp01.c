#include <stdio.h>
#include <stdlib.h>
#include "stm32l1xx.h"
#include "xtsys_type.h"
#include "VOC_zp01.h"

unsigned char VOC_getVal(void)
 {
	char valTemp=0;
	if(VOC_readA())
		valTemp|=0x02;
	if(VOC_readB())
		valTemp|=0x01;
	return valTemp;
 }    

void VOC_GPIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_AHBPeriphClockCmd(GPIO_VOC_A_CLK|GPIO_VOC_B_CLK, ENABLE);
  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_VOC_A_PIN|GPIO_VOC_B_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIO_VOC_A_PORT, &GPIO_InitStructure);

  
}
 


