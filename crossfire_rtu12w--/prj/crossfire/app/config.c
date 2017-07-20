/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>

#include "xtsys_type.h"
#include "xtsys_cfg.h"

#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"
#include "xtos_timer.h"

#include "xt_bsp_rtu09c.h"

#include "drv_uart.h"

#include "xt_conn.h"
#include "conn_uart.h"
#include "conn_msg.h"

#include "xt_dev.h"
#include "dev_wcdma_youfang.h"
#include "pro_dev_iot.h"

#include "global_para.h"
#include "pro_ap.h"
#include "xt_pro_ap.h"
#include "pro_ap_iot.h"
#include "pro_ap_hz.h"

#include "ap.h"
#include "ap_comm.h"
#include "ap_rtu.h"
#include "ap_measure.h"
#include "misc_timer.h"

#include "config.h"



//task priority list
//os_idle		0(tskIDLE_PRIORITY)
//os_timer		1
typedef enum
{
	AP_RTU_PRIO = tskIDLE_PRIORITY + 2,
	AP_COMM_PRIO,
	AP_MEASURE_PRIO,
} ap_task_priority_n;

typedef enum
{
	AP_RTU_STACKSIZE =		configMINIMAL_STACK_SIZE,
	AP_COMM_STACKSIZE =		configMINIMAL_STACK_SIZE*2,
	AP_MEASURE_STACKSIZE =	configMINIMAL_STACK_SIZE,
} ap_task_stack_n;

static ap_info_t gApList[] =
{
	{AP_RTU, NULL, "ap_rtu", AP_RTU_PRIO, AP_RTU_STACKSIZE},
	{AP_COMM, NULL, "ap_comm", AP_COMM_PRIO, AP_COMM_STACKSIZE},
	{AP_MEASURE, NULL, "ap_measure", AP_MEASURE_PRIO, AP_MEASURE_STACKSIZE},
};

xt_conn_t gConnList[CONN_MAX] =
{
	{
		CONN_UART,
		connUartInit,
		connUartOpen,
		connUartClose,
		connUartWrite,
		connUartRead,
		connUartRespWrite,
		connUartRespRead,
		connUartCtrl,
	},
	/*{
		CONN_MSG,
		ConnMsgInit,
		ConnMsgOpen,
		ConnMsgClose,
		ConnMsgWrite,
		ConnMsgRead,
		ConnMsgRespWrite,
		ConnMsgRespRead,
		ConnMsgCtrl,
	},*/
};

xt_dev_t gDevList[DEV_MAX] =
{
/*
	{
		CONN_UART,
		DevRisecommInit,
		NULL,
		NULL,
		DevRisecommSend,
		DevRisecommRecv,
		NULL,
	},*/
	
	{
		CONN_UART,
		DevWcdmaYoufangInit,
		NULL,
		NULL,
		DevWcdmaYoufangSend,
		DevWcdmaYoufangRecv,
		NULL,
	},

};


xt_pro_ap_t gProApList[PRO_AP_MAX] =
{
	{
		PRO_AP_IOT,
		NULL,
		NULL,
		NULL,
		ProApIotPack,
		ProApIotParse,
		NULL,
	},/*
	{
		PRO_AP_HZ,
		NULL,
		NULL,
		NULL,
		ProApHzPack,
		ProApHzParse,
		NULL,
	},*/
};

os_queue_t ApConfigGetMsgQ(xt_u8 apId)
{
	return gApList[apId].pApInstance->apTaskMsgQId;
}



xt_void ApConfig()
{
	xt_u8 i = 0;
	xt_u8 apCnt = 0;
	ap_info_t* pApInfo = NULL;

	for (i = 0; i < sizeof(gConnList)/sizeof(xt_conn_t); i++)
	{
		if(gConnList[i].xConnInit != NULL)
			gConnList[i].xConnInit();
	}

	gApList[AP_RTU].pApInstance = (ap_t*)ApRTUGetInstance();
	gApList[AP_COMM].pApInstance = (ap_t*)ApCommGetInstance();
	gApList[AP_MEASURE].pApInstance = (ap_t*)ApMeasureGetInstance();
	apCnt = AP_MAX;

	i = 0;
	do {
		pApInfo = gApList + i++;
		OS_ASSERT(pApInfo->pApInstance != NULL);

		pApInfo->pApInstance->ApInit();

		if (pApInfo->pApInstance->ApTask != NULL) {
	
			pApInfo->pApInstance->apTaskMsgQId = QueueCreateXTOS(AP_MSGQ_LENGTH, sizeof(ap_msg_t));

			TaskCreateXTOS(pApInfo->pApInstance->ApTask, pApInfo->apName, pApInfo->apStackSize, 
				NULL, pApInfo->apPriority,&pApInfo->pApInstance->apTaskId);

		}
	} while (i < apCnt);

	MiscTimerInit();
	
	PrintfXTOS("#################os start###############\n");
	TaskStartXTOS();
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
