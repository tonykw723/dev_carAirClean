// IotPackage.cpp: implementation of the CIotPackage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "rtuReleaseTool.h"
#include "IotPackage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIotPackage::CIotPackage()
{

}

CIotPackage::~CIotPackage()
{

}

bool CIotPackage::IotPkgCheckParam(char *pCur)
{
	bool ret = FALSE;
	for (int i = 0; i < __argc; i++)
	{
		TRACE("p[%d] = %s\n", i, __argv[i]);
	}

	if (__argc > 1)
	{
		ret = TRUE;
		if (__argc > 2)
			CIotPackage::IotPkgRun(pCur, __argv[1], __argv[2]);
		else
			CIotPackage::IotPkgRun(pCur, __argv[1], "0");
	}
	else
		ret = FALSE;

	return ret;
}

unsigned char auchCRCHi[]={
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

unsigned char auchCRCLo[]={
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

int UTCrc16(unsigned char *crc,unsigned char *puchMsg, int usDataLen)
{ 
	unsigned char uchCRCHi = 0xFF ; /* 低CRC 字节初始化 */ 
	unsigned char uchCRCLo = 0xFF ; 
	unsigned int uIndex ;			/* CRC循环中的索引 */ 
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


int CIotPackage::IotPkgRun(char *pCur,char *pDirName,char *pVerName)
{
	char pBuf[MAX_PATH];
	char pFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData; 
	HANDLE hFind;
	iot_pack_head_t phSt = {0};
	iot_pack_entry_t peSt = {0};
	char rFileName[MAX_PATH];
	char wFileName[MAX_PATH];
	char tmpName[MAX_PATH];
	char tmpIniName[MAX_PATH];
	int iniFileSize = 0;
	int iniFileSizeBak = 0;
	int iniFileCrc = 0;
	int iniFileCrcBak = 0;
	int iniFileVer = 0;
	FILE* rfd = NULL;
	FILE* wfd = NULL;
	FILE* inifd = NULL;
	int fileCnt = 0;
	int fileSeek = 0;
	CString cmdLine;
	char* pDataBuf = NULL;
	int dataLen = 0;
	unsigned short crc16 = 0;
	
	//GetCurrentDirectory(MAX_PATH,pFind);
	strcpy(pFind,pCur);
	//GetCurrentDirectory(MAX_PATH,pBuf);
	strcpy(pBuf,pCur);
	strcat(pFind,"\\");
	strcat(pFind,pDirName);
	strcat(pFind,"\\*.*");

	sprintf(tmpName,"%s\\upCfg.ini",pCur);
	cmdLine = "del \""+ CString(tmpName) + "\" /f /q";
	system(cmdLine);

	iniFileVer = atoi(pVerName);
	sprintf(tmpIniName,"%s\\nand\\crossfire.ini",pCur);

	sprintf(tmpName,"%s\\cc09.pkg",pCur);
	wfd = fopen(tmpName, "wb+");
	if (wfd != NULL) {
		
		hFind = FindFirstFile(pFind, &FindFileData);    
		if(INVALID_HANDLE_VALUE == hFind)    
			return 1;        
		while(TRUE)    
		{
			if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{   
				iot_pack_entry_t packItem = {0};
				char * pBuff = NULL;
				if (fileSeek == 0)
				{
					fseek(wfd,sizeof(iot_pack_head_t),SEEK_SET);
					fileSeek = 1;
				}
				printf("%s\\%s\\%s\n",pBuf,pDirName,FindFileData.cFileName);
				sprintf(rFileName,"%s\\%s\\%s",pBuf,pDirName,FindFileData.cFileName);
				
				if(strcmp(FindFileData.cFileName,"crossfire.ini") == 0)
				{
					if(iniFileSize >0)
					{
						char tmpStr[64] = {0};
						sprintf(tmpStr,"%d",iniFileSize);
						WritePrivateProfileString("RTUUpdateFile","fileSize",tmpStr,tmpIniName);
						sprintf(tmpStr,"%d",iniFileCrc);
						WritePrivateProfileString("RTUUpdateFile","fileCrc",tmpStr,tmpIniName);
					}

					if(iniFileSizeBak >0)
					{
						char tmpStr[64] = {0};
						sprintf(tmpStr,"%d",iniFileSizeBak);
						WritePrivateProfileString("RTUUpdateFile","fileSizeBak",tmpStr,tmpIniName);
						sprintf(tmpStr,"%d",iniFileCrcBak);
						WritePrivateProfileString("RTUUpdateFile","fileCrcBak",tmpStr,tmpIniName);
					}

					if(iniFileVer > 0)
					{
						char tmpStr[64] = {0};
						sprintf(tmpStr,"%d",iniFileVer);
						WritePrivateProfileString("RTUUpdateFile","fileVer",tmpStr,tmpIniName);
					}
					else
					{
						//todo:
					}
				}

				rfd = fopen(rFileName, "rb+");
				if (rfd != NULL) {

					fileCnt ++;
					fseek(rfd,0,SEEK_END);
					packItem.peSize = ftell(rfd);

					pDataBuf = (char *)malloc(packItem.peSize);
					fseek(rfd,0,SEEK_SET);
					dataLen = fread(pDataBuf,1,packItem.peSize,rfd);
					if (dataLen == packItem.peSize)
					{
						UTCrc16((unsigned char *)&crc16,(unsigned char *)pDataBuf,dataLen);
						packItem.peCrc = crc16;
					}
					
					if(strcmp(FindFileData.cFileName,"crossfire.bin") == 0)
					{
						iniFileSize = packItem.peSize;
						iniFileCrc = packItem.peCrc;
					}

					if(strcmp(FindFileData.cFileName,"cf.bin") == 0)
					{
						iniFileSizeBak = packItem.peSize;
						iniFileCrcBak = packItem.peCrc;
					}

					//todo:check param for different application

					char secName[64] = {0};
					char keyName[64] = {0};
					char secSize[64] = {0};
					char secCrc16[64] = {0};
					char fileName[PACK_MAX_DIR_LEN] = {0};

					sprintf(secName,"upItem%d",fileCnt);
					sprintf(secSize,"%d",packItem.peSize);
					sprintf(secCrc16,"%d",packItem.peCrc);
					sprintf(tmpName,"%s\\upCfg.ini",pCur);
					strncpy(fileName,FindFileData.cFileName,PACK_MAX_DIR_LEN);


					sprintf(keyName,"upSize%d",fileCnt);
					WritePrivateProfileString(secName,keyName,secSize,tmpName);

					sprintf(keyName,"upCrc%d",fileCnt);
					WritePrivateProfileString(secName,keyName,secCrc16,tmpName);

					sprintf(keyName,"upName%d",fileCnt);
					WritePrivateProfileString(secName,keyName,fileName,tmpName);
					
					/*packItem.peType = 0;
					packItem.peLocal = 0;
					strncpy((char *)packItem.packRes,FindFileData.cFileName,PACK_MAX_DIR_LEN);
					fseek(rfd,0,SEEK_SET);

					fwrite(&packItem,sizeof(packItem),1,wfd);

					if (packItem.peSize > 0)
					{
						pBuff = (char *)malloc(packItem.peSize);
						fread(pBuff,packItem.peSize,1,rfd);
						fwrite(pBuff,packItem.peSize,1,wfd);
						free(pBuff);
					}*/

					free(pDataBuf);
					fclose(rfd);
				}
				//todo:
			}
			
			if(!FindNextFile(hFind,&FindFileData))
				break;    
		}    
		
		FindClose(hFind);

		if (fileSeek == 1)
		{
			iot_pack_head_t packHead = {0};
			char pMagic[] = {"CC09"};
			time_t now;
			int iniSize = 0;
			char * pBuff = NULL;

			packHead.packCnt = fileCnt;
			memcpy(&packHead.packMagic,pMagic,sizeof(int));
			strncpy((char *)packHead.packDir,pDirName,PACK_MAX_DIR_LEN);
			time(&now);
			packHead.packTime = now;

			sprintf(tmpName,"%s\\upCfg.ini",pCur);
			inifd = fopen(tmpName, "rb+");
			if (inifd != NULL) {
				fseek(inifd,0,SEEK_END);
				iniSize = ftell(inifd);

				if (iniSize > 0)
				{
					fseek(inifd,0,SEEK_SET);
					pBuff = (char *)malloc(iniSize);
					fread(pBuff,iniSize,1,inifd);
					fwrite(pBuff,iniSize,1,wfd);
					free(pBuff);
				}

				packHead.packSize = iniSize;
				fclose(inifd);
			}

			hFind = FindFirstFile(pFind, &FindFileData);    
			if(INVALID_HANDLE_VALUE == hFind)    
				return 1;        
			while(TRUE)    
			{
				if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{   
					char * pBuff = NULL;
					int itemSize = 0;
					printf("%s\\%s\\%s\n",pBuf,pDirName,FindFileData.cFileName);
					sprintf(rFileName,"%s\\%s\\%s",pBuf,pDirName,FindFileData.cFileName);
					
					rfd = fopen(rFileName, "rb+");
					if (rfd != NULL) {

						fseek(rfd,0,SEEK_END);
						itemSize = ftell(rfd);

						if (itemSize > 0)
						{
							fseek(rfd,0,SEEK_SET);
							pBuff = (char *)malloc(itemSize);
							fread(pBuff,itemSize,1,rfd);
							fwrite(pBuff,itemSize,1,wfd);
							free(pBuff);
						}

						fclose(rfd);
					}
					//todo:
				}
				
				if(!FindNextFile(hFind,&FindFileData))
					break;    
			}    
			
			FindClose(hFind);

			sprintf(tmpName,"%s\\upCfg.ini",pCur);
			cmdLine = "del \""+ CString(tmpName) + "\" /f /q";
			system(cmdLine);

			fseek(wfd, 0, SEEK_SET);
			fwrite(&packHead,sizeof(packHead),1,wfd);
		}
		fclose(wfd);

		//zlib
		FILE* fd;
		FILE* zfd;
		int fLen;
		int rLen;
		int zLen;
		int wLen;
		unsigned char* pInBuff;
		unsigned char* pOutBuff;
		iot_version_t verHeader = {0};
		
		sprintf(tmpName,"%s\\cc09.pkg",pCur);
		fd = fopen(tmpName, "rb+");
		if (fd != NULL) {
			//printf("file exist\n");
			fseek(fd, 0, SEEK_END);
			fLen = ftell(fd);
			//printf("file flen = %d\n", fLen);
			
			if (fLen > 0) 
			{
				pInBuff = (unsigned char*)malloc(fLen);
				memset(pInBuff,0,fLen);
				if (pInBuff == NULL)
					return 1;
				
				fseek(fd, 0 , SEEK_SET);
				
				/*for (int k=0;k<fLen;k++)
				{
					rLen = fread(pInBuff + k, sizeof(unsigned char), 1, fd);	
				}*/
				//rLen = fLen;

				rLen = fread(pInBuff, sizeof(unsigned char), fLen, fd);
				if (rLen != fLen)
				{
					fclose(fd);
					//printf("read file data err, fd=%d rL=%d !\n", fd, rLen);
					return 1;
				}
				
				//printf("file rlen = %d\n", rLen);
				
				pOutBuff = (unsigned char*)malloc(rLen);
				
				if (pOutBuff == NULL)
					return 1;
				
				zLen = rLen;
				compress(pOutBuff, (unsigned long*)&zLen, pInBuff, rLen);
				//printf("compress len %d \n",zLen);
				
				/*unsigned char* pNewBuff = (unsigned char*)malloc(4 * 1024 * 1024);
				int newLen;
				uncompress(pNewBuff, (unsigned long*)&newLen, pOutBuff, zLen);*/

				sprintf(tmpName,"%s\\cc09.z",pCur);
				zfd = fopen(tmpName, "wb+");
				
				verHeader.magic = 0x39304343;
				
				wLen = fwrite(&verHeader, 1, sizeof(verHeader), zfd);	
				if (wLen != sizeof(verHeader))
				{
					fclose(zfd);
					//printf("write file header err, fd=%d wL=%d !\n", zfd, wLen);
					return 1;
				}
				
				wLen = fwrite(pOutBuff, sizeof(unsigned char), zLen, zfd);	
				if (wLen != zLen)
				{
					fclose(zfd);
					//printf("write file data err, fd=%d wL=%d !\n", zfd, wLen);
					return 1;
				}
				
				fclose(zfd);
				//printf("file wlen = %d\n", wLen);
			}
			
		}

		fclose(fd);
	}

	return 0;
}
