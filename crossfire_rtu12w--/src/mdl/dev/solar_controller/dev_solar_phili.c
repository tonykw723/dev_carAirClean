
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
#include "dev_solar_phili.h"
#ifdef PHILI_SOLAR
#define  READ_SOLAR_CTRL_TIMEOUT 60
extern phili_sloar_polling_t philiSloarPolling[];


xt_s32 DevSolarPhiliWriteAndRead(xt_u8 id_temp,xt_u8 cmd_temp,xt_u8 *set_value,xt_u8 set_len);
//----------extern para------------------
extern rtu_para_t glbRtuPara;
extern xt_s32 rs485DataLen ;
extern ap_msg_t ap485MsgData ;
extern comfirm_enum_t is_sloar_module_ini;
extern xt_time_t ap485RecvTime ;
extern phili_solar_dev_enum philiSolarDevID;
extern phili_solar_t philiSolar;
//--------------------------------------
//static xt_u8 *temp_buf=NULL;
sloarPhiliTask_t solarPhiliTask;
phili_solar_protocol_t phili_solar_protocol;
//solar_NML_t solar_data;
xt_u32 solarParaTemp[20];
xt_u16 phili_sloar_paraNameValue[ID_MAX]={
	ID_IDLE,
	MULTI_CMD,
	ON_OFF,
	DIM_LVL,
	DATE_TIME_SETTINGS ,
	CONTROL_START_STOP,
	CONTROL_HEARTBEAT_COUNTER,
	PREPARE_FOR_SOFTWARE_UPGRADE,
	TEST_MODE_START_STOP,

	MONITOR_PV_VOLTAGE,
	MONITOR_PV_CURRENT,
	MONITOR_BATT_VOLTAGE,
	MONITOR_BATT_CURRENT,
	MONITOR_BATTERY_TEMPERATURE,
	MONITOR_BATTERY_PRESENT_CAPACITY,
	MONITOR_BATTERY_TOTAL_CAPACITY,
	MONITOR_DAILY_CHARGE_CAPACITY,
	MONITOR_DAILY_DISCHARGE_CAPACITY,
	MONITOR_LOAD_VOLTAGE ,
	MONITOR_LOAD_CURRENT,
	MONITOR_CHARGE_CONTROLLER_TEMPERATURE,
	MONITOR_NIGHT_LENGTH,
	MONITOR_ENERGY_TRANSFER_PV_TO_BATT,
	MONITOR_BATTRY_STATUS ,
	MONITOR_LOAD_STATUS ,
	//------------power management para,PM:POWER_MANAGEMENT
	PM_BATTERY_TYPE ,
	PM_BATTERY_CAPACITY ,
	PM_LOAD_DISCONNECT_VOLTAGE ,
	PM_LOAD_RECONNECT_VOLTAGE ,
	PM_CABLE_VOLTAGE_DROP_COMPENSATION , 
	PM_LUMINAIRES_POWER_RATING ,
	PM_RF_MODULE_POWER_DURING_LVD ,
	PM_WAKEUP_TIME ,
	//------------light management para,LM:LIGHT_MANAGEMENT
	LM_HYSTERSIS_DELAY_TIME_DUSK ,
	LM_HYSTERSIS_DELAY_TIME_DAWN ,
	LM_DAWN_DETECTION_LEVEL ,
	LM_DUSK_DETECTION_LEVEL ,
	LM_WORKING_MODE ,
	LM_DIMMING_MODE ,
	LM_RTE_ENABLE_DISABLE ,
	LM_RTE_TRIGGER_VOLTAGE ,
	LM_RTE_DARK_DAYS ,
	LM_RTE_TRIGGER_BATTRY_CAPACITY,
	LM_DYNADIM_HISTORY_MINIMUM_DURATION_OF_NIGHT ,
	LM_DYNADIM_HISTORY_MAXIMUM_DURATION_OF_NIGHT ,
	LM_DYNADIM_HISTORY_MAXIMUM_NIGHT_DAVIATION ,
	LM_SCENE ,
	LM_DUSKDAWN_EVENT,	
	//-------------new solar controller----------------
	SLAVE_MCHINE_ID,
	HCU_LOAD_TO_AC_OR_BATT,
	MONITOR_HCU_PV_PARA,
	MONITOR_HCU_BATT_PARA,
	MONITOR_HCU_DISCHARGE_MODE,
	MONITOR_HCU_STATUS,

	MONITOR_EPS_PARA,
	MONITOR_EPS_ALARM,

	MONITOR_DESS_BATT_PARA,
	MONITOR_DESS_PCS_PARA,
	MONITOR_DESS_STATUS,
	MONITOR_DESS_ALARM,

	MONITOR_SCS_BATT_PARA,
	MONITOR_SCS_PARA,
	MONITOR_SCS_STATUS,	
};




//======================================================


unsigned char calculateCRC8(unsigned char *buffer, unsigned int length)
{

    unsigned int sum = 0,i;
    for (i = 0; i < length; i++)
    {
        sum += (unsigned int)buffer[i];
    }

    sum = sum % (1<<8);
    sum = (~sum + 1) & 0xff;
    return sum;
}

xt_u32 acii2Hex(unsigned char *pt_in,unsigned char len)
{
	xt_u32 result=0,temp;
	xt_u8 i;
	for(i=0;i<len;i++)
	{
		temp=0;
		if((pt_in[i]>='0')&&(pt_in[i]<='9'))
		{
			temp|=(pt_in[i]-'0');
			temp<<=(4*(len-i-1));
			result|=temp;
		}
		else if((pt_in[i]>='A')&&(pt_in[i]<='F'))
		{
			temp|=(pt_in[i]-'A'+0x0a);
			temp<<=(4*(len-i-1));
			result|=temp;		
		}
		else if((pt_in[i]>='a')&&(pt_in[i]<='f'))
		{
			temp|=(pt_in[i]-'a'+0x0a);
			temp<<=(4*(len-i-1));
			result|=temp;
		}
		else
		{
			result=0;
			break;
		}
		//PrintfXTOS("..result=%x ",result);
	}
	
	return result;
}

char halfByte2Ascii(xt_u8 halfByteTemp,xt_u8 is_zero_ascii_retain)
{
	xt_u8 ascTemp=0;
	if((halfByteTemp==0)&&(is_zero_ascii_retain==NO))
	{
		ascTemp=0;
	}
	else
	{
		if(halfByteTemp>=0x0a)
			ascTemp=halfByteTemp-0x0a+'A';
		else
			ascTemp=halfByteTemp+'0';	
	}
	return(ascTemp);
}

