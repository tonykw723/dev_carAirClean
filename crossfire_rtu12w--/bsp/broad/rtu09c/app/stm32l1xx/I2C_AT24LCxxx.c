/**
  ******************************************************************************
  * @file    I2C_AT24LCxxx-R.c
  * @author TONY Application Team
  * @version V1.0.0
  * @date    17-4-2014
  * @brief  
  *
  *     +-----------------------------------------------------------------+
  *     |                        Pin assignment                           |
  *     +---------------------------------------+-----------+-------------+
  *     |  STM32 I2C Pins                       |   AT24LCxxx  |   Pin       |
  *     +---------------------------------------+-----------+-------------+
  *     +---------------------------------------+-----------+-------------+
  ******************************************************************************
  * @attention
  *
  ******************************************************************************


/* Includes ------------------------------------------------------------------*/
#include "I2C_AT24LCxxx.h"
#include "stm32l1xx_i2c.h"
#include "xtsys_type.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
//#define USE_STLIB
#ifndef USE_STLIB
#define EE_SCL_SET() STM_EVAL_GPIOOff(GPIO_I2C_SCL)//GPIO_SetBits(AT24LCxxx_I2C_SCL_GPIO_PORT, AT24LCxxx_I2C_SCL_PIN)
#define EE_SCL_CLR() STM_EVAL_GPIOOn(GPIO_I2C_SCL)//GPIO_ResetBits(AT24LCxxx_I2C_SCL_GPIO_PORT, AT24LCxxx_I2C_SCL_PIN)
#define EE_SDA_SET() STM_EVAL_GPIOOff(GPIO_I2C_SDA)//GPIO_SetBits(AT24LCxxx_I2C_SDA_GPIO_PORT,AT24LCxxx_I2C_SDA_PIN)
#define EE_SDA_CLR() STM_EVAL_GPIOOn(GPIO_I2C_SDA)//GPIO_ResetBits(AT24LCxxx_I2C_SDA_GPIO_PORT,AT24LCxxx_I2C_SDA_PIN)
#define EE_SDA_RD() GPIO_ReadInputDataBit(AT24LCxxx_I2C_SDA_GPIO_PORT, AT24LCxxx_I2C_SDA_PIN)
#define EE_DEALY() delay_us(1);
void delay_us(xt_u32 us)
{
	xt_u32 i;
	for(i=0;i<us*3+us/2;i++);
}



void EE_start(void) //起始信号
{
	EE_DEALY();
       EE_SDA_SET();
	EE_DEALY();
       EE_SCL_SET();
       EE_DEALY();
       EE_SDA_CLR();
       EE_DEALY();
       EE_SCL_CLR();	  
}

void EE_stop(void) //停止信号
{
	EE_DEALY();	
       EE_SDA_CLR();
	EE_DEALY();	
       EE_SCL_SET();
       EE_DEALY();	
       EE_SDA_SET();
	EE_DEALY();	 
}

xt_u8 EE_waitack(void)//等待应答信号
{
	xt_u16 i=0;
	xt_u8 status=0;
	static xt_u8 ccc=0;
	
	EE_SDA_SET();////
	EE_DEALY();
       EE_SCL_SET();

	
       while(EE_SDA_RD()==1)
	{
		if(i++>500)
		{
			status =1;
			ccc++;
			break;
		}	
	}
	if((i<500)||(EE_SDA_RD()==0))
	{
		status=0;
		ccc=0;
	}
	EE_DEALY();	
       EE_SCL_CLR();
	return status;	
}

void EE_noack(void) //非应答信号
{
	  EE_SDA_SET();	  //sda=1
	  EE_DEALY();			    //延时5us
	  EE_SCL_SET();	  //scl=1
	  EE_DEALY();			    //延时5us
	  EE_SCL_CLR();	  //scl=0
	  EE_DEALY();			    //延时5us
	  EE_SDA_CLR();	 //sda=0
	  EE_DEALY();
}

