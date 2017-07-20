#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <assert.h>
#include "x11base.h"
#ifdef MEM_SAFE
#include "memsafe.h"
#endif

AD_DECLARE  int  AsStreamClose(PASTREAM sm)
{
    int rlt = 0;
    if(sm)
    {
        if (sm->close)
            sm->close( sm );
        //free(sm);
        rlt =1;
    }
    return rlt;
}


AD_DECLARE  int  AsStreamSeek(PASTREAM sm,unsigned long  pos)
{
    int rlt = 0;
    if(sm && sm->seek && sm->maxsize > pos)
    {
        //if(pos != sm->pos)    //EY   sometime file pos is incorrect
            sm->seek(sm,pos);

        rlt = 1;
    }
    return rlt;
}

AD_DECLARE  int AsStreamResize(PASTREAM sm,unsigned long  size)
{
    if(size == sm->size)
        return sm->size;
    if(size > sm->maxsize)
        return 0;
    return sm->resize(sm,size);
}

AD_DECLARE  int AsStreamPos(PASTREAM sm)
{
    return sm->pos;
}

AD_DECLARE  int AsStreamSize(PASTREAM sm)
{
    if (sm == NULL)  return 0;
    return sm->size;
}

AD_DECLARE  AT_PBYTE AsStreamMemory(PASTREAM sm)
{
    if (sm==NULL) return NULL;
    return sm->memory;
}

AD_DECLARE  int AsStreamSet(PASTREAM sm,unsigned long   pos,unsigned char ch,unsigned long  size)
{
    int i;

    if(sm==NULL)
        return 0;

    AsStreamSeek(sm,pos);
    for(i=0;i<size && sm->pos <sm->size ;i++)
    {
        AsStreamWriteChar(sm,ch);
    }
    return 1;
}

AD_DECLARE  int AsStreamRead(PASTREAM sm,
                  unsigned char*   buffer,
                  unsigned long   count )
{
    int rlt =0;
    unsigned long  read_bytes;
    unsigned long  rlen;

    if(sm->read && sm->size > sm->pos )
    {
        if(sm->pos +count >=sm->size)
            rlen = sm->size -sm->pos;
        else
            rlen = count;

        if(rlen)
        {
            read_bytes = sm->read( sm, buffer, rlen );
            sm->pos = sm->pos + read_bytes;
            rlt = read_bytes;
        }
    }
    return rlt;
}

AD_DECLARE  int AsStreamReadAt(PASTREAM sm,
                  unsigned long   pos,
                  unsigned char*   buffer,
                  unsigned long   count )
{
    int rlt =0;

    rlt = AsStreamSeek(sm,pos);

    if(rlt)
    {
        rlt = AsStreamRead(sm,buffer,count);
    }
    return rlt;
}

AD_DECLARE  int AsStreamReadChar(PASTREAM sm,
                  unsigned char*   ch)
{
    return AsStreamRead(sm,ch,1);
}

AD_DECLARE  int AsStreamReadCharAt(PASTREAM sm,
                  unsigned long   pos,
                  unsigned char*   ch )
{
    return AsStreamReadAt(sm,pos,ch,1);
}

AD_DECLARE  int AsStreamReadShort(PASTREAM sm,
                  unsigned short*   sh)
{
    return AsStreamRead(sm,(unsigned char*)sh,2);
}

AD_DECLARE  int AsStreamReadShortAt(PASTREAM sm,
                  unsigned long   pos,
                  unsigned short*   sh )
{
    return AsStreamReadAt(sm,pos,(unsigned char*)sh,2);
}

AD_DECLARE  int AsStreamReadInt(PASTREAM sm,
                  unsigned int*   it)
{
    return AsStreamRead(sm,(unsigned char*)it,sizeof(unsigned int));
}

AD_DECLARE  int AsStreamReadIntAt(PASTREAM sm,
                  unsigned long   pos,
                  unsigned int*   it )
{
    return AsStreamReadAt(sm,pos,(unsigned char*)it,sizeof(unsigned int));
}

