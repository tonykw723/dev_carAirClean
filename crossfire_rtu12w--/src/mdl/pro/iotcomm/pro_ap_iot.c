/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>

#include "xtsys_type.h"
#include "xtsys_cfg.h"

#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"

#include "xt_bsp_rtu09c.h"

#include "drv_uart.h"

#include "xt_conn.h"
#include "conn_uart.h"

#include "xt_dev.h"
#include "dev_rise.h"

#include "pro_dev_iot.h"

#include "global_para.h"
#include "pro_ap.h"
#include "xt_pro_ap.h"
#include "pro_ap_iot.h"
xt_s32 ProApIotPack(xt_u8* pSrcAp, xt_s32 srcApLen, xt_u8* pDstApSpec, xt_u8* pDstApSpecLen)
{
	xt_u8 ret = FAILURE_XT;
	xt_u8 mCmdId = accNULL;
	mCmdId = *pSrcAp;
	switch(mCmdId)
	{
		case accLogin:
		{
			pro_ap_iot_login_up_t *pApIot = NULL;
			pro_ap_login_up_t *pAp = NULL;
			pAp = (pro_ap_login_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_login_up_t *)pDstApSpec;
			
			pApIot->mCmd = ccLogin;		
			pAp->len=sizeof(pro_ap_login_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->signalVal = pAp->signalVal;
			*pDstApSpecLen = sizeof(pro_ap_login_up_t);
			ret = SUCCESS_XT;
			break;
		}
		case accHeartBeat:
		{
			pro_ap_iot_heartBeat_up_t *pApIot = NULL;
			pro_ap_heartBeat_up_t *pAp = NULL;
			pAp = (pro_ap_heartBeat_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_heartBeat_up_t *)pDstApSpec;
			
			pApIot->mCmd = ccHeartBeat;		
			pAp->len=sizeof(pro_ap_heartBeat_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->signalVal = pAp->signalVal;
			pApIot->TP_val=pAp->TP_val;
			pApIot->HM_val=pAp->HM_val;
			pApIot->PM2_5_val=pAp->PM2_5_val;
			pApIot->VOC_val=pAp->VOC_val;
			pApIot->taskStatus=pAp->taskStatus;
			pApIot->batteryStatus=pAp->batteryStatus;
			pApIot->fanStatus=pAp->fanStatus;
			pApIot->DLZ_Status=pAp->DLZ_Status;
			pApIot->backLedStatus=pAp->backLedStatus;
			pApIot->powerLedStatus=pAp->powerLedStatus;
			pApIot->year= pAp->year;
			pApIot->month= pAp->month;
			pApIot->day= pAp->day;
			pApIot->week= pAp->week;
			pApIot->hour= pAp->hour;
			pApIot->minute= pAp->minute;
			pApIot->second= pAp->second;
			*pDstApSpecLen = sizeof(pro_ap_heartBeat_up_t);
			ret = SUCCESS_XT;
			break;
		}
		case accDoAll:
		{
			pro_ap_iot_doAll_up_t *pApIot = NULL;
			pro_ap_doAll_up_t *pAp = NULL;
			pAp = (pro_ap_doAll_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_doAll_up_t *)pDstApSpec;
			
			pApIot->resp= ccRespond;		
			pAp->len=sizeof(pro_ap_doAll_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccDoCtrl;
			pApIot->sCmd= ccDoAll;
			pApIot->isSuccess = pAp->isSuccess;
			pApIot->doFlag = pAp->doFlag;
			*pDstApSpecLen = sizeof(pro_ap_doAll_up_t);
			ret = SUCCESS_XT;
			break;
		}
		case accOpenFan:
		{
			pro_ap_iot_openFan_up_t *pApIot = NULL;
			pro_ap_openFan_up_t *pAp = NULL;
			pAp = (pro_ap_openFan_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_openFan_up_t *)pDstApSpec;
			
			pApIot->resp= ccRespond;		
			pAp->len=sizeof(pro_ap_openFan_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccDoCtrl;
			pApIot->sCmd= ccOpenFan;
			pApIot->isSuccess = pAp->isSuccess;
			*pDstApSpecLen = sizeof(pro_ap_openFan_up_t);
			ret = SUCCESS_XT;
			break;
		}
		case accCloseFan:
		{
			pro_ap_iot_closeFan_up_t *pApIot = NULL;
			pro_ap_closeFan_up_t *pAp = NULL;
			pAp = (pro_ap_closeFan_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_closeFan_up_t *)pDstApSpec;
			
			pApIot->resp= ccRespond;		
			pAp->len=sizeof(pro_ap_closeFan_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccDoCtrl;
			pApIot->sCmd= ccCloseFan;
			pApIot->isSuccess = pAp->isSuccess;
			*pDstApSpecLen = sizeof(pro_ap_closeFan_up_t);
			ret = SUCCESS_XT;
			break;
		}	
		case accOpenDLZ:
		{
			pro_ap_iot_openDLZ_up_t *pApIot = NULL;
			pro_ap_openDLZ_up_t *pAp = NULL;
			pAp = (pro_ap_openDLZ_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_openDLZ_up_t *)pDstApSpec;
			
			pApIot->resp= ccRespond;		
			pAp->len=sizeof(pro_ap_openDLZ_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccDoCtrl;
			pApIot->sCmd= ccOpenDLZ;
			pApIot->isSuccess = pAp->isSuccess;
			*pDstApSpecLen = sizeof(pro_ap_openDLZ_up_t);
			ret = SUCCESS_XT;
			break;
		}
		case accCloseDLZ:
		{
			pro_ap_iot_closeDLZ_up_t *pApIot = NULL;
			pro_ap_closeDLZ_up_t *pAp = NULL;
			pAp = (pro_ap_closeDLZ_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_closeDLZ_up_t *)pDstApSpec;
			
			pApIot->resp= ccRespond;		
			pAp->len=sizeof(pro_ap_closeDLZ_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccDoCtrl;
			pApIot->sCmd= ccCloseDLZ;
			pApIot->isSuccess = pAp->isSuccess;
			*pDstApSpecLen = sizeof(pro_ap_closeDLZ_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accOpenLED:
		{
			pro_ap_iot_openLED_up_t *pApIot = NULL;
			pro_ap_openLED_up_t *pAp = NULL;
			pAp = (pro_ap_openLED_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_openLED_up_t *)pDstApSpec;
			
			pApIot->resp= ccRespond;		
			pAp->len=sizeof(pro_ap_openLED_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccDoCtrl;
			pApIot->sCmd= ccOpenLED;
			pApIot->isSuccess = pAp->isSuccess;
			pApIot->flagLED = pAp->flagLED;
			*pDstApSpecLen = sizeof(pro_ap_openLED_up_t);
			ret = SUCCESS_XT;
			break;
		}
		case accCloseLED:
		{
			pro_ap_iot_closeLED_up_t *pApIot = NULL;
			pro_ap_closeLED_up_t *pAp = NULL;
			pAp = (pro_ap_closeLED_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_closeLED_up_t *)pDstApSpec;
			
			pApIot->resp= ccRespond;		
			pAp->len=sizeof(pro_ap_closeLED_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccDoCtrl;
			pApIot->sCmd= ccCloseLED;
			pApIot->isSuccess = pAp->isSuccess;
			pApIot->flagLED = pAp->flagLED;
			*pDstApSpecLen = sizeof(pro_ap_closeLED_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accQuerySensor:
		{
			pro_ap_iot_querySensor_up_t *pApIot = NULL;
			pro_ap_querySensor_up_t *pAp = NULL;
			pAp = (pro_ap_querySensor_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_querySensor_up_t *)pDstApSpec;
			
			pApIot->mCmd= ccQuery;		
			pAp->len=sizeof(pro_ap_querySensor_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->sCmd= ccSensorInfo;
			pApIot->TP_val=pAp->TP_val;
			pApIot->HM_val=pAp->HM_val;
			pApIot->PM2_5_val=pAp->PM2_5_val;
			pApIot->VOC_val=pAp->VOC_val;
			pApIot->taskStatus=pAp->taskStatus;
			pApIot->batteryStatus=pAp->batteryStatus;
			pApIot->fanStatus=pAp->fanStatus;
			pApIot->DLZ_Status=pAp->DLZ_Status;
			pApIot->backLedStatus=pAp->backLedStatus;
			pApIot->powerLedStatus=pAp->powerLedStatus;
			*pDstApSpecLen = sizeof(pro_ap_querySensor_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accQueryStatus:
		{
			pro_ap_iot_queryStatus_up_t *pApIot = NULL;
			pro_ap_queryStatus_up_t *pAp = NULL;
			pAp = (pro_ap_queryStatus_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_queryStatus_up_t *)pDstApSpec;
			
			pApIot->mCmd= ccQuery;		
			pAp->len=sizeof(pro_ap_queryStatus_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->sCmd= ccStatusInfo;
			pApIot->taskStatus=pAp->taskStatus;
			pApIot->batteryStatus=pAp->batteryStatus;
			pApIot->fanStatus=pAp->fanStatus;
			pApIot->DLZ_Status=pAp->DLZ_Status;
			*pDstApSpecLen = sizeof(pro_ap_queryStatus_up_t);
			ret = SUCCESS_XT;
			break;
		}
		case accReset:
		{
			pro_ap_iot_reset_up_t *pApIot = NULL;
			pro_ap_reset_up_t *pAp = NULL;
			pAp = (pro_ap_reset_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_reset_up_t *)pDstApSpec;
			
			pApIot->resp= ccRespond;		
			pAp->len=sizeof(pro_ap_reset_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccReset;
			pApIot->isSuccess=pAp->isSuccess;
			*pDstApSpecLen = sizeof(pro_ap_reset_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accRestart:
		{
			pro_ap_iot_restart_up_t *pApIot = NULL;
			pro_ap_restart_up_t *pAp = NULL;
			pAp = (pro_ap_restart_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_restart_up_t *)pDstApSpec;
			
			pApIot->resp= ccRespond;		
			pAp->len=sizeof(pro_ap_restart_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccRestart;
			pApIot->isSuccess=pAp->isSuccess;
			*pDstApSpecLen = sizeof(pro_ap_restart_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accTimming:
		{
			pro_ap_iot_timming_up_t *pApIot = NULL;
			pro_ap_timming_up_t *pAp = NULL;
			pAp = (pro_ap_timming_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_timming_up_t *)pDstApSpec;
			
			pApIot->resp= ccRespond;		
			pAp->len=sizeof(pro_ap_timming_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccTimming;
			pApIot->isSuccess=pAp->isSuccess;
			*pDstApSpecLen = sizeof(pro_ap_timming_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accTimeGet:
		{
			pro_ap_iot_timeGet_up_t *pApIot = NULL;
			pro_ap_timeGet_up_t *pAp = NULL;
			pAp = (pro_ap_timeGet_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_timeGet_up_t *)pDstApSpec;
			
			pApIot->mCmd= ccTimeGet;		
			pAp->len=sizeof(pro_ap_timeGet_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->year= pAp->year;
			pApIot->month= pAp->month;
			pApIot->day= pAp->day;
			pApIot->week= pAp->week;
			pApIot->hour= pAp->hour;
			pApIot->minute= pAp->minute;
			pApIot->second= pAp->second;
			*pDstApSpecLen = sizeof(pro_ap_timeGet_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accServerParaSet:
		{
			pro_ap_iot_serverParaSet_up_t *pApIot = NULL;
			pro_ap_serverParaSet_up_t *pAp = NULL;
			pAp = (pro_ap_serverParaSet_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_serverParaSet_up_t *)pDstApSpec;
			
			pApIot->mCmd= ccServerParaSet;		
			pAp->len=sizeof(pro_ap_serverParaSet_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->isSuccess=pAp->isSuccess;
			*pDstApSpecLen = sizeof(pro_ap_serverParaSet_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accServerParaGet:
		{
			pro_ap_iot_serverParaGet_up_t *pApIot = NULL;
			pro_ap_serverParaGet_up_t *pAp = NULL;
			pAp = (pro_ap_serverParaGet_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_serverParaGet_up_t *)pDstApSpec;
			
			pApIot->mCmd= ccServerParaGet;		
			pAp->len=sizeof(pro_ap_serverParaGet_up_t)-2;
			pApIot->len = pAp->len;
			MemMemcpyXTOS(&pApIot->linkType,&pAp->linkType,  pApIot->len);
			*pDstApSpecLen = sizeof(pro_ap_serverParaGet_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accVersion:
		{
			pro_ap_iot_version_up_t *pApIot = NULL;
			pro_ap_version_up_t *pAp = NULL;
			pAp = (pro_ap_version_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_version_up_t *)pDstApSpec;
			
			pApIot->mCmd= ccVersion;		
			pAp->len=sizeof(pro_ap_version_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->version=pAp->version;
			*pDstApSpecLen = sizeof(pro_ap_version_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accRunModeSet:
		{
			pro_ap_iot_runModeSet_up_t *pApIot = NULL;
			pro_ap_runModeSet_up_t *pAp = NULL;
			pAp = (pro_ap_runModeSet_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_runModeSet_up_t *)pDstApSpec;
			
			pApIot->resp=ccRespond;
			pAp->len=sizeof(pro_ap_runModeSet_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccRunModeSet;	
			pApIot->isSuccess=pAp->isSuccess;
			*pDstApSpecLen = sizeof(pro_ap_runModeSet_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accTimeTaskSet:
		{
			pro_ap_iot_timeTaskSet_up_t *pApIot = NULL;
			pro_ap_timeTaskSet_up_t *pAp = NULL;
			pAp = (pro_ap_timeTaskSet_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_timeTaskSet_up_t *)pDstApSpec;
			
			pApIot->resp=ccRespond;
			pAp->len=sizeof(pro_ap_timeTaskSet_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccTimeTaskSet;	
			pApIot->isSuccess=pAp->isSuccess;
			*pDstApSpecLen = sizeof(pro_ap_timeTaskSet_up_t);
			ret = SUCCESS_XT;
			break;
		}

		case accTimeTaskGet:
		{
			pro_ap_iot_timeTaskGet_up_t *pApIot = NULL;
			pro_ap_timeTaskGet_up_t *pAp = NULL;
			pAp = (pro_ap_timeTaskGet_up_t*)(pSrcAp);
			pApIot = (pro_ap_iot_timeTaskGet_up_t *)pDstApSpec;
			
			pApIot->resp=ccRespond;
			pAp->len=sizeof(pro_ap_timeTaskGet_up_t)-2;
			pApIot->len = pAp->len;
			pApIot->mCmd= ccTimeTaskGet;	
			pApIot->isSuccess=pAp->isSuccess;
			MemMemcpyXTOS(&pApIot->taskId, &pAp->taskId, pApIot->len-2);
			*pDstApSpecLen = sizeof(pro_ap_timeTaskGet_up_t);
			ret = SUCCESS_XT;
			break;
		}
		
		default:
			*pDstApSpecLen = 0;
			MDBG("ProApIotPack unsupport cmdId = 0x%x\n", mCmdId);
			break;
	}

	return ret;
}

xt_s32 ProApIotParse(xt_u8* pSrcApSpec, xt_s32 srcApSpecLen, xt_u8* pDstAp, xt_u8* pDstApLen)
{
	xt_u8 ret = FAILURE_XT;
	xt_u8 mCmd = ccNULL;
	xt_u8 sCmd = 0;
	xt_u8 sCmd2 = 0;
	mCmd = *pSrcApSpec;
	sCmd = *(pSrcApSpec+2);

	switch(mCmd)
	{
		case ccRespond:
		{
			switch(sCmd)
			{
				case ccLogin:
				{
					pro_ap_iot_login_down_t *pApIot = NULL;
					pro_ap_login_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_login_down_t*)(pSrcApSpec);
					pAp = (pro_ap_login_down_t *)pDstAp;
					
					pAp->mCmdId = accLogin;//accRespond;		
					pApIot->len=sizeof(pro_ap_login_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmdId = accLogin;
					pAp->isSuccess=pApIot->isSuccess;
					*pDstApLen = sizeof(pro_ap_login_down_t);
					ret = SUCCESS_XT;
					break;
				}
					

				case ccHeartBeat:
				{
					pro_ap_iot_heartBeat_down_t *pApIot = NULL;
					pro_ap_heartBeat_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_heartBeat_down_t*)(pSrcApSpec);
					pAp = (pro_ap_heartBeat_down_t *)pDstAp;
					
					pAp->mCmdId = accHeartBeat;//accRespond;		
					pApIot->len=sizeof(pro_ap_heartBeat_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmdId = accHeartBeat;
					pAp->isSuccess=pApIot->isSuccess;
					*pDstApLen = sizeof(pro_ap_heartBeat_down_t);
					ret = SUCCESS_XT;
					break;
				}


									
				default:
					break;
			}
		}
		break;

		case ccDoCtrl:
		{
			switch(sCmd)
			{
				case ccDoAll:
				{
					pro_ap_iot_doAll_down_t *pApIot = NULL;
					pro_ap_doAll_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_doAll_down_t*)(pSrcApSpec);
					pAp = (pro_ap_doAll_down_t *)pDstAp;
					
					pAp->mCmdId = accDoAll;//accDoCtrl;		
					pApIot->len=sizeof(pro_ap_doAll_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmdId = accDoAll;
					pAp->doFlag=pApIot->doFlag;
					*pDstApLen = sizeof(pro_ap_doAll_down_t);
					ret = SUCCESS_XT;
					break;
				}			
				case ccOpenFan:
				{
					pro_ap_iot_openFan_down_t *pApIot = NULL;
					pro_ap_openFan_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_openFan_down_t*)(pSrcApSpec);
					pAp = (pro_ap_openFan_down_t *)pDstAp;
					
					pAp->mCmdId = accOpenFan;//accDoCtrl;		
					pApIot->len=sizeof(pro_ap_openFan_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmdId = accOpenFan;
					pAp->PWM_val=pApIot->PWM_val;
					pAp->runTime=pApIot->runTime;
					*pDstApLen = sizeof(pro_ap_openFan_down_t);
					ret = SUCCESS_XT;
					break;
				}
				case ccCloseFan:
				{
					pro_ap_iot_closeFan_down_t *pApIot = NULL;
					pro_ap_closeFan_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_closeFan_down_t*)(pSrcApSpec);
					pAp = (pro_ap_closeFan_down_t *)pDstAp;
					
					pAp->mCmdId = accCloseFan;//accDoCtrl;		
					pApIot->len=sizeof(pro_ap_closeFan_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmdId = accCloseFan;
					*pDstApLen = sizeof(pro_ap_closeFan_down_t);
					ret = SUCCESS_XT;
					break;
				}
				case ccOpenDLZ:
				{
					pro_ap_iot_openDLZ_down_t *pApIot = NULL;
					pro_ap_openDLZ_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_openDLZ_down_t*)(pSrcApSpec);
					pAp = (pro_ap_openDLZ_down_t *)pDstAp;
					
					pAp->mCmdId = accOpenDLZ;//accDoCtrl;		
					pApIot->len=sizeof(pro_ap_openFan_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmdId = accOpenDLZ;
					pAp->reserve=pApIot->reserve;
					pAp->runTime=pApIot->runTime;
					*pDstApLen = sizeof(pro_ap_openDLZ_down_t);
					ret = SUCCESS_XT;
					break;
				}					
				case ccCloseDLZ:
				{
					pro_ap_iot_closeDLZ_down_t *pApIot = NULL;
					pro_ap_closeDLZ_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_closeDLZ_down_t*)(pSrcApSpec);
					pAp = (pro_ap_closeDLZ_down_t *)pDstAp;
					
					pAp->mCmdId = accCloseDLZ;//accDoCtrl;		
					pApIot->len=sizeof(pro_ap_closeDLZ_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmdId = accCloseDLZ;
					*pDstApLen = sizeof(pro_ap_closeDLZ_down_t);
					ret = SUCCESS_XT;
					break;
				}	

				case ccOpenLED:
				{
					pro_ap_iot_openLED_down_t *pApIot = NULL;
					pro_ap_openLED_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_openLED_down_t*)(pSrcApSpec);
					pAp = (pro_ap_openLED_down_t *)pDstAp;
					
					pAp->mCmdId = accOpenLED;//accDoCtrl;		
					pApIot->len=sizeof(pro_ap_openLED_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmdId = accOpenLED;
					pAp->flagLED=pApIot->flagLED;
					*pDstApLen = sizeof(pro_ap_openLED_down_t);
					ret = SUCCESS_XT;
					break;
				}					
				case ccCloseLED:
				{
					pro_ap_iot_closeLED_down_t *pApIot = NULL;
					pro_ap_closeLED_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_closeLED_down_t*)(pSrcApSpec);
					pAp = (pro_ap_closeLED_down_t *)pDstAp;
					
					pAp->mCmdId = accCloseLED;//accDoCtrl;		
					pApIot->len=sizeof(pro_ap_closeLED_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmdId = accCloseLED;
					pAp->flagLED=pApIot->flagLED;
					*pDstApLen = sizeof(pro_ap_closeLED_down_t);
					ret = SUCCESS_XT;
					break;
				}	
				default:
					break;
			}
		}
			break;
		case ccQuery:
		{
			switch(sCmd)
			{
				case ccSensorInfo:
				{
					pro_ap_iot_querySensor_down_t *pApIot = NULL;
					pro_ap_querySensor_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_querySensor_down_t*)(pSrcApSpec);
					pAp = (pro_ap_querySensor_down_t *)pDstAp;
					
					pAp->mCmdId = accQuerySensor;
					pApIot->len=sizeof(pro_ap_querySensor_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmd = pApIot->sCmd;
					*pDstApLen = sizeof(pro_ap_querySensor_down_t);
					ret = SUCCESS_XT;
					break;
				}
				case ccStatusInfo:
				{
					pro_ap_iot_queryStatus_down_t *pApIot = NULL;
					pro_ap_queryStatus_down_t *pAp = NULL;
					pApIot = (pro_ap_iot_queryStatus_down_t*)(pSrcApSpec);
					pAp = (pro_ap_queryStatus_down_t *)pDstAp;
					
					pAp->mCmdId = accQueryStatus;
					pApIot->len=sizeof(pro_ap_queryStatus_down_t)-2;
					pAp->len = pApIot->len;
					pAp->sCmd = pApIot->sCmd;
					*pDstApLen = sizeof(pro_ap_queryStatus_down_t);
					ret = SUCCESS_XT;
					break;
				}
			}
			break;
		}
		case ccReset:
		{
			pro_ap_iot_reset_down_t *pApIot = NULL;
			pro_ap_reset_down_t *pAp = NULL;
			pApIot = (pro_ap_iot_reset_down_t*)(pSrcApSpec);
			pAp = (pro_ap_reset_down_t *)pDstAp;
			
			pAp->mCmdId = accReset;
			*pDstApLen = sizeof(pro_ap_reset_down_t);
			ret = SUCCESS_XT;
			break;
		}
		case ccRestart:
		{
			pro_ap_iot_restart_down_t *pApIot = NULL;
			pro_ap_restart_down_t *pAp = NULL;
			pApIot = (pro_ap_iot_restart_down_t*)(pSrcApSpec);
			pAp = (pro_ap_restart_down_t *)pDstAp;
			
			pAp->mCmdId = accRestart;
			*pDstApLen = sizeof(pro_ap_restart_down_t);
			ret = SUCCESS_XT;
			break;
		}

		case ccTimming:
		{
			pro_ap_iot_timming_down_t *pApIot = NULL;
			pro_ap_timming_down_t *pAp = NULL;
			pApIot = (pro_ap_iot_timming_down_t*)(pSrcApSpec);
			pAp = (pro_ap_timming_down_t *)pDstAp;
			
			pAp->mCmdId = accTimming;

			pAp->year=pApIot->year;
			pAp->month=pApIot->month;
			pAp->day=pApIot->day;
			pAp->week=pApIot->week;
			pAp->hour=pApIot->hour;
			pAp->minute=pApIot->minute;
			pAp->second=pApIot->second;
			*pDstApLen = sizeof(pro_ap_timming_down_t);
			ret = SUCCESS_XT;
			break;
		}

		case ccTimeGet:
		{
			pro_ap_iot_timeGet_down_t *pApIot = NULL;
			pro_ap_timeGet_down_t *pAp = NULL;
			pApIot = (pro_ap_iot_timeGet_down_t*)(pSrcApSpec);
			pAp = (pro_ap_timeGet_down_t *)pDstAp;
			
			pAp->mCmdId = accTimeGet;

			*pDstApLen = sizeof(pro_ap_timeGet_down_t);
			ret = SUCCESS_XT;
			break;
		}

		case ccServerParaSet:
		{
			pro_ap_iot_serverParaSet_down_t *pApIot = NULL;
			pro_ap_serverParaSet_down_t *pAp = NULL;
			pApIot = (pro_ap_iot_serverParaSet_down_t*)(pSrcApSpec);
			pAp = (pro_ap_serverParaSet_down_t *)pDstAp;
			
			pAp->mCmdId = accServerParaSet;

			MemMemcpyXTOS(&pAp->linkType, &pApIot->linkType, pApIot->len);

			*pDstApLen = sizeof(pro_ap_serverParaSet_down_t);
			ret = SUCCESS_XT;
			break;
		}

		case ccServerParaGet:
		{
			pro_ap_iot_serverParaGet_down_t *pApIot = NULL;
			pro_ap_serverParaGet_down_t *pAp = NULL;
			pApIot = (pro_ap_iot_serverParaGet_down_t*)(pSrcApSpec);
			pAp = (pro_ap_serverParaGet_down_t *)pDstAp;
			
			pAp->mCmdId = accServerParaGet;

			*pDstApLen = sizeof(pro_ap_serverParaGet_down_t);
			ret = SUCCESS_XT;
			break;
		}
		case ccVersion:
		{
			pro_ap_iot_version_down_t *pApIot = NULL;
			pro_ap_version_down_t *pAp = NULL;
			pApIot = (pro_ap_iot_version_down_t*)(pSrcApSpec);
			pAp = (pro_ap_version_down_t *)pDstAp;
			
			pAp->mCmdId = accVersion;

			*pDstApLen = sizeof(pro_ap_version_down_t);
			ret = SUCCESS_XT;
			break;
		}		

		case ccRunModeSet:
		{
			pro_ap_iot_runModeSet_down_t *pApIot = NULL;
			pro_ap_runModeSet_down_t *pAp = NULL;
			pApIot = (pro_ap_iot_runModeSet_down_t*)(pSrcApSpec);
			pAp = (pro_ap_runModeSet_down_t *)pDstAp;
			
			pAp->mCmdId = accRunModeSet;
			pAp->runMode = pApIot ->runMode;
			*pDstApLen = sizeof(pro_ap_runModeSet_down_t);
			ret = SUCCESS_XT;
			break;
		}

		case ccTimeTaskSet:
		{
			pro_ap_iot_timeTaskSet_down_t *pApIot = NULL;
			pro_ap_timeTaskSet_down_t *pAp = NULL;
			pApIot = (pro_ap_iot_timeTaskSet_down_t*)(pSrcApSpec);
			pAp = (pro_ap_timeTaskSet_down_t *)pDstAp;
			
			pAp->mCmdId = accTimeTaskSet;
			MemMemcpyXTOS(&pAp->taskId, &pApIot->taskId, pApIot->len);
			*pDstApLen = sizeof(pro_ap_timeTaskSet_down_t);
			ret = SUCCESS_XT;
			break;
		}
		
		case ccTimeTaskGet:
		{
			pro_ap_iot_timeTaskGet_down_t *pApIot = NULL;
			pro_ap_timeTaskGet_down_t *pAp = NULL;
			pApIot = (pro_ap_iot_timeTaskGet_down_t*)(pSrcApSpec);
			pAp = (pro_ap_timeTaskGet_down_t *)pDstAp;
			
			pAp->mCmdId = accTimeTaskGet;
			pAp->taskId= pApIot->taskId;
			*pDstApLen = sizeof(pro_ap_timeTaskGet_down_t);
			ret = SUCCESS_XT;
			break;
		}
		default:
			*pDstApLen = 0;
			MDBG("ProApIotParse unsupport cmd = 0x%x\n", mCmd);
			break;
	}

	return ret;
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
