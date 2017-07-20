#ifndef _DEV_SOLAR_PHILI_H
#define _DEV_SOLAR_PHILI_H

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus
//#define PHILI_SOLAR_1TH//第一期太阳能控制器
#define PHILI_SOLAR_2TH//第二期太阳能控制器new solar controller
//--------------------phili protocol-----------------------------------------
#define SD_HCU_VALUE 0X6
#define SD_EPS_VALUE 0X7
#define SD_DESS_VALUE 0X8
#define SD_SCS_VALUE 0X9

typedef enum
{
	CHARGING, CHARGING_FAILE, NO_CHARGING
}chargeing_status_enum;

typedef enum
{
	LV_70TO100,LV_40TO69,LV_00TO39, LOW_VALTAGE
}battery_status_enum;

typedef enum
{
	LOAD_ON=0,OVER_LOAD_SHORT,LOAD_OFF,LOAD_NULL
}load_status_enum;

typedef enum
{
	SOLAR_SELFCTRL=0,LCU_CTRL=1,NULL_CTRL=2,
}ctrl_mode_enum;

typedef enum
{
	PRO_NML,PRO_HCU
}pro_enum;

typedef struct
{
	xt_u32    time_stamp;
	xt_u32	pv_v;
	xt_u32	pv_i;
	xt_u32	battery_v;
	xt_u32	battery_i;
	xt_u32    battery_tp;//...
	xt_u32    daily_charged_enery;
	xt_u32    daily_discharged_enery;
	xt_u32    load_v;
	xt_u32    load_i;
	xt_u32    controller_tp;//...
	xt_u32    light_length;//....
	xt_u8      battery_ratio;//DISCARD
	xt_u8      battery_charge_status;
	xt_u8      battery_capacity_statas;
	xt_u8      load_status;
	xt_u32    battery_present_capacity;
	xt_u32    battery_static_capacity;	
	xt_u8      sw_status;
	xt_u32    dim_v;
	xt_u8      is_setOk;
	xt_u8      ctrl_mode;
}PACKED solar_NML_t;

typedef struct
{
	xt_u32    time_stamp;
	xt_u32	pv_v;
	xt_u32	pv_i;
	xt_u32	battery_v;
	xt_u32	battery_i;
	xt_u32    battery_tp;//...
	xt_u32    daily_charged_enery;
	xt_u32    daily_discharged_enery;
	xt_u32    load_v;
	xt_u32    load_i;
	xt_u32    controller_tp;//...
	xt_u32    light_length;//....
	xt_u8      battery_ratio;//DISCARD
	xt_u8      battery_charge_status;
	xt_u8      battery_capacity_statas;
	xt_u8      load_status;
	xt_u32    battery_present_capacity;
	xt_u32    battery_static_capacity;	
	xt_u8      sw_status;
	xt_u32    dim_v;
}PACKED solar_NMLdata_store_t;
//------------new solar controller------------------
typedef struct
{
	
	xt_u32    timeStamp;
	xt_u8      devType;
	//xt_u8      loadSwitch;
	xt_u32    pvVoltage;
	xt_u32    pvCurrent;
	xt_u32    batteryVoltage;
	xt_u32    batteryChargeCurrent;
	xt_u32    batteryDischargeCurrent;
	xt_u8      batteryTemp;
	xt_u8      controllerTemp;
	xt_u8      dischargeMode;
	xt_u16      status;
}PACKED solar_HCU_t;

typedef struct
{
	
	xt_u32    timeStamp;
	xt_u8      devType;
	xt_u32    pvVoltage;
	xt_u32    pvCurrent;
	xt_u32    batteryVoltage;
	xt_u32    batteryDischargeCurrent;
	xt_u32    batteryChargeCurrent;
	xt_u16    inverterVoltage;
	xt_u16    inverterFrequency;
	xt_u8      status0;
	xt_u8      status1;
	xt_u8      status2;
	xt_u8      status3;
	xt_u8      status4;
	xt_u8      status5;
}PACKED solar_EPS_t;


