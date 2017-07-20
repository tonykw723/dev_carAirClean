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
	xt_s32 tm_sec;	/* 秒–取值区间为[0,59] */
	xt_s32 tm_min;	/* 分 - 取值区间为[0,59] */
	xt_s32 tm_hour;	/* 时 - 取值区间为[0,23] */
	xt_s32 tm_mday;	/* 一个月中的日期 - 取值区间为[1,31] */
	xt_s32 tm_mon;	/* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
	xt_s32 tm_year;	/* 年份，其值从1970开始 */
	xt_s32 tm_wday;	/* 星期–取值区间为[0,6]，其中0代表星期天，1代表星期一，以此类推 */
	xt_s32 tm_yday;	/* 从每年的1月1日开始的天数–取值区间为[0,365]，其中0代表1月1日，1代表1月2日，以此类推 */
	xt_s32 tm_isdst;	/* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。*/ 
}xt_tm_t;

//reference to  "timers.h" in FreeRTOS

#define TimerCreateXTOS( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction )  xTimerCreate( pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction )

#define TimerStartXTOS(xTimer)		xTimerStart( xTimer, 0 )

#define TimerStopXTOS(xTimer)		xTimerStop( xTimer, 0 )

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XTOS_TIMER_H_

