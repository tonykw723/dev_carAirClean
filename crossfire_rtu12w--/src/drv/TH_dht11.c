#include <stdio.h>
#include <stdlib.h>
#include "stm32l1xx.h"
#include "xtsys_type.h"
#include "TH_dht11.h"




//----------------------------------------------//
//----------------IO口定义区--------------------//
//----------------------------------------------//
//sbit  P2_0  = P2^0 ;

//----------------------------------------------//
//----------------定义区--------------------//
//----------------------------------------------//
unsigned char  U8FLAG=0,U8temp=0;
unsigned char  U8T_data_H=0,U8T_data_L=0,U8RH_data_H=0,U8RH_data_L=0,U8checkdata=0;
unsigned char str[5];

void TH_Delay2(unsigned int j)
{      unsigned int i;
	    for(;j>0;j--)
	   { 	
		for(i=0;i<4000;i++);

	   }
}
 void  TH_Delay_10us(void)
{      
	   unsigned  i;
	   for(i=48;i>0;i--) ;
    
}

void TH_GPIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO_LED Clock */
  RCC_AHBPeriphClockCmd(GPIO_TH_CLK, ENABLE);

  /* Configure the GPIO_LED pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_TH_PIN;
  
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
  GPIO_Init(GPIO_TH_PORT, &GPIO_InitStructure);

  //test
  /*
  while(1)
  {
  	GPIO_WriteBit(GPIO_TH_PORT,GPIO_TH_PIN,Bit_RESET);
  	TH_Delay2(180);
  	GPIO_WriteBit(GPIO_TH_PORT,GPIO_TH_PIN,Bit_SET);
  	TH_Delay2(180);
  }*/
}

unsigned char TH_com(void)
{
     
	unsigned char i,U8comdata ;
          
       for(i=0;i<8;i++)	   
	    {
		
	   	    U8FLAG=2;	
	   	while((!TH_readBit())&&U8FLAG++);
			TH_Delay_10us();
		    TH_Delay_10us();
			TH_Delay_10us();
	  		U8temp=0;
	     if(TH_readBit())U8temp=1;
		    U8FLAG=2;
		 while((TH_readBit())&&U8FLAG++);
	   	//超时则跳出for循环		  
	   	 if(U8FLAG==1)break;
	   	//判断数据位是0还是1	 
	   	   
		// 如果高电平高过预定0高电平值则数据位为 1 
	   	 
		   U8comdata<<=1;
	   	   U8comdata|=U8temp;        //0
	     }//rof
	  return  U8comdata	;
	}

	//--------------------------------
	//-----湿度读取子程序 ------------
	//--------------------------------
	//----以下变量均为全局变量--------
	//----温度高8位== U8T_data_H------
	//----温度低8位== U8T_data_L------
	//----湿度高8位== U8RH_data_H-----
	//----湿度低8位== U8RH_data_L-----
	//----校验 8位 == U8checkdata-----
	//----调用相关子程序如下----------
	//--------------------------------

void TH_getVal(unsigned char* H,char* T)
{
 unsigned char  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
	  //主机拉低18ms 
       TH_WriteBit(0);
	   TH_Delay2(180);
	   TH_WriteBit(1);
	 //总线由上拉电阻拉高 主机延时20us
	   TH_Delay_10us();
	   TH_Delay_10us();
	   TH_Delay_10us();
	   TH_Delay_10us();
	 //主机设为输入 判断从机响应信号 
	   TH_WriteBit(1);
	 //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行	  
	   if(!TH_readBit())		 //T !	  
	   {
	   U8FLAG=2;
	 //判断从机是否发出 80us 的低电平响应信号是否结束	 
	   while((!TH_readBit())&&U8FLAG++);
	   U8FLAG=2;
	 //判断从机是否发出 80us 的高电平，如发出则进入数据接收状态
	   while((TH_readBit())&&U8FLAG++);
	 //数据接收状态		 
	  U8RH_data_H_temp= TH_com();
	  U8RH_data_L_temp= TH_com();
	  U8T_data_H_temp=TH_com();
	  U8T_data_L_temp=TH_com();
	  U8checkdata_temp=TH_com();
	  TH_WriteBit(1);
	 //数据校验 
	 
	   U8temp=(U8T_data_H_temp+U8T_data_L_temp+U8RH_data_H_temp+U8RH_data_L_temp);
	   //printf("01-H_data:%0.2x %0.2x;T_data:%0.2x %0.2x;checkdata:%0.2x\n",U8RH_data_H,U8RH_data_L,U8T_data_H,U8T_data_L,U8checkdata_temp);
	   if(U8temp==U8checkdata_temp)
	   {
	   	  U8RH_data_H=U8RH_data_H_temp;
	   	  U8RH_data_L=U8RH_data_L_temp;
		  U8T_data_H=U8T_data_H_temp;
	   	  U8T_data_L=U8T_data_L_temp;
	   	  U8checkdata=U8checkdata_temp;
                //printf("02-H_data:%0.2x %0.2x;T_data:%0.2x %0.2x\n",U8RH_data_H,U8RH_data_L,U8T_data_H,U8T_data_L);
		  *H=U8RH_data_H_temp;
		  *T=(char)U8T_data_H;
	   }//fi
	   }//fi

	}

