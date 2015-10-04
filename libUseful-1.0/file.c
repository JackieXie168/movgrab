#include "includes.h"
#include "DataProcessing.h"
#include "pty.h"
#include "expect.h"

#ifdef HAVE_LIBSSL
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

int FDIsWritable(int fd)
{
fd_set selectset;
int result;


FD_ZERO(&selectset);
FD_SET(fd, &selectset);
result=select(fd+1,NULL, &selectset,NULL,NULL);
if ((result==-1) && (errno==EBADF)) return(-1);
if ((result  > 0) && (FD_ISSET(fd, &selectset))) return(TRUE);

 return(FALSE);
}



int FDCheckForBytes(int fd)
{
fd_set selectset;
struct timeval tv;
int result;


FD_ZERO(&selectset);
FD_SET(fd, &selectset);
tv.tv_sec=0;
tv.tv_usec=0;
result=select(fd+1,&selectset,NULL,NULL,&tv);
  if ((result==-1) && (errno==EBADF)) return(-1);
  if ((result  > 0) && (FD_ISSET(fd, &selectset))) return(TRUE);

  return(FALSE);
}


int STREAMCheckForBytes(STREAM *Stream)
{
  if (! Stream) return(0);
  if (Stream->InEnd > Stream->InStart) return(1);
  return(FDCheckForBytes(Stream->in_fd));
}


void STREAMSetNonBlock(STREAM *S, int val)
{
int flags;

	if (val) S->Flags |= SF_NONBLOCK;
  else S->Flags &= (~SF_NONBLOCK);


  fcntl(S->in_fd,F_GETFL,&flags);
  if (val) flags |= O_NONBLOCK;
  else flags &= (~O_NONBLOCK);

  fcntl(S->in_fd, F_SETFL, flags);
}


int STREAMCountWaitingBytes(STREAM *S)
{
int read_result=0, result;

result=FDCheckForBytes(S->in_fd);
if (result > 0) read_result=STREAMReadCharsToBuffer(S);
else if (result < 0) read_result=STREAM_CLOSED;
result=S->InEnd - S->InStart;

if (result > 0) return(result);
if (read_result==STREAM_CLOSED) return(EOF);
if (read_result==STREAM_DATA_ERROR) return(EOF);
return(0);
}


int STREAMCheckForWaitingChar(STREAM *S, char check_char)
{
int read_result=0, result, trash;
char *found_char;

result=FDCheckForBytes(S->in_fd);
if (result > 0) read_result=STREAMReadCharsToBuffer(S);
else if (result < 0) read_result=STREAM_CLOSED;

if (S->InStart < S->InEnd) 
{
  found_char=memchr(S->InputBuff + S->InStart,check_char,S->InEnd - S->InStart);
  if (found_char > 0) return(TRUE);
}

if (read_result==STREAM_CLOSED) return(EOF);
if (read_result==STREAM_DATA_ERROR) return(EOF);
return(FALSE);
}



int STREAMInternalFinalWriteBytes(STREAM *S, const char *Data, int DataLen)
{
fd_set selectset;
int result, count=0;
struct timeval tv;

if (! S) return(STREAM_CLOSED);
if (S->out_fd==-1) return(STREAM_CLOSED);


while (count < DataLen)
{
if (S->Flags & SF_SSL)
{
#ifdef HAVE_LIBSSL
result=SSL_write((SSL *) S->Extra, Data + count, DataLen - count);
#endif
}
else
{
if (S->Timeout > 0)
{
   FD_ZERO(&selectset);
   FD_SET(S->out_fd, &selectset);
   tv.tv_sec=S->Timeout;
   tv.tv_usec=0;
   result=select(S->out_fd+1,NULL,&selectset,NULL,&tv);

  if (result==-1)  return(STREAM_CLOSED);
  if ((result == 0) || (! FD_ISSET(S->out_fd, &selectset))) return(STREAM_TIMEOUT);
}

  result=write(S->out_fd, Data + count, DataLen - count);
}

  if (result < 1 && ((errno !=EINTR) && (errno !=EAGAIN)) ) break;
  if (result < 0) result=0;
  count+=result;
}

return(count);
}



