 /**
  ******************************************************************************
  * @file    discover_board.h
  * @author  Microcontroller Division
  * @version V1.0.3
  * @date    May-2013
  * @brief   Input/Output defines
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  */

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __DISCOVER_BOARD_H
#define __DISCOVER_BOARD_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"  

#define bool _Bool
#define FFALSE 0
#define TTRUE !FFALSE


typedef enum 
{
  LED_RUN 
} Led_TypeDef;

typedef enum 
{
  GPIO_DO_DLZ = 0,
  GPIO_DO_3GPOWEREN = 1,
  GPIO_DO_3GRESET=2,
  GPIO_DO_3GONOFF=3,
  GPIO_DO_OTHER_POWEREN=4,
  GPIO_I2C_SCL= 5,
  GPIO_I2C_SDA = 6,  
} Gpio_TypeDef;

typedef enum 
{
  BUTTON_USER = 0,
} Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

typedef enum 
{
  COM1 = 0,
  COM2 = 1,
  COM3 = 2,
} COM_TypeDef;

#define COMn                             3


/**
 * @brief Definition for COM port1, connected to USART1
 */ 
#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1

#define EVAL_COM1_TX_PIN                 GPIO_Pin_6
#define EVAL_COM1_TX_GPIO_PORT           GPIOB
#define EVAL_COM1_TX_GPIO_CLK            RCC_AHBPeriph_GPIOB
#define EVAL_COM1_TX_SOURCE              GPIO_PinSource6
#define EVAL_COM1_TX_AF                  GPIO_AF_USART1

#define EVAL_COM1_RX_PIN                 GPIO_Pin_7
#define EVAL_COM1_RX_GPIO_PORT           GPIOB
#define EVAL_COM1_RX_GPIO_CLK            RCC_AHBPeriph_GPIOB
#define EVAL_COM1_RX_SOURCE              GPIO_PinSource7
#define EVAL_COM1_RX_AF                  GPIO_AF_USART1

#define EVAL_COM1_CTS_PIN                GPIO_Pin_11
#define EVAL_COM1_CTS_GPIO_PORT          GPIOA
#define EVAL_COM1_CTS_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define EVAL_COM1_CTS_SOURCE             GPIO_PinSource11
#define EVAL_COM1_CTS_AF                 GPIO_AF_USART1

#define EVAL_COM1_RTS_PIN                GPIO_Pin_12
#define EVAL_COM1_RTS_GPIO_PORT          GPIOA
#define EVAL_COM1_RTS_GPIO_CLK           RCC_AHBPeriph_GPIOA
#define EVAL_COM1_RTS_SOURCE             GPIO_PinSource12
#define EVAL_COM1_RTS_AF                 GPIO_AF_USART1
   
#define EVAL_COM1_IRQn                   USART1_IRQn


/**
 * @brief Definition for COM port2, connected to USART2
 */ 
#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM2_TX_PIN                 GPIO_Pin_2
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK            RCC_AHBPeriph_GPIOA
#define EVAL_COM2_TX_SOURCE              GPIO_PinSource2
#define EVAL_COM2_TX_AF                  GPIO_AF_USART2
#define EVAL_COM2_RX_PIN                 GPIO_Pin_3
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK            RCC_AHBPeriph_GPIOA
#define EVAL_COM2_RX_SOURCE              GPIO_PinSource3
#define EVAL_COM2_RX_AF                  GPIO_AF_USART2
#define EVAL_COM2_IRQn                   USART2_IRQn
#define EVAL_COM2_TX_STREAM          	0
#define EVAL_COM2_RX_STREAM          	0
#define EVAL_COM2_TX_CHANNEL			0
#define EVAL_COM2_RX_CHANNEL          	0
#define EVAL_COM2_TX_IRQ          		0
#define EVAL_COM2_RX_IRQ          		0

#define EVAL_COM2_RTS_PIN                 GPIO_Pin_1
#define EVAL_COM2_RTS_GPIO_PORT           GPIOA
#define EVAL_COM2_RTS_GPIO_CLK            RCC_AHBPeriph_GPIOA
#define EVAL_COM2_RTS_SOURCE              GPIO_PinSource1
#define EVAL_COM2_RTS_AF                  GPIO_AF_USART2
#define EVAL_COM2_CTS_PIN                 GPIO_Pin_0
#define EVAL_COM2_CTS_GPIO_PORT           GPIOA
#define EVAL_COM2_CTS_GPIO_CLK            RCC_AHBPeriph_GPIOA
#define EVAL_COM2_CTS_SOURCE              GPIO_PinSource0
#define EVAL_COM2_CTS_AF                  GPIO_AF_USART2


