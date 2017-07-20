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
extern rtu_para_t glbRtuPara;
static xt_u8 zgb_buf_temp[40]={0};
static xt_u8 zgb_buf_len=0;

#include "lib_st_aes.h"

#define BUFFER_LENGTH (256+32)
extern rtu_para_t glbRtuPara;
extern xt_u8 *sendBuf;//[BUFFER_LENGTH];
extern xt_s32 sendLen;
static pro_pkt_parse_t	devZigbeeRecvData = {0};
#if defined(ENCRY_LIB_NONE)
#else
#define DEV_RSC_USE_ENCRYPT
#endif
#define PRINTF_REAL_PROTOCOL
#ifdef DEV_RSC_USE_ENCRYPT
//static xt_u8 encryRaw[BUFFER_LENGTH] = {0};
static xt_u8 *encryBuf=NULL;//[BUFFER_LENGTH] = {0};
static xt_u32 encryLen = 0;

//static xt_u8 encrySendbuf[BUFFER_LENGTH];
static xt_u32 encrySendLen = 0;
#endif
#if 0
static const xt_u8 pRdZigbeeCfg[]={0x20,0x07,0xff,0xff,0xff,0xff,0xff};//{0x23,0xA0};
static const xt_u8 psWrZigbeeNet[]={0x22,0x09,0xff,0xff,0xff,0xff,0x01,0x06,0x02};
static const xt_u8 psWrZigbeeNode[]={0x22,0x09,0xff,0xff,0xff,0xff,0x01,0x07,0x02};
static const xt_u8 psWrZigbeeCast[]={0x22,0x09,0xff,0xff,0xff,0xff,0x01,0x08,0x01};
static const xt_u8 psWrZigbeeRate[]={0x22,0x09,0xff,0xff,0xff,0xff,0x01,0x10,0x04};
static const xt_u8 psWrZigbeeCode[]={0x22,0x09,0xff,0xff,0xff,0xff,0x01,0x14,0x02};
#else
static uint8_t z_read_cfg[]={0x23,0xa0};
static uint8_t z_write_cfg[]={0x23,0xfe,0x39,0x42,0xff,0x02,0x03,0x01,0x04,0x01,0x01,0x02,0x0a,0x0f,0x00,0x01};
static uint8_t z_close_cfg[]={0x23,0x23};

//static uint8_t V5_read_cfg[]={0x20,0x07,0xFF,0xFF,0xFF,0xFF,0xFF};



#endif
typedef enum
{
	U_SERVER,U_DEVICE
}Uid_enum_t;
static xt_u32 is_uid_same(xt_dev_info_t * pDevParam,Uid_enum_t u_type)
{
	xt_u8 _8bits[8],j;
	xt_u8 *tp=_8bits;	
	tp=(xt_u8 *)&pDevParam->commDst;
	//PrintfXTOS("recv uid:");	
	//for(j=0;j<6;j++)
	//	PrintfXTOS("%0.2x ",tp[j+2]);
	//PrintfXTOS("\n");	

	
	
	//----判断uid是否是server uid，如果是，不返回--------
	switch(u_type)
	{
		case U_SERVER:			
			if(memcmp(&tp[2],glbRtuPara.flashConfig.value.sever_uid,sizeof(glbRtuPara.flashConfig.value.sever_uid))==0)
			{
				PrintfXTOS("is sever uid\n");
				return SUCCESS_XT;
			}
			else
			{
				PrintfXTOS("is'nt sv uid\n");
				PrintfXTOS("sv uid:");	
				for(j=0;j<6;j++)
					PrintfXTOS("%0.2x ",glbRtuPara.flashConfig.value.sever_uid[j]);
				PrintfXTOS("\n");				
			}
			break;
		case U_DEVICE:
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
			break;
		default:
			break;
	}
	return FAILURE_XT;
	//-----------------------------------------------------------------	
}

