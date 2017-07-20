/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2010 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include "NUC200Series.h"

#define PLLCON_SETTING      SYSCLK_PLLCON_50MHz_XTAL
#define PLL_CLOCK           50000000

void CKO_Init(void)
{
    /* CKO = HCLK / 2^(1+1) */
    SYS_EnableCKO(SYSCLK_CLKSEL2_FRQDIV_HCLK, 3);

}

void ADC_Init(void)
{
    /* Set to convert ADC channel 2 */
    ADC->ADCHER = 0x40;

}


void SPI0_Init(void)
{
    /* Initial SPI data format and SPI clock: 32-bit data output at clock falling and latched at clock rising */    
    SPI0->CNTRL   = SPI_CNTRL_CLK_IDLE_HIGH | SPI_CNTRL_TX_FALLING | 
                       SPI_CNTRL_RX_RISING | SPI_CNTRL_TX_BIT_LEN(32);

    /* SPI clock freq = system clock / 4 */
    SPI0->DIVIDER = SPI_DIVIDER_DIV(4); 
    
    /* Enable automatic slave select control and set it as active low */
    SPI0->SSR = SPI_SSR_HW_AUTO_ACTIVE_LOW;
}


void SPI1_Init(void)
{
    /* Initial SPI data format and SPI clock: 16-bit data output at clock falling and latched at clock rising */    
    SPI1->CNTRL   = SPI_CNTRL_CLK_IDLE_HIGH | SPI_CNTRL_TX_FALLING | 
                       SPI_CNTRL_RX_RISING | SPI_CNTRL_TX_BIT_LEN(16);

    /* SPI clock freq = system clock / 4 */
    SPI1->DIVIDER = SPI_DIVIDER_DIV(4); 
    
    /* Enable automatic slave select control and set it as active low */
    SPI1->SSR = SPI_SSR_HW_AUTO_ACTIVE_LOW;
}

#if 0
void I2C0_Init(void)
{
    /* Enable I2C0 Controller */
    I2C0->I2CON = I2C_I2CON_ENS1_Msk;
    
    /* I2C0 clock divider, I2C Bus Clock = PCLK / (4*125) */
    I2C0->I2CLK = I2C_I2CLK_DIV4(125);

    /* Set I2C0 4 Slave Addresses */            
    _I2C_SET_SLAVE_ADDRESS_0(I2C0, 0x15);   /* Slave Address : 0x15 */
    _I2C_SET_SLAVE_ADDRESS_1(I2C0, 0x35);   /* Slave Address : 0x35 */
    _I2C_SET_SLAVE_ADDRESS_2(I2C0, 0x55);   /* Slave Address : 0x55 */
    _I2C_SET_SLAVE_ADDRESS_3(I2C0, 0x75);   /* Slave Address : 0x75 */

    /* Enable I2C0 interrupt and set corresponding NVIC bit */
    //I2C->I2CON |= I2C_I2CON_EI_Msk;
    //NVIC_EnableIRQ(I2C0_IRQn);
}
#endif