/**
 * @brief Definition for COM port3, connected to USART3
 */ 

#define EVAL_COM3                        USART3
#define EVAL_COM3_CLK                    RCC_APB1Periph_USART3
#define EVAL_COM3_TX_PIN                 GPIO_Pin_10
#define EVAL_COM3_TX_GPIO_PORT           GPIOC
#define EVAL_COM3_TX_GPIO_CLK            RCC_AHBPeriph_GPIOC
#define EVAL_COM3_TX_SOURCE              GPIO_PinSource10
#define EVAL_COM3_TX_AF                  GPIO_AF_USART3
#define EVAL_COM3_RX_PIN                 GPIO_Pin_11
#define EVAL_COM3_RX_GPIO_PORT           GPIOC
#define EVAL_COM3_RX_GPIO_CLK            RCC_AHBPeriph_GPIOC
#define EVAL_COM3_RX_SOURCE              GPIO_PinSource11
#define EVAL_COM3_RX_AF                  GPIO_AF_USART3
#define EVAL_COM3_IRQn                   USART3_IRQn
#define EVAL_COM3_TX_STREAM          	0
#define EVAL_COM3_RX_STREAM          	0
#define EVAL_COM3_TX_CHANNEL			0
#define EVAL_COM3_RX_CHANNEL          	0
#define EVAL_COM3_TX_IRQ          		0
#define EVAL_COM3_RX_IRQ          		0

#define EVAL_COM3_RTS_PIN                 GPIO_Pin_14
#define EVAL_COM3_RTS_GPIO_PORT           GPIOB
#define EVAL_COM3_RTS_GPIO_CLK            RCC_AHBPeriph_GPIOB
#define EVAL_COM3_RTS_SOURCE              GPIO_PinSource14
#define EVAL_COM3_RTS_AF                  GPIO_AF_USART3
#define EVAL_COM3_CTS_PIN                 GPIO_Pin_13
#define EVAL_COM3_CTS_GPIO_PORT           GPIOB
#define EVAL_COM3_CTS_GPIO_CLK            RCC_AHBPeriph_GPIOB
#define EVAL_COM3_CTS_SOURCE              GPIO_PinSource13
#define EVAL_COM3_CTS_AF                  GPIO_AF_USART3

/**
  * @}
  */ 

/** @defgroup STM32F0_DISCOVERY_LOW_LEVEL_Exported_Constants
  * @{
  */ 

/** @addtogroup STM32F0_DISCOVERY_LOW_LEVEL_LED
  * @{
  */
#define LEDn                             1
  
  
#define LED_RUN_PIN                         GPIO_Pin_4
#define LED_RUN_GPIO_PORT                   GPIOA
#define LED_RUN_GPIO_CLK                    RCC_AHBPeriph_GPIOA

#define GPIOn                            7

#define GPIO_DO_DLZ_PIN                    GPIO_Pin_8
#define GPIO_DO_DLZ_PORT              GPIOC
#define GPIO_DO_DLZ_CLK               RCC_AHBPeriph_GPIOC


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

#define GPIO_I2C_SCL_PIN                 GPIO_Pin_6                  
#define GPIO_I2C_SCL_GPIO_PORT           GPIOB    
#define GPIO_I2C_SCL_GPIO_CLK               RCC_AHBPeriph_GPIOB

#define GPIO_I2C_SDA_PIN                 GPIO_Pin_7                  
#define GPIO_I2C_SDA_GPIO_PORT           GPIOB    
#define GPIO_I2C_SDA_GPIO_CLK               RCC_AHBPeriph_GPIOB
/**
  * @}
  */ 


/**
  * @}
  */ 
  
/**
  * @brief  M24LR16E-R I2C Interface pins
  */
#define M24LR04E_I2C                         I2C1
#define M24LR04E_I2C_CLK                     RCC_APB1Periph_I2C1

#define M24LR04E_I2C_SCL_PIN                 GPIO_Pin_6                  
#define M24LR04E_I2C_SCL_GPIO_PORT           GPIOB    
#define M24LR04E_I2C_CLK_SCL_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define M24LR04E_I2C_SCL_SOURCE               GPIO_PinSource6
#define M24LR04E_I2C_SCL_AF                   GPIO_AF_I2C1

