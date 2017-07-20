/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>
#include <stdlib.h>

#include "xtsys_type.h"
#include "xtsys_cfg.h"

#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"
#include "xtos_sem.h"
#include "xtos_timer.h"

#include "ringqueue.h"

#include "xt_bsp_rtu09c.h"
#include "config.h"
#include "TH_am23xx.h"
#include "VOC_zp01.h"
#include "displayLed.h"

AD_type  ad_cvs = {0};

xt_bsp_param_win32_t win32ParamSt = {0};
#include "xtsys_type.h"
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)

#include "stm32l1xx.h"
#include "discover_board.h"
#include "stm32l_discovery_lcd.h"
//#include "i2c_m24lr04e-r.h"
#include "i2c_AT24LCxxx.h"
#elif defined(NUVOTON_NUC200)
#include "NUC200Series.h"
#include "initialization.h"
#else
#include <io.h>
#include <fcntl.h>
#include <windows.h>

HANDLE hComDebug;
xt_s32 fdComDebug;
FILE *pfComDebug;

HANDLE hComCtrl;
HANDLE hComCtrlResp = -1;
#endif
os_mutex_t ctrlUartMutex = NULL;
os_mutex_t odbUartMutex = NULL;
ringqueue_t *rq = NULL;
ringqueue_t *rq_odb = NULL;

volatile xt_u8 keyPressed = 0;

static xt_s32 powerCnt = 0;

#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)

#define NoRTC FALSE
#define WITHRTC !NoRTC
#define NoDIV2 FALSE
#define DIV2 !NoDIV2

/* ADC converter value for Bias current value*/
#if   (defined ( __CC_ARM ))
uint8_t Bias_Current __attribute__((at(0x08080000)));     
#elif (defined (__ICCARM__))
uint8_t Bias_Current @ ".DataFlash" ;  
#elif (defined (__GNUC__))
/* ADC converter value for Bias current value*/
uint8_t Bias_Current __attribute__((section(".DataFlash")));
#endif

void  Init_GPIOs (void)
{
#if 0
	/* GPIO, EXTI and NVIC Init structure declaration */
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	powerCnt = 0;
	
	/* Configure User Button pin as input */
	GPIO_InitStructure.GPIO_Pin = USERBUTTON_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(USERBUTTON_GPIO_PORT, &GPIO_InitStructure);

	/* Select User Button pin as input source for EXTI Line */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);

	/* Configure EXT1 Line 0 in interrupt mode trigged on Rising edge */
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 ;  // PA0 for User button AND IDD_WakeUP
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
#endif
}

void  ReInit_GPIOs (void)
{
	/* GPIO, EXTI and NVIC Init structure declaration */
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef RTU12
	/* Configure User Button pin as input */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Select User Button pin as input source for EXTI Line */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB,EXTI_PinSource11);

	/* Configure EXT1 Line 0 in interrupt mode trigged on Rising edge */
	EXTI_InitStructure.EXTI_Line = EXTI_Line11 ;  // PA0 for User button AND IDD_WakeUP
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure); 
#endif
	
}

void  UnInit_GPIOs (void)
{
	/* GPIO, EXTI and NVIC Init structure declaration */
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef RTU12
	/* Configure EXT1 Line 0 in interrupt mode trigged on Rising edge */
	EXTI_InitStructure.EXTI_Line = EXTI_Line11 ;  // PA0 for User button AND IDD_WakeUP
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = DISABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI0 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
	NVIC_Init(&NVIC_InitStructure); 
#endif
	
}

void TIM_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIOA Clocks enable */
  #if 1
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_TIM3);

  //-------------for sensor-----------------
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM11);
  
  #else
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_TIM3);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_TIM3);

 
  
  #endif
}

xt_s32 LXXPWMCfg(xt_u8 chn, xt_u8 dim)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint32_t TimerPeriod = 0;
	uint32_t Channel1Pulse = 0, Channel2Pulse = 0, Channel4Pulse = 0;

	/* Compute the value to be set in ARR regiter to generate signal frequency at 17.57 Khz */
	TimerPeriod = (SystemCoreClock / 25000 ) - 1;
	
	/* TIM3 counter disable */
	TIM_Cmd(TIM3, DISABLE);
	
	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3 , ENABLE);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* Channel 1, and 2 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	/*
	if (chn == 0)
	{
		if(dim > 0)
			Channel1Pulse = (uint16_t) (((uint32_t) dim * (TimerPeriod - 1)) / 100);
		else
			Channel1Pulse = 0;

		TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	}
	else if (chn == 1)
	{
		if(dim > 0)
			Channel2Pulse = (uint16_t) (((uint32_t) dim * (TimerPeriod - 1)) / 100);
		else
			Channel2Pulse = 0;

		TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
		TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	}*/

	//else if (chn == 4)
	{
		if(dim > 0)
			Channel4Pulse = (uint16_t) (((uint32_t) dim * (TimerPeriod - 1)) / 100);
		else
			Channel4Pulse = 0;

		TIM_OCInitStructure.TIM_Pulse = Channel4Pulse;
		TIM_OC4Init(TIM3, &TIM_OCInitStructure);
	}

	/* TIM3 counter enable */
	TIM_Cmd(TIM3, ENABLE);
	return 0;
}


xt_s32 LXXPWMCfg_forPM2_5(xt_u8 dim)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint32_t TimerPeriod = 0;
	uint32_t Channel1Pulse = 0, Channel2Pulse = 0, Channel4Pulse = 0;

	/* Compute the value to be set in ARR regiter to generate signal frequency at 17.57 Khz */
	TimerPeriod = (SystemCoreClock / 500 ) - 1;
	
	/* TIM3 counter disable */
	TIM_Cmd(TIM11, DISABLE);
	
	/* TIM3 clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11 , ENABLE);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Prescaler = 4;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);

	/* Channel 1, and 2 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	/*
	if (chn == 0)
	{
		if(dim > 0)
			Channel1Pulse = (uint16_t) (((uint32_t) dim * (TimerPeriod - 1)) / 100);
		else
			Channel1Pulse = 0;

		TIM_OCInitStructure.TIM_Pulse = Channel1Pulse;
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	}
	else if (chn == 1)
	{
		if(dim > 0)
			Channel2Pulse = (uint16_t) (((uint32_t) dim * (TimerPeriod - 1)) / 100);
		else
			Channel2Pulse = 0;

		TIM_OCInitStructure.TIM_Pulse = Channel2Pulse;
		TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	}*/

	//else if (chn == 4)
	{
		if(dim > 0)
			Channel4Pulse = (uint16_t) (((uint32_t) dim * (TimerPeriod - 1)) / 100);
		else
			Channel4Pulse = 0;

		TIM_OCInitStructure.TIM_Pulse = Channel4Pulse;
		TIM_OC1Init(TIM11, &TIM_OCInitStructure);
	}

	/* TIM3 counter enable */
	TIM_Cmd(TIM11, ENABLE);
	return 0;
}

void LSI_RCC_Configuration(void)
{
    RCC_LSICmd(ENABLE);                              //打开LSI
    while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);//等待直到LSI稳定
}

void IWDG_Configuration(void)
{
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);   // 打开使能，因为iwdg的寄存器有写保护,必须先写入0x5555，才能操作寄存器
  IWDG_SetPrescaler(IWDG_Prescaler_256);          // 独立看门狗使用内部低速振荡器LSI，对LSI进行256分频
  IWDG_SetReload(1023);                              // 设定独立看门狗计数器的计数值(0x000~0xFFF;0~4095)，复位时间为6s
  IWDG_ReloadCounter();                              // 重载独立看门狗计数器，向寄存器写入0xAAAA，或者更新计数值
  IWDG_Enable();                                  // 开启看门狗，向寄存器写入0xCCCC即可
}

void WWDG_Configuration(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	WWDG_SetPrescaler(WWDG_Prescaler_8);
	WWDG_SetWindowValue(0x7f);
	WWDG_Enable(0x7f);
}

void  Init_RTC (void)
{
	RTC_InitTypeDef RTC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	
	PWR_RTCAccessCmd(ENABLE);

	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();

	RTC_StructInit(&RTC_InitStructure);
	
	RTC_Init(&RTC_InitStructure);
}

void SetHSICLKToMSI(uint32_t freq,bool div2,bool With_RTC)
{
  /* RCC system reset */
  RCC_DeInit();

  /* Flash no latency*/
  FLASH_SetLatency(FLASH_Latency_0);
  
  /* Disable Prefetch Buffer */
  FLASH_PrefetchBufferCmd(DISABLE);

  /* Disable 64-bit access */
  FLASH_ReadAccess64Cmd(DISABLE);
         
  /* Disable FLASH during SLeep  */
  FLASH_SLEEPPowerDownCmd(ENABLE);
 
  /* Enable the PWR APB1 Clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /* Select the Voltage Range 3 (1.2V) */
  PWR_VoltageScalingConfig(PWR_VoltageScaling_Range3);

  /* Wait Until the Voltage Regulator is ready */
  while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET);

  /* Configure the MSI frequency */
  RCC_MSIRangeConfig(freq);
  
  /* Select MSI as system clock source */
  RCC_SYSCLKConfig(RCC_SYSCLKSource_MSI);

  /* Wait until MSI is used as system clock source */
  while (RCC_GetSYSCLKSource() != 0x00);

  if (div2)
  {
    RCC_HCLKConfig(RCC_SYSCLK_Div2);    
  }

  RCC_HSICmd(DISABLE);

  /* Disable HSE clock */
  RCC_HSEConfig(RCC_HSE_OFF);

  /* Disable LSE clock */
  if (! With_RTC)
    RCC_LSEConfig(RCC_LSE_OFF);

  /* Disable LSI clock */
  //todo: use LSI in discovery
  //RCC_LSICmd(DISABLE);  

}