#ifdef MEM_SAFE
unsigned char copyReadBuff[ASTREAM_RW_BUFFER_SIZE];
#endif
AD_DECLARE  int AsStreamCopy(PASTREAM dsm,unsigned long dpos,PASTREAM ssm,unsigned long spos,unsigned long   count )
{
    int rlt =0;

    unsigned long  wlen;
    int i;
    unsigned char ch;

    if(dsm->write )
    {
        if(dpos +count >=dsm->maxsize)
            wlen = dsm->maxsize - dpos;
        else
            wlen = count;



        if(wlen)
        {
            if(dpos +wlen >dsm->size)
                AsStreamResize(dsm,dpos +wlen);

		if(ssm->memory) //fast
		{
			AsStreamWriteAt(dsm, dpos, ssm->memory+spos  ,wlen);
		}
		/*else if (dsm->memory) //fast
		{
			AsStreamReadAt(ssm, spos,dsm->memory + dpos, wlen);
		}*/
		else
		{
			unsigned char buff[ASTREAM_RW_BUFFER_SIZE];
			unsigned long  nlen;
			unsigned long  len;
			unsigned char re = 0;

			nlen = wlen;
			AsStreamSeek(dsm,dpos);
			AsStreamSeek(ssm,spos);
			while(nlen>0)
			{
				if(nlen>= ASTREAM_RW_BUFFER_SIZE)
					len = ASTREAM_RW_BUFFER_SIZE;
				else
					len = nlen;
#ifdef MEM_SAFE
	#if 0
				AsStreamRead(ssm,buff,len);
	#else
				re = 1;
				while(re != 0)
				{
					memset(buff,0,ASTREAM_RW_BUFFER_SIZE);
					memset(copyReadBuff,0,ASTREAM_RW_BUFFER_SIZE);
				
					AsStreamRead(ssm,buff,len);
					
					AsStreamSeek(ssm,AsStreamPos(ssm)- len);
					AsStreamRead(ssm, copyReadBuff, len);

					if(memcmp(buff,copyReadBuff,len) == 0)
					{
						re = 0;
					}
					else
					{
						PrintfXTOS("!!! ### ---------- AsCopy ERROR #1 ----------- ### !!!\n");
						AsStreamSeek(ssm,AsStreamPos(ssm)- len);
					}
					
				}
	#endif

				ApWatchDogFeed();

	#if 0
				AsStreamWrite(dsm,buff,len);
	#else
				re = 1;
				while(re != 0)
				{
					AsStreamWrite(dsm,buff,len);

					memset(copyReadBuff,0,ASTREAM_RW_BUFFER_SIZE);
					AsStreamSeek(dsm,AsStreamPos(dsm)- len);
					AsStreamRead(dsm, copyReadBuff, len);

					if(memcmp(buff,copyReadBuff,len) == 0)
					{
						re = 0;
					}
					else
					{
						PrintfXTOS("!!! ### ---------- AsCopy ERROR 0x%x #2 ----------- ### !!!\n",AsStreamPos(dsm)- len);
						AsStreamSeek(dsm,AsStreamPos(dsm)- len);
					}
					
				}
	#endif
#else
				AsStreamRead(ssm,buff,len);
				AsStreamWrite(dsm,buff,len);
#endif

				nlen -=len;
			}
		}



            rlt = wlen;
        }
    }
    return rlt;
}

AD_DECLARE  int AsStreamCopyFast(PASTREAM dsm,unsigned long dpos,PASTREAM ssm,unsigned long spos,unsigned long   count )
{
    int rlt =0;

    unsigned long  wlen;
    int i;
    unsigned char ch;

    if(dsm->write )
    {
        if(dpos +count >=dsm->maxsize)
            wlen = dsm->maxsize - dpos;
        else
            wlen = count;



        if(wlen)
        {
            if(dpos +wlen >dsm->size)
                AsStreamResize(dsm,dpos +wlen);

		if(ssm->memory) //fast
		{
			AsStreamWriteAt(dsm, dpos, ssm->memory+spos  ,wlen);
		}
		/*else if (dsm->memory) //fast
		{
			AsStreamReadAt(ssm, spos,dsm->memory + dpos, wlen);
		}*/
		else
		{
			unsigned char* buff = NULL;
			unsigned long  len = wlen;

			buff = (unsigned char*)malloc(len);
			AsStreamSeek(dsm,dpos);
			AsStreamSeek(ssm,spos);

			if(buff != NULL)
			{
				AsStreamRead(ssm,buff,len);
				AsStreamWrite(dsm,buff,len);
				free(buff);
			}
		}

            rlt = wlen;
        }
    }
    return rlt;
}

