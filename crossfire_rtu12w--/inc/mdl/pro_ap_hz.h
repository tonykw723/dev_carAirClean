/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __PRO_AP_HZ_H_
#define __PRO_AP_HZ_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#define PRO_AP_HZ_PROID 1

typedef  enum	
{
	//specific application level protocol for light open
	hccOpen		=	0x32,

	//specific application level protocol for light close
	hccClose	=	0x33,
	hccNULL		=	0,
}PACKED pro_ap_hz_ctrl_cmd_n;

typedef  struct 
{
	xt_u8	cmd;
	xt_u8	index;
}PACKED pro_ap_hz_ctrl_t;

typedef  struct 
{
	xt_u8	proId;
	xt_u8	DimmingIndex:4;//low 4 bits
	xt_u8	lightIndex:4;//hight 4 bits
}PACKED pro_ap_hz_resp_t;

typedef  struct 
{
	pro_ap_hz_resp_t resp;
	xt_u8	cmd;
	xt_u8	index;
}PACKED pro_ap_hz_ctrl_out_t;

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
xt_s32 ProApHzPack(xt_u8* pSrcAp, xt_s32 apSrcLen, xt_u8* pDstApSpec, xt_u8* pDstApSpecLen);

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
xt_s32 ProApHzParse(xt_u8* pSrcApSpec, xt_s32 srcApSpecLen, xt_u8* pDstAp, xt_u8* pDstApLen);

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __PRO_AP_HZ_H_