/* Variables used for save GPIO configuration */
uint32_t GPIOA_MODER, GPIOB_MODER, GPIOC_MODER,GPIOD_MODER,GPIOE_MODER ,GPIOE_MODER,GPIOH_MODER;
uint32_t GPIOA_PUPDR, GPIOB_PUPDR , GPIOC_PUPDR, GPIOD_PUPDR,GPIOE_PUPDR,GPIOH_PUPDR;

void Restore_GPIO_Config(void)
{
  GPIOA->MODER = GPIOA_MODER;
  GPIOB->MODER = GPIOB_MODER;
  GPIOC->MODER = GPIOC_MODER;
  GPIOD->MODER = GPIOD_MODER;
  GPIOE->MODER = GPIOE_MODER;
  GPIOH->MODER = GPIOH_MODER;

  GPIOA->PUPDR = GPIOA_PUPDR;
  GPIOB->PUPDR = GPIOB_PUPDR;
  GPIOC->PUPDR = GPIOC_PUPDR;
  GPIOD->PUPDR = GPIOD_PUPDR;
  GPIOE->PUPDR = GPIOE_PUPDR;
  GPIOH->PUPDR = GPIOH_PUPDR;
}

/**
  * @brief  Function used to Configure the GPIO in low consumption
  * @caller ADC_Icc_Test
  * @param None
  * @retval None
  */
void GPIO_LowPower_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* store GPIO configuration before lowpower switch */
  GPIOA_MODER = GPIOA->MODER;
  GPIOB_MODER = GPIOB->MODER;
  GPIOC_MODER = GPIOC->MODER;
  GPIOD_MODER = GPIOD->MODER;
  GPIOE_MODER = GPIOE->MODER;
  GPIOH_MODER = GPIOH->MODER;
  GPIOA_PUPDR = GPIOA->PUPDR;
  GPIOB_PUPDR = GPIOB->PUPDR;
  GPIOC_PUPDR = GPIOC->PUPDR;
  GPIOD_PUPDR = GPIOD->PUPDR;
  GPIOE_PUPDR = GPIOE->PUPDR;
  GPIOH_PUPDR = GPIOH->PUPDR;
  
  /* Configure all GPIO port pins in Analog input mode (trigger OFF) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_400KHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;

  
  GPIOD->MODER   = 0xFFFFFFFF;
  GPIOE->MODER   = 0xFFFFFFFF;
  GPIOH->MODER   = 0xFFFFFFFF;
  
  /* all GPIOA except PA2 and PA3 which is used for UART1 */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1 | GPIO_Pin_6| GPIO_Pin_7 \
    | GPIO_Pin_13 | GPIO_Pin_14|GPIO_Pin_5 | GPIO_Pin_8  |GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |  GPIO_Pin_12 |GPIO_Pin_15 ;

	GPIO_Init(GPIOA, &GPIO_InitStructure);  

   /* All GPIOC except PC13 which is used for mesurement */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4|  GPIO_Pin_5 |GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8 \
                                 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |  GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15 ;
  GPIO_Init(GPIOC, &GPIO_InitStructure);  

   /* all GPIOB except PB6 and PB7 used for LED*/
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4|  GPIO_Pin_5 | GPIO_Pin_8 \
                                 | GPIO_Pin_9 |  GPIO_Pin_12 |GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15 ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  GPIO_LOW(GPIOB,GPIO_Pin_6);
  GPIO_LOW(GPIOB,GPIO_Pin_7);
}

void ADC_Icc_Init(void)
{
  ADC_InitTypeDef ADC_InitStructure;

/* Enable ADC clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

/* de-initialize ADC */
  ADC_DeInit(ADC1);

/*  ADC configured as follow:
  - NbrOfChannel = 1 - ADC_Channel_4
  - Mode = Single ConversionMode(ContinuousConvMode disabled)
  - Resolution = 12Bits
  - Prescaler = /1
  - sampling time 192 */

    /* ADC Configuration */
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
  ADC_InitStructure.ADC_ScanConvMode = ENABLE;
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfConversion = 1;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel4 configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_192Cycles);
  ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);

  ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE);
  
  /* Enable ADC1 */
  ADC_Cmd(ADC1, ENABLE);
  
  /* Wait until ADC1 ON status */
  while (ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET);
}

/* Theorically BandGAP 1.224volt */
#define VREF 		1.224L

/*
	ADC Converter 
	LSBIdeal = VREF/4096 or VDA/4096
*/
#define ADC_CONV 	4096

#define MAX_CURRENT 	99999


uint16_t Current_Measurement (void)
{
  uint16_t res,i;
#if 0
  /* re-start ADC chanel 24 for Current measurement */
  ADC_Icc_Init();	

  /* initialize result */
  res = 0;

  for(i=4; i>0; i--)
  {
    /* start ADC convertion by software */
    ADC_SoftwareStartConv(ADC1);
    
    /* wait until end-of-covertion */
    while( ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0 );
    
    /* read ADC convertion result */
    res += ADC_GetConversionValue(ADC1);
  }
#endif
  return (res>>2);
}

uint16_t ADC_Supply(void)
{
  uint8_t i;
  uint16_t res;
#if 0
    /* Initializes ADC */
  ADC_Icc_Init();
 
  ADC_TempSensorVrefintCmd(ENABLE);

  /* ADC1 regular channel 17 for VREF configuration */
  ADC_RegularChannelConfig(ADC1, ADC_Channel_17, 1, ADC_SampleTime_192Cycles);
  
  /* initialize result */
  res = 0;
  for(i=4; i>0; i--)
  {
  /* start ADC convertion by software */
    ADC_SoftwareStartConv(ADC1);

    /* wait until end-of-covertion */
    while( ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0 );
  /* read ADC convertion result */
    res += ADC_GetConversionValue(ADC1);
  }
	
  /* de-initialize ADC */
  ADC_TempSensorVrefintCmd(DISABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);
#endif
  return (res>>2);
}

float Vdd_appli(void)
{
  uint16_t MeasurINT ;

  float f_Vdd_appli ;
  
  /*Read the BandGap value on ADC converter*/
  MeasurINT = ADC_Supply();	
  
  /* We use the theorical value */
  f_Vdd_appli = (VREF/MeasurINT) * ADC_CONV;

  /* convert Vdd_appli into mV */  
  f_Vdd_appli *= 1000L;
	
  return f_Vdd_appli;
}

void convert_into_char(uint32_t number, uint16_t *p_tab)
{
  uint16_t units=0, tens=0, hundreds=0, thousands=0, misc=0;
  
  units = (((number%10000)%1000)%100)%10;
  tens = ((((number-units)/10)%1000)%100)%10;
  hundreds = (((number-tens-units)/100))%100%10;
  thousands = ((number-hundreds-tens-units)/1000)%10;
  misc = ((number-thousands-hundreds-tens-units)/10000);
  
  *(p_tab+4) = units + 0x30;
  *(p_tab+3) = tens + 0x30;
  *(p_tab+2) = hundreds + 0x30;
  *(p_tab+1) = thousands + 0x30;
  *(p_tab) = misc + 0x30;

}

float Icc_RUN(void)
{
  float Current;
  uint16_t MeasurINT;
  uint16_t tab[6];	
  /* Get Icc current value in Run mode*/	
  MeasurINT = Current_Measurement();
  /* Convert value in mA*/	
  Current = MeasurINT * Vdd_appli()/ADC_CONV;
  Current *= 100L; 
  /* Convert value in ASCII and store it into tab*/
  convert_into_char((uint32_t)(Current), tab);
  /* Add unit and decimal point  */
  tab[5] = 'A';
  tab[4] = 'm';
  tab[3] = ' ';
  tab[0] |= DOT; 
  /* Display mesured value */
  //LCD_GLASS_DisplayStrDeci(tab);

  PrintfXTOS("--- In Icc_RUN %s mA ...\n",tab);
  
  return (Current);
}

void Config_RCC(RCC_TypeDef *sav_RCC)
{
  /* Save the RCC configuration registers */
  sav_RCC->AHBENR   = RCC->AHBENR;
  sav_RCC->APB1ENR  = RCC->APB1ENR;
  sav_RCC->APB2ENR  = RCC->APB2ENR;
  sav_RCC->AHBLPENR   = RCC->AHBLPENR;
  sav_RCC->APB1LPENR  = RCC->APB1LPENR;
  sav_RCC->APB2LPENR  = RCC->APB2LPENR;
  
  /* Set low power configuration */
  RCC->AHBENR = 0x05; // Ports A and C enable
  RCC->AHBLPENR = 0x05; 
  RCC->APB1ENR =  RCC_APB1ENR_PWREN;     // PWR management enable     
  RCC->APB2ENR = RCC_APB2ENR_SYSCFGEN;
  
}