static xt_void ZigbeeDataOut(xt_u8 *pt_out ,xt_u8 len,xt_u16 sleep_time)
{
	xt_u8 i;
	XtBspCtrlWrite(pt_out,len);	
	PrintfXTOS(" ttttttttt\n");
	for(i=0;i<len;i++)
		PrintfXTOS(" %0.2x",pt_out[i]);
	PrintfXTOS("\n ttttttttt\n");	
	TaskSleepMs(sleep_time);
}
//#ifndef V5_NEW_ZIGBEE
#if 1
xt_void  DevZigbeeSzInit(void)
{
	static xt_u8 step=0;
	static xt_u8 errCnt=0;
	xt_u8 len;
	#define ZIGBEE_CFG_CLOSE 1
	#define ZIGBEE_CFG_OPEN 0
	if((glbRtuPara.devPara.flag_dev_ini==YES)||(glbRtuPara.devPara.flag_dev_ini==ERR))
		return ;
	if(sendBuf==NULL)
		sendBuf=MemMallocXTOS(BUFFER_LENGTH);
	#ifdef DEV_RSC_USE_ENCRYPT
	if(encryBuf==NULL)
		encryBuf=MemMallocXTOS(BUFFER_LENGTH);
	#endif
	
	switch(step)
	{
		case 0:
			XtBspZigbeeCfg(ZIGBEE_CFG_OPEN);
			TaskSleepMs(3000);
			step=1;		
			break;
		case 1:
			{
				zigbeeSz_cfg_req_i_t *zcfg=NULL;				
				zcfg=(zigbeeSz_cfg_req_i_t *)zgb_buf_temp;						
				if((zcfg->head==0xa2)&&(zcfg->nettype<8)&&(zcfg->nodetype<5)&&(zcfg->sendModel<4))
				{	
					PrintfXTOS("zigbee uid: %0.2x %0.2x\n",zcfg->moduleAddr[0],zcfg->moduleAddr[1]);
					PrintfXTOS("dev uid:%0.2x %0.2x\n",glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[4],glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[5]);

					//if(memcmp(zcfg->moduleAdd,&glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[4],sizeof(zcfg->moduleAddr))!=0)
					if((zcfg->moduleAddr[0]!=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[4])
						||(zcfg->moduleAddr[1]!=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[5])
						||(zcfg->sendModel!=SD_MS_CAST)||(zcfg->freqDot!=glbRtuPara.flashConfig.value.comFreq))
					{
						zigbeeSz_cfg_resp_i_t *zcfg_wr= NULL;
						zcfg_wr=(zigbeeSz_cfg_resp_i_t *)z_write_cfg;
						zcfg_wr->moduleAddr[0]=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[4];
						zcfg_wr->moduleAddr[1]=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[5];
						zcfg_wr->sendModel=SD_MS_CAST;
						zcfg_wr->freqDot=glbRtuPara.flashConfig.value.comFreq;
						memset(zgb_buf_temp,0,sizeof(zgb_buf_temp));
						ZigbeeDataOut(z_write_cfg,sizeof(z_write_cfg),1200);
						
					}
					else
						step=2;
				}
				else
				{			
					memset(zgb_buf_temp,0,sizeof(zgb_buf_temp));
					ZigbeeDataOut(z_read_cfg,sizeof(z_read_cfg),1200);		
					errCnt++;
					if(errCnt>5)
					{
						errCnt=0;
						XtBspDOCtrl(XT_BSP_DO_CFG, XT_BSP_DO_OP_CLOSE);
						PrintfXTOS(" zigbee close cfg\n");
						glbRtuPara.devPara.flag_dev_ini=ERR;
						TaskSleepMs(500);
						step=0;
					}
				}
			}
			break;
		case 2:
			XtBspDOCtrl(XT_BSP_DO_CFG, XT_BSP_DO_OP_CLOSE);
			memset(zgb_buf_temp,0,sizeof(zgb_buf_temp));
			ZigbeeDataOut(z_close_cfg,sizeof(z_close_cfg),100);					
			PrintfXTOS(" zigbee sz init ok\n");
			//XtBspZigbeeCfg(ZIGBEE_CFG_CLOSE);	
			//XtBspZigbeeRst();
			//glbRtuPara.devPara.flag_dev_ini=YES;	
			glbRtuPara.runningInfo.zigbee_err_chkTime=ZIGBEE_CHKTIME_MAX;
			glbRtuPara.runningInfo.flag_zibee_err=NO;
			step=0;
			break;
		default:
			break;
	}	
}

