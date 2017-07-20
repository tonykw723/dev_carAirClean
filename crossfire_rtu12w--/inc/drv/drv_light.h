/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __DRV_LIGHT_H_
#define __DRV_LIGHT_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

//todo: add mutex for each drv (or in bsp module)

#define drvLightInit					NULL
#define drvLightDimming					NULL
#define drvLightOpen(dochn)				XtBspDOCtrl(dochn, XT_BSP_DO_OP_OPEN)
#define drvLightClose(dochn)			XtBspDOCtrl(dochn, XT_BSP_DO_OP_CLOSE)
#define drvLightCtrl					NULL

#define drvLightDim(pwmchn,dim)			XtBspPWMCtrl(pwmchn, dim)

#define drvDLZOpen2()				XtBspDOCtrl(GPIO_DO_DLZ, XT_BSP_DO_OP_OPEN);PrintfXTOS("^^DLZ opened\n");glbRtuPara.runningInfo.flag_DLZ_Opened=YES
#define drvDLZOpen()				XtBspDOCtrl(GPIO_DO_DLZ, XT_BSP_DO_OP_OPEN);PrintfXTOS("^^DLZ opened\n");glbRtuPara.runningInfo.DLZ_timeOut=20*60;glbRtuPara.runningInfo.flag_DLZ_Opened=YES
#define drvDLZClose()			XtBspDOCtrl(GPIO_DO_DLZ, XT_BSP_DO_OP_CLOSE);PrintfXTOS("^^DLZ closed\n");glbRtuPara.runningInfo.flag_DLZ_Opened=NO
#define drvFanDim(dim)			XtBspPWMCtrl(XT_BSP_PWM_0, dim);PrintfXTOS("^^dim pwm =%d\n",dim)
#define drvTurnOff()    			drvFanDim(0);drvDLZClose();	glbRtuPara.runningInfo.flag_DLED_en=NO;glbRtuPara.runningInfo.flag_FLED_en=NO;glbRtuPara.runningInfo.flag_powerEn=NO;\
								glbRtuPara.runningInfo.flag_fanOpened=NO;
#define drvTurnOn()    			drvDLZOpen();glbRtuPara.runningInfo.flag_DLED_en=NO;glbRtuPara.runningInfo.flag_FLED_en=YES;glbRtuPara.runningInfo.flag_powerEn=YES;\
								glbRtuPara.runningInfo.flag_fanOpened=YES;


#define FAN_SPEED1() drvFanDim( 20)
#define FAN_SPEED2() drvFanDim( 30)
#define FAN_SPEED3() drvFanDim( 40)
#define FAN_SPEED4() drvFanDim( 60)

#define drv3GOpen()	     XtBspDOCtrl(GPIO_DO_3GPOWEREN, XT_BSP_POWER_OPEN);PrintfXTOS("^^3G power opened\n")
#define drv3GClose()	     XtBspDOCtrl(GPIO_DO_3GPOWEREN, XT_BSP_POWER_CLOSE);PrintfXTOS("^^3G power closed\n")

#define drvPowerOpen()	     XtBspDOCtrl(GPIO_DO_OTHER_POWEREN, XT_BSP_POWER_OPEN);PrintfXTOS("^^3G Sensor power opened\n")
#define drvPowerClose()	     XtBspDOCtrl(GPIO_DO_OTHER_POWEREN, XT_BSP_POWER_CLOSE);PrintfXTOS("^^3G Sensor power closed\n")

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __DRV_LIGHT_H_

