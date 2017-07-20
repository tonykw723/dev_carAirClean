/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/

#ifndef __XTOS_TASK_H_
#define __XTOS_TASK_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

//reference to  "task.h" in FreeRTOS

#define TaskCreateXTOS( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask ) xTaskCreate( pvTaskCode, pcName, usStackDepth, pvParameters, uxPriority, pxCreatedTask )

#define TaskSleepMs(x)		vTaskDelay(x)

#define TaskStartXTOS()		vTaskStartScheduler()

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XTOS_TASK_H_

