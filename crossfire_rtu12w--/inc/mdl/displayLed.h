#ifndef DISPLAY_LED_H
#define DISPLAY_LED_H
/*
#define GPIO_LED_EN_PIN               GPIO_Pin_12
#define GPIO_LED_EN_PORT             GPIOB
#define GPIO_LED_EN_CLK               RCC_AHBPeriph_GPIOB*/

#define GPIO_LED_BLUE_PIN               GPIO_Pin_14
#define GPIO_LED_BLUE_PORT             GPIOB
#define GPIO_LED_BLUE_CLK               RCC_AHBPeriph_GPIOB

#define GPIO_LED_RED_PIN               GPIO_Pin_12
#define GPIO_LED_RED_PORT            GPIOB
#define GPIO_LED_RED_CLK               RCC_AHBPeriph_GPIOB

#define GPIO_LED_GREEN_PIN               GPIO_Pin_13
#define GPIO_LED_GREEN_PORT             GPIOB
#define GPIO_LED_GREEN_CLK               RCC_AHBPeriph_GPIOB

#define GPIO_LED_PORT GPIOB

#define GPIO_FLAG_LED_RED_PIN               GPIO_Pin_15
#define GPIO_FLAG_LED_RED_PORT             GPIOB
#define GPIO_FLAG_LED_RED_CLK               RCC_AHBPeriph_GPIOB

#define GPIO_FLAG_LED_BLUE_PIN               GPIO_Pin_7
#define GPIO_FLAG_LED_BLUE_PORT            GPIOC
#define GPIO_FLAG_LED_BLUE_CLK               RCC_AHBPeriph_GPIOC

#define GPIO_FLAG_LED_GREEN_PIN               GPIO_Pin_6
#define GPIO_FLAG_LED_GREEN_PORT             GPIOC
#define GPIO_FLAG_LED_GREEN_CLK               RCC_AHBPeriph_GPIOC


//#define DLED_WriteLedEn(x) GPIO_WriteBit(GPIO_LED_EN_PORT, GPIO_LED_EN_PIN,x)
#define DLED_WriteBlue(x) GPIO_WriteBit(GPIO_LED_BLUE_PORT, GPIO_LED_BLUE_PIN,x)
#define DLED_WriteRed(x) GPIO_WriteBit(GPIO_LED_RED_PORT, GPIO_LED_RED_PIN,x)
#define DLED_WriteGreen(x) GPIO_WriteBit(GPIO_LED_GREEN_PORT, GPIO_LED_GREEN_PIN,x)

#define FLED_WriteBlue(x) GPIO_WriteBit(GPIO_FLAG_LED_BLUE_PORT, GPIO_FLAG_LED_BLUE_PIN,x)
#define FLED_WriteRed(x) GPIO_WriteBit(GPIO_FLAG_LED_RED_PORT, GPIO_FLAG_LED_RED_PIN,x)
#define FLED_WriteGreen(x) GPIO_WriteBit(GPIO_FLAG_LED_GREEN_PORT, GPIO_FLAG_LED_GREEN_PIN,x)
void DLED_GPIOInit(void);
void DLED_close(void);
void DLED_green(void);
void DLED_blue(void);
void DLED_yellow(void);
void DLED_red(void);

void FLED_close(void);
void FLED_green(void);
void FLED_blue(void);
void FLED_yellow (void);
void FLED_red(void);
#endif



