#ifndef _GLOBAL_PARA_H
#define _GLOBAL_PARA_H


#define PWM_DECAY_CUT         0   //��˥��ֵ��ֱ�Ӽ���
#define PWM_DECAY_RATIO      1   //��˥������������
#define PWM_DECAY_MOD         PWM_DECAY_RATIO
typedef enum
{
	DIM_ROUGH,//�ֵ�
	DIM_FINE,//����
}dim_enum;

typedef enum
{
	ERR_NONE=0,
	ERR_FUSE=1,ERR_RELAY=2,ERR_CAPACITOR=4,ERR_LAMP_FAILURE=7,
	ERR_V_LEAKAGE=8,ERR_POLE_INCLINE=9,ERR_POLE_DORR=10,ERR_COMMUNICATION=0xe1,
}err_value_enum;

#define ONE_PRO_ACTIONNUM	7// һ�������·����������

typedef enum
{
	RTU_MANU_TASK = 1,//�ֶ�
	RTU_MANU_AND_AUTO_TASK,//����һ��
	RTU_AUTO_TASK,//�Զ�
}rtu_task_type_m;

typedef enum
{
	RTU_RTC_TYPE=0,
	RTU_DELAY_TYPE,
}rtu_task_time_type_m;

typedef enum
{
	NO=0,	YES,   ERR,OTHER,DONE
} comfirm_enum_t;

typedef enum
{
	PWM_A=1,PWM_N=2
} PWM_enum_t;

typedef enum
{
	NONE_LAMP,SODIUM_LAMP=01,LED_LAMP=04,SOLAR_LAMP=10,
} LAMP_enum_t;

typedef enum
{
	LAMP_DRIGHT,LAMP_DARK,
}LAMP_light_enum_t;

typedef enum
{
	HAND_MODE,AUTO_MODE,
}runMode_enum_t;

typedef enum
{
	SET_DEFAULT,
	RESET_PARA,
	WRITE_PARA,
} para_value_n;

typedef struct
{
	xt_u8	light;// ��ͷ��
	xt_u8	actionType;// ִ�ж���
	xt_u8	dimming;// ������
}PACKED ap_rtu_action_t;

#define HEART_BEAT_PERIOD 120
#define HEART_BEAT_CNT_MAX 3
#define LOGIN_PERIOD 30
#define LOGIN_CNT_MAX 3
typedef struct 
{
	xt_u8 dimming_now_100[LAMP_NUM_MAX];//��ǰ�����ʣ���Χ0~100
	xt_u8 dimming_last_100[LAMP_NUM_MAX];//�ϴε����ʣ���Χ0~100 	
	xt_u8 dimming_target_100[LAMP_NUM_MAX];//Ŀ������ʣ���Χ0~100 
	xt_u8 dimming_for_lookup[LAMP_NUM_MAX];//�ϴε����ʣ���Χ0~100 	
	dim_enum dimming_kind;//���ⷽʽ
	xt_u8 status_now;//��ǰ״̬
	xt_u8 status_last;	
	LAMP_light_enum_t flag_lamp_lighting[LAMP_NUM_MAX];
	ap_rtu_action_t actionBackUp;
	xt_u8 heartBeatPeriod;
	xt_u8 heartBeatCnt;
	xt_u8 heartBeatErrCnt;
	xt_u8 loginPeriod;
	xt_u8 loginCnt;
	xt_u8 loginErrCnt;
	xt_u16 DLZ_timeOut;
	xt_u16 taskRunTime;
	xt_u8   flag_taskStart;
	xt_u8   flag_taskStart_backup;
	
	xt_u32 flag_dimming_start:1;//���⿪ʼ��־
	xt_u32 flag_onine:1;
	xt_u32 flag_onlineCmdStart:1;
	xt_u32 flag_heartBeatCmdStart:1;
	xt_u32 flag_fanOpened:1;
	xt_u32 flag_fanAdjLevel:3;
	xt_u32 flag_DLZ_Opened:1;
	xt_u32 flag_powerEn:1;
	xt_u32 flag_DLED:1;
	xt_u32 flag_FLED:1;
	xt_u32 flag_doReport:1;
	xt_u32 flag_carPowerOn:1;
	xt_u32 flag_FLED_en:1;
	xt_u32 flag_DLED_en:1;
	
	
}PACKED rtu_running_para_t;


