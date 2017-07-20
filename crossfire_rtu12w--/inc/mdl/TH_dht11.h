#ifndef TH_DHT11_H
#define TH_DHT11_H


#define GPIO_TH_PIN                    GPIO_Pin_5
#define GPIO_TH_PORT              GPIOA
#define GPIO_TH_CLK               RCC_AHBPeriph_GPIOA

#define TH_readBit() GPIO_ReadInputDataBit(GPIO_TH_PORT, GPIO_TH_PIN)
#define TH_WriteBit(x) GPIO_WriteBit(GPIO_TH_PORT, GPIO_TH_PIN,x)
void TH_GPIOInit(void);
void TH_getVal(unsigned char* H,char* T);
#endif
