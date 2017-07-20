/**********************************************************************/
/* Copyright (c) 2013 Iotcomm - All Rights Reserved                   */
/* Owner: Julian Huang <congda.huang@iotcomm.com>                     */
/**********************************************************************/
#ifdef __cplusplus
extern "C" {
#endif // End for __cplusplus

#include <stdio.h>
#include <string.h>

#include "pro_dev_iot.h"

static void proLongLong2DevAddr(xt_s64 addr, packet_head_t *pHead)
{
	//only use the lower 48bits in xt_s64,
	memcpy(pHead->devaddr, (xt_u8*)&addr + 2, DEV_ADDR_BYTES);
}

static xt_u8 proPackChkSum(const packet_head_t* pHead, const xt_u8* pBuf, xt_s32 count)
{
	xt_u8 chkSum;
	xt_u8 *t;
	xt_s32 i;

	chkSum = 0;
	t = (xt_u8*)&pHead->stx;
	for (i = 0; i < sizeof(packet_head_t); i++)
		chkSum += *t++;
	
	for (i = 0; i < count; i++)
		chkSum += pBuf[i];

	return chkSum ;
}

xt_s32 ProDevIotPack(xt_s64 addr, xt_u8 cmd, ctrl_data_in_t* di, xt_u8* buf, xt_s32* bufSize)
{
	xt_s32 ret = 0;
	packet_head_t pHead;
	packet_tail_t ptTail;
	xt_u8* pData = NULL;
	xt_u8 dataLen = 0;	
	
	pHead.stx = PACK_START_FLAG;

	proLongLong2DevAddr(addr, &pHead);

	pHead.stx2 = PACK_START_FLAG;
	pHead.cmd =  cmd;	

	if(di && di->dataLen > 0)
	{
		pData = (xt_u8*)di->pDataBuf;
		dataLen = di->dataLen;
	}
	
	//OS_ASSERT(*bufSize >= sizeof(packet_head_t) + sizeof(packet_tail_t) + dataLen);
	pHead.len = dataLen;		
	
	ptTail.checksum = proPackChkSum(&pHead, pData, dataLen);
	ptTail.etx = PACK_END_FLAG;		
	
	memcpy(buf, &pHead, sizeof(packet_head_t));
	ret += sizeof(packet_head_t);
	if(dataLen > 0)
	{		
		memcpy(buf + ret, pData, dataLen);
		ret += dataLen;
	}
	
	memcpy(buf + ret, &ptTail, sizeof(packet_tail_t));
	ret += sizeof(packet_tail_t);

	*bufSize = ret;
	return ret;
}

static xt_u8 proDevIotChkInBuf(pro_pkt_parse_t* p, xt_u32 *rSt, xt_u32*rLen)
{
	xt_u32 i = 0;
	xt_u32 j = 0;
	xt_u32 iDataCnt = 0;
	xt_u32 iPktSt = 0;
	xt_u32 iTmp = 0;

	//PrintfXTOS("mdl pro: ProParseChkInBuf st=%d \n", iPktSt);
	while (p->chkLen - iPktSt - p->dataLen > 0) {
		switch(p->pktSt)
		{
		case PRO_PKT_ST_STX1:
			{
				for (i = iPktSt; i < p->chkLen; i++) {
					if(p->chkBuf[i] == PACK_START_FLAG) {
						iPktSt = i;
						p->dataLen = 1;
						p->needLen= 7;
						p->pktSt = PRO_PKT_ST_STX2;
						//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: find STX1 st=%d \n", iPktSt);
						//MDBG("mdl pro: st=%d dl=%d nl=%d\n", iPktSt,p->dataLen,p->needLen);
						break;
					}
				}

				if (i == p->chkLen) {
					*rSt = iPktSt;
					*rLen = p->dataLen;
					//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: can not find STX1! \n");
					return FAILURE_XT;
				}
			}
			break;
		case PRO_PKT_ST_STX2:
			{
				iTmp = p->chkLen - iPktSt - p->dataLen;
				if (iTmp >= p->needLen) {
					if (p->chkBuf[iPktSt + 7] == PACK_START_FLAG) {
						p->dataLen += p->needLen;
						p->needLen = 2;
						p->pktSt = PRO_PKT_ST_DLEN;
						//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: find STX2 \n");
						//MDBG("mdl pro: st=%d dl=%d nl=%d\n", iPktSt,p->dataLen,p->needLen);
					}
					else {
						iPktSt ++;
						p->dataLen = 0;
						p->needLen = 0;
						p->pktSt = PRO_PKT_ST_STX1;
						//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: find STX2 error, backtrace! \n");
					}
				}
				else {
					p->dataLen += iTmp;
					p->needLen -= iTmp;
				}
			}
			break;
		case PRO_PKT_ST_DLEN:
			{
				iTmp = p->chkLen - iPktSt - p->dataLen;
				if (iTmp >= p->needLen) {
					iDataCnt = p->chkBuf[iPktSt + 9];
					p->dataLen += p->needLen;
					p->needLen = iDataCnt;
					p->pktSt = PRO_PKT_ST_DATA;
					//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: find cmd and dataLen \n");
					//MDBG("mdl pro: st=%d dl=%d nl=%d\n", iPktSt,p->dataLen,p->needLen);
				}
				else {
					p->dataLen += iTmp;
					p->needLen -= iTmp;
				}
			}
			break;
		case PRO_PKT_ST_DATA:
			{
				iTmp = p->chkLen - iPktSt - p->dataLen;
				if (iTmp >= p->needLen) {
					p->dataLen += p->needLen;
					p->needLen = 2;
					p->pktSt = PRO_PKT_ST_ETX;
					//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: find total data \n");
					//MDBG("mdl pro: st=%d dl=%d nl=%d\n", iPktSt,p->dataLen,p->needLen);
				}
				else {
					p->dataLen += iTmp;
					p->needLen -= iTmp;
				}
			}
			break;
		case PRO_PKT_ST_ETX:
			{
				iTmp = p->chkLen - iPktSt - p->dataLen;
				if (iTmp >= p->needLen) {
					if (p->chkBuf[iPktSt + p->dataLen + p->needLen - 1] == PACK_END_FLAG) {
						xt_u8 *pData;
						xt_u8 chkSum;
						pData = &p->chkBuf[iPktSt];
						chkSum = 0;
						//LOG_PRINTF(LOG_LEVEL_DEBUG, "---------mdl pro: total hex data: ----------\n");
						for (j = iPktSt; j < iPktSt + p->dataLen + p->needLen ; j++) {
							if(j<iPktSt + p->dataLen + p->needLen - 2)
								chkSum += *pData;
							//LOG_PRINTF(LOG_LEVEL_DEBUG, "%0.2x ",*pData);
							pData++;
						}
						//LOG_PRINTF(LOG_LEVEL_DEBUG, "\n---------------------------------------\n");
						//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: calc chkSum=0x%0.2x vs pkt chkSum=0x%0.2x \n", chkSum,p->chkBuf[iPktSt + p->dataLen + p->needLen - 2]);

						if (p->chkBuf[iPktSt + p->dataLen + p->needLen - 2] == chkSum) {
							p->dataLen += p->needLen;
							p->needLen = 0;
							p->pktSt = PRO_PKT_ST_STX1;
							*rSt = iPktSt;
							*rLen = p->dataLen;

							p->dataLen = 0;
							//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: find chksum correct! \n");
							//MDBG("mdl pro: st=%d dl=%d nl=%d\n", iPktSt,p->dataLen,p->needLen);

							return SUCCESS_XT;
						}
						else {
							iPktSt ++;
							p->dataLen = 0;
							p->needLen = 0;
							p->pktSt = PRO_PKT_ST_STX1;
							//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: chksum error,backtrace! \n");
						}	
					}
					else {
						iPktSt ++;
						p->dataLen = 0;
						p->needLen = 0;
						p->pktSt = PRO_PKT_ST_STX1;
						//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: find ETX error,backtrace! \n");
					}
				}
				else {
					p->dataLen += iTmp;
					p->needLen -= iTmp;
				}
			}
			break;
		default:
			break;
		}
	}

	*rSt = iPktSt;
	*rLen = p->dataLen;
	//LOG_PRINTF(LOG_LEVEL_DEBUG, "mdl pro: not enough data \n");
	return FAILURE_XT;
}

xt_s32 ProDevIotParse(pro_pkt_parse_t* p)
{
	pro_pkt_parse_t* pCtrl = (pro_pkt_parse_t*)p;
	xt_u8 ret = 0,i;
	xt_u32 stOff = 0;
	xt_u32 stLen = 0;

	//PrintfXTOS("mdl pro: ProParsePktInBuf pkt=%d tmp=%d\n", pCtrl->pktLen,pCtrl->tmpLen);
	//if (pCtrl->pktLen > 0)
		//MemMemcpyXTOS(pCtrl->chkBuf, pCtrl->pktBuf, pCtrl->pktLen);
	//if (pCtrl->tmpLen > 0)
		//MemMemcpyXTOS(pCtrl->chkBuf + pCtrl->pktLen, pCtrl->tmpBuf, pCtrl->tmpLen);
	if (pCtrl->chkOff > 0)
		memcpy(pCtrl->chkBuf, pCtrl->chkBuf + pCtrl->chkOff,
			pCtrl->chkLen);

	//pCtrl->chkLen =  pCtrl->tmpLen + pCtrl->pktLen;
	/*
	MDBG("chkLen=%d tmpLen=%d \n",pCtrl->chkLen,pCtrl->tmpLen);
	for(i=0;i<pCtrl->chkLen;i++)
	{
		MDBG("%0.2x ",pCtrl->chkBuf[i]);
	}
	MDBG("\n");*/
	
	ret = proDevIotChkInBuf(pCtrl, &stOff, &stLen);

	if (ret == SUCCESS_XT) {
		//MemMemcpyXTOS(pCtrl->pktBuf, pCtrl->chkBuf + stOff, stLen);
		pCtrl->pktBuf = pCtrl->chkBuf + stOff;
		//clear data and  start next test
		//MDBG("mdl pro: raw left=%d \n", pCtrl->chkLen - stOff - stLen);
		if (pCtrl->chkLen - stOff - stLen > 0) {
			/*memcpy(pCtrl->chkBuf, pCtrl->chkBuf+stOff + stLen,
				pCtrl->chkLen - stOff - stLen);*/
			pCtrl->chkOff = stOff + stLen;
			pCtrl->chkLen -= (stOff + stLen);
			
		}
		else
		{
			pCtrl->chkOff = 0;
			pCtrl->chkLen = 0;
		}

		pCtrl->pktLen = stLen;
		//pCtrl->chkLen = stOff + stLen;
		return SUCCESS_XT;
	}
	else {
		//MemMemcpyXTOS(pCtrl->pktBuf, pCtrl->chkBuf + stOff, stLen);
		pCtrl->pktBuf = pCtrl->chkBuf + stOff;
		pCtrl->pktLen = stLen;
		//pCtrl->chkLen = stOff + stLen;
		pCtrl->chkOff = 0;
		//pCtrl->chkLen = 0;
		return FAILURE_XT;
	}
}

#ifdef __cplusplus
}
#endif // End for __cplusplus
