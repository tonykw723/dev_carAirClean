/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/

#ifndef __XTOS_MSG_H_
#define __XTOS_MSG_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

//reference to "queue.h" in FreeRTOS

#define QueueCreateXTOS( uxQueueLength, uxItemSize )			xQueueCreate( uxQueueLength, uxItemSize )

#define QueueSendXTOS( xQueue, pvItemToQueue )					xQueueSend( xQueue, pvItemToQueue, 0U )

#define QueueReceiveXTOS( xQueue, pvBuffer, xTicksToWait )		xQueueReceive( xQueue, pvBuffer, xTicksToWait )

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XTOS_MSG_H_