void pack_Hex2Ascii(xt_u32 value,xt_u8 *index,xt_u8 *pt,xt_u8 is_zero_ascii_retain,xt_u8 valueSize)
{
	xt_u8 halfByteAscii=0;
	xt_s8 i;
	for(i=valueSize*2;i>0;i--)
	{
		halfByteAscii=halfByte2Ascii(value>>((i-1)*4)&0x0f,is_zero_ascii_retain);
		if(halfByteAscii)
		{
			pt[(*index)++]=halfByteAscii;
			is_zero_ascii_retain=YES;
		}	
	}
}

void pack_16bitHex2ascii(xt_u16 value,xt_u8 *index,xt_u8 *pt,xt_u8 is_zero_ascii_retain)
{
	xt_u8 halfByteAscii=0;
	halfByteAscii=halfByte2Ascii((value&0x0f00)>>8,is_zero_ascii_retain);
	if(halfByteAscii)
	{
		pt[(*index)++]=halfByteAscii;
		is_zero_ascii_retain=YES;
	}
	halfByteAscii=halfByte2Ascii((value&0x00f0)>>4,is_zero_ascii_retain);
	if(halfByteAscii)
	{
		pt[(*index)++]=halfByteAscii;
		is_zero_ascii_retain=YES;
	}
	halfByteAscii=halfByte2Ascii(value&0x000f,is_zero_ascii_retain);
	if(halfByteAscii)
	{
		pt[(*index)++]=halfByteAscii;
	}
}

void pack_8bitHex2ascii(xt_u8 value,xt_u8 *index,xt_u8 *pt,xt_u8 is_zero_ascii_retain)
{
	xt_u8 halfByteAscii=0;
	halfByteAscii=halfByte2Ascii((value&0xf0)>>4,is_zero_ascii_retain);
	if(halfByteAscii)
	{
		pt[(*index)++]=halfByteAscii;
		is_zero_ascii_retain=YES;
	}	
	halfByteAscii=halfByte2Ascii((value&0x0f),is_zero_ascii_retain);
	if(halfByteAscii)
	{
		pt[(*index)++]=halfByteAscii;
	}	
}

void packMulitPara(xt_u32 *val,xt_u8 *pt,xt_u8 size)
{
	xt_u8 i,j=0;
	for(i=0;i<size;i++)
	{
		pack_Hex2Ascii(val[i],&j,pt,YES,sizeof(xt_u32));
		if(i<size-1)
			pt[j++]=';';
		else
			pt[j++]=0;
	}
	
}

xt_u8  paraName2AsciiStyle(xt_u16 paraNameT,xt_u8 *dataBuf)
{
	xt_u8 i=0;
	#if 0
	if(paraNameT<256)
	{
		dataBuf[i++]='0';
		dataBuf[i++]='0';
		pack_8bitHex2ascii((xt_u8)paraNameT,&i,dataBuf,YES);
		dataBuf[i++]=':';
	}
	else
	{
		dataBuf[i++]='0';
		pack_16bitHex2ascii(paraNameT,&i,dataBuf,YES);	
		dataBuf[i++]=':';
	}
	#else

	pack_Hex2Ascii(paraNameT,&i,dataBuf,YES,sizeof(xt_u16));	
	dataBuf[i++]=':';
	#endif

	return i;
}


static xt_u8 packSolarData(xt_u8 *buf ,xt_u16 tranID,xt_u16 cmd,xt_u8 *dataBuf,xt_u8 data_len)
{
	xt_u8 len_temp=0,crc8_temp=0,i=0,j;

	buf[i++]='[';
	buf[i++]='A';//vertion...
	buf[i++]=',';
	buf[i++]='2';//source:CC = '1', RF= '2', PC='3', ATE='4', ND="255"
	buf[i++]=',';
	pack_Hex2Ascii(tranID,&i,buf,NO,sizeof(xt_u16));
	//pack_16bitHex2ascii(tranID,&i,buf,NO);
	buf[i++]=',';
	pack_Hex2Ascii(cmd,&i,buf,NO,sizeof(xt_u16));
	//pack_16bitHex2ascii(cmd,&i,buf,NO);
	buf[i++]=',';
	for(j=0;j<data_len;j++)
	{
		buf[i++]=dataBuf[j];
	}
	buf[i++]='#';
	crc8_temp=calculateCRC8(&buf[1], i-1);
	pack_Hex2Ascii(crc8_temp,&i,buf,YES,sizeof(xt_u8));
	//pack_8bitHex2ascii(crc8_temp,&i,buf,YES);
	buf[i++]=']';
	len_temp=i;
	return len_temp;
}


xt_void packByIdAndSend(xt_u8 id_temp,xt_u8 *temp_buf,xt_u8 cmd_temp,xt_u8 *valBuf,xt_u8 valLen)
{
	xt_time_t now = 0;
	
	xt_u8 data_buf[150],data_len=0,protocol_len=0,i;
	//PrintfXTOS("..tranID=%x,paraName=%x,cmd=%x\n",id_temp,phili_sloar_paraNameValue[id_temp],cmd_temp);
	//if(cmd_temp==GET_PARA)
	{
		data_len=paraName2AsciiStyle(phili_sloar_paraNameValue[id_temp],data_buf);
	}
	if(((cmd_temp==SET_PARA)||(cmd_temp==GET_MULTI_PARA))&&(valBuf!=NULL))
	{
		//设置命令，在paraName : 后插入设置值
		for(i=0;i<valLen;i++)
		{
			data_buf[data_len+i]=*(valBuf+i);			
		}
		data_len=data_len+i;
	}

	protocol_len=packSolarData(temp_buf,id_temp,cmd_temp,data_buf,data_len);

	now = XtBspGetTime();

	//PrintfXTOS(".. packByIdAndSend time now=%d,ap485RecvTime=%d\n",now,ap485RecvTime);
	
	if(now > ap485RecvTime)
	{
		if(now - ap485RecvTime >= 60)//485 recv timeout of 60s
			ap485RecvTime = 0;
	}
	
	if(ap485RecvTime == 0) //if 485 recv is not busy then send out
		DevSolarPhiliSend(temp_buf,protocol_len);	
	
}

static xt_u8 getSolarPara(xt_u8 *pData,xt_u32 *solarParaTemp,pro_enum proMode)
{
	xt_u8 idxStart=0,i,j,len,solarParaCnt=0;
	xt_u8 temp[50]={0};
	//-----------
	//nml: ...xx;xx;#xx]
	//hcu: ...xx;xx#xx]
	for(i=0;i<150;i++)
	{
		if((pData[i]==';')||(pData[i]=='#'))
		{
			if((pData[i]=='#')&&(proMode==PRO_NML))
			{
				//PrintfXTOS("break");
				break;
			}		
			len=i-idxStart;
			MemMemsetXTOS(temp, 0, 8);
			//PrintfXTOS("data:\n");
			for(j=0;j<len;j++)
			{
				temp[j]=pData[idxStart+j];
				//PrintfXTOS("%c ",temp[j]);
			}
			solarParaTemp[solarParaCnt]=acii2Hex(temp,len);
			PrintfXTOS("...%d %dth=0x%x %c\n",idxStart,solarParaCnt,solarParaTemp[solarParaCnt],pData[i]);
			idxStart=(i+1);
			solarParaCnt++;
			if((pData[i]=='#')&&(proMode==PRO_HCU))
			{
				//PrintfXTOS("break");
				break;
			}
		}
	}
	PrintfXTOS("solarParaCnt=%d\n",solarParaCnt);
	return solarParaCnt;
}

