#include <stdio.h>
#include <stdlib.h>
#include "stm32l1xx.h"
#include "xtsys_cfg.h"
#include "xtsys_type.h"
#include "xtos_task.h"
#include "xtos_timer.h"

#include "xt_bsp_rtu09c.h"
#include "odbCtrl.h"
#include "global_para.h"

extern rtu_para_t glbRtuPara;

xt_s32 odb_readDatas(xt_u8* pData,xt_s32* pDataLen)
{
	xt_s32 ret=0xff,i;
	ret = XtBspOdbRead( pData, pDataLen);
	if(ret>0)
	{
		PrintfXTOS("odb read are:\n");
		for(i=0;i<*pDataLen;i++)
			PrintfXTOS(" %0.2x",pData[i]);
		PrintfXTOS(" \n");
	}
	return ret;
}

xt_s32 odb_chkDatas(xt_u8* pData,xt_s32* pDataLen)
{
	xt_u8 i,j,len=0,sum=0;
	xt_u8 buf[32];
	xt_s32 val=-1;
	for(i=0;i<*pDataLen-2;i++)
	{
		if((pData[i]==0xaa)&&(pData[i+1]==0x55))
		{
			len=pData[i+2];
			break;
		}
	}
	if(len<5)
	{
		PrintfXTOS("odb datas are little err!:\n");
		return val;
	}
	for(j=0;j<len;j++)
	{
		buf[j]=pData[i+j];
	}

	PrintfXTOS("odb datas get are:\n");
	for(i=0;i<len;i++)
		PrintfXTOS(" %0.2x",pData[i]);
	PrintfXTOS(" \n");
	for(i=2;i<len-1;i++)
		sum+=buf[i];
	if(sum==buf[i])//chksum success
	{
		val=buf[i-1];
		PrintfXTOS("odb val is:");
		PrintfXTOS(" %0.2x\n",val);
	}
	else
		PrintfXTOS("chk Err!");



	
	return val;
	
}

void odb_doTask(xt_u8* pData,xt_s32* pDataLen)
{
	static xt_u8 flag0=0,flag1=0;
	if(odb_readDatas(pData,pDataLen)>0)
	{
		glbRtuPara.runningInfo.flag_carPowerOn=odb_chkDatas(pData,pDataLen);
	}
	if(glbRtuPara.runningInfo.flag_carPowerOn==YES)
	{
		if(flag0==0)
		{
			flag0=1;
			PrintfXTOS("car power on and do sensor task\n");
		}
		flag1=0;
		do_sensorTask();
	}
	else
	{
		if(flag1==0)
		{
			flag1=1;
			PrintfXTOS("car power off and do time sensor task\n");
		}
		flag0=0;
		do_timeSensorTask();
	}
	
}

