#include "StdAfx.h"
#include "rtpUpdateThread.h"
#include "pro_dev_iot.h"
#include "x11base.h"
#include "rtuUpdateToolDlg.h"

HANDLE hCom = (HANDLE)-1;
pro_pkt_parse_t	Dev485Data = {0};

#define PrintfXTOS TRACE
#define SprintfXTOS sprintf
#define sleep_ms Sleep
#define MemMemcpyXTOS memcpy
#define MemMemsetXTOS memset
#define TimeXTOS time

#define	CMD_TRANSFILE	0xa0

#define CMD_ACK_STATUS_EMPTY	0
#define CMD_ACK_STATUS_ACK		1
#define CMD_ACK_STATUS_NOACK	2
#define CMD_ACK_STATUS_BUSY		3

#define QUEUE_ITEM_MAX_SIZE (256 + 16*2)

#define PACK_MAX_SIZE						512
#define RECV_BUFF_MAX_SIZE					(PACK_MAX_SIZE*2)
#define SEND_BUFF_MAX_SIZE				    (PACK_MAX_SIZE*2)

typedef enum	
{
	TRANSFILE_SUBCMD_INFO = 1,
	TRANSFILE_SUBCMD_DATA,
	TRANSFILE_SUBCMD_RTU_START,
	TRANSFILE_SUBCMD_RTU_STOP,
	TRANSFILE_SUBCMD_RTU_QUERY,
	TRANSFILE_SUBCMD_MAX,
}cmd_transfile_subcmd_n;

#ifndef O_RDONLY
#define O_RDONLY	00
#endif

#define AP_PLC_MAX_NODE 1

xt_u8 apPLCRTUUpdateRetBuf[QUEUE_ITEM_MAX_SIZE] = {0};
xt_u8 apPLCRTUUpdateRetLen = 0;

xt_u8 auchCRCHi[]={
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
	0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 
	0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 
};

xt_u8 auchCRCLo[]={
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 
	0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
	0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
	0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4, 
	0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 
	0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
	0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
	0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 
	0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 
	0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
	0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
	0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 
	0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 
	0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
	0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
	0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 
	0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 
	0x43, 0x83, 0x41, 0x81, 0x80, 0x40 
};

xt_s32 UTCrc16(xt_u8 *crc,xt_u8 *puchMsg, xt_s32 usDataLen)
{ 
	xt_u8 uchCRCHi = 0xFF ; /* 低CRC 字节初始化 */ 
	xt_u8 uchCRCLo = 0xFF ; 
	xt_u32 uIndex ;			/* CRC循环中的索引 */ 
	if(usDataLen<=0) return -1;
	while (usDataLen--)		/* 传输消息缓冲区 */
	{ 
		uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */ 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; /* 计算CRC 高低字节对调*/
		uchCRCLo = auchCRCLo[uIndex] ; 
	}  

	crc[1] = uchCRCHi;
	crc[0] = uchCRCLo;
	return 0;
} 

xt_s32 UTCrc16Separate(xt_u8 *crc,xt_u8 *puchMsg, xt_s32 usDataLen)
{ 
	xt_u8 uchCRCHi = 0xFF ; /* 低CRC 字节初始化 */ 
	xt_u8 uchCRCLo = 0xFF ; 
	xt_u32 uIndex ;			/* CRC循环中的索引 */ 

	uchCRCHi = crc[1];
	uchCRCLo = crc[0];
	if(usDataLen<=0) return -1;
	while (usDataLen--)		/* 传输消息缓冲区 */
	{ 
		uIndex = uchCRCHi ^ *puchMsg++ ; /* 计算CRC */ 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; /* 计算CRC 高低字节对调*/
		uchCRCLo = auchCRCLo[uIndex] ; 
	}  

	crc[1] = uchCRCHi;
	crc[0] = uchCRCLo;
	return 0;
} 

#ifdef __cplusplus
extern "C"
{
#endif
AD_DECLARE  PASTREAM  AsStreamOpenFromWin32File(const char* filename, int *mode);
#ifdef __cplusplus
}
#endif

static xt_void apPLCSendRespCmd(xt_u8 commId, xt_u8 cmd, xt_u8 status,
	xt_u8* pExtData, xt_u8 extDataLen)
{
#if 0
	xt_s32 ret = 0;
	cmd_t stCmd = {0};
	xt_u8 cmdBuf[QUEUE_ITEM_MAX_SIZE];
	MDBG("apPLCSendRespCmd\n");
	ret = SEND_BUFF_MAX_SIZE;
	stCmd.cmdId = AP_COMM_CMD_SEND_RAWSEND;
	stCmd.connId = commId;

	if (pExtData && extDataLen > 0) {
		ProMakeExtRespCmd(glbCcedPara.devPara.sysInfo.sn, 
			cmd, status, &stCmd,
			pExtData, extDataLen, cmdBuf, &ret);	
	}
	else {
		ProMakeRespCmd(glbCcedPara.devPara.sysInfo.sn, 
			cmd, status, &stCmd, cmdBuf, &ret);	
	}

	ApFrmGetInstance()->ApFrmDoCommand(APP_COMM, &stCmd);
#endif
}

typedef enum	
{
	lccInfo	= 0x13,
	lccUpdate	= 0x16,
} light_ctrl_cmd_n;

typedef enum	
{
	lccUpInfo = 1,
	lccUpData,
	lccUpStatus,
	lccUpCheck,
} light_update_type_n;

typedef enum	
{
	lccUpLocalPriv = 0,
	lccUpLocalNext,
	lccUpLocalCommon,
} light_update_local_n;

typedef struct 
{
	xt_u8		cmd;
	xt_u8		subCmd;
} light_update_header_t;

typedef struct 
{
	xt_u32		fileSize;		//version size
	xt_u32		fileSizeBak;	//version size in other local
	xt_u16		fileCrc;		//version crc
	xt_u16		fileVer;		//version number
	xt_u16		fileCrcBak;		//version crc in other local
	xt_u8		pkgSize;		//package size
	xt_u8		forceFlag;		//force update flag
} light_update_info_t;

#define RTU_UPDATE_PKG_MAP_MAX_SIZE	(128)

#define RTU_UPDATE_PKG_MAP_FIRSTHALF	(0)
#define RTU_UPDATE_PKG_MAP_SECONDHALF	(1)

typedef struct 
{
	xt_u32		fileSize;		//version size
	xt_u16		fileCrc;		//version crc
	xt_u16		fileVer;		//version number
	xt_u8		pkgMapIdx;		//package map index
	xt_u8		pkgMapData[RTU_UPDATE_PKG_MAP_MAX_SIZE];
} light_status_info_t;

