/**
  ******************************************************************************
  * @file    I2C_M24LR16E-R.c
  * @author  MMY Application Team
  * @version V1.0.0
  * @date    19-december-2011
  * @brief   This file provides a set of functions needed to manage the I2C M24LR16E
  *          temperature sensor mounted on STM8xx-EVAL board (refer to stm8_eval.h
  *          to know about the boards supporting this sensor).
  *          It implements a high level communication layer for read and write
  *          from/to this sensor. The needed STM8 hardware resources (I2C and
  *          GPIO) are defined in stm8xx_eval.h file, and the initialization is
  *          performed in M24LR04E_LowLevel_Init() function declared in stm8xx_eval.c
  *          file.
  *          You can easily tailor this driver to any other development board,
  *          by just adapting the defines for hardware resources and
  *          M24LR04E_LowLevel_Init() function.
  *
  *     +-----------------------------------------------------------------+
  *     |                        Pin assignment                           |
  *     +---------------------------------------+-----------+-------------+
  *     |  STM8 I2C Pins                       |   STLM75  |   Pin       |
  *     +---------------------------------------+-----------+-------------+
  *     | M24LR04E_I2C_SDA_PIN/ SDA                 |   SDA     |    1        |
  *     | M24LR04E_I2C_SCL_PIN/ SCL                 |   SCL     |    2        |
  *     | M24LR04E_I2C_SMBUSALERT_PIN/ SMBUS ALERT  |   OS/INT  |    3        |
  *     | .                                     |   GND     |    4  (0V)  |
  *     | .                                     |   GND     |    5  (0V)  |
  *     | .                                     |   GND     |    6  (0V)  |
  *     | .                                     |   GND     |    7  (0V)  |
  *     | .                                     |   VDD     |    8  (3.3V)|
  *     +---------------------------------------+-----------+-------------+
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


/* Includes ------------------------------------------------------------------*/
#include "I2C_M24LR04E-R.h"

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
  * @brief      This file includes the M24LR16E Temperature Sensor driver of
  *             STM8-EVAL boards.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/** @defgroup STM8_EVAL_I2C_TSENSOR_Private_Functions
  * @{
  */
/**
  * @brief  DeInitializes the M24LR04E_I2C.
  * @param  None
  * @retval None
  */
void M24LR04E_DeInit(void)
{
  M24LR04E_LowLevel_DeInit();
}

/**
  * @brief  Initializes the M24LR04E_I2C.
  * @param  None
  * @retval None
  */
void M24LR04E_Init(void)
{
  I2C_InitTypeDef  I2C_InitStructure;	
  M24LR04E_LowLevel_Init();

  /* I2C DeInit */
  ///I2C_DeInit(M24LR04E_I2C);

  /* I2C configuration */

	//I2C_Init(M24LR04E_I2C, M24LR04E_I2C_SPEED, 0x00, I2C_Mode_I2C,
      //     I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_7bit);
 
  /*I2C Struct Initialize */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0xa0;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = M24LR04E_I2C_SPEED;
  
#ifndef I2C_10BITS_ADDRESS
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */
  
  /*I2C Initialize */
  I2C_Init(M24LR04E_I2C, &I2C_InitStructure);
  


  /*!< M24LR04E_I2C Init */
  I2C_Cmd(M24LR04E_I2C, ENABLE);
}

/**
  * @brief  Checks the M24LR16E status.
  * @param  None
  * @retval ErrorStatus: M24LR16E Status (ERROR or SUCCESS).
  */
