#include "containerfiles.h"
#include "outputfiles.h"


#define M3U_STREAMINFO 1
#define M3U_PLAYLIST   2

char *ContainerTypes[]={".m3u",".m3u8",".pls",".asx",NULL};

int GetContainerFileType(char *URL)
{
char *Tempstr=NULL, *ptr;
int val;


Tempstr=CopyStr(Tempstr,URL);
//strip off any 'http GET' style arguments
ptr=strrchr(Tempstr,'?');
if (ptr) *ptr='\0';

//get path file extension
ptr=strrchr(Tempstr,'.');

val=MatchTokenFromList(ptr,ContainerTypes,0);

DestroyString(Tempstr);

return(val);
}


int DownloadStream(char *URL, char *Title, ListNode *Items, int Flags)
{
STREAM *Con=NULL, *S=NULL;
ListNode *Curr;
char *Tempstr=NULL, *ptr;
char *Server=NULL, *Doc=NULL;
int Port, len=0, BytesRead=0;

	Curr=ListGetNext(Items);

	if (Flags & (FLAG_TEST | FLAG_TEST_SITES) )
	{
		if (Flags & FLAG_TEST) fprintf(stderr,"TEST MODE: would have downloaded '%s' url=%s\n",Title,Curr->Item);
		if (Curr) return(TRUE);
		return(FALSE);
	}

	OpenOutputFiles(Title,URL,&BytesRead);
	while (Curr)
	{
		if (strncmp((char *) Curr->Item,"http:",5)==0) Tempstr=CopyStr(Tempstr,(char *) Curr->Item);
		else
		{
			Tempstr=CopyStr(Tempstr,URL);
			ptr=strrchr(Tempstr,'/');
			if (ptr) *ptr='\0';
			Tempstr=MCatStr(Tempstr,"/",(char *) Curr->Item,NULL);
		}
		ptr=HTTPParseURL(Tempstr,NULL,&Server,&Port,NULL,NULL);
		if (Port==0) Port=DefaultPort;
		Doc=CopyStr(Doc,ptr);

		Con=ConnectAndRetryUntilDownload(Server, Doc, Port, 0, 0);
		if (Con)
		{
			ptr=STREAMGetValue(Con,"HTTP:content-length");
			if (ptr) len=atoi(ptr);
			TransferItem(Con, Title, Curr->Item, len, &BytesRead);
			STREAMClose(Con);
		}
		Curr=ListGetNext(Curr);
	}
	CloseOutputFiles();

DestroyString(Tempstr);
DestroyString(Server);
DestroyString(Doc);

return(TRUE);
}

int DownloadM3U(char *URL, char *Title, int Flags)
{
char *Tempstr=NULL, *Server=NULL, *Doc=NULL, *ptr;
int Port=0, BytesRead=0, len=0;
int RetVal=FALSE;
ListNode *Items, *Curr;
int M3UType=M3U_PLAYLIST;
STREAM *Con;

ptr=HTTPParseURL(URL,&Tempstr,&Server,&Port,NULL,NULL);
if (Port==0) Port=DefaultPort;
Doc=CopyStr(Doc,ptr);

Items=ListCreate();
Con=ConnectAndRetryUntilDownload(Server, Doc, Port, 0, 0);
if (Con)
{
Tempstr=STREAMReadLine(Tempstr,Con);
while (Tempstr)
{
StripTrailingWhitespace(Tempstr);
StripLeadingWhitespace(Tempstr);

if (StrLen(Tempstr))
{
	if (*Tempstr != '#') ListAddItem(Items,CopyStr(NULL,Tempstr));
	else if (strncmp("#EXT-X-STREAM-INF",Tempstr,StrLen("#EXT-X-STREAM-INF"))==0) M3UType=M3U_STREAMINFO;
	else if (strncmp("#EXT-X-MEDIA-SEQUENCE",Tempstr,StrLen("#EXT-X-MEDIA-SEQUENCE"))==0) M3UType=M3U_PLAYLIST;
}

Tempstr=STREAMReadLine(Tempstr,Con);
}

if (M3UType==M3U_STREAMINFO) 
{
	Curr=ListGetNext(Items);
	RetVal=DownloadM3U(Curr->Item,Title,Flags);
}
else RetVal=DownloadStream(URL, Title, Items, Flags);
	
}

ListDestroy(Items,DestroyString);
DestroyString(Tempstr);
DestroyString(Server);
DestroyString(Doc);


return(RetVal);
}


