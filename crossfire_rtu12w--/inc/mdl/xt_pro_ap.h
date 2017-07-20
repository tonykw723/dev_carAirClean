/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __XT_PRO_AP_H_
#define __XT_PRO_AP_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

typedef struct
{

	xt_u8				proApId;

	xt_void				(*xProApInit)(void);

	xt_void				(*xProApOpen)(void);

	xt_void				(*xProApClose)(void);

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
	xt_s32				(*xProApPack)(xt_u8* pSrcAp, xt_s32 apSrcLen, xt_u8* pDstApSpec, xt_u8* pDstApSpecLen);

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
	xt_s32				(*xProApParse)(xt_u8* pSrcApSpec, xt_s32 srcApSpecLen, xt_u8* pDstAp, xt_u8* pDstApLen);

	xt_void				(*xProApCtrl)(void);
} xt_pro_ap_t;

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XT_PRO_AP_H_