/*A stream can have a series of 'processor modules' associated with it' */
/*which do things to the data before it is read/written. This function  */
/*pumps the data through the processor list, and eventually writes it out */
int STREAMPump(STREAM *S, int Final)
{
TProcessingModule *Mod;
ListNode *Curr;
char *InBuff=NULL, *OutBuff=NULL;
int len, olen, result=0;
int AllDataWritten=FALSE;

if (! S) return(-1);
if (S->out_fd==-1) return(-1);

len=S->OutEnd - S->OutStart;
InBuff=SetStrLen(InBuff,len+1);
memset(InBuff,0,len+1);
memcpy(InBuff,S->OutputBuff + S->OutStart,len);

//This looks strange.. but we have copied all the data
//into the buffer that we pass to the DataProcessors
S->OutEnd=0;
S->OutStart=0;

while (! AllDataWritten)
{
  AllDataWritten=TRUE;
  if ((! Final) && (len==0)) break;
  Curr=GetNextListItem(S->ProcessingModules);
  while (Curr)
  {
     Mod=(TProcessingModule *) Curr->Item;

     if (len < (BUFSIZ / 2)) olen=BUFSIZ;
     else
     {
        olen=len*2;
        if (Final) AllDataWritten=FALSE;
     }

     OutBuff=SetStrLen(OutBuff,olen);
		 result=0;

     if (Final && Mod->Flush) result=Mod->Flush(Mod,InBuff,len,OutBuff,olen);
     else if (Mod->Write && ((len > 0) || Final)) result=Mod->Write(Mod,InBuff,len,OutBuff,olen);
		
		 len=0;			
     while (result > 0)
     {
          if (Final) AllDataWritten=FALSE;
          InBuff=SetStrLen(InBuff,len+result);
          memcpy(InBuff+len,OutBuff,result);
					len+=result;
					if (result < olen) break;

					if (Final) result=EOF;
					else result=0;
			    if (Final && Mod->Flush) result=Mod->Flush(Mod,InBuff,0,OutBuff,olen);
					else if (Mod->Write) result=Mod->Write(Mod,InBuff,0,OutBuff,olen);
     }
     Curr=GetNextListItem(Curr);
  }

//Whatever happened above, InBuff should now contain the data to be written!
if (len > 0)
{
        result=STREAMInternalFinalWriteBytes(S, InBuff, len);
        if (result==0) result=STREAM_TIMEOUT;
        if (result < 1)
        {
                S->Flags |= SF_WRITE_ERROR;
                break;
        }
}
len=0;
}

DestroyString(OutBuff);
DestroyString(InBuff);

return(result);
}




int  STREAMFlush(STREAM *S)
{
return(STREAMPump(S,0));
}

void STREAMClear(STREAM *S)
{
 STREAMPump(S,0);
 S->OutStart=0;
 S->InStart=0;
}


/*A stream can have a series of 'processor modules' associated with it' */
/*which do things to the data before it is read/written. This function  */
/*pumps the data through the processor list, and eventually writes it out */
int STREAMReadThroughProcessors(STREAM *S, char *Bytes, int InLen)
{
TProcessingModule *Mod;
ListNode *Curr;
char *InBuff=NULL, *OutBuff=NULL;
int len=0, olen=0;

len=InLen;

if (InLen > 0)
{
InBuff=SetStrLen(InBuff,len+1);
memcpy(InBuff,Bytes ,len);
}

Curr=GetNextListItem(S->ProcessingModules);
while (Curr)
{
Mod=(TProcessingModule *) Curr->Item;
if (len==0) olen=1024;
else olen=len * 8;

OutBuff=SetStrLen(OutBuff,olen);

len=Mod->Read(Mod,InBuff,len,OutBuff,olen);

if (len==-1)
{
    S->Flags |=SF_DATA_ERROR;
    break;
}

if (len==0) break;
else
{
	InBuff=SetStrLen(InBuff,len);
	memcpy(InBuff,OutBuff,len);
}

Curr=GetNextListItem(Curr);
}

if (
			(! (S->Flags & SF_DATA_ERROR)) &&
			len
		)
{
//Whatever happened above, InBuff should now contain the data to be written!
//note that we resize buff to S->InEnd + len, where len is length of the new
//data. Even if S->InStart > 0 (meaning there are 'sent' bytes in the buffer)
//we consider S->InStart to be 0 as regards sizeing the buffer, because those
//sent bytes are still there.
S->InputBuff=SetStrLen(S->InputBuff,len + S->InEnd );
memcpy(S->InputBuff + S->InEnd, InBuff, len);
S->InEnd+=len;
}

DestroyString(OutBuff);
DestroyString(InBuff);

olen=S->InEnd - S->InStart;

if ((olen==0) && (S->Flags & SF_DATA_ERROR)) return(STREAM_DATA_ERROR);
return(olen);
}