void EE_ack(void)	  //应答信号
{ 
	  EE_SDA_CLR();	 //sda=0
	  EE_DEALY();			    //延时5us
	  EE_SCL_SET();	  //scl=1
	  EE_DEALY(); 			    //延时5us
	  EE_SCL_CLR();	  //scl=0
	  EE_DEALY();				 //延时5us
	  EE_SDA_SET();	  //sda=1
	  EE_DEALY();

}


void EE_write8bit(xt_u8 input)	 //写一个字节数据
{
    unsigned char i;
    for(i=0;i<8;i++)
    {
        if((input&0x80)>0)
        {
		EE_DEALY();
              EE_SDA_SET();
		EE_DEALY();
              EE_SCL_SET();
              EE_DEALY();
              EE_SCL_CLR();
        }
        else
        {
		EE_DEALY();
              EE_SDA_CLR();
		EE_DEALY();
              EE_SCL_SET();
              EE_DEALY();
              EE_SCL_CLR();
        }
    	input=input<<1;
    }	  
}
xt_u8 EE_read8bit(void)		   //读一个字节数据
{
    unsigned char num=0xff,mm=0x01,uu=0xfe;
    unsigned char j;
	//EE_SDA_SET();
    for(j=0;j<8;j++)
    {
        EE_DEALY();
        EE_SCL_SET();
	 EE_DEALY();
		num<<=1;
        if(EE_SDA_RD()==0)
        	num=(num&uu);
        else
	{
        	num=(num|mm);
	}		
        EE_SCL_CLR();
	 EE_DEALY();
    }

    return(num);
}

#endif

void AT24LCxxx_DeInit(void)
{
  AT24LCxxx_LowLevel_DeInit();
}

/*****************************************************************
  * @brief  Initializes the AT24LCxxx_I2C.
  * @param  None
  * @retval None
  ****************************************************************/
void AT24LCxxx_Init(void)
{
   #ifdef USE_STLIB
  I2C_InitTypeDef  I2C_InitStructure;	
  AT24LCxxx_LowLevel_Init(); 
  /*I2C Struct Initialize */
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = AT24LCxxx_ADDR;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_ClockSpeed = AT24LCxxx_I2C_SPEED;
  
#ifndef I2C_10BITS_ADDRESS
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
#else
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_10bit;
#endif /* I2C_10BITS_ADDRESS */
  
  /*I2C Initialize */
  I2C_Init(AT24LCxxx_I2C, &I2C_InitStructure);
  


  /*!< AT24LCxxx_I2C Init */
  I2C_Cmd(AT24LCxxx_I2C, ENABLE);
  #else
/* GPIO Init structure declaration */
  GPIO_InitTypeDef GPIO_InitStructure;	
  AT24LCxxx_LowLevel_DeInit();
    /*SDA SCL GPIO clock enable */
  RCC_AHBPeriphClockCmd(AT24LCxxx_I2C_CLK_SDA_GPIO_CLK , ENABLE);
  
  /*!< Configure AT24LCxxx_I2C pins: SCL SDA */
  GPIO_InitStructure.GPIO_Pin = AT24LCxxx_I2C_SCL_PIN|AT24LCxxx_I2C_SDA_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(AT24LCxxx_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);  
  /*
  //GPIO_SetBits(AT24LCxxx_I2C_SCL_GPIO_PORT, AT24LCxxx_I2C_SCL_PIN);
  STM_EVAL_GPIOOn(GPIO_I2C_SCL);
  EE_SCL_SET();
  EE_SCL_CLR();
  delay_us(100);
  EE_SCL_SET();
  delay_us(100);
  EE_SCL_CLR();
  delay_us(50);
  EE_SCL_SET();
  delay_us(50);
  EE_SCL_CLR();
  delay_us(25);
  EE_SCL_SET();
  delay_us(25);
  EE_SCL_CLR();
  delay_us(10);
  EE_SCL_SET();
  delay_us(10);  
  EE_SCL_CLR();
  delay_us(5);
  EE_SCL_SET();
  delay_us(5);    
  
  EE_SDA_SET();*/
  #endif  
}