ErrorStatus M24LR04E_GetStatus(void)
{
  uint32_t I2C_TimeOut = I2C_TIMEOUT;

  /* Clear the M24LR04E_I2C AF flag */
  I2C_ClearFlag(M24LR04E_I2C, I2C_FLAG_AF);

  /* Enable M24LR04E_I2C acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(M24LR04E_I2C, ENABLE);

  /*---------------------------- Transmission Phase ---------------------------*/

  /* Send M24LR04E_I2C START condition */
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  /* Test on M24LR04E_I2C EV5 and clear it */
  while ((!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT)) && I2C_TimeOut)  /* EV5 */
  {
    I2C_TimeOut--;
  }
  if (I2C_TimeOut == 0)
  {
    return ERROR;
  }
  I2C_TimeOut = I2C_TIMEOUT;

  /*!< Send M24LR04E-R slave address for write */
  I2C_Send7bitAddress(M24LR04E_I2C, M24LR04E_ADDR, I2C_Direction_Transmitter);

  while ((!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && I2C_TimeOut)/* EV6 */
  {
    I2C_TimeOut--;
  }

  if ((I2C_GetFlagStatus(M24LR04E_I2C, I2C_FLAG_AF) != 0x00) || (I2C_TimeOut == 0))
  {
    return ERROR;
  }
  else
  {
    return SUCCESS;
  }
}
/**
  * @brief  Read the specified register from the M24LR16E.
  * @param  RegName: specifies the M24LR16E register to be read.
  *              This member can be one of the following values:
  *                  - M24LR04E_REG_TEMP: temperature register
  *                  - M24LR04E_REG_TOS: Over-limit temperature register
  *                  - M24LR04E_REG_THYS: Hysteresis temperature register
  * @retval M24LR16E register value.
  */
uint16_t M24LR04E_ReadReg(uint8_t RegName)
{
  __IO uint16_t RegValue = 0;

  /* Enable M24LR04E_I2C acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(M24LR04E_I2C, ENABLE);

  /*--------------------------- Transmission Phase ----------------------------*/
  /* Send M24LR04E_I2C START condition */
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  /* Test on M24LR04E_I2C EV5 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for write */
  I2C_Send7bitAddress(M24LR04E_I2C, M24LR04E_ADDR, I2C_Direction_Transmitter);

  /* Test on M24LR04E_I2C EV6 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
  {
  }

  /* Send the specified register data pointer */
  I2C_SendData(M24LR04E_I2C, RegName);

  /* Test on M24LR04E_I2C EV8 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /*------------------------------ Reception Phase ----------------------------*/
  /* Send Re-STRAT condition */
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for read */
  I2C_Send7bitAddress(M24LR04E_I2C, M24LR04E_ADDR, I2C_Direction_Receiver);

  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* EV6 */
  {
  }

  /* Test on EV7 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV7 */
  {
  }

  /* Store M24LR04E_I2C received data */
  RegValue = (uint16_t)(I2C_ReceiveData(M24LR04E_I2C) << 8);

  /* Disable M24LR04E_I2C acknowledgement */
  I2C_AcknowledgeConfig(M24LR04E_I2C, DISABLE);

  /* Send M24LR04E_I2C STOP Condition */
  I2C_GenerateSTOP(M24LR04E_I2C, ENABLE);

  /* Test on RXNE flag */
  while (I2C_GetFlagStatus(M24LR04E_I2C, I2C_FLAG_RXNE) == RESET)
  {}

  /* Store M24LR04E_I2C received data */
  RegValue |= I2C_ReceiveData(M24LR04E_I2C);

  /* Return register value */
  return (RegValue);
}

/**
  * @brief  Write to the specified register of the M24LR16E.
  * @param  RegName: specifies the M24LR16E register to be written.
  *              This member can be one of the following values:
  *                  - M24LR04E_REG_TOS: Over-limit temperature register
  *                  - M24LR04E_REG_THYS: Hysteresis temperature register
  * @param  RegValue: value to be written to M24LR16E register.
  * @retval None
  */
void M24LR04E_WriteReg(uint8_t RegName, uint16_t RegValue)
{
  /*-------------------------------- Transmission Phase -----------------------*/
  /* Send M24LR04E_I2C START condition */
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  /* Test on M24LR04E_I2C EV5 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for write */
  I2C_Send7bitAddress(M24LR04E_I2C, M24LR04E_ADDR, I2C_Direction_Transmitter);

  /* Test on M24LR04E_I2C EV6 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
  {
  }

  /* Send the specified register data pointer */
  I2C_SendData(M24LR04E_I2C, RegName);

  /* Test on M24LR04E_I2C EV8 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /* Send M24LR04E_I2C data */
  I2C_SendData(M24LR04E_I2C, (uint8_t)(RegValue >> 8));

  /* Test on M24LR04E_I2C EV8 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /* Send M24LR04E_I2C data */
  I2C_SendData(M24LR04E_I2C, (uint8_t)RegValue);

  /* Test on M24LR04E_I2C EV8 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /* Send M24LR04E_I2C STOP Condition */
  I2C_GenerateSTOP(M24LR04E_I2C, ENABLE);
}