void PWMA_Init(void)
{

    /* PWM Timer0: Clk = HCLK / 120 / 16, Freq = clk / 6250, dute cycle = 3125/6250 % */
    /* PWM Timer1: Clk = HCLK / 120 / 16, Freq = clk / 3125, dute cycle = 1563/3125 % */
    /* PWM Timer2: Clk = HCLK / 60 / 16, Freq = clk / 3125, dute cycle = 1563/3125 % */
    /* PWM Timer3: Clk = HCLK / 60 / 1, Freq = clk / 50, dute cycle = 25/50 % */
    /* PWM0 = 12000000 / 120 / 16 / 6250 =    1Hz */
    /* PWM1 = 12000000 / 120 / 16 / 3125 =    2Hz */
    /* PWM2 = 12000000 /  60 / 16 / 3125 =    4Hz */
    /* PWM3 = 12000000 /  60 /  1 /   50 = 4000Hz */
    PWMA->PPR = PWM_PPR_CP01(120) | PWM_PPR_CP23(60);
    PWMA->CSR = PWM_CSR_CSR0(PWM_CSR_DIV16) | PWM_CSR_CSR1(PWM_CSR_DIV16) |
                   PWM_CSR_CSR2(PWM_CSR_DIV16) | PWM_CSR_CSR3(PWM_CSR_DIV1);

    /* Enable PWM0, PWM1, PWM2, PWM3 counter. We must set PWM mode before setting CNR, CMR. */
    PWMA->PCR = PWM_PCR_CH0EN_Msk | PWM_PCR_CH0MOD_AUTO_RELOAD | 
                   PWM_PCR_CH1EN_Msk | PWM_PCR_CH1MOD_AUTO_RELOAD |
                   PWM_PCR_CH2EN_Msk | PWM_PCR_CH2MOD_AUTO_RELOAD |
                   PWM_PCR_CH3EN_Msk | PWM_PCR_CH3MOD_AUTO_RELOAD;

    PWMA->CNR0= 6250 - 1;
    PWMA->CMR0= 3125 - 1;
    PWMA->CNR1= 3125 - 1;
    PWMA->CMR1= 1563 - 1;
    PWMA->CNR2= 3125 - 1;
    PWMA->CMR2= 1563 - 1;
    PWMA->CNR3= 50 - 1;
    PWMA->CMR3= 25 - 1;

    /* Enable PWM channle 0, 1, 2, 3 Output */
    PWMA->POE = PWM_POE_POE0_Msk | PWM_POE_POE1_Msk | PWM_POE_POE2_Msk | PWM_POE_POE3_Msk;
}


void PWMB_Init(void)
{

    /* PWM Timer0: Clk = HCLK / 120 / 16, Freq = clk / 6250, dute cycle = 3125/6250 % */
    /* PWM Timer1: Clk = HCLK / 120 / 16, Freq = clk / 3125, dute cycle = 1563/3125 % */
    /* PWM Timer2: Clk = HCLK / 60 / 16, Freq = clk / 3125, dute cycle = 1563/3125 % */
    /* PWM Timer3: Clk = HCLK / 60 / 1, Freq = clk / 50, dute cycle = 25/50 % */
    /* PWM0 = 12000000 / 120 / 16 / 6250 =    1Hz */
    /* PWM1 = 12000000 / 120 / 16 / 3125 =    2Hz */
    /* PWM2 = 12000000 /  60 / 16 / 3125 =    4Hz */
    /* PWM3 = 12000000 /  60 /  1 /   50 = 4000Hz */
    PWMB->PPR = PWM_PPR_CP01(120) | PWM_PPR_CP23(60);
    PWMB->CSR = PWM_CSR_CSR0(PWM_CSR_DIV16) | PWM_CSR_CSR1(PWM_CSR_DIV16) |
                   PWM_CSR_CSR2(PWM_CSR_DIV16) | PWM_CSR_CSR3(PWM_CSR_DIV1);

    /* Enable PWM0, PWM1, PWM2, PWM3 counter. We must set PWM mode before setting CNR, CMR. */
    PWMB->PCR = PWM_PCR_CH0EN_Msk | PWM_PCR_CH0MOD_AUTO_RELOAD | 
                   PWM_PCR_CH1EN_Msk | PWM_PCR_CH1MOD_AUTO_RELOAD |
                   PWM_PCR_CH2EN_Msk | PWM_PCR_CH2MOD_AUTO_RELOAD |
                   PWM_PCR_CH3EN_Msk | PWM_PCR_CH3MOD_AUTO_RELOAD;

    PWMB->CNR0= 6250 - 1;
    PWMB->CMR0= 3125 - 1;
    PWMB->CNR1= 3125 - 1;
    PWMB->CMR1= 1563 - 1;
    PWMB->CNR2= 3125 - 1;
    PWMB->CMR2= 1563 - 1;
    PWMB->CNR3= 50 - 1;
    PWMB->CMR3= 25 - 1;

    /* Enable PWM channle 0, 1, 2, 3 Output */
    PWMB->POE = PWM_POE_POE0_Msk | PWM_POE_POE1_Msk | PWM_POE_POE2_Msk | PWM_POE_POE3_Msk;
}