void RCC_Configuration(void)
{  
  /* Enable HSI Clock */
  //RCC_HSICmd(ENABLE);
  
  /*!< Wait till HSI is ready */
  //while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

  /* Set HSI as sys clock*/
  //RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  
  /* Set MSI clock range to ~4.194MHz*/
  RCC_MSIRangeConfig(RCC_MSIRange_6);
  
  /* Enable the GPIOs clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC| RCC_AHBPeriph_GPIOD| RCC_AHBPeriph_GPIOE| RCC_AHBPeriph_GPIOH, ENABLE);     

  /* Enable comparator, LCD and PWR mngt clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_COMP | RCC_APB1Periph_PWR,ENABLE);
    
  /* Enable ADC & SYSCFG clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_SYSCFG , ENABLE);

  /* Allow access to the RTC */
  //PWR_RTCAccessCmd(ENABLE);

  /* Reset RTC Backup Domain */
  //RCC_RTCResetCmd(ENABLE);
  //RCC_RTCResetCmd(DISABLE);

  /* LSE Enable */
  //RCC_LSEConfig(RCC_LSE_ON);

  /* Wait until LSE is ready */
  //while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
  
   /* RTC Clock Source Selection */ 
  //RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE); 
  
  /* Enable the RTC */
  //RCC_RTCCLKCmd(ENABLE);   
  
  /*Disable HSE*/
  //RCC_HSEConfig(RCC_HSE_OFF);
  //if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET )
  //{
    /* Stay in infinite loop if HSE is not disabled*/
    //while(1); 
  //}
}

void DEBUG_USART_Configuration()
{
#if 1
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	STM_EVAL_COMInit(COM3, &USART_InitStructure);
#else
	STM_EVAL_SIM_COMInit();
	
	//STM_EVAL_SIM_COM_PUTC(0x55);
#endif
}

void CTRL_USART_Configuration(xt_u32 buadrate,xt_u16 parity)
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = buadrate;
	if(parity==USART_Parity_No)
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	else
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//USART_StopBits_2;	//;
	USART_InitStructure.USART_Parity = parity;//USART_Parity_Even;//USART_Parity_Odd;// 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	STM_EVAL_COM_NVIC_Config(COM2, ENABLE);
	STM_EVAL_COMInit(COM2, &USART_InitStructure);
	USART_ITConfig(EVAL_COM2, USART_IT_RXNE, ENABLE);

	//USART_ClearFlag(EVAL_COM1, USART_FLAG_TC);
}

void ODB_USART_Configuration(xt_u32 buadrate,xt_u16 parity)
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = buadrate;
	if(parity==USART_Parity_No)
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	else
		USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//USART_StopBits_2;	//;
	USART_InitStructure.USART_Parity = parity;//USART_Parity_Even;//USART_Parity_Odd;// 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	
	STM_EVAL_COM_NVIC_Config(COM1, ENABLE);
	STM_EVAL_COMInit(COM1, &USART_InitStructure);
	USART_ITConfig(EVAL_COM1, USART_IT_RXNE, ENABLE);

}



#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

PUTCHAR_PROTOTYPE
{
#if 1
	if (ch == '\n')
	{
		uint8_t rch = '\r';

		while (USART_GetFlagStatus(EVAL_COM3, USART_FLAG_TC) == RESET)
		{}
		
		USART_SendData(EVAL_COM3, (uint8_t) rch);
	}

	while (USART_GetFlagStatus(EVAL_COM3, USART_FLAG_TC) == RESET)
	{}
	
	USART_SendData(EVAL_COM3, (uint8_t) ch);
#else
	if (ch == '\n')
	{
		uint8_t rch = '\r';

		STM_EVAL_SIM_COM_PUTC(rch);
	}

	STM_EVAL_SIM_COM_PUTC(ch);
#endif
	return ch;
}

xt_s32 XtBspWdgFeed();

void vApplicationTickHook( void )
{
	//systick(1ms) hook
#if defined (USE_WWDG_AS_WDG)
	XtBspWdgFeed();
#endif
}

void RTC_Wackup_Config(void)
{
	RTC_AlarmTypeDef  RTC_AlarmStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RTC_TimeTypeDef rtcTime = {0};
	xt_u32 totalTime = 0;
	xt_u16 nHours = 0;
	xt_u16 nMinutes = 0;
	xt_u16 nSeconds = 0;
	xt_u32 sleepTime = 0;
	
	PWR_RTCAccessCmd(ENABLE);
	
	/*rtcTime.RTC_Hours = 23;
	rtcTime.RTC_Minutes = 59;
	rtcTime.RTC_Seconds = 30;
	rtcTime.RTC_H12 = RTC_H12_PM;
	RTC_SetTime(RTC_Format_BIN,&rtcTime);*/

	RTC_GetTime(RTC_Format_BIN, &rtcTime);
	PrintfXTOS("--- In RTC_Wackup_Config %d:%d:%d \n", rtcTime.RTC_Hours, rtcTime.RTC_Minutes, rtcTime.RTC_Seconds);

	/* EXTI configuration */
	EXTI_ClearITPendingBit(EXTI_Line17);
	EXTI_InitStructure.EXTI_Line = EXTI_Line17;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* NVIC configuration */
	NVIC_InitStructure.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xf;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0xf;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Set the alarm X+5s */
	totalTime += rtcTime.RTC_Hours * 60 * 60;
	totalTime += rtcTime.RTC_Minutes * 60;
	totalTime += rtcTime.RTC_Seconds;

	sleepTime = 1;
	if(MCU_SLEEP_TIME > sleepTime)
		sleepTime = MCU_SLEEP_TIME;
	
	totalTime += sleepTime;
	totalTime = totalTime % (24 * 60 * 60);
	nHours = totalTime / (60 * 60);
	nMinutes = (totalTime - nHours * 60 * 60) / 60;
	nSeconds = (totalTime - nHours * 60 * 60) % 60;
	
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = nHours;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = nMinutes;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds= nSeconds;
	
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 31;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;

	PrintfXTOS("--- In RTC_Wackup_Config %d:%d:%d \n", RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours, RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes, RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds);
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_AlarmStructure);

	/* Enable the alarm */
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	/* Enable the RTC Alarm A interrupt */
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	/* Clear the Alarm A Pending Bit */
	RTC_ClearITPendingBit(RTC_IT_ALRA);  
}

#elif defined(NUVOTON_NUC200)
//todo:
/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8DeviceAddr;
uint8_t g_au8TxData[3];
uint8_t g_u8RxData;
uint8_t g_u8DataLen;
volatile uint8_t g_u8EndFlag = 0;

typedef void (*I2C_FUNC)(uint32_t u32Status);


static I2C_FUNC s_I2C0HandlerFn = NULL;
/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_IRQHandler(void)
{
    uint32_t u32Status;

    u32Status = I2C0->I2CSTATUS;

    if (I2C0->I2CTOC & I2C_I2CTOC_TIF_Msk)
    {
        /* Clear I2C0 Timeout Flag */
        I2C0->I2CTOC |= I2C_I2CTOC_TIF_Msk;                   
    }    
    else
    {
        if (s_I2C0HandlerFn != NULL)
            s_I2C0HandlerFn(u32Status);
    }
}
/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_MasterRx(uint32_t u32Status)
{
    if (u32Status == 0x08)                      /* START has been transmitted and prepare SLA+W */
    {
        I2C0->I2CDAT = g_u8DeviceAddr << 1;     /* Write SLA+W to Register I2CDAT */
        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_SI);
    }   
    else if (u32Status == 0x18)                 /* SLA+W has been transmitted and ACK has been received */
    {
        I2C0->I2CDAT = g_au8TxData[g_u8DataLen++];
        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_SI);
    }
    else if (u32Status == 0x20)                 /* SLA+W has been transmitted and NACK has been received */
    {
        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_STA_STO_SI);
    }
    else if (u32Status == 0x28)                 /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8DataLen != 2)
        {
            I2C0->I2CDAT = g_au8TxData[g_u8DataLen++];
            _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_SI);
        }
        else
        {
            _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_STA_SI);
        }       
    }
    else if (u32Status == 0x10)                 /* Repeat START has been transmitted and prepare SLA+R */
    {
        I2C0->I2CDAT = ((g_u8DeviceAddr << 1) | 0x01);   /* Write SLA+R to Register I2CDAT */
        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_SI);
    }
    else if (u32Status == 0x40)                 /* SLA+R has been transmitted and ACK has been received */
    {
        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_SI);
    }       
    else if (u32Status == 0x58)                 /* DATA has been received and NACK has been returned */
    {
        g_u8RxData = I2C0->I2CDAT;
        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_STO_SI);
        g_u8EndFlag = 1;
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }           
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C0 Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C0_MasterTx(uint32_t u32Status)
{
    if (u32Status == 0x08)                      /* START has been transmitted */
    {
        I2C0->I2CDAT = g_u8DeviceAddr << 1;     /* Write SLA+W to Register I2CDAT */
        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_SI);
    }   
    else if (u32Status == 0x18)                 /* SLA+W has been transmitted and ACK has been received */
    {
        I2C0->I2CDAT = g_au8TxData[g_u8DataLen++];
        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_SI);
    }
    else if (u32Status == 0x20)                 /* SLA+W has been transmitted and NACK has been received */
    {
        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_STA_STO_SI);
    }   
    else if (u32Status == 0x28)                 /* DATA has been transmitted and ACK has been received */
    {
        if (g_u8DataLen != 3)
        {
            I2C0->I2CDAT = g_au8TxData[g_u8DataLen++];
            _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_SI);
        }
        else
        {
            _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_STO_SI);
            g_u8EndFlag = 1;
        }       
    }
    else
    {
        /* TO DO */
        printf("Status 0x%x is NOT processed\n", u32Status);
    }       
}

