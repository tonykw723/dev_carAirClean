#ifndef _DEV_SOLAR_CONTROLLER_H
#define _DEV_SOLAR_CONTROLLER_H
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus
//--------------------------------------
#define CONNECT_SOLAR_MODULE
#ifdef CONNECT_SOLAR_MODULE
#define DEV_LEVEL2 CONNECT_SOLAR_MODULE
#define PHILI_SOLAR
//#define YF_SOLAR	
//#define EP_SOLAR
//#define HN_SOLAR
//-------------------------------------
typedef enum
{
	#if defined (PHILI_SOLAR)
	SOLAR_PHILI,
	#elif defined(YF_SOLAR)
	SOLAR_YF,
	#elif defined(HN_SOLAR)
	SOLAR_HN,
	#endif
	SLOAR_MAX
}SOLAR_DEV_ENUM;


typedef struct
{

	xt_void				(*xSolarInit)(void);

	xt_void				(*xSolarOpen)(void);

	xt_void				(*xSolarClose)(void);

	xt_s32				(*xSolarSend)(xt_u8* pData, xt_s32 dataLen);

	xt_s32				(*xSolarRecv)(xt_u8* pData, xt_u32 maxLen);

	xt_void				(*xSolarCtrl)(xt_u8* src_pData,xt_u8* dec_pData);
	xt_s32        (*xSolarReadWrite)(xt_u8 id,xt_u8 cmd,xt_u8 *value,xt_u8 len);
} xt_solar_dev_t;

xt_void  DevLv2_SolarInit(void);
xt_void  DevLv2_SolarOpen(void);
//xt_s32	DevLv2_Send2Solar(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead);
//xt_s32	DevLv2_RecvFromSolar(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam);
xt_void  DevLv2_SolarCtrl(xt_u8* src_pData,xt_u8* dec_pData);


#endif
#endif

