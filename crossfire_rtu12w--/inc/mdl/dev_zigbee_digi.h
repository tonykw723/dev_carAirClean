#ifndef DEV_ZIGBEE_DIGI_H
#define DEV_ZIGBEE_DIGI_H
typedef struct
{
	xt_u8 head;
	xt_u16 len;//注-大端模式，用时要转换
	xt_u8 apID;
	xt_u8 frameID;
	xt_u16 cast;//注-大端模式，用时要转换
	xt_u16 castAddr;
	xt_u8   castRadius;
	xt_u8 txSelect;
	xt_u8 DataBuf[100];
	xt_u8 crc8;
}PACKED digiProtocolType;
xt_void  DevZigbeeDigiInit(void);
xt_s32	DevZigbeeDigiSend(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead);
xt_s32	DevZigbeeDigiRecv(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam);
#endif

