/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __PRO_AP_IOT_H_
#define __PRO_AP_IOT_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

typedef  enum	
{
	//specific application level protocol for login
	ccLogin	=	0xf0,

	//specific application level protocol for heart beat
	ccHeartBeat	=	0xf1,

	//specific application level protocol for Respond cmd
	ccRespond	=	0x80,	

	//specific application level protocol for do ctrl cmd
	ccDoCtrl	=	0xf5,	

	//specific application level protocol for query cmd
	ccQuery	=	0x01,	

	//specific application level protocol for reset cmd
	ccReset	=	0xf2,	

	//specific application level protocol for restart cmd
	ccRestart	=	0xa2,	

	//specific application level protocol for timming
	ccTimming	=	0x8c,	

	//specific application level protocol for timeGet
	ccTimeGet	=	0x8d,

	//specific application level protocol for server para set
	ccServerParaSet	=	0x8e,	

	//specific application level protocol for server para get
	ccServerParaGet	=	0x8f,

	//specific application level protocol for run mode set
	ccRunModeSet	=	0x90,	

	//specific application level protocol for get version
	ccVersion	=	0xa1,

	//specific application level protocol for set time task
	ccTimeTaskSet	=	0x82,

	//specific application level protocol for get time task
	ccTimeTaskGet	=	0x85,
	ccNULL	=	0,
}PACKED pro_ap_iot_ctrl_cmd_n;

typedef  enum	
{
	//specific application level protocol for do all
	ccDoAll	=	0x0,

	//specific application level protocol for open fan
	ccOpenFan	=	0x1,

	//specific application level protocol for close fan
	ccCloseFan	=	0x2,

	//specific application level protocol for open denglizifashengqi
	ccOpenDLZ	=	0x3,	

	//specific application level protocol for close denglizifashengqi
	ccCloseDLZ	=	0x4,	

	//specific application level protocol for open LED
	ccOpenLED	=	0x5,	

	//specific application level protocol for close LED
	ccCloseLED	=	0x6,	

    ccSubDoCtrlNULL	=	0,
}PACKED pro_ap_iot_subDoCtrl_cmd_n;

typedef  enum	
{
	//specific application level protocol for query sensor
	ccSensorInfo	=	0x28,

	//specific application level protocol for query status
	ccStatusInfo 	=	0x29,


    ccSubQueryNULL	=	0,
}PACKED pro_ap_iot_subQuery_cmd_n;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	signalVal;
}PACKED pro_ap_iot_login_up_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmd;
	xt_u8      isSuccess;
}PACKED pro_ap_iot_login_down_t;

typedef  struct 
{
	xt_u8	mCmd;
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
}PACKED pro_ap_iot_heartBeat_up_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmd;
	xt_u8      isSuccess;
}PACKED pro_ap_iot_heartBeat_down_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmd;
	xt_u8      doFlag;

}PACKED pro_ap_iot_doAll_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8      isSuccess;
	xt_u8	sCmd;
	xt_u8      doFlag;
}PACKED pro_ap_iot_doAll_up_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmd;
	xt_u8      PWM_val;
	xt_u8      runTime;
}PACKED pro_ap_iot_openFan_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8      isSuccess;
	xt_u8	sCmd;
	
}PACKED pro_ap_iot_openFan_up_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmd;
}PACKED pro_ap_iot_closeFan_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8      isSuccess;
	xt_u8	sCmd;
	
}PACKED pro_ap_iot_closeFan_up_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmd;
	xt_u8      reserve;
	xt_u8      runTime;
}PACKED pro_ap_iot_openDLZ_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8      isSuccess;
	xt_u8	sCmd;
	
}PACKED pro_ap_iot_openDLZ_up_t;


typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmd;
}PACKED pro_ap_iot_closeDLZ_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8      isSuccess;
	xt_u8	sCmd;
	
}PACKED pro_ap_iot_closeDLZ_up_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmdId;
	xt_u8      flagLED;
}PACKED pro_ap_iot_openLED_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8      isSuccess;
	xt_u8	sCmd;
	xt_u8      flagLED;
	
}PACKED pro_ap_iot_openLED_up_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmdId;
	xt_u8      flagLED;
}PACKED pro_ap_iot_closeLED_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8      isSuccess;
	xt_u8	sCmd;
	xt_u8      flagLED;
	
}PACKED pro_ap_iot_closeLED_up_t;


typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmd;
}PACKED pro_ap_iot_querySensor_down_t;

