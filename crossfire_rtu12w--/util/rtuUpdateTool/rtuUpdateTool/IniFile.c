#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "x11base.h"
#ifdef MEM_SAFE
#include "memsafe.h"
#endif

AD_DECLARE  PASFINI  AsFIniOpenFromStream(PASTREAM sm)
{
    PASFINI ini;

    ini = (PASFINI)malloc(sizeof(ASFINI));
    memset(ini,0,sizeof(ASFINI));
    ini->sm = sm;

    return ini;
}

AD_DECLARE  int  AsFIniClose(PASFINI ini)
{
    free(ini);
    return 1;
}

AD_DECLARE  int AsFIniFindSection(PASFINI ini, const char* pSection)
{
    int rlt =0 ;
    int p;
    int st=0;
    int len;


    //if(ini->cursect && strcmp(ini->cursect,pSection)==0)
    //    return 1;

    len = strlen(pSection);
    if ((len ==0)||(ini==NULL))
        return rlt;

findagain:
    p = AsStreamFind(ini->sm,st,ini->sm->size,(unsigned char*)pSection,len);
    if(p >0)
    {
        unsigned char ch;
        st = p + len;
        AsStreamReadCharAt(ini->sm,p-1,&ch);
        if(ch !='[')
            goto findagain;
        AsStreamReadCharAt(ini->sm,st,&ch);
        if(ch !=']')
            goto findagain;

        ini->cursect = (char*)pSection;
        ini->secthead = p-1;
        p = AsStreamFindChar(ini->sm,st+1,ini->sm->size,'\n');
        if(p==-1)
            ini->sectbegin = st+1 ;
        else
            ini->sectbegin = p+1;

        p = AsStreamFind(ini->sm,st+1,ini->sm->size,"\n[",2);
        if(p==-1)
            ini->sectend = ini->sm->size;
        else
            ini->sectend = p+1;

        rlt =1;
    }

    return rlt;
}

AD_DECLARE  int AsFIniFindKey(PASFINI ini, const char* pSection,const char* pKey)
{
    int rlt =0 ;
    int p;
    int st;
    int len;

    if(AsFIniFindSection(ini,pSection)==0)
        return rlt;

    //if(ini->curkey && strcmp(ini->curkey,pKey)==0)
    //    return 1;

    len = strlen(pKey);
    if ((len ==0)||(ini==NULL))
        return rlt;

    st = ini->sectbegin;
findagain:
    p = AsStreamFind(ini->sm,st,ini->sectend,(unsigned char*)pKey,len);
    if(p >0)
    {
        unsigned char ch;
        st = p + len;
        AsStreamReadCharAt(ini->sm,p-1,&ch);
        if(ch !='\n')
            goto findagain;
        AsStreamReadCharAt(ini->sm,st,&ch);
        if(ch !='=')
            goto findagain;

        ini->curkey = (char*)pKey;
        ini->keyhead = p;
        ini->keybegin = st+1;
        p = AsStreamFindChar(ini->sm,st+1,ini->sm->size,'\n');
        if(p==-1)
            ini->keyend = ini->sm->size;
        else
        {
            AsStreamReadCharAt(ini->sm,p-1,&ch);
            if(ch =='\r')
                ini->keyend = p-1;
            else
                ini->keyend = p;
        }
        rlt =1;
    }

    return rlt;
}

AD_DECLARE  int AsFIniGetKeyValueSize(PASFINI ini, const char* pSection,const char* pKey)
{
    int rlt =0;

    if(AsFIniFindKey(ini,pSection,pKey))
    {
        rlt = ini->keyend - ini->keybegin;
    }

    return rlt;
}

AD_DECLARE  int AsFIniReadKeyValueDef(PASFINI ini, const char* pSection,const char* pKey,char* vbuf,unsigned long buflen,char* def)
{
    int rlt = 0;
    int rlen = 0;
    if(AsFIniFindKey(ini,pSection,pKey))
    {
        if( buflen  > ini->keyend - ini->keybegin)
            rlen = ini->keyend - ini->keybegin ;
        else
            rlen = buflen;

        rlt = AsStreamReadAt(ini->sm,ini->keybegin,vbuf,rlen);
    }
    else
    {
        if(def)
        {
            if(strlen(def)>buflen)
                rlen = buflen;
            else
                rlen = strlen(def);

            strncpy(vbuf,def,rlen);
        }
    }
    vbuf[rlen]=0;   //EY maybe overrun
    return rlt;

}

AD_DECLARE  int AsFIniReadKeyValue(PASFINI ini, const char* pSection,const char* pKey,char* vbuf,unsigned long buflen)
{
    return AsFIniReadKeyValueDef(ini,pSection,pKey,vbuf,buflen,NULL);
}

