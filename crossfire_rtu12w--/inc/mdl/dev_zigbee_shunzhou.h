#ifndef _DEV_ZIGBEE_SHUNZHOU_H
#define _DEV_ZIGBEE_SHUNZHOU_H
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

//#define V5_NEW_ZIGBEE

typedef enum
{
	#ifndef V5_NEW_ZIGBEE
	NT_WET=1,//��״��
	NT_START=2,//������
	NT_P2P=7,//�Ե���
	#else
	NT_WET=1,//��״��
	NT_START=2,//������
	NT_LINK=3,
	NT_P2P=4,//�Ե���	
	#endif
}z_net_t;

typedef enum
{
	#ifndef V5_NEW_ZIGBEE
	ND_CENTRE=1,//���Ľڵ�
	ND_ROUTE=3,//·���м̽ڵ�
	ND_END_DEVICE=4,//�ն˽ڵ�
	#else
	ND_CENTRE=1,//���Ľڵ�
	ND_ROUTE=2,//·���м̽ڵ�
	ND_END_DEVICE=3,//�ն˽ڵ�
	#endif
}z_node_t;

typedef enum
{
	SD_BOARD_CAST=1,//�㲥����
	SD_MS_CAST=2,//Master-Slave���ӷ���
	SD_PROTOCOL_CAST=3,//Э�鷢��
}z_send_t;


typedef enum
{
	PT_NONE=1,//�㲥����
	PT_EVEN=2,//Master-Slave���ӷ���
	PT_ODD=3,//Э�鷢��
}z_parity_t;

typedef enum
{
	bits_8=1,//8bits
	bits_9=3,//9bits

}z_bits_num_t;

typedef enum
{
	BR_1200=1,//1200 ������
	BR_2400=2,//
	BR_4800=3,//
	BR_9600=4,//9600 ������
	BR_19200=5,//
	BR_38400=6,//	
}z_baudRate_t;

typedef enum
{
	DC_ASCII=1,//ASCII ����
	DC_HEX=2,   //HEX����
	
}z_dataCode_t;

typedef enum
{
	AO_NO=1,       //�����
	AO_ASCII=2,  //ASCII�������
	AO_HEX=3,    //HEX�������
	
}z_scrAddrOut_t;

typedef struct
{                     
	#if 0
	xt_u8 head;// д����ʱ��ͷ                                                 
	xt_u8 length; // ��������ǰ���ͷ���Լ�                                    
	xt_u32 moduleAddr;//ģ���ַ1,2                                             
	xt_u8 moduleInfo[5];// �豸��Ϣ                                            
	xt_u8 freqDot;// Ƶ��                                                      
	xt_u8 sendPower;// ���书��                                                
	xt_u8 netId[2];//����ID				3  00-FF	                                   
	z_net_t nettype;//��������		  1-��״ 2-���� 7-�Ե���                       
	z_node_t nodetype;//�ڵ�����		  1-���Ľڵ� 3-�м�4-�ն�                      
	z_send_t sendModel;//����ģʽ	  1-�㲥 2-���� 3-����ַ                       
	xt_u8 fixObj[4]; // �̶�Ŀ��                                               
	z_baudRate_t baudRate;//������			  1-1200 2-2400 3-4800 4-9600 5-19200 6-38400  
	z_parity_t uartParity;//����У��	 1-none 2-even 3-odd                           
	xt_u8 databits;// ����λ			   8λ 9λ                                   
	xt_u8 stop;	// ֹͣλ                                                      
	z_dataCode_t dataCode; // ���ݱ���	                                               
	z_scrAddrOut_t addrOut;//����ģʽ		  1-�����2-ASCII  3-16����                    
	xt_u8 sleepCtrlMode; // 1 ���߿���ģʽ                                     
	xt_u8 lowPowerMode; //           3-�͹���ģʽ                              
	xt_u8 workTime;	// ����ʱ�� FF                                             
	xt_u16 sleepTime; // ����ʱ�� FF FF                                        
	xt_u16 initiativeReport; // �����ϱ�FF FF                                  
	xt_u8 volStatus;// ��ѹ״̬                                                
	xt_u8 lineSignal; // ��·�ź�ǿ��
	#else
	xt_u8 head;// д����ʱ��ͷ    
	xt_u8 len;
	xt_u8 moduleAddr[4];//ģ���ַ
	xt_u8 devInfo;
	xt_u8 freqDot;// Ƶ��
	xt_u8 sendPower;// ���书�� 	
	xt_u8 netId;//����ID	
	z_net_t nettype;//��������		  1-��״ 2-���� 7-�Ե���      
	z_node_t nodetype;//�ڵ�����		  1-���Ľڵ� 3-�м�4-�ն�      
	z_send_t sendModel;//����ģʽ	  1-�㲥 2-���� 3-����ַ   
	xt_u8 fixAddr[4];
	z_baudRate_t baudRate;//������			  1-1200 2-2400 3-4800 4-9600 5-19200 6-38400  
	z_parity_t uartParity;//����У��	 1-none 2-even 3-odd 
	z_bits_num_t databits;// ����λ			   1-8λ 3-9λ 
	xt_u8 stopBit;
	z_dataCode_t dataCode; // ���ݱ���	
	xt_u8 srcAddrOut;
	xt_u8 sleepTpye;
	xt_u8 lowPowerMode;
	xt_u8 workTime;
	xt_u8 sleepTime;
	xt_u8 selfReportTime;
	xt_u8 voltageStatus;
	xt_u8 linkSignalPower;
	xt_u8 reserve;
	
	#endif

}zigbeeSzV5_cfg_req_i_t;    


