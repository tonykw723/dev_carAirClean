/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __CONFIG_H_
#define __CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#define RTU12
#define UART_POLL_TIMES		20
#define UART_POLL_DELAY_MS	1
#define USE_IWDG_AS_WDG
//#define USE_WWDG_AS_WDG

typedef enum
{
	CONN_UART = 0,
	CONN_MSG,
	CONN_MAX = 1,
} conn_id_n;

typedef enum
{
	//DEV_RISE=0 ,
	DEV_ZIGBEE=0, 
	DEV_MAX,
} dev_id_n;

typedef enum
{
	PRO_AP_IOT = 0,
	//PRO_AP_HZ,
	PRO_AP_MAX,
} pro_ap_id_n;

typedef enum
{
	AP_RTU = 0,
	AP_COMM,
	AP_MEASURE,
	AP_MAX,
} ap_id_n;

//max message queue length for each application
#define AP_MSGQ_LENGTH		(1)

//timeout for message queue in application
#define AP_RTU_MSG_TIMEOUT		(10)
#define AP_COMM_MSG_TIMEOUT		(10)
#define AP_MEASURE_MSG_TIMEOUT	(10)

//get application message queue by id
os_queue_t ApConfigGetMsgQ(xt_u8 apId);

//config project API
xt_void ApConfig(void);

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __CONFIG_H_

