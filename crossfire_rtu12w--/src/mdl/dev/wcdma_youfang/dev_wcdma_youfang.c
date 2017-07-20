#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>
#include <stdlib.h> 
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
#include "dev_wcdma_youfang.h"
#include "global_para.h"
#include "lib_st_aes.h"
extern rtu_para_t glbRtuPara;

#define BUFFER_LENGTH (256+32)
#define WCDMA_ERR_MAX 30
xt_u8 *sendBuf=NULL;
xt_s32 sendLen = 0;

static xt_u8 wcdma_buf_temp[40]={0};
static xt_u8 wcdma_buf_len=0;
static pro_pkt_parse_t	devRecvData = {0};

xt_u8 bufData[]={0x7e,0x00,0x14,0x10,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0xfe,0x00,0x00,0x54,0x78,0x44,0x61,0x74,0x61,0xad};

static xt_u32 is_uid_same(xt_dev_info_t * pDevParam)
{
	xt_u8 _8bits[8],j;
	xt_u8 *tp=_8bits;	
	tp=(xt_u8 *)&pDevParam->commDst;
	
	if(memcmp(&tp[2],glbRtuPara.flashConfig.value.rtuOrPlc_uid_own,sizeof(glbRtuPara.flashConfig.value.rtuOrPlc_uid_own))==0)
	{
		PrintfXTOS("is device uid\n");
		return SUCCESS_XT;
	}	
	else
	{
		PrintfXTOS("is'nt dv uid\n");
		PrintfXTOS("dv uid:");	
		for(j=0;j<6;j++)
			PrintfXTOS("%0.2x ",glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[j]);
		PrintfXTOS("\n");				
	}
	return FAILURE_XT;
}


char *itoa(int d1, char *s)
{
	int i=0,j,k;
	//PrintfXTOS("0-%d\n",d1);
	s[i++]=d1/10000+'0';                    //0
	//PrintfXTOS("1-%c\n",s[i-1]);
	s[i++]=d1%10000/1000+'0';          //5
	//PrintfXTOS("1-%c\n",s[i-1]);
	s[i++]=d1%1000/100+'0';             //8
	//PrintfXTOS("1-%c\n",s[i-1]);
	s[i++]=d1%100/10+'0';                //8
	//PrintfXTOS("1-%c\n",s[i-1]);
	s[i++]=d1%10+'0';                       //7
	//PrintfXTOS("1-%c\n",s[i-1]);
	s[i++]=0;

	for(i=0;i<6;i++)
	{
		if(s[i]!='0')
			break;
	}
	k=i;
	//PrintfXTOS("i=%d\n",i);
	for(j=0;j<6;j++,i++)
	{
		if(j<6-k)
			s[j]=s[i];
		else 
			s[j]=0;
	}
	//for(i=0;i<6;i++)
	//{
	//	PrintfXTOS("d-%c\n",s[i]);
	//}
	return s;
	

}


xt_s32 DevWcdmaYoufang_writeAndRead(xt_u8* pwData,xt_s32 wdataLen,xt_u8* prData,xt_s32 *rdataLen)
{
	xt_s32 ret=FAILURE_XT;
	xt_u8 buf[50];
	xt_u8 len;
	xt_s32 i;
	buf[0]='A';
	buf[1]='T';
	buf[2]='+';
	MemMemcpyXTOS(&buf[3], pwData, wdataLen);
	len=3+wdataLen;
	buf[len++]='\r';
	buf[len++]='\n';
	drvUartWrite(buf,len);
	PrintfXTOS("WR wcdma wrData len=%d : \n",len);
	for(i=0;i<len;i++)
		PrintfXTOS("%c",buf[i]);
	PrintfXTOS("\n");
	
	//TaskSleepMs(1000);
	*rdataLen=0;
	//drvUartRead(prData,rdataLen);
	if(drvUartRead(prData,rdataLen)==0)
		PrintfXTOS("wcdma read Err : \n");
	else
	{	
		PrintfXTOS("wcdma read Ok len=%d : \n",*rdataLen);
		for(i=0;i<*rdataLen;i++)
		{
			PrintfXTOS("%0.2x ",prData[i]);
		}
		PrintfXTOS("\nRD1????\n");
		for(i=0;i<*rdataLen;i++)
		{
			PrintfXTOS("%c",prData[i]);
			if(i<*rdataLen-4)
			if((prData[i]=='F')&&(prData[i+1]=='A')&&(prData[i+2]=='I')&&(prData[i+3]=='L'))
			{
				PrintfXTOS("\nfind Err... to tcp reset and resetup");
				glbRtuPara.devPara.flag_dev_ini=NO;
			}
		}
		PrintfXTOS("\nRD2????\n");
		ret=SUCCESS_XT;
		
	}
	return ret;
	
}

