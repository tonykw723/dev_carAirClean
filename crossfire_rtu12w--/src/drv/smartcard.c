/**
  ******************************************************************************
  * @file    USART/Smartcard/smartcard.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    13-April-2012
  * @brief   This file provides all the Smartcard firmware functions.
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

/** @addtogroup STM32F2xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup USART_SmartCard
  * @{
  */
  
/* Includes ------------------------------------------------------------------*/
#include "smartcard.h"
#include "xtsys_type.h"
#include "ringqueue.h"
#include "xtos_mem.h"
#include "xtos_task.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables definition and initialization ----------------------------*/
SC_ATR SC_A2R;
uint8_t SC_ATR_Table[80];
static __IO uint8_t SCData = 0;
static uint32_t F_Table[16] = {0, 372, 558, 744, 1116, 1488, 1860, 0,
                               0, 512, 768, 1024, 1536, 2048, 0, 0};
static uint32_t D_Table[8] = {0, 1, 2, 4, 8, 16, 0, 0};

#define SC_UART_RX_BUFFER_SIZE	(1024)
#define SC_UART_POLL_DELAY_MS	(1)
#define SC_UART_POLL_TIMES		(20)

volatile uint32_t scrxcnt = 0;
ringqueue_t *scrxrq;

#define sleep_ms(ticks)  TaskSleepMs(ticks)

/* Private function prototypes -----------------------------------------------*/
/* Transport Layer -----------------------------------------------------------*/
/*--------------APDU-----------*/
void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_ResponceStatus);

/*------------ ATR ------------*/
static void SC_AnswerReq(SC_State *SCState, uint8_t *card, uint8_t length);  /* Ask ATR */
static uint8_t SC_decode_Answer2reset(uint8_t *card);  /* Decode ATR */

/* Physical Port Layer -------------------------------------------------------*/
static void SC_Init(void);
static void SC_DeInit(void);
static void SC_VoltageConfig(uint32_t SC_Voltage);
static uint8_t SC_Detect(void);
static ErrorStatus USART_ByteReceive(uint8_t *Data, uint32_t TimeOut);

/* Private functions ---------------------------------------------------------*/
static xt_s32 scUartReadChar(xt_u8 *pData)
{
	if(FALSE==RQueueReadOutByte(scrxrq, pData))
		return FAILURE_XT;
	return SUCCESS_XT;
}

static xt_s32 SCUartRead(xt_u8* pData, xt_s32 maxDataLen)
{
	xt_s32 ret=0;
	xt_u32 i = 0;

	//USART_ITConfig(SC_USART, USART_IT_RXNE, ENABLE);
	for(i=0;i<maxDataLen;i++)
	{
		int j;

		for(j=0;j<SC_UART_POLL_TIMES;j++)
		{
			if(scUartReadChar(&pData[ret])== SUCCESS_XT)
			{
				ret++;
				break;
			}
			sleep_ms(SC_UART_POLL_DELAY_MS);
		}
		if(j == SC_UART_POLL_TIMES)
			break;		
	}

	//USART_ITConfig(SC_USART, USART_IT_RXNE, DISABLE);
	return ret;	
}

/**
  * @brief  Handles all Smartcard states and serves to send and receive all
  *         communication data between Smartcard and reader.
  * @param  SCState: pointer to an SC_State enumeration that will contain the 
  *         Smartcard state.
  * @param  SC_ADPU: pointer to an SC_ADPU_Commands structure that will be initialized.  
  * @param  SC_Response: pointer to a SC_ADPU_Responce structure which will be initialized.
  * @retval None
  */
void SC_Handler(SC_State *SCState, SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_Response)
{
  uint32_t i = 0;

  switch(*SCState)
  {
    case SC_POWER_ON:
      if (SC_ADPU->Header.INS == SC_GET_A2R)
      {
        /* Smartcard intialization ------------------------------------------*/
        SC_Init();

        /* Reset Data from SC buffer -----------------------------------------*/
        for (i = 0; i < 40; i++)
        {
          SC_ATR_Table[i] = 0;
        }
        
        /* Reset SC_A2R Structure --------------------------------------------*/
        SC_A2R.TS = 0;
        SC_A2R.T0 = 0;
        for (i = 0; i < SETUP_LENGTH; i++)
        {
          SC_A2R.T[i] = 0;
        }
        for (i = 0; i < HIST_LENGTH; i++)
        {
          SC_A2R.H[i] = 0;
        }
        SC_A2R.Tlength = 0;
        SC_A2R.Hlength = 0;
        
        /* Next State --------------------------------------------------------*/
		SC_Reset(Bit_RESET);
        *SCState = SC_RESET_LOW;
      }
    break;

    case SC_RESET_LOW:
      if(SC_ADPU->Header.INS == SC_GET_A2R)
      {
        /* If card is detected then Power ON, Card Reset and wait for an answer) */
        if (SC_Detect())
        {
          while(((*SCState) != SC_POWER_OFF) && ((*SCState) != SC_ACTIVE))
          {
            SC_AnswerReq(SCState, &SC_ATR_Table[0], 80); /* Check for answer to reset */
          }
        }
        else
        {
          (*SCState) = SC_POWER_OFF;
        } 
      }
    break;

    case SC_ACTIVE:
      if (SC_ADPU->Header.INS == SC_GET_A2R)
      {
        if(SC_decode_Answer2reset(&SC_ATR_Table[0]) == T0_PROTOCOL)
        {
          (*SCState) = SC_ACTIVE_ON_T0;
        }
        else
        {
          (*SCState) = SC_POWER_OFF; 
        }
      }
    break;

    case SC_ACTIVE_ON_T0:
      SC_SendData(SC_ADPU, SC_Response);
    break;

    case SC_POWER_OFF:
      SC_DeInit(); /* Disable Smartcard interface */
	  *SCState = SC_POWER_ON;
    break;

    default: (*SCState) = SC_POWER_OFF;
  }
}