typedef struct
{
	
	xt_u32    timeStamp;
	xt_u8      devType;
	xt_u32    batteryTotalChargeEnergy;
	xt_u32    batteryTotalDischargeEnergy;
	xt_u16    batterySoc;//capcity ratio
	xt_u16    batteryVoltage;
	xt_u16    batteryCurrent;
	xt_u16    batteryWattage;
	xt_u16    batterySoh;
	xt_u16    pcsTotalActivePower;
	xt_u16    pcsTotalReactivePower;
	xt_u16    pcsTotalCurrent;
	xt_u16    pcsPhaseA_current;
	xt_u16    pcsPhaseB_current;
	xt_u16    pcsPhaseC_current;
	xt_u16    pcsPhaseA_voltage;
	xt_u16    pcsPhaseB_voltage;
	xt_u16    pcsPhaseC_voltage;	
	xt_u16    pcsPhaseA_activePower;
	xt_u16    pcsPhaseB_activePower;
	xt_u16    pcsPhaseC_activePower;		
	xt_u16    pcsStatus;
	xt_u16    pcsAlarm;
}PACKED solar_DESS_t;

typedef struct
{
	
	xt_u32    timeStamp;
	xt_u8      devType;
	xt_u32    batteryTotalChargeEnergy;
	xt_u32    batteryTotalDischargeEnergy;
	xt_u16    batterySoc;//capcity ratio
	xt_u16    batteryVoltage;
	xt_u16    batteryCurrent;
	xt_u16    batteryWattage;
	xt_u16    batteryTemp;
	xt_u16    pcsTotalActivePower;
	xt_u32    pcsTotalEnergy;
	xt_u16    pcsTotalCurrent;
	xt_u16    pcsPhaseA_current;
	xt_u16    pcsPhaseB_current;
	xt_u16    pcsPhaseC_current;
	xt_u16    pcsPhaseA_voltage;
	xt_u16    pcsPhaseB_voltage;
	xt_u16    pcsPhaseC_voltage;	
	xt_u16    pcsPhaseA_activePower;
	xt_u16    pcsPhaseB_activePower;
	xt_u16    pcsPhaseC_activePower;		
	xt_u16    pcsStatus;
}PACKED solar_SCS_t;

typedef struct
{
	//5byte
	xt_u32    timeStamp;
	xt_u8      devType;
	xt_u16    lightLength;//min
	//PV-4byte
	xt_u16    pvVoltage;//10mV
	xt_u16    pvCurrent;//10mA
	//BATTERY-29byte
	xt_u32    batteryTotalChargeEnergy;//wh
	xt_u32    batteryTotalDischargeEnergy;//wh
	xt_u16    batteryDailyChargedCapacity;//10mAh
	xt_u16    batteryDailyDischargedCapacity;//10mAh
	xt_u16    batteryVoltage;//10mV
	xt_u16    batteryChargeCurrent;//10mA
	xt_u16    batteryDischargeCurrent;//10mA
	xt_u16    batteryWattage;//1w
	xt_u16    batteryPresentCapacity;//10mAH
	xt_u16    batteryStaticCapacity;//10mAH
	xt_u8      batterySoc;//capcity ratio
	xt_u8      batterySoh;//helthy ratio
	xt_u8    	batteryTemp;
	xt_u8      batteryChargeStatus;
	xt_u8      batteryCapacityStatas;
	//LOAD-5byte
	xt_u16    loadVoltage;//10mV
	xt_u16    loadCurrent;//10mA
	xt_u8      loadStatus;
	//CONTROLLER-4byte
	xt_u8      controllerTemp;
	xt_u16    controllerStatus;//
	xt_u8      controllerDischargeMode;
	xt_u16    controllerAlarm;
	//AC-26byte
	xt_u32    acTotalEnery;//wH
	xt_u16    acTotalActivePower;//W
	xt_u16    acTotalRectivePower;//VA
	xt_u16    acTotalCurrent;//100mA
	xt_u16    acPhaseA_current;//100mA
	xt_u16    acPhaseB_current;//100mA
	xt_u16    acPhaseC_current;//100mA
	xt_u16    acPhaseA_voltage;//100mV
	xt_u16    acPhaseB_voltage;//100mV
	xt_u16    acPhaseC_voltage;//100mV
	xt_u16    acPhaseA_activePower;//W
	xt_u16    acPhaseB_activePower;//W
	xt_u16    acPhaseC_activePower;//W
	
}PACKED solar_All_t;

typedef union
{
	xt_u8 buf[100];
	solar_NML_t    normal;
	solar_HCU_t    hcu;
	solar_EPS_t     eps;
	solar_DESS_t  dess;
	solar_SCS_t    scs;
	solar_All_t       all;
}PACKED phili_solar_t;

