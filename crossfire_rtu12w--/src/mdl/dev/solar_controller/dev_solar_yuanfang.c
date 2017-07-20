
#include <stdio.h>
#include <string.h>

#include "xtsys_type.h"
#include "xtsys_cfg.h"


#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"

#include "xt_bsp_rtu09c.h"
#include "global_para.h"
#include "drv_uart.h"


#include "conn_uart.h"

#include "xt_conn.h"
#include "xt_dev.h"
#include "ap.h"

#include "pro_dev_iot.h"
#include "config.h"

#include "dev_solar_controller.h"
#include "dev_solar_yuanfang.h"
#ifdef YF_SOLAR
#define  CHK_COMMUNICATION_ERR_UPPER 40
//--------------------------------------------
extern rtu_para_t glbRtuPara;
extern xt_s32 rs485DataLen ;
extern ap_msg_t ap485MsgData ;
extern comfirm_enum_t is_sloar_module_ini;
//-------------------------------------------------
#define  READ_SOLAR_CTRL_TIMEOUT 500
solar_getTask_yf_t solarGetYfTask;
sloarYf_t solarYfData;
//solar_setTask_yf_t solarSetDataYF;
xt_u8 solarYF_maxCurrent=0;
static xt_u8 yfDataLen=0;
xt_u8 recvTempBuf[60];
xt_u8* dec_temp=(xt_u8*)&solarGetYfTask;
extern xt_time_t ap485RecvTime ;

//=========================================
xt_void  DevSolarYfInit(void)
{
	xt_u8 sendBuf[]={0x20,0x04,0x00,0x24};
	if(is_sloar_module_ini==YES)
		return;
	if(DevSolarYfWriteAndRead(0,0,sendBuf,sizeof(sendBuf))==SUCCESS_XT)
	{
		is_sloar_module_ini=YES;
		printf("loadStatus=%x\n",solarYfData.loadStatus);
		if(solarYfData.loadStatus==0)//负载关灯
		{
			glbRtuPara.runningInfo.dimming_for_lookup[LAMP1_INDEX]=0;
			glbRtuPara.runningInfo.status_now=0xfa;
			glbRtuPara.runningInfo.status_last=0xfa;
			glbRtuPara.runningInfo.actionBackUp.actionType=0X43;
			glbRtuPara.runningInfo.actionBackUp.light=0x01;
			glbRtuPara.runningInfo.actionBackUp.dimming=0X0;			
		}
		else if(solarYfData.loadStatus==1)//负载开灯
		{
			glbRtuPara.runningInfo.dimming_for_lookup[LAMP1_INDEX]=0x14;
			glbRtuPara.runningInfo.status_now=0xf5;
			glbRtuPara.runningInfo.status_last=0xf5;
			glbRtuPara.runningInfo.actionBackUp.actionType=0X42;
			glbRtuPara.runningInfo.actionBackUp.light=0x01;
			glbRtuPara.runningInfo.actionBackUp.dimming=0X14;			
		}
	}
	else
	{
		glbRtuPara.runningInfo.communication_err_cnt++;
		if(glbRtuPara.runningInfo.communication_err_cnt>=CHK_COMMUNICATION_ERR_UPPER)
		{
			printf("DevSolarYfInit:over %d err,think communication_err\n",glbRtuPara.runningInfo.communication_err_cnt);						
			glbRtuPara.runningInfo.communication_err_cnt=0;
			glbRtuPara.runningInfo.status_now=0xf0;
			glbRtuPara.runningInfo.status_last=0xf0;
			glbRtuPara.runningInfo.flag_communication_err=YES;
			glbRtuPara.runningInfo.flag_doCmd_retry_finished=YES;
		}
	}
	TaskSleepMs(1000);

}
		
xt_s32	DevSolarYfSend(xt_u8* pData, xt_s32 dataLen)
{
	xt_u8 i;
	xt_time_t now = 0;
	
	PrintfXTOS("--->\n");
	for(i=0;i<dataLen;i++)
		PrintfXTOS("%0.2x ",pData[i]);
	PrintfXTOS("\n");
	now = XtBspGetTime();

	//PrintfXTOS(".. packByIdAndSend time now=%d,ap485RecvTime=%d\n",now,ap485RecvTime);
	
	if(now > ap485RecvTime)
	{
		if(now - ap485RecvTime >= 60)//485 recv timeout of 60s
			ap485RecvTime = 0;
	}
	
	if(ap485RecvTime == 0) //if 485 recv is not busy then send out
		XtBspCtrlLv2_Write(pData, dataLen);
	return SUCCESS_XT;
}

