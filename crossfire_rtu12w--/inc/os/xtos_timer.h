/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/

#ifndef __XTOS_TIMER_H_
#define __XTOS_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

typedef struct{
	xt_s32 tm_sec;	/* ��Cȡֵ����Ϊ[0,59] */
	xt_s32 tm_min;	/* �� - ȡֵ����Ϊ[0,59] */
	xt_s32 tm_hour;	/* ʱ - ȡֵ����Ϊ[0,23] */
	xt_s32 tm_mday;	/* һ�����е����� - ȡֵ����Ϊ[1,31] */
	xt_s32 tm_mon;	/* �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11] */
	xt_s32 tm_year;	/* ��ݣ���ֵ��1970��ʼ */
	xt_s32 tm_wday;	/* ���ڨCȡֵ����Ϊ[0,6]������0���������죬1��������һ���Դ����� */
	xt_s32 tm_yday;	/* ��ÿ���1��1�տ�ʼ�������Cȡֵ����Ϊ[0,365]������0����1��1�գ�1����1��2�գ��Դ����� */
	xt_s32 tm_isdst;	/* ����ʱ��ʶ����ʵ������ʱ��ʱ��tm_isdstΪ������ʵ������ʱ�Ľ���tm_isdstΪ0�����˽����ʱ��tm_isdst()Ϊ����*/ 
}xt_tm_t;

//reference to  "timers.h" in FreeRTOS

#define TimerCreateXTOS( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction )  xTimerCreate( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction )

#define TimerStartXTOS(xTimer)		xTimerStart( xTimer, 0 )

#define TimerStopXTOS(xTimer)		xTimerStop( xTimer, 0 )

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XTOS_TIMER_H_