typedef  struct 
{
	xt_u8 	batteryType;
	xt_u16 	batteryCapacity;//10mAh
	xt_u16    loadDisconnectVolage;//(LVD)10mV
	xt_u16    loadReconnectVolage;//10mV
	xt_u32 	cableVoltageDropCompensation;//mOhm
	xt_u16 	luminairesPowerRating;//额定功率W
	xt_u8 	rfModulePowerDuringLVD;//BOOL (Disable=0,Enable=1)
	xt_u16 	wakeupTime;//唤醒时间s
	xt_u16 	hystersisDelayTimeDusk;//黄昏延迟时间s
	xt_u16 	hystersisDelayTimeDawn;//黎明延迟时间s
	xt_u16	dawnDetectionLevel;//黎明判断电平10mV
	xt_u16 	duskDetectionLevel;//黄昏判断电平10mV
	xt_u8 	workingMode;//工作模式Enumeration [DuskToDawn,  RealTimeClock,GeneralPurpose ] 
	xt_u8 	dimmingMode;//调光模式Enumeration [NoDimming, D2D,DynaDim,RTC,Evening]
	xt_u8      rteEnableDisable;//运行任务使能	
	xt_u16 	rteTriggerVoltage;//运行触发电压10mV
	xt_u8 	rteDarkDays;//运行黑暗天数
	xt_u8 	rteTriggerBatteryCapacity;//运行触发的电池容量
	xt_u16 	dynaDimhistoryMinDurationOfNigh;//动态调光夜晚最小运行时长min
	xt_u16 	dynaDimhistoryMaxDurationOfNigh;//动态调光夜晚最大运行时长min
	xt_u16 	dynaDimhistoryMaxNighDaviation;//动态调光夜晚最大运行时长容差min
	xt_u32 	scene[6];//运行场景-hhmmddtt hh:hour,mm:minute,dd:dim value,tt:dim chage time
	xt_u32 	evenDuskDawn[12];	//一年黄昏事件时间-小时分钟，黎明时间-小时分钟
}PACKED sloarPhiliTask_t;


/*
typedef struct
{
	solar_NML_t controller_1th;
	philiSolarController_t controller_2th;
}PACKED phili_solar_t;*/

//-----------------------------------------------------------
typedef struct
{
	char       start;
	char       vertion;
	char       dot1;
	char       source;
	char       dot2;
	char       tranID;
	char       dot3;
	char       cmd;
	char       dot4;
}phili_solar_protocol_head;

typedef struct
{
	char       sign;
	char       crc8[2];
	char       end;
}phili_solar_protocol_tail;

