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
#include "pro_ap_hz.h"

xt_s32 ProApHzPack(xt_u8* pSrcAp, xt_s32 srcApLen, xt_u8* pDstApSpec, xt_u8* pDstApSpecLen)
{
	xt_u8 ret = FAILURE_XT;
	xt_u8 cmd = accNULL;

	cmd = *(xt_u8*)pSrcAp;

	switch(cmd)
	{
	case accOpen:
	case accClose:
		{
			pro_ap_hz_ctrl_out_t *pApHz = NULL;
			pro_ap_open_close_ctrl_out_t *pAp = NULL;

			//todo: check length
			pAp = (pro_ap_open_close_ctrl_out_t *)pSrcAp;
			pApHz = (pro_ap_hz_ctrl_out_t*)(pDstApSpec);

			//todo: check resp data from param data
			pApHz->resp.proId = PRO_AP_HZ_PROID;
			pApHz->resp.lightIndex = 1;
			pApHz->resp.DimmingIndex = 1;

			if (cmd == accOpen)
				pApHz->cmd = hccOpen;
			else if (cmd == accClose)
				pApHz->cmd = hccClose;

			pApHz->index = pAp->index;
	
			*pDstApSpecLen = sizeof(pro_ap_hz_ctrl_out_t);
			
			ret = SUCCESS_XT;
		}
		break;
	default:
		*pDstApSpecLen = 0;
		MDBG("ProApHzPack unsupport cmd = 0x%x\n", cmd);
		break;
	}

	return ret;
}

xt_s32 ProApHzParse(xt_u8* pSrcApSpec, xt_s32 srcApSpecLen, xt_u8* pDstAp, xt_u8* pDstApLen)
{
	xt_u8 ret = FAILURE_XT;
	xt_u8 cmd = hccNULL;

	cmd = *pSrcApSpec;

	switch(cmd)
	{
	case hccOpen:
	case hccClose:
		{
			pro_ap_hz_ctrl_t *pApHz = NULL;
			pro_ap_open_close_dim_ctrl_t *pAp = NULL;

			//todo: check length
			pApHz = (pro_ap_hz_ctrl_t*)(pSrcApSpec);
			pAp = (pro_ap_open_close_dim_ctrl_t *)pDstAp;

			if(cmd == hccOpen)
			{
				pAp->cmd = accOpen;
				pAp->dimming = PRO_AP_MAX_DIMMING;
			}
			else if(cmd == hccClose)
			{
				pAp->cmd = accClose;
				pAp->dimming = PRO_AP_MIN_DIMMING;
			}
			
			pAp->index = pApHz->index;

			*pDstApLen = sizeof(pro_ap_open_close_dim_ctrl_t);

			ret = SUCCESS_XT;
		}
		break;
	default:
		*pDstApLen = 0;
		MDBG("ProApHzParse unsupport cmd = 0x%x\n", cmd);
		break;
	}

	return ret;;
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
