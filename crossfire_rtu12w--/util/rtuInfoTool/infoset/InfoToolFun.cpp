
#include"stdafx.h"
#include"test.h"
#include <tchar.h>

rtu_cfg_para_t rtu_cfg_para_get;
port portset;
HANDLE hCom;
OVERLAPPED m_ov;
COMSTAT comstat;
DWORD m_dwCommEvents;
PACK_SET_2 set_pack;
HWND OPEN_PORT_WARNING;

bool openport(_TCHAR *portname)
{

	hCom=CreateFile(
		portname,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		0);
	if(hCom==INVALID_HANDLE_VALUE)
	{
		CloseHandle(hCom);
		MessageBoxA(OPEN_PORT_WARNING,"串口打开失败!","标题",MB_ICONEXCLAMATION|MB_ICONWARNING);
		return false;
	}
	else
		return true;
}

bool setupdcb(BYTE Stopbits,BYTE Parity,DWORD BaudRate)
{ 
	DCB dcb;

	if(!GetCommState(hCom,&dcb))
		return false;
	memset(&dcb,0,sizeof(dcb));
	dcb.DCBlength=sizeof(dcb);
	dcb.BaudRate  =BaudRate;
	dcb.Parity    =Parity;
	dcb.StopBits  =Stopbits;
	dcb.ByteSize  =8;

	if(!SetCommState(hCom,&dcb))
		return false;
	SetupComm(hCom,1024,1024);
	return true;
}

/***************超时限制************************/
bool setuptimeout()
{ 
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout=MAXDWORD;
	timeouts.ReadTotalTimeoutConstant=0;
	timeouts.ReadTotalTimeoutMultiplier=0;
	timeouts.WriteTotalTimeoutConstant=50;
	timeouts.WriteTotalTimeoutMultiplier=2000;
	if(!SetCommTimeouts(hCom,&timeouts))
		return false;
	else
		return true;

}
static char ParseChkSum( unsigned  char* pBuf, int count)
{
	unsigned char chkSum;
	int i;
	chkSum = 0;

	for (i = 0; i < count; i++)
		chkSum += pBuf[i];
	chkSum=chkSum%(0xffffffff);
	return chkSum ;
}
/***************读取配置**********************/
bool PrintInfo(unsigned char *p,int d_len)
{

	unsigned char   *plen;
	plen=p;
	memcpy(set_pack.project_name,plen,6);
	plen+=6;
	memcpy(set_pack.rtu_sn,plen,4);
	plen+=4;
	memcpy(set_pack.hardware_vertion,plen,4);
	plen+=4;
	memcpy(set_pack.software_version,plen,4);
	plen+=4;
	memcpy(&set_pack.lamp_type,plen,2);
	plen+=2;
	memcpy(&set_pack.lamp_num,plen,1);
	plen+=1;
	memcpy(set_pack.rtu_addr,plen,6);
	plen+=6;
	memcpy(set_pack.rtu_group,plen,3);
	plen+=3;
	memcpy(set_pack.rtu_domain,plen,2);
	plen+=2;
	memcpy(&set_pack.pwm_type,plen,1);
	plen+=1;
	memcpy(&set_pack.pwm_frequence,plen,2);
	plen+=2;
	memcpy(&set_pack.pwm_decay_opened,plen,1);
	plen+=1;
	memcpy(&set_pack.pwm_decay_value,plen,1);
	plen+=1;
	memcpy(&set_pack.lamp_swap,plen,1);

	return TRUE;

}