AD_DECLARE  int AsStreamDup(PASTREAM dsm,PASTREAM ssm)
{
	int rlt =0;

	AsStreamResize(dsm, AsStreamSize(ssm));
	rlt = AsStreamCopy(dsm,0, ssm, 0, AsStreamSize(ssm));
	return rlt;
}


AD_DECLARE  int AsStreamWrite(PASTREAM sm,
                  unsigned char*   buffer,
                  unsigned long   count )
{
    int rlt =0;
    unsigned long  write_bytes;
    unsigned long  wlen;

    if(sm->write )
    {
        if(sm->pos +count >=sm->maxsize)
            wlen = sm->maxsize -sm->pos;
        else
            wlen = count;



        if(wlen)
        {
            if(sm->pos +wlen >sm->size)
                AsStreamResize(sm,sm->pos +wlen);

            write_bytes = sm->write( sm, buffer, wlen );
            sm->pos = sm->pos + write_bytes;

            rlt = write_bytes;
        }
    }
    return rlt;
}

AD_DECLARE  int AsStreamWriteAt(PASTREAM sm,
                  unsigned long   pos,
                  unsigned char*   buffer,
                  unsigned long   count )
{
    int rlt =0;

    rlt = AsStreamSeek(sm,pos);

    if(rlt)
    {
        rlt = AsStreamWrite(sm,buffer,count);
    }
    return rlt;
}

AD_DECLARE  int AsStreamWriteChar(PASTREAM sm,
                  unsigned char   ch)
{
    return AsStreamWrite(sm,&ch,1);
}

AD_DECLARE  int AsStreamWriteCharAt(PASTREAM sm,
                  unsigned long   pos,
                  unsigned char   ch )
{
    return AsStreamWriteAt(sm,pos,&ch,1);
}

AD_DECLARE  int AsStreamWriteShort(PASTREAM sm,
                  unsigned short   sh)
{
    return AsStreamWrite(sm,(unsigned char*)&sh,2);
}

AD_DECLARE  int AsStreamWriteShortAt(PASTREAM sm,
                  unsigned long   pos,
                  unsigned short   sh )
{
    return AsStreamWriteAt(sm,pos,(unsigned char*)&sh,2);
}

AD_DECLARE  int AsStreamWriteInt(PASTREAM sm,
                  unsigned int   it)
{
    return AsStreamWrite(sm,(unsigned char*)&it,sizeof(unsigned int));
}

AD_DECLARE  int AsStreamWriteIntAt(PASTREAM sm,
                  unsigned long   pos,
                  unsigned int   it )
{
    return AsStreamWriteAt(sm,pos,(unsigned char*)&it,sizeof(unsigned int));
}

AD_DECLARE  int AsStreamInsertBuffer(PASTREAM sm,unsigned long pos,unsigned long count ,unsigned char* buffer )
{
    int rlt =0;
    unsigned long  inscount;
    unsigned long  mvcount;

    if(sm->size > pos)
    {
        if(sm->size+count >sm->maxsize)
            inscount = sm->maxsize -sm->size;
        else
            inscount = count;

        mvcount = sm->size - pos;
        if(mvcount)
        {
            int i;
            int tlen;
            int rlen;
            unsigned long ps;
            unsigned long pd;
            unsigned char buff[ASTREAM_RW_BUFFER_SIZE];

            ps = sm->size;
            pd = sm->size + inscount;
            for(i=0;i<mvcount;)
            {
                if(mvcount-i >  sizeof(buff))
                    tlen = sizeof(buff);
                else
                    tlen = mvcount -i;

                ps -= tlen;
                pd -= tlen;
                rlen = AsStreamReadAt(sm,ps,buff,tlen);
                if(rlen!=tlen)
                    break;
                tlen = AsStreamWriteAt(sm,pd, buff,rlen);
                if(rlen!=tlen)
                    break;
                i+=rlen;
            }

            AsStreamSeek(sm,pos);
            if(buffer)
            {
                AsStreamWrite(sm,buffer,count);
            }
            else
            {
                for(i=0;i<inscount;i++)
                {
                    AsStreamWriteChar(sm,'\0');
                }
            }


        }
        //sm->size +=inscount;
        rlt = inscount;

    }
    return rlt;
}