void TMR0_Init(void)
{
    TIMER0->TCSR = TIMER_TCSR_INIT_IE(12);
    TIMER0->TCMPR = 10000; // 12MHz / 12 / 10000 = 100Hz

    //NVIC_EnableIRQ(TMR0_IRQn);
}

void TMR1_Init(void)
{
    TIMER1->TCSR = TIMER_TCSR_INIT_IE(12);
    TIMER1->TCMPR = 10000; // 12MHz / 12 / 10000 = 100Hz

    //NVIC_EnableIRQ(TMR0_IRQn);
}


void GPIO_Init(void)
{
    /* Debounce function control */
    GPIO->DBNCECON = GPIO_DBNCECON_ICLK_ON | GPIO_DBNCECON_DBCLKSRC_HCLK | GPIO_DBNCECON_DBCLKSEL_32768;

    /* Enable P3.2 debounce function */
    PD->DBEN = GPIO_DBEN_ENABLE(2);

    /* Configure external interrupt EINT0 (P3.2) */
    GPIO_EnableInt(PD, 2, GPIO_INT_FALLING);
    NVIC_EnableIRQ(EINT0_IRQn);
}

void UART0_Init(void)
{
    /* Set 115200 baudrate according to 50MHz system clock */
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_DIV_MODE2(PLL_CLOCK, 115200);
    _UART_SET_DATA_FORMAT(UART0, UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1);
}

void UART1_IRQHandler(void)
{
	uint8_t u8InChar=0xFF;
	uint32_t u32IntSts= UART1->ISR;
	
	if(u32IntSts & UART_ISR_RDA_INT_Msk)
	{
		while(_UART_IS_RX_READY(UART1)) 
		{
			_UART_RECEIVEBYTE(UART1,u8InChar); 
			//printf("u1 r =0x%x\n", u8InChar & 0xFF);
			XtBspCtrlUartRxIsr(u8InChar & 0xFF);
		}
	}
}

void UART1_Init(xt_u32 buadrate)
{
    /* Set 9600 baudrate according to 50MHz system clock */
    UART1->BAUD = UART_BAUD_MODE2 | UART_BAUD_DIV_MODE2(PLL_CLOCK, buadrate);
    _UART_SET_DATA_FORMAT(UART1, UART_WORD_LEN_8 | UART_PARITY_EVEN | UART_STOP_BIT_1);
}

void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
    /* Enable external 12MHz XTAL, internal 22.1184MHz, 10kHz */
    SYSCLK->PWRCON |= SYSCLK_PWRCON_XTL12M_EN_Msk | SYSCLK_PWRCON_XTL32K_EN_Msk | SYSCLK_CLKSTATUS_IRC10K_STB_Msk;

    SYSCLK->PLLCON = PLLCON_SETTING;

    /* Waiting for clock ready */
    SYS_WaitingForClockReady(SYSCLK_CLKSTATUS_PLL_STB_Msk | SYSCLK_CLKSTATUS_XTL12M_STB_Msk | 
    							SYSCLK_CLKSTATUS_IRC10K_STB_Msk | SYSCLK_CLKSTATUS_XTL32K_STB_Msk);

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    SYSCLK->CLKSEL0 = SYSCLK_CLKSEL0_STCLK_HCLK_DIV2 | SYSCLK_CLKSEL0_HCLK_PLL;

    /* Enable IP clock */        
    SYSCLK->APBCLK = SYSCLK_APBCLK_ADC_EN_Msk | SYSCLK_APBCLK_TMR0_EN_Msk |
                        SYSCLK_APBCLK_PWM45_EN_Msk | SYSCLK_APBCLK_RTC_EN_Msk |
                        SYSCLK_APBCLK_UART1_EN_Msk | SYSCLK_APBCLK_UART0_EN_Msk | 
                        SYSCLK_APBCLK_I2C0_EN_Msk | SYSCLK_APBCLK_WDT_EN_Msk;

    /* Select IP clock source */
    SYSCLK->CLKSEL1 = SYSCLK_CLKSEL1_UART_PLL | SYSCLK_CLKSEL1_TMR0_XTAL |
                         SYSCLK_CLKSEL1_ADC_XTAL;

    SYSCLK->CLKSEL2 = SYSCLK_CLKSEL2_PWM45_XTAL | SYSCLK_CLKSEL2_WWDT_IRC10K;
    	
    /* IP clock divider */
    SYSCLK->CLKDIV = SYSCLK_CLKDIV_ADC(1) |  // ADC clock = ADC clock source / 4
                        SYSCLK_CLKDIV_UART(1) | // UART clock = UART clock source / 1
                        SYSCLK_CLKDIV_HCLK(1);  // HCLK clock = HCLK clock source / 1


    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    //SystemCoreClockUpdate(); 
    PllClock        = PLL_CLOCK;            // PLL
    SystemCoreClock = PLL_CLOCK / 1;        // HCLK
    CyclesPerUs     = PLL_CLOCK / 1000000;  // For SYS_SysTickDelay()


