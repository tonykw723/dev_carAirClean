/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __XT_DEV_H_
#define __XT_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus




typedef  struct 
{
	xt_s64	commSrc;
	xt_s64	commDst;
	xt_u8	commCmd;
	xt_u8	commGroup;
	
}PACKED xt_dev_info_t;

typedef struct
{
	xt_u8				devId;

	xt_void				(*xDevInit)(void);

	xt_void				(*xDevOpen)(void);

	xt_void				(*xDevClose)(void);

	/**
	* @brief:				pack specific application level protocol with communication param to device protocol and send through specific connection 
	*
	* @param pData:			pointer to specific application level protocol
	*
	* @param dataLen:		specific application level protocol length
	*
	* @param pDevParam:		communication param, such as destination address , group ...
	*
	* @param pxWrite:		pointer to specific connection write
	*
	* @param pxRead:		pointer to specific connection read
	*
	* @return:				data length of success sent 
	**/
	xt_s32				(*xDevSend)(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite,pxConnRead pxRead);

	/**
	* @brief:				parse raw receive data to device protocol and output specific application level protocol with communication param
	*
	* @param pData:			pointer to raw receive data
	*
	* @param dataLen:		raw receive data length
	*
	* @param pRetData:		pointer to specific application level protocol @return
	*
	* @param pRetDataLen:	specific application level protocol length @return
	*
	* @param pDevParam:		communication param, such as destination address , group ... @return
	*
	* @return:				SUCCESS_XT: parse a package successful and maybe have some left data
	*						FAILURE_XT: parse done for all receive data 
	*						-- caller should check return code for multiple package process
	**/
	xt_s32				(*xDevRecv)(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam);

	xt_void				(*xDevCtrl)(xt_u8* src_pData,xt_u8* dec_pData);
} xt_dev_t;

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XT_DEV_H_