int DownloadPLS(char *URL, char *Title, int Flags)
{
char *Tempstr=NULL, *Server=NULL, *Token=NULL, *Doc=NULL, *ptr;
int Port=0, len=0;
STREAM *Con=NULL;
int RetVal=FALSE;
ListNode *Items;

ptr=HTTPParseURL(URL,&Tempstr,&Server,&Port,NULL,NULL);
if (Port==0) Port=DefaultPort;
Doc=CopyStr(Doc,ptr);

Items=ListCreate();
Con=ConnectAndRetryUntilDownload(Server, Doc, Port, 0, 0);
if (Con)
{
Tempstr=STREAMReadLine(Tempstr,Con);
while (Tempstr)
{
StripTrailingWhitespace(Tempstr);
StripLeadingWhitespace(Tempstr);

if (StrLen(Tempstr))
{
	ptr=GetToken(Tempstr,"=",&Token,0);
	if (strncmp(Token,"File",4)==0) ListAddItem(Items,CopyStr(NULL,ptr));
}

Tempstr=STREAMReadLine(Tempstr,Con);
}

RetVal=DownloadStream(URL, Title, Items, Flags);
}
	
ListDestroy(Items,DestroyString);
DestroyString(Tempstr);
DestroyString(Server);
DestroyString(Doc);


return(RetVal);
}


int DownloadASX(char *URL, char *Title, int Flags)
{
char *Tempstr=NULL, *Server=NULL, *Token=NULL, *Doc=NULL, *ptr;
int Port=0, len=0;
int RetVal=FALSE;
STREAM *Con=NULL;
ListNode *Items;

ptr=HTTPParseURL(URL,&Tempstr,&Server,&Port,NULL,NULL);
if (Port==0) Port=DefaultPort;
Doc=CopyStr(Doc,ptr);

Items=ListCreate();
Con=ConnectAndRetryUntilDownload(Server, Doc, Port, 0, 0);
if (Con)
{
Tempstr=STREAMReadLine(Tempstr,Con);
while (Tempstr)
{
StripTrailingWhitespace(Tempstr);
StripLeadingWhitespace(Tempstr);

if (StrLen(Tempstr) && (strncasecmp(Tempstr,"<Ref href",9)==0))
{
		ptr=GetToken(Tempstr,"=",&Token,0);
		while (isspace(*ptr)) ptr++;
		if (*ptr=='"') ptr++;
		ptr=GetToken(ptr,"\"",&Token,0);

		ListAddItem(Items,CopyStr(NULL,Token));
}

Tempstr=STREAMReadLine(Tempstr,Con);
}

RetVal=DownloadStream(URL, Title, Items, Flags);
}
	
ListDestroy(Items,DestroyString);
DestroyString(Tempstr);
DestroyString(Server);
DestroyString(Doc);


return(RetVal);
}


int DownloadContainer(char *Path, char *Title, int Flags)
{
char *ptr;
int val, RetVal=FALSE;

val=GetContainerFileType(Path);

switch (val)
{
	case CONT_M3U:
	case CONT_M3U8:
   RetVal=DownloadM3U(Path, Title, Flags);
	break;

	case CONT_PLS:
    RetVal=DownloadPLS(Path, Title, Flags);
	break;

	case CONT_ASX:
  	RetVal=DownloadASX(Path, Title, Flags);
	break;
}

return(RetVal);
}
