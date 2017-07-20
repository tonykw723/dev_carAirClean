#ifndef _DEV_SOLAR_YUANFANG_H
#define _DEV_SOLAR_YUANFANG_H

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus
//--------------------yuanfang protocol-----------------------------------------

#define DEV_YFDL 5
typedef enum
{
	GET_TASK=1,
	SET_TASK=2,
	TOGGLE_SW=3,
	GET_SOLAR=4,
	CTRL_UNITY=5,
}cmdYf_enum;



typedef enum
{
	SDL=0,SDP=1,SDSZ=2,SDLB=3,SDH=4,
}production_enum;

typedef enum
{
	_60W=0,_100W,_150W,_30W,
}power_enum;

typedef enum
{
	SC_CLOSE=0,SC_MODE1,SC_MODE2,
}smartCtrl_enum;

typedef enum
{
	AD_CLOSE=0,AD_OPEN
}advance_enum;

typedef enum
{
	MD_MANUAL=0,MD_AUTO,MD_DEBUG,MD_PHOTO
}mode_enum;

typedef enum
{
	BT_COLLOID=0,BT_SEALED,BT_CUSTOM,
}battery_enum;

typedef /*__packed */ struct
{
	xt_u8 	productionType;
	xt_u8 	maxPower;
	xt_u8      runTime;
	xt_u8      overDischargeTimes;
	xt_u8 	timePeriod_1th;
	xt_u8 	current_1th;
	xt_u8 	timePeriod_2th;
	xt_u8 	current_2th;
	xt_u8 	timePeriod_3th;
	xt_u8 	current_3th;
	xt_u8 	smartCtrl;
	xt_u8 	advance;
	xt_u8 	ctrlMode;
	xt_u8 	photoCtrlDelay;
	xt_u8      photoVoltage;	
	xt_u8 	batteryType;
	xt_u8 	voltageOfOverCharge;
	xt_u8 	voltageOfOverDischarge;
	xt_u8 	volatgeOfOverDischargeReturn;
	xt_u8 	voltageOfTisheng;
	xt_u8 	voltageOfFuChong;
	xt_u8 	priorityOfPowerSupply;
	xt_u8 	timePeriod_4th;
	xt_u8 	current_4th;
	xt_u8 	timePeriod_5th;
	xt_u8 	current_5th;
	xt_u8 	timePeriod_6th;
	xt_u8 	current_6th;	
}PACKED solar_getTask_yf_t;

typedef /*__packed */ struct
{
	xt_u8 	productionType;
	xt_u8 	maxPower;
	xt_u8 	timePeriod_1th;
	xt_u8 	current_1th;
	xt_u8 	timePeriod_2th;
	xt_u8 	current_2th;
	xt_u8 	timePeriod_3th;
	xt_u8 	current_3th;
	xt_u8 	smartCtrl;
	xt_u8 	advance;
	xt_u8 	ctrlMode;
	xt_u8 	photoCtrlDelay;
	xt_u8      photoVoltage;	
	xt_u8 	batteryType;
	xt_u8 	voltageOfOverCharge;
	xt_u8 	voltageOfOverDischarge;
	xt_u8 	volatgeOfOverDischargeReturn;
	xt_u8 	voltageOfTisheng;
	xt_u8 	voltageOfFuChong;
	xt_u8 	reserve1;
	xt_u8 	timePeriod_4th;
	xt_u8 	current_4th;
	xt_u8 	timePeriod_5th;
	xt_u8 	current_5th;
	xt_u8 	timePeriod_6th;
	xt_u8 	reserve2;
	xt_u8 	reserve3;
	xt_u8 	current_6th;
}PACKED solar_setTask_yf_t;

typedef  struct 
{
	xt_u32    timeStamp;   //按飞利浦方式
	xt_u8      devType;
	xt_u8      batteryVoltage; //0.1V
	xt_u8      batteryStatus; //00-过放01-欠压02-正常03-充电限制04-超压
	xt_u8      loadVoltage; // 1v
	xt_u8      loadCurrent;// 0.1A
	xt_u8      loadStatus; //00-关01-开02超压保护03-短路保护
	xt_u8      pvVoltage;// 1V
	xt_u8      pvStatus;// 00-未达到光控条件01-达到光控条件02-光电池未达到充电电压
	xt_u8      dailyDischargeEnergy_h;//WH
	xt_u8      dailyDischargeEnergy_l;//WH
	xt_u8      totaldischargeEnergy_h;//KWH
	xt_u8      totaldischargeEnergy_l;//KWH	

}PACKED sloarYf_t;
//---------------------------------------------------------------------------------------------

xt_void  DevSolarYfInit(void);
xt_s32	DevSolarYfSend(xt_u8* pData, xt_s32 dataLen);
xt_s32	DevSolarYfRecv(xt_u8* pData, xt_u32 maxLen);
xt_void  DevSolarYfCtrl(xt_u8* src_pData,xt_u8* dec_pData);
xt_s32 DevSolarYfWriteAndRead(xt_u8 id_temp,xt_u8 cmd_temp, xt_u8 *sendBuf,xt_u8 sendLen);
#ifdef __cplusplus
extern "C" }
#endif // End for __cplusplus

#endif