/***********************************************************************************
  * @brief this function reads one byte of data from the AT24LCxxx EEPROM . 
  * @param[in] pBuffer pointer to the buffer that receives the data read
  * from the EEPROM.
  * @param[in] WriteAddr EEPROM's internal address to read from.
  * @param[in] NumByteToWrite EEPROM's number of bytes to read from the EEPROM.  
  * @retval None
  * @par Required preconditions:
  * None
  ***********************************************************************************/
xt_u8 AT24LCxxx_ReadOneByte (xt_u8 EE_address, uint16_t ReadAddr,xt_u8* pBuffer)
{
   #ifdef USE_STLIB
  xt_u32 I2C_TimeOut = I2C_TIMEOUT;
  xt_u8 retErr=0;
  // Enable AT24LCxxx_I2C EE_acknowledgement if it is already disabled by other function 
  I2C_AcknowledgeConfig(AT24LCxxx_I2C, ENABLE);
  
  while(I2C_GetFlagStatus(AT24LCxxx_I2C, I2C_FLAG_BUSY))
  {
    	if((I2C_TimeOut--) == 0) 
	{
		PrintfXTOS("busy\n");
		I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
		 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
		return 1;
	}
  }
  
  //------------------------------------- Transmission Phase ------------------
  // Send AT24LCxxx_I2C EE_start condition 
  I2C_GenerateSTART(AT24LCxxx_I2C, ENABLE);
  I2C_TimeOut = I2C_TIMEOUT;
	  // Test on AT24LCxxx_I2C EV5 and clear it 
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_MODE_SELECT)) && (I2C_TimeOut>0))  // EV5 
  {
	I2C_TimeOut--;		
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e1\n");
	retErr|=1;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
  // Send AT24LCxxx-R slave address for write 
  I2C_Send7bitAddress(AT24LCxxx_I2C, EE_address , I2C_Direction_Transmitter);

  // Test on EV6 and clear it 
  I2C_TimeOut = I2C_TIMEOUT;
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&& (I2C_TimeOut>0)) // EV6 
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e2\n");
	retErr|=2;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
  // Send Address of first byte to be read & wait event detection 
	I2C_SendData(AT24LCxxx_I2C,(xt_u8)(ReadAddr >> 8)); // MSB 
	// Send the temperature register data pointer 
  //I2C_SendData(AT24LCxxx_I2C, AT24LCxxx_REG_TEMP);

  // Test on AT24LCxxx_I2C EV8 and clear it 
  I2C_TimeOut = I2C_TIMEOUT;
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&& (I2C_TimeOut>0)) // EV8 
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e3\n");
	retErr|=4;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
	// Send Address of first byte to be read & wait event detection 
	I2C_SendData(AT24LCxxx_I2C,(xt_u8)ReadAddr); // LSB 
	
  // Test on AT24LCxxx_I2C EV8 and clear it 
  I2C_TimeOut = I2C_TIMEOUT;
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&&(I2C_TimeOut>0)) // EV8 
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e4\n");
	retErr|=8;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
  //-------------------------------- Reception Phase --------------------------
  // Send Re-EE_start condition 
  I2C_GenerateSTART(AT24LCxxx_I2C, ENABLE);

  // Test on EV5 and clear it 
  I2C_TimeOut = I2C_TIMEOUT;
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_MODE_SELECT))&&(I2C_TimeOut>0))  // EV5 
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e5\n");
	retErr|=0x10;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
  // Send STLM75 slave address for read 
  I2C_Send7bitAddress(AT24LCxxx_I2C, EE_address, I2C_Direction_Receiver);

  // Test on EV6 and clear it 
  I2C_TimeOut = I2C_TIMEOUT;
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))&&(I2C_TimeOut>0))  // EV6 
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e6\n");
	retErr|=0x20;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
  // Test on EV7 and clear it 
  I2C_TimeOut = I2C_TIMEOUT;
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED))&& (I2C_TimeOut>0))  // EV7 
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e7\n");
	retErr|=0x40;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
  // Store AT24LCxxx_I2C received data 
  *pBuffer = I2C_ReceiveData(AT24LCxxx_I2C);

  // Disable AT24LCxxx_I2C EE_acknowledgement 
  I2C_AcknowledgeConfig(AT24LCxxx_I2C, DISABLE);

  // Send AT24LCxxx_I2C EE_stop Condition 
  I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
  I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);

  // Test on RXNE flag 
  I2C_TimeOut = I2C_TIMEOUT;
  while (I2C_GetFlagStatus(AT24LCxxx_I2C, I2C_FLAG_RXNE) == RESET &&(I2C_TimeOut>0))
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e8\n");
	retErr|=0x80;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
  return retErr;
  #else
	xt_u16 i=0;
	EE_start();
	EE_write8bit(AT24LCxxx_EEPROM_ADDRESS_USER_WRITE);
	if(EE_waitack()==1)
	{
		EE_stop();
		return 1;
	}
	i=0;
	EE_write8bit(ReadAddr>>8);
	if(EE_waitack()==1)
	{
		EE_stop();
		return 2;
	}
	i=0;
	EE_write8bit(ReadAddr);
	if(EE_waitack()==1)
	{
		EE_stop();
		return 3;
	}
	EE_start();
	EE_write8bit(AT24LCxxx_EEPROM_ADDRESS_USER_READ);
	i=0;
	if(EE_waitack()==1)
	{
		EE_stop();
		return 4;
	}
	*pBuffer=EE_read8bit();
	EE_noack();
	EE_stop();
	return 	0;
	
  
  #endif  
}