AD_DECLARE  int AsStreamInsert(PASTREAM sm,unsigned long pos,unsigned long count )
{
    return AsStreamInsertBuffer(sm,pos,count,NULL);
}

AD_DECLARE  int AsStreamRemove(PASTREAM sm,unsigned long pos,unsigned long count )
{
    int rlt =0;
    unsigned long  delcount;
    unsigned long  mvcount;

    if(sm->size > pos)
    {
        if(pos +count >=sm->size)
            delcount = sm->size -pos;
        else
            delcount = count;

        mvcount = sm->size - pos - delcount;
        if(mvcount)
        {
            int i;
            int tlen;
            int rlen;
            unsigned long ps;
            unsigned long pd;
            unsigned char buff[ASTREAM_RW_BUFFER_SIZE];

            ps = pos+delcount;
            pd = pos;
            for(i=0;i<mvcount;)
            {
                if(mvcount-i >  sizeof(buff))
                    tlen = sizeof(buff);
                else
                    tlen = mvcount -i;


                rlen = AsStreamReadAt(sm,ps,buff,tlen);
                //assert(rlen==tlen);
                tlen = AsStreamWriteAt(sm,pd, buff,rlen);
                //assert(rlen==tlen);
                ps += tlen;
                pd += tlen;
                i+=rlen;
            }


        }
        sm->size -=delcount;
        rlt = delcount;

    }
    return rlt;
}

AD_DECLARE  int AsStreamReplace(PASTREAM sm,unsigned long   pos,unsigned long   slen,unsigned char*   buffer,unsigned long   dlen )
{
    int rlt =0;

    if(slen>dlen)
    {
        AsStreamRemove(sm,pos,slen-dlen);
    }
    else if(slen < dlen)
    {
        AsStreamInsert(sm,pos,dlen-slen);
    }
    else // slen == dlen
    {

    }

    rlt = AsStreamWriteAt(sm,pos,buffer,dlen);

    return rlt;
}

AD_DECLARE  int AsStreamFindChar(PASTREAM sm,unsigned long   stp,unsigned long   etp,unsigned char ch)
{
    int rlt =-1;

    unsigned char buff[ASTREAM_FIND_BUFFER_SIZE];
    int i;
    unsigned long ph;
    int rlen;

    ph = stp;
    AsStreamSeek(sm,ph);
    while(ph <=etp)
    {
        if( (etp-ph+1) > ASTREAM_FIND_BUFFER_SIZE)
            rlen = ASTREAM_FIND_BUFFER_SIZE;
        else
            rlen = etp-ph+1;

        if(AsStreamRead(sm,buff,rlen)==0)
            break;
        for(i=0;i<rlen;i++)
        {
            if(ch == buff[i])
            {
                rlt = ph+i;
                goto willexit;
            }
        }
        ph+=rlen;
    }

willexit:
    return rlt;
}



AD_DECLARE  int AsStreamFind(PASTREAM sm,unsigned long   stp,unsigned long   etp,unsigned char*   buffer,unsigned long   count )
{
    int rlt =-1;

    int pos;
    int i;
    unsigned long ph;
    unsigned char ch;

    ph = stp;
    i = 0;
    while(ph <etp)
    {
        pos = AsStreamFindChar(sm,ph,etp,buffer[0]);
        if(pos== -1)
            break;
        if(count ==1)
        {
            rlt = pos;
            goto willexit;
        }
        ph =pos+1;
        AsStreamSeek(sm,ph);
        for(i=1;i<count;i++)
        {
            if(AsStreamReadChar(sm,&ch)==0)
                break;
            if(ch!=buffer[i])
                break;
            if((i+1) == count)
            {
                rlt = pos;
                goto willexit;
            }
        }
        
    }

willexit:
    return rlt;
}


AD_DECLARE  AT_PBYTE AsBinToHex(unsigned char ch,short *hstr)
{
    sprintf((char*)hstr,"%02X",ch);
	return (AT_PBYTE)hstr;
}

