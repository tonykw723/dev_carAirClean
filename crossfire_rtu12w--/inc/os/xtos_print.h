/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/

#ifndef __XTOS_PRINT_H_
#define __XTOS_PRINT_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdarg.h>	
#include "xtsys_type.h"
typedef enum
{
	LOG_LEVEL_DEBUG = 0,
	LOG_LEVEL_INFO,
	LOG_LEVEL_NOTICE,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
} log_level_n;

xt_s32 VprintfXTOS(const xt_s8* pFormat, va_list arg);

xt_s32 VSprintfXTOS(xt_s8* pBuf,const xt_s8* pFormat, va_list arg);

xt_s32 PrintfXTOS(const xt_s8* pFormat,...);

xt_s32 SprintfXTOS(xt_s8* pBuf, const xt_s8* pFormat,...);

//todo:
xt_s32 LogPrintf(log_level_n level, const xt_s8* pFormat, ...);

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XTOS_PRINT_H_

