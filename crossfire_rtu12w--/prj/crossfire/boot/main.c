

/** @addtogroup rtu_boot
  * @{
  */

/* Includes ------------------------------------------------------------------*/


#include "xtsys_type.h"
#include "xtsys_cfg.h"
#include "stdio.h"
#include "stdint.h"
#include "stm32l1xx.h"
#include "discover_board.h"
#include "i2c_m24lr04e-r.h"
#include "xtsys_type.h"
#include "xt_bsp_rtu09c.h"
#include "global_para.h"
#include "ymodem.h"
#include "common.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

#define APPLICATION0_ADDRESS   (uint32_t)0x08002000 
#define APPLICATION1_ADDRESS   (uint32_t)0x08011000 

uint8_t tab_1024[1024] =
  {
    0
  };
uint8_t FileName[FILE_NAME_LENGTH];

void SerialDownload(uint8_t *pLocal)
{
  uint8_t Number[10] = "          ";
  int32_t Size = 0;

  //FLASH_If_Init();
  SerialPutString("Waiting for the file to be sent ... (press 'a' to abort)\n\r");
  Size = Ymodem_Receive(&tab_1024[0], pLocal);
  if (Size > 0)
  {
    SerialPutString("\n\n\r Programming Completed Successfully!\n\r--------------------------------\r\n Name: ");
    SerialPutString(FileName);
    Int2Str(Number, Size);
    SerialPutString("\n\r Size: ");
    SerialPutString(Number);
    SerialPutString(" Bytes\r\n");
    SerialPutString("-------------------\n");
  }
  else if (Size == -1)
  {
    SerialPutString("\n\n\rThe image size is higher than the allowed space memory!\n\r");
  }
  else if (Size == -2)
  {
    SerialPutString("\n\n\rVerification failed!\n\r");
  }
  else if (Size == -3)
  {
    SerialPutString("\r\n\nAborted by user.\n\r");
  }
  else
  {
    SerialPutString("\n\rFailed to receive the file!\n\r");
  }
}

/* Private functions ---------------------------------------------------------*/



#define FLASH_APP_ADDR	0x08040000
RCC_ClocksTypeDef RCC_Clocks;

void Sramtest(void)
{
	#define T_SRAM_ADDRESS		0x60000000
	#define T_SRAM_SIZE			0x80000


	int i;
	unsigned char* p;
	int j;
													
	for(j=0;j<10000;j++)
	{
		for(i=0;i<T_SRAM_SIZE;i++)
		{
			p=(unsigned char*)(T_SRAM_ADDRESS +i);
			*p=i+j;
		}
	
		for(i=0;i<T_SRAM_SIZE;i++)
		{
			p=(unsigned char*)(T_SRAM_ADDRESS +i);
			if(*p!=((i+j)&0xff))
			{
				  while (1)
				  {}
	
			}
		}
	}
}

void RCC_Configuration(void)
{  

  /* Enable HSI Clock */
  RCC_HSICmd(ENABLE);
  
  /*!< Wait till HSI is ready */
  while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET)
  {}

  RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
  
  RCC_MSIRangeConfig(RCC_MSIRange_6);
		
  /* Enable the GPIOs clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC| RCC_AHBPeriph_GPIOD| RCC_AHBPeriph_GPIOE| RCC_AHBPeriph_GPIOH, ENABLE);     

  RCC_HSEConfig(RCC_HSE_OFF);  
  if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET )
  {
    while(1);
  }

}

void  Init_GPIOs (void)
{
  /* GPIO, EXTI and NVIC Init structure declaration */
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Configure User Button pin as input */
  GPIO_InitStructure.GPIO_Pin = USERBUTTON_GPIO_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(USERBUTTON_GPIO_PORT, &GPIO_InitStructure);

}  


unsigned char getAppAddrFromNFC(void)
{
	unsigned char rlt = 0;
	uint8_t OneByte = 0x00;
	uint16_t ReadAddr = 0x0010;	
	M24LR04E_Init();
	

	
	// check the E1 at address 0
	M24LR04E_ReadOneByte (M24LR16_EEPROM_ADDRESS_USER_READ, ReadAddr, &OneByte);		
	
	if(OneByte & 0x01)
		rlt = 1;	
	
	return rlt;
}


void runtoApp(unsigned int aadd)
{
      /* Jump to user application */
      JumpAddress = *(__IO uint32_t*) (aadd + 4);
      Jump_To_Application = (pFunction) JumpAddress;
      /* Initialize user application's Stack Pointer */
      __set_MSP(*(__IO uint32_t*) aadd);
      Jump_To_Application();

}

