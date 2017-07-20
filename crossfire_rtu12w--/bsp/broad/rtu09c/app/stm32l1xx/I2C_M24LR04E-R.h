/**
  ******************************************************************************
  * @file    I2C_M24LR16E-R.h
  * @author  MMY Application Team
  * @version V1.0.0
  * @date    19-december-2011
  * @brief   This file contains all the functions prototypes for the 
  *          firmware I2C driver.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_M24LR04_H
#define __I2C_M24LR04_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_i2c.h"
#include "discover_board.h"


/** @addtogroup Utilities
  * @{
  */
  
/** @addtogroup STM8_EVAL
  * @{
  */ 

/** @addtogroup Common
  * @{
  */
  
/** @addtogroup STM8_EVAL_I2C_TSENSOR
  * @{
  */  

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/**
  * @brief  Block Size
  */
#define M24LR04E_REG_TEMP        0x00  /* Temperature Register of LM75 */
#define M24LR04E_REG_CONF        0x01  /* Configuration Register of LM75 */
#define M24LR04E_REG_THYS        0x02  /* Temperature Register of LM75 */
#define M24LR04E_REG_TOS         0x03  /* Over-temp Shutdown threshold Register of LM75 */
#define I2C_TIMEOUT         		(uint32_t)0x000FF /*!< I2C Time out */
#define M24LR04E_ADDR           0xa0   /*!< LM75 address */
//#define M24LR04E_I2C_SPEED      100000 /*!< I2C Speed */
#define M24LR04E_I2C_SPEED      200000 /*!< I2C Speed */

// I2C address for M24LR16-E Datalogger v2.0
#define M24LR16_EEPROM_ADDRESS_USER_READ		0xA7 /* I2C DeviceSelect */
#define M24LR16_EEPROM_ADDRESS_USER_WRITE		0xA6 /* I2C DeviceSelect */
#define M24LR16_EEPROM_ADDRESS_SYSTEM_WRITE	0xAE /* I2C DeviceSelect */
#define M24LR16_EEPROM_ADDRESS_SYSTEM_READ        0xAF /* I2C DeviceSelect */

/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void M24LR04E_LowLevel_DeInit(void);
static void M24LR04E_LowLevel_Init(void);

/* Private function prototypes -----------------------------------------------*/
void M24LR04E_DeInit(void);
void M24LR04E_Init(void);
ErrorStatus M24LR04E_GetStatus(void);
uint16_t M24LR04E_ReadTemp(void);
uint16_t M24LR04E_ReadReg(uint8_t RegName);
void M24LR04E_WriteReg(uint8_t RegName, uint16_t RegValue);
uint8_t M24LR04E_ReadConfReg(void);
void M24LR04E_WriteConfReg(uint8_t RegValue);
void M24LR04E_ShutDown(FunctionalState NewState);  


void M24LR04E_ReadOneByte (uint8_t EE_address, uint16_t ReadAddr,uint8_t* pBuffer);
void M24LR04E_ReadBuffer (uint8_t EE_address, uint16_t ReadAddr, uint8_t NumByteToRead,uint8_t* pBuffer);
void M24LR04E_WriteOneByte (uint8_t EE_address, uint16_t ReadAddr,uint8_t pBuffer);



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
#endif 
/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