/**
  * @brief this function reads one byte of data from the M24LR16E EEPROM . 
  * @param[in] pBuffer pointer to the buffer that receives the data read
  * from the EEPROM.
  * @param[in] WriteAddr EEPROM's internal address to read from.
  * @param[in] NumByteToWrite EEPROM's number of bytes to read from the EEPROM.  
  * @retval None
  * @par Required preconditions:
  * None
  */
void M24LR04E_ReadOneByte (uint8_t EE_address, uint16_t ReadAddr,uint8_t* pBuffer)
{
int32_t I2C_TimeOut = I2C_TIMEOUT;

  // Enable M24LR04E_I2C acknowledgement if it is already disabled by other function 
  I2C_AcknowledgeConfig(M24LR04E_I2C, ENABLE);

  //------------------------------------- Transmission Phase ------------------
  // Send M24LR04E_I2C START condition 
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

	  // Test on M24LR04E_I2C EV5 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT) && I2C_TimeOut-->0)  // EV5 
  {
		
  }

  // Send M24LR04E-R slave address for write 
  I2C_Send7bitAddress(M24LR04E_I2C, EE_address , I2C_Direction_Transmitter);

  // Test on EV6 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)&& I2C_TimeOut-->0) // EV6 
  {
  }

  // Send Address of first byte to be read & wait event detection 
	I2C_SendData(M24LR04E_I2C,(uint8_t)(ReadAddr >> 8)); // MSB 
	// Send the temperature register data pointer 
  //I2C_SendData(M24LR04E_I2C, M24LR04E_REG_TEMP);

  // Test on M24LR04E_I2C EV8 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)&& I2C_TimeOut-->0) // EV8 
  {
  }
	
	// Send Address of first byte to be read & wait event detection 
	I2C_SendData(M24LR04E_I2C,(uint8_t)ReadAddr); // LSB 
	
  // Test on M24LR04E_I2C EV8 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)&& I2C_TimeOut-->0) // EV8 
  {
  }

  //-------------------------------- Reception Phase --------------------------
  // Send Re-START condition 
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  // Test on EV5 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT)&& I2C_TimeOut-->0)  // EV5 
  {
  }

  // Send STLM75 slave address for read 
  I2C_Send7bitAddress(M24LR04E_I2C, EE_address, I2C_Direction_Receiver);

  // Test on EV6 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)&& I2C_TimeOut-->0)  // EV6 
  {
  }

  // Test on EV7 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)&& I2C_TimeOut-->0)  // EV7 
  {
  }

  // Store M24LR04E_I2C received data 
  *pBuffer = I2C_ReceiveData(M24LR04E_I2C);

  // Disable M24LR04E_I2C acknowledgement 
  I2C_AcknowledgeConfig(M24LR04E_I2C, DISABLE);

  // Send M24LR04E_I2C STOP Condition 
  I2C_GenerateSTOP(M24LR04E_I2C, ENABLE);

  // Test on RXNE flag 
  while (I2C_GetFlagStatus(M24LR04E_I2C, I2C_FLAG_RXNE) == RESET && I2C_TimeOut-->0)
  {}

}

/**
  * @brief  Write to the configuration register of the M24LR16E.
  * @param  RegValue: sepecifies the value to be written to M24LR16E configuration
  *         register.
  * @retval None
  */
