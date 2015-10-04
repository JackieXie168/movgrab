#include "download.h"

	
/*
Functions relating to connecting to hosts and downloading webpages.
All the HTTP stuff is in here
*/ 



extern int STREAMTimeout;

STREAM *ConnectAndSendHeaders(char *Server, char *Doc, int Port, int Flags, int BytesRange)
{
STREAM *Con;
char *Tempstr=NULL, *Method=NULL;
HTTPInfoStruct *Info;
static char *LastPage=NULL;

if (Flags & FLAG_POST) Method=CopyStr(Method,"POST");
else Method=CopyStr(Method,"GET");

if (Port==0) Port=80;

//Tempstr=MCopyStr(Tempstr,"http://",Server,"/",Doc,NULL);;
Tempstr=FormatStr(Tempstr,"http://%s:%d/%s",Server,Port,Doc);

Info=HTTPInfoFromURL(Method, Tempstr);
//if (DLFlags & DL_NOREDIRECT) Info->Flags |=HTTP_NOREDIRECT;
if (Flags & FLAG_HTTPS) Info->Flags |= HTTP_SSL|HTTP_SSL_REWRITE;
if (Flags & FLAG_DEBUG3) Info->Flags |= HTTP_DEBUG;


SetVar(Info->CustomSendHeaders,"Referer",LastPage); 

LastPage=CopyStr(LastPage,Tempstr);
if (BytesRange > 0)
{
	Tempstr=FormatStr(Tempstr,"bytes=%d-",BytesRange);
	SetVar(Info->CustomSendHeaders,"Range",Tempstr); 
}

Con=HTTPTransact(Info);
if ((! Con) && (! (Flags & FLAG_QUIET))) 
{
	if (StrLen(Info->ResponseCode)) fprintf(stderr,"ERROR: Server %s item '%s' not retrieved\nResponseCode: %s\n",Server, Doc,Info->ResponseCode);
	else fprintf(stderr,"ERROR: Connection failed to %s can't get file=%s \n",Server, Doc);
}
DestroyString(Tempstr);
DestroyString(Method);

return(Con);
}



STREAM *ConnectAndRetryUntilDownload(char *Server, char *Doc, int Port, int Flags, int BytesRead)
{
STREAM *Con;
int i, val=1;
char *Tempstr=NULL, *ptr;


for (i=0; i < 20; i++)
{
Con=ConnectAndSendHeaders(Server, Doc, Port, Flags, BytesRead);

if (! (Flags & FLAG_RETRY_DOWNLOAD)) break;

if (Con)
{
ptr=GetVar(Con->Values,"HTTP:Content-Length");
if (ptr) val=atoi(ptr);

ptr=GetVar(Con->Values,"HTTP:Content-Type");
if (Flags & FLAG_DEBUG) fprintf(stderr,"ContentType: %s\n",ptr);

//If the Content Type is not text, then we have the video
//else read the text, disconnect, and reconnect
if ((val > 0) && (strncmp(ptr,"text/",5) !=0)) break;

Tempstr=STREAMReadLine(Tempstr,Con);
while (Tempstr) Tempstr=STREAMReadLine(Tempstr,Con);
STREAMClose(Con);
Con=NULL;
}

if (Flags & FLAG_TEST_SITES) break;

fprintf(stderr,"Failed to get item. Sleeping 5s and retrying\n");
sleep(5);
CheckForKeyboardInput();

}

DestroyString(Tempstr);

return(Con);
}



