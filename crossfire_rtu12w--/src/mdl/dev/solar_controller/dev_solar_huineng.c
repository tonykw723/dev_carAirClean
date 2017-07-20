
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
#include "dev_solar_huineng.h"
#ifdef HN_SOLAR
hn_data_t hn_para;
solar_hn_send_pack  last_send;//在发送命令前必须把发送的命令赋给此包，以判断返回值
//static xt_u8 recvTempBuf[60];
sloarHn_t  solar_hn_data;
sloarHN_to_YF_t solar_hn_to_yf_data;
xt_u8 hn_lamp_max_current=50;
solar_getTask_Hn_to_yf_t  solarGet_HN_to_YfTask={0};//以远方动力的格式设置江能精电自动工作模式
//xt_u8 hnT=0,hn_cnt=0;
//--------------------------------------------
extern rtu_para_t glbRtuPara;
extern xt_s32 rs485DataLen ;
extern ap_msg_t ap485MsgData ;
extern comfirm_enum_t is_sloar_module_ini;
//--------------------------------------------
xt_u8 hnRecvTempBuf[60];
xt_u8* dec_temp;//=(xt_u8*)&solarGetYfTask;
extern xt_time_t ap485RecvTime ;
xt_s32 Qu_packge_and_send( xt_u8 *p,xt_u8 Lengh);
//=========================================
xt_u16 get_crc16(xt_u8 *buf,xt_u32 length)
{
    xt_u32 i;
    xt_u32 j;
    xt_u32 c;
    xt_u16 crc = 0xFFFF;
    for (i=0; i<length; i++)
    {
        c = *(buf+i) & 0x00FF;
        crc^=c;
        for (j=0; j<8; j++)
        {
             if (crc & 0x0001)
             {
                crc >>= 1;
                crc ^= 0xA001;
             }
             else
             { 
                crc >>= 1;
             }
        }
   }
    crc = (crc>>8) + (crc<<8);//低位在前
    return(crc);
}
//交换高低位，例：交换前：03 04 01 02，交换之后：01 02 03 04
xt_void  SwapHbittoLbit(xt_u8 *psw1,xt_u8 *psw2,xt_u8 *psw3)
{
	xt_u8 temp=0;
	
	temp=*psw1;
	*psw1=*(psw1+2);
	*(psw1+2)=temp;
	temp=*(psw1+1);
	*(psw1+1)=*(psw1+3);
	*(psw1+3)=temp;
	
	temp=*psw2;
	*psw2=*(psw2+2);
	*(psw2+2)=temp;
	temp=*(psw2+1);
	*(psw2+1)=*(psw2+3);
	*(psw2+3)=temp;

	temp=*psw3;
	*psw3=*(psw3+2);
	*(psw3+2)=temp;
	temp=*(psw3+1);
	*(psw3+1)=*(psw3+3);
	*(psw3+3)=temp;	
}
	
xt_void  DevSolarHnInit(void)
{
	xt_u8 get_id_char[]={0xf8,0x45,0x00,0x01,0x01,0xf8,0x89,0xbe};//查询控制器id
	//xt_u8 init_char[]={0x01,0x06,0x90,0x3d,0x00,0x00,0x35,0x06};//设置为手动运行模式
	xt_u8 set_id[]={0xf8,0x45,0x00,0x01,0x01,0x01,0x49,0xfc};//设置设备id为01
	if(is_sloar_module_ini==NO)
	{
		//hn_para.id=1;
		//hn_lamp_max_current=glbRtuPara.flashConfig.value.hnjd_lamp_max_current;
		solar_hn_data.devType=0x06;
		DevSolarHnWriteAndRead(NULL,NULL,get_id_char,sizeof(get_id_char));
		refresh_HN_task_data();
		//set_led_current();
		if(hn_para.id!=0x01)
		{	
			DevSolarHnWriteAndRead(NULL,NULL,set_id,sizeof(set_id));
		}
		else
		{
			is_sloar_module_ini=YES;
		}
	}
}
xt_void set_led_current(void)
{
		xt_u8 setLedCurrent[8]={0x01,0x06,0x90,0x78};//led电流
		xt_u16 crc_chk;

				    	 	 	setLedCurrent[4]=glbRtuPara.flashConfig.value.solarMaxCurrent>>8;
		setLedCurrent[5]=glbRtuPara.flashConfig.value.solarMaxCurrent;
		crc_chk=get_crc16(setLedCurrent,6);
		crc_chk = (crc_chk>>8) + (crc_chk<<8);//低位在前
		MemMemcpyXTOS(&setLedCurrent[6],&crc_chk,2);
		DevSolarHnWriteAndRead(NULL,NULL,setLedCurrent,sizeof(setLedCurrent));
}

