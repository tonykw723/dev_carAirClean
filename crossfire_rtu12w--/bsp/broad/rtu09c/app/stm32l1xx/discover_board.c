/**
  ******************************************************************************
  * @file    stm32f0_discovery.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   This file provides set of firmware functions to manage Leds and
  *          push-button available on STM32F0-DISCOVERY Kit from STMicroelectronics.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Includes ------------------------------------------------------------------*/
#include "xtsys_type.h"
#include "xtsys_cfg.h"
#include "config.h"
#include "discover_board.h"
#include "xt_bsp_rtu09c.h"
#include "global_para.h"
/** @addtogroup Utilities
  * @{
  */ 

/** @addtogroup STM32F0_DISCOVERY
  * @{
  */ 
    
/** @defgroup STM32F0_DISCOVERY_LOW_LEVEL 
  * @brief This file provides firmware functions to manage Leds and push-buttons, 
  *        available on STM32F0_DISCOVERY evaluation board from STMicroelectronics.
  * @{
  */ 

/** @defgroup STM32F0_DISCOVERY_LOW_LEVEL_Private_TypesDefinitions
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32F0_DISCOVERY_LOW_LEVEL_Private_Defines
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32F0_DISCOVERY_LOW_LEVEL_Private_Macros
  * @{
  */ 
/**
  * @}
  */ 


/** @defgroup STM32F0_DISCOVERY_LOW_LEVEL_Private_Variables
  * @{
  */ 
//extern rtu_para_t glbRtuPara;
GPIO_TypeDef* GPIO_PORT[LEDn] = { LED_RUN_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = { LED_RUN_PIN};
const uint32_t GPIO_CLK[LEDn] = { LED_RUN_GPIO_CLK};

#define GPIO_DO_3GPOWEREN_PIN                    GPIO_Pin_8
#define GPIO_DO_3GPOWEREN_PORT              GPIOB
#define GPIO_DO_3GPOWEREN_CLK               RCC_AHBPeriph_GPIOB 

#define GPIO_DO_3GRESET_PIN                    GPIO_Pin_9
#define GPIO_DO_3GRESET_PORT              GPIOB
#define GPIO_DO_3GRESET_CLK               RCC_AHBPeriph_GPIOB 

#define GPIO_DO_3GONOFF_PIN                    GPIO_Pin_4
#define GPIO_DO_3GONOFF_PORT              GPIOA
#define GPIO_DO_3GONOFF_CLK               RCC_AHBPeriph_GPIOA

#define GPIO_DO_OTHER_POWEREN_PIN                    GPIO_Pin_12
#define GPIO_DO_OTHER_POWEREN_PORT              GPIOA
#define GPIO_DO_OTHER_POWEREN_CLK               RCC_AHBPeriph_GPIOA 



GPIO_TypeDef* GPIO_DO_PORT[GPIOn] = {GPIO_DO_DLZ_PORT, GPIO_DO_3GPOWEREN_PORT,GPIO_DO_3GRESET_PORT,
									GPIO_DO_3GONOFF_PORT,GPIO_DO_OTHER_POWEREN_PORT,
									GPIO_I2C_SCL_GPIO_PORT,GPIO_I2C_SDA_GPIO_PORT};
const uint16_t GPIO_DO_PIN[GPIOn] = {GPIO_DO_DLZ_PIN, GPIO_DO_3GPOWEREN_PIN,GPIO_DO_3GRESET_PIN,
									GPIO_DO_3GONOFF_PIN,GPIO_DO_OTHER_POWEREN_PIN,
									GPIO_I2C_SCL_PIN,GPIO_I2C_SDA_PIN};
const uint32_t GPIO_DO_CLK[GPIOn] = {GPIO_DO_DLZ_CLK,GPIO_DO_3GPOWEREN_CLK,GPIO_DO_3GRESET_CLK,
									GPIO_DO_3GONOFF_CLK,GPIO_DO_OTHER_POWEREN_CLK,
									GPIO_I2C_SCL_GPIO_CLK,GPIO_I2C_SDA_GPIO_CLK};



#define EVAL_COM1_TX_BUFFER          		(0)
#define EVAL_COM1_RX_BUFFER          		(0)

#define EVAL_COM2_TX_BUFFER          		(0)
#define EVAL_COM2_RX_BUFFER          		(0)

USART_TypeDef* COM_USART[COMn] = {EVAL_COM1,EVAL_COM2,EVAL_COM3}; 

GPIO_TypeDef* COM_TX_PORT[COMn] = {EVAL_COM1_TX_GPIO_PORT,EVAL_COM2_TX_GPIO_PORT,EVAL_COM3_TX_GPIO_PORT};
 
GPIO_TypeDef* COM_RX_PORT[COMn] = {EVAL_COM1_RX_GPIO_PORT,EVAL_COM2_RX_GPIO_PORT,EVAL_COM3_RX_GPIO_PORT};

const uint32_t COM_USART_CLK[COMn] = {EVAL_COM1_CLK,EVAL_COM2_CLK,EVAL_COM3_CLK};

const uint32_t COM_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK,EVAL_COM2_TX_GPIO_CLK,EVAL_COM3_TX_GPIO_CLK};
 
