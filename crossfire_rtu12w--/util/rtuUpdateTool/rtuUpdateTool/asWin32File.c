#include <stdio.h>
#include "x11base.h"

static int ffRead(PASTREAM sm,unsigned char* data,unsigned long len)
{
	int frRe= 0;
	int rByteNum=0;
	frRe = fread(data, 1, len, (FILE*)sm->handle);
	if(frRe > 0)
		rByteNum = frRe;
    return rByteNum;
}

static int ffWrite(PASTREAM sm,unsigned char* data,unsigned long len)
{
	int frRe= 0;
	int wByteNum=0;
	frRe = fwrite(data, 1, len, (FILE*)sm->handle);
	if(frRe > 0)
		wByteNum = frRe;
    return wByteNum;
}

static int ffSeek(PASTREAM sm,unsigned long offset)
{
    //fseek((FILE*)sm->handle,offset,SEEK_SET);
    fseek((FILE*)sm->handle, offset, SEEK_SET);
    sm->pos = offset;
    return sm->pos;
}

static int ffResize(PASTREAM sm,unsigned long size)
{
    /*
    FILE* fh=(FILE*)sm->handle;
    sm->size = size;

    //ffSeek(sm,sm->size);  //EY if pos out of size will be pos to size
    chsize(fh->fd,sm->size);
    ffSeek(sm,sm->pos);
    return sm->size;
    */
	sm->size = size;
	return sm->size;
}

static int ffClose(PASTREAM sm)
{
    //ffResize(sm,sm->size);
    //fclose((FILE*)sm->handle);
	fclose((FILE*)sm->handle);
	free(sm);

    return 1;
}

AD_DECLARE  PASTREAM  AsStreamOpenFromWin32File(const char* filename, int *mode)
{
    PASTREAM sm;
    FILE* outh;
	
	outh = fopen(filename, "rb+");

    //fd = fopen(filename,mode);
    /*if(fd ==NULL)
        fd = fopen(filename,"r+b");
    if(fd ==NULL)
        fd = fopen(filename,"w+b");*/
    if(outh == NULL)
    {
        return NULL;
    }

    sm = (PASTREAM)malloc(sizeof(ASTREAM));
    if(sm==0)
    {
       	 return NULL;
    }
    sm->memory = NULL;
    sm->handle = (void *)outh;
	fseek(outh,0,SEEK_END);
    sm->size = ftell((FILE*)outh);
    fseek(outh, 0, SEEK_SET);
    sm->maxsize = 0x8FFFFFFF;
    sm->pos = 0;

    sm->read = ffRead;
    sm->write = ffWrite;
    sm->seek = ffSeek;
    sm->resize = ffResize;
    sm->close = ffClose;


    return sm;

}

