/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/

#ifndef __XTOS_MEM_H_
#define __XTOS_MEM_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#define MemMallocXTOS(numBytes)			pvPortMalloc(numBytes)

#define MemFreeXTOS(pBuf)				vPortFree(pBuf)

#define MemMemsetXTOS(pBuf, c, n)		memset(pBuf, c, n)

#define MemMemcpyXTOS(pDst, pSrc, size) memcpy(pDst, pSrc, size)

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __XTOS_MEM_H_