xt_void refresh_HN_task_data(void)
{
	xt_u8 tmp_buf[2];
	xt_u8 get_lengh=0;

	printf("refresh hn task data\n");
	solarGet_HN_to_YfTask.productionType=NULL;
	solarGet_HN_to_YfTask.maxPower=NULL;	
	solarGet_HN_to_YfTask.runTime=NULL;
	solarGet_HN_to_YfTask.overDischargeTimes=NULL;
	solarGet_HN_to_YfTask.smartCtrl=NULL;
	solarGet_HN_to_YfTask.advance=NULL;
	solarGet_HN_to_YfTask.photoCtrlDelay=NULL;


	solarGet_HN_to_YfTask.volatgeOfOverDischargeReturn=solarGet_HN_to_YfTask.voltageOfOverDischarge;
	solarGet_HN_to_YfTask.priorityOfPowerSupply=NULL;
	solarGet_HN_to_YfTask.timePeriod_4th=NULL;
	solarGet_HN_to_YfTask.current_4th=NULL;
	solarGet_HN_to_YfTask.timePeriod_5th=NULL;
	solarGet_HN_to_YfTask.current_5th=NULL;
	solarGet_HN_to_YfTask.timePeriod_6th=NULL;
	solarGet_HN_to_YfTask.current_6th=NULL;
	
	tmp_buf[0]=0x90;
	tmp_buf[1]=0x00;
	get_lengh=18;
	Qu_packge_and_send(tmp_buf,get_lengh);

	tmp_buf[0]=0x90;
	tmp_buf[1]=0x0e;
	get_lengh=2;
	Qu_packge_and_send(tmp_buf,get_lengh);

	tmp_buf[0]=0x90;
	tmp_buf[1]=0x1e;
	get_lengh=2;
	Qu_packge_and_send(tmp_buf,get_lengh);
	
	tmp_buf[0]=0x90;
	tmp_buf[1]=0x3d;
	get_lengh= 8;
	Qu_packge_and_send(tmp_buf,get_lengh);

	tmp_buf[0]=0x90;
	tmp_buf[1]=0x5a;
	get_lengh=6;
	Qu_packge_and_send(tmp_buf,get_lengh);

}
	
xt_s32	DevSolarHnSend(xt_u8* pData, xt_s32 dataLen)
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