xt_s32 NUC200PWMCfg(xt_u8 chn, xt_u8 dim)
{
	/* Assume PWM output frequency is 400Hz and duty ratio is 50%, user can calculate PWM settings by follows.
       duty ratio = (CMR+1)/(CNR+1)
       cycle time = CNR+1
       High level = CMR+1
       PWM clock source frequency = __XTAL = 12000000
       (CNR+1) = PWM clock source frequency/prescaler/clock source divider/PWM output frequency 
               = 12000000/2/1/400 = 15000 
       (Note: CNR is 16 bits, so if calculated value is larger than 65536, user should increase prescale value.)
       CNR = 14999
       duty ratio = 50% ==> (CMR+1)/(CNR+1) = 50% ==> CMR = (CNR+1)*0.5-1 = 15000*50/100-1
       CMR = 7499
       Prescale value is 1 : prescaler= 2
       Clock divider is PWM_CSR_DIV1 : clock divider =1             
    */

	/* Disable PWM Timer */
    _PWM_DISABLE_TIMER(PWMB, chn); 
	
	/* Disable PWM Output pin */
	_PWM_DISABLE_PWM_OUT(PWMB, chn);                                                               
    
	/*Set Pwm mode*/
	_PWM_SET_TIMER_AUTO_RELOAD_MODE(PWMB,chn);  
	    
	/*Set PWM Timer clock prescaler*/
	_PWM_SET_TIMER_PRESCALE(PWMB,chn, 1); // Divided by 2  
	                         
	/*Set PWM Timer clock divider select*/
	_PWM_SET_TIMER_CLOCK_DIV(PWMB,chn,PWM_CSR_DIV1); 

	/*Set PWM Timer duty*/
	if (chn == PWM_CH0)
	{
		if(dim > 0)
			PWMB->CMR0 = 15000 / 100 * dim - 1;
		else
			PWMB->CMR0 = 0;

		/*Set PWM Timer period*/
		PWMB->CNR0 = 14999;
	}
	else if (chn == PWM_CH1)
	{
		if(dim > 0)
			PWMB->CMR1 = 15000 / 100 * dim - 1;
		else
			PWMB->CMR1 = 0;

		/*Set PWM Timer period*/
		PWMB->CNR1 = 14999;
	}
	
	/* Enable PWM Output pin */
	_PWM_ENABLE_PWM_OUT(PWMB, chn); 
                                                                                
    /* Enable PWM Timer */
    _PWM_ENABLE_TIMER(PWMB, chn); 
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Set RTC Date/Time is 2012/07/10 08:40:00 on Tuesday                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void SetRTCStartupDateTime(void)
{
    /* Wait RTC Register Access Enable */
    RTC_WaitAccessEnable();

    RTC_SetCurrentCalendar(12, 7, 10);
    RTC_SetCurrentTime(8, 40, 0);
    _RTC_SET_DWR(RTC_DWR_TUESDAY);

    /* Check if RTC Date/Time are ok except [SEC] data */
    if ((_RTC_GET_YEAR() != 12) || (_RTC_GET_MON() != 7)  || (_RTC_GET_DAY() != 10) ||
        (_RTC_GET_HOUR() != 8)  || (_RTC_GET_MIN() != 40) || (_RTC_GET_DWR() != RTC_DWR_TUESDAY))
    {
        printf("Set RTC start-up Date/Time fail. \n\n");
        return ;
    }

    printf("# Set RTC start-up Date/Time is 2012/07/10 08:40:00 on Tuesday. \n");
}

void TMR0_IRQHandler(void)
{
	xt_u8  i;
	
	/* Wait conversion done */
    _ADC_WAIT_COVERSION_DONE();
	
	//读取每个通道的AD值
	for(i=0;i<CHANNLE;i++)
	{
		ad_cvs.buf[i][ad_cvs.sample]=_ADC_GET_CONVERSION_DATA(i) & 0xfff;
	}
	ad_cvs.sample++;
	if(ad_cvs.sample>=SAMPLE_NUM)
	{
		XtBspAdcTimerStop();
	}
	
    /* Clear TIMER0 Timeout Interrupt Flag */
    _TIMER_CLEAR_CMP_INT_FLAG(TIMER0);      

	//PrintfXTOS("tim0 int...\n");
}

void I2C0_Init(void)
{
    /* Reset I2C0 */
    SYS->IPRSTC2 |=  SYS_IPRSTC2_I2C0_RST_Msk;
    SYS->IPRSTC2 &= ~SYS_IPRSTC2_I2C0_RST_Msk;

    /* Enable I2C0 Controller */
    I2C0->I2CON |= I2C_I2CON_ENS1_Msk;
    
    /* I2C0 clock divider, I2C Bus Clock = PCLK / (4*125) = 100kHz */
    I2C0->I2CLK = I2C_I2CLK_DIV4(125);

    /* Get I2C0 Bus Clock */
    //printf("I2C clock %d Hz\n", _I2C_GET_BUS_CLOCK(I2C0));

    /* Set I2C0 4 Slave Addresses */            
    _I2C_SET_SLAVE_ADDRESS_0(I2C0, 0x16);   /* Slave Address : 0x15 */
    _I2C_SET_SLAVE_ADDRESS_1(I2C0, 0x36);   /* Slave Address : 0x35 */
    _I2C_SET_SLAVE_ADDRESS_2(I2C0, 0x56);   /* Slave Address : 0x55 */
    _I2C_SET_SLAVE_ADDRESS_3(I2C0, 0x76);   /* Slave Address : 0x75 */

    /* Enable I2C0 interrupt and set corresponding NVIC bit */
    I2C0->I2CON |= I2C_I2CON_EI_Msk;
    NVIC_EnableIRQ(I2C0_IRQn);
}

void I2C0_Close(void)
{
    /* Disable I2C0 interrupt and clear corresponding NVIC bit */
    I2C0->I2CON &= ~I2C_I2CON_EI_Msk;
    NVIC_DisableIRQ(I2C0_IRQn);
    
    /* Disable I2C0 and close I2C1 clock */
    I2C0->I2CON &= ~I2C_I2CON_ENS1_Msk;
    SYSCLK->APBCLK &= ~SYSCLK_APBCLK_I2C0_EN_Msk;

}

#else
xt_s32 Win32DebugInit()
{
	//todo: config debug com number using debug.ini
	hComDebug = CreateFile("COM5", GENERIC_READ|GENERIC_WRITE, 0,NULL , OPEN_EXISTING, 0, NULL);
	if (hComDebug==(HANDLE)-1)
	{
		DWORD nErrorNo = GetLastError();
		return FAILURE_XT;
	}
	else
	{
		DCB wdcb;
		GetCommState (hComDebug, &wdcb);
		wdcb.BaudRate=115200;
		wdcb.ByteSize=8;
		wdcb.Parity=NOPARITY;
		wdcb.StopBits=0;
		SetCommState (hComDebug, &wdcb);
		SetupComm(hComDebug,1024,1024);
		PurgeComm(hComDebug, PURGE_TXCLEAR);

		//redirect stdout to com debug
		fdComDebug = _open_osfhandle((intptr_t)hComDebug,0x4000);
		pfComDebug = _fdopen( fdComDebug, "w");
		*stdout = *pfComDebug;
		setvbuf(stdout,NULL,_IONBF,0);
	}

	return SUCCESS_XT;
}

xt_s32 Win32CtrlInit()
{
	//todo: config debug com number using debug.ini
	hComCtrl = CreateFile("COM3", GENERIC_READ|GENERIC_WRITE, 0,NULL , OPEN_EXISTING, 0, NULL);
	if (hComCtrl==(HANDLE)-1)
	{
		return FAILURE_XT;
	}
	else
	{
		DCB wdcb;
		COMMTIMEOUTS to;

		memset(&to,0,sizeof(to));
		//to.ReadIntervalTimeout = MAXDWORD;
		to.ReadTotalTimeoutConstant = 10;
		to.WriteTotalTimeoutConstant = 10;
		SetCommTimeouts(hComCtrl,&to);

		GetCommState (hComCtrl, &wdcb);
		wdcb.BaudRate=9600;
		wdcb.fBinary=TRUE;
		wdcb.ByteSize=8;
		wdcb.Parity=EVENPARITY;
		wdcb.StopBits=0;//todo: check in feature
		SetCommState (hComCtrl, &wdcb);
		SetupComm(hComCtrl,1024,1024);
		PurgeComm(hComCtrl, PURGE_TXCLEAR);
	}

	return SUCCESS_XT;
}

xt_s32 Win32CtrlRespInit()
{
	//todo: config debug com number using debug.ini

	if (hComCtrlResp!=(HANDLE)-1)
	{
		return SUCCESS_XT;
	}

	hComCtrlResp = CreateFile("COM4", GENERIC_READ|GENERIC_WRITE, 0,NULL , OPEN_EXISTING, 0, NULL);
	if (hComCtrlResp==(HANDLE)-1)
	{
		return FAILURE_XT;
	}
	else
	{
		DCB wdcb;
		COMMTIMEOUTS to;

		memset(&to,0,sizeof(to));
		//to.ReadIntervalTimeout = MAXDWORD;
		to.ReadTotalTimeoutConstant = 10;
		to.WriteTotalTimeoutConstant = 10;
		SetCommTimeouts(hComCtrlResp,&to);

		GetCommState (hComCtrlResp, &wdcb);
		wdcb.BaudRate=9600;
		wdcb.fBinary=TRUE;
		wdcb.ByteSize=8;
		wdcb.Parity=EVENPARITY;
		wdcb.StopBits=0;//todo: check in feature
		SetCommState (hComCtrlResp, &wdcb);
		SetupComm(hComCtrlResp,1024,1024);
		PurgeComm(hComCtrlResp, PURGE_TXCLEAR);
	}

	return SUCCESS_XT;
}

xt_s32 Win32CtrlRespDeinit()
{
	//todo: config debug com number using debug.ini
	if (hComCtrlResp!=(HANDLE)-1)
	{
		CloseHandle(hComCtrlResp);
		hComCtrlResp = -1;
	}

	return SUCCESS_XT;
}

#endif



xt_s32 XtBspInit()
{
	xt_u8 StanbyWakeUp = 0;
	xt_u16 Int_CurrentSTBY = 0;
	float Current_STBY;
	xt_u32 i = 0;
		
	DEBUG_USART_Configuration();
	ODB_USART_Configuration(115200,USART_Parity_No);

	/* Check if the system has resumed from IWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_IWDGRST) != RESET)
	{
		/* IWDGRST flag set */
		printf("Stm32 reset from iwdg !!!\n");
		/* Clear reset flags */
		RCC_ClearFlag();
	}

	/* Check if the system has resumed from IWDG reset */
	if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
	{
		/* IWDGRST flag set */
		printf("Stm32 reset from wwdg !!!\n");
		/* Clear reset flags */
		RCC_ClearFlag();
	}
	
	
	Int_CurrentSTBY = Current_Measurement();

	//cfg RUN LED
	STM_EVAL_LEDInit(LED_RUN);

	//wackup pin PA0
	Init_GPIOs();
	//cfg GPIO DO
	STM_EVAL_GPIOInit(GPIO_DO_DLZ);
	STM_EVAL_GPIOInit(GPIO_DO_3GPOWEREN);
	STM_EVAL_GPIOInit(GPIO_DO_3GRESET);
	STM_EVAL_GPIOInit(GPIO_DO_3GONOFF);
	STM_EVAL_GPIOInit(GPIO_DO_OTHER_POWEREN);
	
	XtBspDOCtrl(GPIO_DO_DLZ, XT_BSP_DO_OP_CLOSE);
	//
	//init GPIO DO
	//XtBspDOCtrl(XT_BSP_DO_DLZ, XT_BSP_DO_OP_OPEN);
	XtBspDOCtrl(GPIO_DO_3GPOWEREN, XT_BSP_POWER_OPEN);
	//XtBspDOCtrl(GPIO_DO_OTHER_POWEREN,XT_BSP_POWER_CLOSE);//, XT_BSP_POWER_OPEN);
	XtBspDOCtrl(GPIO_DO_3GRESET, XT_BSP_POWER_OPEN);
	delay_us(200000);
	XtBspDOCtrl(GPIO_DO_3GRESET, XT_BSP_POWER_CLOSE);

	

	//init GPIO for PWM
	TIM_Config();

	//init GPIO PWM
	XtBspPWMCtrl(XT_BSP_PWM_0, 0x01);
	

	//ini GPIO temprature sensor
	TH_GPIOInit();
	//ini GPIO VOC sensor
	VOC_GPIOInit();
       //ini GPIO display LED
	DLED_GPIOInit();
	FLED_GPIOInit();
	//默认绿色
	DLED_close();
	FLED_green();
	
	//ini GPIO key
	KEY_GPIOInit();
	  
	
	XtBspAdcInit();

	//M24LR04E_Init();
	//AT24LCxxx_Init();
	
	Init_RTC();

	/* Check if the StandBy flag is set */
	if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
	{
		/* System resumed from STANDBY mode */
		/* Clear StandBy flag */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
		PWR_ClearFlag(PWR_FLAG_SB); 
		/* set StandbyWakeup indicator*/
		StanbyWakeUp = TRUE;
	} else
	{
		/* Reset StandbyWakeup indicator*/
		StanbyWakeUp = FALSE;    
	}
	
	if(StanbyWakeUp == TRUE)
	{
	  uint16_t tab[6];
	  
	  /* Wake UP: Return of RESET by Current STAND BY measurement */
      //LCD_GLASS_ScrollSentence("     STANDBY WAKEUP",1,SCROLL_SPEED);
      /* Substract bias current from operational amplifier*/
      if ( Int_CurrentSTBY > Bias_Current )
        Int_CurrentSTBY -= Bias_Current;
      Current_STBY = Int_CurrentSTBY * Vdd_appli()/ADC_CONV;  
      Current_STBY *= 20L;
	  
		convert_into_char(Current_STBY, tab);
		tab[5] = 'A';
		tab[4] = '?';
			
		/* Test the significant digit for displays 3 or 4 digits*/
		if ( tab[0] != '0')
		{
		tab[1] |= DOT; /* To add decimal point */
		}  else  {
		/* To shift for suppress '0' before decimal */
		tab[0] = tab[1] ;	
		tab[0] |= DOT ;
		tab[1] = tab[2] ;
		tab[2] = tab[3] ;		
		tab[3] = ' ';
		}

	  PrintfXTOS("*** In XtBspInit %s uA ...\n",tab);
      //display_MuAmp((uint32_t)Current_STBY);
      /* Wait for user button press to continue */
	  
	}

	if (ctrlUartMutex == NULL)
		MutexCreateXTOS(ctrlUartMutex);
	if (odbUartMutex == NULL)
		MutexCreateXTOS(odbUartMutex);
	

	return SUCCESS_XT;
}


