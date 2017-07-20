#ifndef DEV_WCMDA_YOUFANG_H
#define DEV_WCMDA_YOUFANG_H

xt_void  DevWcdmaYoufangInit(void);
xt_s32	DevWcdmaYoufangSend(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead);
xt_s32	DevWcdmaYoufangRecv(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam);
#endif