xt_s32	DevSolarHnRecv(xt_u8* pData, xt_u32 maxLen)
{
	xt_u8 i,iBegin=0;
	xt_u16 chkSum=0;
	xt_u8 temp=0;
	xt_u8 dataLen=0,rcvlen=0;

	dataLen=XtBspCtrlLv2_Read(pData, maxLen);
	if(dataLen!=0)
	{
		rs485DataLen=dataLen;
		MemMemcpyXTOS(ap485MsgData.proAp.proApData, pData, dataLen);
	}	
	if(dataLen>0)
	{
		PrintfXTOS("huineng dev recv:pData=%x,dataLen=%d\n",pData,dataLen);
		for(i=0;i<dataLen;i++)
		{
			PrintfXTOS("%0.2x ",pData[i]);
		}
		for(i=0;i<dataLen;i++)		
		{
			if((pData[i]==last_send.id)&&pData[i+1]==last_send.cmd)      //收到的数据与发送的ID的命令号一致
			{
				iBegin=i;
				switch(last_send.cmd)
				{
					case READ_DIG:
					case DIGTTAL_I_O:    //
						{
							rcvlen=4;
						}
						break;
					case WRITE_DIG:
						{
							rcvlen=6;
						}
						break;
					case WRITE_O_HOLDING_REG:
						{
							rcvlen=6;
						}
						break;
					case READ_HOLDING_REG://读保持寄存器
						{
							rcvlen=pData[iBegin+2]+3;
						}
						break;
					case INPUT_REG:       //读输入寄存器
						{
							rcvlen=pData[iBegin+2]+3;
						}
						break;
					case WRITE_N_HOLDING_REG:  //写保持寄存器
						{
							rcvlen=6;
						}
						break;
					case GET_SET_ID:
					{
						//hn_para.id=pData[2];
						rcvlen=3;
					}
						break;
					default:
						break;
				}
			}
			chkSum=get_crc16(&pData[iBegin],rcvlen);
			if(chkSum==(pData[iBegin+rcvlen]*256+pData[iBegin+rcvlen+1]))
			{
				PrintfXTOS("..*****chk ok:iBegin=%d len=%d:*****\n..",iBegin,rcvlen+2);
				for(i=0;i<rcvlen+2;i++)
				{
					temp=pData[iBegin+i];
					pData[i]=temp;
					PrintfXTOS("%0.2x ",pData[i]);
			  	}
				return SUCCESS_XT;
		 	 }
		}
	}
	return FAILURE_XT;
}
			

