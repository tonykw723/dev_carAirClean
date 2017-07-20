
#ifndef _X11BASE_H
#define _X11BASE_H

typedef unsigned char   AT_BOOL;
typedef unsigned char   AT_BYTE;
typedef unsigned char*  AT_PBYTE;
typedef char*           	AT_PCHAR;
typedef void*           	AT_PVOID;

typedef int 			AT_COORD;
typedef unsigned int	AT_COLOR;
typedef unsigned int	AT_SIZE;
typedef long long		AT_INT64;

typedef unsigned char   	AT_U8;
typedef unsigned short   	AT_U16;
typedef unsigned int   		AT_U32;
typedef unsigned long long   AT_U64;

typedef char   	AT_CHAR;
typedef short   	AT_SHORT;
typedef int   		AT_INT;

#ifdef _DLL_
#define DLL_EXP _declspec(dllexport)
#else
#define DLL_EXP _declspec(dllimport)
#endif

#ifndef IN
	#define IN
#endif
#ifndef OUT
	#define OUT
#endif


#ifndef WIN32
#define NONDLL 1
#endif /* WIN32 */

#ifndef AD_DECLARE
#ifdef WIN32
//#define AD_DECLARE  DLL_EXP _stdcall
#define AD_DECLARE  
#else
#define AD_DECLARE
#endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif


#define BYTE_BIT_SIZE                           8
#define BYTE_BIT_INC                            (BYTE_BIT_SIZE - 1)
#define WORD_BYTE_SIZE                          4               //sizeof(int)
#define WORD_BIT_SIZE                           (WORD_BYTE_SIZE * BYTE_BIT_SIZE)
#define WORD_BIT_INC                           (WORD_BIT_SIZE - 1)     //32 - 1

#define ASTREAM_RW_BUFFER_SIZE                  1024
#define ASTREAM_FIND_BUFFER_SIZE                32



typedef struct _astream_t ASTREAM,*PASTREAM;
typedef int ( *FP_AS_READ)(PASTREAM sm,unsigned char* data,unsigned long len);
typedef int ( *FP_AS_WRITE)(PASTREAM sm,unsigned char* data,unsigned long len);
typedef int ( *FP_AS_SEEK)(PASTREAM sm,unsigned long offset);
typedef int ( *FP_AS_RESIZE)(PASTREAM sm,unsigned long size);
typedef int ( *FP_AS_CLOSE)(PASTREAM sm);

struct  _astream_t
{

    void*             handle;
    unsigned char*       memory;

    unsigned long        size;
    unsigned long        maxsize;
    unsigned long        pos;
    unsigned long        pagesize;

    FP_AS_READ        read;
    FP_AS_WRITE       write;
    FP_AS_SEEK        seek;
    FP_AS_RESIZE      resize;
    FP_AS_CLOSE       close;


};


AD_DECLARE  PASTREAM  AsStreamOpenFromMemory(unsigned char* buffer,unsigned long datasize,unsigned long buffersize);
AD_DECLARE  PASTREAM  AsStreamOpenFromFile(const char* filename,const char *mode);
AD_DECLARE  PASTREAM  AsStreamOpenFromAutomem(void);
AD_DECLARE  PASTREAM  AsStreamOpenFromAutomemEx(int PagesSize);
AD_DECLARE  PASTREAM  AsStreamOpenFromStream(PASTREAM psm,unsigned long ppos,unsigned long datasize,unsigned long maxsize);
//AD_DECLARE  PASTREAM  AsStreamOpenFromWinFile(const char* filename,const char *mode);

AD_DECLARE  int AsStreamClose(PASTREAM sm);

AD_DECLARE  int AsStreamSaveToFile(PASTREAM sm,const char* filename);
AD_DECLARE  int AsStreamSavePartToFile(PASTREAM sm,const char* filename,unsigned long   pos,unsigned long   count);

AD_DECLARE  int AsStreamSeek(PASTREAM sm,unsigned long  pos);
AD_DECLARE  int AsStreamPos(PASTREAM sm);
AD_DECLARE  int AsStreamSize(PASTREAM sm);
AD_DECLARE  int AsStreamResize(PASTREAM sm,unsigned long  size);
AD_DECLARE  int AsStreamSet(PASTREAM sm,unsigned long   pos,unsigned char ch,unsigned long  size);
AD_DECLARE  AT_PBYTE AsStreamMemory(PASTREAM sm);			//dont try to write to this address
AD_DECLARE  int AsStreamFindChar(PASTREAM sm,unsigned long   stp,unsigned long   etp,unsigned char ch);
AD_DECLARE  int AsStreamFind(PASTREAM sm,unsigned long   stp,unsigned long   etp,unsigned char*   buffer,unsigned long   count );

