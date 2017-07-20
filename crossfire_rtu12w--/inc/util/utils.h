#ifndef __UTILS_H_
#define __UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

xt_void UTPrinthex(xt_u8 *buf, int len);
xt_u8 UTCrc8(xt_u8 *data,xt_u8 datalen);
xt_s32 UTCheckProData(packet_head_t* pHead);

#ifdef __cplusplus
}
#endif

#endif
 