/*---------------------------------------------------------------------------------------------------------*/
/* Init I/O Multi-function                                                                                 */
/*---------------------------------------------------------------------------------------------------------*/

    /* 
       PA.0, PA.1, PA.2 for ADC   - analog to digital converter channel 0, 1, 2.
       PA.8, PA.9 for I2C0 - NFC
    */
    SYS->GPA_MFP = SYS_GPA_MFP_PA0_ADC0 | SYS_GPA_MFP_PA1_ADC1 |
                   SYS_GPA_MFP_PA2_ADC2 |
                   SYS_GPA_MFP_PA8_SDA0 | SYS_GPA_MFP_PA9_SCL0;
	
    /* 
       PB.0, PB.1  for UART0  - RXD0, TXD0
       PB.5, PB.4  for UART1 - TXD1, RXD1
       PB.8        for RUN
       PB.9, PB.10 for RELAY - 1, 2
       PB.11       for PWM - 1
    */
    SYS->GPB_MFP = SYS_GPB_MFP_PB0_RXD0 | SYS_GPB_MFP_PB1_TXD0 | 
                   SYS_GPB_MFP_PB4_RXD1 | SYS_GPB_MFP_PB5_TXD1 |
                   SYS_GPB_MFP_PB8_GPIO | 
                   SYS_GPB_MFP_PB9_GPIO | SYS_GPB_MFP_PB10_GPIO | 
                   SYS_GPB_MFP_PB11_PWM4;

	/* 
       PA.2 for PLC   - PLC power.
    */
    SYS->GPC_MFP = SYS_GPC_MFP_PC2_GPIO;
    
    /* 
       PE.5        for PWM - 2
    */
    SYS->GPE_MFP = SYS_GPE_MFP_PE5_PWM5;
                     
    
    SYS->ALT_MFP = SYS_ALT_MFP_PA0_ADC0 |
				   SYS_ALT_MFP_PA1_ADC1 | SYS_ALT_MFP_PA2_ADC2 |
				   SYS_ALT_MFP_PA8_SDA0 | SYS_ALT_MFP_PA9_SCL0 |
				   SYS_ALT_MFP_PB8_GPIO | 
                   SYS_ALT_MFP_PB9_GPIO | SYS_ALT_MFP_PB10_GPIO | 
                   SYS_ALT_MFP_PB11_PWM4|
                   SYS_ALT_MFP_PE5_PWM5 |
                   SYS_ALT_MFP_PC2_GPIO;

    SYS->ALT_MFP1= SYS_ALT_MFP1_PA0_ADC0| 
				   SYS_ALT_MFP1_PA1_ADC1 | SYS_ALT_MFP1_PA2_ADC2 |
				   SYS_ALT_MFP1_PA8_SDA0 | SYS_ALT_MFP1_PA9_SCL0 |
				   SYS_ALT_MFP1_PB8_GPIO | 
                   SYS_ALT_MFP1_PB9_GPIO | SYS_ALT_MFP1_PB10_GPIO | 
                   SYS_ALT_MFP1_PB11_PWM4 |
                   SYS_ALT_MFP1_PE5_PWM5 |
                   SYS_ALT_MFP1_PC2_GPIO;
    /* Lock protected registers */
    SYS_LockReg();
                         
}