const uint32_t COM_RX_PORT_CLK[COMn] = {EVAL_COM1_RX_GPIO_CLK,EVAL_COM2_RX_GPIO_CLK,EVAL_COM3_RX_GPIO_CLK};

const uint16_t COM_TX_PIN[COMn] = {EVAL_COM1_TX_PIN,EVAL_COM2_TX_PIN,EVAL_COM3_TX_PIN};

const uint16_t COM_RX_PIN[COMn] = {EVAL_COM1_RX_PIN,EVAL_COM2_RX_PIN,EVAL_COM3_RX_PIN};
 
const uint16_t COM_TX_PIN_SOURCE[COMn] = {EVAL_COM1_TX_SOURCE,EVAL_COM2_TX_SOURCE,EVAL_COM3_TX_SOURCE};

const uint16_t COM_RX_PIN_SOURCE[COMn] = {EVAL_COM1_RX_SOURCE,EVAL_COM2_RX_SOURCE,EVAL_COM3_RX_SOURCE};
 
const uint16_t COM_TX_AF[COMn] = {EVAL_COM1_TX_AF,EVAL_COM2_TX_AF,EVAL_COM3_TX_AF};
 
const uint16_t COM_RX_AF[COMn] = {EVAL_COM1_RX_AF,EVAL_COM2_RX_AF,EVAL_COM3_RX_AF};

const uint8_t COM_IRQ[COMn] = {EVAL_COM1_IRQn,EVAL_COM2_IRQn,EVAL_COM3_IRQn};


/**
  * @}
  */ 


/** @defgroup STM32F0_DISCOVERY_LOW_LEVEL_Private_FunctionPrototypes
  * @{
  */ 

/**
  * @}
  */ 

/** @defgroup STM32F0_DISCOVERY_LOW_LEVEL_Private_Functions
  * @{
  */ 

/**
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be configured. 
  *   This parameter can be one of following parameters:
  *     @arg LED3
  *     @arg LED4
  * @retval None
  */
void STM_EVAL_LEDInit(Led_TypeDef Led)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_AHBPeriphClockCmd(GPIO_CLK[Led], ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
}

/**
  * @brief  Turns selected LED On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg LED3
  *     @arg LED4  
  * @retval None
  */
void STM_EVAL_LEDOn(Led_TypeDef Led)
{
  GPIO_PORT[Led]->ODR &= ~GPIO_PIN[Led];
}

/**
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg LED3
  *     @arg LED4 
  * @retval None
  */
void STM_EVAL_LEDOff(Led_TypeDef Led)
{
  GPIO_PORT[Led]->BSRRL |= GPIO_PIN[Led];
}

/**
  * @brief  Configures DO GPIO.
  * @param  Led: Specifies the gpio do to be configured. 
  *   This parameter can be one of following parameters:
  *     @arg GPIO_DO_0
  *     @arg GPIO_DO_1
  * @retval None
  */
void STM_EVAL_GPIOInit(Gpio_TypeDef dochn)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_AHBPeriphClockCmd(GPIO_DO_CLK[dochn], ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_DO_PIN[dochn];
  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIO_DO_PORT[dochn], &GPIO_InitStructure);
}

/**
  * @brief  Turns selected gpio do On.
  * @param  Led: Specifies the Led to be set on. 
  *   This parameter can be one of following parameters:
  *     @arg GPIO_DO_0
  *     @arg GPIO_DO_1  
  * @retval None
  */
