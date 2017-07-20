#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>
#include <string.h>

#include "xtsys_type.h"
#include "xtsys_cfg.h"


#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"

#include "xt_bsp_rtu09c.h"
#include "global_para.h"
#include "drv_uart.h"


#include "conn_uart.h"

#include "xt_conn.h"
#include "xt_dev.h"
#include "ap.h"

#include "pro_dev_iot.h"
#include "config.h"
#include "dev_solar_controller.h"
#include "dev_solar_phili.h"
#include "dev_solar_yuanfang.h"
#include "dev_solar_huineng.h"

#ifdef CONNECT_SOLAR_MODULE
extern xt_dev_t gDevList_level2[DEV_LV2_MAX];
comfirm_enum_t is_sloar_module_ini=NO;


xt_solar_dev_t gSolarList[SLOAR_MAX] =
{
	#if defined (PHILI_SOLAR)
	{
		DevSolarPhiliInit,
		NULL,
		NULL,
		DevSolarPhiliSend,
		DevSolarPhiliRecv,
		DevSolarPhiliCtrl,
		DevSolarPhiliWriteAndRead,
	},
	#elif defined(YF_SOLAR)
	{
		DevSolarYfInit,
		NULL,
		NULL,
		DevSolarYfSend,
		DevSolarYfRecv,
		DevSolarYfCtrl,
		DevSolarYfWriteAndRead
	},
	#elif defined(HN_SOLAR)
	{
		DevSolarHnInit,
		NULL,
		NULL,
		DevSolarHnSend,
		DevSolarHnRecv,
		DevSolarHnCtrl,
		DevSolarHnWriteAndRead
	},
	#endif
};

xt_void  DevLv2_SolarInit(void)
{
	#if defined (PHILI_SOLAR)
	gSolarList[SOLAR_PHILI].xSolarInit();
	#elif defined(YF_SOLAR)
	gSolarList[SOLAR_YF].xSolarInit();
	#elif defined(HN_SOLAR)
	gSolarList[SOLAR_HN].xSolarInit();
	#endif	
	
}


xt_void  DevLv2_SolarOpen(void)
{
	XtBspCtrlLv2UsartConfig();
}

xt_s32	DevLv2_Send2Solar(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead)
{
	#if defined (EP_SOLAR)
	#elif defined (PHILI_SOLAR)
	return gSolarList[SOLAR_PHILI].xSolarSend(pData,dataLen);
	#elif defined(YF_SOLAR)
	return gSolarList[SOLAR_YF].xSolarSend(pData,dataLen);
	#elif defined(HN_SOLAR)
	return gSolarList[SOLAR_HN].xSolarSend(pData,dataLen);
	#else
	return FAILURE_XT;	
	#endif
}

xt_s32	DevLv2_RecvFromSolar(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam)
{
	#if defined (EP_SOLAR)
	#elif defined (PHILI_SOLAR)
	return gSolarList[SOLAR_PHILI].xSolarRecv(pData,dataLen);
	#elif defined(YF_SOLAR)
	return gSolarList[SOLAR_YF].xSolarRecv(pData,dataLen);
	#elif defined(HN_SOLAR)
	return gSolarList[SOLAR_HN].xSolarRecv(pData,dataLen);
	#else
	return FAILURE_XT;
	#endif
}

xt_void  DevLv2_SolarCtrl(xt_u8* src_pData,xt_u8* dec_pData)
{
	#if defined( EP_SOLAR)
	#elif defined( PHILI_SOLAR)
	gSolarList[SOLAR_PHILI].xSolarCtrl(src_pData,dec_pData);
	#elif defined(YF_SOLAR)
	gSolarList[SOLAR_YF].xSolarCtrl(src_pData,dec_pData);
	#elif defined(HN_SOLAR)
	gSolarList[SOLAR_HN].xSolarCtrl(src_pData,dec_pData);
	#endif
}


#endif
#ifdef __cplusplus
extern "C" }
#endif // End for __cplusplus