static xt_u8 getSolarPara2(xt_u8 *pData,xt_u32 *solarParaTemp)
{
	xt_u8 idxStart=0,i,j,len,solarParaCnt=0;
	xt_u8 temp[8]={0};
	for(i=0;i<80;i++)
	{
		if((pData[i]==';')||(pData[i]=='#'))
		{
			if(pData[i]=='#')
			{
				//PrintfXTOS("break");
				break;
			}		
			len=i-idxStart;
			MemMemsetXTOS(temp, 0, 8);
			//PrintfXTOS("data:\n");
			for(j=0;j<len;j++)
			{
				temp[j]=pData[idxStart+j];
				//PrintfXTOS("%c ",temp[j]);
			}
			solarParaTemp[solarParaCnt]=acii2Hex(temp,len);
			PrintfXTOS("...%d %dth=0x%x %c\n",idxStart,solarParaCnt,solarParaTemp[solarParaCnt],pData[i]);
			idxStart=(i+1);
			solarParaCnt++;

		}
	}
	PrintfXTOS("solarParaCnt=%d\n",solarParaCnt);
	return solarParaCnt;
}


xt_void  DevSolarPhiliInit(void)
{
	/*
	{
		xt_u8 temp_buf[60];
		xt_u8 set_value='1';//'1'---START,'0'---STOP
		if(glbRtuPara.devPara.flag_dev_ini==NO)
			return ;			
		if(is_sloar_module_ini==NO)
		{
			packByIdAndSend(ID_CONTROL_START_STOP,temp_buf,GET_PARA,NULL,0);
			TaskSleepMs(1000);
		}
	}*/
	if(is_sloar_module_ini==NO)
	{
		//if(temp_buf==NULL)	
			//temp_buf=MemMallocXTOS(200);
		DevSolarPhiliWriteAndRead(ID_SLAVE_MCHINE_ID,GET_PARA,NULL,0);
		TaskSleepMs(100);
		DevSolarPhiliWriteAndRead(ID_SLAVE_MCHINE_ID,GET_PARA,NULL,0);
		TaskSleepMs(100);
		DevSolarPhiliWriteAndRead(ID_SLAVE_MCHINE_ID,GET_PARA,NULL,0);
		is_sloar_module_ini=YES;
	}

}
		
xt_s32	DevSolarPhiliSend(xt_u8* pData, xt_s32 dataLen)
{
	xt_u8 i;
	PrintfXTOS("--->\n");
	for(i=0;i<dataLen;i++)
		PrintfXTOS("%c",pData[i]);	
	PrintfXTOS("\n");
	XtBspCtrlLv2_Write(pData, dataLen);
	return SUCCESS_XT;
}

xt_s32	DevSolarPhiliRecv(xt_u8* pData, xt_u32 maxLen)
{

	xt_u8 i,j,f_start=0,len=0,chk_crc8,dot_cnt=0,start_index;
	xt_u8 temp[3]={0},temp_len=0;
	//xt_u8 sloar_temp[80]={0};
	xt_u8*sloar_temp=NULL;//MemMallocXTOS(60);
	xt_u32 dataLen=0;
	/*
	if(sloar_temp==NULL)
	{
		PrintfXTOS("Malloc temp buff err\n");
		return FAILURE_XT;
	}*/
	dataLen=XtBspCtrlLv2_Read(pData, maxLen);
	if(dataLen!=0)
	{
		rs485DataLen=dataLen;
		MemMemcpyXTOS(ap485MsgData.proAp.proApData, pData, dataLen);
	}
	if(dataLen<12)//至少要12个字节
	{
		return FAILURE_XT;
	}
	PrintfXTOS("..*****recv original solar data: len=%d:*****\n..",dataLen);
	for(i=0;i<dataLen;i++)
	PrintfXTOS("%c",pData[i]);
	PrintfXTOS("\n..**********************************\n");	
	//----------按包头'[' 和包尾']' 取出数据-------------------
	for(i=0;i<dataLen;i++)
	{	
		if(pData[i]=='[')
		{
			j=0;
			f_start=1;
		}
		if(f_start)
		{
			
			//sloar_temp[j]=pData[i];
			temp[0]=pData[i];
			pData[j]=temp[0];
			j++;		
			if((pData[i]==']'))
			{
				len=j;
				break;
			}			
		}
	}	
	if(len<12)
	{
		PrintfXTOS("..len=%d<12,discard\n",len);
		return FAILURE_XT;
	}
	sloar_temp=pData;
	//----------------判断CRC-----------------
	//从第二个字节，到#，计算CRC。
	//eg:[A,2,2,A,0064:#43][A,1,2,32A,0;#78]
       //都是从A到#计算CRC
       //第一个计算CRC等于0x43，用ascii表示
       //第二个计算CRC等于0x78，用ascii表示
    #if 1
	chk_crc8=calculateCRC8(&sloar_temp[1],len-4);
	if(chk_crc8!=(xt_u8)acii2Hex(&sloar_temp[len-3],2))
	{
		PrintfXTOS("..crc8 err:%0.2x vs %0.2x\n",chk_crc8,acii2Hex(&sloar_temp[len-3],2));
		return FAILURE_XT;
	}
	#endif
	//------按字符打印正确指令---------
	PrintfXTOS("recv parse success\n");
	//PrintfXTOS("***after chk solar data :len=%d:\n",len);
	//for(i=0;i<len;i++)
	//	PrintfXTOS("%c",sloar_temp[i]);
	//PrintfXTOS("\n");		
	//-------解析出正确的指令-----------
	
	for(i=0;i<len;i++)
	{
		if((sloar_temp[i]==',')||(sloar_temp[i]=='#'))
		{
			switch(dot_cnt)
			{
				case 0:
					if(sloar_temp[i]!=',')
						return FAILURE_XT;
					if(i>0)
						phili_solar_protocol.vertion=sloar_temp[i-1];
					//PrintfXTOS("..vertion=%x\n",phili_solar_protocol.vertion);
					start_index=i+1;
					dot_cnt=1;
					break;
				case 1:
					if(sloar_temp[i]!=',')
						return FAILURE_XT;			
					temp_len=i-start_index;
					for(j=0;j<temp_len;j++)
					{
						temp[j]=sloar_temp[j+start_index];
					}
					phili_solar_protocol.source=(xt_u16)acii2Hex(temp,temp_len);
					//PrintfXTOS("..source_len=%d,source=%x\n",temp_len,phili_solar_protocol.source);
					start_index=i+1;
					dot_cnt=2;									
					break;
				case 2:
					if(sloar_temp[i]!=',')
						return FAILURE_XT;			
					temp_len=i-start_index;
					for(j=0;j<temp_len;j++)
					{
						temp[j]=sloar_temp[j+start_index];
					}
					phili_solar_protocol.tranID=(xt_u16)acii2Hex(temp,temp_len);
					//PrintfXTOS("..tranID_len=%d,tranID=%x\n",temp_len,phili_solar_protocol.tranID);
					start_index=i+1;
					dot_cnt=3;									
					break;
				case 3:
					if(sloar_temp[i]!=',')
						return FAILURE_XT;			
					temp_len=i-start_index;
					for(j=0;j<temp_len;j++)
					{
						temp[j]=sloar_temp[j+start_index];
					}
					phili_solar_protocol.cmd=(xt_u16)acii2Hex(temp,temp_len);
					//PrintfXTOS("..cmd_len=%d,cmd=%x\n",temp_len,phili_solar_protocol.cmd);
					start_index=i+1;
					dot_cnt=4;									
					break;	
				case 4:
					if(sloar_temp[i]!='#')
						return FAILURE_XT;	
					temp_len=i-start_index;
					phili_solar_protocol.dataLen=temp_len;
					//PrintfXTOS("..dataBuf_len=%d,dataBuf:",temp_len);
					for(j=0;j<temp_len;j++)
					{
						phili_solar_protocol.dataBuf[j]=sloar_temp[j+start_index];
						//PrintfXTOS("%0.2x ",phili_solar_protocol.dataBuf[j]);
					}
					phili_solar_protocol.dataBuf[j]='#';
					//PrintfXTOS("\n");					
					start_index=i+1;
					dot_cnt=4;					
				default:
					break;
			}
		}
	}
	return SUCCESS_XT;
}