void STM_EVAL_GPIOOn(Gpio_TypeDef dochn)
{
  GPIO_DO_PORT[dochn]->ODR &= ~GPIO_DO_PIN[dochn];
}

/**
  * @brief  Turns selected gpio do Off.
  * @param  Led: Specifies the Led to be set off. 
  *   This parameter can be one of following parameters:
  *     @arg GPIO_DO_0
  *     @arg GPIO_DO_1 
  * @retval None
  */
void STM_EVAL_GPIOOff(Gpio_TypeDef dochn)
{
  GPIO_DO_PORT[dochn]->BSRRL |= GPIO_DO_PIN[dochn];
}

void STM_EVAL_GPIOToggle(Gpio_TypeDef dochn)
{
  GPIO_DO_PORT[dochn]->ODR ^= GPIO_DO_PIN[dochn];
}


void TIM2_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void STM_EVAL_Tim2Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	unsigned int val;
	unsigned int psc;
	unsigned int arr;
	RCC_ClocksTypeDef RCC_Clocks;
	
	RCC_GetClocksFreq(&RCC_Clocks);
	
	TIM2_Config();

	//----------625us
	//PCLK2_Frequency=16000000,val =10000,psc=0,arr=9999(period)
	//time=(psc+1)*arr/PCLK2=625us
	val = RCC_Clocks.PCLK2_Frequency / (100);
	psc = (val + 49999) / 50000 - 1;
	arr = (val / (psc + 1)) - 1;

	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;//;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM2, psc, TIM_PSCReloadMode_Update);

	TIM_SetAutoreload(TIM2, arr);

	/* TIM Interrupts enable */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	/* TIM2 enable counter */
	TIM_Cmd(TIM2, ENABLE);
}

volatile int adc1Buf[1] = {0};

void STM_EVAL_ADCConfig(void)
{
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	DMA_InitTypeDef       DMA_InitStructure;
	GPIO_InitTypeDef      GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable ADC1, and GPIO clocks ****************************************/
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 | RCC_AHBPeriph_GPIOC, ENABLE);     
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	DMA_DeInit(DMA1_Channel1);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)adc1Buf;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);
	DMA_Cmd(DMA1_Channel1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	/* ADC Common Init **********************************************************/
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInit(&ADC_CommonInitStructure);

	ADC_DeInit(ADC1);
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;		//使能ADC扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//连续扫描模式
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 14;
	ADC_Init(ADC1, &ADC_InitStructure);
//	ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T3_CC2);

	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  	NVIC_Init(&NVIC_InitStructure);

	/*enable transfer complete interrupt------------------*/
	//DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE); 
	
	/* ADC3 regular channel7 configuration *************************************/

	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 1, ADC_SampleTime_16Cycles);

	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, ENABLE);
	ADC_Cmd(ADC1, ENABLE);
	//ADC_SoftwareStartConv(ADC1);

}

/**
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be toggled. 
  *   This parameter can be one of following parameters:
  *     @arg LED3
  *     @arg LED4  
  * @retval None
  */
void STM_EVAL_LEDToggle(Led_TypeDef Led)
{
  GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}


//-------------------------COM--------------------------------------------
void TIM4_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* TIM3 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

	/* Enable the TIM4 gloabal Interrupt */
	/*
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);*/
}

void STM_EVAL_Tim4Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	unsigned int val;
	unsigned int psc;
	unsigned int arr;
	RCC_ClocksTypeDef RCC_Clocks;
	TIM_DeInit(TIM2);
	RCC_GetClocksFreq(&RCC_Clocks);
	
	TIM4_Config();
	//---------------------------------------------------------------------
	//----------115200:8.68us [115200波特率个别字节偶尔会出错]
	//PCLK2_Frequency=16000000,val =10000,psc=0,arr=9999(period)
	//time=(psc+1)*(arr+1)/PCLK2=8.68us
	//psc=0;
	//arr=138;	
	//----------57600:17.36us[选择57600 刚刚好，不会出错]	
	//PCLK2_Frequency=16000000,val =10000,psc=0,arr=9999(period)
	//time=(psc+1)*(arr+1)/PCLK2=8.68us
	//psc=0;
	//arr=277;		
	//----------9600:104us
	//PCLK2_Frequency=16000000,val =10000,psc=0,arr=9999(period)
	//time=(psc+1)*(arr+1)/PCLK2=104us
	//psc=0;
	//arr=1666;	
	//------------------------------------------------------------------------
	
	psc=0;
	arr=277;
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = arr;
	TIM_TimeBaseStructure.TIM_Prescaler = psc;//;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	/* Prescaler configuration */
	TIM_PrescalerConfig(TIM4, psc, TIM_PSCReloadMode_Update);

	TIM_SetAutoreload(TIM4, arr);

	/* TIM Interrupts enable */
	//TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

	/* TIM4 enable counter */
	TIM_Cmd(TIM4, ENABLE);
}