xt_s32	DevSolarYfRecv(xt_u8* pData, xt_u32 maxLen)
{
	xt_u8 i,len,iBegin,chkSum=0;
	xt_u8 temp=0;
	xt_u32 dataLen=0;
	//xt_u8 *bufT=(xt_u8*)&solarGetYfTask;
	dataLen=XtBspCtrlLv2_Read(pData, maxLen);
	if(dataLen!=0)
	{
		rs485DataLen=dataLen;
		MemMemcpyXTOS(ap485MsgData.proAp.proApData, pData, dataLen);
	}	
	if(dataLen>0)
	{
		for(i=0;i<dataLen;i++)
		{
			PrintfXTOS("%0.2x ",pData[i]);
		}	
		for(i=0;i<dataLen;i++)
		{
			if(pData[i]==0x40)
			{
				len=pData[i+2];
				iBegin=i;
				break;
			}				
		}
		yfDataLen=len+4;	
		if((yfDataLen>dataLen)||(i==dataLen))
		{
			PrintfXTOS("get data err\n");
			return FAILURE_XT;
		}
		PrintfXTOS("bl:%d %d\n ",iBegin,len);
		for(i=0;i<yfDataLen-1;i++)
		{
			chkSum+=pData[iBegin+i];
		}
		PrintfXTOS("chk:%0.2x %0.2x\n ",chkSum,pData[iBegin+i]);
		if(chkSum==pData[iBegin+i])
		{
			/*
			if(yfDataLen>0x10)
			{
				printf("renew YFsolar data\n");
				for(i=3;i<yfDataLen-1;i++)
				{
					bufT[i-3]=pData[iBegin+i];
				}
			}*/
			PrintfXTOS("..*****chk ok: len=%d:*****\n..",yfDataLen);
			for(i=0;i<yfDataLen;i++)
			{
				temp=pData[iBegin+i];
				pData[i]=temp;
				PrintfXTOS("%0.2x ",pData[i]);
			}
			PrintfXTOS("\n..**********************************\n");		
		}
		else
			return FAILURE_XT;
		return SUCCESS_XT;
	}
	else
		return FAILURE_XT;
	
}

xt_void  DevSolarYfCtrl(xt_u8* src_pData,xt_u8* dec_pData)
{
	xt_u8 cmd=src_pData[1];

	switch(cmd)
	{
		case GET_TASK:
			{
				xt_u8 *bufT=(xt_u8*)&solarGetYfTask,i;
				glbRtuPara.runningInfo.is_getSolarOK=YES;
				for(i=3;i<yfDataLen-1;i++)
				{
					bufT[i-3]=src_pData[i];
				}				
			}
			break;
		case SET_TASK:
			{
				if(yfDataLen==5)
				{
					xt_u8 is_ok=src_pData[3];
					if(is_ok==1)
					{
						glbRtuPara.runningInfo.is_setSolarOK=YES;
						PrintfXTOS("  set OK:%\n");
					}
					else
					{
						glbRtuPara.runningInfo.is_setSolarOK=NO;
						PrintfXTOS("  set ERR:%\n");
					}			
				}				
			}
			break;
		case TOGGLE_SW:
			break;
		case GET_SOLAR:
			{
				xt_u8 *bufT=(xt_u8*)&solarYfData,i;
				glbRtuPara.runningInfo.is_getSolarOK=YES;
				PrintfXTOS("GET_SOLAR\n");
				for(i=3;i<yfDataLen-1;i++)
				{
					bufT[i-3+5]=src_pData[i];
				}				
			}
			break;
		case CTRL_UNITY:
			{
				if(yfDataLen==5)
				{
					xt_u8 is_ok=src_pData[3];
					if(is_ok==1)
					{
						glbRtuPara.runningInfo.is_setSolarOK=YES;
						PrintfXTOS("  set OK:%\n");
					}
					else
					{
						glbRtuPara.runningInfo.is_setSolarOK=NO;
						PrintfXTOS("  set ERR:%\n");
					}			
				}				
			}			
			break;
		default:
			break;
	}

}


xt_s32 DevSolarYfWriteAndRead(xt_u8 id_temp,xt_u8 cmd_temp, xt_u8 *sendBuf,xt_u8 sendLen)
{
	#ifdef YF_SOLAR
	xt_u8 wait_time_cnt;	
	//---set start control
	DevSolarYfSend(sendBuf,sendLen);	
	TaskSleepMs(800);
	//---chk recv
	glbRtuPara.runningInfo.is_setSolarOK=NO;
	glbRtuPara.runningInfo.is_getSolarOK=NO;	
	if(DevSolarYfRecv(recvTempBuf,sizeof(recvTempBuf))==SUCCESS_XT)//delay 20ms tiemout
	{
		DevSolarYfCtrl(recvTempBuf,dec_temp);
		glbRtuPara.runningInfo.flag_communication_err=NO;
		glbRtuPara.runningInfo.communication_err_cnt=0;
		glbRtuPara.errInfo.self_report_flag[LAMP1_INDEX].bits.communication_err=NO;
				
		//printf("yf success\n");
		return SUCCESS_XT;
	}	
	#endif
	//printf("yf failure\n");
	glbRtuPara.runningInfo.communication_err_cnt++;
	printf("c_err_cnt=%d\n",glbRtuPara.runningInfo.communication_err_cnt);
	if(glbRtuPara.runningInfo.communication_err_cnt>=CHK_COMMUNICATION_ERR_UPPER)
	{
		printf("over %d err,think communication_err\n",glbRtuPara.runningInfo.communication_err_cnt);				
		glbRtuPara.runningInfo.flag_communication_err=YES;
		glbRtuPara.runningInfo.communication_err_cnt=0;
	}	
	return FAILURE_XT;
}
#endif

