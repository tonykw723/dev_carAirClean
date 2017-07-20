/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __CONN_UART_H_
#define __CONN_UART_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include "drv_uart.h"

#define connUartInit				drvUartInit
#define connUartOpen				drvUartOpen
#define connUartClose				drvUartClose
#define connUartWrite				drvUartWrite
#define connUartRead				drvUartRead
#define connUartRespWrite			NULL
#define connUartRespRead			NULL
#define connUartCtrl				drvUartCtrl

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __CONN_UART_H_

