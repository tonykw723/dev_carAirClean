#ifndef TH_AM23XX_H
#define TH_AM23XX_H

#define GPIO_TH_SCL_PIN               GPIO_Pin_10
#define GPIO_TH_SCL_PORT            GPIOB
#define GPIO_TH_SCL_CLK               RCC_AHBPeriph_GPIOB

#define GPIO_TH_SDA_PIN               GPIO_Pin_11
#define GPIO_TH_SDA_PORT             GPIOB
#define GPIO_TH_SDA_CLK               RCC_AHBPeriph_GPIOB

#define TH_readSda() GPIO_ReadInputDataBit(GPIO_TH_SDA_PORT, GPIO_TH_SDA_PIN)
#define TH_WriteSda(x) GPIO_WriteBit(GPIO_TH_SDA_PORT, GPIO_TH_SDA_PIN,x)
#define TH_WriteScl(x) GPIO_WriteBit(GPIO_TH_SCL_PORT, GPIO_TH_SCL_PIN,x)
void TH_GPIOInit(void);
unsigned char TH_getVal(unsigned char* H,char* T);
#endif

