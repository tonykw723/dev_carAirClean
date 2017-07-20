/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifndef __PRO_IOT_H_
#define __PRO_IOT_H_

#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

typedef char xt_s8;
typedef unsigned char xt_u8;
typedef short xt_s16;
typedef unsigned short xt_u16;
typedef int xt_s32;
typedef unsigned int xt_u32;
typedef long long xt_s64;
typedef char * xt_string;
typedef unsigned char xt_bool;
typedef void xt_void;
typedef float xt_float;
typedef double xt_double;
typedef time_t xt_time_t;

//#define NULL (0)

#define TRUE 1
#define FALSE 0

#define SUCCESS_XT 1
#define FAILURE_XT 0

#define PACK_START_FLAG		0x68
#define PACK_END_FLAG		0x16
#define PACK_MIN_SIZE		12
#define PACK_OFFSET_DATA	10
#define PACK_MAX_DATA_SIZE	255
#define DEV_ADDR_BYTES		6



typedef struct 
{
	xt_void*	pDataBuf;
	xt_u8		dataLen;
}ctrl_data_in_t;

typedef  struct 
{
	xt_u8	stx;
	xt_u8	devaddr[DEV_ADDR_BYTES];
	xt_u8	stx2;
	xt_u8	cmd;
	xt_u8	len;
} packet_head_t;

typedef  struct 
{
	xt_u8	checksum;
	xt_u8	etx;	
} packet_tail_t;

typedef enum
{
	PRO_PKT_ST_STX1 = 0x0,
	PRO_PKT_ST_STX2,	
	PRO_PKT_ST_DLEN,
	PRO_PKT_ST_DATA,
	PRO_PKT_ST_ETX,
	PRO_PKT_ST_END,
} pro_pkt_st_n;

#define PRO_PKT_MAX_SIZE (256+32)

typedef struct 
{
	xt_u8			chkBuf[PRO_PKT_MAX_SIZE];
	xt_u32			chkLen;
	xt_u32			chkOff;
	xt_u8*			pktBuf;
	xt_u32			pktLen;
	xt_u32			dataLen;
	xt_u32			needLen;
	xt_u8			pktSt;
}pro_pkt_parse_t;

#define CMD_UNICAST		0x1

#define CMD_GRPCAST		0x2

#define CMD_BROADCAST	0x3

#define CMD_REPORT		0x4

#define CMD_CFG		0x11

/**
* @brief:				pack specific application level protocol with communication param to device protocol
*
* @param addr:			communication address
*
* @param cmd:			communication command, such as unicast...
*
* @param di:			pointer to specific application level protocol
*
* @param buf:			pointer to device protocol @return
*
* @param bufSize:		max size for input buffer 
*						device protocol length @return
*
* @return:				data length of success pack 
**/
xt_s32 ProDevIotPack(xt_s64 addr, xt_u8 cmd, ctrl_data_in_t* di, xt_u8* buf, xt_s32* bufSize);

/**
* @brief:				parse raw receive data to device protocol
*
* @usage:				step1.copy data to p->tmpBuf and setup p->tmpLen
*						step2.call ProDevIotParse
*						step3.check return code SUCCESS_XT(process p->pktBuf with p->pktLe goto step4) or FAILURE_XT(exit)
*						step4.set p->pktLen = 0 and goto step2
*
* @param p:				pointer to pro_pkt_parse_t @return
*
* @return:				SUCCESS_XT: parse device protocol successful
*						FAILURE_XT: parse device level protocol failure 
**/
xt_s32 ProDevIotParse(pro_pkt_parse_t* p);

#ifdef __cplusplus
}
#endif // End for __cplusplus

#endif // End for __PRO_IOT_H_

