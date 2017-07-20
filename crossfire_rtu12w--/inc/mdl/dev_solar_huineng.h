
#define DEV_HNJD 6

typedef struct 
{
	xt_u8 id;					//设备ID号
	xt_u8 cmd;				//MODBUS命令
	//xt_u8 addr_h;			//寄存器高位
	//xt_u8 addr_l;			//寄存器低位
	xt_u16 addr_register;  //寄存器地址
  xt_u16 n_register;    //读取写入多少个寄存器，每个寄存器16位
	//xt_u16 len;				//读取数据的长度
	xt_u8 data[32];
	xt_u16 crc;				//低位在前
}PACKED solar_hn_send_pack;                  

typedef struct
{
	xt_u8 id;
	xt_u8 loadStatus;
}PACKED hn_data_t;


typedef  struct 
{
	 /*xt_u8	 time_minute;
	xt_u8	  time_secend;
	xt_u8	  time_day;
	xt_u8	  time_hour;
	xt_u8	  time_year;
	xt_u8	  time_month;*/
	xt_u8	  devType;			//设备类型
	xt_u8		chargeDevStatus_h;	 //充电设备状态高位D15~D8
	xt_u8		chargeDevStatus_l;	  //充电设备状态高位D7~D0
	xt_u8		chargeDevVoltage_h;
	xt_u8		chargeDevVoltage_l;
	xt_u8		chargeDevCurrent_h;
	xt_u8		chargeDevCurrent_l;
	xt_u8		chargeDevPwH_h;		//充电设备功率，4byte
	xt_u8		chargeDevPwH_l;
	xt_u8		chargeDevPwL_h;
	xt_u8		chargeDevPwL_l;
	
	xt_u8	  batteryStatus_h;
	xt_u8	  batteryStatus_l;
	xt_u8		batteryVoltage_h;
	xt_u8		batteryVoltage_l;
	xt_u8		batteryCurrent_h;
	xt_u8		batteryCurrent_l;
	xt_u8		batteryPwH_h;		//蓄电池功率，4byte
	xt_u8		batteryPwH_l;
	xt_u8		batteryPwL_h;
	xt_u8		batteryDevPwL_l;

	//xt_u8	  loadStatus_h;
	//xt_u8	  loadStatus_l;
	xt_u8		loadVoltage_h;
	xt_u8		loadVoltage_l;
	xt_u8		loadCurrent_h;
	xt_u8		loadCurrent_l;
	xt_u8		loadPwH_h;		//负载功率，4byte
	xt_u8		loadPwH_l;
	xt_u8		loadPwL_h;
	xt_u8		loadDevPwL_l;

	xt_u8	  dailyDischargeEnergy_h_a;	  //日用电量
	xt_u8	  dailyDischargeEnergy_h_b;
	xt_u8	  dailyDischargeEnergy_l_a;
	xt_u8	  dailyDischargeEnergy_l_b;
	xt_u8	  monthTotalDischargeEnergy_h_a;//月用电量
	xt_u8	  monthTotalDischargeEnergy_h_b;
	xt_u8	  monthTotalDischargeEnergy_l_a;
	xt_u8	  monthTotalDischargeEnergy_l_b;
	xt_u8	  yearTotalDischargeEnergy_h_a;//年用电量
	xt_u8	  yearTotalDischargeEnergy_h_b;
	xt_u8	  yearTotalDischargeEnergy_l_a;
	xt_u8	  yearTotalDischargeEnergy_l_b;

}PACKED sloarHn_t;

typedef  struct 
{

	xt_u8      devType;
	xt_u8      batteryVoltage_h;// 0.01V
	xt_u8      batteryVoltage_l;// 0.01V
	xt_u8      batteryStatus;//00-过放01-欠压02-正常03-充电限制04-超压
	//xt_u8      loadVoltage;
	xt_u8      loadCurrent_h;// 0.01A
	xt_u8      loadCurrent_l;// 0.01A
	xt_u8      loadVoltage_h;// 0.01V
	xt_u8      loadVoltage_l;// 0.01V
	xt_u8      loadStatus;//00-关01-开02超压保护03-短路保护
	xt_u8      pvVoltage_h;// 0.01V
	xt_u8      pvVoltage_l;// 0.01V
	xt_u8      pvStatus;// 00-未达到光控条件01-达到光控条件02-光电池未达到充电电压
	xt_u8      dailyDischargeEnergy_h;//WH
	xt_u8      dailyDischargeEnergy_l;//WH
	xt_u8      totaldischargeEnergy_h;//KWH
	xt_u8      totaldischargeEnergy_l;//KWH	

}PACKED sloarHN_to_YF_t;

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
}PACKED solar_getTask_Hn_to_yf_t;

typedef enum
{
	READ_DIG=0x01,                         //读开关量
	DIGTTAL_I_O,                    //离散量输入，只读
	READ_HOLDING_REG=0x03,						//读保持寄存器
	INPUT_REG=0x04,                   //读输入寄存器
	WRITE_DIG,
	WRITE_O_HOLDING_REG=0x06,				//write one data to holding register
	WRITE_N_HOLDING_REG=0X10,	
	GET_SET_ID=0x45,
}modbus_protocol;

xt_u16  get_crc16(xt_u8 *buf,xt_u32 length);
xt_void  DevSolarHnInit(void);
xt_s32	DevSolarHnSend(xt_u8* pData, xt_s32 dataLen);
xt_s32	DevSolarHnRecv(xt_u8* pData, xt_u32 maxLen);
xt_void  DevSolarHnCtrl(xt_u8* src_pData,xt_u8* dec_pData);
xt_s32 DevSolarHnWriteAndRead(xt_u8 id_temp,xt_u8 cmd_temp, xt_u8 *sendBuf,xt_u8 sendLen);
xt_s32 packge_and_send( xt_u8 *p,xt_u8 Lengh);
xt_void set_led_current(void);
xt_void refresh_HN_task_data(void);

extern hn_data_t hn_para;
extern xt_u8 hnT;
extern xt_u8 hn_cnt;
extern sloarHn_t  solar_hn_data;
extern sloarHN_to_YF_t solar_hn_to_yf_data;
//extern xt_u8 hn_lamp_max_current;