xt_void  DevWcdmaYoufangInit(void)
{
	static xt_u8 wcdmaInitStep=0;
	static xt_u8 wcdmaErrCnt=0;
	xt_u8 readBuf[100];
	xt_s32 readLen;
	if(sendBuf==NULL)
		sendBuf=MemMallocXTOS(BUFFER_LENGTH);
	KEY_getVal();
	KEY_doAction();
	//if(glbRtuPara.devPara.flag_dev_ini==YES)
	//	return ;
	TaskSleepMs(200);
	switch(wcdmaInitStep)
	{
		case 0:
			//AT+SRST
			TaskSleepMs(1000);
			DevWcdmaYoufang_writeAndRead("SRST",strlen("SRST"),readBuf,&readLen);
			wcdmaInitStep=1;
			TaskSleepMs(6000);
			break;
		case 1:
		{
			//AT+CCID
                     //
                     //+CCID: 89860116838015282176
                     //
                     //OK
			xt_u8 temp[]={0x41,0x54,0x2B,0x43,0x43,0x49,0x44,0x0D,0x0D,0x0A,0x2B,0x43,0x43,0x49,0x44,0x3A //20 38 39 38 36 30 31 31 36 38 33 38 30 31 35 32 38 32 31 37 36 0D 0A 0D 0A 4F 4B 0D 0A 
				};
			DevWcdmaYoufang_writeAndRead("CCID",strlen("CCID"),readBuf,&readLen);
			if(memcmp(readBuf,temp,sizeof(temp))==0)
			{
				wcdmaInitStep=2;
				wcdmaErrCnt=0;
			}
			else
			{
				wcdmaErrCnt++;
				if(wcdmaErrCnt>=WCDMA_ERR_MAX)
				{
					wcdmaErrCnt=0;
					wcdmaInitStep=0;
				}
			}
			break;
		}
		case 2:
		{
			//AT+CGDCONT=1,"IP","CMNET"
			//
			//
			//OK
			//
			xt_u8 temp[]={0x41,0x54,0x2b,0x43,0x47,0x44,0x43,0x4f,0x4e,0x54,
			                        0x3d,0x31,0x2c,0x22,0x49,0x50,0x22,0x2c,0x22,0x43,
			                        0x4d,0x4e,0x45,0x54,0x22,0x0d,0x0d,0x0a,
			                        0x4F,0x4B,0x0D,0x0A};
			DevWcdmaYoufang_writeAndRead("CGDCONT=1,\"IP\",\"CMNET\"",strlen("CGDCONT=1,\"IP\",\"CMNET\""),readBuf,&readLen);//\" Ä¿µÄÊäÈë"
			if(memcmp(readBuf,temp,sizeof(temp))==0)
			{
				wcdmaInitStep=3;
				wcdmaErrCnt=0;
			}
			else
			{
				wcdmaErrCnt++;
				if(wcdmaErrCnt>=WCDMA_ERR_MAX)
				{
					wcdmaErrCnt=0;
					wcdmaInitStep=0;
				}
			}
			break;
		}
		case 3:
		{
                     //AT+XIIC=1
                     //
                     //OK
                     //
                     //+NEWIP:10.8.56.79
                     //
                     //+PPPSTATUS:OPENED
			xt_u8 temp[]={0x41,0x54,0x2B,0x58,0x49,0x49,0x43,0x3D,0x31,0x0D,
				                 0x0D,0x0A,0x4F,0x4B,0x0D,0x0A// 0D 0A 2B 4E 45 57 49 50 3A 31 30 2E 38 2E 35 36 2E 37 39 0D 0A 0D 0A 2B 50 50 50 53 54 41 54 55 53 3A 4F 50 45 4E 45 44 0D 0A 
				                 };
			TaskSleepMs(200);
			DevWcdmaYoufang_writeAndRead("XIIC=1",strlen("XIIC=1"),readBuf,&readLen);
			if(memcmp(readBuf,temp,sizeof(temp))==0)
			{
				wcdmaInitStep=4;
				wcdmaErrCnt=0;
			}
			else
			{
				wcdmaErrCnt++;
				if(wcdmaErrCnt>=WCDMA_ERR_MAX*4)
				{
					wcdmaErrCnt=0;
					wcdmaInitStep=0;
				}
			}
			break;
		}
		/*
		case 4:
		{
			//OK
			//
			xt_u8 temp[]={0x4F,0x4B,0x0D,0x0A};
			DevWcdmaYoufang_writeAndRead("TCPKEEPALIVE=1,120,75",sizeof("TCPKEEPALIVE=1,120,75"),readBuf,&readLen);
			if(memcmp(readBuf,temp,sizeof(temp)==0))
				wcdmaInitStep=5;
				glbRtuPara.devPara.flag_dev_ini=YES;
			break;
		}	*/
		case 4:
		{
			xt_u8 i;
			DevWcdmaYoufang_writeAndRead("TCPKEEPALIVE=1,120,75",strlen("TCPKEEPALIVE=1,120,75"),readBuf,&readLen);
			for(i=0;i<readLen-1;i++)
			{
				if((readBuf[i]='O')&&(readBuf[i+1]=='K'))
					break;
			}
			if(i!=readLen-1)
			{
				wcdmaInitStep=5;
				PrintfXTOS("tcp keep alive success!!!");
			}
			break;
		}
		case 5:
		{
			//41 54 2B 54 43 50 53 45 54 55 50 3D 30 2C 31 32 33 2E 35 36 2E 34 30 2E 31 32 33 2C 38 36 38 36 0D 0D 0A 4F 4B 0D 0A 0D 0A 2B 54 43 50 53 45 54 55 50 3A 30 2C 4F 4B 0D 0A 
			//AT+TCPSETUP=0,123.56.40.123,8686
			//OK
			//
			//+TCPSETUP:0,OK
			char s_pNum[10]={0},string[100]={0};
			xt_u8 i;
			//xt_u8 idxT=sizeof("AT+TCPSETUP=0,1519yq1416.iask.in,16895")+2;
			xt_u8 idxT=sizeof("AT+TCPSETUP=0,1496h849l0.iask.in,23478")+2;
			//xt_u8 idxT=sizeof("AT+TCPSETUP=0,123.56.40.123,8686")+2;
			//xt_u8 idxT=sizeof("AT+TCPSETUP=0,air.semsplus.com,5887")+2;
			//xt_u8 idxT=sizeof("AT+TCPSETUP=0,139.224.223.49,5887")+2;
			xt_u8 temp[]={0x4F,0x4B,0x0D,0x0A,0x0D,0x0A,0x2B,0x54,0x43,0x50,0x53,0x45,0x54,0x55,0x50};//,0x3A,0x30,0x2C,0x4F,0x4B,0x0D,0x0A};
			
			//DevWcdmaYoufang_writeAndRead("TCPSETUP=0,139.224.223.49,5887",strlen("TCPSETUP=0,139.224.223.49,5887"),readBuf,&readLen);
			//DevWcdmaYoufang_writeAndRead("TCPSETUP=0,air.semsplus.com,5887",strlen("TCPSETUP=0,air.semsplus.com,5887"),readBuf,&readLen);
			//DevWcdmaYoufang_writeAndRead("TCPSETUP=0,123.56.40.123,8686",strlen("TCPSETUP=0,123.56.40.123,8686"),readBuf,&readLen);
			MemMemcpyXTOS(string, "TCPSETUP=0,", sizeof("TCPSETUP=0,"));
			PrintfXTOS("Cmd1:%s\n",string);
			strcat(string,glbRtuPara.flashConfig.value.severIp);
			PrintfXTOS("Cmd2:%s\n",string);
			itoa(glbRtuPara.flashConfig.value.portNum, s_pNum);
			PrintfXTOS("port:%s\n",s_pNum);
			strcat(string,s_pNum);
			PrintfXTOS("Cmd3:%s\n",string);
			idxT=strlen(string)+1;
			DevWcdmaYoufang_writeAndRead(string,strlen(string),readBuf,&readLen);
			//DevWcdmaYoufang_writeAndRead("TCPSETUP=0,1519yq1416.iask.in,16895",strlen("TCPSETUP=0,1519yq1416.iask.in,16895"),readBuf,&readLen);
			//vWcdmaYoufang_writeAndRead("TCPSETUP=0,1496h849l0.iask.in,23478",strlen("TCPSETUP=0,1496h849l0.iask.in,23478"),readBuf,&readLen);
			/*PrintfXTOS("compare:\n");
			for(i=0;i<sizeof(temp);i++)
			{
				PrintfXTOS("%0.2x ",readBuf[idxT+i]);
			}
			PrintfXTOS("\n");*/
			for(i=0;i<readLen-1;i++)
			{
				if((readBuf[i]='O')&&(readBuf[i+1]=='K'))
					break;
			}
			if(i!=readLen-1)
			//if(memcmp(&readBuf[idxT],temp,sizeof(temp))==0)
			{
				wcdmaErrCnt=0;
				wcdmaInitStep=0;
				glbRtuPara.devPara.flag_dev_ini=YES;
				glbRtuPara.runningInfo.flag_onine=NO;
				glbRtuPara.runningInfo.loginErrCnt=0;
				glbRtuPara.runningInfo.flag_onlineCmdStart=YES;
				PrintfXTOS("wcdma module init ok,haha...\n");
			}
			else
			{
				wcdmaErrCnt++;
				if(wcdmaErrCnt>=WCDMA_ERR_MAX*2)
				{
					wcdmaErrCnt=0;
					wcdmaInitStep=0;
				}
			}
			break;
		}
//AT+TCPCLOSE=0
//
//OK
//
//+TCPCLOSE:0,OK
//41 54 2B 54 43 50 43 4C 4F 53 45 3D 30 0D 0D 0A 4F 4B 0D 0A 0D 0A 2B 54 43 50 43 4C 4F 53 45 3A 30 2C 4F 4B 0D 0A 

		default:
			break;
	}
}