/* This reads chunks from a file and when if finds a newline it resets */
/* the file pointer to that position */
void STREAMResizeBuffer(STREAM *Stream, int size)
{
   Stream->InputBuff=(char *) realloc(Stream->InputBuff,size);
   Stream->OutputBuff=(char *) realloc(Stream->OutputBuff,size);
   Stream->BuffSize=size;
   if (Stream->InStart > Stream->BuffSize) Stream->InStart=0;
   if (Stream->InEnd > Stream->BuffSize) Stream->InEnd=0;
   if (Stream->OutStart > Stream->BuffSize) Stream->OutStart=0;
   if (Stream->OutEnd > Stream->BuffSize) Stream->OutEnd=0;
}


void STREAMSetTimeout(STREAM *Stream, int val)
{
Stream->Timeout=val;
}


void STREAMSetFlushType(STREAM *Stream, int Type, int val)
{
Stream->Flags|=Type;
//Stream->FlushVal=val;
}


int STREAMLock(STREAM *S, int val)
{
int result;

result=flock(S->in_fd,val);

if (result==0) return(TRUE);
return(FALSE);
}


STREAM *STREAMCreate()
{
STREAM *S;

S=(STREAM *) calloc(1,sizeof(STREAM));
STREAMResizeBuffer(S,4096);
S->in_fd=-1;
S->out_fd=-1;
S->Timeout=30;

return(S);
}

STREAM *STREAMFromFD(int fd)
{
STREAM *Stream;

if (fd==-1) return(NULL);

Stream=STREAMCreate();
Stream->in_fd=fd;
Stream->out_fd=fd;
return(Stream);
}

STREAM *STREAMFromDualFD(int in_fd, int out_fd)
{
STREAM *Stream;

if (in_fd==-1) return(NULL);
if (out_fd==-1) return(NULL);

Stream=STREAMCreate();
Stream->in_fd=in_fd;
Stream->out_fd=out_fd;
return(Stream);
}


STREAM *STREAMSpawnCommand(char *Command, int Type)
{
int to_fd, from_fd;
STREAM *S=NULL;

if (Type==COMMS_BY_PIPE)
{
if (! PipeSpawn(&to_fd, &from_fd, NULL, Command)) return(NULL);
S=STREAMFromDualFD(from_fd, to_fd);
}
else if (Type==COMMS_BY_PTY) 
{
PseudoTTYSpawn(&to_fd,Command);
S=STREAMFromFD(to_fd);
}
STREAMSetFlushType(S,FLUSH_LINE,0);
return(S);
}


STREAM *STREAMOpenFile(char *FilePath, int Flags)
{
int fd, Mode=FALSE;
STREAM *Stream;
struct stat myStat;

Mode = Flags & ~(O_LOCK|O_TRUNC);

if (strcmp(FilePath,"-")==0)
{
return(STREAMFromDualFD(0,1));
}

fd=open(FilePath, Mode, 0600);
if (fd==-1) return(NULL);

if (Flags & O_LOCK)
{
	if (flock(fd,LOCK_EX | LOCK_NB)==-1)
	{
		close(fd);
		return(NULL);
	}

}

// check for symlink naughtyness. Basically a malicious user can
// try to guess the name of the file we are going to open in order
// to get us to write somewhere other than intended.


if (lstat(FilePath, &myStat) !=0)
{
  close(fd);
  return(NULL);
}

if (S_ISLNK(myStat.st_mode))
{
	syslog(LOG_USER | LOG_WARNING, "STREAMOpenFile Opened symlink when trying to open %s. Possible DOS attack?",FilePath);
  close(fd);
  return(NULL);
}

if (Flags & O_TRUNC) ftruncate(fd,0);

Stream=STREAMFromFD(fd);
Stream->Path=CopyStr(Stream->Path,FilePath);
return(Stream);
}