int TransferItem(STREAM *Con, STREAM *S,char *Title, char *URL, int DocSize, int *BytesRead)
{
// 'S' is the  save file 'SIn' in 
STREAM *Cache=NULL,*StdOut=NULL;
char *Tempstr=NULL;
time_t Now, LastProgressDisplay;
int result, ReadThisTime=0, RetVal=FALSE;;



if (Flags & FLAG_STREAM)
{
	Cache=STREAMOpenFile(S->Path,O_RDONLY);
	StdOut=STREAMFromFD(1);
	STREAMSetTimeout(StdOut,1);
	if (! (Flags & FLAG_STREAMCACHE)) unlink(S->Path);
}

DisplayProgress(Title, *BytesRead,DocSize,Now,TRUE);
Tempstr=SetStrLen(Tempstr,BUFSIZ);
result=STREAMReadBytes(Con,Tempstr,BUFSIZ);
while (result != EOF) 
{
	ReadThisTime +=result;
	(*BytesRead) +=result;
	time(&Now);
	if (Now != LastProgressDisplay) 
	{
		DisplayProgress(Title, *BytesRead,DocSize,Now,FALSE);
		LastProgressDisplay=Now;
	}
	STREAMWriteBytes(S,Tempstr,result);
	if (Cache && FDIsWritable(StdOut->out_fd))
	{
		result=STREAMReadBytes(Cache,Tempstr,result);
		STREAMWriteBytes(StdOut,Tempstr,result);
	}
	if ((DocSize > 0) && (ReadThisTime >= DocSize))
	{
		 break;
	}
	result=STREAMReadBytes(Con,Tempstr,BUFSIZ);
}
RetVal=TRUE;


if (Cache)
{
	//We've reused tempstr, so we can't trust its size
	Tempstr=SetStrLen(Tempstr,BUFSIZ);
	STREAMSetTimeout(StdOut,0);
	result=STREAMReadBytes(Cache,Tempstr,BUFSIZ);
	while (result != EOF)
	{
		STREAMWriteBytes(StdOut,Tempstr,result);
		result=STREAMReadBytes(Cache,Tempstr,BUFSIZ);
	}

STREAMFlush(StdOut);
STREAMClose(Cache);
STREAMDisassociateFromFD(StdOut);

//give a bit of time for 'player' program to finish
sleep(3);
}

DestroyString(Tempstr);

return(RetVal);
}



//----- Download an actual Video ----------------------
int DownloadItem(char *URL, char *Title, int Flags)
{
STREAM *Con=NULL, *S=NULL;
char *Tempstr=NULL, *Token=NULL, *ptr;
char *Server=NULL, *Doc=NULL, *ContentType=NULL;
int DocSize=0, BytesRead=0;
int Port;
int RetVal=FALSE;
char *Extn=NULL;


if (Flags & FLAG_TEST) 
{
	fprintf(stderr,"TEST MODE: would have downloaded '%s' url=%s\n",Title,URL);
	return;
}


if (Flags & FLAG_DEBUG) fprintf(stderr,"Next URL: %s\n",URL);

ptr=HTTPParseURL(URL,&Tempstr,&Server,&Port,NULL,NULL);
if (Port==0) Port=DefaultPort;
Doc=CopyStr(Doc,ptr);

if (Flags & FLAG_STREAMCACHE) 
{
	Con=OpenCacheFile(Title, URL);

	//if cached file exists, just change to 'write to stdout'
	if (Con) 
	{
		Flags &= ~(FLAG_STREAM | FLAG_STREAMCACHE);
		SaveFilePath=CopyStr(SaveFilePath,"-");
	}
}
if (! Con) Con=ConnectAndRetryUntilDownload(Server, Doc, Port, Flags, BytesRead);
if (Con)
{
//Some sites throttle excessively
STREAMSetTimeout(Con,STREAMTimeout);
ptr=strrchr(Doc,'?');
if (ptr) *ptr='\0';
Extn=CopyStr(Extn,GuessExtn(GetVar(Con->Values,"HTTP:Content-Type"), Doc));

Token=CopyStr(Token,GetVar(Con->Values,"HTTP:Content-Range"));
if (StrLen(Token))
{
	ptr=strrchr(Token,'/');
	ptr++;
	DocSize=atoi(ptr);
}
else
{
	Token=CopyStr(Token,GetVar(Con->Values,"HTTP:content-length"));
	if (StrLen(Token)) DocSize=atoi(Token);
}


		if (Flags & FLAG_TEST_SITES) 
		{
			RetVal=TRUE;
		}
		else
		{
			S=OpenSaveFile(Title,URL,&BytesRead);
			if (! S) 
			{
				if (Flags & FLAG_STREAM) S=OpenSaveFile("-",URL,&BytesRead);
				if (! S)
				{
					fprintf(stderr,"ERROR: Cannot open output file\n");
					return(FALSE);
				}
			}
			ptr=STREAMGetValue(Con,"HTTP:content-length");
			RetVal=TransferItem(Con,S, Title, URL, atoi(ptr), &BytesRead);
			if ((Flags & FLAG_STREAMCACHE) || (! (Flags & FLAG_STREAM)))
			{
				 Tempstr=MCopyStr(Tempstr,S->Path,Extn,NULL);
				 rename(S->Path,Tempstr);
			}
		}
		STREAMClose(Con);
		STREAMClose(S);
}


DestroyString(ContentType);
DestroyString(Tempstr);
DestroyString(Extn);
DestroyString(Token);
DestroyString(Server);
DestroyString(Doc);

return(RetVal);
}