/**
  * @brief  Enables or disables the power to the Smartcard.
  * @param  NewState: new state of the Smartcard power supply. 
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void SC_PowerCmd(FunctionalState NewState)
{
#if 0
  if(NewState != DISABLE)
  {
    GPIO_ResetBits(SC_CMDVCC_GPIO_PORT, SC_CMDVCC_PIN);
  }
  else
  {
    GPIO_SetBits(SC_CMDVCC_GPIO_PORT, SC_CMDVCC_PIN);
  } 
#endif
}

/**
  * @brief  Sets or clears the Smartcard reset pin.
  * @param  ResetState: this parameter specifies the state of the Smartcard 
  *         reset pin. BitVal must be one of the BitAction enum values:
  *                 @arg Bit_RESET: to clear the port pin.
  *                 @arg Bit_SET: to set the port pin.
  * @retval None
  */
void SC_Reset(BitAction ResetState)
{
  GPIO_WriteBit(SC_RESET_GPIO_PORT, SC_RESET_PIN, ResetState);
}

/**
  * @brief  Resends the byte that failed to be received (by the Smartcard) correctly.
  * @param  None
  * @retval None
  */
void SC_ParityErrorHandler(void)
{
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  } 
}

/**
  * @brief  Configures the IO speed (BaudRate) communication.
  * @param  None
  * @retval None
  */