bool IotParse(unsigned char *p,int len)
{
	int i;
	int j=0,n;
	unsigned char ChkSum=0;
	unsigned char ID[6];
	unsigned char *pID,*p2,*p3,*p4,*p5,*p6,*pCmd;
	pID=ID;
	unsigned char RecCmd,RecDataLen;
	int SetReault=0,GetResult=0;
	unsigned char GetBuf[256]={0};

	for(i=0;i<len;i++)
	{
		if(*p==0x68)
		{
			p2=p+7;
			if(*p2==0x68)
			{
				pCmd=p2+3;
				if(*pCmd==0x13)
				{
					memcpy(pID,p+1,6);	
					p3=p2+1;
					memcpy(&RecCmd,p3,1);                          //接收命令
					p4=p2+2;
					memcpy(&RecDataLen,p4,1);                       //数据长度
					p6=p4+2;                                        

					p5=p4+RecDataLen+1;

					n=RecDataLen+10;
					ChkSum=ParseChkSum(p,n);

					if((ChkSum==*p5)&&(*(p5+1)==0x16))

					{
						GetResult=PrintInfo(p6,RecDataLen);
						return TRUE;
					}
				}
			}
			p++;
		}

	}
	return FALSE;
}
/********************接收数据********************/
bool ReceiveChar()
{
	BOOL bRead  =TRUE;
	BOOL bResult=TRUE;
	DWORD dwError=0;
	DWORD j,BytesRead=0;
	unsigned char RXBuff[100]={0};
	unsigned char RecBuff[1024];
	unsigned char* pRec;
	pRec=RecBuff;
	int RceLen=0,IotParseResult,nTimes=0;

	for(;;)
	{
		if(bRead)
		{
			bResult=ReadFile(hCom,RXBuff,sizeof(RXBuff),&BytesRead,NULL);

			for(j=0;j<BytesRead;j++)
			{	
				*pRec=RXBuff[j];
				pRec++;
				RceLen+=1;
			}

			IotParseResult=IotParse(RecBuff,RceLen);
			if(IotParseResult||RceLen==1024)                                //跳出循环
				break;
			if(!bResult)
			{
				switch(dwError=GetLastError())
				{
				case ERROR_IO_PENDING:
					{
						bRead=FALSE;
						break;
					}
				default:
					{
						break;
					}
				}
			}
			else
				bRead=TRUE;
			if(!bRead)
			{	
				bRead=TRUE;
				bResult=GetOverlappedResult(hCom,&m_ov,&BytesRead,TRUE);
			}
			if(nTimes>=1024)
			{
				break;
			}
			nTimes++;
		}
	}

	return TRUE;
}
/*****************写串口数据*****************************/

bool WriteChar( char *buf,int bufLen)
{  
	DWORD BytesSent=0;
	BOOL bResult;
	bResult=WriteFile(hCom,
		buf,
		bufLen,
		&BytesSent,
		NULL);
	if(!bResult)
	{
		return FALSE; 
	}
	else
		return TRUE;
}
/****************检查串口版本号*********************/
typedef struct {
	char	stx;
	char addr[6];
	char stx2;
	char cmd;
	char len;
}pack;  
pack pHead;



static char proPackChkSum(const pack * pHead, const char* pBuf, int count)
{
	char chkSum;
	char *t;
	int i;

	chkSum = 0;
	t = (char*)&pHead->stx;
	for (i = 0; i < sizeof(pack); i++)
		chkSum += *t++;
	for (i = 0; i < count; i++)
		chkSum += pBuf[i];

	return chkSum ;
}
int IotPack(char pHeadaddr,char cmd,const char* data,int len,char *buf,int *pLen)
{


	pHead.stx=START_FLAG;
	pHead.stx2=START_FLAG;
	pHead.cmd=cmd;
	pHead.len=len;
	int n;
	int Chk;

	Chk = proPackChkSum(&pHead,data, len);
	memset(buf,0,sizeof(buf));
	memcpy(buf,&pHead,sizeof(pHead));
	memcpy(buf+sizeof(pHead),data,len);
	n=sizeof(pHead)+len;
	memset(buf+n,Chk,1);
	memset(buf+n+1,TAIL_FLAG,1);
	*pLen = n+2;

	return TRUE;

}

bool CheckCmd(char *pUid,char cmd,const char* pdata,int dataLen)
{

	char buf[256];
	int bufLen = 0;
	bool RecResult=0;

	if(pUid==NULL)
		memset(pHead.addr,0,6);
	else
		memset(pHead.addr,*pUid,sizeof(pUid));

	IotPack(*pHead.addr,cmd,pdata,dataLen,buf,&bufLen); 
	WriteChar(buf,bufLen);
	Sleep(500);
	RecResult=ReceiveChar();

	return RecResult;
}
#define PORT_UNLOCK 0;
#define PORT_LOCK 1;
char Uid[6]={};
bool PortState=PORT_UNLOCK;
unsigned char LastPort[10];

bool RtuInfoGet(_TCHAR *p)
{
	bool GetResult,OpenPortResult;
	DWORD BaudRate;
	BYTE Parity,Stopbits;
	Stopbits=ONESTOPBIT;
	Parity=EVEN;
	BaudRate=115200;
	char cmd=READ;
	const char data=0x13;

	if(memcmp(LastPort,(_TCHAR*)portset.COM,strlen((_TCHAR*)portset.COM)))
	{
		PortState=PORT_UNLOCK;
		CloseHandle(hCom); 
	}
	if(!PortState)
	{
		OpenPortResult=openport(p);
		if(OpenPortResult)
		{
			PurgeComm((_TCHAR*)portset.COM,PURGE_RXCLEAR|PURGE_TXCLEAR|PURGE_RXABORT|PURGE_TXABORT);
			setupdcb(Stopbits,Parity,BaudRate);
			setuptimeout();
			memcpy(LastPort,(_TCHAR*)portset.COM,strlen((_TCHAR*)portset.COM));
			PortState=PORT_LOCK;
		}
	}	

	GetResult=CheckCmd(Uid,cmd,&data,sizeof(data));
	return GetResult;
}

