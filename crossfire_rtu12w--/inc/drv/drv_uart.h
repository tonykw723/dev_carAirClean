/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __DRV_UART_H_
#define __DRV_UART_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

//todo: add mutex for each drv (or in bsp module)

#define drvUartInit					NULL
#define drvUartOpen					NULL
#define drvUartClose				NULL
#define drvUartWrite				XtBspCtrlWrite
#define drvUartRead					XtBspCtrlRead
#define drvUartCtrl					NULL

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __DRV_UART_H_s

