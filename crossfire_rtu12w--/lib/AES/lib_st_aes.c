/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include "lib_st_aes.h"

//#define PRINTF_ON

#ifdef PRINTF_ON
#define OS_DEBUG     PrintfXTOS
#else
#define OS_DEBUG(...)     do{}while(0)
#endif

static uint8_t iotKeys[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};

static void preset(void)
{

}

#if defined (LIB_USE_EXT_CRYPE)
#include "xtsys_type.h"
#include "xtos_task.h"
#include "xtos_mem.h"

#include "smartcard.h"

/* APDU Transport Structures */
SC_ADPU_Commands SC_ADPU;
SC_ADPU_Responce SC_Responce;

const uint8_t aes128EncConfig[5] = {0x30, 0xf0, 0xf0, 0x01, 0x02};

const uint8_t aes128DecConfig[5] = {0x31, 0xf0, 0xf0, 0x01, 0x02};

const uint8_t SM4EncConfig[5] = {0x30, 0xf0, 0xf0, 0x01, 0x04};

const uint8_t SM4DecConfig[5] = {0x31, 0xf0, 0xf0, 0x01, 0x04};

const uint8_t keyCreateConfig[7] = {
	0x3f, 0x00, 0x80, 0xff, 0xf0, 0xff, 0xff};

SC_State SCState = SC_POWER_OFF;
static xt_u8 SCInitSt = 0;
static xt_u8 SCPPSSt = 0;
static xt_u8 SCERRCNT = 0;

static xt_void SCInit(xt_void)
{
	OS_DEBUG("^^^^^ SCInit ^^^^^\n");

	preset();
	
	SC_IOConfig();
		
	/* Wait A2R --------------------------------------------------------------*/
    SCState = SC_POWER_ON;

    SC_ADPU.Header.CLA = 0x00;
    SC_ADPU.Header.INS = SC_GET_A2R;
    SC_ADPU.Header.P1 = 0x00;
    SC_ADPU.Header.P2 = 0x00;
    SC_ADPU.Body.LC = 0x00;

	SCPPSSt = 0;
	SCERRCNT = 0;
}

