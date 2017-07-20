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
#include "xtos_sem.h"

#include "xt_bsp_rtu09c.h"
#include "global_para.h"
#include "drv_uart.h"

#include "conn_uart.h"

#include "xt_conn.h"
#include "xt_dev.h"

#include "pro_dev_iot.h"
#include "config.h"
#include "dev_solar_controller.h"
#include "dev_solar_phili.h"
#include "dev_solar_yuanfang.h"
#include "dev_solar_huineng.h"
#include "i2c_AT24LCxxx.h"
#include "solar_data_manager.h"
extern rtu_para_t glbRtuPara;
extern void delay_us(xt_u32 us);
extern uint16_t eeGet_targetByteBufIndexOrAddrOffset(uint8_t *beginByte_pt,uint8_t *targetByte_pt);
//extern uint32_t STM_EVAL_FlashEepromWriteByte(uint16_t Addr,uint8_t *Buffer,uint16_t Length);
//extern uint32_t STM_EVAL_FlashEepromReadByte(uint16_t Addr,uint8_t *Buffer,uint16_t Length);
#define apInnerFlashEerpomWriteByte STM_EVAL_FlashEepromWriteByte
#define apInnerFlashEerpomReadByte STM_EVAL_FlashEepromReadByte

os_mutex_t exEepromMutex = NULL;
solar_NMLdata_store_t solarDataStore;
solar_data_store_queue *solarDataStoreQ;
void SDM_test(void)
{
	xt_u8 i,temp[100]={0};
	static xt_u8 ff=0;
	PrintfXTOS("SDM_test:wr\n");
	for(i=0;i<50;i++)
		PrintfXTOS("%0.2x-%0.2x ",AT24LCxxx_WriteOneByte(AT24LCxxx_ADDR,i,ff==0?i+1:50-i),ff==0?i+1:50-i);
	PrintfXTOS("\nSDM_test:rd\n");
	for(i=0;i<50;i++)
	{
		PrintfXTOS("%0.2x-",AT24LCxxx_ReadOneByte(AT24LCxxx_ADDR,i,&temp[i]));
		PrintfXTOS("%0.2x ",temp[i]);
	}
	PrintfXTOS("\n......end test......\n");
	ff^=0x01;
}

//------------------------------------------------------------------------------------------------------
solar_data_store_queue* SolarQueueNew(xt_u8* buf,xt_u16 size)
{
	solar_data_store_queue * pNode = (solar_data_store_queue*)MemMallocXTOS(sizeof(solar_data_store_queue));
	//PrintfXTOS("SolarQueueNew\n");
	if (NULL == pNode) 
	{
		PrintfXTOS("malloc fail\n");
		return NULL;
	}

	pNode->qin= glbRtuPara.flashConfig.value.storeQueue_inNum;
	pNode->qout= glbRtuPara.flashConfig.value.storeQueue_OutNum;
	pNode->buffaddr= buf;
	pNode->buffsize = size;
	//PrintfXTOS("buffaddr=%x\n",pNode->buffaddr);
	return pNode;
}

xt_s32 SolarQueueIsEmpty(solar_data_store_queue* pQ)
{
	return (pQ->qin == pQ->qout);
}

xt_s32 SolarQueueIsFull(solar_data_store_queue* pQ)
{
	return (((pQ->qin+1)%pQ->buffsize)== pQ->qout);
}

xt_s32 SolarQueueFree(solar_data_store_queue* pQ)
{

	MemFreeXTOS(pQ);
	return TRUE;
}

