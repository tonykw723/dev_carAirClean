#include <stdio.h>
#include <stdlib.h>
#include "stm32l1xx.h"
#include "xtsys_type.h"
#include "TH_am23xx.h"

// ��������
unsigned char Sensor_Data[5]={0x00,0x00,0x00,0x00,0x00};
unsigned char Sensor_AnswerFlag;  //�յ���ʼ��־λ
unsigned char Sensor_ErrorFlag;   //��ȡ�����������־
unsigned int  Sys_CNT;
unsigned int  Tmp;



/********************************************\
|* ���ܣ� ��ʱ	����Ϊ12Mʱ				    *|
|*  t = 1 Ϊ 20us  Ȼ��ɱ�����10us����		*|
\********************************************/
void TH_Delay_10us(void)
{
	   unsigned  i;
	   for(i=48;i>0;i--) ;
}
/********************************************\
|* ���ܣ� ��ʱ	����Ϊ12Mʱ					*|
|* ��ʱ��Լ 1ms			    				*|
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
|* ���ܣ� �����������͵ĵ����ֽ�	        *|
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
		while(!TH_readSda())	//����ϴε͵�ƽ�Ƿ����
		{
		  TH_Delay_10us();
		  if(++cnt >= 10)
		   {
		   	  printf("**some err1\r\n");
			  break;
		   }
		}
		TH_Delay_10us();	 //��ʱ30us 
		TH_Delay_10us();
		TH_Delay_10us();
		TH_Delay_10us();
		
		//�жϴ�������������λ
		tmp =0;
		if(TH_readSda())	 
		{
		  tmp = 1;
		}  
		cnt =0;
		while(TH_readSda())		//�ȴ��ߵ�ƽ ����
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
	   	//��ʱ������forѭ��		  
	   	 if(U8FLAG==1)
	   	 {
	   	 	printf("**some err\r\n");
		 	break;
	   	 }
	   	//�ж�����λ��0����1	 
	   	   
		// ����ߵ�ƽ�߹�Ԥ��0�ߵ�ƽֵ������λΪ 1 
	   	 
		   U8comdata<<=1;
	   	   U8comdata|=U8temp;        //0
	     }//rof
	     printf("...U8comdata=%0.2x\r\n");
	  return  U8comdata	;
	#endif
  }

/********************************************\
|* ���ܣ� ��������              	        *|
\********************************************/
unsigned char TH_getVal(unsigned char* H,char* T)
  {
	unsigned char i,chkSum=0;
	//��������(Min=800US Max=20Ms) 
      TH_WriteSda(0);
	TH_Delay_N1ms(2);  //��ʱ2Ms
	  
	//�ͷ����� ��ʱ(Min=30us Max=50us)
	TH_WriteSda(1);
	TH_Delay_10us();//��ʱ30us
	TH_Delay_10us();
	TH_Delay_10us();
	//������Ϊ���� �жϴ�������Ӧ�ź� 
	TH_WriteSda(1);
     	  
	Sensor_AnswerFlag = 0;  // ��������Ӧ��־	 

	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	if(TH_readSda()==0)
	{
	   Sensor_AnswerFlag = 1;//�յ���ʼ�ź�
	   Sys_CNT = 0;
	   //�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����	 
	   while((!TH_readSda()))
	   {
	   	TH_Delay_10us();
	     if(++Sys_CNT>15) //��ֹ������ѭ��
		 {
		   
		   Sensor_ErrorFlag = 1;
		   return 0;
		  } 
	    }
	    Sys_CNT = 0;
	    //�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
	    while((TH_readSda()))
	    {
	    	TH_Delay_10us();
	       if(++Sys_CNT>15) //��ֹ������ѭ��
		   {
		     
		     Sensor_ErrorFlag = 1;
		     return 0;
		   } 
	    } 		 
	    // ���ݽ���	������������40λ���� 
	    // ��5���ֽ� ��λ����  5���ֽڷֱ�Ϊʪ�ȸ�λ ʪ�ȵ�λ �¶ȸ�λ �¶ȵ�λ У���
	    // У���Ϊ��ʪ�ȸ�λ+ʪ�ȵ�λ+�¶ȸ�λ+�¶ȵ�λ
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
	    Sensor_AnswerFlag = 0;	  // δ�յ���������Ӧ	
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
	//TH_getVal();		  // ��ȡ����������
	//TH_Delay_N1ms(2000);	  // ��ʱ 2S(���ζ�ȡ�������2S) 
  }
}

