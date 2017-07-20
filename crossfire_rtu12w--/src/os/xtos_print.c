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
#include "xtos_sem.h"

static os_mutex_t printXTOSMutex = NULL;

xt_s32 VprintfXTOS(const xt_s8* pFormat, va_list arg)
{
	return vprintf(pFormat, arg);
}

xt_s32 VSprintfXTOS(xt_s8* pBuf,const xt_s8* pFormat, va_list arg)
{
	return vsprintf(pBuf, pFormat, arg);
}

xt_s32 PrintfXTOS(const xt_s8* pFormat,...)
{
	va_list arg; 
	xt_s32 done = 0; 

#ifndef WIN32
	if (printXTOSMutex == NULL)
		MutexCreateXTOS(printXTOSMutex);
	MutexLockXTOS(printXTOSMutex);
#else
	taskENTER_CRITICAL();
#endif
	va_start(arg, pFormat); 
	done += VprintfXTOS(pFormat, arg); 
	va_end(arg);
#ifndef WIN32
	MutexUnlockXTOS(printXTOSMutex);
#else
	taskEXIT_CRITICAL();
#endif
	return done;
}

xt_s32 SprintfXTOS(xt_s8* pBuf, const xt_s8* pFormat,...)
{
	va_list arg; 
	xt_s32 done = 0; 

#ifndef WIN32
	if (printXTOSMutex == NULL)
		MutexCreateXTOS(printXTOSMutex);
	MutexLockXTOS(printXTOSMutex);
#else
	taskENTER_CRITICAL();
#endif
	va_start(arg, pFormat); 
	done += VSprintfXTOS(pBuf, pFormat, arg); 
	va_end(arg);
#ifndef WIN32
	MutexUnlockXTOS(printXTOSMutex);
#else
	taskEXIT_CRITICAL();
#endif
	return done;
}

//todo:
xt_s32 LogPrintf(log_level_n level, const xt_s8* pFormat, ...)
{
	//PrintfXTOS("in LogPrintf: %d %d\n",level, glbCcedPara.debugPara.tLogLevel);
	if (level >= 0)
	{
		va_list arg; 
	    xt_s32 done = 0; 

#ifndef WIN32
		if (printXTOSMutex == NULL)
			MutexCreateXTOS(printXTOSMutex);
		MutexLockXTOS(printXTOSMutex);
#else
		taskENTER_CRITICAL();
#endif
	    va_start(arg, pFormat); 
	    done += VprintfXTOS(pFormat, arg); 
	    va_end(arg);
#ifndef WIN32
		MutexUnlockXTOS(printXTOSMutex);
#else
		taskEXIT_CRITICAL();
#endif
		//return done;
	}
	return SUCCESS_XT;
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
