// IotPackage.h: interface for the CIotPackage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOTPACKAGE_H__31B77673_AFFB_4057_AD6B_583086CCBC35__INCLUDED_)
#define AFX_IOTPACKAGE_H__31B77673_AFFB_4057_AD6B_583086CCBC35__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdlib.h>
#include <time.h>
#include "zlib.h"

#define PACK_MAX_DIR_LEN 32

#pragma pack(push,1)
typedef struct 
{
	unsigned int	packMagic;
	unsigned int	packSize;//??
	unsigned int	packTime;
	unsigned int	packCnt;
	unsigned char	packDir[PACK_MAX_DIR_LEN];
	unsigned char	packRes[16];
}iot_pack_head_t;

typedef struct 
{
	unsigned int	peSize;
	unsigned int	peCrc;//??
	unsigned int	peOffset;//??
	unsigned int	peTime;//??
	unsigned char	peType;//0:app 1:file
	unsigned char	peLocal;//0:fs 1:bin addr
	unsigned char	packRes[46];
}iot_pack_entry_t;

typedef struct 
{
	int	magic;
}iot_version_t;

#pragma pack (pop)

class CIotPackage  
{
public:
	static int IotPkgRun(char *pCur,char *pDirName,char *pVerName);
	static bool IotPkgCheckParam(char *pCur);
	CIotPackage();
	virtual ~CIotPackage();

};

#endif // !defined(AFX_IOTPACKAGE_H__31B77673_AFFB_4057_AD6B_583086CCBC35__INCLUDED_)