AD_DECLARE  char AsHexToBin(char hc)
{
    int rlt = 0;

    if(hc >='0' && hc<='9')
            rlt = hc -'0';
    else if(hc >='a' && hc<='f')
            rlt = hc -'a'+ 10;
    else if(hc >='A' && hc<='F')
            rlt = hc -'A'+ 10;
    return rlt;
}

AD_DECLARE  char AsHexToChar(const short* hstr)
{
	char rlt =0;
    char* cc = (char*)hstr;
    rlt = AsHexToBin(cc[0])<<4 ;
    rlt+= AsHexToBin(cc[1]);
	return rlt;

}

AD_DECLARE  int AsABitGet(const unsigned char *set, int number)
{
        set += number / BYTE_BIT_SIZE;
        return (*set & (0x80 >> (number % BYTE_BIT_SIZE))) != 0;    /* 0 or 1   */
}

AD_DECLARE  void AsABitSet(unsigned char *set, int number, int value)
{
        set += number / BYTE_BIT_SIZE;
        if (value)
                *set |= 0x80 >> (number % BYTE_BIT_SIZE);           /* set bit  */
        else    *set &= ~(0x80 >> (number % BYTE_BIT_SIZE));        /* clear bit*/
}

AD_DECLARE  void AsABitFlip(unsigned char *set, int number)
{
        set += number / BYTE_BIT_SIZE;
        *set ^= 0x80 >> (number % BYTE_BIT_SIZE);                   /* flip bit */
}

AD_DECLARE  int AsBBitGet(const unsigned char *set, int number)
{
        set += number / BYTE_BIT_SIZE;
        return (*set & (1 << (number % BYTE_BIT_SIZE))) != 0;    /* 0 or 1   */
}

AD_DECLARE  void AsBBitSet(unsigned char *set, int number, int value)
{
        set += number / BYTE_BIT_SIZE;
        if (value)
                *set |= 1 << (number % BYTE_BIT_SIZE);           /* set bit  */
        else    *set &= ~(1 << (number % BYTE_BIT_SIZE));        /* clear bit*/
}

AD_DECLARE  void AsBBitFlip(unsigned char *set, int number)
{
        set += number / BYTE_BIT_SIZE;
        *set ^= 1 << (number % BYTE_BIT_SIZE);                   /* flip bit */
}


AD_DECLARE PASSLIST  AsSlistNew( AT_PVOID data )
{
	PASSLIST  nn = (PASSLIST)malloc(sizeof(ASSLIST));
	
	if (NULL == nn) return NULL;

	nn->next = NULL;
	nn->data = data;
	
	return nn;
}


AD_DECLARE PASSLIST AsSlistInsert( PASSLIST curItem, PASSLIST newItem )
{
  	PASSLIST item;
	item = curItem->next;
	curItem->next = newItem;
	newItem->next = item;

	return newItem;	
}


AD_DECLARE PASSLIST AsSlistRemove( PASSLIST head, PASSLIST item )
{
  	PASSLIST cur;
	PASSLIST pre;

	if (head == item)
	  {
		head = head->next;
	  }
	else
	  {
		cur = head;
		do
		  {
			pre = cur;
			cur = cur->next;
			if (NULL == cur) return head;
		  } while (cur != item);
		
		pre->next = item->next;
	  }

	free(item);
	return head;
}

AD_DECLARE PASSLIST AsSlistFind( PASSLIST head, void* data)
{
  	PASSLIST cur;


	cur = head;
	while(cur !=NULL)
	{

		if(cur->data ==data)
		{
			break;
		}

		cur = cur->next;
	}
	return cur;
}


AD_DECLARE PASSLIST AsSlistTail( PASSLIST head )
{
  	PASSLIST cur = head;
	if (cur)
	  {
		while (cur->next) cur = cur->next;
	  }

	return cur;
}


AD_DECLARE void AsSlistFree( PASSLIST head ,AT_BOOL freedata)
{
  	PASSLIST cur = head;
	PASSLIST tmp;

	while (cur) 
	{
		tmp=cur;
		if(freedata)
			free(tmp->data);
		cur = cur->next;
		free(tmp);
	}
	

	return ;
}



AD_DECLARE PASQUEUE AsQueueNew(void)
{
	PASQUEUE nn = (PASQUEUE)malloc(sizeof(ASQUEUE));
	
	if (NULL == nn) return NULL;

	nn->head = NULL;
	nn->tail = NULL;
	nn->length = 0;
	return nn;
}

