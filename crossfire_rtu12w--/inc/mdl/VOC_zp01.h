#ifndef VOC_ZP01_H
#define VOC_ZP01_H

#define GPIO_VOC_A_PIN               GPIO_Pin_6
#define GPIO_VOC_A_PORT            GPIOA
#define GPIO_VOC_A_CLK               RCC_AHBPeriph_GPIOA

#define GPIO_VOC_B_PIN               GPIO_Pin_5
#define GPIO_VOC_B_PORT             GPIOA
#define GPIO_VOC_B_CLK               RCC_AHBPeriph_GPIOA

#define VOC_readA() GPIO_ReadInputDataBit(GPIO_VOC_A_PORT, GPIO_VOC_A_PIN)
//#define VOC_WriteA(x) GPIO_WriteBit(GPIO_VOC_A_PORT, GPIO_VOC_A_PIN,x)
#define VOC_readB() GPIO_ReadInputDataBit(GPIO_VOC_B_PORT, GPIO_VOC_B_PIN)
//#define VOC_WriteB(x) GPIO_WriteBit(GPIO_VOC_B_PORT, GPIO_VOC_B_PIN,x)
void VOC_GPIOInit(void);
unsigned char VOC_getVal(void);
#endif