#define BUFFER_SIZE (256+32)
//--------odb uart------------

//xt_u8 odbComBuf[BUFFER_SIZE] = {0};
//xt_u8 odbComRespBuf[BUFFER_SIZE] = {0};
xt_void XtBspOdbUartRxIsr(xt_u8 data)
{
#ifndef WIN32
	if(rq_odb== NULL)
		rq_odb= RQueueNew(MemMallocXTOS(BUFFER_SIZE), BUFFER_SIZE);
	RQueueWriteInByte(rq_odb, data);
#endif
}



xt_s32 XtBspOdbUartRead(xt_u8 *pData)
{
#ifndef WIN32
	if(rq_odb== NULL)
		rq_odb= RQueueNew(MemMallocXTOS(BUFFER_SIZE), BUFFER_SIZE);
	if(FALSE==RQueueReadOutByte(rq_odb, pData))
		return FAILURE_XT;
	return SUCCESS_XT;
#endif
}

xt_s32 XtBspOdbRead(xt_u8* pData,xt_s32* pDataLen)
{
	xt_u32 i = 0,j = 0;
	xt_s32 ret = 0;

	MutexLockXTOS(odbUartMutex);
	for(i=0;i<BUFFER_SIZE;i++)
	{
		for(j=0;j<UART_POLL_TIMES;j++)
		{
			if( XtBspOdbUartRead(&pData[ret])== SUCCESS_XT)
			{
				ret++;
				break;
			}
			TaskSleepMs(UART_POLL_DELAY_MS);
		}
		if(j == UART_POLL_TIMES)
			break;	
	}
	* pDataLen = ret;
	MutexUnlockXTOS(odbUartMutex);
	return ret;
}

//-----ctrl uart------------------

//#define BUFFER_SIZE (256+32)
xt_u8 bspComBuf[BUFFER_SIZE] = {0};
xt_u8 bspComRespBuf[BUFFER_SIZE] = {0};
xt_void XtBspCtrlUartRxIsr(xt_u8 data)
{
#ifndef WIN32
	if(rq == NULL)
		rq = RQueueNew(MemMallocXTOS(BUFFER_SIZE), BUFFER_SIZE);
	RQueueWriteInByte(rq, data);
#endif
}



xt_s32 XtBspCtrlUartRead(xt_u8 *pData)
{
#ifndef WIN32
	if(rq == NULL)
		rq = RQueueNew(MemMallocXTOS(BUFFER_SIZE), BUFFER_SIZE);
	if(FALSE==RQueueReadOutByte(rq, pData))
		return FAILURE_XT;
	return SUCCESS_XT;
#endif
}

xt_s32 XtBspCtrlRead(xt_u8* pData,xt_s32* pDataLen)
{
	xt_u32 i = 0,j = 0;
	xt_s32 ret = 0;

	MutexLockXTOS(ctrlUartMutex);
#ifndef WIN32
	for(i=0;i<BUFFER_SIZE;i++)
	{
		for(j=0;j<UART_POLL_TIMES;j++)
		{
			if( XtBspCtrlUartRead(&pData[ret])== SUCCESS_XT)
			{
				ret++;
				break;
			}
			TaskSleepMs(UART_POLL_DELAY_MS);
		}
		if(j == UART_POLL_TIMES)
			break;	
	}
	* pDataLen = ret;
#else
	if (hComCtrl!=(HANDLE)-1)
	{
		DWORD rCount = 0;
		TaskSleepMs(10);
		ReadFile(hComCtrl, bspComBuf, BUFFER_SIZE, &rCount, NULL);
		if(rCount > 0)
		{
			memcpy(pData,bspComBuf,rCount);
			* pDataLen = rCount;
		}
		ret = rCount;
	}
#endif
	MutexUnlockXTOS(ctrlUartMutex);
	return ret;
}