/*********************************************************************************
  * @brief  Write to the configuration register of the AT24LCxxx.
  * @param  RegValue: sepecifies the value to be written to AT24LCxxx configuration
  *         register.
  * @retval None
  *********************************************************************************/
xt_u8 AT24LCxxx_WriteOneByte (xt_u8 EE_address, uint16_t WriteAddr,xt_u8 pBuffer)
{
   #ifdef USE_STLIB
  xt_u32 I2C_TimeOut = I2C_TIMEOUT;
  xt_u8 retErr=0;
  //AT24LCxxx_Init();
  while(I2C_GetFlagStatus(AT24LCxxx_I2C, I2C_FLAG_BUSY))
  {
    	if((I2C_TimeOut--) == 0) 
	{
		PrintfXTOS("busy\n");
		I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
		 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
		return 1;
	}
  }
  
  /*-------------------------------- Transmission Phase -----------------------*/
  /* Send AT24LCxxx_I2C EE_start condition */
  I2C_GenerateSTART(AT24LCxxx_I2C, ENABLE);
  I2C_TimeOut = I2C_TIMEOUT;
  /* Test on AT24LCxxx_I2C EV5 and clear it */
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_MODE_SELECT))&& (I2C_TimeOut>0))  /* EV5 */
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e1\n");
	retErr|=1;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
  /* Send STLM75 slave address for write */
   I2C_TimeOut = I2C_TIMEOUT;
  I2C_Send7bitAddress(AT24LCxxx_I2C, EE_address, I2C_Direction_Transmitter);

  /* Test on AT24LCxxx_I2C EV6 and clear it */
 
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))&&(I2C_TimeOut>0)) /* EV6 */
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e2\n");
	retErr|=2;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }	
	// Send Address of first byte to be read & wait event detection 
	I2C_SendData(AT24LCxxx_I2C,(xt_u8)(WriteAddr >> 8)); // MSB 

  // Test on AT24LCxxx_I2C EV8 and clear it 
  I2C_TimeOut = I2C_TIMEOUT;
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&&(I2C_TimeOut>0)) // EV8 
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e3\n");
	retErr|=4;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
	// Send Address of first byte to be read & wait event detection 
	I2C_SendData(AT24LCxxx_I2C,(xt_u8)WriteAddr); // LSB 
  // Test on AT24LCxxx_I2C EV8 and clear it 
  I2C_TimeOut = I2C_TIMEOUT;
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&&(I2C_TimeOut>0)) // EV8 
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e4\n");
	retErr|=8;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }
  /* Send the configuration register data pointer */
  I2C_SendData(AT24LCxxx_I2C, pBuffer);

  /* Test on AT24LCxxx_I2C EV8 and clear it */
  I2C_TimeOut = I2C_TIMEOUT;
  while ((!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED))&& (I2C_TimeOut>0)) /* EV8 */
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e5\n");
	retErr|=0x10;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }

  // Test on TXE flag 
  I2C_TimeOut = I2C_TIMEOUT;
  while (I2C_GetFlagStatus(AT24LCxxx_I2C, I2C_FLAG_TXE) == RESET &&(I2C_TimeOut>0))
  {
  	I2C_TimeOut--;
  }
  if(I2C_TimeOut==0)
  {
  	PrintfXTOS("e6\n");
	retErr|=0x20;
	I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
	 I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
	return retErr;
  }  
  
  I2C_ClearFlag(AT24LCxxx_I2C,I2C_FLAG_AF);
  /* Send AT24LCxxx_I2C EE_stop Condition */
  I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
  //I2C_Standby_24C();
  return retErr;
  #else
	uint16_t i=0;
	EE_start();
	EE_write8bit(AT24LCxxx_EEPROM_ADDRESS_USER_WRITE);
	if(EE_waitack()==1)
	{
		EE_stop();
		return 1;
	}
	EE_write8bit(WriteAddr>>8);
	i=0;
	if(EE_waitack()==1)
	{
		EE_stop();
		return 2;
	}	
	EE_write8bit(WriteAddr);
	i=0;
	if(EE_waitack()==1)
	{
		EE_stop();
		return 3;
	}
	EE_write8bit(pBuffer);
	EE_noack();

	EE_stop();
	return 0;  
  #endif  
}



