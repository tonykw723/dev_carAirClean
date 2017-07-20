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
		unsigned char weekday:1;//����//low
		unsigned char monday:1;//��һ//high
		unsigned char thes:1;//�ܶ�
		unsigned char wed:1;//����
		unsigned char thur:1;//����
		unsigned char fri:1;//����
		unsigned char sat:1;//����
		unsigned char dt:1;//ģʽ 0:RTCģʽ1:��ʱģʽ
	}PACKED ch;
}PACKED action_time_type_t;

typedef struct
{
	action_time_t	actionTime;// ִ��ʱ��
	xt_u8	light;// ��ͷ��
	xt_u8	actionType;// ִ�ж���
	xt_u8	dimming;// ������
}PACKED ap_rtu_light_action_t;

typedef  struct 
{
	xt_u8			cmd;
	xt_u8      		number;// �������
	xt_u8      		reserver;// Ԥ��
	action_time_type_t	timeType;// ʱ������
	xt_u8			actionNum;// ��������
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