xt_s32 SolarQueueWriteIn(solar_data_store_queue* pQ, xt_u8* pData,xt_u8 len)
{
	//if(SolarQueueIsFull(pQ)==TRUE)
	//	return FALSE;
	xt_u8 i,j,retryNum=2,err_cnt=0,temp;
	xt_u8 *bufT=pData;//pQ->buffaddr;
	xt_u16 addrBase=SDM_EEPROM_START_ADDR+pQ->qin*SDM_EACH_PAKAGE_SIZE;
	//PrintfXTOS("&pQ->buffaddr[pQ->qin]=%x\n",&pQ->buffaddr[pQ->qin]);
	xt_u8 buf_offset=0;
	xt_u8 storeQueue_inNum_temp[2]={0};

	
	PrintfXTOS("SolarQueueWriteIn:addr=0x%x,qout=%d,qin=%d,eeaddrBase=%x\n",pQ->buffaddr,pQ->qout,pQ->qin,addrBase);
	for(i=0;i<len;i++)
	{
		//*(bufT+i)=i;
		PrintfXTOS("%.2x ",*(bufT+i));
	}
	PrintfXTOS("\n");
	//--------------wr eerpom now----------------------
	/*
	if (exEepromMutex == NULL)
	{
		MutexCreateXTOS(exEepromMutex);
		if(exEepromMutex == NULL)
		{
			PrintfXTOS("create exEepromMutex err\n");
			return FALSE;			
		}
	}
	MutexLockXTOS(exEepromMutex);*/
	for(j=0;j<retryNum;j++)
	{
		err_cnt=0;
		for(i=0;i<len;i++)
		{
			temp=AT24LCxxx_WriteOneByte(AT24LCxxx_ADDR,addrBase+i,*(bufT+i));
			err_cnt+=temp;
			//两次写之间要延时大于4ms，否则会写失败......!!!!!!!.......
			//PrintfXTOS("%0.2x-%0.2x ",temp,err_cnt);
			delay_us(1000*4);
		}	
		if(err_cnt==0)
		{
			break;
		}
		PrintfXTOS("\n");
	}
	//MutexUnlockXTOS(exEepromMutex);
	if(err_cnt!=0)
	{
		PrintfXTOS("wr ex eeprom fail\n");
		return FALSE;
	}
	PrintfXTOS("wr ex eeprom success\n");
	//------------指针指向下个包---------------
	pQ->qin++;
	if(pQ->qin>=pQ->buffsize)
		pQ->qin = 0 ;
	glbRtuPara.flashConfig.value.storeQueue_inNum=pQ->qin;
	//------------写到内部EEPROM--------------
	//------------------wr flash------------------
	buf_offset=eeGet_targetByteBufIndexOrAddrOffset(glbRtuPara.flashConfig.buf,(xt_u8 *)&glbRtuPara.flashConfig.value.storeQueue_inNum);
	apInnerFlashEerpomWriteByte(buf_offset,&glbRtuPara.flashConfig.buf[buf_offset],2);
	apInnerFlashEerpomReadByte(buf_offset,storeQueue_inNum_temp,2);
	if(memcmp(&glbRtuPara.flashConfig.buf[buf_offset],storeQueue_inNum_temp,2)==0)
	{
		PrintfXTOS("wr storeQueue_inNum success\n");
	}
	else
	{
		PrintfXTOS("wr storeQueue_inNum fail\n");
	}	
	return TRUE;

}

xt_s32 SolarQueueReadOut(solar_data_store_queue* pQ,xt_u8 len)
{
	//static xt_u8 qOutNum=0xff;
	xt_u8 i,j,retryNum=3,retErr=0;
	xt_u8 *bufT=(xt_u8 *)pQ->buffaddr;	
	xt_u16 addrBase=0;
	
	if (SolarQueueIsEmpty(pQ)==TRUE)
		return NULL;

	
	//------------计算eeprom读基地址----------
	addrBase=SDM_EEPROM_START_ADDR+pQ->qout*SDM_EACH_PAKAGE_SIZE;
	PrintfXTOS("SolarQueueReadOut:sdmT addr= %x,qout=%d,qin=%d,eeaddrBase=%x\n",bufT,pQ->qout,pQ->qin,addrBase);
	//------------读EEPROM，到bufT数组中-----
	//if(pQ->qout==qOutNum)//已经读成功过，不用再读
	//	return TRUE;
	/*
	if (exEepromMutex == NULL)
	{
		MutexCreateXTOS(exEepromMutex);
		if(exEepromMutex == NULL)
		{
			PrintfXTOS("create exEepromMutex err\n");
			return FALSE;			
		}
	}	
	MutexLockXTOS(exEepromMutex);
	*/
	for(j=0;j<retryNum;j++)
	{
		for(i=0;i<len;i++)
		{			
			//两次读，可以不用延时
			retErr=AT24LCxxx_ReadOneByte(AT24LCxxx_ADDR,addrBase+i,&bufT[i]);			
			if(retErr!=0)
			{
				PrintfXTOS("retErr=%0.2x\n",retErr);
				break;
			}
		}	
		if(retErr==0)
		{
			break;
		}
	}
	//MutexUnlockXTOS(exEepromMutex);
	if(retErr!=0)
	{
		PrintfXTOS("rd ex eeprom fail\n");
		return FALSE;
	}
	PrintfXTOS("rd ex eeprom success\n");
	//qOutNum=pQ->qout;
	//---------------debug printf read data---------------
	for(i=0;i<len;i++)
	{
		PrintfXTOS("%0.2x ",*(bufT+i));
	}
	PrintfXTOS("\n");	
	return TRUE;
}