void SC_PTSConfig(void)
{
  RCC_ClocksTypeDef RCC_ClocksStatus;
  uint32_t workingbaudrate = 0, apbclock = 0;
  uint8_t locData = 0, PTSConfirmStatus = 1;
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure;
  xt_u8 locRetData[4] = {0};
  xt_s32 ret = 0;
  
/* Reconfigure the USART Baud Rate -------------------------------------------*/
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  apbclock = RCC_ClocksStatus.PCLK2_Frequency;
  apbclock /= ((SC_USART->GTPR & (uint16_t)0x00FF) * 2);

  /* Enable the DMA Receive (Set DMAR bit only) to enable interrupt generation
     in case of a framing error FE */  
  USART_DMACmd(SC_USART, USART_DMAReq_Rx, ENABLE);
  
  if((SC_A2R.T0 & (uint8_t)0x10) == 0x10)
  {
    if(SC_A2R.T[0] != 0x11)
    {
      /* Send PTSS */
      SCData = 0xFF;
      USART_SendData(SC_USART, SCData);
      while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
      {
      }

      /* Send PTS0 */
      SCData = 0x10;
      USART_SendData(SC_USART, SCData);
      while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
      {
      }

      /* Send PTS1 */
      SCData = SC_A2R.T[0]; 
      USART_SendData(SC_USART, SCData);
      while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
      {
      }

      /* Send PCK */
      SCData = (uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SC_A2R.T[0]; 
      USART_SendData(SC_USART, SCData);
      while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
      {
      }

      /* Disable the DMA Receive (Reset DMAR bit only) */  
      USART_DMACmd(SC_USART, USART_DMAReq_Rx, DISABLE);
      
#if 1
	  USART_ITConfig(SC_USART, USART_IT_RXNE, ENABLE);
	  ret = SCUartRead(locRetData, sizeof(locRetData));
	  USART_ITConfig(SC_USART, USART_IT_RXNE, DISABLE);
	  if(ret == sizeof(locRetData))
	  {
		if(locRetData[0] != 0xFF)
		{
		   PTSConfirmStatus = 0x00;
		}

		if(locRetData[1] != 0x10)
		{
		   PTSConfirmStatus = 0x00;
		}
		
		if(locRetData[2] != SC_A2R.T[0])
		{
		   PTSConfirmStatus = 0x00;
		}
		
		if(locRetData[3] != ((uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SC_A2R.T[0]))
		{
		   PTSConfirmStatus = 0x00;
		}
	  }
	  else
	  	PTSConfirmStatus = 0x00;
#else
      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
      {
        if(locData != 0xFF)
        {
           PTSConfirmStatus = 0x00;
        }
      }
      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
      {
        if(locData != 0x10)
        {
           PTSConfirmStatus = 0x00;
        }
      }
      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
      {
        if(locData != SC_A2R.T[0])
        {
           PTSConfirmStatus = 0x00;
        }
      }
      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
      {
        if(locData != ((uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SC_A2R.T[0]))
        {
           PTSConfirmStatus = 0x00;
        }
      }
      else
      {
        PTSConfirmStatus = 0x00;
      }
#endif
      /* PTS Confirm */
      if(PTSConfirmStatus == 0x01)
      {
        workingbaudrate = apbclock * D_Table[(SC_A2R.T[0] & (uint8_t)0x0F)];
        workingbaudrate /= F_Table[((SC_A2R.T[0] >> 4) & (uint8_t)0x0F)];

        USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
        USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
        USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
        USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
        USART_ClockInit(SC_USART, &USART_ClockInitStructure);

        USART_InitStructure.USART_BaudRate = workingbaudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
        USART_InitStructure.USART_Parity = USART_Parity_Even;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_Init(SC_USART, &USART_InitStructure);
      }
    }
  }  
}

void SC_PTSConfigCritical(void)
{
  RCC_ClocksTypeDef RCC_ClocksStatus;
  uint32_t workingbaudrate = 0, apbclock = 0;
  uint8_t locData = 0, PTSConfirmStatus = 1;
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure;
  
/* Reconfigure the USART Baud Rate -------------------------------------------*/
  RCC_GetClocksFreq(&RCC_ClocksStatus);
  apbclock = RCC_ClocksStatus.PCLK2_Frequency;
  apbclock /= ((SC_USART->GTPR & (uint16_t)0x00FF) * 2);

  /* Enable the DMA Receive (Set DMAR bit only) to enable interrupt generation
     in case of a framing error FE */  
  USART_DMACmd(SC_USART, USART_DMAReq_Rx, ENABLE);
  
  if((SC_A2R.T0 & (uint8_t)0x10) == 0x10)
  {
    if(SC_A2R.T[0] != 0x11)
    {
      /* Send PTSS */
      SCData = 0xFF;
      USART_SendData(SC_USART, SCData);
      while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
      {
      }

      /* Send PTS0 */
      SCData = 0x10;
      USART_SendData(SC_USART, SCData);
      while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
      {
      }

      /* Send PTS1 */
      SCData = SC_A2R.T[0]; 
      USART_SendData(SC_USART, SCData);
      while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
      {
      }

      /* Send PCK */
      SCData = (uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SC_A2R.T[0]; 
      USART_SendData(SC_USART, SCData);
      while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
      {
      }

      /* Disable the DMA Receive (Reset DMAR bit only) */  
      USART_DMACmd(SC_USART, USART_DMAReq_Rx, DISABLE);
      
      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
      {
        if(locData != 0xFF)
        {
           PTSConfirmStatus = 0x00;
        }
      }
      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
      {
        if(locData != 0x10)
        {
           PTSConfirmStatus = 0x00;
        }
      }
      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
      {
        if(locData != SC_A2R.T[0])
        {
           PTSConfirmStatus = 0x00;
        }
      }
      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
      {
        if(locData != ((uint8_t)0xFF^(uint8_t)0x10^(uint8_t)SC_A2R.T[0]))
        {
           PTSConfirmStatus = 0x00;
        }
      }
      else
      {
        PTSConfirmStatus = 0x00;
      }
      /* PTS Confirm */
      if(PTSConfirmStatus == 0x01)
      {
        workingbaudrate = apbclock * D_Table[(SC_A2R.T[0] & (uint8_t)0x0F)];
        workingbaudrate /= F_Table[((SC_A2R.T[0] >> 4) & (uint8_t)0x0F)];

        USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
        USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
        USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
        USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
        USART_ClockInit(SC_USART, &USART_ClockInitStructure);

        USART_InitStructure.USART_BaudRate = workingbaudrate;
        USART_InitStructure.USART_WordLength = USART_WordLength_9b;
        USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
        USART_InitStructure.USART_Parity = USART_Parity_Even;
        USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_Init(SC_USART, &USART_InitStructure);
      }
    }
  }  
}

/**
  * @brief  Manages the Smartcard transport layer: send APDU commands and receives
  *   the APDU responce.
  * @param  SC_ADPU: pointer to a SC_ADPU_Commands structure which will be initialized.  
  * @param  SC_Response: pointer to a SC_ADPU_Responce structure which will be initialized.
  * @retval None
  */
void SC_SendData(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_ResponceStatus)
{
  uint32_t i = 0;
  uint8_t locData = 0;
  uint8_t aLE;
  xt_u8 locRetData[2] = {0};
  xt_s32 ret = 0;
  
  /* Reset responce buffer ---------------------------------------------------*/
  for(i = 0; i < LC_MAX; i++)
  {
    SC_ResponceStatus->Data[i] = 0;
  }
  
  SC_ResponceStatus->SW1 = 0;
  SC_ResponceStatus->SW2 = 0;

  /* Enable the DMA Receive (Set DMAR bit only) to enable interrupt generation
     in case of a framing error FE */  
  USART_DMACmd(SC_USART, USART_DMAReq_Rx, ENABLE);

  /* Send header -------------------------------------------------------------*/
  SCData = SC_ADPU->Header.CLA;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  }  
  
  SCData = SC_ADPU->Header.INS;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  }
   
  SCData = SC_ADPU->Header.P1;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  } 
  
  SCData = SC_ADPU->Header.P2;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  }   

  /* Send body length to/from SC ---------------------------------------------*/
  if(SC_ADPU->Body.LC || SC_ADPU->Body.LE == 0)
  {
    SCData = SC_ADPU->Body.LC;
    USART_SendData(SC_USART, SCData);
    while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
    {
    }     
  }
  else if(SC_ADPU->Body.LE)
  { 
    SCData = SC_ADPU->Body.LE;
    USART_SendData(SC_USART, SCData);
    while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
    {
    }     
  }
  /* Flush the SC_USART DR */
  (void)USART_ReceiveData(SC_USART);

  /* --------------------------------------------------------
    Wait Procedure byte from card:
    1 - ACK
    2 - NULL
    3 - SW1; SW2
   -------------------------------------------------------- */
   USART_ITConfig(SC_USART, USART_IT_RXNE, ENABLE);
  	if(SC_ADPU->Header.INS != SC_GET_RESPONCE){
  ret = SCUartRead(locRetData, sizeof(locRetData));
#if 1
  if (ret == 2)
  {
  	if(((locRetData[0] & (uint8_t)0xF0) == 0x60) || ((locRetData[0] & (uint8_t)0xF0) == 0x90))
    {
      /* SW1 received */
      SC_ResponceStatus->SW1 = locRetData[0];

      /* SW2 received */
      SC_ResponceStatus->SW2 = locRetData[1];
    }
  }
  else if (ret == 1)
  {
  	if (((locRetData[0] & (uint8_t)0xFE) == (((uint8_t)~(SC_ADPU->Header.INS)) & \
             (uint8_t)0xFE))||((locRetData[0] & (uint8_t)0xFE) == (SC_ADPU->Header.INS & (uint8_t)0xFE)))
    {
      SC_ResponceStatus->Data[0] = locRetData[0];/* ACK received */
    }
  }
#else
  if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
  {
    if(((locData & (uint8_t)0xF0) == 0x60) || ((locData & (uint8_t)0xF0) == 0x90))
    {
      /* SW1 received */
      SC_ResponceStatus->SW1 = locData;

      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
      {
        /* SW2 received */
        SC_ResponceStatus->SW2 = locData;
      }
    }
    else if (((locData & (uint8_t)0xFE) == (((uint8_t)~(SC_ADPU->Header.INS)) & \
             (uint8_t)0xFE))||((locData & (uint8_t)0xFE) == (SC_ADPU->Header.INS & (uint8_t)0xFE)))
    {
      SC_ResponceStatus->Data[0] = locData;/* ACK received */
    }
  }
#endif
  	}
  /* If no status bytes received ---------------------------------------------*/
  if(SC_ResponceStatus->SW1 == 0x00 || (SC_ResponceStatus->SW2 > 0))
  {
    /* Send body data to SC--------------------------------------------------*/
    if (SC_ADPU->Body.LC || SC_ADPU->Body.LE == 0)
    {
      if(SC_ADPU->Body.LC)
      {
      	  USART_ITConfig(SC_USART, USART_IT_RXNE, DISABLE);
	      for(i = 0; i < SC_ADPU->Body.LC; i++)
	      {
	        SCData = SC_ADPU->Body.Data[i];
	        
	        USART_SendData(SC_USART, SCData);
	        while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
	        {
	        } 
	      }
	      /* Flush the SC_USART DR */
	      (void)USART_ReceiveData(SC_USART);
	      /* Disable the DMA Receive (Reset DMAR bit only) */  
	      USART_DMACmd(SC_USART, USART_DMAReq_Rx, DISABLE);
		  USART_ITConfig(SC_USART, USART_IT_RXNE, ENABLE);
	  }
	  
	  ret = SCUartRead(locRetData, sizeof(locRetData));
	  if (ret == 2)
	  {
	    SC_ResponceStatus->SW1 = locRetData[0];
		SC_ResponceStatus->SW2 = locRetData[1];
	  }
	  USART_ITConfig(SC_USART, USART_IT_RXNE, DISABLE);
	  
    }
    /* Or receive body data from SC ------------------------------------------*/
    else if (SC_ADPU->Body.LE)
    {
      if(SC_ResponceStatus->SW1 == 0x00)
	  	aLE = SC_ADPU->Body.LE;
	  else if (SC_ResponceStatus->SW2 > 0)
	  	aLE = SC_ResponceStatus->SW2;
#if 1
	  ret = SCUartRead(SC_ResponceStatus->Data, aLE + 2);
	  if(ret == aLE + 2)
	  {
	    SC_ResponceStatus->SW1 = SC_ResponceStatus->Data[aLE];
		SC_ResponceStatus->Data[aLE] = 0;
		SC_ResponceStatus->SW2 = SC_ResponceStatus->Data[aLE + 1];
		SC_ResponceStatus->Data[aLE + 1] = 0;
	  }
	  else
	  {
	  	SC_ResponceStatus->SW1 = 0;
		SC_ResponceStatus->SW2 = 0;
	  }
	  USART_ITConfig(SC_USART, USART_IT_RXNE, DISABLE);
	  //PrintfXTOS("~~~~~ SC_AnswerReq ret = %d rx = %d ~~~~~\n", ret, scrxcnt);
	  //if(ret > 0)
	    //UTPrinthex(SC_ResponceStatus->Data, ret);
#else
      for(i = 0; i < aLE; i++)
      {
        if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
        {
          SC_ResponceStatus->Data[i] = locData;
        }
      }
#endif
    }
#if 0
    /* Wait SW1 --------------------------------------------------------------*/
    i = 0;
    while(i < 10)
    {
      if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
      {
        SC_ResponceStatus->SW1 = locData;
        i = 11;
      }
      else
      {
        i++;
      }
    }
    /* Wait SW2 ------------------------------------------------------------*/   
    i = 0;
    while(i < 10)
    {
      if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
      {
        SC_ResponceStatus->SW2 = locData;
        i = 11;
      }
      else
      {
        i++;
      }
    }
#endif
  }
}