typedef struct
{
	xt_u8 head;// д����ʱ��ͷ                                                 
	xt_u8 moduleAddr[2];//ģ���ַ
	xt_u8 netId;//����ID	
	z_net_t nettype;//��������		  1-��״ 2-���� 7-�Ե���      
	z_node_t nodetype;//�ڵ�����		  1-���Ľڵ� 3-�м�4-�ն�      
	z_send_t sendModel;//����ģʽ	  1-�㲥 2-���� 3-����ַ    
	z_baudRate_t baudRate;//������			  1-1200 2-2400 3-4800 4-9600 5-19200 6-38400  
	z_parity_t uartParity;//����У��	 1-none 2-even 3-odd 
	z_bits_num_t databits;// ����λ			   1-8λ 3-9λ 
	z_dataCode_t dataCode; // ���ݱ���	
	xt_u8 usartTimeout;
	xt_u8 freqDot;// Ƶ��
	xt_u8 sendPower;// ���书�� 	
	z_scrAddrOut_t addrOut;//����ģʽ		  1-�����2-ASCII  3-16����
}zigbeeSz_cfg_req_i_t;    

typedef struct
{                     

	xt_u8 head[2];// д����ʱ��ͷ                                                 
	xt_u8 moduleAddr[2];//ģ���ַ
	xt_u8 netId;//����ID	
	z_net_t nettype;//��������		  1-��״ 2-���� 7-�Ե���      
	z_node_t nodetype;//�ڵ�����		  1-���Ľڵ� 3-�м�4-�ն�      
	z_send_t sendModel;//����ģʽ	  1-�㲥 2-���� 3-����ַ    
	z_baudRate_t baudRate;//������			  1-1200 2-2400 3-4800 4-9600 5-19200 6-38400  
	z_parity_t uartParity;//����У��	 1-none 2-even 3-odd 
	z_bits_num_t databits;// ����λ			   1-8λ 3-9λ 
	z_dataCode_t dataCode; // ���ݱ���	
	xt_u8 usartTimeout;
	xt_u8 freqDot;// Ƶ��
	xt_u8 sendPower;// ���书�� 	
	z_scrAddrOut_t addrOut;//����ģʽ		  1-�����2-ASCII  3-16����


}zigbeeSz_cfg_resp_i_t;       



xt_void  DevZigbeeSzInit(void);
xt_s32	DevZigbeeSzSend(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead);
xt_s32	DevZigbeeSzRecv(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam);


#endif