void M24LR04E_WriteOneByte (uint8_t EE_address, uint16_t ReadAddr,uint8_t pBuffer)
{
int32_t I2C_TimeOut = I2C_TIMEOUT;
  /*-------------------------------- Transmission Phase -----------------------*/
  /* Send M24LR04E_I2C START condition */
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  /* Test on M24LR04E_I2C EV5 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT)&& I2C_TimeOut-->0)  /* EV5 */
  {
  }

  /* Send STLM75 slave address for write */
  I2C_Send7bitAddress(M24LR04E_I2C, EE_address, I2C_Direction_Transmitter);

  /* Test on M24LR04E_I2C EV6 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)&& I2C_TimeOut-->0) /* EV6 */
  {
  }
	
	// Send Address of first byte to be read & wait event detection 
	I2C_SendData(M24LR04E_I2C,(uint8_t)(ReadAddr >> 8)); // MSB 

  // Test on M24LR04E_I2C EV8 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)&& I2C_TimeOut-->0) // EV8 
  {
  }

	// Send Address of first byte to be read & wait event detection 
	I2C_SendData(M24LR04E_I2C,(uint8_t)ReadAddr); // LSB 
  // Test on M24LR04E_I2C EV8 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)&& I2C_TimeOut-->0) // EV8 
  {
  }
	
  /* Send the configuration register data pointer */
  I2C_SendData(M24LR04E_I2C, pBuffer);

  /* Test on M24LR04E_I2C EV8 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)&& I2C_TimeOut-->0) /* EV8 */
  {
  }

  /* Send M24LR04E_I2C STOP Condition */
  I2C_GenerateSTOP(M24LR04E_I2C, ENABLE);
}



/**
  * @brief this function reads a block of data from the M24LR16E EEPROM . 
  * @param[in] pBuffer pointer to the buffer that receives the data read
  * from the EEPROM.
  * @param[in] WriteAddr EEPROM's internal address to read from.
  * @param[in] NumByteToWrite EEPROM's number of bytes to read from the EEPROM.  
  * @retval None
  * @par Required preconditions:
  * None
  */

void M24LR04E_ReadBuffer (uint8_t EE_address, uint16_t ReadAddr, uint8_t NumByteToRead,uint8_t* pBuffer)
{
int32_t I2C_TimeOut = I2C_TIMEOUT;


  // Enable M24LR04E_I2C acknowledgement if it is already disabled by other function 
  I2C_AcknowledgeConfig(M24LR04E_I2C, ENABLE);

  //------------------------------------- Transmission Phase ------------------
  // Send M24LR04E_I2C START condition 
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  // Test on M24LR04E_I2C EV5 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT)&& I2C_TimeOut-->0)  // EV5 
  {
  }

  //* Send STLM75 slave address for write 
  I2C_Send7bitAddress(M24LR04E_I2C, EE_address , I2C_Direction_Transmitter);

  // Test on M24LR04E_I2C EV6 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)&& I2C_TimeOut-->0) // EV6 
  {
  }

  // Send Address of first byte to be read & wait event detection 
	I2C_SendData(M24LR04E_I2C,(uint8_t)(ReadAddr >> 8)); // MSB 
	// Send the temperature register data pointer 
  //I2C_SendData(M24LR04E_I2C, M24LR04E_REG_TEMP);

  // Test on M24LR04E_I2C EV8 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)&& I2C_TimeOut-->0) // EV8 
  {
  }
	
	// Send Address of first byte to be read & wait event detection 
	I2C_SendData(M24LR04E_I2C,(uint8_t)ReadAddr); // LSB 
	
  // Test on M24LR04E_I2C EV8 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)&& I2C_TimeOut-->0) // EV8 
  {
  }

  //-------------------------------- Reception Phase --------------------------
  // Send Re-STRAT condition 
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  // Test on EV5 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT)&& I2C_TimeOut-->0)  // EV5 
  {
  }

  // Send M24LR16E slave address for read 
  I2C_Send7bitAddress(M24LR04E_I2C, EE_address, I2C_Direction_Receiver);

  // Test on EV6 and clear it 
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)&& I2C_TimeOut-->0)  // EV6 
  {
  }


	// While there is data to be read 
  while(NumByteToRead )  
  {

		// Test on EV7 and clear it 
		while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)&& I2C_TimeOut-->0)  
		{
		}
		
		// Store M24LR04E_I2C received data 
		*pBuffer = I2C_ReceiveData(M24LR04E_I2C);
		
		
		if(NumByteToRead == 1)
    {
			// Disable M24LR04E_I2C acknowledgement 
			I2C_AcknowledgeConfig(M24LR04E_I2C, DISABLE);
		}
		
		
		// Point to the next location where the byte read will be saved 
    pBuffer++; 
	  // Decrement the read bytes counter 
    NumByteToRead--; 
		
    if(NumByteToRead == 0)
    {
			// Send M24LR04E_I2C STOP Condition 
			I2C_GenerateSTOP(M24LR04E_I2C, ENABLE);
		}
	
	}
	
	// Store M24LR04E_I2C received data 
	*pBuffer = I2C_ReceiveData(M24LR04E_I2C);
  // Test on RXNE flag 
  while (I2C_GetFlagStatus(M24LR04E_I2C, I2C_FLAG_RXNE) == RESET&& I2C_TimeOut-->0)
  {}

}