void SC_SendDataCritical(SC_ADPU_Commands *SC_ADPU, SC_ADPU_Responce *SC_ResponceStatus)
{
  uint32_t i = 0;
  uint8_t locData = 0;
  uint8_t aLE;
  
  /* Reset responce buffer ---------------------------------------------------*/
  for(i = 0; i < LC_MAX; i++)
  {
    SC_ResponceStatus->Data[i] = 0;
  }
  
  SC_ResponceStatus->SW1 = 0;
  SC_ResponceStatus->SW2 = 0;

  /* Enable the DMA Receive (Set DMAR bit only) to enable interrupt generation
     in case of a framing error FE */  
  USART_DMACmd(SC_USART, USART_DMAReq_Rx, ENABLE);

  /* Send header -------------------------------------------------------------*/
  SCData = SC_ADPU->Header.CLA;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  }  
  
  SCData = SC_ADPU->Header.INS;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  }
   
  SCData = SC_ADPU->Header.P1;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  } 
  
  SCData = SC_ADPU->Header.P2;
  USART_SendData(SC_USART, SCData);
  while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
  {
  }   

  /* Send body length to/from SC ---------------------------------------------*/
  if(SC_ADPU->Body.LC || SC_ADPU->Body.LE == 0)
  {
    SCData = SC_ADPU->Body.LC;
    USART_SendData(SC_USART, SCData);
    while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
    {
    }     
  }
  else if(SC_ADPU->Body.LE)
  { 
    SCData = SC_ADPU->Body.LE;
    USART_SendData(SC_USART, SCData);
    while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
    {
    }     
  }
  /* Flush the SC_USART DR */
  (void)USART_ReceiveData(SC_USART);

  /* --------------------------------------------------------
    Wait Procedure byte from card:
    1 - ACK
    2 - NULL
    3 - SW1; SW2
   -------------------------------------------------------- */
  	if(SC_ADPU->Header.INS != SC_GET_RESPONCE){

	  if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
	  {
	    if(((locData & (uint8_t)0xF0) == 0x60) || ((locData & (uint8_t)0xF0) == 0x90))
	    {
	      /* SW1 received */
	      SC_ResponceStatus->SW1 = locData;

	      if((USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT)) == SUCCESS)
	      {
	        /* SW2 received */
	        SC_ResponceStatus->SW2 = locData;
	      }
	    }
	    else if (((locData & (uint8_t)0xFE) == (((uint8_t)~(SC_ADPU->Header.INS)) & \
	             (uint8_t)0xFE))||((locData & (uint8_t)0xFE) == (SC_ADPU->Header.INS & (uint8_t)0xFE)))
	    {
	      SC_ResponceStatus->Data[0] = locData;/* ACK received */
	    }
	  }

  	}
  /* If no status bytes received ---------------------------------------------*/
  if(SC_ResponceStatus->SW1 == 0x00 || (SC_ResponceStatus->SW2 > 0))
  {
    /* Send body data to SC--------------------------------------------------*/
    if (SC_ADPU->Body.LC || SC_ADPU->Body.LE == 0)
    {
      if(SC_ADPU->Body.LC)
      {
      	  USART_ITConfig(SC_USART, USART_IT_RXNE, DISABLE);
	      for(i = 0; i < SC_ADPU->Body.LC; i++)
	      {
	        SCData = SC_ADPU->Body.Data[i];
	        
	        USART_SendData(SC_USART, SCData);
	        while(USART_GetFlagStatus(SC_USART, USART_FLAG_TC) == RESET)
	        {
	        } 
	      }
	      /* Flush the SC_USART DR */
	      (void)USART_ReceiveData(SC_USART);
	      /* Disable the DMA Receive (Reset DMAR bit only) */  
	      USART_DMACmd(SC_USART, USART_DMAReq_Rx, DISABLE);
	  }
	  
    }
    /* Or receive body data from SC ------------------------------------------*/
    else if (SC_ADPU->Body.LE)
    {
      if(SC_ResponceStatus->SW1 == 0x00)
	  	aLE = SC_ADPU->Body.LE;
	  else if (SC_ResponceStatus->SW2 > 0)
	  	aLE = SC_ResponceStatus->SW2;
      for(i = 0; i < aLE; i++)
      {
        if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
        {
          SC_ResponceStatus->Data[i] = locData;
        }
      }
    }

    /* Wait SW1 --------------------------------------------------------------*/
    i = 0;
    while(i < 10)
    {
      if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
      {
        SC_ResponceStatus->SW1 = locData;
        i = 11;
      }
      else
      {
        i++;
      }
    }
    /* Wait SW2 ------------------------------------------------------------*/   
    i = 0;
    while(i < 10)
    {
      if(USART_ByteReceive(&locData, SC_RECEIVE_TIMEOUT) == SUCCESS)
      {
        SC_ResponceStatus->SW2 = locData;
        i = 11;
      }
      else
      {
        i++;
      }
    }

  }
}