AD_DECLARE  int AsFIniReadKeyIntDef(PASFINI ini, const char* pSection,const char* pKey,unsigned int* value,unsigned int def)
{
    int len;
    char* str;
    len = AsFIniGetKeyValueSize(ini,pSection,pKey);

    if(len)
    {
        str = (char*)malloc(len+1);
        memset(str,0,len+1);

        AsFIniReadKeyValue(ini,pSection,pKey ,str,len );

        *value = atoi(str);
        free(str);

    }
    else
    {
        *value = def;
    }
    return  len;
}


AD_DECLARE  int AsFIniReadKeyStream(PASFINI ini, const char* pSection,const char* pKey,PASTREAM sm)
{
    int len;
    char str[2];
    unsigned char ch;
    int i;
    len = AsFIniGetKeyValueSize(ini,pSection,pKey);

    sm->size =0;
    AsStreamSeek(sm,0);
    if(len)
    {
        AsStreamSeek(ini->sm,ini->keybegin);
        for(i=0;i<len/2;i++)
        {
            AsStreamRead(ini->sm,str,2);
            ch = AsHexToChar((short*)str);
            AsStreamWriteChar(sm,ch);            
        }

    }
    return  len;
}





AD_DECLARE  int AsFIniWriteKeyValue(PASFINI ini, const char* pSection,const char* pKey,const char* vbuf)
{
    int rlt =0;
    if (ini==NULL)
        return rlt;
    if(AsFIniFindSection(ini,pSection)==0)   // not find
    {
        //AsStreamInsert(ini->sm,0,1+strlen(pSection)+1+1+1);
        
        AsStreamSeek(ini->sm,ini->sm->size);
        AsStreamWriteChar(ini->sm,'\n');
        AsStreamWriteChar(ini->sm,'[');
        AsStreamWrite(ini->sm,(unsigned char*)pSection,strlen(pSection));
        AsStreamWriteChar(ini->sm,']');
        AsStreamWriteChar(ini->sm,'\n');
        
    }

    if(AsFIniFindKey(ini,pSection,pKey)==0)
    {
        AsStreamInsert(ini->sm,ini->sectend,strlen(pKey)+1+strlen(vbuf)+1);
        AsStreamSeek(ini->sm,ini->sectend);
        AsStreamWrite(ini->sm,(unsigned char*)pKey,strlen(pKey));
        AsStreamWriteChar(ini->sm,'=');
        AsStreamWrite(ini->sm,(unsigned char*)vbuf,strlen(vbuf));
        AsStreamWriteChar(ini->sm,'\n');
    }
    else
    {
        AsStreamReplace(ini->sm,ini->keybegin,ini->keyend -ini->keybegin,(unsigned char*)vbuf ,strlen(vbuf));
    }
    AsFIniFindKey(ini,pSection,pKey); // get new keybegin&keyend
    rlt = 1;
    return rlt;
}

AD_DECLARE  int AsFIniWriteKeyInt(PASFINI ini, const char* pSection,const char* pKey, int value)
{
    char str[10];
    //itoa(value,str,10);
	sprintf(str,"%d",value);
    return  AsFIniWriteKeyValue(ini,pSection,pKey,str);
}

AD_DECLARE  int AsFIniWriteKeyStream(PASFINI ini, const char* pSection,const char* pKey,PASTREAM sm)
{
    int i;
    unsigned char ch;
    char str[3];
    AsFIniWriteKeyValue(ini,pSection,pKey,"");

    AsStreamSeek(sm,0);
    str[2] ='\0';
    AsStreamInsert(ini->sm,ini->keybegin,sm->size*2);
    AsStreamSeek(ini->sm,ini->keybegin);
    for(i=0;i<sm->size;i++)
    {
        AsStreamReadChar(sm,&ch);
        AsBinToHex(ch,(short*)str);
        AsStreamWrite(ini->sm,str,2);
    }

    return sm->size;
}

AD_DECLARE  int AsFIniDeleteKey(PASFINI ini, const char* pSection,const char* pKey)
{
    int rlt =0;

    if(AsFIniFindKey(ini,pSection,pKey))
    {
        unsigned char ch;
        rlt = AsStreamRemove(ini->sm,ini->keyhead,ini->keyend - ini->keyhead+1);

        AsStreamReadCharAt(ini->sm,ini->keyhead,&ch);
        if(ch =='\n')
            AsStreamRemove(ini->sm,ini->keyhead,1);

    }
    return rlt;
}

AD_DECLARE  int AsFIniDeleteSection(PASFINI ini, const char* pSection)
{
    int rlt =0;

    if(AsFIniFindSection(ini,pSection))
    {
        rlt = AsStreamRemove(ini->sm,ini->secthead,ini->sectend - ini->secthead);

    }
    return rlt;
}