typedef enum
{
	IDLE=0,
	MULTI_CMD=99,
	ON_OFF=100,
	DIM_LVL=101,
	DATE_TIME_SETTINGS =102,
	CONTROL_START_STOP=103,
	CONTROL_HEARTBEAT_COUNTER=104,
	PREPARE_FOR_SOFTWARE_UPGRADE=105,
	TEST_MODE_START_STOP=106,

	MONITOR_PV_VOLTAGE=200,
	MONITOR_PV_CURRENT=201,
	MONITOR_BATT_VOLTAGE=202,
	MONITOR_BATT_CURRENT=203,
	MONITOR_BATTERY_TEMPERATURE=204,
	MONITOR_BATTERY_PRESENT_CAPACITY=205,
	MONITOR_BATTERY_TOTAL_CAPACITY=206,
	MONITOR_DAILY_CHARGE_CAPACITY=207,
	MONITOR_DAILY_DISCHARGE_CAPACITY=208,
	MONITOR_LOAD_VOLTAGE =209,
	MONITOR_LOAD_CURRENT=210,
	MONITOR_CHARGE_CONTROLLER_TEMPERATURE=211,
	MONITOR_NIGHT_LENGTH=212,
	MONITOR_ENERGY_TRANSFER_PV_TO_BATT=213,
	MONITOR_BATTRY_STATUS =214,
	MONITOR_LOAD_STATUS =215,
	//------------power management para,PM:POWER_MANAGEMENT
	PM_BATTERY_TYPE =300,
	PM_BATTERY_CAPACITY =301,
	PM_LOAD_DISCONNECT_VOLTAGE =302,
	PM_LOAD_RECONNECT_VOLTAGE =303,
	PM_CABLE_VOLTAGE_DROP_COMPENSATION =304, 
	PM_LUMINAIRES_POWER_RATING =305,
	PM_RF_MODULE_POWER_DURING_LVD =306,
	PM_WAKEUP_TIME =307,
	//------------light management para,LM:LIGHT_MANAGEMENT
	LM_HYSTERSIS_DELAY_TIME_DUSK =500,
	LM_HYSTERSIS_DELAY_TIME_DAWN =501,
	LM_DAWN_DETECTION_LEVEL =502,
	LM_DUSK_DETECTION_LEVEL =503,
	LM_WORKING_MODE =504,
	LM_DIMMING_MODE =505,
	LM_RTE_ENABLE_DISABLE =506,
	LM_RTE_TRIGGER_VOLTAGE =507,
	LM_RTE_DARK_DAYS =508,
	LM_RTE_TRIGGER_BATTRY_CAPACITY=509,
	LM_DYNADIM_HISTORY_MINIMUM_DURATION_OF_NIGHT =511,
	LM_DYNADIM_HISTORY_MAXIMUM_DURATION_OF_NIGHT =512,
	LM_DYNADIM_HISTORY_MAXIMUM_NIGHT_DAVIATION =513,
	LM_SCENE =514,
	LM_DUSKDAWN_EVENT=515,	
	//------------new solar controller------------------
	SLAVE_MCHINE_ID=700,
	HCU_LOAD_TO_AC_OR_BATT=701,
	MONITOR_HCU_PV_PARA=709,
	MONITOR_HCU_BATT_PARA=710,
	MONITOR_HCU_DISCHARGE_MODE=702,
	MONITOR_HCU_STATUS=703,

	MONITOR_EPS_PARA=801,
	MONITOR_EPS_ALARM=802,

	MONITOR_DESS_BATT_PARA=803,
	MONITOR_DESS_PCS_PARA=804,
	MONITOR_DESS_STATUS=805,
	MONITOR_DESS_ALARM=806,

	MONITOR_SCS_BATT_PARA=901,
	MONITOR_SCS_PARA=902,
	MONITOR_SCS_STATUS=903,
}phili_prarName_e;

typedef enum
{
	ID_IDLE=0,
	ID_MULTI_CMD,
	ID_ON_OFF,// 2
	ID_DIM_LVL,// 3
	ID_DATE_TIME_SETTINGS ,///4
	ID_CONTROL_START_STOP,// 5
	ID_CONTROL_HEARTBEAT_COUNTER,// 6
	ID_PREPARE_FOR_SOFTWARE_UPGRADE,// 7
	ID_TEST_MODE_START_STOP,// 8

	ID_MONITOR_PV_VOLTAGE,
	ID_MONITOR_PV_CURRENT,
	ID_MONITOR_BATT_VOLTAGE,
	ID_MONITOR_BATT_CURRENT,
	ID_MONITOR_BATTERY_TEMPERATURE,
	ID_MONITOR_BATTERY_PRESENT_CAPACITY,
	ID_MONITOR_BATTERY_TOTAL_CAPACITY,
	ID_MONITOR_DAILY_CHARGE_CAPACITY,
	ID_MONITOR_DAILY_DISCHARGE_CAPACITY,
	ID_MONITOR_LOAD_VOLTAGE ,
	ID_MONITOR_LOAD_CURRENT,
	ID_MONITOR_CHARGE_CONTROLLER_TEMPERATURE,
	ID_MONITOR_NIGHT_LENGTH,
	ID_MONITOR_ENERGY_TRANSFER_PV_TO_BATT,
	ID_MONITOR_BATTRY_STATUS ,
	ID_MONITOR_LOAD_STATUS ,
	//------------power management para,PM:POWER_MANAGEMENT
	ID_PM_BATTERY_TYPE ,
	ID_PM_BATTERY_CAPACITY ,
	ID_PM_LOAD_DISCONNECT_VOLTAGE ,
	ID_PM_LOAD_RECONNECT_VOLTAGE ,
	ID_PM_CABLE_VOLTAGE_DROP_COMPENSATION , 
	ID_PM_LUMINAIRES_POWER_RATING ,
	ID_PM_RF_MODULE_POWER_DURING_LVD ,
	ID_PM_WAKEUP_TIME ,
	//------------light management para,LM:LIGHT_MANAGEMENT
	ID_LM_HYSTERSIS_DELAY_TIME_DUSK ,
	ID_LM_HYSTERSIS_DELAY_TIME_DAWN ,
	ID_LM_DAWN_DETECTION_LEVEL ,
	ID_LM_DUSK_DETECTION_LEVEL ,
	ID_LM_WORKING_MODE ,
	ID_LM_DIMMING_MODE ,
	ID_LM_RTE_ENABLE_DISABLE ,
	ID_LM_RTE_TRIGGER_VOLTAGE ,
	ID_LM_RTE_DARK_DAYS ,
	ID_LM_RTE_TRIGGER_BATTRY_CAPACITY,
	ID_LM_DYNADIM_HISTORY_MINIMUM_DURATION_OF_NIGHT ,
	ID_LM_DYNADIM_HISTORY_MAXIMUM_DURATION_OF_NIGHT ,
	ID_LM_DYNADIM_HISTORY_MAXIMUM_NIGHT_DAVIATION ,
	ID_LM_SCENE ,
	ID_LM_DUSKDAWN_EVENT,
	//----------------new solar controller-----------------
	ID_SLAVE_MCHINE_ID,//25
	ID_HCU_LOAD_TO_AC_OR_BATT,
	ID_MONITOR_HCU_PV_PARA,
	ID_MONITOR_HCU_BATT_PARA,
	ID_MONITOR_HCU_DISCHARGE_MODE,
	ID_MONITOR_HCU_STATUS,

	ID_MONITOR_EPS_PARA,
	ID_MONITOR_EPS_ALARM,

	ID_MONITOR_DESS_BATT_PARA,//33
	ID_MONITOR_DESS_PCS_PARA,
	ID_MONITOR_DESS_STATUS,
	ID_MONITOR_DESS_ALARM,

	ID_MONITOR_SCS_BATT_PARA,
	ID_MONITOR_SCS_PARA,
	ID_MONITOR_SCS_STATUS,
	ID_MAX,
	
}phili_prarName_id_e;


