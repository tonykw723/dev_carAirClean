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
#include "discover_board.h"
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