AD_DECLARE int AsQueuePush(PASQUEUE q, AT_PVOID data)
{
	PASSLIST item = AsSlistNew(data);

	if(q->length)
	{
		q->tail->next = item;
		q->tail = item;
	}
	else
	{

		q->head = item;
		q->tail = item;
	}
	q->length++;

	return 1;

}

AD_DECLARE int AsQueuePop(PASQUEUE q)
{
	
	if(q->length)
	{
		 PASSLIST item;
		 item = q->head;
		 q->head = item->next;
		 free(item);
		 q->length--;
	}
	else
	{
		return 0;

	}

	return 1;

}

AD_DECLARE AT_PVOID AsQueueHead(PASQUEUE q)
{ 
	if(q->length && q->head)
	{
		return q->head->data;
	}
	else
	{
		return NULL;
	}	

}

AD_DECLARE AT_PVOID AsQueueTail(PASQUEUE q)
{ 
	if(q->length)
	{
		return q->tail->data;
	}
	else
	{
		return NULL;
	}	

}

AD_DECLARE int AsQueueEmpty(PASQUEUE q)
{
	return (q->length == 0);
}

AD_DECLARE unsigned int AsQueueLength(PASQUEUE q)
{
	if(q==NULL)
		return 0;
	return q->length;
}

AD_DECLARE int AsQueueFree(PASQUEUE q,AT_BOOL freedata)
{
    if(q==NULL)
		return 0;
	while(q->length)
	{
		 PASSLIST item;
		 item = q->head;
		if(freedata)
			free(item->data);
		
		 q->head = item->next;
		 free(item);
		 q->length--;
	}
	free(q);

	return 1;

}

AD_DECLARE PASDLIST  AsDlistNew( AT_PVOID data )
{
	PASDLIST  nn = (PASDLIST)malloc(sizeof(ASDLIST));
	
	if (NULL == nn) return NULL;

	nn->next = NULL;
	nn->prev = NULL;
	nn->data = data;
	
	return nn;

}

AD_DECLARE void AsDlistFree( PASDLIST any ,AT_BOOL freedata)
{
  	PASDLIST cur = any;
	PASDLIST tmp;

	

	return ;

}

AD_DECLARE PASSTACK AsStackNew(void)
{
	PASSTACK nn = (PASSTACK)malloc(sizeof(ASSTACK));
	
	if (NULL == nn) return NULL;

	nn->tail = NULL;
	nn->length = 0;
	return nn;
}

AD_DECLARE int AsStackPush(PASSTACK q, AT_PVOID data)
{
	PASDLIST item = AsDlistNew(data);

	if(q->length)
	{
		item->prev = q->tail;
		q->tail->next = item;
		q->tail = item;
	}
	else
	{
		q->tail = item;
	}
	q->length++;

	return 1;

}

AD_DECLARE AT_PVOID AsStackTail(PASSTACK q)
{ 
	if(q->length)
	{
		return q->tail->data;
	}
	else
	{
		return NULL;
	}	

}

AD_DECLARE int AsStackPop(PASSTACK q)
{
	
	if(q->length)
	{
		 PASDLIST item;
		 item = q->tail;
		 q->tail = item->prev;
		 free(item);
		 q->length--;
	}
	else
	{
		return 0;

	}

	return 1;

}


AD_DECLARE int AsStackEmpty(PASSTACK q)
{
	return (q->length == 0);
}

AD_DECLARE unsigned int AsStackLength(PASSTACK q)
{
	if(q==NULL)
		return 0;
	return q->length;
}

AD_DECLARE int AsStackFree(PASSTACK q,AT_BOOL freedata)
{
    if(q==NULL)
		return 0;
	while(q->length)
	{
		 PASDLIST item;
		 item = q->tail;
		 if(freedata)
		 	free(item->data);
		 q->tail= item->prev;
		 free(item);
		 q->length--;
	}
	free(q);

	return 1;

}

AD_DECLARE AT_PCHAR AsStringDup(const char* src)
{
	int len;
	char *dst=NULL;
    if (src) {
        len = strlen(src);
        dst = (char*)malloc(len+1);
        strcpy(dst,src);
        dst[len]=0;
    }
	return dst;
}