void STM_EVAL_Tim4_STOP(void)
{
	//TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
	TIM_Cmd(TIM4, DISABLE);
}

volatile xt_u32 stm_sim_tim_flag = 0;
#define STM_EVAL_SIM_COM_LOW()   GPIOA->ODR &= ~GPIO_Pin_1//low
#define STM_EVAL_SIM_COM_HIGH() GPIOA->BSRRL |= GPIO_Pin_1//high


void STM_EVAL_SIM_COMInit()
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;  	
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  STM_EVAL_SIM_COM_HIGH();
  
}
/*
void STM_EVAL_SIM_COM_DELAY(xt_u32 us)
{
#if 1
	xt_u32 i = 0,j = 0;
	for(i = 0; i < us + 0; i++)
		for(j = 0; j < 3; j++);
#else
	xt_u32 i = 0,j = 0;
	
	stm_sim_tim_flag = 0;
	STM_EVAL_Tim4Config();
	
	while(stm_sim_tim_flag < 3)
	{
		STM_EVAL_Tim4Config();
	}

	STM_EVAL_Tim4_STOP();
	stm_sim_tim_flag = 0;

	for(i = 0; i < 5; i++)
#endif
}*/
#define 	STM_EVAL_SIM_COM_DELAY() TIM4->SR = (uint16_t)~TIM_FLAG_Update;while ((TIM4->SR & TIM_FLAG_Update) == (uint16_t)RESET);\
																	TIM4->SR = (uint16_t)~TIM_FLAG_Update

void STM_EVAL_SIM_COM_PUTC(xt_u8 ch)
{
	xt_u32 us = 103; // 9600 bps;
	xt_u32 i = 0,j = 0;

#if defined (USE_IWDG_AS_WDG)
	IWDG_ReloadCounter();
#elif defined (USE_WWDG_AS_WDG)
	WWDG_SetCounter(0x7f);
#endif
	//__set_FAULTMASK(1);
	vTaskSuspendAll();
	STM_EVAL_Tim4Config();
	STM_EVAL_SIM_COM_LOW();		//start bit 1
	STM_EVAL_SIM_COM_DELAY();

	for(i = 0; i < 8; i++)		//data bit 8
	{
		if ((ch & (0x1<<i)) != 0)
			STM_EVAL_SIM_COM_HIGH();
		else
			STM_EVAL_SIM_COM_LOW();

		STM_EVAL_SIM_COM_DELAY();
	}

	STM_EVAL_SIM_COM_HIGH();	//stop bit 1
	STM_EVAL_SIM_COM_DELAY();
	//__set_FAULTMASK(0);
	STM_EVAL_Tim4_STOP();
	xTaskResumeAll();
}

void STM_EVAL_COM_NVIC_Config(COM_TypeDef COM,uint8_t en)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = COM_IRQ[COM];
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
  NVIC_InitStructure.NVIC_IRQChannelCmd = (FunctionalState)en;
  NVIC_Init(&NVIC_InitStructure);
}

void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], ENABLE);

  /* Enable USART clock */
  if (COM == COM1)
  	RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
  else if (COM == COM2)
  	RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE); 
  else if (COM == COM3)
  	RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);   
  
  /* Connect PXx to USARTx_Tx */
  GPIO_PinAFConfig(COM_TX_PORT[COM], COM_TX_PIN_SOURCE[COM], COM_TX_AF[COM]);

  /* Connect PXx to USARTx_Rx */
  GPIO_PinAFConfig(COM_RX_PORT[COM], COM_RX_PIN_SOURCE[COM], COM_RX_AF[COM]);
  
  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = COM_TX_PIN[COM];
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(COM_TX_PORT[COM], &GPIO_InitStructure);
    
  /* Configure USART Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = COM_RX_PIN[COM];
  GPIO_Init(COM_RX_PORT[COM], &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(COM_USART[COM], USART_InitStruct);
    
  /* Enable USART */
  USART_Cmd(COM_USART[COM], ENABLE);
}