#else
xt_void  DevZigbeeSzInit(void)
{
	static xt_u8 step=0;
	xt_u8 len,i;
	#define ZIGBEE_CFG_CLOSE 1
	#define ZIGBEE_CFG_OPEN 0
	if(glbRtuPara.devPara.flag_dev_ini==YES)
		return ;
	#if 0
	glbRtuPara.devPara.flag_dev_ini=YES;	
	glbRtuPara.runningInfo.zigbee_err_chkTime=ZIGBEE_CHKTIME_MAX;
	glbRtuPara.runningInfo.flag_zibee_err=NO;
	return;
	#else
	//step=4;
	#endif
	switch(step)
	{
		case 0:
			XtBspZigbeeCfg(ZIGBEE_CFG_OPEN);
			TaskSleepMs(3000);
			step=1;		
			break;
		case 1:
			{
				memset(zgb_buf_temp,0,sizeof(zgb_buf_temp));
				ZigbeeDataOut(V5_read_cfg,sizeof(V5_read_cfg),800);	
				ZigbeeDataOut(V5_read_cfg,sizeof(V5_read_cfg),800);	
				ZigbeeDataOut(V5_read_cfg,sizeof(V5_read_cfg),1500);	
				
				step =2;
			}
			break;
		case 2:
			//zigbeeSzV5_cfg_req_i_t *zcfg=NULL;				
			for(i=0;i<40;i++)
			{
				if(zgb_buf_temp[i]==0x21)
					break;
			}
			
			/*
			if(i==40)
			{
				step=3;
				break;
			}
			zcfg=(zigbeeSzV5_cfg_req_i_t *)zgb_buf_temp[i];		
			if(zcfg>len!=25)
			{
				step=3;
				break;
			}
			if((zcfg->nettype<=NT_P2P)&&(zcfg->nodetype<=ND_END_DEVICE)&&(zcfg->sendModel<=SD_PROTOCOL_CAST))
			{	
				PrintfXTOS("zigbee uid: %0.2x %0.2x %0.2x %0.2x\n",zcfg->moduleAddr[0],zcfg->moduleAddr[1],zcfg->moduleAddr[2],zcfg->moduleAddr[3]);
				PrintfXTOS("dev uid:%0.2x %0.2x %0.2x %0.2x\n",glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[2],glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[3],
					glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[4],glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[5]);

				if((zcfg->moduleAddr[0]!=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[5])
					||(zcfg->moduleAddr[1]!=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[4])
					||(zcfg->moduleAddr[2]!=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[3])
					||(zcfg->moduleAddr[3]!=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[2]))
				{
					step=2;
				}					
			}*/
			step=4;
			break;
		case 3:
			/*
			zigbeeSz_cfg_resp_i_t *zcfg_wr= NULL;
			zcfg_wr=(zigbeeSz_cfg_resp_i_t *)z_write_cfg;
			zcfg_wr->moduleAddr[0]=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[4];
			zcfg_wr->moduleAddr[1]=glbRtuPara.flashConfig.value.rtuOrPlc_uid_own[5];
			zcfg_wr->sendModel=SD_MS_CAST;
			zcfg_wr->freqDot=glbRtuPara.flashConfig.value.comFreq;
			memset(zgb_buf_temp,0,sizeof(zgb_buf_temp));
			ZigbeeDataOut(z_write_cfg,sizeof(z_write_cfg),1000);*/
			break;
		case 4:
			XtBspDOCtrl(XT_BSP_DO_CFG, XT_BSP_DO_OP_CLOSE);
			//memset(zgb_buf_temp,0,sizeof(zgb_buf_temp));
			//ZigbeeDataOut(z_close_cfg,sizeof(z_close_cfg),100);					
			PrintfXTOS(" zigbee sz init ok\n");
			XtBspZigbeeCfg(ZIGBEE_CFG_CLOSE);	
			XtBspZigbeeRst();
			glbRtuPara.devPara.flag_dev_ini=YES;	
			glbRtuPara.runningInfo.zigbee_err_chkTime=ZIGBEE_CHKTIME_MAX;
			glbRtuPara.runningInfo.flag_zibee_err=NO;
			step=0;
			break;
		default:
			break;
	}	
}
#endif



