#include <stdio.h>
#include <stdlib.h>
#include "stm32l1xx.h"
#include "xtsys_type.h"
#include "TH_dht11.h"




//----------------------------------------------//
//----------------IO�ڶ�����--------------------//
//----------------------------------------------//
//sbit  P2_0  = P2^0 ;

//----------------------------------------------//
//----------------������--------------------//
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
	   	//��ʱ������forѭ��		  
	   	 if(U8FLAG==1)break;
	   	//�ж�����λ��0����1	 
	   	   
		// ����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽֵ������λΪ 1 
	   	 
		   U8comdata<<=1;
	   	   U8comdata|=U8temp;        //0
	     }//rof
	  return  U8comdata	;
	}

	//--------------------------------
	//-----ʪ�ȶ�ȡ�ӳ��� ------------
	//--------------------------------
	//----���±�����Ϊȫ�ֱ���--------
	//----�¶ȸ�8λ== U8T_data_H------
	//----�¶ȵ�8λ== U8T_data_L------
	//----ʪ�ȸ�8λ== U8RH_data_H-----
	//----ʪ�ȵ�8λ== U8RH_data_L-----
	//----У�� 8λ == U8checkdata-----
	//----��������ӳ�������----------
	//--------------------------------

void TH_getVal(unsigned char* H,char* T)
{
 unsigned char  U8T_data_H_temp,U8T_data_L_temp,U8RH_data_H_temp,U8RH_data_L_temp,U8checkdata_temp;
	  //��������18ms 
       TH_WriteBit(0);
	   TH_Delay2(180);
	   TH_WriteBit(1);
	 //������������������ ������ʱ20us
	   TH_Delay_10us();
	   TH_Delay_10us();
	   TH_Delay_10us();
	   TH_Delay_10us();
	 //������Ϊ���� �жϴӻ���Ӧ�ź� 
	   TH_WriteBit(1);
	 //�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	   if(!TH_readBit())		 //T !	  
	   {
	   U8FLAG=2;
	 //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
	   while((!TH_readBit())&&U8FLAG++);
	   U8FLAG=2;
	 //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
	   while((TH_readBit())&&U8FLAG++);
	 //���ݽ���״̬		 
	  U8RH_data_H_temp= TH_com();
	  U8RH_data_L_temp= TH_com();
	  U8T_data_H_temp=TH_com();
	  U8T_data_L_temp=TH_com();
	  U8checkdata_temp=TH_com();
	  TH_WriteBit(1);
	 //����У�� 
	 
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