xt_s32	DevWcdmaYoufangSend(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead)
{
	ctrl_data_in_t di = {0};
	xt_u32 i= 0;
	xt_u8 temp[]="TCPSEND=0,10";
	xt_u8 len=strlen(temp);
	xt_u8 len_H,len_L;
	xt_u8 readBuf[100];
	xt_s32 readLen=0;
	xt_u8 charT[]={0x0d,0x0a,0x4f,0x4b,0x0d,0x0a,0x2b,0x54,0x43,0x50,0x53,0x45,0x4e,0x44,0x3a};
	
	di.pDataBuf = pData;
	di.dataLen = (xt_u8)dataLen;	
	sendLen = BUFFER_LENGTH;
	ProDevIotPack(pDevParam->commDst, NULL, &di, sendBuf, &sendLen);	
	if (sendLen > 0)
	{
		len_H=sendLen%100/10+'0';
		len_L=sendLen%10+'0';
		temp[len-2]=len_H;
		temp[len-1]=len_L;
		//wcdma_buf_len=0;
		PrintfXTOS("set send total num=%d\n",sendLen);		
		DevWcdmaYoufang_writeAndRead(temp,(xt_s32)len,readBuf,&readLen);
		//DevWcdmaYoufang_writeAndRead(temp,(xt_s32)len,readBuf,(xt_s32*)&readLen);
		
		//DevWcdmaYoufang_writeAndRead(temp,(xt_s32)len,wcdma_buf_temp,(xt_s32*)&wcdma_buf_len);
		//PrintfXTOS("send ->\n");		
		//DevWcdmaYoufang_writeAndRead(sendBuf,(xt_s32)sendLen,wcdma_buf_temp,(xt_s32*)&wcdma_buf_len);;
		//TaskSleepMs(30);
		//PrintfXTOS("----pxWrite=%x : \n",pxWrite);
		if (pxWrite != NULL)
		{
			//#ifdef 1
			//xt_u8 charT[]={0x0d,0x0a,0x4f,0x4b,0x0d,0x0a,0x2b,0x54,0x43,0x50,0x53,0x45,0x4e,0x44,0x3a};
			PrintfXTOS("----DevSend : \n");
			for (i = 0; i< sendLen;i++) {
				PrintfXTOS("%0.2x ",sendBuf[i]);
			}
			PrintfXTOS("\n");			
			PrintfXTOS("\n----\n");	
			//#endif
			
			pxWrite(sendBuf, (xt_s32)sendLen);
			
			//TaskSleepMs(50);
			//len=0;
			//memset(sendBuf,0,sizeof(sendBuf));
			if(pxRead(readBuf,&readLen)==0)
				PrintfXTOS("wcdma read Err : \n");
			else
			{	
				PrintfXTOS("wcdma read Ok len=%d : \n",readLen);
				for(i=0;i<readLen;i++)
					PrintfXTOS("%0.2x ",readBuf[i]);
				PrintfXTOS("\n");
				for(i=sendLen;i<readLen;i++)
					PrintfXTOS("%c",readBuf[i]);
				PrintfXTOS("\n");
				
			}
			if(memcmp(&readBuf[sendLen],charT,sizeof(charT))==0)
			{
				PrintfXTOS("SEND SUCCESS...\n");
				return SUCCESS_XT;
			}
		}
		
	}
	return FAILURE_XT;
}

