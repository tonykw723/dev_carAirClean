/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __PRO_AP_H_
#define __PRO_AP_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#define PRO_AP_MAX_DIMMING 20
#define PRO_AP_MIN_DIMMING 0

typedef  enum	
{
	//common application level protocol for login
	accLogin		=	0x1,

	//common application level protocol for heart beat
	accHeartBeat	=	0x2,

	//common application level protocol for respond cmd
	accRespond 	=	0x3,

	//common application level protocol for open fan
	accOpenFan	=	0x4,

	//common application level protocol for close fan
	accCloseFan	=	0x5,

	//common application level protocol for open denglizifashengqi
	accOpenDLZ	=	0x6,	

	//common application level protocol for close denglizifashengqi
	accCloseDLZ	=	0x7,	

	//common application level protocol for query sensor
	accQuerySensor	=	0x8,	

	//common application level protocol for query status
	accQueryStatus	=	0x9,
	
	//common application level protocol for reset
	accReset	=	0xa,	

	//common application level protocol for restart
	accRestart	=	0xb,	

	//common application level protocol for timming
	accTimming	=	0xc,	

	//common application level protocol for timeGet
	accTimeGet	=	0xd,	

	//common application level protocol for server para set
	accServerParaSet	=	0xe,	

	//common application level protocol for server para get
	accServerParaGet	=	0xf,	

	//common application level protocol for get version
	accVersion	=	0x10,	

	//common application level protocol for set run mode
	accRunModeSet	=	0x11,	

	//common application level protocol for open LED
	accOpenLED	=	0x12,	

	//common application level protocol for close LED
	accCloseLED	=	0x13,	

	//common application level protocol for do all
	accDoAll	=	0x14,	

	//common application level protocol for set time task
	accTimeTaskSet	=	0x15,

	//common application level protocol for get time task
	accTimeTaskGet	=	0x16,
	accNULL		=	0,
}PACKED pro_ap_ctrl_cmd_n;


typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	signalVal;
}PACKED pro_ap_login_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmdId;
	xt_u8      isSuccess;
}PACKED pro_ap_login_down_t;


typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	signalVal;
	xt_u16    TP_val;
	xt_u16    HM_val;
	xt_u16    PM2_5_val;
	xt_u16    VOC_val;
	xt_u8      taskStatus;
	xt_u8      batteryStatus;
	xt_u8      fanStatus;
	xt_u8      DLZ_Status;
	xt_u8      backLedStatus;
	xt_u8      powerLedStatus;
	xt_u8      year;
	xt_u8      month;
	xt_u8      day;
	xt_u8      week;
	xt_u8      hour;
	xt_u8      minute;
	xt_u8      second;
}PACKED pro_ap_heartBeat_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmdId;
	xt_u8      isSuccess;
}PACKED pro_ap_heartBeat_down_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmdId;
	xt_u8      doFlag;

}PACKED pro_ap_doAll_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
	xt_u8	sCmdId;
	xt_u8      doFlag;
}PACKED pro_ap_doAll_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmdId;
	xt_u8      PWM_val;
	xt_u8      runTime;
}PACKED pro_ap_openFan_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
	xt_u8	sCmdId;
}PACKED pro_ap_openFan_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmdId;
}PACKED pro_ap_closeFan_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
	xt_u8	sCmdId;
	
}PACKED pro_ap_closeFan_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmdId;
	xt_u8      reserve;
	xt_u8      runTime;
}PACKED pro_ap_openDLZ_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
	xt_u8	sCmdId;
	
}PACKED pro_ap_openDLZ_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmdId;
}PACKED pro_ap_closeDLZ_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
	xt_u8	sCmdId;
	
}PACKED pro_ap_closeDLZ_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmdId;
	xt_u8      flagLED;
}PACKED pro_ap_openLED_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
	xt_u8	sCmdId;
	xt_u8      flagLED;
	
}PACKED pro_ap_openLED_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmdId;
	xt_u8      flagLED;
}PACKED pro_ap_closeLED_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
	xt_u8	sCmdId;
	xt_u8      flagLED;
	
}PACKED pro_ap_closeLED_up_t;


typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmd;
}PACKED pro_ap_querySensor_down_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmd;
	xt_u16    TP_val;
	xt_u16    HM_val;
	xt_u16    PM2_5_val;
	xt_u16    VOC_val;
	xt_u8      taskStatus;
	xt_u8      batteryStatus;
	xt_u8      fanStatus;
	xt_u8      DLZ_Status;
	xt_u8      backLedStatus;
	xt_u8      powerLedStatus;
}PACKED pro_ap_querySensor_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmd;
}PACKED pro_ap_queryStatus_down_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8	sCmd;
	xt_u8      taskStatus;
	xt_u8      batteryStatus;
	xt_u8      fanStatus;
	xt_u8      DLZ_Status;
}PACKED pro_ap_queryStatus_up_t;


typedef  struct 
{
	xt_u8	mCmdId;
}PACKED pro_ap_reset_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
}PACKED pro_ap_reset_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
}PACKED pro_ap_restart_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
}PACKED pro_ap_restart_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8      len;
	xt_u8      year;
	xt_u8      month;
	xt_u8      day;
	xt_u8      week;
	xt_u8      hour;
	xt_u8      minute;
	xt_u8      second;
}PACKED pro_ap_timming_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
}PACKED pro_ap_timming_up_t;


typedef  struct 
{
	xt_u8	mCmdId;

}PACKED pro_ap_timeGet_down_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8      year;
	xt_u8      month;
	xt_u8      day;
	xt_u8      week;
	xt_u8      hour;
	xt_u8      minute;
	xt_u8      second;
}PACKED pro_ap_timeGet_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8      len;
	xt_u8      linkType;
	xt_u8      severIp[32];
	xt_u16    portNum;
	xt_u8      apn[16];
	xt_u8      telephone[20];

}PACKED pro_ap_serverParaSet_down_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8      isSuccess;
}PACKED pro_ap_serverParaSet_up_t;

typedef  struct 
{
	xt_u8	mCmdId;

}PACKED pro_ap_serverParaGet_down_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u8      linkType;
	xt_u8      severIp[32];
	xt_u16    portNum;
	xt_u8      apn[16];
	xt_u8      telephone[20];
}PACKED pro_ap_serverParaGet_up_t;

typedef  struct 
{
	xt_u8	mCmdId;

}PACKED pro_ap_version_down_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8	len;
	xt_u16     version;
}PACKED pro_ap_version_up_t;


typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8      len;
	xt_u8      runMode;

}PACKED pro_ap_runModeSet_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
}PACKED pro_ap_runModeSet_up_t;

typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8      len;
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

	

}PACKED pro_ap_timeTaskSet_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
}PACKED pro_ap_timeTaskSet_up_t;


typedef  struct 
{
	xt_u8	mCmdId;
	xt_u8      len;
	xt_u8      taskId;	

}PACKED pro_ap_timeTaskGet_down_t;

typedef  struct 
{
	xt_u8	respId;
	xt_u8	len;
	xt_u8	mCmdId;
	xt_u8      isSuccess;
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
}PACKED pro_ap_timeTaskGet_up_t;



#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __PRO_AP_H_