static xt_void SCCheck()
{
	uint32_t i = 0;
	
	if(SCInitSt == 0)
	{
		SCInit();
		SCInitSt = 1;
	}

	while(SCState != SC_ACTIVE_ON_T0) 
	{
		OS_DEBUG("^^^^^ SCCheck handler before = %d ^^^^^\n", SCState);
		vTaskSuspendAll();
		SC_Handler(&SCState, &SC_ADPU, &SC_Responce);
		xTaskResumeAll();
		OS_DEBUG("^^^^^ SCCheck handler after = %d ^^^^^\n", SCState);
	}

	if(SCPPSSt == 0)
	{
		OS_DEBUG("^^^^^ SCCheck SC_PTSConfig ^^^^^\n");
		/* Apply the Procedure Type Selection (PTS) */
		//__set_FAULTMASK(1);
		vTaskSuspendAll();
		SC_PTSConfigCritical();
		//__set_FAULTMASK(0);
		xTaskResumeAll();

		/* Inserts delay(400ms) for Smartcard clock resynchronisation */
		TaskSleepMs(400);

#if 1
		OS_DEBUG("^^^^^ SCCheck TEST key erase!!! ^^^^^\n");
		
		/* Select MF -------------------------------------------------------------*/
		SC_ADPU.Header.CLA = 0x80;
		SC_ADPU.Header.INS = 0x0e;
		SC_ADPU.Header.P1 = 0x00;
		SC_ADPU.Header.P2 = 0x00;
		SC_ADPU.Body.LC = 0x00;

		i = 0;
		while(i < LC_MAX) 
		{    
			SC_ADPU.Body.Data[i++] = 0;
		}
		SC_ADPU.Body.LE = 0;

		MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
		vTaskSuspendAll();
		SC_SendDataCritical(&SC_ADPU, &SC_Responce);
		xTaskResumeAll();

		OS_DEBUG("SC resp erase = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);

		if(SC_Responce.SW1 != 0x90 || SC_Responce.SW2!= 0x0)
		{
			return;
		}
#endif

#if 1
		TaskSleepMs(1);

		OS_DEBUG("^^^^^ SCCheck TEST key create!!! ^^^^^\n");
		
		/* Select MF -------------------------------------------------------------*/
		SC_ADPU.Header.CLA = 0x80;
		SC_ADPU.Header.INS = 0xe0;
		SC_ADPU.Header.P1 = 0x00;
		SC_ADPU.Header.P2 = 0x00;
		SC_ADPU.Body.LC = 0x07;

		for(i = 0; i < SC_ADPU.Body.LC; i++)
		{
			SC_ADPU.Body.Data[i] = keyCreateConfig[i];
		}
		while(i < LC_MAX) 
		{    
			SC_ADPU.Body.Data[i++] = 0;
		}
		SC_ADPU.Body.LE = 0;

		MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
		vTaskSuspendAll();
		SC_SendDataCritical(&SC_ADPU, &SC_Responce);
		xTaskResumeAll();

		OS_DEBUG("SC resp create = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);

		if(SC_Responce.SW1 != 0x90 || SC_Responce.SW2!= 0x0)
		{
			return;
		}
#endif

#if 1
		TaskSleepMs(1);

		OS_DEBUG("^^^^^ SCCheck TEST AES128 enc cfg!!! ^^^^^\n");
		
		/* Select MF -------------------------------------------------------------*/
		SC_ADPU.Header.CLA = 0x80;
		SC_ADPU.Header.INS = 0xd4;
		SC_ADPU.Header.P1 = 0x01;
		SC_ADPU.Header.P2 = 0x01;
		SC_ADPU.Body.LC = 0x15;

		for(i = 0; i < SC_ADPU.Body.LC; i++)
		{
			if(i < 5)
				SC_ADPU.Body.Data[i] = aes128EncConfig[i];
			else
				SC_ADPU.Body.Data[i] = iotKeys[i - 5];
		}
		while(i < LC_MAX) 
		{    
			SC_ADPU.Body.Data[i++] = 0;
		}
		SC_ADPU.Body.LE = 0;

		MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
		vTaskSuspendAll();
		SC_SendDataCritical(&SC_ADPU, &SC_Responce);
		xTaskResumeAll();
		
		OS_DEBUG("SC resp enc cfg = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
		if(SC_Responce.SW1 != 0x90 || SC_Responce.SW2!= 0x0)
		{
			return;
		}
#endif

#if 1
		TaskSleepMs(1);

		OS_DEBUG("^^^^^ SCCheck TEST AES128 dec cfg!!! ^^^^^\n");
		
		/* Select MF -------------------------------------------------------------*/
		SC_ADPU.Header.CLA = 0x80;
		SC_ADPU.Header.INS = 0xd4;
		SC_ADPU.Header.P1 = 0x01;
		SC_ADPU.Header.P2 = 0x01;
		SC_ADPU.Body.LC = 0x15;

		for(i = 0; i < SC_ADPU.Body.LC; i++)
		{
			if(i < 5)
				SC_ADPU.Body.Data[i] = aes128DecConfig[i];
			else
				SC_ADPU.Body.Data[i] = iotKeys[i - 5];
		}
		while(i < LC_MAX) 
		{    
			SC_ADPU.Body.Data[i++] = 0;
		}
		SC_ADPU.Body.LE = 0;

		MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
		vTaskSuspendAll();
		SC_SendDataCritical(&SC_ADPU, &SC_Responce);
		xTaskResumeAll();
		
		OS_DEBUG("SC resp dec cfg = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
		if(SC_Responce.SW1 != 0x90 || SC_Responce.SW2!= 0x0)
		{
			return;
		}
#endif

#if 1
		TaskSleepMs(1);

		OS_DEBUG("^^^^^ SCCheck TEST SM4 enc cfg!!! ^^^^^\n");
		
		/* Select MF -------------------------------------------------------------*/
		SC_ADPU.Header.CLA = 0x80;
		SC_ADPU.Header.INS = 0xd4;
		SC_ADPU.Header.P1 = 0x01;
		SC_ADPU.Header.P2 = 0x02;
		SC_ADPU.Body.LC = 0x15;

		for(i = 0; i < SC_ADPU.Body.LC; i++)
		{
			if(i < 5)
				SC_ADPU.Body.Data[i] = SM4EncConfig[i];
			else
				SC_ADPU.Body.Data[i] = iotKeys[i - 5];
		}
		while(i < LC_MAX) 
		{    
			SC_ADPU.Body.Data[i++] = 0;
		}
		SC_ADPU.Body.LE = 0;

		MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
		vTaskSuspendAll();
		SC_SendDataCritical(&SC_ADPU, &SC_Responce);
		xTaskResumeAll();
		
		OS_DEBUG("SC resp enc cfg = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
		if(SC_Responce.SW1 != 0x90 || SC_Responce.SW2!= 0x0)
		{
			return;
		}
#endif

#if 1
		TaskSleepMs(1);

		OS_DEBUG("^^^^^ SCCheck TEST SM4 dec cfg!!! ^^^^^\n");
		
		/* Select MF -------------------------------------------------------------*/
		SC_ADPU.Header.CLA = 0x80;
		SC_ADPU.Header.INS = 0xd4;
		SC_ADPU.Header.P1 = 0x01;
		SC_ADPU.Header.P2 = 0x02;
		SC_ADPU.Body.LC = 0x15;

		for(i = 0; i < SC_ADPU.Body.LC; i++)
		{
			if(i < 5)
				SC_ADPU.Body.Data[i] = SM4DecConfig[i];
			else
				SC_ADPU.Body.Data[i] = iotKeys[i - 5];
		}
		while(i < LC_MAX) 
		{    
			SC_ADPU.Body.Data[i++] = 0;
		}
		SC_ADPU.Body.LE = 0;

		MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
		vTaskSuspendAll();
		SC_SendDataCritical(&SC_ADPU, &SC_Responce);
		xTaskResumeAll();

		OS_DEBUG("SC resp dec cfg = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
		if(SC_Responce.SW1 != 0x90 || SC_Responce.SW2!= 0x0)
		{
			return;
		}
#endif

		SCPPSSt = 1;
	}
}

static ErrorStatus SCAESEnc16Byte(xt_u8* pInData, xt_u8* pOutData)
{
	uint32_t i = 0;

	SCCheck();
	
	TaskSleepMs(1);

	OS_DEBUG("^^^^^ SCAESEnc16Byte TEST AES128 enc !!! ^^^^^\n");
	
	SC_ADPU.Header.CLA = 0x00;
	SC_ADPU.Header.INS = 0x88;
	SC_ADPU.Header.P1 = 0x00;
	SC_ADPU.Header.P2 = 0x01;
	SC_ADPU.Body.LC = 0x10;

	for(i = 0; i < SC_ADPU.Body.LC; i++)
	{
		SC_ADPU.Body.Data[i] = pInData[i];
	}
	while(i < LC_MAX) 
	{    
		SC_ADPU.Body.Data[i++] = 0;
	}
	SC_ADPU.Body.LE = 0;

	MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
	vTaskSuspendAll();
	SC_SendDataCritical(&SC_ADPU, &SC_Responce);
	xTaskResumeAll();

	OS_DEBUG("SC resp raw = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
	
	if(SC_Responce.SW1 != 0x61 || SC_Responce.SW2!= 0x10)
	{
		OS_DEBUG("^^^^^ SC enc cmd ERROR !!! ^^^^^\n");
		
		return ERROR;
	}

	TaskSleepMs(1);
	
	SC_ADPU.Header.CLA = SC_CLA_LKT4201N;
	SC_ADPU.Header.INS = SC_GET_RESPONCE;
	SC_ADPU.Header.P1 = 0x00;
	SC_ADPU.Header.P2 = 0x00;
	SC_ADPU.Body.LC = 0x00;
	SC_ADPU.Body.LE = SC_Responce.SW2;

	MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
	vTaskSuspendAll();
	SC_SendDataCritical(&SC_ADPU, &SC_Responce);
	xTaskResumeAll();
	
	OS_DEBUG("SC resp res = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
	
	if(SC_Responce.SW1 != 0x90 || SC_Responce.SW2!= 0x0)
	{
		OS_DEBUG("^^^^^ SC enc data ERROR !!! ^^^^^\n");
		
		return ERROR;
	}

	for(i = 0; i < 16; i++)
	{
		pOutData[i] = SC_Responce.Data[i];
	}
	
	return SUCCESS;
}

static ErrorStatus SCAESDec16Byte(xt_u8* pInData, xt_u8* pOutData)
{
	uint32_t i = 0;

	SCCheck();
	
	TaskSleepMs(1);

	OS_DEBUG("^^^^^ SCAESDec16Byte TEST AES128 dec !!! ^^^^^\n");
	
	SC_ADPU.Header.CLA = 0x00;
	SC_ADPU.Header.INS = 0x88;
	SC_ADPU.Header.P1 = 0x01;
	SC_ADPU.Header.P2 = 0x01;
	SC_ADPU.Body.LC = 0x10;

	for(i = 0; i < SC_ADPU.Body.LC; i++)
	{
		SC_ADPU.Body.Data[i] = pInData[i];
	}
	while(i < LC_MAX) 
	{    
		SC_ADPU.Body.Data[i++] = 0;
	}
	SC_ADPU.Body.LE = 0;

	MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
	vTaskSuspendAll();
	SC_SendDataCritical(&SC_ADPU, &SC_Responce);
	xTaskResumeAll();

	OS_DEBUG("SC resp raw = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
	
	if(SC_Responce.SW1 != 0x61 || SC_Responce.SW2!= 0x10)
	{
		OS_DEBUG("^^^^^ SC dec cmd ERROR !!! ^^^^^\n");
		
		return ERROR;
	}

	TaskSleepMs(1);
	
	SC_ADPU.Header.CLA = SC_CLA_LKT4201N;
	SC_ADPU.Header.INS = SC_GET_RESPONCE;
	SC_ADPU.Header.P1 = 0x00;
	SC_ADPU.Header.P2 = 0x00;
	SC_ADPU.Body.LC = 0x00;
	SC_ADPU.Body.LE = SC_Responce.SW2;

	MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
	vTaskSuspendAll();
	SC_SendDataCritical(&SC_ADPU, &SC_Responce);
	xTaskResumeAll();
	
	OS_DEBUG("SC resp res = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
	
	if(SC_Responce.SW1 != 0x90 || SC_Responce.SW2!= 0x0)
	{
		OS_DEBUG("^^^^^ SC dec data ERROR !!! ^^^^^\n");
		
		return ERROR;
	}

	for(i = 0; i < 16; i++)
	{
		pOutData[i] = SC_Responce.Data[i];
	}
	
	return SUCCESS;
}

static ErrorStatus SCSM4Enc16Byte(xt_u8* pInData, xt_u8* pOutData)
{
	uint32_t i = 0;

	SCCheck();
	
	TaskSleepMs(1);

	OS_DEBUG("^^^^^ SCSM4Enc16Byte TEST SM4 enc !!! ^^^^^\n");
	
	SC_ADPU.Header.CLA = 0x00;
	SC_ADPU.Header.INS = 0x88;
	SC_ADPU.Header.P1 = 0x00;
	SC_ADPU.Header.P2 = 0x02;
	SC_ADPU.Body.LC = 0x10;

	for(i = 0; i < SC_ADPU.Body.LC; i++)
	{
		SC_ADPU.Body.Data[i] = pInData[i];
	}
	while(i < LC_MAX) 
	{    
		SC_ADPU.Body.Data[i++] = 0;
	}
	SC_ADPU.Body.LE = 0;

	MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
	vTaskSuspendAll();
	SC_SendDataCritical(&SC_ADPU, &SC_Responce);
	xTaskResumeAll();

	OS_DEBUG("SC resp raw = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
	
	if(SC_Responce.SW1 != 0x61 || SC_Responce.SW2!= 0x10)
	{
		OS_DEBUG("^^^^^ SC enc cmd ERROR !!! ^^^^^\n");
		
		return ERROR;
	}

	TaskSleepMs(1);
	
	SC_ADPU.Header.CLA = SC_CLA_LKT4201N;
	SC_ADPU.Header.INS = SC_GET_RESPONCE;
	SC_ADPU.Header.P1 = 0x00;
	SC_ADPU.Header.P2 = 0x00;
	SC_ADPU.Body.LC = 0x00;
	SC_ADPU.Body.LE = SC_Responce.SW2;

	MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
	vTaskSuspendAll();
	SC_SendDataCritical(&SC_ADPU, &SC_Responce);
	xTaskResumeAll();
	
	OS_DEBUG("SC resp res = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
	
	if(SC_Responce.SW1 != 0x90 || SC_Responce.SW2!= 0x0)
	{
		OS_DEBUG("^^^^^ SC enc data ERROR !!! ^^^^^\n");
		
		return ERROR;
	}

	for(i = 0; i < 16; i++)
	{
		pOutData[i] = SC_Responce.Data[i];
	}
	
	return SUCCESS;
}

static ErrorStatus SCSM4Dec16Byte(xt_u8* pInData, xt_u8* pOutData)
{
	uint32_t i = 0;

	SCCheck();
	
	TaskSleepMs(1);

	OS_DEBUG("^^^^^ SCSM4Dec16Byte TEST SM4 dec !!! ^^^^^\n");
	
	SC_ADPU.Header.CLA = 0x00;
	SC_ADPU.Header.INS = 0x88;
	SC_ADPU.Header.P1 = 0x01;
	SC_ADPU.Header.P2 = 0x02;
	SC_ADPU.Body.LC = 0x10;

	for(i = 0; i < SC_ADPU.Body.LC; i++)
	{
		SC_ADPU.Body.Data[i] = pInData[i];
	}
	while(i < LC_MAX) 
	{    
		SC_ADPU.Body.Data[i++] = 0;
	}
	SC_ADPU.Body.LE = 0;

	MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
	vTaskSuspendAll();
	SC_SendDataCritical(&SC_ADPU, &SC_Responce);
	xTaskResumeAll();

	OS_DEBUG("SC resp raw = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
	
	if(SC_Responce.SW1 != 0x61 || SC_Responce.SW2!= 0x10)
	{
		OS_DEBUG("^^^^^ SC dec cmd ERROR !!! ^^^^^\n");
		
		return ERROR;
	}

	TaskSleepMs(1);
	
	SC_ADPU.Header.CLA = SC_CLA_LKT4201N;
	SC_ADPU.Header.INS = SC_GET_RESPONCE;
	SC_ADPU.Header.P1 = 0x00;
	SC_ADPU.Header.P2 = 0x00;
	SC_ADPU.Body.LC = 0x00;
	SC_ADPU.Body.LE = SC_Responce.SW2;

	MemMemsetXTOS(&SC_Responce, 0,sizeof(SC_Responce));
	vTaskSuspendAll();
	SC_SendDataCritical(&SC_ADPU, &SC_Responce);
	xTaskResumeAll();
	
	OS_DEBUG("SC resp res = 0x%x 0x%x\n", SC_Responce.SW1, SC_Responce.SW2);
	
	if(SC_Responce.SW1 != 0x90 || SC_Responce.SW2!= 0x0)
	{
		OS_DEBUG("^^^^^ SC dec data ERROR !!! ^^^^^\n");
		
		return ERROR;
	}

	for(i = 0; i < 16; i++)
	{
		pOutData[i] = SC_Responce.Data[i];
	}
	
	return SUCCESS;
}

ErrorStatus IotSm4EncryptEcb(uint8_t* Input, uint32_t Ilength, uint8_t* Output)
{
	xt_u8 loop = Ilength / 16;
	xt_u8 i = 0, j = 0;
	ErrorStatus ret = SUCCESS;
	
	for(i = 0; i < loop; i++)
	{
		for(j = 0; j < 3 ;j ++)
		{
			if(SUCCESS == SCSM4Enc16Byte(Input + 16 * i, Output + 16 * i))
			{
				SCERRCNT = 0;
				break;
			}
			else
			{
				SCERRCNT ++;
				if(SCERRCNT >= 2)
				{
					SCInitSt = 0;
				}
			}
		}

		if(j >= 3)
		{
			ret = ERROR;
		}
	}

	return ret;
}

ErrorStatus IotSm4DecryptEcb(uint8_t* Input, uint32_t Ilength, uint8_t* Output)
{
	xt_u8 loop = Ilength / 16;
	xt_u8 i = 0, j = 0;
	ErrorStatus ret = SUCCESS;
	
	for(i = 0; i < loop; i++)
	{
		for(j = 0; j < 3 ;j ++)
		{
			if(SUCCESS == SCSM4Dec16Byte(Input + 16 * i, Output + 16 * i))
			{
				SCERRCNT = 0;
				break;
			}
			else
			{
				SCERRCNT ++;
				if(SCERRCNT >= 2)
				{
					SCInitSt = 0;
				}
			}
		}

		if(j >= 3)
		{
			ret = ERROR;
		}
	}

	return ret;
}

#endif

ErrorStatus IotAesEncryptEcb(uint8_t* Input, uint32_t Ilength, uint8_t* Output)
{
#if defined (LIB_USE_MCU_CRYPE)
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_AES, ENABLE);

	preset();
	
	return AES_ECB_Encrypt(iotKeys, 
                         Input, Ilength, Output);
#elif defined (LIB_USE_EXT_CRYPE)
	xt_u8 loop = Ilength / 16;
	xt_u8 i = 0, j = 0;
	ErrorStatus ret = SUCCESS;
	
	for(i = 0; i < loop; i++)
	{
		for(j = 0; j < 3 ;j ++)
		{
			if(SUCCESS == SCAESEnc16Byte(Input + 16 * i, Output + 16 * i))
			{
				SCERRCNT = 0;
				break;
			}
			else
			{
				SCERRCNT ++;
				if(SCERRCNT >= 2)
				{
					SCInitSt = 0;
				}
			}
		}

		if(j >= 3)
		{
			ret = ERROR;
		}
	}

	return ret;
#endif
}

ErrorStatus IotAesDecryptEcb(uint8_t* Input, uint32_t Ilength, uint8_t* Output)
{
#if defined (LIB_USE_MCU_CRYPE)
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_AES, ENABLE);

	preset();
	
	return AES_ECB_Decrypt(iotKeys, 
                         Input, Ilength, Output);
#elif defined (LIB_USE_EXT_CRYPE)
	xt_u8 loop = Ilength / 16;
	xt_u8 i = 0, j = 0;
	ErrorStatus ret = SUCCESS;
	
	for(i = 0; i < loop; i++)
	{
		for(j = 0; j < 3 ;j ++)
		{
			if(SUCCESS == SCAESDec16Byte(Input + 16 * i, Output + 16 * i))
			{
				SCERRCNT = 0;
				break;
			}
			else
			{
				SCERRCNT ++;
				if(SCERRCNT >= 2)
				{
					SCInitSt = 0;
				}
			}
		}

		if(j >= 3)
		{
			ret = ERROR;
		}
	}

	return ret;
#endif
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