xt_void  DevSolarHnCtrl(xt_u8* src_pData,xt_u8* dec_pData)
{
	//----------太阳能电能数据
	xt_u8 i=0;
	xt_u8* pbuf;
	xt_u8 state_temp=0;
	
	if(src_pData[1]>0x80)
	{
		PrintfXTOS("dev_solar_hn_ctrl cmd error!\n");
	}
	else
	{
		switch(src_pData[1])
		{
			case READ_DIG:
				{
					//if((src_pData[2]==0x00)&&(src_pData[3]==0x002))
					if(last_send.addr_register==0x0002)
					{
						hn_para.loadStatus=src_pData[3];
					}
				}
				break;
			case DIGTTAL_I_O:    //
			case WRITE_DIG:
			case WRITE_O_HOLDING_REG:
				break;
			case READ_HOLDING_REG://读保持寄存器
				{
 					switch(last_send.addr_register)
						{
							case 0x9000:
							{
								i=4;
								if(src_pData[i]==0x00)
								{
									solarGet_HN_to_YfTask.batteryType=0x03;
								}
								if(src_pData[i]==0x02)
								{
									solarGet_HN_to_YfTask.batteryType=0x01;
								}
								if(src_pData[i]==0x03)
								{
									solarGet_HN_to_YfTask.batteryType=0x02;
								}
								i=9;
								solarGet_HN_to_YfTask.voltageOfOverCharge=((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])/10;

								i=17;
								solarGet_HN_to_YfTask.voltageOfTisheng=((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])/10;

								i=19;
								solarGet_HN_to_YfTask.voltageOfFuChong=((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])/10;

							}
							break;
							case 0x900e:
							{
								i=3;
								solarGet_HN_to_YfTask.voltageOfOverDischarge=((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])/10;

							}
							break;
							case 0x901e:
							{
								i=3;
								solarGet_HN_to_YfTask.photoVoltage=((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])/10;
							}
							break;
							case 0x903d:
							{
								i=4;

								if(src_pData[i]==0x00)
									solarGet_HN_to_YfTask.ctrlMode=0x01;
								else if(src_pData[i]==0x01)
									solarGet_HN_to_YfTask.ctrlMode=0x04;
								else if(src_pData[i]==0x02)
									solarGet_HN_to_YfTask.ctrlMode=0x02;
								i=5;
								solarGet_HN_to_YfTask.timePeriod_1th=(src_pData[i])*0x10+(src_pData[i+1])/10;
								i=7;
								solarGet_HN_to_YfTask.timePeriod_2th=(src_pData[i])*0x10+(src_pData[i+1])/10;
								i=9;
								solarGet_HN_to_YfTask.timePeriod_3th=(src_pData[i])*0x10+(src_pData[i+1])/10;

							}
							break;
							case 0x905a:
							{
				
								/****按LED电流=hnjd_lamp_max_current  设置百分比****
								i=3;
								solarGet_HN_to_YfTask.current_1th=((((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])/100)*glbRtuPara.flashConfig.value.hnjd_lamp_max_current)/100;
								i=5;
								solarGet_HN_to_YfTask.current_2th=((((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])/100)*glbRtuPara.flashConfig.value.hnjd_lamp_max_current)/100;
								i=7;
								solarGet_HN_to_YfTask.current_3th=((((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])/100)*glbRtuPara.flashConfig.value.hnjd_lamp_max_current)/100;
								*/
								//****按LED电流=2A  设置百分比****
								i=3;
								solarGet_HN_to_YfTask.current_1th=(((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])*2)/100;
								i=5;
								solarGet_HN_to_YfTask.current_2th=(((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])*2)/100;
								i=7;
								solarGet_HN_to_YfTask.current_3th=(((xt_u32)src_pData[i]*256+(xt_u32)src_pData[i+1])*2)/100;
								
							}
							break;
							
							default:
								break;
						}
				 }
				break;
			case INPUT_REG:       //读输入寄存器
				{
					switch(last_send.addr_register)
						{
							case 0x3100:
							{
								i=3;
								pbuf=(xt_u8*)&solar_hn_data.chargeDevVoltage_h;
								MemMemcpyXTOS(pbuf,&src_pData[i],8);
								i+=8;
								pbuf=(xt_u8*)&solar_hn_data.batteryVoltage_h;
								MemMemcpyXTOS(pbuf,&src_pData[i],8);
								i+=16;
								pbuf=(xt_u8*)&solar_hn_data.loadVoltage_h;
								MemMemcpyXTOS(pbuf,&src_pData[i],8);
								//将功率高低们交换
								SwapHbittoLbit((xt_u8*)&solar_hn_data.chargeDevPwH_h,(xt_u8*)&solar_hn_data.batteryPwH_h,(xt_u8*)&solar_hn_data.loadPwH_h);

								//---将汇能精电数据的格式转换为远方动力的格式
								//solar_hn_to_yf_data.pvVoltage=((xt_u16)solar_hn_data.chargeDevVoltage_h*256+solar_hn_data.chargeDevVoltage_l)/100;
								solar_hn_to_yf_data.pvVoltage_h=solar_hn_data.chargeDevVoltage_h;
								solar_hn_to_yf_data.pvVoltage_l=solar_hn_data.chargeDevVoltage_l;
								//solar_hn_to_yf_data.batteryVoltage=((xt_u16)solar_hn_data.batteryVoltage_h*256+solar_hn_data.batteryVoltage_l)/10;
								solar_hn_to_yf_data.batteryVoltage_h=solar_hn_data.batteryVoltage_h;
								solar_hn_to_yf_data.batteryVoltage_l=solar_hn_data.batteryVoltage_l;
								//solar_hn_to_yf_data.loadVoltage=((xt_u16)solar_hn_data.loadVoltage_h*256+solar_hn_data.loadVoltage_l)/100;
								solar_hn_to_yf_data.loadVoltage_h=solar_hn_data.loadVoltage_h;
								solar_hn_to_yf_data.loadVoltage_l=solar_hn_data.loadVoltage_l;
								//solar_hn_to_yf_data.loadCurrent=((xt_u16)solar_hn_data.loadCurrent_h*256+solar_hn_data.loadCurrent_l)/10;
								solar_hn_to_yf_data.loadCurrent_h=solar_hn_data.loadCurrent_h;
								solar_hn_to_yf_data.loadCurrent_l=solar_hn_data.loadCurrent_l;

								if(((xt_u16)solar_hn_data.loadPwL_h*256+solar_hn_data.loadDevPwL_l)==0)
								{
									solar_hn_to_yf_data.loadStatus=0x00;//负载状态---关
								}
								else
								{
									solar_hn_to_yf_data.loadStatus=0x01;//负载状态---开
								}
	
							}
							break;
						  case 0x3200:
						  	{
							  		i=3;
									pbuf=&solar_hn_data.batteryStatus_h;
									MemMemcpyXTOS(pbuf,&src_pData[i],2);	
									i=5;
									pbuf=&solar_hn_data.chargeDevStatus_h;
									MemMemcpyXTOS(pbuf,&src_pData[i],2);

									//蓄电池状态
									state_temp=solar_hn_data.batteryStatus_l&0x0f;
									if(state_temp==0x04)	//过放
									{
										solar_hn_to_yf_data.batteryStatus=0x00;
									}
									if(state_temp==0x02)	//欠压
									{
										solar_hn_to_yf_data.batteryStatus=0x01;
									}
									if(state_temp==0x00)	//正常
									{
										solar_hn_to_yf_data.batteryStatus=0x02;
									}
									//if(state_temp==0x04)	
									// //充电限制汇能精电无此状态
									//{
									//	solar_hn_to_yf_data.pvStatus=0x03;
									//}
									if(state_temp==0x01)	//超压
									{
										solar_hn_to_yf_data.batteryStatus=0x04;
									}
									//光电池状态-----此处与远方动力完全不一样，暂时这样做
									state_temp=0;
									state_temp=solar_hn_data.chargeDevStatus_l&0x0f;

									if(state_temp>0)
									{
										solar_hn_to_yf_data.pvStatus=0x02;//达到充电电压
									}
									else if(state_temp==0)
									{
										solar_hn_to_yf_data.pvStatus=0x01;//达到光控条件
									}
									   
						  	}
						  	break;
						  case 0x3300:
						  	{
							  i=11;
								pbuf=&solar_hn_data.dailyDischargeEnergy_h_a;
								MemMemcpyXTOS(pbuf,&src_pData[i],12);
								glbRtuPara.runningInfo.is_getSolarOK=YES;
								SwapHbittoLbit((xt_u8*)&solar_hn_data.dailyDischargeEnergy_h_a,(xt_u8*)&solar_hn_data.monthTotalDischargeEnergy_h_a,(xt_u8*)&solar_hn_data.yearTotalDischargeEnergy_h_a);

								solar_hn_to_yf_data.dailyDischargeEnergy_h=((xt_u16)solar_hn_data.dailyDischargeEnergy_h_a*256+solar_hn_data.dailyDischargeEnergy_h_b)*10;//单位--WH
								solar_hn_to_yf_data.dailyDischargeEnergy_l=((xt_u16)solar_hn_data.dailyDischargeEnergy_l_a*256+solar_hn_data.dailyDischargeEnergy_l_b)*10;
								solar_hn_to_yf_data.totaldischargeEnergy_h=((xt_u16)solar_hn_data.yearTotalDischargeEnergy_h_a*256+solar_hn_data.yearTotalDischargeEnergy_h_b)/100;//单位--KWH
								solar_hn_to_yf_data.totaldischargeEnergy_l=((xt_u16)solar_hn_data.yearTotalDischargeEnergy_l_a*256+solar_hn_data.yearTotalDischargeEnergy_l_b)/100;
						  	}
						  	break;
						default:
							break;
						}	
				}
				break;
			case WRITE_N_HOLDING_REG:  //写保持寄存器
				break;
			case GET_SET_ID:
				{
					hn_para.id=src_pData[2];
				}
			default:
				break;

		}
	}
}