//-------------------------FLASH--------------------------------------
#define DATA_EEPROM_START_ADDR     0x08080100
uint32_t STM_EVAL_FlashEepromWriteByte(uint16_t Addr,uint8_t *Buffer,uint16_t Length)
{
	uint32_t j,FLASHStatus,Address=DATA_EEPROM_START_ADDR+Addr;

	DATA_EEPROM_Unlock();	
	for(j=0;j<Length;j++)
	{
		FLASHStatus=DATA_EEPROM_FastProgramByte(Address, Buffer[j]);
		//PrintfXTOS("!%d %d %0.2x\n",j,FLASHStatus,Buffer[j]);
		if(FLASHStatus == FLASH_COMPLETE)
		{
		  Address++;
		}
		else
		{
		  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
		                       | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
		}				
	}	
	DATA_EEPROM_Lock();
	return 0;	
	
}

uint32_t STM_EVAL_FlashEepromReadByte(uint16_t Addr,uint8_t *Buffer,uint16_t Length)
{
	uint32_t j=0,Address=DATA_EEPROM_START_ADDR+Addr;
	
	DATA_EEPROM_Unlock();	
	for(j=0;j<Length;j++)
	{
		while(FLASH_GetStatus()==FLASH_BUSY);
		Buffer[j]=*(__IO uint8_t*)Address;
		Address = Address ++;

	}
	DATA_EEPROM_Lock();
	return 0;
}

uint8_t Main_checkKey(void)
{
	return CkeckKeyDown();
}

/**
  * @brief  Initialize the IAP: Configure USART.
  * @param  None
  * @retval None
  */
void IAP_Init(void)
{
 USART_InitTypeDef USART_InitStructure;

  /* USART resources configuration (Clock, GPIO pins and USART registers) ----*/
  /* USART configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

  STM_EVAL_COMInit(COM1, &USART_InitStructure);

  //USART_ClearFlag(COM1, USART_FLAG_TC);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	unsigned char appStartPoint=0 ; //0=app0      not 0=app1
	unsigned char externInput=0 ;
//	rtu_update_para_t updateParam = {0};
	unsigned char appFlag0 = 0;
	unsigned char appFlag1 = 0;
	uint8_t local = 0xff;
	
	//Sramtest();	
  
	/* Configure Clocks for Application need */
	RCC_Configuration();

#if 0
	/* Init I/O ports */	
	Init_GPIOs();


	appStartPoint=getAppAddrFromNFC();

	if(((*(__IO uint32_t*)APPLICATION0_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
	{
		if (((*(__IO uint32_t*)APPLICATION1_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
		{
			if((USERBUTTON_GPIO_PORT->IDR & USERBUTTON_GPIO_PIN) != 0x00)
				appStartPoint=!appStartPoint;
			

		}
		else//app1 invalid
		{
			appStartPoint=0; //force use app0

		}

	}
	else //app0 invalid
	{
		if (((*(__IO uint32_t*)APPLICATION1_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
		{
			appStartPoint=1; //force use app1
		}
		else//app1 invalid
		{
			appStartPoint=0; //force use app0
		}


	}
#endif

	IAP_Init();
/*
	if(0 != Main_checkKey())
	{   
		SerialDownload(&local);
		if(local == 0 || local == 1)
		{
			STM_EVAL_FlashEepromReadByte(IFLASH_UPDATE_PARAM_START_ADDR_OFFSET, (uint8_t *)&updateParam, sizeof(updateParam));
			updateParam.bootFlag = local;
			STM_EVAL_FlashEepromWriteByte(IFLASH_UPDATE_PARAM_START_ADDR_OFFSET, (uint8_t *)&updateParam, sizeof(updateParam));
		}
	}
*/
	if(1)
	{
		//nfc is invalid
/*
		if(((*(__IO uint32_t*)APPLICATION0_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
		{
			appFlag0 = 1;
		}
		
		if(((*(__IO uint32_t*)APPLICATION1_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
		{
			appFlag1 = 1;
		}
		
		STM_EVAL_FlashEepromReadByte(IFLASH_UPDATE_PARAM_START_ADDR_OFFSET, (uint8_t *)&updateParam, sizeof(updateParam));

		if(updateParam.bootFlag == 0)
		{
			//software flag 0
			if(appFlag0 == 1)
			{
				//app 0 valid
				appStartPoint = 0;
			}
			else
			{
				//app 0 invalid
				if(appFlag1 == 1)
					appStartPoint = 1;
			}
		}
		else if(updateParam.bootFlag == 1)
		{
			//software flag 1
			if(appFlag1 == 1)
			{
				//app 1 valid
				appStartPoint = 1;
			}
			else
			{
				//app 1 invalid
				if(appFlag0 == 1)
					appStartPoint = 0;
			}
		}
	}
	else
	{
		//nfc is valid*/
	}
	
	if(appStartPoint==0)
		runtoApp(APPLICATION0_ADDRESS);
	else
		runtoApp(APPLICATION1_ADDRESS);

	while (1)
	{}
}


#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