xt_s32	DevZigbeeSzSend(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead)
{
	ctrl_data_in_t di = {0};
	xt_u32 j = 0;
	di.pDataBuf = pData;
	di.dataLen = (xt_u8)dataLen;
	
	//--------如是是sever UID cast ，no respond，针对广播和组播------
	if(is_uid_same(pDevParam,U_SERVER)==SUCCESS_XT)
		return FAILURE_XT;
	//--------如果不是终端UID，no respond，针对广播和组播-------
	if(is_uid_same(pDevParam,U_DEVICE)==FAILURE_XT)
		return FAILURE_XT;
	sendLen = BUFFER_LENGTH;
	ProDevIotPack(pDevParam->commDst, pDevParam->commCmd, &di, sendBuf, &sendLen);

	if (sendLen > 0)
	{
		if (pxWrite != NULL)
		{
			
			#ifdef PRINTF_REAL_PROTOCOL
			PrintfXTOS("----DevZigbeeSend : \n");
			for (j = 0; j < sendLen; j++) {
				PrintfXTOS("%0.2x ",sendBuf[j]);
			}
			PrintfXTOS("\n");			
			PrintfXTOS("\n----\n");	
			#endif
			
			#ifdef DEV_RSC_USE_ENCRYPT
			
			//MemMemsetXTOS(encryRaw, 0, BUFFER_LENGTH);
			MemMemsetXTOS(encryBuf, 0, BUFFER_LENGTH);
			//MemMemsetXTOS(encrySendbuf, 0, BUFFER_LENGTH);
			encrySendLen = BUFFER_LENGTH;
			encryLen = 0;

			//MemMemcpyXTOS(encryRaw, (xt_void *)sendBuf, sendLen);
			encryLen = (sendLen % 16 == 0) ? (sendLen / 16):(sendLen / 16 + 1);
			encryLen = encryLen * 16;

#if defined(ENCRY_LIB_SM4)
			IotSm4EncryptEcb(sendBuf, encryLen, encryBuf);
#elif defined(ENCRY_LIB_AES)
			IotAesEncryptEcb(sendBuf, encryLen, encryBuf);
#endif

			di.pDataBuf = encryBuf;
			di.dataLen = encryLen;

			ProDevIotPack(pDevParam->commDst, pDevParam->commCmd, &di,
				sendBuf, &encrySendLen);

			PrintfXTOS("----DevZigbeeSend : \n");
			for (j = 0; j < encrySendLen; j++) {
				PrintfXTOS("%0.2x ",sendBuf[j]);
			}
			PrintfXTOS("\n");			
			PrintfXTOS("\n----\n");
			return pxWrite(sendBuf, encrySendLen);
			#else	
			return pxWrite(sendBuf, sendLen);
			#endif
		}
		
	}
	return FAILURE_XT;
}