xt_s32 DevSolarHnWriteAndRead(xt_u8 id_temp,xt_u8 cmd_temp, xt_u8 *sendBuf,xt_u8 sendLen)
{
	#ifdef HN_SOLAR
	//xt_u8 wait_time_cnt;	
	//---set start control

	MemMemsetXTOS(&last_send.id,0,sizeof(last_send));
	MemMemsetXTOS(hnRecvTempBuf,0,sizeof(hnRecvTempBuf));
	last_send.id=sendBuf[0];
	last_send.cmd=sendBuf[1];
	last_send.addr_register=256*sendBuf[2]+sendBuf[3];
	last_send.n_register=256*sendBuf[4]+sendBuf[5];
	
	DevSolarHnSend(sendBuf,sendLen);	
	TaskSleepMs(400);
	is_sloar_module_ini=NO;
	glbRtuPara.runningInfo.is_setSolarOK=NO;
	if(DevSolarHnRecv(hnRecvTempBuf,sizeof(hnRecvTempBuf))==SUCCESS_XT)//delay 20ms tiemout
	{
		DevSolarHnCtrl(hnRecvTempBuf,dec_temp);
		glbRtuPara.runningInfo.is_setSolarOK=YES;
		is_sloar_module_ini=YES;
		return SUCCESS_XT;
	}
	else if(DevSolarHnRecv(hnRecvTempBuf,sizeof(hnRecvTempBuf))==FAILURE_XT)
	{
		hn_para.loadStatus=0x02;
	}
	#endif
	return FAILURE_XT;
}