/**
  * @brief  Requests the reset answer from card.
  * @param  SCState: pointer to an SC_State enumeration that will contain the Smartcard state.
  * @param  card: pointer to a buffer which will contain the card ATR.
  * @param  length: maximum ATR length
  * @retval None
  */
static void SC_AnswerReq(SC_State *SCState, uint8_t *card, uint8_t length)
{
  uint8_t Data = 0;
  uint32_t i = 0;
  uint8_t *pcard = card;
  int32_t ret = 0;
  
  switch(*SCState)
  {
    case SC_RESET_LOW:
#if 0
      /* Check responce with reset low ---------------------------------------*/
      for (i = 0; i < length; i++)
      {
        if((USART_ByteReceive(&Data, SC_RECEIVE_TIMEOUT)) == SUCCESS)
        {
          card[i] = Data;
        }
      }
      if(card[0])
      {
        (*SCState) = SC_ACTIVE;
        SC_Reset(Bit_SET);
      }
      else
      {
        (*SCState) = SC_RESET_HIGH;
      }
#else
	  (*SCState) = SC_RESET_HIGH;
#endif
    break;

    case SC_RESET_HIGH:
      /* Check responce with reset high --------------------------------------*/
      SC_Reset(Bit_SET); /* Reset High */
	  
      #if 0
	  USART_ITConfig(SC_USART, USART_IT_RXNE, ENABLE);
	  ret = SCUartRead(card, length);
	  USART_ITConfig(SC_USART, USART_IT_RXNE, DISABLE);
	  #else
      while(length--)
      {
        if((USART_ByteReceive(&Data, SC_RECEIVE_TIMEOUT)) == SUCCESS)
        {
          *pcard++ = Data; /* Receive data for timeout = SC_RECEIVE_TIMEOUT */
        }
      }
	  #endif

	  //PrintfXTOS("~~~~~ SC_AnswerReq ret = %d rx = %d ~~~~~\n", ret, scrxcnt);
	  //if(ret > 0)
	    //UTPrinthex(card, ret);
	  
      if(card[0])
      {
        (*SCState) = SC_ACTIVE;
      }
      else
      {
        (*SCState) = SC_POWER_OFF;
      }
    break;

    case SC_ACTIVE:
    break;
    
    case SC_POWER_OFF:
      /* Close Connection if no answer received ------------------------------*/
      SC_Reset(Bit_SET); /* Reset high - a bit is used as level shifter from 3.3 to 5 V */
      SC_PowerCmd(DISABLE);
    break;

    default:
      (*SCState) = SC_RESET_LOW;
  }
}