/*************************************************************************
  * @brief this function reads a block of data from the AT24LCxxx EEPROM . 
  * @param[in] pBuffer pointer to the buffer that receives the data read
  * from the EEPROM.
  * @param[in] WriteAddr EEPROM's internal address to read from.
  * @param[in] NumByteToWrite EEPROM's number of bytes to read from the EEPROM.  
  * @retval None
  * @par Required preconditions:
  * None
  *************************************************************************/

void AT24LCxxx_ReadBuffer (xt_u8 EE_address, uint16_t ReadAddr, xt_u8 NumByteToRead,xt_u8* pBuffer)
{
xt_u32 I2C_TimeOut = I2C_TIMEOUT;


  // Enable AT24LCxxx_I2C EE_acknowledgement if it is already disabled by other function 
  I2C_AcknowledgeConfig(AT24LCxxx_I2C, ENABLE);

  //------------------------------------- Transmission Phase ------------------
  // Send AT24LCxxx_I2C EE_start condition 
  I2C_GenerateSTART(AT24LCxxx_I2C, ENABLE);

  // Test on AT24LCxxx_I2C EV5 and clear it 
  while (!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_MODE_SELECT)&& I2C_TimeOut-->0)  // EV5 
  {
  }

  //* Send STLM75 slave address for write 
  I2C_Send7bitAddress(AT24LCxxx_I2C, EE_address , I2C_Direction_Transmitter);

  // Test on AT24LCxxx_I2C EV6 and clear it 
  while (!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)&& I2C_TimeOut-->0) // EV6 
  {
  }

  // Send Address of first byte to be read & wait event detection 
	I2C_SendData(AT24LCxxx_I2C,(xt_u8)(ReadAddr >> 8)); // MSB 
	// Send the temperature register data pointer 
  //I2C_SendData(AT24LCxxx_I2C, AT24LCxxx_REG_TEMP);

  // Test on AT24LCxxx_I2C EV8 and clear it 
  while (!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)&& I2C_TimeOut-->0) // EV8 
  {
  }
	
	// Send Address of first byte to be read & wait event detection 
	I2C_SendData(AT24LCxxx_I2C,(xt_u8)ReadAddr); // LSB 
	
  // Test on AT24LCxxx_I2C EV8 and clear it 
  while (!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED)&& I2C_TimeOut-->0) // EV8 
  {
  }

  //-------------------------------- Reception Phase --------------------------
  // Send Re-STRAT condition 
  I2C_GenerateSTART(AT24LCxxx_I2C, ENABLE);

  // Test on EV5 and clear it 
  while (!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_MODE_SELECT)&& I2C_TimeOut-->0)  // EV5 
  {
  }

  // Send AT24LCxxx slave address for read 
  I2C_Send7bitAddress(AT24LCxxx_I2C, EE_address, I2C_Direction_Receiver);

  // Test on EV6 and clear it 
  while (!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)&& I2C_TimeOut-->0)  // EV6 
  {
  }


	// While there is data to be read 
  while(NumByteToRead )  
  {

		// Test on EV7 and clear it 
		while (!I2C_CheckEvent(AT24LCxxx_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED)&& I2C_TimeOut-->0)  
		{
		}
		
		// Store AT24LCxxx_I2C received data 
		*pBuffer = I2C_ReceiveData(AT24LCxxx_I2C);
		
		
		if(NumByteToRead == 1)
    {
			// Disable AT24LCxxx_I2C EE_acknowledgement 
			I2C_AcknowledgeConfig(AT24LCxxx_I2C, DISABLE);
		}
		
		
		// Point to the next location where the byte read will be saved 
    pBuffer++; 
	  // Decrement the read bytes counter 
    NumByteToRead--; 
		
    if(NumByteToRead == 0)
    {
			// Send AT24LCxxx_I2C EE_stop Condition 
			I2C_GenerateSTOP(AT24LCxxx_I2C, ENABLE);
		}
	
	}
	
	// Store AT24LCxxx_I2C received data 
	*pBuffer = I2C_ReceiveData(AT24LCxxx_I2C);
  // Test on RXNE flag 
  while (I2C_GetFlagStatus(AT24LCxxx_I2C, I2C_FLAG_RXNE) == RESET&& I2C_TimeOut-->0)
  {}

}