/**
  * @brief  Read Temperature register of M24LR16E: double temperature value.
  * @param  None
  * @retval M24LR16E measured temperature value.
  */
uint16_t M24LR04E_ReadTemp(void)
{
  __IO uint16_t RegValue = 0;

  /* Enable M24LR04E_I2C acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(M24LR04E_I2C, ENABLE);

  /*------------------------------------- Transmission Phase ------------------*/
  /* Send M24LR04E_I2C START condition */
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  /* Test on M24LR04E_I2C EV5 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for write */
  I2C_Send7bitAddress(M24LR04E_I2C, M24LR04E_ADDR, I2C_Direction_Transmitter);

  /* Test on M24LR04E_I2C EV6 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
  {
  }

  /* Send the temperature register data pointer */
  I2C_SendData(M24LR04E_I2C, M24LR04E_REG_TEMP);

  /* Test on M24LR04E_I2C EV8 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /*-------------------------------- Reception Phase --------------------------*/
  /* Send Re-STRAT condition */
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for read */
  I2C_Send7bitAddress(M24LR04E_I2C, M24LR04E_ADDR, I2C_Direction_Receiver);

  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* EV6 */
  {
  }

  /* Test on EV7 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))  /* EV7 */
  {
  }

  /* Store M24LR04E_I2C received data */
  RegValue = I2C_ReceiveData(M24LR04E_I2C) << 8;

  /* Disable M24LR04E_I2C acknowledgement */
  I2C_AcknowledgeConfig(M24LR04E_I2C, DISABLE);

  /* Send M24LR04E_I2C STOP Condition */
  I2C_GenerateSTOP(M24LR04E_I2C, ENABLE);

  /* Test on RXNE flag */
  while (I2C_GetFlagStatus(M24LR04E_I2C, I2C_FLAG_RXNE) == RESET)
  {}

  /* Store M24LR04E_I2C received data */
  RegValue |= I2C_ReceiveData(M24LR04E_I2C);

  /* Return Temperature value */
  return (RegValue >> 7);
}

/**
  * @brief  Read the configuration register from the M24LR16E.
  * @param  None
  * @retval M24LR16E configuration register value.
  */
uint8_t M24LR04E_ReadConfReg(void)
{
  __IO uint8_t RegValue = 0;

  /* Enable M24LR04E_I2C acknowledgement if it is already disabled by other function */
  I2C_AcknowledgeConfig(M24LR04E_I2C, ENABLE);
  /*----------------------------- Transmission Phase --------------------------*/
  /* Send M24LR04E_I2C START condition */
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  /* Test on M24LR04E_I2C EV5 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for write */
  I2C_Send7bitAddress(M24LR04E_I2C, M24LR04E_ADDR, I2C_Direction_Transmitter);

  /* Test on M24LR04E_I2C EV6 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
  {
  }

  /* Send the configuration register data pointer */
  I2C_SendData(M24LR04E_I2C, M24LR04E_REG_CONF);

  /* Test on M24LR04E_I2C EV8 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /*----------------------------- Reception Phase -----------------------------*/
  /* Send Re-STRAT condition */
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  /* Test on EV5 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for read */
  I2C_Send7bitAddress(M24LR04E_I2C, M24LR04E_ADDR, I2C_Direction_Receiver);

  /* Test on EV6 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))  /* EV6 */
  {
  }

  /* Disable M24LR04E_I2C acknowledgement */
  I2C_AcknowledgeConfig(M24LR04E_I2C, DISABLE);

  /* Send M24LR04E_I2C STOP Condition */
  I2C_GenerateSTOP(M24LR04E_I2C, ENABLE);

  /* Test on RXNE flag */
  while (I2C_GetFlagStatus(M24LR04E_I2C, I2C_FLAG_RXNE) == RESET);

  /* Store M24LR04E_I2C received data */
  RegValue = I2C_ReceiveData(M24LR04E_I2C);

  /* Return configuration register value */
  return (RegValue);
}