#define RTU_UPDATE_PKG_MAX_SIZE	(160)

typedef struct 
{
	xt_u32		fileSize;		//version size
	xt_u32		fileSizeBak;	//version size in other local
	xt_u16		fileCrc;		//version crc
	xt_u16		fileVer;		//version number
	xt_u16		fileCrcBak;		//version crc in other local
	xt_u16		pkgCrc;			//package crc
	xt_u16		pkgIdx;			//package inde
	xt_u8		verIdx;			//version local index
	xt_u8		verData[RTU_UPDATE_PKG_MAX_SIZE];
} light_data_info_t;

HANDLE comOpen(LPVOID arg, CString comName)
{
	HANDLE com = (HANDLE)-1;
	CrtuUpdateToolDlg* pDlg = (CrtuUpdateToolDlg *)arg;
	CString s;

	com = CreateFile(comName, GENERIC_READ|GENERIC_WRITE, 0,NULL , OPEN_EXISTING, 0, NULL);
	if (com == (HANDLE)-1)
	{
		DWORD nErrorNo = GetLastError();
		s.Format("%s OPEN ERROR %d !", comName ,nErrorNo);
		pDlg->m_info = s;
		pDlg->SendMessage(WM_WRITEDATA,0,0); 
		TRACE("open %s fail,errno = %d\n", comName, nErrorNo);
	}
	else
	{
		DCB wdcb;
		COMMTIMEOUTS to;
		CString tmp;

		memset(&to,0,sizeof(to));
		//to.ReadIntervalTimeout = MAXDWORD;
		to.ReadTotalTimeoutConstant = 10;
		to.WriteTotalTimeoutConstant = 10;
		SetCommTimeouts(com,&to);

		GetCommState (com, &wdcb);

		pDlg->m_comBr.GetLBText(pDlg->m_comBr.GetCurSel(),tmp);
		wdcb.BaudRate=atoi(tmp);

		wdcb.fBinary=TRUE;

		pDlg->m_comDatabit.GetLBText(pDlg->m_comDatabit.GetCurSel(),tmp);
		wdcb.ByteSize=atoi(tmp);

		pDlg->m_comParity.GetLBText(pDlg->m_comParity.GetCurSel(),tmp);
		if (tmp == "N")
		{
			wdcb.Parity=NOPARITY;
		}
		else if (tmp == "E")
		{
			wdcb.Parity=EVENPARITY;
		}
		else if (tmp == "O")
		{
			wdcb.Parity=ODDPARITY;
		}

		pDlg->m_comStopbit.GetLBText(pDlg->m_comStopbit.GetCurSel(),tmp);
		if (tmp == "1")
		{
			wdcb.StopBits=ONESTOPBIT;
		}
		else if (tmp == "1.5")
		{
			wdcb.StopBits=ONE5STOPBITS;
		}
		else if (tmp == "2")
		{
			wdcb.StopBits=TWOSTOPBITS;
		}

		SetCommState (com, &wdcb);
		SetupComm(com,1024,1024);
		PurgeComm(com, PURGE_TXCLEAR);

		s.Format("%s OPEN SUCCESS!", comName );
		pDlg->m_info = s;
		pDlg->SendMessage(WM_WRITEDATA,0,0); 
		TRACE("open %s success\n", comName);
	}

	return com;
}

void comClose(LPVOID arg, HANDLE com)
{
	CrtuUpdateToolDlg* pDlg = (CrtuUpdateToolDlg *)arg;
	CString s;

	CloseHandle(com);
	s.Format("COM CLOSE SUCCESS!");
	pDlg->m_info += s;
	pDlg->SendMessage(WM_WRITEDATA,0,0); 
}

#if 1
typedef enum
{
	MISC_UPDATE_ST_IDLE = 0x0,
	MISC_UPDATE_ST_SEND_INFO,
	MISC_UPDATE_ST_SEND_DATA,
	MISC_UPDATE_ST_GET_STATUS,
	MISC_UPDATE_ST_SEND_CHECK,
	MISC_UPDATE_ST_MAX,
} ap_plc_misc_update_st_n;

//runtime param
static xt_u8 	miscRTUUpdateFlag = FALSE;//store???
static xt_u8 	miscRTUUpdateCurLoop = 0;//store???
static xt_u32 	miscRTUUpdateCurIndex = 0;//store???
static xt_u32 	miscRTUUpdateSt = 0;//store???
static xt_u32 	miscRTUUpdateStatusIdx = 0;//store???
static PASTREAM miscRTUUpdateBinSm = NULL;
static PASTREAM miscRTUUpdateBinSmBak = NULL;

//config param
static xt_u32 	miscRTUUpdateFileSize = 38384;//cfg
static xt_u32 	miscRTUUpdateFileSizeBak = 38388;//cfg
static xt_u16 	miscRTUUpdateFileCrc = 65372;//cfg
static xt_u16 	miscRTUUpdateFileCrcBak = 48190;//cfg
static xt_u16 	miscRTUUpdateFileVer = 118;//cfg
static xt_u32 	miscRTUUpdatePacketSize = 160;//cfg
static xt_u32 	miscRTUUpdateForce = 1;//cfg

static xt_u8 	miscRTUUpdateMaxLoop = 3;//cfg
static xt_u8 	miscRTUUpdateQueryLoop = 3;//cfg
static xt_u8 	miscRTUUpdateUnicastRetry = 4;//cfg
static xt_u8 	miscRTUUpdateMulticastRetry = 3;//cfg
static xt_u8 	miscRTUUpdateDelay = 8; //cfg