/**
  * @brief  DeInitializes the AT24LCxxx_I2C.
  * @param  None
  * @retval None
  */
static void AT24LCxxx_LowLevel_DeInit(void)
{	
  /*!< Disable AT24LCxxx_I2C */
  I2C_Cmd(AT24LCxxx_I2C, DISABLE);
  /*!< DeInitializes the AT24LCxxx_I2C */
  I2C_DeInit(AT24LCxxx_I2C); 
}

/**
  * @brief  Initializes the AT24LCxxx_I2C..
  * @param  None
  * @retval None
  */
static void AT24LCxxx_LowLevel_Init(void)
{
  #ifdef USE_STLIB
  GPIO_InitTypeDef  GPIO_InitStructure;


  /*I2C Peripheral clock enable */
  RCC_APB1PeriphClockCmd(AT24LCxxx_I2C_CLK, ENABLE);
  
  /*SDA GPIO clock enable */
  RCC_AHBPeriphClockCmd(AT24LCxxx_I2C_CLK_SDA_GPIO_CLK , ENABLE);
  
  /*SCL GPIO clock enable */
  RCC_AHBPeriphClockCmd(AT24LCxxx_I2C_CLK_SCL_GPIO_CLK, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Reset I2Cx IP */
  RCC_APB1PeriphResetCmd(AT24LCxxx_I2C_CLK, ENABLE);
  
  /* Release reset signal of I2Cx IP */
  RCC_APB1PeriphResetCmd(AT24LCxxx_I2C_CLK, DISABLE);


  /*!< Configure AT24LCxxx_I2C pins: SCL SDA*/
  GPIO_InitStructure.GPIO_Pin = AT24LCxxx_I2C_SCL_PIN|AT24LCxxx_I2C_SDA_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(AT24LCxxx_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);  

  I2C_AcknowledgeConfig(AT24LCxxx_I2C, ENABLE);
  /* Connect PXx to I2C_SCL */
  GPIO_PinAFConfig(AT24LCxxx_I2C_SCL_GPIO_PORT, AT24LCxxx_I2C_SCL_SOURCE, AT24LCxxx_I2C_SCL_AF);
  
  /* Connect PXx to I2C_SDA */
  GPIO_PinAFConfig(AT24LCxxx_I2C_SDA_GPIO_PORT, AT24LCxxx_I2C_SDA_SOURCE, AT24LCxxx_I2C_SDA_AF);
  #else
  #endif
}


/**
  * @}
  */