/**
  * @brief  Write to the configuration register of the M24LR16E.
  * @param  RegValue: sepecifies the value to be written to M24LR16E configuration
  *         register.
  * @retval None
  */
void M24LR04E_WriteConfReg(uint8_t RegValue)
{
  /*-------------------------------- Transmission Phase -----------------------*/
  /* Send M24LR04E_I2C START condition */
  I2C_GenerateSTART(M24LR04E_I2C, ENABLE);

  /* Test on M24LR04E_I2C EV5 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_MODE_SELECT))  /* EV5 */
  {
  }

  /* Send STLM75 slave address for write */
  I2C_Send7bitAddress(M24LR04E_I2C, M24LR04E_ADDR, I2C_Direction_Transmitter);

  /* Test on M24LR04E_I2C EV6 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) /* EV6 */
  {
  }

  /* Send the configuration register data pointer */
  I2C_SendData(M24LR04E_I2C, M24LR04E_REG_CONF);

  /* Test on M24LR04E_I2C EV8 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /* Send M24LR04E_I2C data */
  I2C_SendData(M24LR04E_I2C, RegValue);

  /* Test on M24LR04E_I2C EV8 and clear it */
  while (!I2C_CheckEvent(M24LR04E_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) /* EV8 */
  {
  }

  /* Send M24LR04E_I2C STOP Condition */
  I2C_GenerateSTOP(M24LR04E_I2C, ENABLE);
}

/**
  * @brief  DeInitializes the M24LR04E_I2C.
  * @param  None
  * @retval None
  */
static void M24LR04E_LowLevel_DeInit(void)
{


	
  /*!< Disable M24LR04E_I2C */
  I2C_Cmd(M24LR04E_I2C, DISABLE);
  /*!< DeInitializes the M24LR04E_I2C */
  I2C_DeInit(M24LR04E_I2C);







}

/**
  * @brief  Initializes the M24LR04E_I2C..
  * @param  None
  * @retval None
  */
static void M24LR04E_LowLevel_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;


  /*I2C Peripheral clock enable */
  RCC_APB1PeriphClockCmd(M24LR04E_I2C_CLK, ENABLE);
  
  /*SDA GPIO clock enable */
  RCC_AHBPeriphClockCmd(M24LR04E_I2C_CLK_SDA_GPIO_CLK , ENABLE);
  
  /*SCL GPIO clock enable */
  RCC_AHBPeriphClockCmd(M24LR04E_I2C_CLK_SCL_GPIO_CLK, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Reset I2Cx IP */
  RCC_APB1PeriphResetCmd(M24LR04E_I2C_CLK, ENABLE);
  
  /* Release reset signal of I2Cx IP */
  RCC_APB1PeriphResetCmd(M24LR04E_I2C_CLK, DISABLE);


  /*!< Configure M24LR04E_I2C pins: SCL */
  GPIO_InitStructure.GPIO_Pin = M24LR04E_I2C_SCL_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(M24LR04E_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);  


  /*!< Configure M24LR04E_I2C pins: SDA */
  GPIO_InitStructure.GPIO_Pin = M24LR04E_I2C_SDA_PIN;
  GPIO_Init(M24LR04E_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);  

  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(M24LR04E_I2C_SCL_GPIO_PORT, M24LR04E_I2C_SCL_SOURCE, M24LR04E_I2C_SCL_AF);
  
  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(M24LR04E_I2C_SDA_GPIO_PORT, M24LR04E_I2C_SDA_SOURCE, M24LR04E_I2C_SDA_AF);
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

/**
  * @}
  */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