xt_u32 ApPLCRTUUpdateLoadParam(void *arg)
{
	PASTREAM iniSrcStream =NULL;
	PASFINI inistream = NULL;
	xt_s8 iniFn[MAX_PATH]={0};//".\\crossfire.ini";
	xt_u32 mode= O_RDONLY;
	xt_u32 ret = FAILURE_XT;
	xt_s8 sdv[200];
	xt_s8 secName[64] = {0};
	xt_s8 keyName[64] = {0};
	xt_u32 tmpData = 0;

	strcpy(iniFn, ((rtpUpdateThread *)arg)->dirName.GetBuffer(MAX_PATH));
	strcat(iniFn,"\\crossfire.ini");

	iniSrcStream = (PASTREAM)AsStreamOpenFromWin32File(iniFn, (int *)&mode);
	if(iniSrcStream != NULL)
	{
		inistream = (PASFINI)AsFIniOpenFromStream(iniSrcStream);

		if(inistream != NULL)
		{
			PrintfXTOS("===== ApPLCRTUUpdateLoadParam ===== \n");

			//file
			SprintfXTOS(secName, "RTUUpdateFile");

			SprintfXTOS(keyName, "fileSize"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdateFileSize = tmpData;
			//miscRTUUpdateFileSize = 500;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam fileSize = %d =====  \n", miscRTUUpdateFileSize);

			SprintfXTOS(keyName, "fileCrc"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdateFileCrc = tmpData;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam fileCrc = %d =====  \n", miscRTUUpdateFileCrc);

			SprintfXTOS(keyName, "fileSizeBak"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdateFileSizeBak = tmpData;
			//miscRTUUpdateFileSizeBak = 500;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam fileSizeBak = %d =====  \n", miscRTUUpdateFileSizeBak);

			SprintfXTOS(keyName, "fileCrcBak"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdateFileCrcBak = tmpData;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam fileCrcBak = %d =====  \n", miscRTUUpdateFileCrcBak);

			SprintfXTOS(keyName, "fileVer"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdateFileVer = tmpData;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam fileVer = %d =====  \n", miscRTUUpdateFileVer);

			SprintfXTOS(keyName, "packageSize"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdatePacketSize = tmpData;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam packageSize = %d =====  \n", miscRTUUpdatePacketSize);

			SprintfXTOS(keyName, "force"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData >= 0)
				miscRTUUpdateForce = tmpData;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam force = %d =====  \n", miscRTUUpdateForce);

			//param
			SprintfXTOS(secName, "RTUUpdateParam");

			SprintfXTOS(keyName, "maxLoop"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdateMaxLoop = tmpData;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam maxLoop = %d =====  \n", miscRTUUpdateMaxLoop);

			miscRTUUpdateMaxLoop = 1;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam modify maxLoop = %d =====  \n", miscRTUUpdateMaxLoop);

			SprintfXTOS(keyName, "queryLoop"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdateQueryLoop = tmpData;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam queryLoop = %d =====  \n", miscRTUUpdateQueryLoop);

			SprintfXTOS(keyName, "unicastRetry"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdateUnicastRetry = tmpData;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam unicastRetry = %d =====  \n", miscRTUUpdateUnicastRetry);

			SprintfXTOS(keyName, "multicastRetry"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdateMulticastRetry = tmpData;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam multicastRetry = %d =====  \n", miscRTUUpdateMulticastRetry);

			SprintfXTOS(keyName, "delay"); 
			SprintfXTOS(sdv, ""); 
			tmpData = 0;
			ret = AsFIniReadKeyValueDef(inistream,  secName, keyName, sdv, 200, sdv);
			tmpData = atoi(sdv);
			if(tmpData > 0)
				miscRTUUpdateDelay = tmpData;
			PrintfXTOS("$===== ApPLCRTUUpdateLoadParam delay = %d =====  \n", miscRTUUpdateDelay);

			if(miscRTUUpdatePacketSize > RTU_UPDATE_PKG_MAX_SIZE)
				miscRTUUpdatePacketSize = RTU_UPDATE_PKG_MAX_SIZE;

			if(miscRTUUpdateForce > 1)
				miscRTUUpdateForce = 1;

			if(miscRTUUpdateQueryLoop > miscRTUUpdateMaxLoop)
				miscRTUUpdateQueryLoop= miscRTUUpdateMaxLoop;

			ret = SUCCESS_XT;

		}
	}

	return ret;
}

xt_u32 ApPLCRTUUpdateDelay()
{
#if 0
	static xt_time_t checkTime = 0;
	xt_time_t now;

	TimeXTOS(&now);
	if(checkTime == 0)
	{
		TimeXTOS(&checkTime);
		return FAILURE_XT;
	}
	else
	{
		if(now - checkTime >= miscRTUUpdateDelay)
		{
			checkTime = 0;
			return SUCCESS_XT;
		}
		else
			return FAILURE_XT;
	}
#else
	return SUCCESS_XT;
#endif
}

xt_u32 ApPLCRTUUpdateDelayTime()
{
	static xt_time_t checkTime = 0;
	xt_time_t now;

	TimeXTOS(&now);
	if(checkTime == 0)
	{
		TimeXTOS(&checkTime);
		return FAILURE_XT;
	}
	else
	{
		if(now - checkTime >= miscRTUUpdateDelay)
		{
			checkTime = 0;
			return SUCCESS_XT;
		}
		else
			return FAILURE_XT;
	}
}

xt_void ApPLCRTUUpdateStart(void *arg)
{
	xt_s32 mode = O_RDONLY;
	xt_s8 binFn[MAX_PATH]={0};//".\\crossfire.bin";
	xt_s8 binBakFn[MAX_PATH]={0};//".\\cf.bin";
	xt_u32 ret = FAILURE_XT;
	xt_u8 retData[2] = {0};
	CrtuUpdateToolDlg* pDlg = (CrtuUpdateToolDlg *)(((rtpUpdateThread *)arg)->dlg);
	CString s;

	if(miscRTUUpdateFlag == TRUE)
	{
		retData[0]= TRANSFILE_SUBCMD_RTU_START;
		retData[1]= 1;
		s.Format("ALREADY START!");
		pDlg->m_info = s;
		pDlg->SendMessage(WM_WRITEDATA,0,0); 
		return;
	}

	miscRTUUpdateFlag = TRUE;
	miscRTUUpdateCurLoop = 0;
	miscRTUUpdateCurIndex = 0;
	miscRTUUpdateSt = MISC_UPDATE_ST_SEND_INFO;
	miscRTUUpdateStatusIdx = 0;
	strcpy(binFn, ((rtpUpdateThread *)arg)->dirName.GetBuffer(MAX_PATH));
	strcat(binFn,"\\crossfire.bin");
	strcpy(binBakFn, ((rtpUpdateThread *)arg)->dirName.GetBuffer(MAX_PATH));
	strcat(binBakFn,"\\cf.bin");
	if(miscRTUUpdateBinSm == NULL)
		miscRTUUpdateBinSm = (PASTREAM)AsStreamOpenFromWin32File(binFn, &mode);
	if(miscRTUUpdateBinSmBak == NULL)
		miscRTUUpdateBinSmBak = (PASTREAM)AsStreamOpenFromWin32File(binBakFn, &mode);

	ret = ApPLCRTUUpdateLoadParam(arg);

	retData[0]= TRANSFILE_SUBCMD_RTU_START;
	if(ret == SUCCESS_XT)
	{
		s.Format("START OK!");
		pDlg->m_info += s;
		pDlg->SendMessage(WM_WRITEDATA,0,0); 
	}
	else
	{
		s.Format("START PARAM ERROR!");
		pDlg->m_info += s;
		pDlg->SendMessage(WM_WRITEDATA,0,0); 
	} 

	s.Format("%d",miscRTUUpdateFileVer);
	pDlg->m_iniver = s;
	pDlg->m_rtu_start_ver = "";
	pDlg->m_rtu_ver = "";

	pDlg->GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	pDlg->GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
	pDlg->GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);
}

xt_void ApPLCRTUUpdateStop(void *arg)
{
	xt_u8 retData[2] = {0};
	CrtuUpdateToolDlg* pDlg = (CrtuUpdateToolDlg *)arg;
	CString s;

	miscRTUUpdateFlag = FALSE;
	miscRTUUpdateCurLoop = 0;
	miscRTUUpdateCurIndex = 0;
	miscRTUUpdateSt = MISC_UPDATE_ST_IDLE;
	miscRTUUpdateStatusIdx = 0;
	if(miscRTUUpdateBinSm != NULL)
	{
		AsStreamClose(miscRTUUpdateBinSm);
		miscRTUUpdateBinSm = NULL;
	}

	if(miscRTUUpdateBinSmBak != NULL)
	{
		AsStreamClose(miscRTUUpdateBinSmBak);
		miscRTUUpdateBinSmBak = NULL;
	}

	retData[0]= TRANSFILE_SUBCMD_RTU_STOP;
	s.Format("STOP OK!");
	pDlg->m_info = s;
	pDlg->m_result = "";
	pDlg->m_curpkg = "0";
	pDlg->m_totalpkg = "0";
	pDlg->SendMessage(WM_WRITEDATA,0,0); 

	pDlg->GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	pDlg->GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	pDlg->GetDlgItem(IDC_BUTTON5)->EnableWindow(TRUE);

	/*if (hCom != (HANDLE)-1)
	{
		comClose(arg, hCom);
		hCom = (HANDLE)-1;
	}*/
}

xt_void ApPLCRTUUpdateQuery(void *arg)
{
	xt_u8 retData[2] = {0};

	retData[0]= TRANSFILE_SUBCMD_RTU_QUERY;
	retData[1]= miscRTUUpdateFlag;
	apPLCSendRespCmd(0, CMD_TRANSFILE, CMD_ACK_STATUS_ACK,
		retData,sizeof(retData));
}

static xt_u32 apPLCRTUUpdateGetUidInx()
{
	xt_u32 i = 0;

#if 0
	for(i = miscRTUUpdateStatusIdx; i < AP_PLC_MAX_NODE; i++)
	{
		if (apPLCIsValidNode(i) == TRUE)
		{
			miscRTUUpdateStatusIdx = i;
			break;
		}
	}

	if(i == AP_PLC_MAX_NODE)
		miscRTUUpdateStatusIdx = i;
#else
	miscRTUUpdateStatusIdx = 0;
#endif

	return miscRTUUpdateStatusIdx;
}

static xt_u8 miscUid[6] = {0};

static xt_u8* apPLCRTUUpdateGetSeqUid()
{
	xt_u32 i = 0;
	static xt_u32 seqIdx = 0;
#if 0
	for(i = seqIdx; i < AP_PLC_MAX_NODE; i++)
	{
		if (apPLCIsValidNode(i) == TRUE)
		{
			seqIdx = i;
			break;
		}
	}

	if(i == AP_PLC_MAX_NODE)
	{
		for(i = 0; i < AP_PLC_MAX_NODE; i++)
		{
			if (apPLCIsValidNode(i) == TRUE)
			{
				seqIdx = i;
				break;
			}
		}

		if(i == AP_PLC_MAX_NODE)
			return NULL;
	}

	return mApPLCData.mNodeList[seqIdx].uId;
#else
	return miscUid;
#endif
}

static xt_u32 apPLCDoSubCmd(xt_u8 cmd, packet_head_t* pHead, xt_u8 connId, xt_u8 retFlag,xt_u8 retry)
{
	//todo:
	int i = 0;
	int j = 0;
	DWORD rCount = 0;
	DWORD wCount = 0;
	int dataLen = 0;
	xt_u8 recvBuf[1024] = {0};
	xt_u8 ret = FAILURE_XT;
	xt_time_t sendTime = 0;
	xt_time_t now = 0;

	for (j = 0; j < 3; j++)
	{
		dataLen = pHead->len + 12;
		TRACE("com send packet %d data:\n", dataLen);
		for (i = 0; i < dataLen; i++)
		{
			TRACE("%02x ",*((xt_u8*)pHead + i));
		}
		TRACE("\n");

		WriteFile(hCom, (xt_u8*)pHead, dataLen, &wCount, NULL);
		TimeXTOS(&sendTime);

		sleep_ms(100);

		while(TRUE)
		{
			ReadFile(hCom, recvBuf, 1024, &rCount, NULL);
			TimeXTOS(&now);

			TRACE("com time offset %d:\n", now - sendTime);
			if(rCount > 0)
			{
				/*TRACE("com recv raw %d data:\n", rCount);
				for (i = 0; i < rCount; i++)
				{
					TRACE("%02x ",recvBuf[i]);
				}
				TRACE("\n");*/

				if (Dev485Data.chkOff > 0)
					memcpy(Dev485Data.chkBuf, Dev485Data.chkBuf + Dev485Data.chkOff,
						Dev485Data.chkLen);

				if(Dev485Data.chkLen + rCount <= PRO_PKT_MAX_SIZE)
				{
					memcpy(Dev485Data.chkBuf + Dev485Data.chkLen, recvBuf, rCount);
					Dev485Data.chkLen += rCount;
				}

				if (ProDevIotParse(&Dev485Data) == SUCCESS_XT) {
					TRACE("com recv packet %d data:\n", Dev485Data.pktLen);
					for (i = 0; i < Dev485Data.pktLen; i++)
					{
						TRACE("%02x ",Dev485Data.pktBuf[i]);
					}
					TRACE("\n");

					MemMemcpyXTOS(apPLCRTUUpdateRetBuf, Dev485Data.pktBuf, Dev485Data.pktLen);
					Dev485Data.pktLen = 0;
					ret = SUCCESS_XT;
					break;
				}
				
			}
			else
			{
				if (now - sendTime > 6)
				{
					break;
				}

				sleep_ms(250);
			}
			
		}
		if (ret == SUCCESS_XT)
		{
			return ret;
		}
	}

	return ret;
}

static xt_u32 apPLCRTUUpdateSendCmd(xt_u8 *pUid, xt_u8 cmd, xt_u8 *pBuf, xt_u32 len)
{
	xt_s32 ret = SEND_BUFF_MAX_SIZE;
	xt_u8 buf[QUEUE_ITEM_MAX_SIZE];
	xt_u8 tmpbuf[QUEUE_ITEM_MAX_SIZE];
	ctrl_data_in_t di = {0};
	xt_s64 devAddr = 0;
	xt_u8 connId = 0;
	xt_u8 retry = 1;
	xt_u8 reqReturn = FALSE;
	xt_u8 i = 0;
	xt_u8	ccuid[DEV_ADDR_BYTES] = {0};

	/*MemMemcpyXTOS(ccuid, (xt_u8*)&glbCcedPara.devPara.sysInfo.sn+2, DEV_ADDR_BYTES);

	if(glbCcedPara.devPara.sysParam.tct == tctEthernet)
	{
		connId = AP_FRM_CONN_TCPC_ID;
	}
	else if(glbCcedPara.devPara.sysParam.tct == tctGPRSSMS)
	{
		connId = AP_FRM_CONN_MCC_ID;
	}*/

	MemMemsetXTOS(tmpbuf, 0, QUEUE_ITEM_MAX_SIZE);

	if(len > 0)
	{
		MemMemcpyXTOS(&tmpbuf[0], pBuf, len);
	}
	di.pDataBuf = tmpbuf;
	di.dataLen = len;

	if(pUid != NULL)
	{
		if(cmd == CMD_UNICAST)
			MemMemcpyXTOS(&devAddr, pUid, DEV_ADDR_BYTES);
		else
		{
			//if (glbCcedPara.debugPara.plcModType == PLC_MOD_TYPE_WL_ZIGBEE)
				MemMemcpyXTOS(&devAddr, ccuid, DEV_ADDR_BYTES);
			//else
				//MemMemcpyXTOS(&devAddr, pUid, DEV_ADDR_BYTES);
		}
	}

	devAddr = devAddr << 16; // uid store low 48 bit

	ret = ProDevIotPack(devAddr, cmd, &di, buf, &ret);

	if(cmd == CMD_UNICAST)
	{
		reqReturn = TRUE;
		retry = miscRTUUpdateUnicastRetry;

		return apPLCDoSubCmd(cmd, (packet_head_t*)buf, connId, reqReturn, retry);
	}
	else
	{
		reqReturn = FALSE;
		retry = miscRTUUpdateMulticastRetry;

		//for(i = 0; i < 1; i++)
		{
			return apPLCDoSubCmd(cmd, (packet_head_t*)buf, connId, reqReturn, 1);

			//sleep_ms(1000);
		}
	}

}

static xt_u32 apPLCRTUUpdateSendInfoCmd(xt_u8 *pUid, xt_u8 cmd, xt_u8 subcmd, xt_u8 *pBuf, xt_u32 len)
{
	xt_u8 buf[QUEUE_ITEM_MAX_SIZE];
	light_update_header_t *pHeader = NULL;

	MemMemsetXTOS(buf, 0, QUEUE_ITEM_MAX_SIZE);

	pHeader = (light_update_header_t *)buf;
	pHeader->cmd = lccUpdate;
	pHeader->subCmd  = subcmd;

	if(len > 0)
	{
		MemMemcpyXTOS(buf + sizeof(light_update_header_t), pBuf, len);
	}

	return apPLCRTUUpdateSendCmd(pUid, cmd, buf, len + sizeof(light_update_header_t));
}

static xt_u32 apPLCRTUUpdateGetStatusCmd(xt_u8 *pUid, xt_u8 cmd, xt_u8 subcmd, xt_u8 idx)
{
	xt_u8 buf[QUEUE_ITEM_MAX_SIZE];
	light_update_header_t *pHeader = NULL;

	MemMemsetXTOS(buf, 0, QUEUE_ITEM_MAX_SIZE);

	pHeader = (light_update_header_t *)buf;
	pHeader->cmd = lccUpdate;
	pHeader->subCmd  = subcmd;

	//get half package status;
	buf[2] = idx;

	return apPLCRTUUpdateSendCmd(pUid, cmd, buf, 1 + sizeof(light_update_header_t));
}

static xt_u32 apPLCRTUUpdateSendDataCmd(xt_u8 *pUid, xt_u8 cmd, xt_u8 subcmd, xt_u16 sNo, xt_u8 id, xt_u8 *pBuf, xt_u32 len)
{
	xt_u8 buf[QUEUE_ITEM_MAX_SIZE];
	xt_u16 crc = 0;
	light_update_header_t *pHeader = NULL;
	light_data_info_t* pDataInfo = NULL;

	MemMemsetXTOS(buf, 0, QUEUE_ITEM_MAX_SIZE);

	pHeader = (light_update_header_t *)buf;
	pHeader->cmd = lccUpdate;
	pHeader->subCmd  = subcmd;

	pDataInfo = (light_data_info_t*)(buf + sizeof(light_update_header_t));

	pDataInfo->fileSize = miscRTUUpdateFileSize;
	pDataInfo->fileSizeBak = miscRTUUpdateFileSizeBak;
	pDataInfo->fileVer = miscRTUUpdateFileVer;
	pDataInfo->fileCrc = miscRTUUpdateFileCrc;
	pDataInfo->fileCrcBak = miscRTUUpdateFileCrcBak;

	pDataInfo->verIdx = id;
	pDataInfo->pkgIdx = sNo;
	UTCrc16((xt_u8 *)&crc,pBuf,len);
	pDataInfo->pkgCrc = crc;

	PrintfXTOS("===== apPLCRTUUpdateSendDataCmd crc = %d===== \n",crc);

	if(len > 0)
	{
		MemMemcpyXTOS(pDataInfo->verData, pBuf, len);
	}

	return apPLCRTUUpdateSendCmd(pUid, cmd, buf, 
		len + sizeof(light_data_info_t) - RTU_UPDATE_PKG_MAX_SIZE + sizeof(light_update_header_t));
}

static xt_u32 apPLCRTUUpdateSendCheckCmd(xt_u8 *pUid, xt_u8 cmd, xt_u8 subcmd)
{
	xt_u8 buf[QUEUE_ITEM_MAX_SIZE];
	light_update_header_t *pHeader = NULL;

	MemMemsetXTOS(buf, 0, QUEUE_ITEM_MAX_SIZE);

	pHeader = (light_update_header_t *)buf;
	pHeader->cmd = lccInfo;
	pHeader->subCmd  = subcmd;

	return apPLCRTUUpdateSendCmd(pUid, cmd, buf, 1);
}

static xt_u32 apPLCRTUUpdateGetBitInArray(xt_u8 *pArray, xt_u16 idx)
{
	xt_u8 ret = 0;
	xt_u8 arrayIndex = idx / 8;
	xt_u8 byteOffset = idx % 8;

	ret = *(pArray + arrayIndex) & (0x1 << byteOffset);

	return (ret != 0)?(TRUE):(FALSE);
}

static xt_u32 ApPLCRTUUpdateProc(void * arg)
{
#if 1
	xt_s32 ret = FAILURE_XT;
	xt_s32 len = 0;
	xt_s32 lenBak = 0;
	xt_u8 buf[QUEUE_ITEM_MAX_SIZE] = {0};
	xt_u8 bufBak[QUEUE_ITEM_MAX_SIZE] = {0};
	xt_u32 toRead = 0;
	xt_u8 queryFlag = FALSE;
	xt_u32 maxPkg = 0;
	static xt_u32 minPkg = 0xffffffff;
	CrtuUpdateToolDlg* pDlg = (CrtuUpdateToolDlg *)arg;
	CString s;

	if(miscRTUUpdateFlag == FALSE)
		return ret;

	if(miscRTUUpdateBinSm == NULL)
		return ret;

	if(miscRTUUpdateBinSmBak == NULL)
		return ret;

	PrintfXTOS("===== ApPLCRTUUpdateProc curLoop = %d ===== \n", miscRTUUpdateCurLoop);

	if(miscRTUUpdateCurLoop < miscRTUUpdateMaxLoop)
	{
		if(miscRTUUpdateMaxLoop >= miscRTUUpdateQueryLoop)
		{
			if(miscRTUUpdateCurLoop >= miscRTUUpdateMaxLoop - miscRTUUpdateQueryLoop)
				queryFlag = TRUE;
		}
	}
	else
	{
		PrintfXTOS("===== ApPLCRTUUpdateProc end... ===== \n");

		//todo:
		ApPLCRTUUpdateStop(arg);
		/*
		if(glbCcedPara.devPara.sysParam.tct==tctGPRSSMS)
			ApPLCRTUUpdateStop(AP_FRM_CONN_MCC_ID);
		else if(glbCcedPara.devPara.sysParam.tct==tctEthernet)
			ApPLCRTUUpdateStop(AP_FRM_CONN_TCPC_ID);*/

	}

	PrintfXTOS("===== ApPLCRTUUpdateProc st = %d ===== \n", miscRTUUpdateSt);

	switch(miscRTUUpdateSt)
	{
	case MISC_UPDATE_ST_SEND_INFO:
		{
			if(1)//if(miscRTUUpdateCurLoop == 0)
			{
				light_update_info_t info = {0};
				xt_u32 idx = 0;
				char verStr[16] = {0};

				PrintfXTOS("===== ApPLCRTUUpdateProc start to send info ===== \n");

				info.fileSize = miscRTUUpdateFileSize;
				info.fileSizeBak = miscRTUUpdateFileSizeBak;
				info.fileVer = miscRTUUpdateFileVer;
				info.fileCrc = miscRTUUpdateFileCrc;
				info.fileCrcBak = miscRTUUpdateFileCrcBak;
				info.pkgSize = miscRTUUpdatePacketSize;
				info.forceFlag = miscRTUUpdateForce;

				pDlg->m_rtu_start_ver = "...";
				pDlg->SendMessage(WM_WRITEDATA,0,0); 

				if(apPLCRTUUpdateSendCheckCmd(apPLCRTUUpdateGetSeqUid(), CMD_BROADCAST, lccUpCheck) != SUCCESS_XT)
				{
					//todo: fail???
				}

				strncpy(verStr, (const char *)(apPLCRTUUpdateRetBuf + sizeof(packet_head_t) + 15), 4);
				s.Format("%s",verStr);
				pDlg->m_rtu_start_ver = s;
				pDlg->SendMessage(WM_WRITEDATA,0,0); 

				idx = apPLCRTUUpdateGetUidInx();
				//todo: send update info to RTU
				PrintfXTOS("===== ApPLCRTUUpdateProc start to send info idx = %d===== \n", idx);

#if 0
				if(idx < AP_PLC_MAX_NODE)
				{
					apPLCRTUUpdateSendInfoCmd(/*mApPLCData.mNodeList[idx].uId*/miscUid, CMD_UNICAST, lccUpInfo,
						(xt_u8*)&info, sizeof(info));
					miscRTUUpdateStatusIdx ++;
				}
				else
				{
					miscRTUUpdateStatusIdx = 0;
					miscRTUUpdateSt = MISC_UPDATE_ST_GET_STATUS;
				}
#else
				s.Format("SEND INFO -->");
				pDlg->m_result += s;
				pDlg->SendMessage(WM_WRITEDATA,0,0);

				if (apPLCRTUUpdateSendInfoCmd(/*mApPLCData.mNodeList[idx].uId*/miscUid, CMD_UNICAST, lccUpInfo,
					(xt_u8*)&info, sizeof(info)) == SUCCESS_XT)
				{
					miscRTUUpdateSt = MISC_UPDATE_ST_GET_STATUS;
				}
				else{
					//todo: fail???
				}
#endif
			}
			else
			{
				PrintfXTOS("===== ApPLCRTUUpdateProc no need to send info for cur loop ===== \n");

				miscRTUUpdateSt = MISC_UPDATE_ST_GET_STATUS;
			}
		}
		break;
	case MISC_UPDATE_ST_GET_STATUS:
		{
			if(queryFlag == TRUE)
			{
				xt_u32 idx = 0;
				xt_u32 ret = 0;
				xt_u32 fileSize = 0;
				xt_u16 fileVer = 0;
				xt_u16 fileCrc = 0;
				xt_u8* pData = NULL;
				xt_u32 i = 0;

				PrintfXTOS("===== ApPLCRTUUpdateProc start to query data===== \n");

				idx = apPLCRTUUpdateGetUidInx();
				PrintfXTOS("===== ApPLCRTUUpdateProc start to query data idx = %d===== \n", idx);

				s.Format(" GET STATUS --> SENDING DATA ");
				pDlg->m_result += s;
				pDlg->SendMessage(WM_WRITEDATA,0,0); 

				//todo:query package status in RTU to filter send package.
				if(idx < AP_PLC_MAX_NODE)
				{
					ret = apPLCRTUUpdateGetStatusCmd(/*mApPLCData.mNodeList[idx].uId*/miscUid, CMD_UNICAST, lccUpStatus, RTU_UPDATE_PKG_MAP_FIRSTHALF);
					if(ret == SUCCESS_XT)
					{
						light_status_info_t* pStatus = NULL;
						light_update_header_t* pHeader = NULL;

						PrintfXTOS("===== ApPLCRTUUpdateProc start to query data return success ===== \n");

						pData = apPLCRTUUpdateRetBuf + sizeof(packet_head_t);
						pHeader = (light_update_header_t*)pData;
						pStatus = (light_status_info_t*)(pData + sizeof(light_update_header_t));

						if(pHeader->subCmd != MISC_UPDATE_ST_GET_STATUS)
						{
							miscRTUUpdateStatusIdx ++;
							break;
						}

						fileSize = pStatus->fileSize;
						fileVer = pStatus->fileVer;
						fileCrc = pStatus->fileCrc;

						if((fileSize == miscRTUUpdateFileSize || fileSize == miscRTUUpdateFileSizeBak)
							&& fileVer == miscRTUUpdateFileVer
							&&(fileCrc == miscRTUUpdateFileCrc || fileCrc == miscRTUUpdateFileCrcBak)
							&& RTU_UPDATE_PKG_MAP_FIRSTHALF == pStatus->pkgMapIdx)
						{
							PrintfXTOS("===== ApPLCRTUUpdateProc start to query data return correct!!! ===== \n");

							maxPkg = (fileSize % miscRTUUpdatePacketSize == 0)?
								(fileSize / miscRTUUpdatePacketSize):
							(fileSize / miscRTUUpdatePacketSize + 1);

							for(i = 0; i < maxPkg; i++)
							{
								if(apPLCRTUUpdateGetBitInArray(pStatus->pkgMapData, i) == FALSE)
								{
									if(i < minPkg)
										minPkg = i;
									break;
								}
							}
						}

						//s.Format(" SENDING DATA");
						//pDlg->m_result += s;
						//pDlg->SendMessage(WM_WRITEDATA,0,0); 
						miscRTUUpdateSt = MISC_UPDATE_ST_SEND_DATA;
					}
					else
					{
						//todo: fail???
					}
					miscRTUUpdateStatusIdx ++;
				}
				else
				{
					miscRTUUpdateStatusIdx = 0;
					miscRTUUpdateSt = MISC_UPDATE_ST_SEND_DATA;
				}

			}
			else
			{
				PrintfXTOS("===== ApPLCRTUUpdateProc no need to query status for cur loop ===== \n");

				miscRTUUpdateSt = MISC_UPDATE_ST_SEND_DATA;
			}
		}
		break;
	case MISC_UPDATE_ST_SEND_DATA:
		{
			xt_u32 idx = 0;
			xt_u32 id = 0;
			xt_u32 maxSize = 0;

			PrintfXTOS("===== ApPLCRTUUpdateProc wait for delay ===== \n");

			if(ApPLCRTUUpdateDelay() == FAILURE_XT)
			{
				return ret;
			}

			//todo: init package index array

			PrintfXTOS("===== ApPLCRTUUpdateProc start to send data ===== \n");

			maxSize = (miscRTUUpdateFileSize >= miscRTUUpdateFileSizeBak)?
				(miscRTUUpdateFileSize):(miscRTUUpdateFileSizeBak);
			maxPkg = (maxSize % miscRTUUpdatePacketSize == 0)?
				(maxSize / miscRTUUpdatePacketSize):
			(maxSize / miscRTUUpdatePacketSize + 1);

			s.Format("%d",maxPkg - 1);
			pDlg->m_totalpkg = s;
			pDlg->SendMessage(WM_WRITEDATA,0,0); 

			if(minPkg != 0xffffffff)
			{
				miscRTUUpdateCurIndex = minPkg;
				PrintfXTOS("===== ApPLCRTUUpdateProc start to send data idx = %d ===== \n",miscRTUUpdateCurIndex);
				minPkg = 0xffffffff;
			}

			if (miscRTUUpdateCurIndex < maxPkg)
			{
				if(miscRTUUpdateCurIndex == maxPkg - 1)
				{
					toRead = maxSize - miscRTUUpdateCurIndex * miscRTUUpdatePacketSize;
				}
				else
				{
					toRead = miscRTUUpdatePacketSize;
				}

				if(AsStreamSeek(miscRTUUpdateBinSm, miscRTUUpdateCurIndex * miscRTUUpdatePacketSize) > 0)
				{
					len = AsStreamRead(miscRTUUpdateBinSm, buf, toRead);

					if (miscRTUUpdateCurIndex * miscRTUUpdatePacketSize + len >= miscRTUUpdateFileSize)
					{
						len = miscRTUUpdateFileSize - miscRTUUpdateCurIndex * miscRTUUpdatePacketSize;
					}
				}

				if(AsStreamSeek(miscRTUUpdateBinSmBak, miscRTUUpdateCurIndex * miscRTUUpdatePacketSize) > 0)
				{
					lenBak = AsStreamRead(miscRTUUpdateBinSmBak, bufBak, toRead);

					if (miscRTUUpdateCurIndex * miscRTUUpdatePacketSize + lenBak >= miscRTUUpdateFileSizeBak)
					{
						lenBak = miscRTUUpdateFileSizeBak - miscRTUUpdateCurIndex * miscRTUUpdatePacketSize;
					}
				}

				if(1)
				{
					PrintfXTOS("===== ApPLCRTUUpdateProc pkg = %d, len = %d, lenBak = %d===== \n",miscRTUUpdateCurIndex,len,lenBak);
					s.Format("%d",miscRTUUpdateCurIndex);
					pDlg->m_curpkg = s;
					pDlg->SendMessage(WM_WRITEDATA,0,0); 

					if ((len == lenBak) && memcmp(buf, bufBak, len) == 0)
					{
						id = lccUpLocalCommon;
						if(apPLCRTUUpdateSendDataCmd(apPLCRTUUpdateGetSeqUid(), CMD_BROADCAST, lccUpData,
							miscRTUUpdateCurIndex, id, buf, len) != SUCCESS_XT)
						{
							//todo: fail???
						}
					}
					else
					{
						if(len > 0)
						{
							id = lccUpLocalPriv;
							if(apPLCRTUUpdateSendDataCmd(apPLCRTUUpdateGetSeqUid(), CMD_BROADCAST, lccUpData,
								miscRTUUpdateCurIndex, id, buf, len) != SUCCESS_XT)
							{
								//todo: fail???
							}
						}

						if(lenBak > 0)
						{
							if(len > 0)
								sleep_ms(50);

							id = lccUpLocalNext;
							if(apPLCRTUUpdateSendDataCmd(apPLCRTUUpdateGetSeqUid(), CMD_BROADCAST, lccUpData,
								miscRTUUpdateCurIndex, id, bufBak, lenBak)!= SUCCESS_XT)
							{
								//todo: fail???
							}
						}
					}

				}

				miscRTUUpdateCurIndex ++;
			}
			else
			{
				PrintfXTOS("===== ApPLCRTUUpdateProc end of send data ===== \n");
				miscRTUUpdateSt = MISC_UPDATE_ST_SEND_CHECK;
				miscRTUUpdateCurIndex = 0;
			}
		}
		break;
	case MISC_UPDATE_ST_SEND_CHECK:
		{
			char verStr[16] = {0};

			if(ApPLCRTUUpdateDelayTime() == FAILURE_XT)
			{
				return ret;
			}

			PrintfXTOS("===== ApPLCRTUUpdateProc send check req ===== \n");

			s.Format(" --> UPDATE END, CHECKING ");
			pDlg->m_result += s;
			pDlg->SendMessage(WM_WRITEDATA,0,0); 

			//todo :req for data check
			if(apPLCRTUUpdateSendCheckCmd(apPLCRTUUpdateGetSeqUid(), CMD_BROADCAST, lccUpCheck) != SUCCESS_XT)
			{
				//todo: fail???
			}

			strncpy(verStr, (const char *)(apPLCRTUUpdateRetBuf + sizeof(packet_head_t) + 15), 4);
			s.Format("%s",verStr);
			pDlg->m_rtu_ver = s;
			pDlg->SendMessage(WM_WRITEDATA,0,0); 

			miscRTUUpdateSt = MISC_UPDATE_ST_SEND_INFO;
			miscRTUUpdateStatusIdx = 0;
			miscRTUUpdateCurLoop ++;
		}
		break;
	default:
		break;
	}

	//sleep_ms(10);

#else
	xt_s32 ret = FAILURE_XT;
	xt_s32 len = 0;
	xt_u8 buf[QUEUE_ITEM_MAX_SIZE];
	xt_u32 toRead = 0;

	if(miscRTUUpdateFlag == FALSE)
		return ret;

	PrintfXTOS("===== ApPLCRTUUpdateProc Begin ===== \n");

	if (miscRTUUpdateBinSm != NULL)
	{
		PrintfXTOS("===== ApPLCRTUUpdateProc bin s = %d ===== \n",AsStreamSize(miscRTUUpdateBinSm));
		PrintfXTOS("===== ApPLCRTUUpdateProc crc16 s = %d ===== \n",miscRTUUpdatecalccrc);

		if(miscRTUUpdateSt == 0)
		{
			if(ApPLCRTUUpdateDelay() == FAILURE_XT)
			{
				return ret;
			}

			if (miscRTUUpdateOff < miscRTUUpdateFileSize)
			{
				if(miscRTUUpdateFileSize - miscRTUUpdateOff > miscRTUUpdatePacketSize)
					toRead = miscRTUUpdatePacketSize;
				else
					toRead = miscRTUUpdateFileSize - miscRTUUpdateOff;

				AsStreamSeek(miscRTUUpdateBinSm, miscRTUUpdateOff);
				len = miscRTUUpdateOff;
				len = AsStreamRead(miscRTUUpdateBinSm, buf, toRead);

				if(len > 0)
				{
					if(miscRTUUpdateOff == 0)
						UTCrc16((xt_u8 *)&miscRTUUpdatecalccrc,buf,len);
					else
						UTCrc16Separate((xt_u8 *)&miscRTUUpdatecalccrc,buf,len);

					apPLCRTUUpdateSendDataCmd(apPLCRTUUpdateGetUid(), CMD_BROADCAST, lccUpdate,
						miscRTUUpdateOff / miscRTUUpdatePacketSize, buf, len);
				}

				miscRTUUpdateOff += len;
			}
			else
			{
				miscRTUUpdateSt = 1;
			}
		}
	}

	sleep_ms(10);
#endif
}

#endif

DWORD WINAPI rtuUpdateThreadFunc(LPVOID arg)
{
	rtpUpdateThread *pCurThread = (rtpUpdateThread *)arg;
	unsigned char recvBuf[1024];
	int recvLen;
	unsigned char sendBuf[1024];
	int sendLen;
	int i = 0;

	while (TRUE)
	{
		//TRACE("in update proc %s\n", pCurThread->comName);

		if (pCurThread->comName.GetLength() > 0)
		{
			if (hCom == (HANDLE)-1)
			{
				hCom = comOpen( pCurThread->dlg,  pCurThread->comName);
			}
		}

		if (hCom != (HANDLE)-1)
		{
			if (pCurThread->startFlag == 1)
			{
				ApPLCRTUUpdateStart(pCurThread);
				pCurThread->startFlag = 0;
			}
			else if (pCurThread->startFlag == 2)
			{
				ApPLCRTUUpdateStop(pCurThread->dlg);
				pCurThread->startFlag = 0;
			}

			ApPLCRTUUpdateProc(pCurThread->dlg);

			if (hCom == (HANDLE)-1)
			{
				pCurThread->comName = "";
			}

			Sleep(1);
		}
		else
			Sleep(1000);
	}

	ExitThread(1);
	return 1;
}

int rtpUpdateThread::SendCheckCmd(LPVOID arg)
{
	CrtuUpdateToolDlg* pDlg = (CrtuUpdateToolDlg *)arg;
	CString s;
	char verStr[16] = {0};

	if(apPLCRTUUpdateSendCheckCmd(apPLCRTUUpdateGetSeqUid(), CMD_BROADCAST, lccUpCheck) != SUCCESS_XT)
	{
		//todo: fail???
	}

	strncpy(verStr, (const char *)(apPLCRTUUpdateRetBuf + sizeof(packet_head_t) + 15), 4);
	s.Format("%s",verStr);
	pDlg->m_rtu_ver = s;
	pDlg->SendMessage(WM_WRITEDATA,0,0); 

	return SUCCESS_XT;
}

rtpUpdateThread::rtpUpdateThread(LPVOID arg)
{
	this->dlg = arg;
	procThread = CreateThread(NULL,0,rtuUpdateThreadFunc,this,0,NULL);
}


rtpUpdateThread::~rtpUpdateThread(void)
{
}