xt_s32	DevWcdmaYoufangRecv(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam)
{
	xt_u8 i;
	static xt_u8 copyFlag = TRUE; 
	packet_head_t* pHead = NULL;
	xt_u8* pSubCmdOffset = NULL;
	xt_u8* pModBuf = NULL;
	xt_u32 modLen = 0;
	xt_u32 j = 0;
	xt_u8* getVailData=NULL;
	xt_u8   getVailDataLen=0;
	xt_u8 temp[]={0x0d,0x0a,0x2b,0x54,0x43,0x50,0x43,0x4c,0x4f,0x53,0x45,0x3a,0x30,0x2c,0x4c,0x69,0x6e,0x6b,0x20,0x43,0x6c,0x6f,0x73,0x65,0x64,0x0d,0x0a };

	if(dataLen>=sizeof(temp))
	{
		if(memcmp(pData,temp,sizeof(temp))==0)
		{
			glbRtuPara.devPara.flag_dev_ini=NO;
			PrintfXTOS("TCPCLOSED and to resetup...\n");
			return FAILURE_XT;
		}
	}

	if(glbRtuPara.devPara.flag_dev_ini==NO)
	{
		MDBG("wcdma recv len=%0.2x:\n",dataLen);
		for(i=0;i<dataLen&&i<40;i++)
		{
			wcdma_buf_temp[i]=pData[i];
			MDBG("%0.2x ", wcdma_buf_temp[i]);
		}
		MDBG("\n");
		wcdma_buf_len=dataLen;
		return FAILURE_XT;
	}	
	if (copyFlag == TRUE)
	{
		if (devRecvData.chkOff > 0) {
			MemMemcpyXTOS(devRecvData.chkBuf, devRecvData.chkBuf + devRecvData.chkOff,
			devRecvData.chkLen);

            devRecvData.chkOff = 0;
		}
		if(devRecvData.chkLen + dataLen <= PRO_PKT_MAX_SIZE)
		{
			MemMemcpyXTOS(devRecvData.chkBuf + devRecvData.chkLen, pData, dataLen);
			devRecvData.chkLen += dataLen;
		}
		
		//MDBG("DevZigbeeOnRecv: recv raw data = %d \n", dataLen);		
		copyFlag = FALSE;
	}else {
        if (devRecvData.chkLen == 0) {
			copyFlag = TRUE;
			return FAILURE_XT;
        }
    }
	#if 1
PARSE:	
	if (ProDevIotParse(&devRecvData) == SUCCESS_XT) {
		if (devRecvData.pktLen == 0)
		{
			copyFlag = TRUE;
			return FAILURE_XT;
		}
	
		pModBuf = devRecvData.pktBuf;
		modLen = devRecvData.pktLen;
		MDBG("DevZigbeeOnRecv: recv packSize = %d\n", modLen);
		
		pHead = (packet_head_t*)pModBuf;
		pDevParam->commCmd = pHead->cmd;

		MemMemcpyXTOS((xt_void *)&pDevParam->commDst, (xt_void *)pHead->devaddr, DEV_ADDR_BYTES);
		pDevParam->commDst = pDevParam->commDst << 16; // uid store low 48 bit

		pSubCmdOffset = pModBuf + PACK_OFFSET_DATA;
		
		MemMemcpyXTOS(pRetData, pSubCmdOffset, modLen - PACK_MIN_SIZE);
		*pRetDataLen = modLen - PACK_MIN_SIZE;
		//-----------------------------------------		
		if(is_uid_same(pDevParam)==FAILURE_XT)
		{
			PrintfXTOS("test-chkLen=%d\n",devRecvData.chkLen);
            		if (devRecvData.chkLen > 0) {
                			goto PARSE;
            		}			
			copyFlag = TRUE;
			devRecvData.pktLen = 0;
			return FAILURE_XT;
		}

		devRecvData.pktLen = 0;
		return SUCCESS_XT; 
	}
	else
	{
		copyFlag = TRUE;
		return FAILURE_XT;
	}	
#endif
	return FAILURE_XT;
}


void wcdmadTest(void)
{
	xt_u8 temp[]={0x68,0x00,0x00,0x00,0x00,0x00,0x00,0x68,0x01,0x09,
		0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x16};
	xt_u8 readBuf[100];
	xt_s32 readLen,i;
	if(glbRtuPara.devPara.flag_dev_ini==NO)
		return ;
	PrintfXTOS("\n----------wcdmadTest---------\n");
	DevWcdmaYoufang_writeAndRead("TCPSEND=0,21",strlen("TCPSEND=0,21"),readBuf,&readLen);
	drvUartWrite(temp,sizeof(temp));
	//TaskSleepMs(50);
	readLen=0;
	XtBspCtrlRead(readBuf,&readLen);
	for(i=0;i<readLen;i++)
		PrintfXTOS("%0.2x ",readBuf[i]);
	PrintfXTOS("\n");
	for(i=sizeof(temp);i<readLen;i++)
		PrintfXTOS("%c",readBuf[i]);
	PrintfXTOS("\n----------wcdmadTestEnd---------\n");
	
}





