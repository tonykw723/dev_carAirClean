#ifndef _KEY_H
#define _KEY_H

#define GPIO_KEY_POWER_PIN               GPIO_Pin_8
#define GPIO_KEY_POWER_PORT             GPIOA
#define GPIO_KEY_POWER_CLK               RCC_AHBPeriph_GPIOA

#define GPIO_KEY_MODE_PIN               GPIO_Pin_9
#define GPIO_KEY_MODE_PORT            GPIOA
#define GPIO_KEY_MODE_CLK               RCC_AHBPeriph_GPIOA

#define GPIO_KEY_FAN_ADJ_PIN               GPIO_Pin_10
#define GPIO_KEY_FAN_ADJ_PORT             GPIOA
#define GPIO_KEY_FAN_ADJ_CLK               RCC_AHBPeriph_GPIOA

#define GPIO_KEY_DLZ_PIN               GPIO_Pin_11
#define GPIO_KEY_DLZ_PORT             GPIOA
#define GPIO_KEY_DLZ_CLK               RCC_AHBPeriph_GPIOA
#define GPIO_KEY_PORT                    GPIOA

#if 0
#define KEY_readPower() GPIO_ReadInputDataBit(GPIO_KEY_POWER_PORT, GPIO_KEY_POWER_PIN)
#define KEY_readMode() GPIO_ReadInputDataBit(GPIO_KEY_MODE_PORT, GPIO_KEY_MODE_PIN)
#define KEY_readFanAdj() GPIO_ReadInputDataBit(GPIO_KEY_FAN_ADJ_PORT, GPIO_KEY_FAN_ADJ_PIN)
#define KEY_readDLZ() GPIO_ReadInputDataBit(GPIO_KEY_DLZ_PORT, GPIO_KEY_DLZ_PIN)
#endif 
#define KEY_readPort() GPIO_ReadInputData(GPIO_KEY_PORT)

#define KEY_WritePower(x) GPIO_WriteBit(GPIO_KEY_POWER_PORT, GPIO_KEY_POWER_PIN,x)
#define KEY_WriteMode(x) GPIO_WriteBit(GPIO_KEY_MODE_PORT, GPIO_KEY_MODE_PIN,x)
#define KEY_WriteFanAdj(x) GPIO_WriteBit(GPIO_KEY_FAN_ADJ_PORT, GPIO_KEY_FAN_ADJ_PIN,x)
#define KEY_WriteDLZ(x) GPIO_WriteBit(GPIO_KEY_DLZ_PORT, GPIO_KEY_DLZ_PIN,x)

typedef struct
{
	int bitPower:1;
	int bitMode:1;
	int bitFanAdj:1;
	int bitDLZ:1;

}key_t;

void KEY_GPIOInit(void);
#endif




