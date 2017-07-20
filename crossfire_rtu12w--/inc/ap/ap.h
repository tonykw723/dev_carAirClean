/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __AP_H_
#define __AP_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

typedef struct
{
	//init function for application
	xt_void		(*ApInit)(void);

	//infinite loop function for application
	xt_void		(*ApTask)(xt_void* p);

	//application handle
	os_task_t	apTaskId;

	//application message queue handle
	os_queue_t	apTaskMsgQId;
} ap_t;

typedef struct
{
	//application id
	xt_u16			apId;

	//runtime config pointer for application
	ap_t*			pApInstance;

	//application name
	const xt_s8*	apName;

	//application priority
	xt_u8			apPriority;

	//application stack size
	xt_u16			apStackSize;
} ap_info_t;

typedef struct
{
	//application command for message
	xt_u16	apCmdId;

	//connection id for message
	xt_u16	connId;

	//communication device id for message
	xt_u16	devId;

	//application command id for message
	xt_u16	proApId;
} msg_info_t;

#define PRO_AP_PACKAGE_MAX (256+32)

typedef  struct 
{
	//common application level protocol data
	xt_u8				proApData[PRO_AP_PACKAGE_MAX];
	
	//common application level protocol length
	xt_u8				proApDataLen;
} pro_ap_info_t;

typedef struct
{
	//store message information
	msg_info_t		msgInfo;

	//store communication device info
	xt_dev_info_t	commInfo;

	//store application level protocol data
	pro_ap_info_t	proAp;
} ap_msg_t;

typedef struct {
	//application command id
	xt_u16 cmd;

	//process function for application command id
	xt_void (*processFunc)(xt_void* p);
} ap_cmd_func_map_t;

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __AP_H_