xt_s32 XtBspCtrlWrite(xt_u8* pData,xt_s32 dataLen)
{
	xt_s32 i = 0;
	xt_s32 ret = 0;

	MutexLockXTOS(ctrlUartMutex);
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)
	for(i = 0; i < dataLen; i++)
	{
		while (USART_GetFlagStatus(EVAL_COM2, USART_FLAG_TC) == RESET)
		{}

		USART_SendData(EVAL_COM2, *(pData + i));		
	}
	ret = dataLen;
#elif defined(NUVOTON_NUC200)
	for(i = 0; i < dataLen; i++)
	{
		//while (USART_GetFlagStatus(EVAL_COM2, USART_FLAG_TC) == RESET)
		//{}

		_UART_SENDBYTE(UART1, *(pData + i));
	}
#else
	if (hComCtrl!=(HANDLE)-1)
	{
		DWORD wCount = 0;
		WriteFile(hComCtrl, pData, dataLen, &wCount, NULL);
		ret =  wCount;
	}
#endif
	MutexUnlockXTOS(ctrlUartMutex);

	return ret;
}





xt_s32 XtBspCtrlRespRead(xt_u8* pData,xt_s32* pDataLen)
{
	xt_u8 i = 0,j = 0;
	xt_s32 ret = 0;

#ifdef WIN32
	if (hComCtrlResp!=(HANDLE)-1)
	{
		DWORD rCount = 0;
		//TaskSleepMs(10);
		ReadFile(hComCtrlResp, bspComRespBuf, BUFFER_SIZE, &rCount, NULL);
		if(rCount > 0)
		{
			memcpy(pData,bspComRespBuf,rCount);
			* pDataLen = rCount;
		}
		ret = rCount;
	}
#endif
	return ret;
}

xt_s32 XtBspCtrlRespWrite(xt_u8* pData,xt_s32 dataLen)
{
	xt_s32 i = 0;
	xt_s32 ret = 0;

#ifdef WIN32
	if (hComCtrlResp!=(HANDLE)-1)
	{
		DWORD wCount = 0;
		WriteFile(hComCtrlResp, pData, dataLen, &wCount, NULL);
		ret =  wCount;
	}
#endif
	return ret;
}

xt_s32 XtBspCommInit(xt_u32 buadrate,xt_u16 parity)
{
	xt_s32 ret = 0;
	xt_u32 i = 0;
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)
	CTRL_USART_Configuration(buadrate,parity);
#elif defined(NUVOTON_NUC200)
	UART1_Init(buadrate);

	_UART_ENABLE_INT(UART1, (UART_IER_RDA_IEN_Msk));
	NVIC_EnableIRQ(UART1_IRQn);
#else
	Win32CtrlInit();
#endif
 
	return ret;
}

xt_s32 XtBspToggleRunLed()
{
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)
	STM_EVAL_LEDToggle(LED_RUN);
#elif defined(NUVOTON_NUC200)
//todo:
#else
	static xt_s32 cnt = 0;
	PrintfXTOS("--- In XtBspToggleRunLed %d ...\n",cnt++);
#endif
 
	return SUCCESS_XT;
}

//@return: open result 1:correct 0:error
xt_s32 XtBspOpenRunLed()
{
	xt_s32 ret = 0;
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)
	STM_EVAL_LEDOn(LED_RUN);
#elif defined(NUVOTON_NUC200)
	PB8 = 0;
#else
	static xt_s32 cnt = 0;
	win32ParamSt.ledSt = 1;
	ret = win32ParamSt.ledConnSt;
	PrintfXTOS("--- In XtBspOpenRunLed %d ...\n",cnt++);
#endif
 
	return ret;
}

xt_s32 XtBspCloseRunLed()
{
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)
	STM_EVAL_LEDOff(LED_RUN);
#elif defined(NUVOTON_NUC200)
	PB8 = 1;
#else
	static xt_s32 cnt = 0;
	win32ParamSt.ledSt =0;
	PrintfXTOS("--- In XtBspCloseRunLed %d ...\n",cnt++);
#endif
 
	return SUCCESS_XT;
}

xt_s32 XtBspDOCtrl(xt_u8 doChn, xt_u8 doOp)
{
	xt_s32 ret = 0;
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)
	if (doChn >= XT_BSP_DO_DLZ && doChn < XT_BSP_DO_MAX
		&& doOp >= XT_BSP_DO_OP_OPEN && doOp < XT_BSP_DO_OP_MAX)
	{

		if(doOp == XT_BSP_DO_OP_OPEN)
			STM_EVAL_GPIOOn(doChn);
		else if(doOp == XT_BSP_DO_OP_CLOSE)
			STM_EVAL_GPIOOff(doChn);
	
	}
#elif defined(NUVOTON_NUC200)
	if (doChn >= XT_BSP_DO_0 && doChn < XT_BSP_DO_MAX
		&& doOp >= XT_BSP_DO_OP_OPEN && doOp < XT_BSP_DO_OP_MAX)
	{
		if(doChn == XT_BSP_DO_0)
		{
			PB9 = doOp;
		}
		else if(doChn == XT_BSP_DO_1)
		{
			PB10 = doOp;
		}
	}
#else
	static xt_s32 cnt = 0;
	//win32ParamSt.ledSt = 1;
	//ret = win32ParamSt.ledConnSt;
	PrintfXTOS("--- In XtBspDOCtrl %d ...\n",cnt++);
#endif
 
	return ret;
}

xt_s32 XtBspToggleDoCtrl(xt_u8 doChn)
{
	STM_EVAL_GPIOToggle( doChn);
}

xt_s32 XtBspPWMCtrl(xt_u8 pwmChn, xt_u8 dim)
{
	xt_s32 ret = 0;
	if (pwmChn >= XT_BSP_PWM_0 && pwmChn < XT_BSP_PWM_MAX
		&& dim >= XT_BSP_PWM_DIM_MIN && dim <= XT_BSP_PWM_DIM_MAX)
	{
		if(pwmChn == XT_BSP_PWM_0)
		{
			LXXPWMCfg(0, 100-dim);
		}
		//else if(pwmChn == XT_BSP_PWM_1)
		//{
		//	LXXPWMCfg(1, dim);
		//}
	}
	//static xt_s32 cnt = 0;
	//win32ParamSt.ledSt = 1;
	//ret = win32ParamSt.ledConnSt;
	//PrintfXTOS("--- In XtBspDOCtrl %d ...\n",cnt++);
 
	return ret;
}

xt_s32 XtBspAdcInit()
{
	xt_s32 ret = 0;
	STM_EVAL_ADCConfig();
	return ret;
}

void delay4ms(void)
{
	xt_u32 i;
	for(i=0;i<3200;i++);
}

xt_s32 XtBspAdcTimerStart()
{
	xt_s32 ret = 0;
	LXXPWMCfg_forPM2_5(4);
	delay4ms();
	STM_EVAL_Tim2Config();
 	
	return ret;
}

xt_s32 XtBspAdcTimerStop()
{
	xt_s32 ret = 0;
	TIM_ITConfig(TIM2, TIM_IT_Update, DISABLE);
	TIM_Cmd(TIM2, DISABLE);
 
	return ret;
}

#define YEAR0                   1900
#define EPOCH_YR                1970
#define YEARSIZE(year)          (LEAPYEAR(year) ? 366 : 365)
#define LEAPYEAR(year)          (!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define SECS_DAY                (24L * 60L * 60L)

const int _ytab[2][12] = 
{
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};
#define TIME_MAX                2147483647L
long _timezone = 0;                 // Difference in seconds between GMT and local time
long _dstbias = 0;                  // Offset for Daylight Saving Time

xt_tm_t *gmtime_rt(const xt_time_t *timer, xt_tm_t *tmbuf)
{
  xt_time_t time = *timer;
  unsigned long dayclock, dayno;
  int year = EPOCH_YR;

  dayclock = (unsigned long) time % SECS_DAY;
  dayno = (unsigned long) time / SECS_DAY;

  tmbuf->tm_sec = dayclock % 60;
  tmbuf->tm_min = (dayclock % 3600) / 60;
  tmbuf->tm_hour = dayclock / 3600;
  tmbuf->tm_wday = (dayno + 4) % 7; // Day 0 was a thursday
  while (dayno >= (unsigned long) YEARSIZE(year)) 
  {
    dayno -= YEARSIZE(year);
    year++;
  }
  tmbuf->tm_year = year - YEAR0;
  tmbuf->tm_yday = dayno;
  tmbuf->tm_mon = 0;
  while (dayno >= (unsigned long) _ytab[LEAPYEAR(year)][tmbuf->tm_mon]) 
  {
    dayno -= _ytab[LEAPYEAR(year)][tmbuf->tm_mon];
    tmbuf->tm_mon++;
  }
  tmbuf->tm_mday = dayno + 1;
  tmbuf->tm_isdst = 0;
  //tmbuf->tm_gmtoff = 0;
  //tmbuf->tm_zone = "UTC";
  return tmbuf;
}

