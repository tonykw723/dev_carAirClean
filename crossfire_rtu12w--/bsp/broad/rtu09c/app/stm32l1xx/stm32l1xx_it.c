/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Template/stm32l1xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.3
  * @date    May-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
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
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "xtsys_type.h"
#include "config.h"
#include "stm32l1xx_it.h"
#include "discover_board.h"

//#include "xtsys_type.h"
#include "xtsys_cfg.h"

#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"
#include "xtos_sem.h"

#include "xt_bsp_rtu09c.h"

extern unsigned char keyPressed;
extern int adc1Buf[3];
extern AD_type  ad_cvs;

unsigned char dmaFlag = 0;
/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
void hard_fault_handler_c(unsigned int * hardfault_args) 
{
	/* Go to infinite loop when Hard Fault exception occurs */

	unsigned int stacked_r0;  
	unsigned int stacked_r1;  
	unsigned int stacked_r2;  
	unsigned int stacked_r3;  
	unsigned int stacked_r12;  
	unsigned int stacked_lr;  
	unsigned int stacked_pc;  
	unsigned int stacked_psr;  
	
	stacked_r0 = ((unsigned long) hardfault_args[0]);  
	stacked_r1 = ((unsigned long) hardfault_args[1]);  
	stacked_r2 = ((unsigned long) hardfault_args[2]);  
	stacked_r3 = ((unsigned long) hardfault_args[3]);  
	
	stacked_r12 = ((unsigned long) hardfault_args[4]);  
	stacked_lr = ((unsigned long) hardfault_args[5]);  
	stacked_pc = ((unsigned long) hardfault_args[6]);  
	stacked_psr = ((unsigned long) hardfault_args[7]);  
	
	printf ("[Hard fault handler]\r\n");  
	printf("R0 = %x\r\n", stacked_r0);  
	printf ("R1 = %x\r\n", stacked_r1);  
	printf ("R2 = %x\r\n", stacked_r2);  
	printf ("R3 = %x\r\n", stacked_r3);  
	printf ("R12 = %x\r\n", stacked_r12);  
	printf ("LR = %x\r\n", stacked_lr);  
	printf ("PC = %x\r\n", stacked_pc);  
	printf ("PSR = %x\r\n", stacked_psr);  
	printf ("BFAR = %x\r\n", (*((volatile unsigned long *)(0xE000ED38))));  
	printf ("CFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED28))));  
	printf ("HFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED2C))));  
	printf("DFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED30))));  
	printf("AFSR = %x\r\n", (*((volatile unsigned long *)(0xE000ED3C)))); 
	while (1)
	{
	}
}
/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
/*void SVC_Handler(void)
{
}*/

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
/*void PendSV_Handler(void)
{
}*/

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
/*void SysTick_Handler(void)
{
    TimingDelay_Decrement();
}*/
/******************************************************************************/
/*                 STM32L1xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32l1xx_md.s).                                            */
/******************************************************************************/

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		XtBspOdbUartRxIsr(USART_ReceiveData(USART1) & 0xFF);
	}	
}

#if 0
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		//printf("u2 r =0x%x\n", USART_ReceiveData(USART2) & 0xFF);
		XtBspCtrlUartRxIsr(USART_ReceiveData(USART2) & 0xFF);
	}

}
#else
#include "smartcard.h"
#include "xtsys_type.h"
#include "ringqueue.h"