xt_u32 SolarQueueDel(solar_data_store_queue* pQ)
{
	xt_u8 buf_offset=0;
	xt_u8 storeQueue_OutNum_temp[2]={0};
	PrintfXTOS("SolarQueueDel \n");
	if (SolarQueueIsEmpty(pQ)==TRUE)
		return FALSE;
	pQ->qout++;
	PrintfXTOS("DEL qout=%d qin=%d\n",pQ->qout,pQ->qin);
	if(pQ->qout>=pQ->buffsize)
		pQ->qout = 0 ;
	glbRtuPara.flashConfig.value.storeQueue_OutNum=pQ->qout;
	//------------写到内部EEPROM--------------
	//------------------wr flash------------------
	buf_offset=eeGet_targetByteBufIndexOrAddrOffset(glbRtuPara.flashConfig.buf,(xt_u8 *)&glbRtuPara.flashConfig.value.storeQueue_OutNum);
	apInnerFlashEerpomWriteByte(buf_offset,&glbRtuPara.flashConfig.buf[buf_offset],2);
	apInnerFlashEerpomReadByte(buf_offset,storeQueue_OutNum_temp,2);
	if(memcmp(&glbRtuPara.flashConfig.buf[buf_offset],storeQueue_OutNum_temp,2)==0)
	{
		PrintfXTOS("wr storeQueue_OutNum success\n");
	}
	else
	{
		PrintfXTOS("wr storeQueue_OutNum fail\n");
	}	
	return TRUE;
}


xt_u32 SloarDataStoreWrtie(xt_u8 *data,xt_u8 len)
{
	if(solarDataStoreQ== NULL)
		solarDataStoreQ = SolarQueueNew((xt_u8*)&solarDataStore, SDM_MAX_PAKAGES_SIZE);
	if(SolarQueueWriteIn(solarDataStoreQ, data,len)==TRUE)
		return TRUE;
	return FALSE;
}


xt_u8 * SloarDataStoreRead(xt_u8 len)
{
	if(solarDataStoreQ == NULL)
		solarDataStoreQ = SolarQueueNew((xt_u8*)&solarDataStore, SDM_MAX_PAKAGES_SIZE);
	if(SolarQueueReadOut(solarDataStoreQ,len)==TRUE)
	{
		return (xt_u8*)(solarDataStoreQ->buffaddr);
	}
	return NULL;
}

xt_void SloarDataStoreDel(void)
{
	SolarQueueDel(solarDataStoreQ);
}

xt_void SloarDataStoreReset(void)
{
	xt_u8 buf_offset=0;
	xt_u8 storeQueue_Num_temp[2]={0};
	solarDataStoreQ->qin=0;
	solarDataStoreQ->qout=0;
	glbRtuPara.flashConfig.value.storeQueue_inNum=0;
	glbRtuPara.flashConfig.value.storeQueue_OutNum=0;
	//------------------wr flash------------------
	buf_offset=eeGet_targetByteBufIndexOrAddrOffset(glbRtuPara.flashConfig.buf,(xt_u8 *)&glbRtuPara.flashConfig.value.storeQueue_inNum);
	apInnerFlashEerpomWriteByte(buf_offset,&glbRtuPara.flashConfig.buf[buf_offset],2);
	apInnerFlashEerpomReadByte(buf_offset,storeQueue_Num_temp,2);	
	if(memcmp(&glbRtuPara.flashConfig.buf[buf_offset],storeQueue_Num_temp,2)==0)
	{
		PrintfXTOS("wr storeQueue_inNum success\n");
	}
	else
	{
		PrintfXTOS("wr storeQueue_inNum fail\n");
	}	

	buf_offset=eeGet_targetByteBufIndexOrAddrOffset(glbRtuPara.flashConfig.buf,(xt_u8 *)&glbRtuPara.flashConfig.value.storeQueue_OutNum);
	apInnerFlashEerpomWriteByte(buf_offset,&glbRtuPara.flashConfig.buf[buf_offset],2);
	apInnerFlashEerpomReadByte(buf_offset,storeQueue_Num_temp,2);	
	if(memcmp(&glbRtuPara.flashConfig.buf[buf_offset],storeQueue_Num_temp,2)==0)
	{
		PrintfXTOS("wr storeQueue_outNum success\n");
	}
	else
	{
		PrintfXTOS("wr storeQueue_outNum fail\n");
	}	
}
#ifdef __cplusplus
}
#endif // End for __cplusplus
