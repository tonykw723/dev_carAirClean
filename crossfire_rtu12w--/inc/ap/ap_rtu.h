/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __AP_RTU_H_
#define __AP_RTU_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

typedef struct
{
	//store runtime config pointer for ap_rtu
	ap_t	instance;
} ap_rtu_priv_data_t;

typedef enum
{
	//ap_comm command for RTU process
	AP_RTU_CMD_COMM = 0x200,
	AP_RTU_CMD_485,
	AP_RTU_CMD_END,
} ap_rtu_cmd_n;



#define RTU_ACTION_MAX_NUM	7
typedef union {
	unsigned short t;
	struct {
		unsigned char hour;
		unsigned char min;
	}PACKED ch;
}PACKED action_time_t;

typedef union {
	unsigned char type;
	struct {
		unsigned char weekday:1;//周日//low
		unsigned char monday:1;//周一//high
		unsigned char thes:1;//周二
		unsigned char wed:1;//周三
		unsigned char thur:1;//周四
		unsigned char fri:1;//周五
		unsigned char sat:1;//周六
		unsigned char dt:1;//模式 0:RTC模式1:延时模式
	}PACKED ch;
}PACKED action_time_type_t;

typedef struct
{
	action_time_t	actionTime;// 执行时间
	xt_u8	light;// 灯头号
	xt_u8	actionType;// 执行动作
	xt_u8	dimming;// 调光率
}PACKED ap_rtu_light_action_t;

typedef  struct 
{
	xt_u8			cmd;
	xt_u8      		number;// 任务序号
	xt_u8      		reserver;// 预留
	action_time_type_t	timeType;// 时间类型
	xt_u8			actionNum;// 动作总数
	ap_rtu_light_action_t	action[ONE_PRO_ACTIONNUM];
}PACKED ap_rtu_task_set_t;

typedef struct
{
	xt_u16	head;
	ap_rtu_task_set_t	taskSet[RTU_ACTION_MAX_NUM];
	xt_u16	end;
}PACKED ap_rtu_task_in_eerom_t;

//get runtime config pointer for ap_rtu
ap_t* ApRTUGetInstance(void);

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __AP_RTU_H_

