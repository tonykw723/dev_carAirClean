/**
  ******************************************************************************
  * @file    I2C_AT24LCxxx.h
  * @author  TONY Application Team
  * @version V1.0.0
  * @date    17-4-2014
  * @brief   
  ******************************************************************************
  * @attention
  ******************************************************************************
  */  
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __I2C_AT24LCXXX_H
#define __I2C_AT24LCXXX_H

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx.h"
#include "stm32l1xx_i2c.h"
#include "discover_board.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define sEE_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define sEE_LONG_TIMEOUT         ((uint32_t)(10 * sEE_FLAG_TIMEOUT))
#define I2C_TIMEOUT         		sEE_LONG_TIMEOUT//(uint32_t)0x000FF /*!< I2C Time out */
#define AT24LCxxx_ADDR               0xa2   /*!< LM75 address */
#define AT24LCxxx_I2C_SPEED      400000 /*!< I2C Speed */

#define AT24LCxxx_EEPROM_ADDRESS_USER_READ		0xA3 /* I2C DeviceSelect */
#define AT24LCxxx_EEPROM_ADDRESS_USER_WRITE		0xA2 /* I2C DeviceSelect */

/* Private function prototypes -----------------------------------------------*/
static void AT24LCxxx_LowLevel_DeInit(void);
static void AT24LCxxx_LowLevel_Init(void);

/* Private function prototypes -----------------------------------------------*/
void AT24LCxxx_DeInit(void);
void AT24LCxxx_Init(void);
ErrorStatus AT24LCxxx_GetStatus(void);

uint8_t AT24LCxxx_ReadOneByte (uint8_t EE_address, uint16_t ReadAddr,uint8_t* pBuffer);
void AT24LCxxx_ReadBuffer (uint8_t EE_address, uint16_t ReadAddr, uint8_t NumByteToRead,uint8_t* pBuffer);
uint8_t AT24LCxxx_WriteOneByte (uint8_t EE_address, uint16_t ReadAddr,uint8_t pBuffer);



#endif 

