#ifndef _DEV_ZIGBEE_SHUNZHOU_H
#define _DEV_ZIGBEE_SHUNZHOU_H
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

//#define V5_NEW_ZIGBEE

typedef enum
{
	#ifndef V5_NEW_ZIGBEE
	NT_WET=1,//网状网
	NT_START=2,//星型网
	NT_P2P=7,//对等网
	#else
	NT_WET=1,//网状网
	NT_START=2,//星型网
	NT_LINK=3,
	NT_P2P=4,//对等网	
	#endif
}z_net_t;

typedef enum
{
	#ifndef V5_NEW_ZIGBEE
	ND_CENTRE=1,//中心节点
	ND_ROUTE=3,//路由中继节点
	ND_END_DEVICE=4,//终端节点
	#else
	ND_CENTRE=1,//中心节点
	ND_ROUTE=2,//路由中继节点
	ND_END_DEVICE=3,//终端节点
	#endif
}z_node_t;

typedef enum
{
	SD_BOARD_CAST=1,//广播发送
	SD_MS_CAST=2,//Master-Slave主从发送
	SD_PROTOCOL_CAST=3,//协议发送
}z_send_t;


typedef enum
{
	PT_NONE=1,//广播发送
	PT_EVEN=2,//Master-Slave主从发送
	PT_ODD=3,//协议发送
}z_parity_t;

typedef enum
{
	bits_8=1,//8bits
	bits_9=3,//9bits

}z_bits_num_t;

typedef enum
{
	BR_1200=1,//1200 波特率
	BR_2400=2,//
	BR_4800=3,//
	BR_9600=4,//9600 波特率
	BR_19200=5,//
	BR_38400=6,//	
}z_baudRate_t;

typedef enum
{
	DC_ASCII=1,//ASCII 编码
	DC_HEX=2,   //HEX编码
	
}z_dataCode_t;

typedef enum
{
	AO_NO=1,       //不输出
	AO_ASCII=2,  //ASCII编码输出
	AO_HEX=3,    //HEX编码输出
	
}z_scrAddrOut_t;

typedef struct
{                     
	#if 0
	xt_u8 head;// 写配置时的头                                                 
	xt_u8 length; // 这个长度是包含头和自己                                    
	xt_u32 moduleAddr;//模块地址1,2                                             
	xt_u8 moduleInfo[5];// 设备信息                                            
	xt_u8 freqDot;// 频点                                                      
	xt_u8 sendPower;// 发射功率                                                
	xt_u8 netId[2];//网络ID				3  00-FF	                                   
	z_net_t nettype;//网络类型		  1-网状 2-星型 7-对等网                       
	z_node_t nodetype;//节点类型		  1-中心节点 3-中继4-终端                      
	z_send_t sendModel;//发送模式	  1-广播 2-主从 3-按地址                       
	xt_u8 fixObj[4]; // 固定目标                                               
	z_baudRate_t baudRate;//波特率			  1-1200 2-2400 3-4800 4-9600 5-19200 6-38400  
	z_parity_t uartParity;//串口校验	 1-none 2-even 3-odd                           
	xt_u8 databits;// 数据位			   8位 9位                                   
	xt_u8 stop;	// 停止位                                                      
	z_dataCode_t dataCode; // 数据编码	                                               
	z_scrAddrOut_t addrOut;//数据模式		  1-不输出2-ASCII  3-16进制                    
	xt_u8 sleepCtrlMode; // 1 休眠控制模式                                     
	xt_u8 lowPowerMode; //           3-低功耗模式                              
	xt_u8 workTime;	// 工作时间 FF                                             
	xt_u16 sleepTime; // 休眠时间 FF FF                                        
	xt_u16 initiativeReport; // 主动上报FF FF                                  
	xt_u8 volStatus;// 电压状态                                                
	xt_u8 lineSignal; // 链路信号强度
	#else
	xt_u8 head;// 写配置时的头    
	xt_u8 len;
	xt_u8 moduleAddr[4];//模块地址
	xt_u8 devInfo;
	xt_u8 freqDot;// 频点
	xt_u8 sendPower;// 发射功率 	
	xt_u8 netId;//网络ID	
	z_net_t nettype;//网络类型		  1-网状 2-星型 7-对等网      
	z_node_t nodetype;//节点类型		  1-中心节点 3-中继4-终端      
	z_send_t sendModel;//发送模式	  1-广播 2-主从 3-按地址   
	xt_u8 fixAddr[4];
	z_baudRate_t baudRate;//波特率			  1-1200 2-2400 3-4800 4-9600 5-19200 6-38400  
	z_parity_t uartParity;//串口校验	 1-none 2-even 3-odd 
	z_bits_num_t databits;// 数据位			   1-8位 3-9位 
	xt_u8 stopBit;
	z_dataCode_t dataCode; // 数据编码	
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
	xt_u8 head;// 写配置时的头                                                 
	xt_u8 moduleAddr[2];//模块地址
	xt_u8 netId;//网络ID	
	z_net_t nettype;//网络类型		  1-网状 2-星型 7-对等网      
	z_node_t nodetype;//节点类型		  1-中心节点 3-中继4-终端      
	z_send_t sendModel;//发送模式	  1-广播 2-主从 3-按地址    
	z_baudRate_t baudRate;//波特率			  1-1200 2-2400 3-4800 4-9600 5-19200 6-38400  
	z_parity_t uartParity;//串口校验	 1-none 2-even 3-odd 
	z_bits_num_t databits;// 数据位			   1-8位 3-9位 
	z_dataCode_t dataCode; // 数据编码	
	xt_u8 usartTimeout;
	xt_u8 freqDot;// 频点
	xt_u8 sendPower;// 发射功率 	
	z_scrAddrOut_t addrOut;//数据模式		  1-不输出2-ASCII  3-16进制
}zigbeeSz_cfg_req_i_t;    

typedef struct
{                     

	xt_u8 head[2];// 写配置时的头                                                 
	xt_u8 moduleAddr[2];//模块地址
	xt_u8 netId;//网络ID	
	z_net_t nettype;//网络类型		  1-网状 2-星型 7-对等网      
	z_node_t nodetype;//节点类型		  1-中心节点 3-中继4-终端      
	z_send_t sendModel;//发送模式	  1-广播 2-主从 3-按地址    
	z_baudRate_t baudRate;//波特率			  1-1200 2-2400 3-4800 4-9600 5-19200 6-38400  
	z_parity_t uartParity;//串口校验	 1-none 2-even 3-odd 
	z_bits_num_t databits;// 数据位			   1-8位 3-9位 
	z_dataCode_t dataCode; // 数据编码	
	xt_u8 usartTimeout;
	xt_u8 freqDot;// 频点
	xt_u8 sendPower;// 发射功率 	
	z_scrAddrOut_t addrOut;//数据模式		  1-不输出2-ASCII  3-16进制


}zigbeeSz_cfg_resp_i_t;       



xt_void  DevZigbeeSzInit(void);
xt_s32	DevZigbeeSzSend(xt_u8* pData, xt_s32 dataLen, xt_dev_info_t * pDevParam, pxConnWrite pxWrite, pxConnRead pxRead);
xt_s32	DevZigbeeSzRecv(xt_u8* pData, xt_s32 dataLen, xt_u8* pRetData, xt_s32* pRetDataLen, xt_dev_info_t * pDevParam);


#endif