xt_s32	DevZigbeeSzRecv(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam)
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

	if(glbRtuPara.devPara.flag_dev_ini==NO)
	{
		MDBG("zigbee ini recv len=%0.2x:\n",dataLen);
		for(i=0;i<dataLen&&i<40;i++)
		{
			zgb_buf_temp[i]=pData[i];
			MDBG("%0.2x ", zgb_buf_temp[i]);
		}
		MDBG("\n");
		zgb_buf_len=dataLen;
		return FAILURE_XT;
	}	

	//---------------join different block recv
	//memset((xt_u8 *)&devZigbeeRecvData,0,sizeof(devZigbeeRecvData));
	/*
	if (devZigbeeRecvData.chkLen > 0)
	{
	#if 0
		//MDBG("=== DevZigbeeSzRecv copy chkLen=%d \n",devZigbeeRecvData.chkLen);
		for(j = 0; j < devZigbeeRecvData.chkLen; j++)
		{
			devZigbeeRecvData.chkBuf[j] = devZigbeeRecvData.chkBuf[devZigbeeRecvData.chkOff + j];
		}	
	}
	#else
		if((pData[0]!=0x68)&&(pData[1]!=0x00))//表示被截断
		{
			if(devZigbeeRecvData.chkLen+dataLen<BUFFER_LENGTH)
			{
				for(j = dataLen; j >=0; j--)
				{
					pData[devZigbeeRecvData.chkLen+j]=pData[j];
					if(j==0)
						break;
				}
				for(j = 0; j < devZigbeeRecvData.chkLen; j++)
				{
					pData[j]=devZigbeeRecvData.chkBuf[devZigbeeRecvData.chkOff + j];
				}	
				dataLen+=devZigbeeRecvData.chkLen;
				//PrintfXTOS("\npingjie:len=%d\n",dataLen);
				//for(i=0;i<dataLen;i++)
				//{
				//	PrintfXTOS("%0.2x ",pData[i]);
				//}
				//PrintfXTOS("\n");				
		  }		

		}
	}
	
	if((dataLen<8)&&(dataLen >0))
	{
		//----为拼接用，所以要拷贝
		//if (copyFlag == TRUE)
		{			
			MemMemsetXTOS(devZigbeeRecvData.chkBuf, 0, sizeof(devZigbeeRecvData.chkBuf));
			devZigbeeRecvData.chkLen=0;
			MemMemcpyXTOS(devZigbeeRecvData.chkBuf + devZigbeeRecvData.chkLen, pData, dataLen);
			devZigbeeRecvData.chkLen += dataLen;	
			//PrintfXTOS("less 8 bytes err\n");		
			copyFlag = TRUE;
		}		
		return FAILURE_XT;
	}
	else if(dataLen==0)
	{
		copyFlag = TRUE;
		return FAILURE_XT;
	}
	//-----------find avid data	
	for(i=0;i<dataLen-8;i++)
	{
		if((pData[i]==0x68)&&(pData[i+7]==0x68))
		{
			if((memcmp(&pData[i+1],glbRtuPara.flashConfig.value.sever_uid,sizeof(glbRtuPara.flashConfig.value.sever_uid))==0))
			{				
				getVailData=&pData[i];
				getVailDataLen=dataLen-i;
				//copyFlag = TRUE;
				//PrintfXTOS("is sever uid,start idx=%d vLen=%d\n",i,getVailDataLen);
				break;
			}
			if((memcmp(&pData[i+1],glbRtuPara.flashConfig.value.rtuOrPlc_uid_own,sizeof(glbRtuPara.flashConfig.value.rtuOrPlc_uid_own))==0))
			{				
				getVailData=&pData[i];
				getVailDataLen=dataLen-i;
				//copyFlag = TRUE;
				//PrintfXTOS("is device uid,start idx=%d vLen=%d\n",i,getVailDataLen);
				break;
			}
		}
	}
	//-----can't find avaid data
	if((getVailData==NULL)||((getVailDataLen<8)&&(getVailDataLen>8)))
	{
		//----为拼接用，所以要拷贝
		//if (copyFlag == TRUE)
		{			
			MemMemsetXTOS(devZigbeeRecvData.chkBuf, 0, sizeof(devZigbeeRecvData.chkBuf));
			devZigbeeRecvData.chkLen=0;
			MemMemcpyXTOS(devZigbeeRecvData.chkBuf + devZigbeeRecvData.chkLen, pData, dataLen);
			devZigbeeRecvData.chkLen += dataLen;	
			//PrintfXTOS("can't find vail data of mine chklen=%d\n",devZigbeeRecvData.chkLen);		
			copyFlag = TRUE;
		}		
		return FAILURE_XT;
	}	
	else if(getVailDataLen==0)
	{
		copyFlag = TRUE;
		return FAILURE_XT;
	}
	
	if (copyFlag == TRUE)
	{	
		if(devZigbeeRecvData.chkLen + getVailDataLen <= PRO_PKT_MAX_SIZE)
		{
			MemMemsetXTOS(devZigbeeRecvData.chkBuf, 0, sizeof(devZigbeeRecvData.chkBuf));
			devZigbeeRecvData.chkLen=0;		
			MemMemcpyXTOS(devZigbeeRecvData.chkBuf + devZigbeeRecvData.chkLen, getVailData, getVailDataLen);
			devZigbeeRecvData.chkLen += getVailDataLen;
		}
		
		//MDBG("DevZigbeeOnRecv: recv raw data = %d \n", getVailDataLen);		
		copyFlag = FALSE;
		
	}
	#endif
	*/
	#if 1
	if (copyFlag == TRUE)
	{
		if (devZigbeeRecvData.chkOff > 0) {
			MemMemcpyXTOS(devZigbeeRecvData.chkBuf, devZigbeeRecvData.chkBuf + devZigbeeRecvData.chkOff,
			devZigbeeRecvData.chkLen);

            devZigbeeRecvData.chkOff = 0;
		}
		if(devZigbeeRecvData.chkLen + dataLen <= PRO_PKT_MAX_SIZE)
		{
			MemMemcpyXTOS(devZigbeeRecvData.chkBuf + devZigbeeRecvData.chkLen, pData, dataLen);
			devZigbeeRecvData.chkLen += dataLen;
		}
		
		//MDBG("DevZigbeeOnRecv: recv raw data = %d \n", dataLen);		
		copyFlag = FALSE;
	}else {
        if (devZigbeeRecvData.chkLen == 0) {
			copyFlag = TRUE;
			return FAILURE_XT;
        }
    }
	#endif