typedef enum
{
	SET_PARA = 1,
	GET_PARA = 10,
	GET_LOG = 11,
	GET_MULTI_PARA=12,
	REPLY_SET_PARA = 801,
	REPLY_GET_PARA = 810,
	REPLY_GET_LOG = 811,
	REPLY_GET_MULTI_PARA = 812
}phili_cmd_e;

typedef struct
{
	xt_u8   vertion;
	xt_u16 source;
	xt_u16 tranID;
	xt_u16 cmd;
	xt_u16 paraName;
	xt_u8    dataLen;
	xt_u8    dataBuf[150];	
	
}phili_solar_protocol_t;

typedef enum
{
	SD_NORMAL_ID=0,SD_HCU_ID,SD_EPS_ID,SD_DESS_ID,S_SCS_ID,SD_NORMAL_GET_TASK_ID,SD_NORMAL_SET_TASK_ID,SD_ID_MAX
}phili_solar_dev_enum;


typedef struct
{
	xt_u16 cmd;
	xt_u8  *st;
	//xt_u8 const *val;
	xt_u8 stSize;
	xt_u8 valSize;
}phili_solar_para_t;

typedef struct
{
	xt_u8 *process;
	xt_u8 size;
	phili_solar_para_t *para;
}phili_sloar_polling_t;


typedef struct
{
	xt_u8 *controller;
	xt_u8 size;
}phili_sloar_controller_t;
//---------------------------------------------------------------------------------

char halfByte2Ascii(xt_u8 halfByteTemp,xt_u8 is_zero_ascii_retain);
void pack_Hex2Ascii(xt_u32 value,xt_u8 *index,xt_u8 *pt,xt_u8 is_zero_ascii_retain,xt_u8 valueSize);
void packMulitPara(xt_u32 *val,xt_u8 *pt,xt_u8 size);
xt_void  DevSolarPhiliInit(void);
xt_s32	DevSolarPhiliSend(xt_u8* pData, xt_s32 dataLen);
xt_s32	DevSolarPhiliRecv(xt_u8* pData, xt_u32 maxLen);
xt_void  DevSolarPhiliCtrl(xt_u8* src_pData,xt_u8* dec_pData);
xt_s32 DevSolarPhiliWriteAndRead(xt_u8 id_temp,xt_u8 cmd_temp,xt_u8 *set_value,xt_u8 set_len);

#ifdef __cplusplus
extern "C" }
#endif // End for __cplusplus

#endif
