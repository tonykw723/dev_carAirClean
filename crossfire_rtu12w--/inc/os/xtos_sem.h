/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/

#ifndef __XTOS_SEM_H_
#define __XTOS_SEM_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

//reference to  "semphr.h" in FreeRTOS

#define MutexCreateXTOS( xSemaphore )							vSemaphoreCreateBinary( xSemaphore )
#define MutexLockXTOS( xSemaphore )								xSemaphoreTake( xSemaphore, portMAX_DELAY )
#define MutexUnlockXTOS( xSemaphore )							xSemaphoreGive( xSemaphore )
#define MutexDeleteXTOS( xSemaphore )							vSemaphoreDelete( xSemaphore )

#define SemCreateXTOS( uxMaxCount, uxInitialCount )				xSemaphoreCreateCounting( uxMaxCount, uxInitialCount )
#define SemTakeXTOS( xSemaphore )								xSemaphoreTake( xSemaphore, portMAX_DELAY )
#define SemGiveXTOS( xSemaphore )								xSemaphoreGive( xSemaphore )
#define SemDeleteXTOS( xSemaphore )								vSemaphoreDelete( xSemaphore )

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XTOS_SEM_H_