typedef /*__packed*/ struct
{
	comfirm_enum_t      flag_dev_ini;
	
}PACKED dev_para_t;



//######################flash info#######################################
//---------------------------------------------------------------------
#define FLASH_RTU_CFG_MAX_SIZE 150
#define FLASH_RTU_CFG_REAL_SIZE 150
#define IFLASH_RTU_CFG_START_ADDR_OFFSET 0  //�ڲ�EEPROM ���ÿ�ʼ��ַ

typedef struct 
{
	xt_u16 have_writed;
	//---------��Ŀ�Ͱ汾-----------
	xt_u8 project_name[6];//��Ŀ��
	xt_u8 rtu_sn[4];            //ϵ�к�
	xt_u8      linkType;
	xt_u8      severIp[32];
	xt_u16    portNum;
	xt_u8      apn[16];
	xt_u8      telephone[20];
	//--------��������--------------
	PWM_enum_t pwm_type;//PWM���ͣ�����/����
	xt_u8 pwm_num; //�����߸���
	xt_u16 pwm_frequence;//PWMƵ��

	//-------PLC��ز���-----------
	xt_u8 sever_uid[6];               //zigbee���鲥�͹㲥���õ�
	xt_u8 rtuOrPlc_uid_own[6];  //uid��ַ
	xt_u8 rtuOrPlc_domain[2];   //���
	xt_u8 rtuOrPlc_group[3];     //���
	//xt_u8 dev_module_id;
	//--------���Ӷ������-------
	xt_u8 runMode;
} rtu_cfg_para_t;

typedef union
{
	xt_u8 buf[FLASH_RTU_CFG_REAL_SIZE];
	rtu_cfg_para_t value;
} rtu_config_t;


//######################flash info#######################################
//---------------------------------------------------------------------
#define FLASH_RTU_TASK_MAX_SIZE 150
#define FLASH_RTU_TASK_REAL_SIZE 15
#define IFLASH_RTU_TASK_START_ADDR_OFFSET (IFLASH_RTU_CFG_START_ADDR_OFFSET+FLASH_RTU_CFG_MAX_SIZE)  //�ڲ�EEPROM ���ÿ�ʼ��ַ

typedef struct 
{
	xt_u16 have_writed;
	xt_u8      taskId;
	xt_u8      whichDay1;
	xt_u8      hour1;
	xt_u8      minute1;
	xt_u16    howmanyMinutes1;
	xt_u8      action1;
	xt_u8      whichDay2;
	xt_u8      hour2;
	xt_u8      minute2;
	xt_u16    howmanyMinutes2;
	xt_u8      action2;
	xt_u8      whichDay3;
	xt_u8      hour3;
	xt_u8      minute3;
	xt_u16    howmanyMinutes3;
	xt_u8      action3;
	xt_u8      whichDay4;
	xt_u8      hour4;
	xt_u8      minute4;
	xt_u16    howmanyMinutes4;
	xt_u8      action4;

} rtu_taskv_t;

typedef union
{
	xt_u8 buf[FLASH_RTU_TASK_REAL_SIZE];
	rtu_taskv_t value;
} rtu_task_t;


typedef struct
{
	
	rtu_config_t flashConfig;//����������Ϣ
	rtu_task_t flashTask;
	rtu_running_para_t  runningInfo;//���в���
	dev_para_t devPara;
	xt_u32 connCount;//���Ӽ���
	xt_u32 lastTime;//�ϴαȽϵ�ʱ��
} rtu_para_t;


#endif