xt_void  DevSolarPhiliCtrl(xt_u8* src_pData,xt_u8* dec_pData)
{
	//xt_u8 i;
	phili_solar_t *philiSolar=(phili_solar_t*)dec_pData;
	PrintfXTOS("..tranID=%d",phili_solar_protocol.tranID);
	if((phili_solar_protocol.tranID==ID_IDLE)||(phili_solar_protocol.tranID>=ID_MAX))
		return;
	if(phili_solar_protocol.cmd==REPLY_SET_PARA)//设置
	{
		if((phili_solar_protocol.dataBuf[0]=='O')&&(phili_solar_protocol.dataBuf[1]=='K'))
		{
			glbRtuPara.runningInfo.is_setSolarOK=YES;
			PrintfXTOS("  setOk\n");	
		}
		else
		{
			glbRtuPara.runningInfo.is_setSolarOK=NO;
			PrintfXTOS("  setErr\n");					
		}		
		return;
	}	
	
	switch(phili_solar_protocol.tranID)
	{
		case ID_MULTI_CMD:
		{
			xt_u8 i=0,j=0;
			if(philiSolarDevID==SD_NORMAL_ID)
			{
				if(getSolarPara(phili_solar_protocol.dataBuf,solarParaTemp,PRO_NML)==philiSloarPolling[philiSolarDevID].para[0].valSize)
				{
					philiSolar->all.pvVoltage=solarParaTemp[i++]/10;//to 10mV
					philiSolar->all.pvCurrent=solarParaTemp[i++]/10;//to 10mA
					philiSolar->all.batteryVoltage=solarParaTemp[i++]/10;//to 10mV
					philiSolar->all.batteryDischargeCurrent=solarParaTemp[i++]/10;//to 10mA
					philiSolar->all.batteryTemp=solarParaTemp[i++];
					philiSolar->all.loadVoltage=solarParaTemp[i++]/10;//to 10mV
					philiSolar->all.loadCurrent=solarParaTemp[i++]/10;//to 10mA
					philiSolar->all.controllerTemp=solarParaTemp[i++];
					philiSolar->all.batteryChargeStatus=solarParaTemp[i++];
					philiSolar->all.batteryCapacityStatas=solarParaTemp[i++];
					philiSolar->all.loadStatus=solarParaTemp[i++];	
					//philiSolar->normal.dim_v=solarParaTemp[i++];	
					PrintfXTOS("MULTI_CMD NML recv 1th:\n");
					PrintfXTOS("..pvVoltage=%x\n",philiSolar->all.pvVoltage);
					PrintfXTOS("..pvCurrent=%x\n",philiSolar->all.pvCurrent);
					PrintfXTOS("..controllerTemp=%x...\n",philiSolar->all.batteryTemp);
				}
				else if(getSolarPara(phili_solar_protocol.dataBuf,solarParaTemp,PRO_NML)==philiSloarPolling[philiSolarDevID].para[1].valSize)
				{

					philiSolar->all.batteryPresentCapacity=solarParaTemp[i++]/10;//to 10mAh
					philiSolar->all.batteryStaticCapacity=solarParaTemp[i++]/10;//to 10mAh
					philiSolar->all.batteryDailyChargedCapacity=solarParaTemp[i++]/10;//10mAh
					philiSolar->all.batteryDailyDischargedCapacity=solarParaTemp[i++]/10;//10mAh
					//philiSolar->all.timeStamp=solarParaTemp[i++];
					PrintfXTOS("MULTI_CMD NML recv 2th:\n");
					PrintfXTOS("..battery_present_capacity=%x\n",philiSolar->all.batteryPresentCapacity);
					PrintfXTOS("..battery_static_capacity=%x\n",philiSolar->all.batteryStaticCapacity);
					PrintfXTOS("..daily_charged_enery=%x\n",philiSolar->all.batteryDailyChargedCapacity);
					glbRtuPara.runningInfo.flag_polling_sloar_end=YES;
				}
			}
			break;
		}			
		//-------------100 CMD-------------------------
		case ID_ON_OFF:
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				philiSolar->all.loadStatus=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);
				//PrintfXTOS("  sw_status=%x\n",philiSolar->normal.sw_status);			
			}
	
			break;
		case ID_DIM_LVL:
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
			}			
			break;
		case ID_DATE_TIME_SETTINGS:
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				philiSolar->all.timeStamp=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);
				//PrintfXTOS("  time_stamp=%x\n",philiSolar->normal.time_stamp);			
			}
			break;
		case ID_CONTROL_START_STOP:
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				glbRtuPara.runningInfo.solarCtrlMode=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);
			}		
			break;
		case ID_CONTROL_HEARTBEAT_COUNTER:
			break;
		case ID_PREPARE_FOR_SOFTWARE_UPGRADE:
			break;
		case ID_TEST_MODE_START_STOP:
			break;

		//--------------200 CMD MONITOR-----------------	
		case ID_MONITOR_PV_VOLTAGE:
			//for(i=0;i<phili_solar_protocol.dataLen;i++)
			//	PrintfXTOS("%0.2x",phili_solar_protocol.dataBuf[i]);
			philiSolar->all.pvVoltage=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;
			//PrintfXTOS("  pv_v=%x\n",philiSolar->normal.pv_v);
			break;
		case ID_MONITOR_PV_CURRENT:
			philiSolar->all.pvCurrent=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;
			//PrintfXTOS("  pv_i=%x\n",philiSolar->normal.pv_i);
			break;
		case ID_MONITOR_BATT_VOLTAGE:
			philiSolar->all.batteryVoltage=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;
			//PrintfXTOS("  battery_v=%x\n",philiSolar->normal.battery_v);
			break;
		case ID_MONITOR_BATT_CURRENT:
			philiSolar->all.batteryDischargeCurrent=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;
			//PrintfXTOS("  battery_i=%x\n",philiSolar->normal.battery_i);
			break;
		case ID_MONITOR_BATTERY_TEMPERATURE:
			philiSolar->all.batteryTemp=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);
			//PrintfXTOS("  battery_tp=%x\n",philiSolar->normal.battery_tp);			
			break;
		case ID_MONITOR_BATTERY_PRESENT_CAPACITY:
			philiSolar->all.batteryPresentCapacity=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;
			//PrintfXTOS("  battery_present_capacity=%x\n",philiSolar->normal.battery_present_capacity);			
			break;
		case ID_MONITOR_BATTERY_TOTAL_CAPACITY:
			philiSolar->all.batteryStaticCapacity=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;
			if(philiSolar->all.batteryStaticCapacity!=0)
				philiSolar->all.batterySoc=philiSolar->all.batteryPresentCapacity*100/philiSolar->all.batteryStaticCapacity;
			//PrintfXTOS("  battery_static_capacity=%x",philiSolar->normal.battery_static_capacity);	
			//PrintfXTOS("  battery_ratio=%x\n",philiSolar->normal.battery_ratio);	
			break;
		case ID_MONITOR_DAILY_CHARGE_CAPACITY:
			philiSolar->all.batteryDailyChargedCapacity=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;//10mAh
			//PrintfXTOS("  daily_charged_enery=%x\n",philiSolar->normal.daily_charged_enery);						
			break;
		case ID_MONITOR_DAILY_DISCHARGE_CAPACITY:
			philiSolar->all.batteryDailyDischargedCapacity=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;//10mAh
			//PrintfXTOS("  daily_discharged_enery=%x\n",philiSolar->normal.daily_discharged_enery);									
			break;
		case ID_MONITOR_LOAD_VOLTAGE :
			philiSolar->all.loadVoltage=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;
			//PrintfXTOS("  load_v=%x\n",philiSolar->normal.load_v);												
			break;
		case ID_MONITOR_LOAD_CURRENT:
			philiSolar->all.loadCurrent=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;
			//PrintfXTOS("  load_i=%x\n",philiSolar->normal.load_i);															
			break;
		case ID_MONITOR_CHARGE_CONTROLLER_TEMPERATURE:
			philiSolar->all.controllerTemp=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);
			//PrintfXTOS("  controller_tp=%x\n",philiSolar->normal.controller_tp);															
			break;
		case ID_MONITOR_NIGHT_LENGTH:
			philiSolar->all.lightLength=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)>>8;//hhmm
			//PrintfXTOS("  light_length=%x\n",philiSolar->normal.light_length);				
			break;
		case ID_MONITOR_ENERGY_TRANSFER_PV_TO_BATT:
			philiSolar->all.batteryChargeStatus=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);
			//PrintfXTOS("  battery_charge_status=%x\n",philiSolar->normal.battery_charge_status);			
			break;
		case ID_MONITOR_BATTRY_STATUS:
			philiSolar->all.batteryCapacityStatas=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);
			//PrintfXTOS("  battery_capacity_statas=%x\n",philiSolar->normal.battery_capacity_statas);			
			break;
		case ID_MONITOR_LOAD_STATUS :
			philiSolar->all.loadStatus=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);
			//PrintfXTOS("  load_status=%x\n",philiSolar->normal.load_status);			
			if(philiSolar->normal.load_status==0)
				glbRtuPara.runningInfo.flag_lamp_lighting[LAMP1_INDEX]=LAMP_DRIGHT;
			else if(philiSolar->normal.load_status==1)
				glbRtuPara.runningInfo.flag_lamp_lighting[LAMP1_INDEX]=LAMP_DARK;
			if(glbRtuPara.runningInfo.flag_polling_sloar_start)
			{
				glbRtuPara.runningInfo.flag_polling_sloar_end=YES;
				PrintfXTOS("  polling end\n");
			}
			break;
	//------------power management para,PM:POWER_MANAGEMENT
		case ID_PM_BATTERY_TYPE :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.batteryType=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);
				//PrintfXTOS("  batteryType=%x\n",solarPhiliTask.batteryType);	
			}
			break;
		case ID_PM_BATTERY_CAPACITY :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.batteryCapacity=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;//to 10mAh
				//PrintfXTOS("  batteryCapacity=%x\n",solarPhiliTask.batteryCapacity);	
			}
			break;
		case ID_PM_LOAD_DISCONNECT_VOLTAGE :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.loadDisconnectVolage=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;//to 10mV
				//PrintfXTOS("  loadDisconnectVolage=%x\n",solarPhiliTask.loadDisconnectVolage);	
			}
			break;
		case ID_PM_LOAD_RECONNECT_VOLTAGE :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.loadReconnectVolage=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;//to 10mV
				//PrintfXTOS("  loadReconnectVolage=%x\n",solarPhiliTask.loadReconnectVolage);	
			}
			break;
		case ID_PM_CABLE_VOLTAGE_DROP_COMPENSATION: 
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.cableVoltageDropCompensation=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//mOhm
				//PrintfXTOS("  cableVoltageDropCompensation=%x\n",solarPhiliTask.cableVoltageDropCompensation);
			}
			break;
		case ID_PM_LUMINAIRES_POWER_RATING :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.luminairesPowerRating=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//w
				//PrintfXTOS("  luminairesPowerRating=%x\n",solarPhiliTask.luminairesPowerRating);	
			}
			break;
		case ID_PM_RF_MODULE_POWER_DURING_LVD :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.rfModulePowerDuringLVD=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//enum
				//PrintfXTOS("  rfModulePowerDuringLVD=%x\n",solarPhiliTask.rfModulePowerDuringLVD);	
			}
			break;
		case ID_PM_WAKEUP_TIME :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.wakeupTime=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//sec
				//PrintfXTOS("  wakeupTime=%x\n",solarPhiliTask.wakeupTime);	
			}
			break;
	//------------light management para,LM:LIGHT_MANAGEMENT
		case ID_LM_HYSTERSIS_DELAY_TIME_DUSK :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.hystersisDelayTimeDusk=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//sec
				//PrintfXTOS("  hystersisDelayTimeDusk=%x\n",solarPhiliTask.hystersisDelayTimeDusk);	
			}
			break;
		case ID_LM_HYSTERSIS_DELAY_TIME_DAWN :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.hystersisDelayTimeDawn=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//sec
				//PrintfXTOS("  hystersisDelayTimeDawn=%x\n",solarPhiliTask.hystersisDelayTimeDawn);	
			}
			break;
		case ID_LM_DAWN_DETECTION_LEVEL :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.dawnDetectionLevel=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;//to 10mV
				//PrintfXTOS("  dawnDetectionLevel=%x\n",solarPhiliTask.dawnDetectionLevel);	
			}
			break;
		case ID_LM_DUSK_DETECTION_LEVEL :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.duskDetectionLevel=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;//to 10mV
				//PrintfXTOS("  duskDetectionLevel=%x\n",solarPhiliTask.duskDetectionLevel);	
			}
			break;
		case ID_LM_WORKING_MODE :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.workingMode=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//enum
				//PrintfXTOS("  workingMode=%x\n",solarPhiliTask.workingMode);	
			}
			break;
		case ID_LM_DIMMING_MODE :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.dimmingMode=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//enum
				//PrintfXTOS("  dimmingMode=%x\n",solarPhiliTask.dimmingMode);
			}
			break;
		case ID_LM_RTE_ENABLE_DISABLE :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.rteEnableDisable=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//enum
				//PrintfXTOS("  rteEnableDisable=%x\n",solarPhiliTask.rteEnableDisable);	
			}
			break;
		case ID_LM_RTE_TRIGGER_VOLTAGE :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.rteTriggerVoltage=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1)/10;//to 10mV
				//PrintfXTOS("  rteTriggerVoltage=%x\n",solarPhiliTask.rteTriggerVoltage);	
			}
			break;
		case ID_LM_RTE_DARK_DAYS :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.rteDarkDays=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//days
				//PrintfXTOS("  rteDarkDays=%x\n",solarPhiliTask.rteDarkDays);	
			}
			break;
		case ID_LM_RTE_TRIGGER_BATTRY_CAPACITY:
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.rteTriggerBatteryCapacity=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//%
				//PrintfXTOS("  rteTriggerBatteryCapacity=%x\n",solarPhiliTask.rteTriggerBatteryCapacity);	
			}
			break;
		case ID_LM_DYNADIM_HISTORY_MINIMUM_DURATION_OF_NIGHT :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.dynaDimhistoryMinDurationOfNigh=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//min
				//PrintfXTOS("  dynaDimhistoryMinDurationOfNigh=%x\n",solarPhiliTask.dynaDimhistoryMinDurationOfNigh);	
			}
			break;
		case ID_LM_DYNADIM_HISTORY_MAXIMUM_DURATION_OF_NIGHT :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.dynaDimhistoryMaxDurationOfNigh=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//min
				//PrintfXTOS("  dynaDimhistoryMaxDurationOfNigh=%x\n",solarPhiliTask.dynaDimhistoryMaxDurationOfNigh);	
			}
			break;
		case ID_LM_DYNADIM_HISTORY_MAXIMUM_NIGHT_DAVIATION :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				solarPhiliTask.dynaDimhistoryMaxNighDaviation=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen-1);//min
				//PrintfXTOS("  dynaDimhistoryMaxNighDaviation=%x\n",solarPhiliTask.dynaDimhistoryMaxNighDaviation);
			}
			break;
		case ID_LM_SCENE :
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				xt_u8 i=0;
				if(getSolarPara(phili_solar_protocol.dataBuf,solarParaTemp,PRO_NML)==6)
				{
					for(i=0;i<6;i++)
					{
						solarPhiliTask.scene[i]=solarParaTemp[i];
						PrintfXTOS("..scene[%d]=%x\n",i,solarPhiliTask.scene[i]);
					}				
				}	
			}
			break;
			
		case ID_LM_DUSKDAWN_EVENT:
			if(phili_solar_protocol.cmd==REPLY_GET_PARA)//查询
			{
				xt_u8 i=0;
				if(getSolarPara(phili_solar_protocol.dataBuf,solarParaTemp,PRO_NML)==12)
				{
					for(i=0;i<12;i++)
					{
						solarPhiliTask.evenDuskDawn[i]=solarParaTemp[i];
						PrintfXTOS("..evenDuskDawn[%d]=%x\n",i,solarPhiliTask.evenDuskDawn[i]);
					}				
				}	
			}
			break;
		//---------------new solar controller-----------------------------------------
		case ID_SLAVE_MCHINE_ID:
			{
				xt_u32 devT=acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen);
				PrintfXTOS("  devT=%x\n",devT);
				philiSolarDevID=SD_NORMAL_ID;

				if(SD_HCU_VALUE==devT)
				{
					philiSolarDevID=SD_HCU_ID;
					PrintfXTOS("  SD_HCU_ID\n");
				}

				else if(SD_EPS_VALUE==devT)
				{
					philiSolarDevID=SD_EPS_ID;
					PrintfXTOS("  SD_EPS_ID\n");
				}		
				else if(SD_DESS_VALUE==devT)
				{
					philiSolarDevID=SD_DESS_ID;
					PrintfXTOS("  SD_DESS_ID\n");
				}	
				else if(SD_SCS_VALUE==devT)
				{
					philiSolarDevID=S_SCS_ID;
					PrintfXTOS("  S_SCS_ID\n");
				}

				else
					PrintfXTOS("  SD_NORMAL_ID\n");	
			}
			break;
		//--------------HCU
		case ID_HCU_LOAD_TO_AC_OR_BATT:
			if(phili_solar_protocol.cmd==REPLY_SET_PARA)//设置
			{
				if((phili_solar_protocol.dataBuf[0]=='O')&&(phili_solar_protocol.dataBuf[1]=='K'))
				{
					glbRtuPara.runningInfo.is_setSolarOK=YES;
					PrintfXTOS("  setOk\n");	
				}
				else
				{
					glbRtuPara.runningInfo.is_setSolarOK=NO;
					PrintfXTOS("  setErr\n");					
				}			
			}
			break;
		case ID_MONITOR_HCU_PV_PARA:
		{
			xt_u8 i=0;
			if(getSolarPara(phili_solar_protocol.dataBuf,solarParaTemp,PRO_HCU)==3)
			{
				philiSolar->all.pvVoltage=solarParaTemp[i++]/10;//to 10mV
				philiSolar->all.pvCurrent=solarParaTemp[i++]/10;//to 10mA
				philiSolar->all.controllerTemp=(xt_u8)solarParaTemp[i++];
				PrintfXTOS("..pvVoltage=%x\n",philiSolar->all.pvVoltage);
				PrintfXTOS("..pvCurrent=%x\n",philiSolar->all.pvCurrent);
				PrintfXTOS("..controllerTemp=%x\n",philiSolar->all.controllerTemp);
				
			}
			break;
		}
		case ID_MONITOR_HCU_BATT_PARA:
		{
			xt_u8 i=0;
			if(getSolarPara(phili_solar_protocol.dataBuf,solarParaTemp,PRO_HCU)==4)
			{
				philiSolar->all.batteryVoltage=solarParaTemp[i++]/10;//to 10mV
				philiSolar->all.batteryChargeCurrent=solarParaTemp[i++]/10;//to 10mA
				philiSolar->all.batteryTemp=(xt_u8)solarParaTemp[i++];
				philiSolar->all.batteryDischargeCurrent=solarParaTemp[i++]/10;//to 10mA
				PrintfXTOS("..batteryVoltage=%x\n",philiSolar->all.batteryVoltage);
				PrintfXTOS("..batteryChargeCurrent=%x\n",philiSolar->all.batteryChargeCurrent);
				PrintfXTOS("..batteryTemp=%x\n",philiSolar->all.batteryTemp);
				PrintfXTOS("..batteryDischargeCurrent=%x\n",philiSolar->all.batteryDischargeCurrent);
			}		
			break;
		}
		case ID_MONITOR_HCU_DISCHARGE_MODE:
			philiSolar->all.controllerDischargeMode = acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen);
			break;
		case ID_MONITOR_HCU_STATUS:
			philiSolar->all.controllerStatus = acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen);
			if(glbRtuPara.runningInfo.flag_polling_sloar_start)
			{
				glbRtuPara.runningInfo.flag_polling_sloar_end=YES;
				PrintfXTOS("  polling end\n");
			}			
			break;
		//--------------EPS
		case ID_MONITOR_EPS_PARA:
			break;
		case ID_MONITOR_EPS_ALARM:
			break;
		//--------------DESS
		case ID_MONITOR_DESS_BATT_PARA:
		{
			xt_u8 i=0;
			if(getSolarPara(phili_solar_protocol.dataBuf,solarParaTemp,PRO_HCU)==7)
			{
				philiSolar->all.batteryTotalChargeEnergy=solarParaTemp[i++];
				philiSolar->all.batteryTotalDischargeEnergy=solarParaTemp[i++];
				philiSolar->all.batterySoc=(xt_u8)solarParaTemp[i++];
				philiSolar->all.batteryVoltage=(xt_u16)solarParaTemp[i++]/10;//to 10mV
				philiSolar->all.batteryDischargeCurrent=(xt_u16)solarParaTemp[i++]/10;//to 10mA
				philiSolar->all.batteryWattage=(xt_u16)solarParaTemp[i++];
				philiSolar->all.batterySoh=(xt_u8)solarParaTemp[i++];
				PrintfXTOS("..battery para:\n");
				PrintfXTOS("..TotalChargeEnergy=%x\n",philiSolar->all.batteryTotalChargeEnergy);
				PrintfXTOS("..TotalDischargeEnergy=%x\n",philiSolar->all.batteryTotalDischargeEnergy);
				PrintfXTOS("..Soc=%x\n",philiSolar->all.batterySoc);
				PrintfXTOS("..Voltage=%x\n",philiSolar->all.batteryVoltage);
				PrintfXTOS("..Current=%x\n",philiSolar->all.batteryDischargeCurrent);
				PrintfXTOS("..Wattage=%x\n",philiSolar->all.batteryWattage);
				PrintfXTOS("..Soh=%x\n",philiSolar->all.batterySoh);				
			}
			break;
		}
		case ID_MONITOR_DESS_PCS_PARA:
		{
			xt_u8 i=0;
			if(getSolarPara(phili_solar_protocol.dataBuf,solarParaTemp,PRO_HCU)==12)
			{
				philiSolar->all.acTotalActivePower=solarParaTemp[i++];
				philiSolar->all.acTotalRectivePower=solarParaTemp[i++];
				philiSolar->all.acTotalCurrent=(xt_u16)solarParaTemp[i++]/100;//to 100mA
				philiSolar->all.acPhaseA_current=(xt_u16)solarParaTemp[i++]/100;//to 100mA
				philiSolar->all.acPhaseB_current=(xt_u16)solarParaTemp[i++]/100;//to 100mA
				philiSolar->all.acPhaseC_current=(xt_u16)solarParaTemp[i++]/100;//to 100mA
				philiSolar->all.acPhaseA_voltage=(xt_u16)solarParaTemp[i++]/100;//to 100mV
				philiSolar->all.acPhaseB_voltage=(xt_u16)solarParaTemp[i++]/100;//to 100mV
				philiSolar->all.acPhaseC_voltage=(xt_u16)solarParaTemp[i++]/100;//to 100mV
				philiSolar->all.acPhaseA_activePower=(xt_u16)solarParaTemp[i++];
				philiSolar->all.acPhaseB_activePower=(xt_u16)solarParaTemp[i++];
				philiSolar->all.acPhaseC_activePower=(xt_u16)solarParaTemp[i++];
				PrintfXTOS("..ac para:\n");
				PrintfXTOS("..TotalActivePower=%x\n",philiSolar->all.acTotalActivePower);
				PrintfXTOS("..TotalRectivePower=%x\n",philiSolar->all.acTotalRectivePower);
				PrintfXTOS("..TotalCurrent=%x\n",philiSolar->all.acTotalCurrent);
				PrintfXTOS("..A_current=%x\n",philiSolar->all.acPhaseA_current);
				PrintfXTOS("..B_current=%x\n",philiSolar->all.acPhaseB_current);
				PrintfXTOS("..C_current=%x\n",philiSolar->all.acPhaseC_current);
				PrintfXTOS("..A_voltage=%x\n",philiSolar->all.acPhaseA_voltage);
				PrintfXTOS("..B_voltage=%x\n",philiSolar->all.acPhaseB_voltage);
				PrintfXTOS("..C_voltage=%x\n",philiSolar->all.acPhaseC_voltage);
				PrintfXTOS("..A_activePower=%x\n",philiSolar->all.acPhaseA_activePower);
				PrintfXTOS("..B_activePower=%x\n",philiSolar->all.acPhaseB_activePower);
				PrintfXTOS("..C_activePower=%x\n",philiSolar->all.acPhaseC_activePower);
			}
			break;
		}			
		case ID_MONITOR_DESS_STATUS:
			philiSolar->all.controllerStatus = acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen);
			break;
		case ID_MONITOR_DESS_ALARM:
			philiSolar->all.controllerAlarm= acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen);
			if(glbRtuPara.runningInfo.flag_polling_sloar_start)
			{
				glbRtuPara.runningInfo.flag_polling_sloar_end=YES;
				PrintfXTOS("  polling end\n");
			}			
			break;
		//--------------SCS
		case ID_MONITOR_SCS_BATT_PARA:
		{
			xt_u8 i=0;
			if(getSolarPara(phili_solar_protocol.dataBuf,solarParaTemp,PRO_HCU)==7)
			{
				philiSolar->all.batteryTotalChargeEnergy=solarParaTemp[i++];
				philiSolar->all.batteryTotalDischargeEnergy=solarParaTemp[i++];
				philiSolar->all.batterySoc=(xt_u8)solarParaTemp[i++];
				philiSolar->all.batteryVoltage=(xt_u16)solarParaTemp[i++]/10;//to 10mV
				philiSolar->all.batteryDischargeCurrent=(xt_u16)solarParaTemp[i++]/10;//to 10mA
				philiSolar->all.batteryWattage=(xt_u16)solarParaTemp[i++];
				philiSolar->all.batteryTemp=(xt_u8)solarParaTemp[i++];
				PrintfXTOS("..battery para:\n");
				PrintfXTOS("..TotalChargeEnergy=%x\n",philiSolar->all.batteryTotalChargeEnergy);
				PrintfXTOS("..TotalDischargeEnergy=%x\n",philiSolar->all.batteryTotalDischargeEnergy);
				PrintfXTOS("..Soc=%x\n",philiSolar->all.batterySoc);
				PrintfXTOS("..Voltage=%x\n",philiSolar->all.batteryVoltage);
				PrintfXTOS("..Current=%x\n",philiSolar->all.batteryDischargeCurrent);
				PrintfXTOS("..Wattage=%x\n",philiSolar->all.batteryWattage);
				PrintfXTOS("..Temp=%x\n",philiSolar->all.batteryTemp);				
			}
			break;
		}
		case ID_MONITOR_SCS_PARA:
		{
			xt_u8 i=0;
			if(getSolarPara(phili_solar_protocol.dataBuf,solarParaTemp,PRO_HCU)==12)
			{
				philiSolar->all.acTotalActivePower=solarParaTemp[i++];
				philiSolar->all.acTotalEnery=solarParaTemp[i++];
				philiSolar->all.acTotalCurrent=(xt_u16)solarParaTemp[i++]/100;//to 100mA
				philiSolar->all.acPhaseA_current=(xt_u16)solarParaTemp[i++]/100;//to 100mA
				philiSolar->all.acPhaseB_current=(xt_u16)solarParaTemp[i++]/100;//to 100mA
				philiSolar->all.acPhaseC_current=(xt_u16)solarParaTemp[i++]/100;//to 100mA
				philiSolar->all.acPhaseA_voltage=(xt_u16)solarParaTemp[i++]/100;//to 100mV
				philiSolar->all.acPhaseB_voltage=(xt_u16)solarParaTemp[i++]/100;//to 100mV
				philiSolar->all.acPhaseC_voltage=(xt_u16)solarParaTemp[i++]/100;//to 100mV
				philiSolar->all.acPhaseA_activePower=(xt_u16)solarParaTemp[i++];
				philiSolar->all.acPhaseB_activePower=(xt_u16)solarParaTemp[i++];
				philiSolar->all.acPhaseC_activePower=(xt_u16)solarParaTemp[i++];
				PrintfXTOS("..ac para:\n");
				PrintfXTOS("..TotalActivePower=%x\n",philiSolar->all.acTotalActivePower);
				PrintfXTOS("..TotalEnery=%x\n",philiSolar->all.acTotalEnery);
				PrintfXTOS("..TotalCurrent=%x\n",philiSolar->all.acTotalCurrent);
				PrintfXTOS("..A_current=%x\n",philiSolar->all.acPhaseA_current);
				PrintfXTOS("..B_current=%x\n",philiSolar->all.acPhaseB_current);
				PrintfXTOS("..C_current=%x\n",philiSolar->all.acPhaseC_current);
				PrintfXTOS("..A_voltage=%x\n",philiSolar->all.acPhaseA_voltage);
				PrintfXTOS("..B_voltage=%x\n",philiSolar->all.acPhaseB_voltage);
				PrintfXTOS("..C_voltage=%x\n",philiSolar->all.acPhaseC_voltage);
				PrintfXTOS("..A_activePower=%x\n",philiSolar->all.acPhaseA_activePower);
				PrintfXTOS("..B_activePower=%x\n",philiSolar->all.acPhaseB_activePower);
				PrintfXTOS("..C_activePower=%x\n",philiSolar->all.acPhaseC_activePower);
			}
			break;
		}
		case ID_MONITOR_SCS_STATUS:	
			philiSolar->all.controllerStatus = acii2Hex(phili_solar_protocol.dataBuf,phili_solar_protocol.dataLen);
			PrintfXTOS("controllerStatus=%x\n",philiSolar->all.controllerStatus);
			if(glbRtuPara.runningInfo.flag_polling_sloar_start)
			{
				glbRtuPara.runningInfo.flag_polling_sloar_end=YES;
				PrintfXTOS("  polling end\n");
			}			
			break;
		default:
			PrintfXTOS("\n");
			break;
	}	
}
//static xt_u8 temp_buf[200];
xt_s32 DevSolarPhiliWriteAndRead(xt_u8 id_temp,xt_u8 cmd_temp,xt_u8 *set_value,xt_u8 set_len)
{
	#ifdef PHILI_SOLAR
	xt_u8 wait_time_cnt;
	xt_u8 multiTimes=glbRtuPara.runningInfo.waittime_multiTimes;
	xt_u8 temp_buf[200];
	if(multiTimes==0)
		multiTimes=1;
	PrintfXTOS("multiTimes=%d\n",multiTimes);
	//multiTimes=60;////test
	//---set start control
	//PrintfXTOS("apWrRdSD:id=%d,cmd=%d\n",id_temp,cmd_temp);
	glbRtuPara.runningInfo.is_setSolarOK=NO;
	packByIdAndSend(id_temp,temp_buf,cmd_temp,set_value,set_len);
	wait_time_cnt=0;
	//---chk recv
	while(wait_time_cnt++<READ_SOLAR_CTRL_TIMEOUT*multiTimes/UART_POLL_TIMES)
	{	
		
		if(DevSolarPhiliRecv(temp_buf,sizeof(temp_buf))==SUCCESS_XT)//delay 20ms tiemout
		{
			DevSolarPhiliCtrl(temp_buf,(xt_u8*)&philiSolar);
			return SUCCESS_XT;
		}	
	}	
	#endif
	return FAILURE_XT;
}
#endif