typedef struct
{
	unsigned char stx_1;
	unsigned char UID[6];
	unsigned char stx_2;
	unsigned char cmd;
	unsigned int data_len;
}PACK_SET_1;

bool SetChar(unsigned char *buf,int bufLen)
{  

	DWORD BytesSent=0;
	BOOL bResult;
	bResult=WriteFile(hCom,
		buf,
		bufLen,
		&BytesSent,
		NULL);
	if(!bResult)
	{
		return FALSE; 
	}
	else
		return TRUE;
}

bool IotParseSet(unsigned char *p,int len)
{
	int i;
	int j=0,n;
	unsigned char ChkSum=0;
	unsigned char GetBuf[256]={0};

	for(i=0;i<len;i++)
	{
		if(*p==0x68)
		{
			if(*(p+7)==0x68)
			{
				n=*(p+9)+10;
				ChkSum=ParseChkSum(p,n);
				if((ChkSum==*(p+12))&&(*(p+13)==0x16))
				{
					if(*(p+11)==1)
					{
						MessageBoxA(OPEN_PORT_WARNING,"设置成功!","参数设置",MB_ICONQUESTION|MB_ICONWARNING);
					}
					else 
					{
						MessageBoxA(OPEN_PORT_WARNING,"设置失败!","参数设置",MB_ICONEXCLAMATION|MB_ICONWARNING);
					}
					return TRUE;
				}
			}
		}
		p++;
	}	
	return FALSE;
}

void InfoSetReply()
{
	BOOL bRead  =TRUE;
	BOOL bResult=TRUE;
	DWORD dwError=0;
	DWORD j,BytesRead=0;
	unsigned char RXBuff[100]={0};
	unsigned char RecBuff[1024];
	unsigned char* pRec;
	pRec=RecBuff;
	int RceLen=0,IotParseResult,nTimes=0;
	//bool IotParseResult=0;

	for(;;)
	{
		if(bRead)
		{
			bResult=ReadFile(hCom,RXBuff,sizeof(RXBuff),&BytesRead,NULL);

			for(j=0;j<BytesRead;j++)
			{	
				*pRec=RXBuff[j];
				pRec++;
				RceLen+=1;
			}
			IotParseResult=IotParseSet(RecBuff,RceLen);
			if(IotParseResult||RceLen==1024)                                //跳出循环
				break;
			if(!bResult)
			{
				switch(dwError=GetLastError())
				{
				case ERROR_IO_PENDING:
					{
						bRead=FALSE;
						break;
					}
				default:
					{
						break;
					}
				}
			}
			else
				bRead=TRUE;
			if(!bRead)
			{	
				bRead=TRUE;
				bResult=GetOverlappedResult(hCom,&m_ov,&BytesRead,TRUE);
			}
			if(nTimes>=1024)
			{
				break;
			}
			nTimes++;
		}
	}
}
bool   RtuInfoSet()
{
	PACK_SET_1 set_pack_head;
	unsigned char setbuf[256],RecDataLen=0x27;
	int len,Chksum;
	bool SetResult=0;

	set_pack_head.stx_1=0x68;
	set_pack_head.stx_2=0x68;
	set_pack_head.data_len=RecDataLen;
	memcpy(set_pack_head.UID,Uid,6);
	memset(&set_pack_head.cmd,0x01,1);
	memset(&set_pack.cmd,INSETCMD,1);

	len=0;
	memcpy(setbuf,&set_pack_head.stx_1,1);	
	len+=1;
	memcpy(setbuf+len,set_pack_head.UID,6);
	len+=6;
	memcpy(setbuf+len,&set_pack_head.stx_2,1);	
	len+=1;
	memcpy(setbuf+len,&set_pack_head.cmd,1);	
	len+=1;
	memcpy(setbuf+len,&set_pack_head.data_len,1);
	len+=1;

	memcpy(setbuf+len,&set_pack.cmd,sizeof(set_pack.cmd));
	len+=sizeof(set_pack.cmd);
	memcpy(setbuf+len,&set_pack.project_name,sizeof(set_pack.project_name));
	len+=sizeof(set_pack.project_name);
	memcpy(setbuf+len,&set_pack.rtu_sn,sizeof(set_pack.rtu_sn));
	len+=sizeof(set_pack.rtu_sn);
	memcpy(setbuf+len,&set_pack.hardware_vertion,sizeof(set_pack.hardware_vertion));
	len+=sizeof(set_pack.hardware_vertion);
	memcpy(setbuf+len,&set_pack.software_version,sizeof(set_pack.software_version));
	len+=sizeof(set_pack.software_version);
	memcpy(setbuf+len,&set_pack.lamp_type,sizeof(set_pack.lamp_type));
	len+=sizeof(set_pack.lamp_type);
	memcpy(setbuf+len,&set_pack.lamp_num,sizeof(set_pack.lamp_num));
	len+=sizeof(set_pack.lamp_num);
	memcpy(setbuf+len,&set_pack.rtu_addr,sizeof(set_pack.rtu_addr));
	len+=sizeof(set_pack.rtu_addr);
	memcpy(setbuf+len,&set_pack.rtu_group,sizeof(set_pack.rtu_group));
	len+=sizeof(set_pack.rtu_group);
	memcpy(setbuf+len,&set_pack.rtu_domain,sizeof(set_pack.rtu_domain));	
	len+=sizeof(set_pack.rtu_domain);
	memcpy(setbuf+len,&set_pack.pwm_type,sizeof(set_pack.pwm_type));
	len+=sizeof(set_pack.pwm_type);
	memcpy(setbuf+len,&set_pack.pwm_frequence,sizeof(set_pack.pwm_frequence));
	len+=sizeof(set_pack.pwm_frequence);
	memcpy(setbuf+len,&set_pack.pwm_decay_opened,sizeof(set_pack.pwm_decay_opened));
	len+=sizeof(set_pack.pwm_decay_opened);
	memcpy(setbuf+len,&set_pack.pwm_decay_value,sizeof(set_pack.pwm_decay_value));
	len+=sizeof(set_pack.pwm_decay_value);
	memcpy(setbuf+len,&set_pack.lamp_swap,sizeof(set_pack.lamp_swap));
	len+=sizeof(set_pack.lamp_swap);	

	Chksum=ParseChkSum(setbuf, len);
	memcpy(setbuf+len,&Chksum,1);
	len+=1;
	memset(setbuf+len,TAIL_FLAG,1);
	len+=1;

	SetResult=SetChar(setbuf,len);
	Sleep(1000);
	InfoSetReply();
	return SetResult;
}

