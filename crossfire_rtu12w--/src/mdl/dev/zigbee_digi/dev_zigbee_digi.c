#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>
#include <string.h>

#include "xtsys_type.h"
#include "xtsys_cfg.h"

#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"

#include "xt_bsp_rtu09c.h"
#include "drv_uart.h"

#include "conn_uart.h"

#include "xt_conn.h"
#include "xt_dev.h"

#include "pro_dev_iot.h"
#include "config.h"
#include "dev_zigbee_shunzhou.h"
#include "global_para.h"
#include "lib_st_aes.h"
#include "dev_zigbee_digi.h"
extern rtu_para_t glbRtuPara;

xt_u8 bufData[]={0x7e,0x00,0x14,0x10,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xfe,0x00,0x00,0x54,0x78,0x44,0x61,0x74,0x61,0xad};

xt_u8 UTDigiCrc8(xt_u8 *data,xt_u8 datalen)
{
	xt_u32 sum=0;
	xt_u8 temp=0;
	xt_u8 i;
	for(i=0;i<datalen;i++)
	{
		sum+=data[i];
	}
	temp=0xff-sum;
//	printf("sum=%d,UTCrc8=%d\n",sum,temp);
	return temp;
}

static xt_void ZigbeeDigiDataOut(xt_u8 *pt_out ,xt_u8 len,xt_u16 sleep_time)
{
	xt_u8 i;
	XtBspCtrlWrite(pt_out,len);	
	PrintfXTOS(" ttttttttt\n");
	for(i=0;i<len;i++)
		PrintfXTOS(" %0.2x",pt_out[i]);
	PrintfXTOS("\n ttttttttt\n");	
	TaskSleepMs(sleep_time);
}

xt_void  DevZigbeeDigiInit(void)
{
	if(glbRtuPara.devPara.flag_dev_ini==YES)
		return ;
	glbRtuPara.devPara.flag_dev_ini=YES;
	XtBspCommInit(115200,USART_Parity_No);
	PrintfXTOS("DigiInit:115200 N 8 1\n");
}


xt_s32	DevZigbeeDigiSend(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead)
{
	return FAILURE_XT;
}

xt_s32	DevZigbeeDigiRecv(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam)
{
	xt_u8 i,crc8T;
	xt_u32 recv_val=0;
	digiProtocolType *digiProT=NULL;
	static xt_u32 recv_ok_cnt=0;
	//static xt_u8 sendT[]={0x7e,0x00,0x13,0x10,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xfe,0x00,0x00,0x54,0x54,0x78,0x44,0x61,0xad};
	if(glbRtuPara.devPara.flag_dev_ini==NO)
	{
		return FAILURE_XT;
	}	
	if(dataLen<16)
	{
		MemMemsetXTOS(pData, 0, dataLen);
		return FAILURE_XT;
	}
	if (dataLen > 0)
	{		
		for(i=0;i<dataLen-8;i++)
		{
			if((pData[i]==0x7e)&&((pData[i+1]&0xf0)==0x00))
			{
				break;
			}
		}			

	}
	digiProT=(digiProtocolType *)&pData[i];
	PrintfXTOS("len=%x\n",digiProT->len);
	digiProT->len>>=8;
	PrintfXTOS("#1=%x %x\n",digiProT->len,digiProT->apID);
	digiProT->crc8=pData[i+digiProT->len+3];
	crc8T=UTDigiCrc8(&digiProT->apID,digiProT->len);

	if(crc8T==digiProT->crc8)
	{
		if(digiProT->len==0x10)
		{
			recv_ok_cnt++;
			recv_val=0;
			recv_val|=digiProT->DataBuf[4];
			recv_val<<=8;
			recv_val|=digiProT->DataBuf[5];
			recv_val<<=8;
			recv_val|=digiProT->DataBuf[6];
			recv_val<<=8;
			recv_val|=digiProT->DataBuf[7];	
		
			PrintfXTOS("-----------chk ok send %d VS recv %d\n",recv_val,recv_ok_cnt);
			for(i=0;i<digiProT->len+4;i++)
				PrintfXTOS("%0.2x ",pData[i]);
			PrintfXTOS("\n");
			MemMemsetXTOS(pData, 0, dataLen);
		}
		/*
		sendCnt++;
		lenT=0x0e+5;
		sendT[2]=lenT;
		sendT[lenT+3-5]='R';
		sendT[lenT+3-4]=sendCnt>>24;
		sendT[lenT+3-3]=sendCnt>>16;
		sendT[lenT+3-2]=sendCnt>>8;
		sendT[lenT+3-1]=sendCnt&0xff;
		crc8T=UTDigiCrc8(&sendT[3],lenT);
		sendT[lenT+3]=crc8T;
		ZigbeeDigiDataOut(sendT,sizeof(sendT),0);
		*/
	}
	else
	{
		PrintfXTOS("chk %0.2x vs %0.2x\n",digiProT->crc8,crc8T);
	}
	return FAILURE_XT;
}