extern volatile uint32_t scrxcnt;
extern ringqueue_t *scrxrq;
#if 0
void SC_USART_IRQHandler(void)
{
  /* If a Frame error is signaled by the card */
  xt_u8 temp;
  if(USART_GetITStatus(SC_USART, USART_IT_FE) != RESET)
  {
    USART_ReceiveData(SC_USART);

    /* Resend the byte that failed to be received (by the Smartcard) correctly */
    SC_ParityErrorHandler();
  }

  /* If the SC_USART detects a parity error */
  if(USART_GetITStatus(SC_USART, USART_IT_PE) != RESET)
  {
    /* Enable SC_USART RXNE Interrupt (until receiving the corrupted byte) */
    USART_ITConfig(SC_USART, USART_IT_RXNE, ENABLE);
    /* Flush the SC_USART DR register */
    USART_ReceiveData(SC_USART);
  }
  
  if(USART_GetITStatus(SC_USART, USART_IT_RXNE) != RESET)
  {
    /* Disable SC_USART RXNE Interrupt */
	//USART_ITConfig(SC_USART, USART_IT_RXNE, DISABLE);
    temp=USART_ReceiveData(SC_USART);
	//printf("-:%c",temp);
   if (scrxrq == NULL)
  {
  	scrxrq = RQueueNew((xt_u8 *)MemMallocXTOS(1024)
		, 1024);
  }
    if (scrxrq != NULL)
    	RQueueWriteInByte(scrxrq, temp);//USART_ReceiveData(SC_USART) & 0xFF);
    //USART_ReceiveData(SC_USART);
	scrxcnt ++;
  }
  
  /* If a Overrun error is signaled by the card */
  if(USART_GetITStatus(SC_USART, USART_IT_ORE) != RESET)
  {
    USART_ReceiveData(SC_USART);
  }
  /* If a Noise error is signaled by the card */
  if(USART_GetITStatus(SC_USART, USART_IT_NE) != RESET)
  {
    USART_ReceiveData(SC_USART);
  }
  
}
#endif
#endif
void USART2_IRQHandler(void)
{
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		XtBspCtrlUartRxIsr(USART_ReceiveData(USART2) & 0xFF);
	}	
	
}
void USART3_IRQHandler(void)
{
}

/**
  * @brief  This function handles DMA Transfer Complete interrupt request.
  * @param  None
  * @retval None
  */
void DMA1_Channel1_IRQHandler    (void)
{
	unsigned char  i;
	
	if(DMA_GetITStatus(DMA1_IT_TC1) == SET) {
		DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, DISABLE);
		if (dmaFlag == 1)
		{
			for(i=0;i<CHANNLE;i++)
			{
				//if( i == 1)
				//printf("adc [%d] = 0x%x \n",i,adc1Buf[i]);
				ad_cvs.buf[i][ad_cvs.sample]=(adc1Buf[i]) & 0xfff;
			}
			ad_cvs.sample++;
			if(ad_cvs.sample>=SAMPLE_NUM)
			{
				XtBspAdcTimerStop();
			}
			dmaFlag = 0;
		}
		DMA_ClearITPendingBit(DMA1_IT_TC1);	
		//ADC_DMARequestAfterLastTransferCmd(ADC1, DISABLE);
		//ADC_DMACmd(ADC1, DISABLE);
		//ADC_Cmd(ADC1, DISABLE);
	}
}


/**
  * @brief  This function handles RTC Auto wake-up interrupt request.
  * @param  None
  * @retval None
  */
void RTC_WKUP_IRQHandler(void)
{
  if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
  {
    /* Clear RTC and EXTI interrupt flags */
  	RTC_ClearITPendingBit(RTC_IT_WUT);
  	EXTI_ClearITPendingBit(EXTI_Line20);    
  } 
}

/**
  * @brief  This function handles external interrupts generated by UserButton.
  * @param  None
  * @retval None
  */

void EXTI0_IRQHandler(void)
{
}

void EXTI15_10_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line11) != RESET)
    EXTI_ClearITPendingBit(EXTI_Line11);
}

void RTC_Alarm_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_ALRA) != RESET)
  {
    /* Clear the Alarm A Pending Bit */
    RTC_ClearITPendingBit(RTC_IT_ALRA);
    
    /* Clear EXTI line17 pending bit */
    EXTI_ClearITPendingBit(EXTI_Line17);
  }
}

void TIM2_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET) {
		//ADC_DMACmd(ADC1, ENABLE);
		//ADC_Cmd(ADC1, ENABLE);
		//XtBspToggleDoCtrl(GPIO_DO_OTHER_POWEREN);
		dmaFlag = 1;
		ADC_SoftwareStartConv(ADC1);
		//ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
		DMA_ITConfig(DMA1_Channel1, DMA1_IT_TC1, ENABLE);
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

extern xt_u32 stm_sim_tim_flag;
void TIM4_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {
		stm_sim_tim_flag ++;
		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
		STM_EVAL_Tim4_STOP();
	}
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
