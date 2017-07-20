#ifndef DEV_ZIGBEE_SIXIN_H
#define DEV_ZIGBEE_SIXIN_H

xt_void  DevZigbeeSixinInit(void);
xt_s32	DevZigbeeSixinSend(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead);
xt_s32	DevZigbeeSixinRecv(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam);
#endif

