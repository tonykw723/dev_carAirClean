/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __LIB_ST_AES_H_
#define __LIB_ST_AES_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

//#define LIB_USE_MCU_CRYPE
#define LIB_USE_EXT_CRYPE

#if defined (LIB_USE_MCU_CRYPE)
#include "stm32l1xx.h"
#include "stm32l1xx_rcc.h"
#elif defined (LIB_USE_EXT_CRYPE)
#include "stm32l1xx_conf.h"

ErrorStatus IotSm4EncryptEcb(uint8_t* Input, uint32_t Ilength, uint8_t* Output);

ErrorStatus IotSm4DecryptEcb(uint8_t* Input, uint32_t Ilength, uint8_t* Output);

#endif

ErrorStatus IotAesEncryptEcb(uint8_t* Input, uint32_t Ilength, uint8_t* Output);

ErrorStatus IotAesDecryptEcb(uint8_t* Input, uint32_t Ilength, uint8_t* Output);

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __LIB_ST_AES_H_