xt_time_t mktime_xrt(xt_tm_t *tmbuf)
{
	long day, year;
	int tm_year;
	int yday, month;
	/*unsigned*/ long seconds;
	int overflow;
	long dst;

	tmbuf->tm_min += tmbuf->tm_sec / 60;
	tmbuf->tm_sec %= 60;
	if (tmbuf->tm_sec < 0) 
	{
		tmbuf->tm_sec += 60;
		tmbuf->tm_min--;
	}
	tmbuf->tm_hour += tmbuf->tm_min / 60;
	tmbuf->tm_min = tmbuf->tm_min % 60;
	if (tmbuf->tm_min < 0) 
	{
		tmbuf->tm_min += 60;
		tmbuf->tm_hour--;
	}
	day = tmbuf->tm_hour / 24;
	tmbuf->tm_hour= tmbuf->tm_hour % 24;
	if (tmbuf->tm_hour < 0) 
	{
		tmbuf->tm_hour += 24;
		day--;
	}
	tmbuf->tm_year += tmbuf->tm_mon / 12;
	tmbuf->tm_mon %= 12;
	if (tmbuf->tm_mon < 0) 
	{
		tmbuf->tm_mon += 12;
		tmbuf->tm_year--;
	}
	day += (tmbuf->tm_mday - 1);
	while (day < 0) 
	{
		if(--tmbuf->tm_mon < 0) 
		{
			tmbuf->tm_year--;
	 		tmbuf->tm_mon = 11;
		}
		day += _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
	}
	while (day >= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon]) 
	{
		day -= _ytab[LEAPYEAR(YEAR0 + tmbuf->tm_year)][tmbuf->tm_mon];
		if (++(tmbuf->tm_mon) == 12) 
		{
			tmbuf->tm_mon = 0;
			tmbuf->tm_year++;
		}
	}
	tmbuf->tm_mday = day + 1;
	year = EPOCH_YR;
	if (tmbuf->tm_year < year - YEAR0) return (xt_time_t) -1;
	seconds = 0;
	day = 0;						// Means days since day 0 now
	overflow = 0;

	// Assume that when day becomes negative, there will certainly
	// be overflow on seconds.
	// The check for overflow needs not to be done for leapyears
	// divisible by 400.
	// The code only works when year (1970) is not a leapyear.
	tm_year = tmbuf->tm_year + YEAR0;

	if (TIME_MAX / 365 < tm_year - year) overflow++;
	day = (tm_year - year) * 365;
	if (TIME_MAX - day < (tm_year - year) / 4 + 1) overflow++;
	day += (tm_year - year) / 4 + ((tm_year % 4) && tm_year % 4 < year % 4);
	day -= (tm_year - year) / 100 + ((tm_year % 100) && tm_year % 100 < year % 100);
	day += (tm_year - year) / 400 + ((tm_year % 400) && tm_year % 400 < year % 400);

	yday = month = 0;
	while (month < tmbuf->tm_mon)
	{
		yday += _ytab[LEAPYEAR(tm_year)][month];
		month++;
	}
	yday += (tmbuf->tm_mday - 1);
	if (day + yday < 0) overflow++;
		day += yday;

	tmbuf->tm_yday = yday;
	tmbuf->tm_wday = (day + 4) % 7;				// Day 0 was thursday (4)

	seconds = ((tmbuf->tm_hour * 60L) + tmbuf->tm_min) * 60L + tmbuf->tm_sec;

	if ((TIME_MAX - seconds) / SECS_DAY < day) overflow++;
		seconds += day * SECS_DAY;

	// Now adjust according to timezone and daylight saving time
	if (((_timezone > 0) && (TIME_MAX - _timezone < seconds))
	  || ((_timezone < 0) && (seconds < -_timezone)))
		  overflow++;
	seconds += _timezone;

	if (tmbuf->tm_isdst)
		dst = _dstbias;
	else 
		dst = 0;

	if (dst > seconds) overflow++;		// dst is always non-negative
		seconds -= dst;

	if (overflow) return (xt_time_t) -1;

	if ((xt_time_t) seconds != seconds) return (xt_time_t) -1;
		return (xt_time_t) seconds;
}

xt_u32 XtBspGetTime()
{
	xt_tm_t tm;
	RTC_TimeTypeDef rtcTime = {0};
	RTC_DateTypeDef rtcDate = {0};
	RTC_GetTime(RTC_Format_BIN, &rtcTime);
	RTC_GetDate(RTC_Format_BIN, &rtcDate);
	
//	PrintfXTOS("--- In XtBspGetTime %d-%d-%d,%d,%d:%d:%d\n",rtcDate.RTC_Year,
//		rtcDate.RTC_Month,rtcDate.RTC_Date,rtcDate.RTC_WeekDay,rtcTime.RTC_Hours, rtcTime.RTC_Minutes, rtcTime.RTC_Seconds);
	tm.tm_year = rtcDate.RTC_Year+100;
	tm.tm_wday = rtcDate.RTC_WeekDay - 1;
	tm.tm_mon = rtcDate.RTC_Month - 1;
	tm.tm_mday = rtcDate.RTC_Date;
	tm.tm_hour = rtcTime.RTC_Hours;
	tm.tm_min = rtcTime.RTC_Minutes;
	tm.tm_sec = rtcTime.RTC_Seconds;
	return mktime_xrt(&tm);
}

xt_s32 XtBspGetTmTime(xt_time_t t,xt_tm_t *pTm)
{
#if 0
	xt_tm_t tm;
	RTC_TimeTypeDef rtcTime = {0};
	RTC_DateTypeDef rtcDate = {0};
	if(RTC_GetTime(RTC_Format_BIN, &rtcTime) == 0)
		return FAILURE_XT;
	if(RTC_GetDate(RTC_Format_BIN, &rtcDate) == 0)
		return FAILURE_XT;
	
	PrintfXTOS("--- In XtBspGetTmTime %d-%d-%d,%d,%d:%d:%d\n",rtcDate.RTC_Year,
		rtcDate.RTC_Month,rtcDate.RTC_Date,rtcDate.RTC_WeekDay,rtcTime.RTC_Hours, rtcTime.RTC_Minutes, rtcTime.RTC_Seconds);
	tm.tm_year = rtcDate.RTC_Year;
	tm.tm_wday = rtcDate.RTC_WeekDay;
	tm.tm_mon = rtcDate.RTC_Month;
	tm.tm_mday = rtcDate.RTC_Date;
	tm.tm_hour = rtcTime.RTC_Hours;
	tm.tm_min = rtcTime.RTC_Minutes;
	tm.tm_sec = rtcTime.RTC_Seconds;
#else
	gmtime_rt(&t,pTm);
#endif
	return SUCCESS_XT;
}

xt_s32 XtBspSettime(xt_tm_t tm)
{
	RTC_TimeTypeDef rtcTime = {0};
	RTC_DateTypeDef rtcDate = {0};

	rtcDate.RTC_Year = tm.tm_year;
	rtcDate.RTC_WeekDay = tm.tm_wday;
	rtcDate.RTC_Month = tm.tm_mon;
	rtcDate.RTC_Date = tm.tm_mday;
	rtcTime.RTC_Hours = tm.tm_hour;
	rtcTime.RTC_Minutes = tm.tm_min;
	rtcTime.RTC_Seconds = tm.tm_sec;
	PWR_RTCAccessCmd(ENABLE);
	if(RTC_SetTime(RTC_Format_BIN, &rtcTime) == 0)
		return FAILURE_XT;
	if(RTC_SetDate(RTC_Format_BIN, &rtcDate) == 0)
		return FAILURE_XT;
	return SUCCESS_XT;
}

#if 0
xt_s32 XtBspGetTime()
{
	xt_s32 ret = 0;
	xt_u32 i = 0;
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)
	uint8_t OneByte = 0;
	uint16_t ReadAddr = 0x0000;	
	
	RTC_TimeTypeDef rtcTime = {0};
	RTC_GetTime(RTC_Format_BIN, &rtcTime);
	PrintfXTOS("--- In XtBspGetTime %d:%d:%d \n", rtcTime.RTC_Hours, rtcTime.RTC_Minutes, rtcTime.RTC_Seconds);

#if 1
	//M24LR04E_WriteOneByte (M24LR16_EEPROM_ADDRESS_USER_WRITE, ReadAddr, rtcTime.RTC_Seconds);
	M24LR04E_ReadOneByte (M24LR16_EEPROM_ADDRESS_USER_READ, ReadAddr, &OneByte);
	PrintfXTOS("--- In TEST I2C1 %d\n", OneByte);
#endif

	Icc_RUN();

#elif defined(NUVOTON_NUC200)
	PrintfXTOS("    %d/%02d/%02d %02d:%02d:%02d \n",
        (2000+_RTC_GET_YEAR()), _RTC_GET_MON(), _RTC_GET_DAY(), _RTC_GET_HOUR(), _RTC_GET_MIN(), _RTC_GET_SEC());