typedef  struct 
{
	xt_u8	mCmd;
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
}PACKED pro_ap_iot_querySensor_up_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmd;
}PACKED pro_ap_iot_queryStatus_down_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8	sCmd;
	xt_u8      taskStatus;
	xt_u8      batteryStatus;
	xt_u8      fanStatus;
	xt_u8      DLZ_Status;
}PACKED pro_ap_iot_queryStatus_up_t;


typedef  struct 
{
	xt_u8	mCmd;
}PACKED pro_ap_iot_reset_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8     isSuccess;
}PACKED pro_ap_iot_reset_up_t;


typedef  struct 
{
	xt_u8	mCmd;
}PACKED pro_ap_iot_restart_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8     isSuccess;
}PACKED pro_ap_iot_restart_up_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8      len;
	xt_u8      year;
	xt_u8      month;
	xt_u8      day;
	xt_u8      week;
	xt_u8      hour;
	xt_u8      minute;
	xt_u8      second;
}PACKED pro_ap_iot_timming_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8      isSuccess;
}PACKED pro_ap_iot_timming_up_t;


typedef  struct 
{
	xt_u8	mCmd;

}PACKED pro_ap_iot_timeGet_down_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8      year;
	xt_u8      month;
	xt_u8      day;
	xt_u8      week;
	xt_u8      hour;
	xt_u8      minute;
	xt_u8      second;
}PACKED pro_ap_iot_timeGet_up_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8      len;
	xt_u8      linkType;
	xt_u8      severIp[32];
	xt_u16    portNum;
	xt_u8      apn[16];
	xt_u8      telephone[20];

}PACKED pro_ap_iot_serverParaSet_down_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8      isSuccess;
}PACKED pro_ap_iot_serverParaSet_up_t;


typedef  struct 
{
	xt_u8	mCmd;

}PACKED pro_ap_iot_serverParaGet_down_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u8      linkType;
	xt_u8      severIp[32];
	xt_u16    portNum;
	xt_u8      apn[16];
	xt_u8      telephone[20];
}PACKED pro_ap_iot_serverParaGet_up_t;

typedef  struct 
{
	xt_u8	mCmd;

}PACKED pro_ap_iot_version_down_t;

typedef  struct 
{
	xt_u8	mCmd;
	xt_u8	len;
	xt_u16     version;
}PACKED pro_ap_iot_version_up_t;


typedef  struct 
{
	xt_u8	mCmd;
	xt_u8      len;
	xt_u8      runMode;

}PACKED pro_ap_iot_runModeSet_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8      isSuccess;
}PACKED pro_ap_iot_runModeSet_up_t;

typedef  struct 
{
	xt_u8	mCmd;
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

	

}PACKED pro_ap_iot_timeTaskSet_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
	xt_u8      isSuccess;
}PACKED pro_ap_iot_timeTaskSet_up_t;


typedef  struct 
{
	xt_u8	mCmd;
	xt_u8      len;
	xt_u8      taskId;

}PACKED pro_ap_iot_timeTaskGet_down_t;

typedef  struct 
{
	xt_u8	resp;
	xt_u8	len;
	xt_u8	mCmd;
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
}PACKED pro_ap_iot_timeTaskGet_up_t;



/**
* @brief:					pack common application level protocol to specific application level protocol
*
* @param pSrcAp:			pointer to common application level protocol
*
* @param apSrcLen:			common application level protocol length
*
* @param pDstApSpec:		pointer to specific application level protocol @return
*
* @param pDstApSpecLen:		specific application level protocol length @return
*
* @return:					SUCCESS_XT: pack application level protocol successful
*							FAILURE_XT: pack application level protocol failure 
**/
xt_s32 ProApIotPack(xt_u8* pSrcAp, xt_s32 apSrcLen, xt_u8* pDstApSpec, xt_u8* pDstApSpecLen);

/**
* @brief:					parse specific application level protocol to common application level protocol
*
* @param pSrcApSpec:		pointer to specific application level protocol
*
* @param srcApSpecLen:		specific application level protocol length
*
* @param pDstAp:			pointer to common application level protocol @return
*
* @param pDstApLen:			common application level protocol length @return
*
* @return:					SUCCESS_XT: parse application level protocol successful
*							FAILURE_XT: parse application level protocol failure 
**/
xt_s32 ProApIotParse(xt_u8* pSrcApSpec, xt_s32 srcApSpecLen, xt_u8* pDstAp, xt_u8* pDstApLen);

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __PRO_AP_IOT_H_