STREAM *STREAMClose(STREAM *S)
{
int len;

if (! S) return;
len=S->OutEnd - S->OutStart; 

STREAMReadThroughProcessors(S, NULL, 0);
STREAMPump(S,1);

if ((S->out_fd != -1) && (S->out_fd != S->in_fd)) close(S->out_fd);
if (S->in_fd != -1) close(S->in_fd);

DestroyList(S->Values,(LIST_ITEM_DESTROY_FUNC)DestroyString);
DestroyList(S->ProcessingModules,DataProcessorDestroy);
DestroyString(S->InputBuff);
DestroyString(S->OutputBuff);
DestroyString(S->Path);
free(S);

return(NULL);
}


int STREAMDisassociateFromFD(STREAM *Stream)
{
int fd;

if (! Stream) return;
fd=Stream->in_fd;
STREAMFlush(Stream);
DestroyString(Stream->InputBuff);
DestroyString(Stream->OutputBuff);
DestroyString(Stream->Path);
free(Stream);
return(fd);
}




int STREAMReadCharsToBuffer(STREAM *S)
{
fd_set selectset;
int result, diff, read_result=0, WaitForBytes=TRUE;
struct timeval tv;
char *tmpBuff=NULL;
int v1, v2,v3;


if (S->InStart >= S->InEnd)
{
S->InEnd=0;
S->InStart=0;
}
diff=S->InEnd-S->InStart;

if (S->InStart > (S->BuffSize / 2))
{
  memmove(S->InputBuff,S->InputBuff + S->InStart,diff);
  S->InStart=0;
  S->InEnd=diff;
}

v1=S->InStart; v2=S->InEnd; v3=S->BuffSize;

//if no room in buffer, we can't read in more bytes
if (S->InEnd >= S->BuffSize) return(1);


//if there are bytes available in the internal OpenSSL buffers, when we don't have to 
//wait on a select, we can just go straight through to SSL_read
#ifdef HAVE_LIBSSL
if (S->Flags & SF_SSL)
{
if (SSL_pending((SSL *) S->Extra) > 0) WaitForBytes=FALSE;
}
//else
#endif



if ((S->Timeout > 0) && WaitForBytes)
{
   FD_ZERO(&selectset);
   FD_SET(S->in_fd, &selectset);
   tv.tv_sec=S->Timeout;
   tv.tv_usec=0;
   result=select(S->in_fd+1,&selectset,NULL,NULL,&tv);

	switch (result)
	{
		case -1:
		if (errno==EINTR) read_result=STREAM_TIMEOUT;
		else read_result=STREAM_CLOSED;
		break;

		case 0:
		errno=ETIMEDOUT;
		read_result=STREAM_TIMEOUT;
		break;

		default:
		 read_result=0;
		break;
	}

}

//must do this, as we need it to be 0 if we don't do the reads
result=0;

if (read_result==0)
{
	tmpBuff=SetStrLen(tmpBuff,S->BuffSize-S->InEnd);

	#ifdef HAVE_LIBSSL
	if (S->Flags & SF_SSL)
	{
		read_result=SSL_read((SSL *) S->Extra, tmpBuff, S->BuffSize-S->InEnd);
	}
	else
	#endif
	{
		read_result=read(S->in_fd, tmpBuff, S->BuffSize-S->InEnd);
	}

	if (read_result > 0) result=read_result;
	else
	{
        if ((read_result == -1) && (errno==EAGAIN)) read_result=STREAM_NODATA;
        else read_result=STREAM_CLOSED;
        result=0;
	}
}

result=STREAMReadThroughProcessors(S, tmpBuff, result);
//if (result==STREAM_DATA_ERROR) read_result=STREAM_DATA_ERROR;
if (result !=0) read_result=result;

//We are not returning number of bytes read. We only return something if
//there is a condition (like socket close) where the thing we are waiting for 
//may not appear

DestroyString(tmpBuff);
return(read_result);
}