//todo: test I2C0
#if 0
    g_u8DeviceAddr = 0xa6>>1;
        
    for (i = 0; i < 5; i++)
    {
        g_au8TxData[0] = (uint8_t)((i & 0xFF00) >> 8);
        g_au8TxData[1] = (uint8_t)(i & 0x00FF);
        g_au8TxData[2] = (uint8_t)(g_au8TxData[1] + 3);
        
        g_u8DataLen = 0;
        g_u8EndFlag = 0;
        /* I2C function to write data to slave */
        s_I2C0HandlerFn = (I2C_FUNC)I2C0_MasterTx;
        
        /* I2C as master sends START signal */
        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_STA);

        /* Wait I2C Tx Finish */
        while (g_u8EndFlag == 0);
        g_u8EndFlag = 0;
        /* I2C function to read data from slave */
        s_I2C0HandlerFn = (I2C_FUNC)I2C0_MasterRx;

        g_u8DataLen = 0;
        g_u8DeviceAddr = 0xa7>>1;

        _I2C_SET_CONTROL_BITS(I2C0, I2C_I2CON_STA);
        
        /* Wait I2C Rx Finish */
        while (g_u8EndFlag == 0);

        /* Compare data */
        if (g_u8RxData != g_au8TxData[2])
        {
            PrintfXTOS("I2C Byte Write/Read Failed, Data 0x%x\n", g_u8RxData);
            //return -1;
        }           
    }
    PrintfXTOS("I2C0 Access EEPROM Test OK\n");

    s_I2C0HandlerFn = NULL;
#endif

#else
#endif
 
	return ret;
}
#endif

xt_s32 XtBspWdgInit()
{
	xt_s32 ret = 0;
	xt_u32 i = 0;
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)
#if defined (USE_IWDG_AS_WDG)
	LSI_RCC_Configuration();
	IWDG_Configuration();
#elif defined (USE_WWDG_AS_WDG)
	WWDG_Configuration();
#endif
#elif defined(NUVOTON_NUC200)
	SYS_Init();
	_WWDT_ENABLE(0x3F, WWDT_WWDTCR_PERIODSEL_1024);
#else
#endif
 
	return ret;
}

xt_s32 XtBspWdgFeed()
{
	xt_s32 ret = 0;
	xt_u32 i = 0;
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)
#if defined (USE_IWDG_AS_WDG)
	IWDG_ReloadCounter();
#elif defined (USE_WWDG_AS_WDG)
	WWDG_SetCounter(0x7f);
#endif
#elif defined(NUVOTON_NUC200)
	_WWDT_RELOAD_COUNTER();
#else
#endif
 
	return ret;
}

xt_s32 XtBspCheckPower()
{
	xt_s32 ret = 0;
	
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)

	if((USERBUTTON_GPIO_PORT->IDR & USERBUTTON_GPIO_PIN) == 1 )
    {
		//PrintfXTOS("exti0 d = %d...\n",1);
		powerCnt = 0;
    }
	else
	{
		//PrintfXTOS("exti0 d = %d...\n",0);
		powerCnt ++;
	}

	if(powerCnt >= MCU_POWERDOWN_CNT)
		return FALSE;
	else
		return TRUE;

#elif defined(NUVOTON_NUC200)
#else
#endif
 
	return ret;
}

xt_s32 XtBspPowerManager(uint8_t Mcu_State)
{
	xt_s32 ret = 0;
#if defined (STM32L1XX_MD) || defined (STM32L1XX_MDP)
	RCC_TypeDef SavRCC;
	uint16_t adc_measure;
	float Current;
	uint16_t tab[6];
	xt_u32 strVTOR = 0;

	RTC_Wackup_Config();
	
	PrintfXTOS("*** enter in XtBspPowerManager... \n");
	strVTOR = SCB->VTOR;
	
	/* Disable the RTC Wakeup Interrupt */
	RTC_ITConfig(RTC_IT_WUT, DISABLE);
	/* Disable LCD */
	LCD_Cmd(DISABLE);
	/* wait until LCD disable */
	while (LCD_GetFlagStatus(LCD_FLAG_ENS) == SET);
	/* Set IO in lowpower configuration*/
	GPIO_LowPower_Config(); 
	/*Disable fast wakeUp*/
	PWR_FastWakeUpCmd(DISABLE);

	switch (Mcu_State)
	{        
		/* STOP modes
		Measurement done to MSI 32 Khz without or with RTC
		*/		
		case MCU_STOP_NoRTC:
		case MCU_STOP_RTC:

		/* Disable PVD */
		PWR_PVDCmd(DISABLE);

		/* Enable Ultra low power mode */
		PWR_UltraLowPowerCmd(ENABLE);           

		/* To save the RCC configuration registers */
		Config_RCC(&SavRCC);  

		/* To stop the sys tick for avoid IT */
		SysTick->CTRL = 0; 
		   
		/* Swith in MSI 32KHz */
		if( Mcu_State == MCU_STOP_NoRTC )
			SetHSICLKToMSI(RCC_MSIRange_0,DIV2,NoRTC) ;
		else
			SetHSICLKToMSI(RCC_MSIRange_0,DIV2,WITHRTC) ;          

		/* Falling edge for start counter */		
		//GPIO_LOW(CTN_GPIO_PORT,CTN_CNTEN_GPIO_PIN);

		//STM_EVAL_COM_NVIC_Config(COM2, DISABLE);
		//STM_EVAL_COMDeInit(COM2);
		//USART_ITConfig(EVAL_COM2, USART_IT_RXNE, DISABLE);

		//PWR_WakeUpPinCmd(PWR_WakeUpPin_1,ENABLE);
		
		//STM_EVAL_COM_NVIC_Config(COM3, DISABLE);
		//STM_EVAL_COMDeInit(COM3);
		//USART_ITConfig(EVAL_COM3, USART_IT_RXNE, DISABLE);

		ReInit_GPIOs();
		
		/* Request Wait For Interrupt */    
		PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFI);              

		break;        

		/* Standby mode without RTC
		Measurement done to MSI 32 Khz without RTC
		*/
		case MCU_STBY:

		/* Disable PVD */
		PWR_PVDCmd(DISABLE);

		/* Enable Ultra low power mode */
		PWR_UltraLowPowerCmd(ENABLE);

		Config_RCC(&SavRCC);

		SysTick->CTRL = 0; 
		      
		/* Swith in MSI 32KHz */
		SetHSICLKToMSI(RCC_MSIRange_0,DIV2,WITHRTC) ;     

		PWR_WakeUpPinCmd(PWR_WakeUpPin_1,ENABLE);

		PWR_UltraLowPowerCmd(ENABLE); 

		PWR_EnterSTANDBYMode();

		break;
	}

	/* Disable the RTC Alarm interrupt */
	RTC_ITConfig(RTC_IT_ALRA, DISABLE);
	RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

	UnInit_GPIOs();
	
	//SetHSICLK();  

	//Config_Systick(); 	
	SystemInit();
	//adjust VTOR
	SCB->VTOR = strVTOR;
	
	vPortSetupTimerInterrupt();
	
	RCC->AHBENR = SavRCC.AHBENR;	
	     
	PWR_VoltageScalingConfig(PWR_VoltageScaling_Range1);
	/* Wait Until the Voltage Regulator is ready */
	while (PWR_GetFlagStatus(PWR_FLAG_VOS) != RESET) ;

	/* Read ADC for current measurmeent */
	adc_measure = Current_Measurement();

	/* To restore RCC registers */
	RCC->APB1ENR = SavRCC.APB1ENR;
	RCC->APB2ENR = SavRCC.APB2ENR; 
	RCC->AHBLPENR = SavRCC.AHBLPENR;	
	RCC->APB1LPENR = SavRCC.APB1LPENR;
	RCC->APB2LPENR = SavRCC.APB2LPENR;

	/* Need to reinit RCC for LCD*/
	RCC_Configuration();

	PWR_EnterLowPowerRunMode(DISABLE);

	/* Disable Ultra low power mode */
	PWR_UltraLowPowerCmd(DISABLE);

	/* Disable FLASH during SLeep LP */
	FLASH_SLEEPPowerDownCmd(DISABLE);

	Restore_GPIO_Config();  

	/* Clear Wake Up flag */
	PWR_ClearFlag(PWR_FLAG_WU);

	/* Enable PVD */
	PWR_PVDCmd(ENABLE);

	STM_EVAL_LEDInit(LED_RUN);

	Init_GPIOs();
 
	//DEBUG_USART_Configuration();

	//CTRL_USART_Configuration(9600,USART_Parity_Even);

	PrintfXTOS("\n!!! restore in XtBspPowerManager... \n");

	switch (Mcu_State)
	{        
		/* STOP modes
		Measurement done to MSI 32 Khz without or with RTC
		*/		
		case MCU_STOP_NoRTC:
		case MCU_STOP_RTC:
			 
		/* Substract operational amplifier bias current from value*/
		if ( adc_measure > Bias_Current )
			adc_measure -= Bias_Current;
		/* Convert value in uA*/
		Current = adc_measure * Vdd_appli()/ADC_CONV; 
		Current *= 20L; 
		/* test if value is correct */
		if ((int) Current < MAX_CURRENT)
		{
			convert_into_char(Current, tab);
			tab[5] = 'A';
			tab[4] = '?';
				
			/* Test the significant digit for displays 3 or 4 digits*/
			if ( tab[0] != '0')
			{
			tab[1] |= DOT; /* To add decimal point */
			}  else  {
			/* To shift for suppress '0' before decimal */
			tab[0] = tab[1] ;	
			tab[0] |= DOT ;
			tab[1] = tab[2] ;
			tab[2] = tab[3] ;		
			tab[3] = ' ';
			}

		  PrintfXTOS("*** In XtBspPowerManager %s uA ...\n",tab);
		}

		break;        

	}
	
#elif defined(NUVOTON_NUC200)
#else
#endif
 
	return ret;
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