/**
  * @brief  Decodes the Answer to reset received from card.
  * @param  card: pointer to the buffer containing the card ATR.
  * @retval None
  */
static uint8_t SC_decode_Answer2reset(uint8_t *card)
{
  uint32_t i = 0, flag = 0, buf = 0, protocol = 0;

  SC_A2R.TS = card[0];  /* Initial character */
  SC_A2R.T0 = card[1];  /* Format character */

  SC_A2R.Hlength = SC_A2R.T0 & (uint8_t)0x0F;

  if ((SC_A2R.T0 & (uint8_t)0x80) == 0x80)
  {
    flag = 1;
  }

  for (i = 0; i < 4; i++)
  {
    SC_A2R.Tlength = SC_A2R.Tlength + (((SC_A2R.T0 & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1);
  }

  for (i = 0; i < SC_A2R.Tlength; i++)
  {
    SC_A2R.T[i] = card[i + 2];
  }

  protocol = SC_A2R.T[SC_A2R.Tlength - 1] & (uint8_t)0x0F;

  while (flag)
  {
    if ((SC_A2R.T[SC_A2R.Tlength - 1] & (uint8_t)0x80) == 0x80)
    {
      flag = 1;
    }
    else
    {
      flag = 0;
    }

    buf = SC_A2R.Tlength;
    SC_A2R.Tlength = 0;

    for (i = 0; i < 4; i++)
    {
      SC_A2R.Tlength = SC_A2R.Tlength + (((SC_A2R.T[buf - 1] & (uint8_t)0xF0) >> (4 + i)) & (uint8_t)0x1);
    }
	
    for (i = 0;i < SC_A2R.Tlength; i++)
    {
      SC_A2R.T[buf + i] = card[i + 2 + buf];
    }
    SC_A2R.Tlength += (uint8_t)buf;
  }

  for (i = 0; i < SC_A2R.Hlength; i++)
  {
    SC_A2R.H[i] = card[i + 2 + SC_A2R.Tlength];
  }

  return (uint8_t)protocol;
}

/**
  * @brief  Initializes all peripheral used for Smartcard interface.
  * @param  None
  * @retval None
  */
static void SC_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure;  
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* Enable GPIO clocks */
  RCC_AHBPeriphClockCmd(SC_USART_TX_GPIO_CLK | SC_USART_RX_GPIO_CLK | SC_USART_CK_GPIO_CLK, ENABLE);
                         
  /* Enable SC_USART clock */
  SC_USART_APBPERIPHCLOCK(SC_USART_CLK, ENABLE);

  /* Connect PXx to SC_USART_TX */
  GPIO_PinAFConfig(SC_USART_TX_GPIO_PORT, SC_USART_TX_SOURCE, SC_USART_TX_AF);

  /* Connect PXx to SC_USART_RX */
  GPIO_PinAFConfig(SC_USART_RX_GPIO_PORT, SC_USART_RX_SOURCE, SC_USART_RX_AF);
  
  /* Connect PXx to SC_USART_CK */
  GPIO_PinAFConfig(SC_USART_CK_GPIO_PORT, SC_USART_CK_SOURCE, SC_USART_CK_AF); 

  /* Configure USART Tx and Rx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Pin = SC_USART_CK_PIN;
  GPIO_Init(SC_USART_CK_GPIO_PORT, &GPIO_InitStructure);
  
  /* Configure USART Tx pin as alternate function open-drain */
  GPIO_InitStructure.GPIO_Pin = SC_USART_TX_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_Init(SC_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SC_USART_RX_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(SC_USART_RX_GPIO_PORT, &GPIO_InitStructure);

  /* Enable SC_USART IRQ */
  NVIC_InitStructure.NVIC_IRQChannel = SC_USART_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;    
  NVIC_Init(&NVIC_InitStructure);
    
/* SC_USART configuration ----------------------------------------------------*/
  /* SC_USART configured as follow:
        - Word Length = 9 Bits
        - 1.5 Stop Bit
        - Even parity
        - BaudRate = 8064 baud
        - Hardware flow control disabled (RTS and CTS signals)
        - Tx and Rx enabled
        - USART Clock enabled
  */

  /* USART Clock set to 4 MHz (PCLK2 (16 MHz) / 4) */
  USART_SetPrescaler(SC_USART, 0x02);
  
  /* USART Guard Time set to 16 Bit */
  USART_SetGuardTime(SC_USART, 16);
  
  USART_ClockInitStructure.USART_Clock = USART_Clock_Enable;
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_1Edge;
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Enable;
  USART_ClockInit(SC_USART, &USART_ClockInitStructure);


  USART_InitStructure.USART_BaudRate = 10753;
  USART_InitStructure.USART_WordLength = USART_WordLength_9b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1_5;
  USART_InitStructure.USART_Parity = USART_Parity_Even;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_Init(SC_USART, &USART_InitStructure); 

  /* Enable the SC_USART Parity Error Interrupt */
  USART_ITConfig(SC_USART, USART_IT_PE, ENABLE);

  /* Enable the SC_USART Framing Error Interrupt */
  USART_ITConfig(SC_USART, USART_IT_ERR, ENABLE);

  if (scrxrq == NULL)
  {
  	scrxrq = RQueueNew((xt_u8 *)MemMallocXTOS(SC_UART_RX_BUFFER_SIZE)
		, SC_UART_RX_BUFFER_SIZE);
  }
  
  /* Enable SC_USART */
  USART_Cmd(SC_USART, ENABLE);

  /* Enable the NACK Transmission */
  USART_SmartCardNACKCmd(SC_USART, ENABLE);

  /* Enable the Smartcard Interface */
  USART_SmartCardCmd(SC_USART, ENABLE);
  
  /* Set RSTIN HIGH */  
  SC_Reset(Bit_SET);
}

/**
  * @brief  Deinitializes all ressources used by the Smartcard interface.
  * @param  None
  * @retval None
  */
static void SC_DeInit(void)
{
#if 0
  /* Disable CMDVCC */
  SC_PowerCmd(DISABLE);

  /* Deinitializes the SC_USART */
  USART_DeInit(SC_USART);
                         
  /* Disable SC_USART clock */
  SC_USART_APBPERIPHCLOCK(SC_USART_CLK, DISABLE);
#endif
}

/**
  * @brief  Configures the card power voltage.
  * @param  SC_Voltage: specifies the card power voltage.
  *         This parameter can be one of the following values:
  *              @arg SC_VOLTAGE_5V: 5V cards.
  *              @arg SC_VOLTAGE_3V: 3V cards.
  * @retval None
  */
static void SC_VoltageConfig(uint32_t SC_Voltage)
{
#if 0
  if(SC_Voltage == SC_VOLTAGE_5V)
  {
    /* Select Smartcard 5V */  
    GPIO_SetBits(SC_3_5V_GPIO_PORT, SC_3_5V_PIN);
  }
  else
  {
    /* Select Smartcard 3V */      
    GPIO_ResetBits(SC_3_5V_GPIO_PORT, SC_3_5V_PIN);
  } 
#endif
}

/**
  * @brief  Configures GPIO hardware resources used for Samrtcard.
  * @param  None
  * @retval None
  */
void SC_IOConfig(void)
{
  EXTI_InitTypeDef EXTI_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable GPIO clock */
  RCC_AHBPeriphClockCmd(SC_RESET_GPIO_CLK, ENABLE);

  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure Smartcard CMDVCC pin */
  GPIO_InitStructure.GPIO_Pin = SC_CMDVCC_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  //GPIO_Init(SC_CMDVCC_GPIO_PORT, &GPIO_InitStructure);

  /* Configure Smartcard Reset pin */
  GPIO_InitStructure.GPIO_Pin = SC_RESET_PIN;
  GPIO_Init(SC_RESET_GPIO_PORT, &GPIO_InitStructure);

#if 0
  /* Configure Smartcard 3/5V pin */
  GPIO_InitStructure.GPIO_Pin = SC_3_5V_PIN;
  GPIO_Init(SC_3_5V_GPIO_PORT, &GPIO_InitStructure);

  /* Select 5V */ 
  SC_VoltageConfig(SC_VOLTAGE_5V);
#endif

  /* Disable CMDVCC */
  SC_PowerCmd(DISABLE);

  /* Set RSTIN HIGH */  
  SC_Reset(Bit_SET);

#if 0    						                                
  /* Configure Smartcard OFF pin */
  GPIO_InitStructure.GPIO_Pin = SC_OFF_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(SC_OFF_GPIO_PORT, &GPIO_InitStructure);
 
  /* Configure EXTI line connected to Smartcard OFF Pin */
  SYSCFG_EXTILineConfig(SC_OFF_EXTI_PORT_SOURCE, SC_OFF_EXTI_PIN_SOURCE);

  EXTI_ClearITPendingBit(SC_OFF_EXTI_LINE);
  
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_Line = SC_OFF_EXTI_LINE;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Clear the SC_OFF_EXTI_IRQn Pending Bit */
  NVIC_ClearPendingIRQ(SC_OFF_EXTI_IRQn);
  NVIC_InitStructure.NVIC_IRQChannel = SC_OFF_EXTI_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
#endif

}

/**
  * @brief  Detects whether the Smartcard is present or not.
  * @param  None. 
  * @retval 0 - Smartcard inserted
  *         1 - Smartcard not inserted
  */
static uint8_t SC_Detect(void)
{
  //return GPIO_ReadInputDataBit(SC_OFF_GPIO_PORT, SC_OFF_PIN);
  return 1;
}

/**
  * @brief  Receives a new data while the time out not elapsed.
  * @param  None
  * @retval An ErrorStatus enumuration value:
  *          - SUCCESS: New data has been received
  *          - ERROR: time out was elapsed and no further data is received
  */
static ErrorStatus USART_ByteReceive(uint8_t *Data, uint32_t TimeOut)
{
  uint32_t Counter = 0;

  while((USART_GetFlagStatus(SC_USART, USART_FLAG_RXNE) == RESET) && (Counter != TimeOut))
  {
    Counter++;
  }

  if(Counter != TimeOut)
  {
    *Data = (uint8_t)USART_ReceiveData(SC_USART);
    return SUCCESS;    
  }
  else 
  {
    return ERROR;
  }
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