AD_DECLARE void AsStringDupFree(AT_PCHAR str)
{
	free(str);
}

//************************************************


AD_DECLARE int  ApABitGet(const unsigned char *set, int number)
{
        set += number / BYTE_BIT_SIZE;
        return (*set & (0x80 >> (number % BYTE_BIT_SIZE))) != 0;    /* 0 or 1   */
}

AD_DECLARE void  ApABitSet(unsigned char *set, int number, int value)
{
        set += number / BYTE_BIT_SIZE;
        if (value)
                *set |= 0x80 >> (number % BYTE_BIT_SIZE);           /* set bit  */
        else    *set &= ~(0x80 >> (number % BYTE_BIT_SIZE));        /* clear bit*/
}

AD_DECLARE void  ApABitFlip(unsigned char *set, int number)
{
        set += number / BYTE_BIT_SIZE;
        *set ^= 0x80 >> (number % BYTE_BIT_SIZE);                   /* flip bit */
}

AD_DECLARE int  ApBBitGet(const unsigned char *set, int number)
{
        set += number / BYTE_BIT_SIZE;
        return (*set & (1 << (number % BYTE_BIT_SIZE))) != 0;    /* 0 or 1   */
}

AD_DECLARE void  ApBBitSet(unsigned char *set, int number, int value)
{
        set += number / BYTE_BIT_SIZE;
        if (value)
                *set |= 1 << (number % BYTE_BIT_SIZE);           /* set bit  */
        else    *set &= ~(1 << (number % BYTE_BIT_SIZE));        /* clear bit*/
}

AD_DECLARE void  ApBBitFlip(unsigned char *set, int number)
{
        set += number / BYTE_BIT_SIZE;
        *set ^= 1 << (number % BYTE_BIT_SIZE);                   /* flip bit */
}


AD_DECLARE void   ApSprintf(char *buf, int ilen, char *buffer)
{
    int nLen = strlen(buffer);
    if (nLen>ilen) {
        strncpy(buf, buffer, ilen);
        buf[ilen] = 0;
    }
    else strcpy(buf, buffer);
}


AD_DECLARE int   ApStrCaseCmp(const char *s1,const char * s2) 
{
#ifdef UNIX
	return strcasecmp(s1,s2);

#else
#ifdef CM3
	return strcasecmp(s1,s2);
#else
	return stricmp(s1,s2);
#endif
#endif


}


AD_DECLARE unsigned int  ApSwapInt(unsigned int num)
{
	return  ((num >> 24) & 0x000000ff) |
		((num >>  8) & 0x0000ff00) |
		((num <<  8) & 0x00ff0000) |
		((num << 24) & 0xff000000);
}

AD_DECLARE unsigned short  ApSwapShort(unsigned short s)
{
	return (s >> 8) | (s << 8);
}


AD_DECLARE unsigned long long  ApTypecastLongLong(const unsigned char* dat)
{
	unsigned long long rlt;

	memcpy(&rlt,dat,sizeof(unsigned long long));

	return rlt;
}


AD_DECLARE unsigned int  ApTypecastInt(const unsigned char* dat)
{
	//don't use below mode
	// i = *(int *) dat;
	// dat point maybe not % 4

	unsigned int rlt;

	memcpy(&rlt,dat,sizeof(unsigned int));

	return rlt;
}

AD_DECLARE unsigned short  ApTypecastShort(const unsigned char* dat)
{
	unsigned short rlt;

	memcpy(&rlt,dat,sizeof(unsigned short));

	return rlt;	

}

AD_DECLARE AT_PBYTE  ApLongLongTypecast(AT_PBYTE dat,unsigned long long l)
{

	memcpy(dat,&l,sizeof(unsigned long long));
        return dat;
}

AD_DECLARE AT_PBYTE  ApIntTypecast(AT_PBYTE dat,unsigned int i)
{
	//don't use below mode
	// *(int *) dat = i;
	// dat point maybe not % 4

	memcpy(dat,&i,sizeof(unsigned int));
        return dat;
}

AD_DECLARE AT_PBYTE  ApShortTypecast(AT_PBYTE dat,unsigned short s)
{
	memcpy(dat,&s,sizeof(unsigned short));
        return dat;
}

