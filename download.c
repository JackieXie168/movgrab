#include "download.h"

pid_t PlayerPid=0;
char *Player=NULL;
int PlayerLaunchPercent=25;
extern char *CmdLine, *ProgName;
	
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


void LaunchPlayer()
{
char *Tempstr=NULL;

Tempstr=MCopyStr(Tempstr,Player," ",OutputFilesGetFilePath(),NULL);
printf("Launch! %s\n\n",Tempstr);
PlayerPid=Spawn(Tempstr);

DestroyString(Tempstr);
}


//Display progress of download
void DisplayProgress(char *FullTitle, char *Format, unsigned int bytes_read, unsigned int DocSize,time_t Now, int PrintName)
{
float Percent, f1, f2;
unsigned int Bps=0;
char *HUDocSize=NULL, *BpsStr=NULL, *Title=NULL;
static time_t SpeedStart=0;
static unsigned int PrevBytesRead=0;

if ((Now-SpeedStart) == 0) return;

if (CheckForKeyboardInput()) PrintName=TRUE;


Title=CopyStrLen(Title,FullTitle,30);
Title=CatStr(Title,"...");
if (! (Flags & FLAG_QUIET)) 
{
if (PrintName) fprintf(stderr,"\nGetting: %s  Size: %s  Format: %s\n",Title,GetHumanReadableDataQty(DocSize,0), Format);
}



BpsStr=CopyStr(BpsStr,"");
if (SpeedStart > 0)
{
	Bps=(bytes_read - PrevBytesRead) / (Now-SpeedStart);
	BpsStr=MCopyStr(BpsStr,GetHumanReadableDataQty(Bps,0),"/s ",NULL);
}

if (DocSize)
{
	HUDocSize=CopyStr(HUDocSize,GetHumanReadableDataQty(DocSize,0));

	f1=(float) bytes_read * 100.0;
	f2=(float) DocSize;
	Percent=f1/f2;

	if (! (Flags & FLAG_QUIET)) fprintf(stderr,"	Progress: %0.2f%%  %s of %s  %s        \r",Percent,GetHumanReadableDataQty(bytes_read,0),HUDocSize,BpsStr);
	sprintf(CmdLine,"%s %0.2f%% %s          \0",ProgName,Percent,Title);

	if ((PlayerPid==0) && (Percent > PlayerLaunchPercent) && (Player)) LaunchPlayer();
}
else
{
	if (! (Flags & FLAG_QUIET)) fprintf(stderr,"	Progress: %s %s     \r",GetHumanReadableDataQty(bytes_read,0),BpsStr);
	sprintf(CmdLine,"%s %s              \0",ProgName,Title);
}

fflush(NULL);
if (Now - SpeedStart > 5) 
{
	SpeedStart=Now;
	PrevBytesRead=bytes_read;
}

DestroyString(HUDocSize);
DestroyString(BpsStr);
DestroyString(Title);
}


int TransferItem(STREAM *Con, char *Title, char *URL, char *Format, int DocSize, int *BytesRead)
{
char *Tempstr=NULL;
time_t Now, LastProgressDisplay;
int result, ReadThisTime=0, RetVal=FALSE;;


DisplayProgress(Title, Format, *BytesRead,DocSize,Now,TRUE);
Tempstr=SetStrLen(Tempstr,BUFSIZ);
result=STREAMReadBytes(Con,Tempstr,BUFSIZ);
while (result != EOF) 
{
	ReadThisTime +=result;
	(*BytesRead) +=result;
	time(&Now);
	if (Now != LastProgressDisplay) 
	{
		DisplayProgress(Title, Format, *BytesRead,DocSize,Now,FALSE);
		LastProgressDisplay=Now;
	}

	WriteOutputFiles(Tempstr,result);
	if ((DocSize > 0) && (ReadThisTime >= DocSize))
	{
		 break;
	}
	result=STREAMReadBytes(Con,Tempstr,BUFSIZ);
}
RetVal=TRUE;

//give a bit of time for 'player' program to finish
sleep(3);

DestroyString(Tempstr);

return(RetVal);
}



//----- Download an actual Video ----------------------
int DownloadItem(char *URL, char *Title, char *Format, int Flags)
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

if (! Con) Con=ConnectAndRetryUntilDownload(Server, Doc, Port, Flags, BytesRead);
if (Con)
{
//Some sites throttle excessively
STREAMSetTimeout(Con,STREAMTimeout);
ptr=strrchr(Doc,'?');
if (ptr) *ptr='\0';

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
			OpenOutputFiles(Title,URL,&BytesRead);
			//if (S) 
			{
			ptr=STREAMGetValue(Con,"HTTP:content-length");
			RetVal=TransferItem(Con,Title, URL, Format, atoi(ptr), &BytesRead);
			Extn=CopyStr(Extn,GuessExtn(GetVar(Con->Values,"HTTP:Content-Type"), Doc));
			CloseOutputFiles(Extn);
			}
	
		}
		STREAMClose(Con);
}


DestroyString(ContentType);
DestroyString(Tempstr);
DestroyString(Extn);
DestroyString(Token);
DestroyString(Server);
DestroyString(Doc);

return(RetVal);
}