PARSE:	
	if (ProDevIotParse(&devZigbeeRecvData) == SUCCESS_XT) {
		if (devZigbeeRecvData.pktLen == 0)
		{
			copyFlag = TRUE;
			return FAILURE_XT;
		}

		glbRtuPara.runningInfo.zigbee_err_chkTime=ZIGBEE_CHKTIME_MAX;
		glbRtuPara.runningInfo.flag_zibee_err=NO;
		#ifdef DEV_RSC_USE_ENCRYPT
		//MemMemsetXTOS(encryRaw, 0, BUFFER_LENGTH);
		MemMemsetXTOS(encryBuf, 0, BUFFER_LENGTH);
		encryLen = 0;

		pHead = (packet_head_t*)devZigbeeRecvData.pktBuf;
		if (pHead->cmd == CMD_UNICAST || pHead->cmd == CMD_BROADCAST ||pHead->cmd == CMD_CFG)
		{
			//MemMemcpyXTOS(encryRaw, (xt_void *)(devZigbeeRecvData.pktBuf + sizeof(packet_head_t)), pHead->len);
			
			encryLen = (pHead->len % 16 == 0) ? 
				(pHead->len / 16):(pHead->len / 16 + 1);
			encryLen = encryLen * 16;
			MDBG("pHead->len = %d encryLen=%d\n", pHead->len,encryLen);
			if(pHead->len < 16)
			{
				devZigbeeRecvData.pktLen = 0;
				copyFlag = TRUE;
				return FAILURE_XT;
			}

#if defined(ENCRY_LIB_SM4)
			IotSm4DecryptEcb(devZigbeeRecvData.pktBuf + sizeof(packet_head_t), encryLen, encryBuf);
#elif defined(ENCRY_LIB_AES)
			IotAesDecryptEcb(devZigbeeRecvData.pktBuf + sizeof(packet_head_t), encryLen, encryBuf);
#endif

			pHead = (packet_head_t*)encryBuf;

			if(UTCheckProData((packet_head_t*)encryBuf) != SUCCESS_XT){
				devZigbeeRecvData.pktLen = 0;
				copyFlag = TRUE;
				return FAILURE_XT;
			}
			pModBuf = encryBuf;
			modLen = pHead->len + PACK_MIN_SIZE;
		}
		else if ( pHead->cmd == CMD_GRPCAST)
		{
			//MemMemcpyXTOS(encryRaw, (xt_void *)(devZigbeeRecvData.pktBuf + sizeof(packet_head_t)+1), pHead->len-1);
			
			encryLen = ((pHead->len-1) % 16 == 0) ? 
				((pHead->len-1) / 16):((pHead->len-1) / 16 + 1);
			encryLen = encryLen * 16;
			MDBG("pHead->len = %d encryLen=%d\n", pHead->len-1,encryLen);
			if(pHead->len < 16)
			{
				devZigbeeRecvData.pktLen = 0;
				copyFlag = TRUE;
				return FAILURE_XT;
			}
			
#if defined(ENCRY_LIB_SM4)
			IotSm4DecryptEcb(devZigbeeRecvData.pktBuf + sizeof(packet_head_t)+1, encryLen, encryBuf);
#elif defined(ENCRY_LIB_AES)
			IotAesDecryptEcb(devZigbeeRecvData.pktBuf + sizeof(packet_head_t)+1, encryLen, encryBuf);
#endif
			pHead = (packet_head_t*)encryBuf;

			if(UTCheckProData((packet_head_t*)encryBuf) != SUCCESS_XT){
				devZigbeeRecvData.pktLen = 0;
				copyFlag = TRUE;
				return FAILURE_XT;
			}
			pModBuf = encryBuf;
			modLen = pHead->len -1+ PACK_MIN_SIZE;		
		}		
		else
		{
			#if 1
			copyFlag = TRUE;
			return FAILURE_XT;
			#else
			pModBuf = devZigbeeRecvData.pktBuf;
			modLen = devZigbeeRecvData.pktLen;
			#endif
		}
		#ifdef PRINTF_REAL_PROTOCOL
		MDBG("-----DevZigbeeOnRecv: recv packSize = %d ----\n", modLen+1);
		for(i=0;i<modLen+1;i++)
		{
			MDBG(" %0.2x",pModBuf[i]);
		}
		MDBG("\n----\n");		
		#endif
		#else
		pModBuf = devZigbeeRecvData.pktBuf;
		modLen = devZigbeeRecvData.pktLen;
		MDBG("DevZigbeeOnRecv: recv packSize = %d\n", modLen);
		#endif

		
		pHead = (packet_head_t*)pModBuf;
		pDevParam->commCmd = pHead->cmd;

		MemMemcpyXTOS((xt_void *)&pDevParam->commDst, (xt_void *)pHead->devaddr, DEV_ADDR_BYTES);
		pDevParam->commDst = pDevParam->commDst << 16; // uid store low 48 bit

		pSubCmdOffset = pModBuf + PACK_OFFSET_DATA;

		if (pHead->cmd == CMD_UNICAST ||pHead->cmd ==CMD_CFG)
		{
			pDevParam->commGroup = 0;
			
			MemMemcpyXTOS(pRetData, pSubCmdOffset, modLen - PACK_MIN_SIZE);
			*pRetDataLen = modLen - PACK_MIN_SIZE;
			//-----------单播，若终端uid不一样，则不动作---------
			
			if(is_uid_same(pDevParam,U_DEVICE)==FAILURE_XT)
			{
                		if (devZigbeeRecvData.chkLen > 0) {
                    			goto PARSE;
                		}			
				copyFlag = TRUE;
				devZigbeeRecvData.pktLen = 0;
				return FAILURE_XT;
			}
			ApRTUOnlineClearConnCount();
			glbRtuPara.runningInfo.zigbeePingPeirod=ZIGBEE_PING_PERIOD;
		}
		else if(pHead->cmd == CMD_BROADCAST)
		{
			pDevParam->commGroup = 0;

			MemMemcpyXTOS(pRetData, pSubCmdOffset, modLen - PACK_MIN_SIZE);
			*pRetDataLen = modLen - PACK_MIN_SIZE;
			if(is_uid_same(pDevParam,U_SERVER)==FAILURE_XT &&
				is_uid_same(pDevParam,U_DEVICE)==FAILURE_XT)
			{
                		if (devZigbeeRecvData.chkLen > 0) {
                    			goto PARSE;
               		 }            			
				copyFlag = TRUE;
				devZigbeeRecvData.pktLen = 0;
				return FAILURE_XT;
			}
			ApRTUOnlineClearConnCount();
			glbRtuPara.runningInfo.zigbeePingPeirod=ZIGBEE_PING_PERIOD;
		}
		else if(pHead->cmd == CMD_GRPCAST)
		{
			pDevParam->commGroup = *(pSubCmdOffset);
			pSubCmdOffset ++;

			if(is_uid_same(pDevParam,U_SERVER)==FAILURE_XT &&
				is_uid_same(pDevParam,U_DEVICE)==FAILURE_XT)
			{
                if (devZigbeeRecvData.chkLen > 0) {
                    goto PARSE;
                }			
				copyFlag = TRUE;
				devZigbeeRecvData.pktLen = 0;
				return FAILURE_XT;
			}
			ApRTUOnlineClearConnCount();

			MemMemcpyXTOS(pRetData, pSubCmdOffset, modLen - PACK_MIN_SIZE);
			*pRetDataLen = modLen  - PACK_MIN_SIZE;
			//--------------组播时，如果组号没有一个相同，则抛弃------------------
			if((pDevParam->commGroup!=glbRtuPara.flashConfig.value.rtuOrPlc_group[0])&&
				(pDevParam->commGroup!=glbRtuPara.flashConfig.value.rtuOrPlc_group[1])&&
				(pDevParam->commGroup!=glbRtuPara.flashConfig.value.rtuOrPlc_group[2]))
			{
				MDBG("group different own :%d,%d,%d vs %d\n", glbRtuPara.flashConfig.value.rtuOrPlc_group[0],glbRtuPara.flashConfig.value.rtuOrPlc_group[1],glbRtuPara.flashConfig.value.rtuOrPlc_group[2],pDevParam->commGroup);
                if (devZigbeeRecvData.chkLen > 0) {
                    goto PARSE;
                }				
				devZigbeeRecvData.pktLen = 0;
				copyFlag = TRUE;
				return FAILURE_XT;
			}
			else
				MDBG("group the same :%d\n",pDevParam->commGroup);
		}

		//todo: check for groupcast...

		devZigbeeRecvData.pktLen = 0;
		return SUCCESS_XT; 
	}
	else
	{
		copyFlag = TRUE;
		return FAILURE_XT;
	}	
}