int STREAMReadBytes(STREAM *S, char *Buffer, int Buffsize)
{
char *ptr=NULL;
int bytes=0, result=0, total=0;

ptr=Buffer;

if (S->InStart >= S->InEnd) 
{
  result=STREAMReadCharsToBuffer(S);
	if (S->InStart >= S->InEnd)
	{
	  if (result==STREAM_CLOSED) return(EOF);
	  if (result==STREAM_TIMEOUT) return(STREAM_TIMEOUT);
	  if (result==STREAM_DATA_ERROR) return(STREAM_DATA_ERROR);
	}
}

while (total < Buffsize)
{
bytes=S->InEnd - S->InStart;
if (bytes > (Buffsize-total)) bytes=(Buffsize-total);

memcpy(ptr+total,S->InputBuff+S->InStart,bytes);
S->InStart+=bytes;
total+=bytes;

bytes=S->InEnd - S->InStart;


if (bytes < 1) 
{
	//in testing, the best way to prevent doing constant checking for new bytes,
	//and so filling up the buffer, was to only check for new bytes if
	//we didn't have enough to satisfy another read like the one we just had

	//We must check for '< 1' rather than '-1' because 
	result=FDCheckForBytes(S->in_fd);

	if (result ==-1) 
	{
		if (total==0) total=EOF;
		break;
	}
	if (result < 1) break;

	result=STREAMReadCharsToBuffer(S);
	if (result < 1)
	{
		if (total > 0) return(total);
		else return(result);
	}
}


}
return(total);
}



int STREAMTell(STREAM *S)
{
int pos;
pos=lseek(S->in_fd,0,SEEK_CUR);
pos-=(S->InEnd-S->InStart);

return(pos);
}


int STREAMSeek(STREAM *S, off_t offset, int whence)
{
int pos, wherefrom;

if (S->OutEnd != S->OutStart)
{
	STREAMFlush(S);
} 

if (whence==SEEK_CUR) 
{
  pos=STREAMTell(S);
  pos+=offset;
  wherefrom=SEEK_SET;
}
else 
{
  pos=offset;
  wherefrom=whence;
}
S->InStart=0;
S->InEnd=0;



return(lseek(S->in_fd,pos,wherefrom));
}



int STREAMWriteBytes(STREAM *S, char *Buffer, int Bytes)
{
int result=TRUE;
int len;
int written=0, remaining;

if (S->Flags & SF_WRITE_ERROR) return(STREAM_CLOSED);

while (written < Bytes)
{
len=S->BuffSize - S->OutEnd;

remaining=Bytes-written;
if (remaining < len) len=remaining;
memcpy(S->OutputBuff+S->OutEnd,Buffer+written,len);
S->OutEnd+=len;
written+=len;

len=S->BuffSize - S->OutEnd;
if (len < 1) STREAMPump(S,0);
}

return(written);
}


int STREAMWriteString(char *Buffer, STREAM *S)
{
int result;

if (StrLen(Buffer) < 1) return(FALSE);
result=STREAMWriteBytes(S,Buffer,strlen(Buffer));
return(result);
}

int STREAMWriteLine(char *Buffer, STREAM *S)
{
int result;

if (StrLen(Buffer) < 1) return(FALSE);
result=STREAMWriteBytes(S,Buffer,strlen(Buffer));
if (result < 0) return(result);
if (S->Flags & FLUSH_LINE) result=STREAMFlush(S);
return(result);
}


int STREAMReadChar(STREAM *S)
{
unsigned char inchar;
int result;

result=STREAMReadBytes(S, &inchar,1);
if (result < 1) return(result);
else return((int) inchar);
}



int STREAMPeekChar(STREAM *S)
{
int result;

if (S->InStart >= S->InEnd) 
{
    result=STREAMReadCharsToBuffer(S);
    if (result < 1) return(result);
}

return(* (S->InputBuff + S->InStart));
}


int STREAMWriteChar(STREAM *S, char inchar)
{
char tmpchar;

tmpchar=inchar;
return(STREAMWriteBytes(S,&tmpchar,1));
}



int STREAMReadBytesToTerm(STREAM *S, char *Buffer, int BuffSize, char Term)
{
int inchar, pos=0;

inchar=STREAMReadChar(S);
while (inchar != EOF) 
{
if (inchar > -1)
{
Buffer[pos]=inchar;
pos++;
if (inchar==Term) break;
if (pos==BuffSize) break;
}
inchar=STREAMReadChar(S);
}

if ((pos==0) && (inchar==EOF)) return(EOF);
return(pos);
}