bool TwoASCtoHEX(unsigned char* psetbuf,unsigned char *pgetbuf,int length)
{
	if((length%2)!=0)
		return FALSE;
	while(length!=0)
	{

		if(('0'<=*pgetbuf)&&(*pgetbuf<='9'))
		{
			*psetbuf=*pgetbuf%(0x30);
		}
		else if(('a'<=*pgetbuf)&&(*pgetbuf<='f'))
		{
			*psetbuf=*pgetbuf%(0x57);
		}
		else if(('A'<=*pgetbuf)&&(*pgetbuf<='F'))
		{
			*psetbuf=*pgetbuf%(0x37);
		}
		*psetbuf*=16;
		pgetbuf++;
		length--;
		if(('0'<=*pgetbuf)&&(*pgetbuf<='9'))
		{
			*psetbuf+=*pgetbuf%(0x30);
		}
		else if(('a'<=*pgetbuf)&&(*pgetbuf<='f'))
		{
			*psetbuf+=*pgetbuf%(0x57);
		}
		else if(('A'<=*pgetbuf)&&(*pgetbuf<='F'))
		{
			*psetbuf+=*pgetbuf%(0x37);
		}
		psetbuf++;
		pgetbuf++;
		length--;
	}
	return TRUE;
}

void HEX2Asc(unsigned char* psetbuf,unsigned char *pgetbuf,int length)
{
	while(length>0)
	{
		if(*pgetbuf!=0)
		{
			while(length>0)
			{
				if((0<=(*pgetbuf/16))&&((*pgetbuf/16)<=9))
				{
					*psetbuf=(*pgetbuf/16)+(0x30);
				}
				else if((0x0a<=(*pgetbuf/16))&&((*pgetbuf/16)<=0x0f))
				{
					*psetbuf=(*pgetbuf/16)+(0x37);
				}
				*psetbuf++;
				if((0<=(*pgetbuf%16))&&((*pgetbuf%16)<=9))
				{
					*psetbuf=(*pgetbuf%16)+(0x30);
				}
				else if((0x0a<=(*pgetbuf%16))&&((*pgetbuf%16)<=0x0f))
				{
					*psetbuf=(*pgetbuf%16)+(0x37);
				}
				*psetbuf++;
				*pgetbuf++;
				length--;
			}
		}
		*pgetbuf++;
		length--;
	}
}

void CutZero(unsigned char* psrc,unsigned char* ptargt,int length)
{
	while(length>0)
	{
		if(*psrc!=0)
		{
			while(length>0)
			{
				*ptargt=*psrc;
				ptargt++;
				psrc++;
				length--;
			}
		}
		else
			psrc++;
		length--;
	}
}