xt_s32 packge_and_send( xt_u8 *p,xt_u8 Lengh)
{
	xt_u8 sendbuf[30],send_lengh=0;
	xt_u16 crc_chk=0;
	
	sendbuf[0]=0x01;	//控制器地址
	sendbuf[1]=0x10;	//写寄存器数据
	sendbuf[2]=*p;	//寄存器地址高位
	p++;
	sendbuf[3]=*p;	//寄存器地址低位
	p++;
	sendbuf[4]=0x00;		//寄存器写入个数高位
	sendbuf[5]=Lengh/2;		//寄存器写入个数低位
	sendbuf[6]=Lengh;			//写入数据长度
	MemMemcpyXTOS(&sendbuf[7],p,Lengh);

	//chkSum=get_crc16(&pData[iBegin],rcvlen);
	crc_chk=get_crc16(sendbuf,7+Lengh);
	crc_chk = (crc_chk>>8) + (crc_chk<<8);//低位在前
	MemMemcpyXTOS(&sendbuf[6+Lengh+1],&crc_chk,2);
	//sendbuf[6+Lengh+1]
	send_lengh=Lengh+7+2;
	DevSolarHnWriteAndRead(NULL,NULL,sendbuf,send_lengh);

	return SUCCESS_XT;
}


xt_s32 Qu_packge_and_send( xt_u8 *p,xt_u8 Lengh)	//查询自动运行数据打包发送
{
	xt_u8 sendbuf[30],send_lengh=0;
	xt_u16 crc_chk=0;
	
	sendbuf[0]=0x01;	//控制器地址
	sendbuf[1]=0x03;	//写寄存器数据
	sendbuf[2]=*p;	//寄存器地址高位
	p++;
	sendbuf[3]=*p;	//寄存器地址低位
	p++;
	sendbuf[4]=0x00;		//寄存器写入个数高位
	sendbuf[5]=Lengh/2;		//寄存器写入个数低位

	crc_chk=get_crc16(sendbuf,6);
	crc_chk = (crc_chk>>8) + (crc_chk<<8);//低位在前
	MemMemcpyXTOS(&sendbuf[6],&crc_chk,2);
	send_lengh=8;
	DevSolarHnWriteAndRead(NULL,NULL,sendbuf,send_lengh);

	return SUCCESS_XT;
}

#endif




