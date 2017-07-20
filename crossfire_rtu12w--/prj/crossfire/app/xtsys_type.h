/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __XTSYS_TYPE_H_
#define __XTSYS_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "timers.h"

typedef char xt_s8;
typedef unsigned char xt_u8;
typedef short xt_s16;
typedef unsigned short xt_u16;
typedef int xt_s32;
typedef unsigned int xt_u32;
typedef long long xt_s64;
typedef char * xt_string;
typedef unsigned char xt_bool;
typedef void xt_void;
typedef float xt_float;
typedef double xt_double;
typedef xt_u32 xt_time_t;

//for os type
typedef xQueueHandle 	os_mutex_t;
typedef xQueueHandle	os_sem_t;
typedef xTaskHandle		os_task_t;
typedef xQueueHandle	os_queue_t;
typedef xTimerHandle	os_timer_t;

//#define NULL (0)

#define TRUE 1
#define FALSE 0

#define SUCCESS_XT 1
#define FAILURE_XT 0

#define OS_ASSERT(condition)                                                \
	do                                                                      \
	{                                                                       \
		if ((condition) != TRUE)                                            \
		{                                                                   \
			PrintfXTOS("\nassert in %s %d\n", __FILE__, __LINE__);				\
			taskDISABLE_INTERRUPTS();										\
			for( ;; );														\
		}                                                                   \
	}                                                                       \
	while (0)

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XTSYS_TYPE_H_