void STM_EVAL_COMDeInit(COM_TypeDef COM)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIO clock */
	RCC_AHBPeriphClockCmd(COM_TX_PORT_CLK[COM] | COM_RX_PORT_CLK[COM], DISABLE);

	if (COM == COM1)
		RCC_APB2PeriphClockCmd(COM_USART_CLK[COM], DISABLE); 
	else if (COM == COM2)
		RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], DISABLE); 
	else if (COM == COM3)
  		RCC_APB1PeriphClockCmd(COM_USART_CLK[COM], ENABLE);

	/* USART configuration */
	USART_DeInit(COM_USART[COM]);

	/* Enable USART */
	USART_Cmd(COM_USART[COM], DISABLE);
}
//-------------------------FLASH--------------------------------------
#define DATA_EEPROM_START_ADDR     0x08080100
#define DATA_EEPROM_END_ADDR         0x080807FF
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
__IO FLASH_Status FLASHStatus = FLASH_COMPLETE;
__IO TestStatus DataMemoryProgramStatus = PASSED;


uint8_t is_eepromAddrAvailable(uint32_t ee_addr)
{
	if((ee_addr<DATA_EEPROM_START_ADDR)||(ee_addr>DATA_EEPROM_END_ADDR))
	{
		PrintfXTOS("disavailable eeprom addr\n");
		return 1;
	}
	return 0;
}

uint32_t STM_EVAL_FlashEepromWriteByte(uint16_t Addr,uint8_t *Buffer,uint16_t Length)
{   
    uint8_t cnt = 0;
	uint32_t j,FLASHStatus,Address=DATA_EEPROM_START_ADDR+Addr;
	if(is_eepromAddrAvailable(Address))
		return 1;	
	

	DATA_EEPROM_Unlock();	

  	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);	
  	//PrintfXTOS("erase:\n ");
  
  	for(j=0;j<Length;j++)
  	{
  		FLASHStatus=DATA_EEPROM_EraseByte( Address);
		//PrintfXTOS("~~%x ",FLASHStatus);
  	}
	//PrintfXTOS("\n ");
	for(j=0;j<Length;j++)
	{

Retry:    
#if defined (USE_IWDG_AS_WDG)
		IWDG_ReloadCounter();
#elif defined (USE_WWDG_AS_WDG)
		WWDG_SetCounter(0x7f);
#endif
		
		FLASHStatus=DATA_EEPROM_FastProgramByte(Address, Buffer[j]);
		//PrintfXTOS("##%x ",FLASHStatus);
		if(FLASHStatus == FLASH_COMPLETE)
		{
		  Address++;
		}
		else
		{
		  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
		                       | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
          if (cnt++ < 5) {
              goto Retry;
          } else {
              break;            
          }
		}				
	}	
	DATA_EEPROM_Lock();
	return 0;	
	
}

uint32_t STM_EVAL_FlashEepromReadByte(uint16_t Addr,uint8_t *Buffer,uint16_t Length)
{
	uint32_t j=0,Address=DATA_EEPROM_START_ADDR+Addr;
	if(is_eepromAddrAvailable(Address))
		return 1;	
	
	DATA_EEPROM_Unlock();	
	//printf("start index=%d len=%d\r\n",Addr,Length);
	for(j=0;j<Length;j++)
	{
#if defined (USE_IWDG_AS_WDG)
		IWDG_ReloadCounter();
#elif defined (USE_WWDG_AS_WDG)
		WWDG_SetCounter(0x7f);
#endif
		while(FLASH_GetStatus()==FLASH_BUSY);
		Buffer[j]=*(__IO uint8_t*)Address;
		Address = Address ++;
		
		//printf("%x ",Buffer[j]);
		//if((j+1)%10==0)
		//	printf("\r\n");
		
	}
	//printf("\r\n");
	DATA_EEPROM_Lock();
	return 0;
}





void STM_EVAL_FlashEepromInit(rtu_para_t *pt)
{
		
}


/**
  * @}
  */ 

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */ 
    
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
