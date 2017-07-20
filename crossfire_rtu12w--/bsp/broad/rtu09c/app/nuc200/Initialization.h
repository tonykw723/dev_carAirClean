/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright(c) 2010 Nuvoton Technology Corp. All rights reserved.                                         */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
#ifndef _INITIALIZATION_H
#define _INITIALIZATION_H

extern void SYS_Init(void);
extern void UART0_Init(void);
extern void UART1_Init(xt_u32 buadrate);
extern void GPIO_Init(void);
extern void TMR0_Init(void);
extern void TMR1_Init(void);
extern void PWMA_Init(void);
extern void PWMB_Init(void);
extern void CKO_Init(void);

#endif

