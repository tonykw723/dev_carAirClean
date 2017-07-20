/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __XTSYS_CFG_H_
#define __XTSYS_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#ifdef WIN32
#define PACKED 
#else
#define PACKED __attribute__ ((packed)) //__packed
 
#endif

//option for debug information output control
#define MDBG		PrintfXTOS
#define LOG_PRINTF	LogPrintf

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XTSYS_CFG_H_