AD_DECLARE  int AsStreamRead(PASTREAM sm, unsigned char*   buffer,unsigned long   count );
AD_DECLARE  int AsStreamReadAt(PASTREAM sm,unsigned long   pos,unsigned char*   buffer,unsigned long   count );
AD_DECLARE  int AsStreamReadChar(PASTREAM sm,unsigned char*   ch);
AD_DECLARE  int AsStreamReadCharAt(PASTREAM sm,unsigned long   pos,unsigned char*   ch );
AD_DECLARE  int AsStreamReadShort(PASTREAM sm,unsigned short*  sh);
AD_DECLARE  int AsStreamReadShortAt(PASTREAM sm,unsigned long   pos,unsigned short*   sh );
AD_DECLARE  int AsStreamReadInt(PASTREAM sm,unsigned int*   it);
AD_DECLARE  int AsStreamReadIntAt(PASTREAM sm,unsigned long   pos,unsigned int*   it );

AD_DECLARE  int AsStreamWrite(PASTREAM sm, unsigned char*   buffer,unsigned long   count );
AD_DECLARE  int AsStreamWriteAt(PASTREAM sm,unsigned long   pos,unsigned char*   buffer,unsigned long   count );
AD_DECLARE  int AsStreamWriteChar(PASTREAM sm,unsigned char   ch);
AD_DECLARE  int AsStreamWriteCharAt(PASTREAM sm,unsigned long   pos,unsigned char   ch );
AD_DECLARE  int AsStreamWriteShort(PASTREAM sm,unsigned short   sh);
AD_DECLARE  int AsStreamWriteShortAt(PASTREAM sm,unsigned long   pos,unsigned short   sh );
AD_DECLARE  int AsStreamWriteInt(PASTREAM sm,unsigned int   it);
AD_DECLARE  int AsStreamWriteIntAt(PASTREAM sm,unsigned long   pos,unsigned int   it );

AD_DECLARE  int AsStreamCopy(PASTREAM dsm,unsigned long dpos,PASTREAM ssm,unsigned long spos,unsigned long   count );
AD_DECLARE  int AsStreamDup(PASTREAM dsm,PASTREAM ssm);

AD_DECLARE  int AsStreamInsertBuffer(PASTREAM sm,unsigned long pos,unsigned long count ,unsigned char* buff );
AD_DECLARE  int AsStreamInsert(PASTREAM sm,unsigned long pos,unsigned long count );
AD_DECLARE  int AsStreamRemove(PASTREAM sm,unsigned long pos,unsigned long count );
AD_DECLARE  int AsStreamReplace(PASTREAM sm,unsigned long   pos,unsigned long   slen,unsigned char*   buffer,unsigned long   dlen );

AD_DECLARE  AT_PBYTE AsBinToHex(unsigned char ch,short *hstr);
AD_DECLARE  char AsHexToBin(char hc);
AD_DECLARE  char AsHexToChar(const short* hstr);

typedef struct _asfini_t ASFINI,*PASFINI;

struct  _asfini_t
{
    PASTREAM            sm;


    char*           cursect;
    char*           curkey;

    unsigned long   secthead;
    unsigned long   sectbegin;
    unsigned long   sectend;

    unsigned long   keyhead;
    unsigned long   keybegin;
    unsigned long   keyend;

};

AD_DECLARE  PASFINI  AsFIniOpenFromStream(PASTREAM sm);
AD_DECLARE  int  AsFIniClose(PASFINI ini);

AD_DECLARE  int AsFIniGetKeyValueSize(PASFINI ini, const char* pSection,const char* pKey);

AD_DECLARE  int AsFIniReadKeyValueDef(PASFINI ini, const char* pSection,const char* pKey,char* vbuf,unsigned long buflen,char* def);
AD_DECLARE  int AsFIniReadKeyIntDef(PASFINI ini, const char* pSection,const char* pKey,unsigned int* value,unsigned int def);
AD_DECLARE  int AsFIniReadKeyValue(PASFINI ini, const char* pSection,const char* pKey,char* vbuf,unsigned long buflen);
AD_DECLARE  int AsFIniReadKeyStream(PASFINI ini, const char* pSection,const char* pKey,PASTREAM sm);

AD_DECLARE  int AsFIniWriteKeyValue(PASFINI ini, const char* pSection,const char* pKey,const char* vbuf);
AD_DECLARE  int AsFIniWriteKeyInt(PASFINI ini, const char* pSection,const char* pKey, int value);
AD_DECLARE  int AsFIniWriteKeyStream(PASFINI ini, const char* pSection,const char* pKey,PASTREAM sm);

AD_DECLARE  int AsFIniDeleteKey(PASFINI ini, const char* pSection,const char* pKey);
AD_DECLARE  int AsFIniDeleteSection(PASFINI ini, const char* pSection);