#define M24LR04E_I2C_SDA_PIN                 GPIO_Pin_7                 
#define M24LR04E_I2C_SDA_GPIO_PORT           GPIOB                     
#define M24LR04E_I2C_CLK_SDA_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define M24LR04E_I2C_SDA_SOURCE               GPIO_PinSource7
#define M24LR04E_I2C_SDA_AF                   GPIO_AF_I2C1


/**
  * @brief  M24LR16E-R I2C Interface pins
  */
#define AT24LCxxx_I2C                         I2C1
#define AT24LCxxx_I2C_CLK                     RCC_APB1Periph_I2C1

#define AT24LCxxx_I2C_SCL_PIN                 GPIO_Pin_6                  
#define AT24LCxxx_I2C_SCL_GPIO_PORT           GPIOB    
#define AT24LCxxx_I2C_CLK_SCL_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define AT24LCxxx_I2C_SCL_SOURCE               GPIO_PinSource6
#define AT24LCxxx_I2C_SCL_AF                   GPIO_AF_I2C1

#define AT24LCxxx_I2C_SDA_PIN                 GPIO_Pin_7                 
#define AT24LCxxx_I2C_SDA_GPIO_PORT           GPIOB                     
#define AT24LCxxx_I2C_CLK_SDA_GPIO_CLK             RCC_AHBPeriph_GPIOB
#define AT24LCxxx_I2C_SDA_SOURCE               GPIO_PinSource7
#define AT24LCxxx_I2C_SDA_AF                   GPIO_AF_I2C1


    
/** @defgroup STM32F0_DISCOVERY_LOW_LEVEL_Exported_Macros
  * @{
  */  
/**
  * @}
  */ 

/** @defgroup STM32F0_DISCOVERY_LOW_LEVEL_Exported_Functions
  * @{
  */
void STM_EVAL_GPIOInit(Gpio_TypeDef dochn);
void STM_EVAL_GPIOOn(Gpio_TypeDef dochn);
void STM_EVAL_GPIOOff(Gpio_TypeDef dochn);
void STM_EVAL_LEDInit(Led_TypeDef Led);
void STM_EVAL_LEDOn(Led_TypeDef Led);
void STM_EVAL_LEDOff(Led_TypeDef Led);
void STM_EVAL_LEDToggle(Led_TypeDef Led);
void STM_EVAL_COMInit(COM_TypeDef COM, USART_InitTypeDef* USART_InitStruct);
void STM_EVAL_COMDeInit(COM_TypeDef COM);
void STM_EVAL_COM_NVIC_Config(COM_TypeDef COM,uint8_t en);
void STM_EVAL_ADCConfig(void);
void STM_EVAL_Tim2Config(void);
uint32_t STM_EVAL_FlashEepromWriteByte(uint16_t Addr,uint8_t *Buffer,uint16_t Length);
uint32_t STM_EVAL_FlashEepromReadByte(uint16_t Addr,uint8_t *Buffer,uint16_t Length);
/* MACROs for SET, RESET or TOGGLE Output port */

#define GPIO_HIGH(a,b) 		a->BSRRL = b
#define GPIO_LOW(a,b)		a->BSRRH = b
#define GPIO_TOGGLE(a,b) 	a->ODR ^= b 

#define USERBUTTON_GPIO_PORT	GPIOA
#define USERBUTTON_GPIO_PIN     GPIO_Pin_0
#define USERBUTTON_GPIO_CLK     RCC_AHBPeriph_GPIOA

#define LD_GPIO_PORT 		GPIOB
#define LD_GREEN_GPIO_PIN 		GPIO_Pin_7
#define LD_BLUE_GPIO_PIN             GPIO_Pin_6
#define LD_GPIO_PORT_CLK             RCC_AHBPeriph_GPIOB

#define CTN_GPIO_PORT           GPIOC
#define CTN_CNTEN_GPIO_PIN      GPIO_Pin_13
#define CTN_GPIO_CLK            RCC_AHBPeriph_GPIOC

#define WAKEUP_GPIO_PORT        GPIOA

#define IDD_MEASURE_PORT	GPIOA
#define IDD_MEASURE             GPIO_Pin_4

#endif


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
