#include <stdio.h>
#include <stdlib.h>
#include "stm32l1xx.h"
#include "xtsys_type.h"
#include "TH_am23xx.h"

// 变量定义
unsigned char Sensor_Data[5]={0x00,0x00,0x00,0x00,0x00};
unsigned char Sensor_AnswerFlag;  //收到起始标志位
unsigned char Sensor_ErrorFlag;   //读取传感器错误标志
unsigned int  Sys_CNT;
unsigned int  Tmp;



/********************************************\
|* 功能： 延时	晶振为12M时				    *|
|*  t = 1 为 20us  然后成倍增加10us左右		*|
\********************************************/
void TH_Delay_10us(void)
{
	   unsigned  i;
	   for(i=48;i>0;i--) ;
}
/********************************************\
|* 功能： 延时	晶振为12M时					*|
|* 延时大约 1ms			    				*|
\********************************************/ 
void TH_Delay_N1ms(unsigned int j)
{
  	unsigned int i;
       for(;j>0;j--)
       { 	
    	   for(i=0;i<4000;i++);
    
       }
}



/********************************************\
|* 功能： 读传感器发送的单个字节	        *|
\********************************************/
unsigned char TH_readData(void)
  {
  #if 1
	unsigned int i,cnt;
	unsigned char buffer,tmp;
	buffer = 0;
	for(i=0;i<8;i++)
	{
		cnt=0;
		while(!TH_readSda())	//检测上次低电平是否结束
		{
		  TH_Delay_10us();
		  if(++cnt >= 10)
		   {
		   	  printf("**some err1\r\n");
			  break;
		   }
		}
		TH_Delay_10us();	 //延时30us 
		TH_Delay_10us();
		TH_Delay_10us();
		TH_Delay_10us();
		
		//判断传感器发送数据位
		tmp =0;
		if(TH_readSda())	 
		{
		  tmp = 1;
		}  
		cnt =0;
		while(TH_readSda())		//等待高电平 结束
		{
			TH_Delay_10us();
		   	if(++cnt >= 10)
			{
				printf("**some err2\r\n");
			  break;
			}
		}
		buffer <<=1;
		buffer |= tmp;	
	}
	return buffer;
	#else
	unsigned char i,U8comdata ,U8FLAG,U8temp;
          
       for(i=0;i<8;i++)	   
	    {
		
	   	    U8FLAG=2;	
	   	while((!TH_readSda())&&U8FLAG++);
			TH_Delay_10us();
		    TH_Delay_10us();
			TH_Delay_10us();
	  		U8temp=0;
	     if(TH_readSda())U8temp=1;
		    U8FLAG=2;
		 while((TH_readSda())&&U8FLAG++);
	   	//超时则跳出for循环		  
	   	 if(U8FLAG==1)
	   	 {
	   	 	printf("**some err\r\n");
		 	break;
	   	 }
	   	//判断数据位是0还是1	 
	   	   
		// 如果高电平高过预定0高电平值则数据位为 1 
	   	 
		   U8comdata<<=1;
	   	   U8comdata|=U8temp;        //0
	     }//rof
	     printf("...U8comdata=%0.2x\r\n");
	  return  U8comdata	;
	#endif
  }

/********************************************\
|* 功能： 读传感器              	        *|
\********************************************/
unsigned char TH_getVal(unsigned char* H,char* T)
  {
	unsigned char i,chkSum=0;
	//主机拉低(Min=800US Max=20Ms) 
      TH_WriteSda(0);
	TH_Delay_N1ms(2);  //延时2Ms
	  
	//释放总线 延时(Min=30us Max=50us)
	TH_WriteSda(1);
	TH_Delay_10us();//延时30us
	TH_Delay_10us();
	TH_Delay_10us();
	//主机设为输入 判断传感器响应信号 
	TH_WriteSda(1);
     	  
	Sensor_AnswerFlag = 0;  // 传感器响应标志	 

	//判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
	if(TH_readSda()==0)
	{
	   Sensor_AnswerFlag = 1;//收到起始信号
	   Sys_CNT = 0;
	   //判断从机是否发出 80us 的低电平响应信号是否结束	 
	   while((!TH_readSda()))
	   {
	   	TH_Delay_10us();
	     if(++Sys_CNT>15) //防止进入死循环
		 {
		   
		   Sensor_ErrorFlag = 1;
		   return 0;
		  } 
	    }
	    Sys_CNT = 0;
	    //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
	    while((TH_readSda()))
	    {
	    	TH_Delay_10us();
	       if(++Sys_CNT>15) //防止进入死循环
		   {
		     
		     Sensor_ErrorFlag = 1;
		     return 0;
		   } 
	    } 		 
	    // 数据接收	传感器共发送40位数据 
	    // 即5个字节 高位先送  5个字节分别为湿度高位 湿度低位 温度高位 温度低位 校验和
	    // 校验和为：湿度高位+湿度低位+温度高位+温度低位
	    for(i=0;i<5;i++)
	    {
	      Sensor_Data[i] = TH_readData();
	    }

	    PrintfXTOS("#####read T&H is:\n");
	    for(i=0;i<5;i++)
	    {
	      PrintfXTOS("%0.2x ",Sensor_Data[i]);  
	    }
		PrintfXTOS("#####\n");
	    for(i=0;i<4;i++)
	    {
	    	chkSum+=Sensor_Data[i];
	    }
	    if(chkSum==Sensor_Data[4])
	    {
	    	H[0]=Sensor_Data[1];
		H[1]=Sensor_Data[0];
	    	T[0]=Sensor_Data[3];
		T[1]=Sensor_Data[2];
	    }
	    else
		Sensor_AnswerFlag = 0;
	  }
	  else
	  {
	    Sensor_AnswerFlag = 0;	  // 未收到传感器响应	
	  }
	  return 1;
  }    

unsigned char TH_ini(void)
{
	TH_WriteScl (0);
}

void TH_GPIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_AHBPeriphClockCmd(GPIO_TH_SCL_CLK|GPIO_TH_SDA_CLK, ENABLE);
  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_TH_SCL_PIN|GPIO_TH_SDA_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIO_TH_SCL_PORT, &GPIO_InitStructure);

  TH_ini();
/*
  while(1)
  {
  TH_Delay_N1ms(2);
  TH_WriteSda(0);
    TH_Delay_N1ms(2);
  TH_WriteSda(1);
  }*/
}



void TH_test(void)
{
  TH_ini();
  while(1)
  {
	//TH_getVal();		  // 读取传感器数据
	//TH_Delay_N1ms(2000);	  // 延时 2S(两次读取间隔至少2S) 
  }
}