/***********************************************************************/
/*
*     bbit       1 << number % BYTE
*            [7][6][5][4][3][2][1][0][7][6][5][4][3][2][1][0]
*
*     abit       0x80 >> number % BYTE
*            [0][1][2][3][4][5][6][7][0][1][2][3][4][5][6][7]
*/
AD_DECLARE int AsABitGet(IN const unsigned char *set,IN int number);
AD_DECLARE void AsABitSet(IN OUT unsigned char *set,IN int number,IN int value);
AD_DECLARE void AsABitFlip(IN OUT unsigned char *set,IN int number);
AD_DECLARE int AsBBitGet(IN const unsigned char *set,IN int number);
AD_DECLARE void AsBBitSet(IN OUT unsigned char *set,IN int number,IN int value);
AD_DECLARE void AsBBitFlip(IN OUT unsigned char *set,IN int number);


typedef struct _asslist_t ASSLIST,*PASSLIST;

struct _asslist_t
{

	struct _asslist_t*	next;
	AT_PVOID	data;
};

AD_DECLARE PASSLIST  AsSlistNew( AT_PVOID data );
AD_DECLARE PASSLIST AsSlistInsert( PASSLIST curItem, PASSLIST newItem );
AD_DECLARE PASSLIST AsSlistRemove( PASSLIST head, PASSLIST item );
AD_DECLARE PASSLIST AsSlistTail( PASSLIST head );
AD_DECLARE PASSLIST AsSlistFind( PASSLIST head, void* data);
AD_DECLARE void AsSlistFree( PASSLIST head ,AT_BOOL freedata);



typedef struct _asqueue_t ASQUEUE,*PASQUEUE;

struct _asqueue_t
{
	PASSLIST			head;
	PASSLIST			tail;
	unsigned int		length;
};


AD_DECLARE PASQUEUE AsQueueNew(void);
AD_DECLARE int AsQueuePush(PASQUEUE q, AT_PVOID data);
AD_DECLARE int AsQueuePop(PASQUEUE q);
AD_DECLARE AT_PVOID AsQueueHead(PASQUEUE q);
AD_DECLARE AT_PVOID AsQueueTail(PASQUEUE q);
AD_DECLARE int AsQueueEmpty(PASQUEUE q);
AD_DECLARE unsigned int AsQueueLength(PASQUEUE q);
AD_DECLARE int AsQueueFree(PASQUEUE q,AT_BOOL freedata);



typedef struct _asdlist_t ASDLIST,*PASDLIST;

struct _asdlist_t
{

	struct _asdlist_t*	prev;
	struct _asdlist_t*	next;
	AT_PVOID	data;
};

AD_DECLARE PASDLIST  AsDlistNew( AT_PVOID data );
AD_DECLARE void AsDlistFree( PASDLIST any ,AT_BOOL freedata);



typedef struct _asstack_t ASSTACK,*PASSTACK;

struct _asstack_t
{
	PASDLIST			tail;
	unsigned int		length;
};


AD_DECLARE PASSTACK AsStackNew(void);
AD_DECLARE int AsStackPush(PASSTACK q, AT_PVOID data);
AD_DECLARE int AsStackPop(PASSTACK q);
AD_DECLARE AT_PVOID AsStackTail(PASSTACK q);
AD_DECLARE int AsStackEmpty(PASSTACK q);
AD_DECLARE unsigned int AsStackLength(PASSTACK q);
AD_DECLARE int AsStackFree(PASSTACK q,AT_BOOL freedata);



AD_DECLARE AT_PCHAR AsStringDup(const char* src);
AD_DECLARE void AsStringDupFree(AT_PCHAR str);


/***********************************************************************/
/*
*     bbit       1 << number % BYTE
*            [7][6][5][4][3][2][1][0][7][6][5][4][3][2][1][0]
*
*     abit       0x80 >> number % BYTE
*            [0][1][2][3][4][5][6][7][0][1][2][3][4][5][6][7]
*/
AD_DECLARE int ApABitGet( const unsigned char *set,int number);
AD_DECLARE void ApABitSet( unsigned char *set, int number,int value);
AD_DECLARE void ApABitFlip( unsigned char *set,int number);
AD_DECLARE int ApBBitGet( const unsigned char *set,int number);
AD_DECLARE void ApBBitSet( unsigned char *set, int number,int value);
AD_DECLARE void ApBBitFlip( unsigned char *set,int number);


AD_DECLARE unsigned int ApSwapInt( unsigned int num);
AD_DECLARE unsigned short   ApSwapShort(unsigned short s);
AD_DECLARE unsigned long long ApTypecastLongLong( const unsigned char* dat);
AD_DECLARE unsigned int ApTypecastInt( const unsigned char* dat);
AD_DECLARE unsigned short ApTypecastShort( const unsigned char* dat);
AD_DECLARE AT_PBYTE ApLongLongTypecast( AT_PBYTE dat,unsigned long long l);
AD_DECLARE AT_PBYTE ApIntTypecast( AT_PBYTE dat,unsigned int i);                
AD_DECLARE AT_PBYTE ApShortTypecast( AT_PBYTE dat, unsigned short s);




AD_DECLARE void ApSprintf(char *buf, int ilen, char *buffer);
AD_DECLARE int   ApStrCaseCmp(const char *s1,const char * s2) ;


#ifdef __cplusplus
}
#endif


#endif      //_LIBAS_H