char *STREAMReadToTerminator(char *Buffer, STREAM *S, char Term)
{
int inchar, len=0;
char *Tempptr;

Tempptr=CopyStr(Buffer,"");

inchar=STREAMReadChar(S);

while (inchar > 0)
{
	//if ((len % 100)== 0) Tempptr=realloc(Tempptr,(len/100 +1) *100 +2);
	//*(Tempptr+len)=inchar;
    	Tempptr=AddCharToBuffer(Tempptr,len,(char) inchar);
			len++;

    if (inchar==Term) break;
    inchar=STREAMReadChar(S);
}
*(Tempptr+len)='\0';

//if ((inchar==EOF) && (errno==ECONNREFUSED)) return(Tempptr);
if (
	((inchar==EOF) || (inchar==STREAM_DATA_ERROR))
	&& 
	(StrLen(Tempptr)==0)
   )
{
  free(Tempptr);
  return(NULL);
}

return(Tempptr);
}



char *STREAMReadToMultiTerminator(char *Buffer, STREAM *S, char *Terms)
{
int inchar, len=0;
char *Tempptr;

Tempptr=CopyStr(Buffer,"");

inchar=STREAMReadChar(S);

while (inchar > 0)
{
	//if ((len % 100)== 0) Tempptr=realloc(Tempptr,(len/100 +1) *100 +2);
	//*(Tempptr+len)=inchar;
    	Tempptr=AddCharToBuffer(Tempptr,len,(char) inchar);
			len++;

    if (strchr(Terms,inchar)) break;
    inchar=STREAMReadChar(S);
}
*(Tempptr+len)='\0';

//if ((inchar==EOF) && (errno==ECONNREFUSED)) return(Tempptr);
if (
	((inchar==EOF) || (inchar==STREAM_DATA_ERROR))
	&& 
	(StrLen(Tempptr)==0)
   )
{
  free(Tempptr);
  return(NULL);
}

return(Tempptr);
}




char *STREAMReadLine(char *Buffer, STREAM *S)
{
return(STREAMReadToTerminator(Buffer,S,'\n'));
}


void STREAMResetInputBuffers(STREAM *S)
{
int pos;

pos=STREAMTell(S);
S->InStart=0;
S->InEnd=0;
STREAMSeek(S,pos,SEEK_SET);
}


int IndexedFileLoad(STREAM *S)
{
char *Tempstr=NULL, *IndexVal=NULL;

if (! S) return(FALSE);
if (! S->Index) S->Index=CreateEmptyList();

Tempstr=STREAMReadLine(Tempstr,S);
while (Tempstr)
{
IndexVal=CopyStrLen(IndexVal,Tempstr,10);
OrderedListAddNamedItem(S->Index, IndexVal,(void *) STREAMTell(S));
Tempstr=STREAMReadLine(Tempstr,S);
}

return(TRUE);
}



int IndexedFileFind(STREAM *S, char *Key)
{
char *Tempstr=NULL, *IndexVal=NULL;
ListNode *Curr;
int result=FALSE;

if (! S) return(FALSE);
if (! S->Index) S->Index=CreateEmptyList();

IndexVal=CopyStrLen(IndexVal,Key,10);
Curr=ListFindNamedItem(S->Index,IndexVal);
while (Curr)
{
STREAMSeek(S, (int) Curr->Item,SEEK_SET);
Tempstr=STREAMReadLine(Tempstr,S);
StripTrailingWhitespace(Tempstr);
if (strcmp(Tempstr,Key)==0)
{
	result=TRUE;
	break;
}
Curr=GetNextListItem(Curr);

if (strcmp(IndexVal,Curr->Tag) !=0) break;
}

DestroyString(Tempstr);
DestroyString(IndexVal);

return(result);
}



int IndexedFileWrite(STREAM *S, char *Line)
{
char *IndexVal=NULL;

if (! S) return(FALSE);
if (! S->Index) S->Index=CreateEmptyList();

IndexVal=CopyStrLen(IndexVal,Line,10);
OrderedListAddNamedItem(S->Index, IndexVal,(void *) STREAMTell(S));
STREAMWriteLine(Line,S);

return(TRUE);
}



