/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __XT_BSP_RTU09C_H_
#define __XT_BSP_RTU09C_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

//#define MCU_RUN		0
//#define MCU_SLEEP       1
//#define MCU_LP_RUN      2
//#define MCU_LP_SLEEP    3
#define MCU_STOP_RTC	4
#define MCU_STOP_NoRTC	5
#define MCU_STBY	6

#define MCU_POWERDOWN_CNT	15
#define MCU_SLEEP_TIME		(30)//s

typedef struct
{
	xt_s32 ledSt;		//light status:open(1) or close(0)
	xt_s32 ledConnSt;	//light connect status:connected(1) or disconnected(0)
} xt_bsp_param_win32_t;

typedef enum 
{
  XT_BSP_DO_DLZ = 0,
  XT_BSP_DO_3GPOWEREN = 1,
  XT_BSP_3GRESET=2,
  XT_BSP_3GONOFF=3,
  XT_BSP_OTHER_POWEREN=4,
  XT_BSP_SCL= 5,
  XT_BSP_SDA = 6,  
  XT_BSP_DO_MAX,
} xt_bsp_do_id_n;

typedef enum 
{
  XT_BSP_DO_OP_OPEN = 0,
  XT_BSP_DO_OP_CLOSE,
  XT_BSP_DO_OP_MAX,
} xt_bsp_do_op_n;

#define XT_BSP_POWER_CLOSE  XT_BSP_DO_OP_OPEN
#define XT_BSP_POWER_OPEN  XT_BSP_DO_OP_CLOSE

typedef enum 
{
  XT_BSP_PWM_0 = 0,
  //XT_BSP_PWM_1,
  XT_BSP_PWM_MAX,
} xt_bsp_pwm_id_n;

typedef enum 
{
  XT_BSP_PWM_DIM_MIN = 0,
  XT_BSP_PWM_DIM_MAX = 100,
} xt_bsp_pwm_dim_n;

//-------LAMP 数目最大值----------
#define  LAMP_NUM_MAX   2

#define  LAMP1_INDEX  0
#define  LAMP2_INDEX  1
#define  LAMP3_INDEX  2
#define  LAMP4_INDEX  3

//----------电压、电流引脚-------------
#define  LAMP1_I_PIN         2
#define  LAMPS_V_PIN         1
#define  LAMP2_I_PIN         0	

//-------AD CHNANNLE 数目----------
#define  CHANNLE		1			   //选用5个通道ad,单灯2个通道，双灯3个通

//-------参考电压5V or 4.096-------
#define  REFERENCE_3V3

//------电能测量AC or DC----------
#define  MEASURE_AC
//#define  MEASURE_DC
#define  SAMPLE			32
#define  SAMPLE_NUM       35

//-------------------------ADC&POWER--------------------------------
typedef struct
{
	xt_u8   sample;	
	xt_u16  buf[CHANNLE][SAMPLE_NUM];			//16bit,8路AD，各采45个
}AD_type;

typedef struct
{
	short TP_val;//温度
	unsigned short HM_val;//湿度
	float PM2_5_val;
	char VOC_val;
	char VOC_val_old;
	
	char flagPM2_5_level;
	char flagPM2_5_level_old;

	//------------------------------------------
}sensor_type;

extern xt_bsp_param_win32_t win32ParamSt;

//init BSP
int XtBspInit(void);
xt_void XtBspCtrlUartRxIsr(xt_u8 data);
//write control com in BSP
xt_s32 XtBspCtrlWrite(xt_u8* pData, xt_s32 dataLen);

//read control com in BSP
xt_s32 XtBspCtrlRead(xt_u8* pData, xt_s32* pDataLen);

xt_s32 Win32CtrlRespInit(void);

xt_s32 Win32CtrlRespDeinit(void);

//write control response com in win32
xt_s32 XtBspCtrlRespWrite(xt_u8* pData,xt_s32 dataLen);

//read control response com in win32
xt_s32 XtBspCtrlRespRead(xt_u8* pData,xt_s32* pDataLen);

xt_s32 XtBspCommInit(xt_u32 buadrate,xt_u16 parity);

xt_s32 XtBspToggleRunLed(void);

xt_s32 XtBspOpenRunLed(void);

xt_s32 XtBspCloseRunLed(void);

xt_s32 XtBspDOCtrl(xt_u8 doChn, xt_u8 doOp);

xt_s32 XtBspPWMCtrl(xt_u8 pwmChn, xt_u8 dim);

xt_s32 XtBspAdcInit(void);

xt_s32 XtBspAdcTimerStart(void);

xt_s32 XtBspAdcTimerStop(void);
	
xt_u32 XtBspGetTime(void);

xt_s32 XtBspWdgInit(void);

xt_s32 XtBspWdgFeed(void);

xt_s32 XtBspCheckPower(void);

xt_s32 XtBspPowerManager(xt_u8 Mcu_State);

xt_void XtBspOdbUartRxIsr(xt_u8 data);
xt_s32 XtBspOdbRead(xt_u8* pData,xt_s32* pDataLen);


#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XT_BSP_RTU09C_H_
