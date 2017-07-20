#ifdef __cplusplus
extern "C" {
#endif

#include "xtsys_type.h"
#include "xtsys_cfg.h"

//#include "xtos_mem.h"
//#include <stdarg.h>
//#include "xtos_print_i.h"
//#include "xtos_print.h"
//#include "xtos_time.h"
#include "pro_dev_iot.h"

#include "utils.h"

xt_void UTPrinthex(xt_u8 *buf, xt_s32 len)
{
	int i;
	//PrintfXTOS("below hex len [%d]\n",len);
	for (i=0;i<len;i++)
	{
		if(((i+1)%16 )== 1)
			PrintfXTOS("%03d     ",i);
		PrintfXTOS("%02x ",buf[i]);
		if(((i+1)%8) == 0)
			PrintfXTOS("   ");
		if(((i+1)%16 )== 0)
			PrintfXTOS("\n");		

	}
	PrintfXTOS("\n");
}

xt_u8 UTCrc8(xt_u8 *data,xt_u8 datalen)
{
	xt_u32 sum=0;
	xt_u8 temp=0;
	xt_u8 i;
	for(i=0;i<datalen;i++)
	{
		sum+=data[i];
	}
	temp=sum&0xff;
//	printf("sum=%d,UTCrc8=%d\n",sum,temp);
	return temp;
}


xt_s32 UTCheckProData(packet_head_t* pHead)
{
	xt_u8 *pCrc,*pEnd;
	if(pHead == NULL)
		return FAILURE_XT;
	if(pHead->stx != PACK_START_FLAG)
		return FAILURE_XT;
	if(pHead->stx2 != PACK_START_FLAG)
		return FAILURE_XT;
	pCrc = (xt_u8*)pHead + sizeof(packet_head_t)+pHead->len;
	pEnd = (xt_u8*)pHead + sizeof(packet_head_t)+pHead->len + 1;
	if(*pEnd != PACK_END_FLAG)
		return FAILURE_XT;
	if(*pCrc != UTCrc8((xt_u8*)pHead,pHead->len + sizeof(packet_head_t))){
		return FAILURE_XT;
	}
	return SUCCESS_XT;
}

#ifdef __cplusplus
}
#endif

