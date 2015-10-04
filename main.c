//Movgrab. A downloader for movie websites.
//Written by Colum Paget.
//Copyright 2010 Colum Paget.

//


/****  Gnu Public Licence ****/
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version. 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


//This is doable thorugh autoconf, but I'm sick of fighting with it
#define Version "1.0.6"

#include "libUseful-1.0/libUseful.h"
#include <string.h>


char *FileTypes[]={".flv",".mp3",".mp4",".mov",".wma",".m4a",".wmv",".avi",".3gp",NULL};
char *DownloadTypes[]={"none","generic","youtube","metacafe","google","dailymotion","break","ehow","vimeo","almostkilled","5min","ign","vbox7","blip.tv","3gpdb","ted","myvideo","clipshack","crazymotion","mytopclip","redbalcony","mobango","izlese","izles", "izles.org","berkeley","yale","sdnhm","uchannel","princeton","ucsd.tv","reuters","clipfish.de","liveleak","academicearth","photobucket","videoemo","videosfacebook","aljazeera",NULL};
char *DownloadNames[]={"none","Generic: Search in page for http://*.flv, http://*.mp3, http//*.mp4 etc, etc, etc","YouTube: http://www.youtube.com","Metacafe: http://www.metacafe.com","Google Video: http://video.google.com","Daily Motion: http://www.dailymotion.com","www.break.com","www.ehow.com","www.vimeo.com","www.almostkilled.com","www.5min.com","www.ign.com","www.vbox7.com","www.blip.tv","www.3gpdb.com","www.ted.com","www.myvideo.de","www.clipshack.com","crazymotion.net","www.mytopclip.com","www.redbalcony.com","www.mobango.com","www.izlese.org","www.izles.net","www.izles.org","Berkeley University: http://webcast.berkeley.edu","Yale University: http://oyc.yale.edu","San Diago Natural History Museum: http://www.sdnhm.org/webcasts/index.html","UChannel: http://uc.princeton.edu","Princeton University: http://www.princeton.edu/WebMedia/","University of California Television","Reuters","clipfish.de","Liveleak","Academic Earth","Photobucket","VideoEmo","Videos Facebook","Aljazeera",NULL};

typedef enum {TYPE_NONE, TYPE_GENERIC, TYPE_YOUTUBE, TYPE_METACAFE, TYPE_GOOGLE_VIDEO, TYPE_DAILYMOTION, TYPE_BREAK_COM, TYPE_EHOW,  TYPE_VIMEO, TYPE_ALMOST_KILLED, TYPE_FIVE_MIN, TYPE_IGN, TYPE_VBOX7,TYPE_BLIP_TV,TYPE_3GPDB,TYPE_TED, TYPE_MYVIDEO, TYPE_CLIPSHACK, TYPE_CRAZYMOTION, TYPE_MYTOPCLIP,TYPE_REDBALCONY, TYPE_MOBANGO,TYPE_IZLESE,TYPE_IZLES, TYPE_IZLES_ORG, TYPE_BERKELEY, TYPE_YALE, TYPE_SDNHM, TYPE_UCHANNEL, TYPE_PRINCETON, TYPE_UCSDTV, TYPE_REUTERS, TYPE_CLIPFISH_DE, TYPE_LIVELEAK, TYPE_LIVELEAK_STAGE2, TYPE_ACADEMIC_EARTH,TYPE_PHOTOBUCKET,TYPE_VIDEOEMO,TYPE_VIDEOSFACEBOOK,TYPE_ALJAZEERA,
/*Following ones are not real types, but used by internal processes */
TYPE_METACAFE_JS_REDIR, TYPE_METACAFE_FINAL, TYPE_VIMEO_STAGE2, TYPE_EHOW_STAGE2,TYPE_3GPDB_STAGE2,TYPE_BERKELEY_STAGE2, TYPE_YOUTUBE_REF, TYPE_CLIPSHACK_STAGE2, TYPE_CLIPSHACK_STAGE3, TYPE_VIDEOEMO_STAGE2}TDT;

#define FLAG_QUIET 1
#define FLAG_BACKGROUND 2
#define FLAG_DEBUG1 4
#define FLAG_DEBUG2 8
#define FLAG_DEBUG3 16
#define FLAG_PORN 32
#define FLAG_PRINT_USAGE 64
#define FLAG_HTTPS 128
#define FLAG_TEST  256
#define FLAG_STDIN 512
#define FLAG_POST  1024
#define FLAG_DL_RETRY  2048

#define EXTRACT_DEQUOTE  1
#define EXTRACT_WITHIN_QUOTES 2
#define EXTRACT_NOSPACES 4


int Flags=0;
char *ItemSelectionArg=NULL;
char *ProgName=NULL, *CmdLine=NULL;
char *FormatPreference=NULL;
char *SaveFilePath=NULL;
int Type=TYPE_NONE, DefaultPort=80;
ListNode *DownloadQueue=NULL;
STREAM *StdIn=NULL;
char *Username=NULL, *Password=NULL;


int DownloadItem(char *URL, char *Path, int Post);
void DownloadPage(char *Path, int Type, char *Title, int Post);

int CheckForKeyboardInput()
{
char *Tempstr=NULL;
int result=FALSE;

if (STREAMCheckForBytes(StdIn)) 
{
	Tempstr=STREAMReadLine(Tempstr,StdIn);
	StripTrailingWhitespace(Tempstr);
	if (StrLen(Tempstr))
	{
		AddItemToList(DownloadQueue,CopyStr(NULL,Tempstr));
		if (! (Flags & FLAG_QUIET)) fprintf(stderr,"\r\nQUEUED: %s\n",Tempstr);
		result=TRUE;
	}
}
DestroyString(Tempstr);

return(result);
}




//This function Extracts Text from a line that's found between two specified
//chunks of text 'ItemStart' and 'ItemEnd'
char *GenericExtractFromLine(char *Line, char *ItemName, char *ItemStart, char *ItemEnd, ListNode *Vars, int Flags)
{
char *ptr, *ptr2, *Token=NULL, *Item=NULL;
int GTF=0;

		if (Flags & EXTRACT_WITHIN_QUOTES) GTF=GETTOKEN_QUOTES;

		if (StrLen(ItemStart)) ptr=GetToken(Line,ItemStart,&Token,0);
		else ptr=Line;

		ptr=GetToken(ptr,ItemEnd,&Token,GTF);
		
		//check if the start string occurs more than once in the Token that we've grabbed
		if (StrLen(ItemStart)) ptr2=strstr(Token,ItemStart);
		else ptr2=NULL;

		while (ptr2)
		{
		ptr2+=StrLen(ItemStart);
		memmove(Token,ptr2,Token+StrLen(Token)-ptr2+1);
		//because of memmove we can strstr in Token again	
		ptr2=strstr(Token,ItemStart);
		}
		if (Flags & EXTRACT_DEQUOTE) Item=HTTPUnQuote(Item,Token);
		else Item=CopyStr(Item,Token);
		StripLeadingWhitespace(Item);
		StripTrailingWhitespace(Item);
		StripQuotes(Item);

		if (Flags & EXTRACT_NOSPACES)
		{
			ptr2=strchr(Item,' ');
			while (ptr2)
			{
				*ptr2='+';
				ptr2=strchr(ptr2,' ');
			}
		}

		SetVar(Vars,ItemName,Item);

DestroyString(Token);
DestroyString(Item);

return(ptr);
}
	

//---------------- Functions to do with files and file names ----------

//Strip out characters that make it difficult to specify a file on the 
//command line, such as '&' or ';' '>' that mean something to the shell
char *MakeFilesystemSafeName(char *Buffer, char *Path)
{
char *Str=NULL;
char BadChars[]="'\"()/?*&<>; ";
char *ptr;

Str=CopyStr(Buffer,"");
ptr=Path;
while (ptr && (*ptr != '\0'))
{
	if (strchr(BadChars,*ptr)) Str=AddCharToStr(Str,'_');
	else Str=AddCharToStr(Str,*ptr);
	ptr++;
}
return(Str);
}


//guess the extension of a file, some sites don't have extensions
//on their filenames
char *GuessExtn(char *ContentType, char *ID)
{
char *ptr=NULL;

if (ID) ptr=strchr(ID,'.');
if (!ptr) ptr=".flv";


if (ContentType)
{
if (strcmp(ContentType,"audio/mp3")==0) ptr=".mp3";
else if (strcmp(ContentType,"audio/mpeg")==0) ptr=".mp3";
else if (strcmp(ContentType,"video/x-flv")==0) ptr=".flv";
else if (strcmp(ContentType,"video/flv")==0) ptr=".flv";
else if (strcmp(ContentType,"video/mp4")==0) ptr=".mp4";
else if (strcmp(ContentType,"video/3gpp")==0) ptr=".3gp";
}

return(ptr);
}


//Open the file we are going to save the move to. Check in the user 
//filename preference stored in the global variable 'SaveFilePath'
//If that's set to '-' then open stdout and write to that, if it's
//set to anything else, then use that as the filename. Otherwise
//build the filename from available info.
//URL is used to provide a unique MD5 so that two downloads from
//different sites with the same title don't overwrite each other
STREAM *OpenSaveFile(char *Title, char *URL, int *FileSize)
{
char *Tempstr=NULL, *Path=NULL, *MD5=NULL, *Extn=NULL, *ptr;
STREAM *S;
struct stat FStat;

*FileSize=0;


if (StrLen(SaveFilePath))
{
	if (strcmp(SaveFilePath,"-")==0)
	{
		return(STREAMFromFD(1));
	}
	else Tempstr=CopyStr(Tempstr,SaveFilePath);
}
else if (StrLen(Title)) ptr=Title;
else 
{
		ptr=strrchr(URL,'?');
		if (ptr) *ptr='\0';


		ptr=strrchr(URL,'/');
		if (ptr) ptr++;
		else ptr=URL;
}


Tempstr=MakeFilesystemSafeName(Tempstr, ptr);
ptr=strrchr(Tempstr,'.');
if (ptr) *ptr='\0';


MD5=HashMD5(MD5,URL,StrLen(URL),0);
Tempstr=MCatStr(Tempstr,"-",MD5,NULL);

S=STREAMOpenFile(Tempstr,O_CREAT | O_RDWR);
if (! STREAMLock(S,LOCK_EX|LOCK_NB)) 
{
if (! (Flags & FLAG_QUIET)) fprintf(stderr,"Already downloading this item!\n");
fflush(NULL);
exit(0);
}
STREAMSeek(S,0,SEEK_END);

//Filesize can be used to resume a part download
fstat(S->in_fd,&FStat);
*FileSize=FStat.st_size;

DestroyString(Tempstr);
DestroyString(Extn);
DestroyString(MD5);

return(S);
}


//---- Functions relating to connecting to hosts and downloading webpages ----

STREAM *ConnectAndSendHeaders(char *Server, char *Doc, int Port, int Post, int BytesRange)
{
STREAM *Con;
char *Tempstr=NULL, *Method=NULL;
HTTPInfoStruct *Info;

if (Post) 
{
Method=CopyStr(Method,"POST");
}
else Method=CopyStr(Method,"GET");

if (Port==0) Port=80;

Tempstr=FormatStr(Tempstr,"http://%s:%d/%s",Server,Port,Doc);
//Info=HTTPInfoCreate(Server, Port, "","", Method, Tempstr, "",0);
Info=HTTPInfoFromURL(Method, Tempstr);
if (Flags & FLAG_HTTPS) Info->Flags |= HTTP_SSL|HTTP_SSL_REWRITE;
if (Flags & FLAG_DEBUG3) Info->Flags |= HTTP_DEBUG;

if (BytesRange > 0)
{
	Tempstr=FormatStr(Tempstr,"bytes=%d-",BytesRange);
	SetVar(Info->CustomSendHeaders,"Range",Tempstr); 
}
Con=HTTPTransact(Info);

DestroyString(Tempstr);
DestroyString(Method);

return(Con);
}

//Dispaly progress of download
void DisplayProgress(char *FullTitle, unsigned int bytes_read, unsigned int DocSize,time_t Now, int PrintName)
{
float Percent, f1, f2;
unsigned int Bps=0;
char *HUDocSize=NULL, *BpsStr=NULL, *Title=NULL;
static time_t SpeedStart=0;
static unsigned int PrevBytesRead=0;

if ((Now-SpeedStart) == 0) return;

if (CheckForKeyboardInput()) PrintName=TRUE;


Title=CopyStrLen(Title,FullTitle,30);
if (! (Flags & FLAG_QUIET)) 
{
if (PrintName) fprintf(stderr,"\nGetting: %s  Size: %s\n",Title,GetHumanReadableDataQty(DocSize,0));
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


STREAM *ConnectAndRetryUntilDownload(char *Server, char *Doc, int Port, int DLFlags, int BytesRead)
{
STREAM *Con;
int i;
char *Tempstr=NULL, *ptr;


for (i=0; i < 20; i++)
{
Con=ConnectAndSendHeaders(Server, Doc, Port, DLFlags & FLAG_POST,BytesRead);
if (! (DLFlags & FLAG_DL_RETRY))  break;

if (Con)
{
ptr=GetVar(Con->Values,"HTTP:Content-Type");
printf("CT: %s\n",ptr);
if (strncmp(ptr,"text/",5) !=0) break;

Tempstr=STREAMReadLine(Tempstr,Con);
while (Tempstr) Tempstr=STREAMReadLine(Tempstr,Con);
STREAMClose(Con);
Con=NULL;
printf("Sleeping 5s\n");
sleep(5);
CheckForKeyboardInput();
}

}

DestroyString(Tempstr);

return(Con);
}

//----- Download an actual Video ----------------------
int DownloadItem(char *URL, char *Title, int DLFlags)
{
STREAM *S, *Con;
char *Tempstr=NULL, *Token=NULL, *ptr;
char *Server=NULL, *Doc=NULL, *FileName=NULL, *ContentType=NULL;
int result, DocSize=0, BytesRead=0;
int len, Port;
int RetVal=FALSE;
char *Extn=NULL;
time_t Now, LastProgressDisplay;


if (Flags & FLAG_TEST) 
{
	fprintf(stderr,"TEST MODE: would have downloaded %s\n",URL);
	return;
}


if (Flags & (FLAG_DEBUG1 | FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"Next URL: %s\n",URL);

ptr=HTTPParseURL(URL,&Tempstr,&Server,&Port,NULL,NULL);
if (Port==0) Port=DefaultPort;
Doc=CopyStr(Doc,ptr);

S=OpenSaveFile(Title,URL,&BytesRead);

Con=ConnectAndRetryUntilDownload(Server, Doc, Port, DLFlags, BytesRead);
if (Con)
{
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
		DisplayProgress(Title, BytesRead,DocSize,Now,TRUE);
		Tempstr=SetStrLen(Tempstr,BUFSIZ);
		result=STREAMReadBytes(Con,Tempstr,BUFSIZ);
		while (result != EOF)
		{
			BytesRead+=result;
			time(&Now);
			if (Now != LastProgressDisplay) 
			{
				DisplayProgress(Title, BytesRead,DocSize,Now,FALSE);
				LastProgressDisplay=Now;
			}
			STREAMWriteBytes(S,Tempstr,result);
			result=STREAMReadBytes(Con,Tempstr,BUFSIZ);
		}
		RetVal=TRUE;

STREAMClose(Con);

Tempstr=MCopyStr(Tempstr,S->Path,Extn,NULL);
rename(S->Path,Tempstr);
}
else 
{
if (! (Flags & FLAG_QUIET)) 
{
fprintf(stderr,"ERROR: Connection failed to %s\n",Server);
fprintf(stderr,"ERROR: Connection failed to get %s (%s)\n",Doc,Title);
}
unlink(S->Path);
}

STREAMClose(S);
DestroyString(ContentType);
DestroyString(Tempstr);
DestroyString(Extn);
DestroyString(Token);
DestroyString(Server);
DestroyString(Doc);

return(RetVal);
}


/*

//Still trying to figure this out

void YoutubeLogin(char *Username, char *Password)
{
STREAM *S;
char *Tempstr=NULL;

S=HTTPGet("https://www.google.com/accounts/ServiceLogin?uilel=3&service=youtube&passive=true");

Tempstr=STREAMReadLine(Tempstr,S);
while (Tempstr)
{
			NextPath=MCopyStr(NextPath,"https://www.google.com/accounts/ServiceLoginAuth?service=youtube&Email=",Username,"&Passwd=",Password,"&signIn=Sign in","&continue=http://www.youtube.com/signin?action_handle_signin=true&next=",Token,NULL);
   		Post=TRUE;
}
}
*/



//-------- Go through the processes involved in getting a video file
void GrabMovie(char *Path, int MovType)
{
int i;
char *Proto=NULL, *Server=NULL, *Doc=NULL, *Tempstr=NULL, *Title=NULL;
char *NextPath=NULL;
char *ptr, *Token=NULL;
int Post=FALSE, Port;

Type=MovType;
NextPath=CopyStr(NextPath,Path);
ptr=HTTPParseURL(Path,&Proto,&Server,&Port,NULL,NULL);
Doc=CopyStr(Doc,ptr);
if (strcasecmp(Proto,"https")==0) 
{
Flags |= FLAG_HTTPS;
DefaultPort=443;
}
if (Port==0) Port=DefaultPort;

if (Type==TYPE_NONE) Type=IdentifyServiceType(Server);


if (Type==TYPE_NONE)
{
if (! (Flags & FLAG_QUIET)) fprintf(stderr,"Unrecognized url type. Try using the -t option to force the service type ( \"movgrab -?\" for more details )\n");
exit(0);
}
else if (Type==TYPE_YOUTUBE)
{

	//hmm.. have we been given the http//www.youtube.com/v/ format?
	if (strncmp(Doc,"v/",2)==0) Token=MCopyStr(Token,"watch?v=",Doc+2,NULL);
	else Token=CopyStr(Token,Doc);

	//Do we have authentication info?
	if (StrLen(Username) && StrLen(Password))
	{

	}
	else NextPath=MCopyStr(NextPath,Server,"/",Token,NULL);

}
else if (Type==TYPE_METACAFE)
{
	ptr=GetToken(Doc,"watch/",&Token,0);
	ptr=GetToken(ptr,"/",&Token,0);
	if (strchr(Token,'-'))
	{
		//Movie is embedded from elsewhere
		if (strncmp(Token,"yt",2)==0) 
		{
			ptr=strrchr(Token,'-');
			if (ptr) 
			{
				ptr++;
	//			Type=TYPE_NONE;
	//			Tempstr=MCopyStr(Tempstr,"http://www.youtube.com/watch?v=",ptr,NULL);
	//			GrabMovie(Tempstr);

				Type=TYPE_YOUTUBE;
				NextPath=MCopyStr(NextPath,"http://www.youtube.com/watch?v=",ptr,NULL);
	
			}
		}
		else if (! (Flags & FLAG_QUIET)) fprintf(stderr,"Movie is not genuinely stored on metacafe, it is embedded from elsewhere, but it's not youtube, so I don't know how to download it. Sorry.\n");
	}
	else 
	{
		STREAM *S;

		if (Flags & FLAG_PORN)
		{
		//Initial 'turn off family filter'
		Tempstr=FormatStr(Tempstr,"%s://%s:%d/f/index.php?inputType=filter&controllerGroup=user&filters=0&submit=Continue+-+I%27m+over+18",Proto,Server,Port);
		S=HTTPMethod("POST",Tempstr,"","");
		STREAMClose(S);

		//But we have to do it twice, probably something to do with cookies
		NextPath=FormatStr(NextPath,"http://%s:%d/f/index.php?inputType=filter&controllerGroup=user&filters=0&Continue=Continue+-+I%27m+over+18&prevURL=%s",Server,Port,Path);
		}
		else NextPath=CopyStr(NextPath,Path);
	}
}
else if (Type==TYPE_DAILYMOTION)
{
	if (Flags & FLAG_PORN)
	{
		//	Tempstr=FormatStr(Tempstr,"http://%s/pageitem/login?username=%s&password=%s&login_submit=Login&urlback=/",Server,Login,Passwd);
		//	HTTPMethod("POST",Tempstr,"","");

		Tempstr=HTTPQuote(Tempstr,Doc);
		NextPath=FormatStr(NextPath,"http://%s:%d/family_filter?urlback=/",Server,Port,Tempstr,"&form_name=dm_pageitem_familyfilter&accept=I+am+over+18+-+set+Family+Filter+OFF");
   Post=FLAG_POST;
	}
	else NextPath=CopyStr(NextPath,Path);
}
else if (Type==TYPE_PRINCETON)
{
	ptr=strrchr(Doc,'/');
	if (ptr)
	{
		ptr++;
		Title=CopyStr(Title,ptr);
		ptr=strrchr(Title,'.');
		if (ptr) *ptr='\0';
	}
 	NextPath=CopyStr(NextPath,Path);
}
else if (Type==TYPE_CLIPFISH_DE)
{
	ptr=strstr(Doc,"video/");
	if (ptr)
	{
		ptr+=6;
 		Tempstr=CopyStr(Tempstr,ptr);
		ptr=strchr(Tempstr,'/');
		if (ptr)
		{
			 *ptr='\0';
			ptr++;
			Title=CopyStr(Title,ptr);
			ptr=strchr(Title,'/');
			if (ptr) *ptr='\0';
		}
		NextPath=MCopyStr(NextPath,"http://www.clipfish.de/video_n.php?p=0|DE&vid=",Tempstr,NULL);
	}
}
else
{
 NextPath=CopyStr(NextPath,Path);
}

DownloadPage(NextPath, Type,Title,Post);

DestroyString(Tempstr);
DestroyString(Server);
DestroyString(Doc);
DestroyString(NextPath);
DestroyString(Token);
DestroyString(Title);
DestroyString(Proto);
}



//For websites with multiple videos on a page (not multiple formats of the
//same video, but actual different videos) decide which one to get
void GetDownloadSelectionList(char *Arg,ListNode *Vars,ListNode *Results)
{ 
char *Tempstr=NULL, *Var=NULL, *ptr;
int MediaCount, i, startpos, endpos;

Var=CopyStr(Var,GetVar(Vars,"MediaCount"));
MediaCount=atoi(Var);
startpos=0;
endpos=MediaCount;

		if (MediaCount==1) endpos=1;
		else if (StrLen(Arg)==0)
		{
			if (! (Flags & FLAG_QUIET))
			{
 			fprintf(stderr,"\nMultiple downloads exist on this page.\n");
			fprintf(stderr,"Please select by using the command-line argument -n <num>\n");
 			fprintf(stderr,"	e.g. -n all\n");
 			fprintf(stderr,"	     -n 0-4\n");
 			fprintf(stderr,"	     -n 3-\n\n");
			for (i=0; i < MediaCount; i++)
			{
			Tempstr=FormatStr(Tempstr,"ID:%d",i);
			ptr=GetVar(Vars,Tempstr);
			if (ptr) ptr=strrchr(ptr,'.');
			if (! ptr) ptr="?";

			Tempstr=FormatStr(Tempstr,"Title:%d",i);
			Var=CopyStr(Var,GetVar(Vars,Tempstr));
			fprintf(stderr,"	% 4d: % 4s  %s\n",i,ptr,Var);
			}
			fprintf(stderr,"\n");
			}
			endpos=startpos;
		}
		else if (strcmp(Arg,"all")==0)
		{
			//do nothing
		}
		else if (strchr(Arg,'-'))
		{
		ptr=GetToken(Arg,"-",&Tempstr,0);
		if (StrLen(Tempstr)) startpos=atoi(Tempstr);
		else startpos=0;
		if (StrLen(ptr)) endpos=atoi(ptr);
		else endpos=MediaCount;
		}
		else
		{
			startpos=atoi(Arg);
			endpos=startpos+1;
		}

		for (i=startpos; i < endpos; i++)
		{
		Tempstr=FormatStr(Tempstr,"ID:%d",i);
		Var=CopyStr(Var,GetVar(Vars,Tempstr));
		Tempstr=FormatStr(Tempstr,"Title:%d",i);
 		AddNamedItemToList(Results,GetVar(Vars,Tempstr),CopyStr(NULL,Var));
		}

DestroyString(Tempstr);
DestroyString(Var);
}

void AddToFormatsList(ListNode *Vars, char *Format)
{
char *Tempstr=NULL;

	Tempstr=CopyStr(Tempstr,GetVar(Vars,"formats"));
	if (! strstr(Tempstr,Format))
	{
		Tempstr=CatStr(Tempstr,"flv:320x240 ");
		StripTrailingWhitespace(Tempstr);
		SetVar(Vars,"formats",Tempstr);
	}

	DestroyString(Tempstr);
}


//------------ Functions specific to getting data from certain sites -----

void DoGetBerkeley(ListNode *Vars)
{
ListNode *Curr, *SelectionList=NULL;

SelectionList=CreateEmptyList();
GetDownloadSelectionList(ItemSelectionArg,Vars,SelectionList);

Curr=GetNextListItem(SelectionList);
while (Curr)
{
	DownloadItem(Curr->Item,Curr->Tag,0);
	Curr=GetNextListItem(Curr);
}

DestroyList(SelectionList,DestroyString);
}

void DoVBOX7(char *ID, char *Title)
{
char *Dir=NULL, *Tempstr=NULL;
int i;

Dir=CopyStrLen(Dir,ID,2);

for (i=2; i < 20; i++)
{
	if (i==0) Tempstr=FormatStr(Tempstr,"http://media.vbox7.com/s/%s/%s.flv",Dir,ID);
  else Tempstr=FormatStr(Tempstr,"http://media%02d.vbox7.com/s/%s/%s.flv",i,Dir,ID);
  if (DownloadItem(Tempstr, Title, 0)) break;
}

DestroyString(Dir);
DestroyString(Tempstr);
}


char *ExtractMetacafeMediaURL(char *RetStr, char *Data, char *Start, char *End)
{
char *Tempstr=NULL, *Token=NULL, *ptr;


		ptr=strstr(Data,Start);
		ptr+=StrLen(Start);
		ptr=GetToken(ptr,End,&Token,0);
		Tempstr=HTTPUnQuote(Tempstr,Token);
		RetStr=MCopyStr(RetStr,Tempstr,"?__gda__=",NULL);
		ptr=GetToken(Data,"gdaKey=",&Token,0);
		ptr=GetToken(ptr,"&",&Token,0);
		RetStr=CatStr(RetStr,Token);
	
DestroyString(Tempstr);
DestroyString(Token);


return(RetStr);
}

void DecodeYouTubeFormats(char *Formats, ListNode *Vars)
{
char *UnQuoted=NULL, *Token=NULL, *Tempstr=NULL, *ptr;

			UnQuoted=DeQuoteStr(UnQuoted, Formats);
			ptr=GetToken(UnQuoted,"|",&Token,0);
			while (ptr)
			{
				switch (atoi(Token))
				{
						case 5:
						ptr=GenericExtractFromLine(ptr, "item:flv:320x240","", ",", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
						AddToFormatsList(Vars,"flv:320x240 ");
						break;

						case 13:
						ptr=GenericExtractFromLine(ptr, "item:3gp","", ",", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
						AddToFormatsList(Vars,"3gp ");
						break;

						case 17:
						ptr=GenericExtractFromLine(ptr, "item:3gp","", ",", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
						AddToFormatsList(Vars,"3gp ");
						break;

						case 18:
						ptr=GenericExtractFromLine(ptr, "item:mp4-480x360","", ",", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
						AddToFormatsList(Vars,"mp4:480x360 ");
						break;

						case 22:
						ptr=GenericExtractFromLine(ptr, "item:mp4-1280x720","", ",", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
						AddToFormatsList(Vars,"mp4:1280x720 ");
						break;

						case 34:
						ptr=GenericExtractFromLine(ptr, "item:flv:h264-320x240","", ",", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
						AddToFormatsList(Vars,"flv:h264-320x240 ");
						break;

						case 35:
						ptr=GenericExtractFromLine(ptr, "item:flv:h264-480x368","", ",", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
						AddToFormatsList(Vars,"flv:h264-480x368 ");
						break;

						case 37:
						ptr=GenericExtractFromLine(ptr, "item:mp4-1920x1080","", ",", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
						AddToFormatsList(Vars,"mp4:1920-1080 ");
						break;
				}
			ptr=GetToken(ptr,"|",&Token,0);
			}

DestroyString(UnQuoted);
DestroyString(Tempstr);
DestroyString(Token);
}


char *DecodeDailyMotionFormats(char *Formats, ListNode *Vars)
{
char *Token=NULL, *ptr;

			printf("DMF: %s\n",Formats);
			ptr=GetToken(Formats,"||",&Token,0);
			while (ptr)
			{
			if (strstr(Token,"FLV-320x240")) 
			{
					SetVar(Vars,"item:flv:320x240",Token);
					AddToFormatsList(Vars,"flv:320x240 ");
			}
			else if (strstr(Token,"FLV-80x60")) 
			{
					SetVar(Vars,"item:flv:80x60",Token);
					AddToFormatsList(Vars,"flv:80x60 ");
			}
			else if (strstr(Token,"ON2-848x480"))
			{
					SetVar(Vars,"item:on2:848x480",Token);
					AddToFormatsList(Vars,"on2:848x480 ");
			}
			else if (strstr(Token,"ON2-320x240")) 
			{
					SetVar(Vars,"item:on2:320x240",Token);
					AddToFormatsList(Vars,"on2:320x240 ");
			}
			else if (strstr(Token,"H264-848x480")) 
			{
					SetVar(Vars,"item:h264:848x480",Token);
					AddToFormatsList(Vars,"h264:848x480 ");
			}
			else if (strstr(Token,"H264-512x384")) 
			{
					SetVar(Vars,"item:h264:512x384",Token);
					AddToFormatsList(Vars,"h264:512x384 ");
			}
			else if (strstr(Token,"H264-1280x720")) 
			{
					SetVar(Vars,"item:h264:1280x720",Token);
					AddToFormatsList(Vars,"h264:1280x720 ");
			}

			ptr=GetToken(ptr,"||",&Token,0);
			}

DestroyString(Token);

return(ptr);
}


//----- Actually get data from server

void GetNextURL(int Type, char *Server, int Post, ListNode *Vars)
{
char *Tempstr=NULL, *Title=NULL, *ptr;

Title=CopyStr(Title,GetVar(Vars,"Title"));

switch (Type)
{
case TYPE_YOUTUBE_REF:
GrabMovie(GetVar(Vars,"ID"),TYPE_YOUTUBE);
break;

case TYPE_YOUTUBE:
	Tempstr=CopyStr(Tempstr,GetVar(Vars,"ID"));
  DownloadItem(Tempstr, Title,Post);
break;

case TYPE_BREAK_COM:
  Tempstr=SubstituteVarsInString(Tempstr,"$(ID)?$(EXTRA)",Vars,0);
  DownloadItem(Tempstr, Title,Post);
break;

case TYPE_EHOW:
 Tempstr=SubstituteVarsInString(Tempstr,"http://$(Server):$(Port)/embedvars.aspx?isEhow=true&show_related=true&id=$(ID)",Vars,0);
  DownloadPage(Tempstr,TYPE_EHOW_STAGE2, Title,Post);
break;

case TYPE_EHOW_STAGE2:
  DownloadItem(GetVar(Vars,"ID"), Title,Post);
break;


case TYPE_METACAFE:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)&$(METACAFE_OVER_18)",Vars,0);
  DownloadItem(Tempstr, Title,0);
break;


case TYPE_METACAFE_JS_REDIR:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)&$(METACAFE_OVER_18)",Vars,0);
 DownloadPage(Tempstr,TYPE_METACAFE,Title,0);
break;


case TYPE_METACAFE_FINAL:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)&$(METACAFE_OVER_18)",Vars,0);
  DownloadItem(Tempstr, Title,FALSE);
break;

case TYPE_DAILYMOTION:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)&allowFullScreen=true&allowScriptAccess=always&callback=player_proxy&lang=en&autoplay=1&uid=$(Extra)",Vars,0);
  DownloadItem(Tempstr, Title,FALSE);
break;

case TYPE_3GPDB:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
  DownloadPage(Tempstr,TYPE_3GPDB_STAGE2,Title,0);
break;

case TYPE_MOBANGO:
 Tempstr=SubstituteVarsInString(Tempstr,"http://media.mobango.com/$(ID)",Vars,0);
  DownloadItem(Tempstr, Title,Post);
break;

case TYPE_LIVELEAK:
 DownloadPage(GetVar(Vars,"ID"),TYPE_LIVELEAK_STAGE2,Title,FLAG_POST);
break;

case TYPE_VIDEOEMO:
 DownloadPage(GetVar(Vars,"ID"),TYPE_VIDEOEMO_STAGE2,Title,0);
break;



case TYPE_TED:
 Tempstr=SubstituteVarsInString(Tempstr,"http://video.ted.com/$(ID)",Vars,0);
  DownloadItem(Tempstr, Title,Post);
break;

case TYPE_MYVIDEO:
 Tempstr=SubstituteVarsInString(Tempstr,"$(MyVidURL)/$(ID).flv",Vars,0);
  DownloadItem(Tempstr, Title,Post);
break;

case TYPE_VBOX7:
	DoVBOX7(GetVar(Vars,"ID"), Title);
break;


case TYPE_VIMEO:
 Tempstr=SubstituteVarsInString(Tempstr,"http://$(Server):$(Port)/moogaloop/load/clip:$(ID)/embed?param_fullscreen=1&param_clip_id=$(ID)&param_show_byline=0&param_server=vimeo.com&param_color=cc6600&param_show_portrait=0&param_show_title=1",Vars,0);
 DownloadPage(Tempstr,TYPE_VIMEO_STAGE2,Title,FLAG_POST);
break;

case TYPE_VIMEO_STAGE2:
 Tempstr=SubstituteVarsInString(Tempstr,"http://$(Server):$(Port)/moogaloop/play/clip:$(ID)/$(Extra)/$(Extra2)/?q=sd&type=embed",Vars,0);
 DownloadItem(Tempstr,Title,FALSE);
break;


case TYPE_BERKELEY:
DoGetBerkeley(Vars);
break;

case TYPE_YALE:
 Tempstr=SubstituteVarsInString(Tempstr,"http://openmedia.yale.edu/cgi-bin/open_yale/media_downloader.cgi?file=$(ID)",Vars,0);
 DownloadItem(Tempstr,Title,FALSE);
break;

case TYPE_CLIPSHACK:
 Tempstr=SubstituteVarsInString(Tempstr,"http://$(Server):$(Port)/playerconfig.aspx?key=$(ID)",Vars,0);
  DownloadPage(Tempstr,TYPE_CLIPSHACK_STAGE2, Title,Post);
break;

case TYPE_CLIPSHACK_STAGE2:
 Tempstr=CopyStr(Tempstr,GetVar(Vars,"ID"));
  DownloadPage(Tempstr,TYPE_CLIPSHACK_STAGE3, Title,Post);
break;

case TYPE_CLIPSHACK_STAGE3:
 Tempstr=CopyStr(Tempstr,GetVar(Vars,"ID"));
 DownloadItem(Tempstr,Title,FALSE);
break;

case TYPE_GOOGLE_VIDEO:
case TYPE_ALMOST_KILLED:
case TYPE_FIVE_MIN:
case TYPE_IGN:
case TYPE_BLIP_TV:
case TYPE_3GPDB_STAGE2:
case TYPE_GENERIC:
case TYPE_CRAZYMOTION:
case TYPE_MYTOPCLIP:
case TYPE_REDBALCONY:
case TYPE_PRINCETON:
case TYPE_SDNHM:
case TYPE_UCHANNEL:
case TYPE_UCSDTV:
case TYPE_IZLESE:
case TYPE_IZLES:
case TYPE_IZLES_ORG:
case TYPE_REUTERS:
case TYPE_LIVELEAK_STAGE2:
case TYPE_CLIPFISH_DE:
case TYPE_ACADEMIC_EARTH:
case TYPE_PHOTOBUCKET:
case TYPE_VIDEOSFACEBOOK:
case TYPE_ALJAZEERA:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);

 DownloadItem(Tempstr, Title,Post);
break;

case TYPE_VIDEOEMO_STAGE2:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
 DownloadItem(Tempstr, Title,Post|FLAG_DL_RETRY);
break;



}

DestroyString(Tempstr);
DestroyString(Title);
}



//this function compares the video formats found on the page to the list of
//preferences expressed by the user with the '-f' flag, and contained in the
//global variable 'FormatPreference'
int SelectDownloadFormat(ListNode *Vars, char *FormatPreference, int WebsiteType)
{
ListNode *Curr;
char *ptr, *Tempstr=NULL, *Fmt=NULL, *Selected=NULL, *p_ItemFormat;
int RetVal=-1;

ptr=GetVar(Vars,"formats");
StripTrailingWhitespace(ptr);
ptr=GetToken(ptr,"\\S",&Tempstr,0);
if (StrLen(ptr))
{
fprintf(stderr, "Multiple Formats are available for this Movie:\n	%s\n\n",GetVar(Vars,"formats"));
}


	ptr=GetToken(FormatPreference,",",&Fmt,0);
	while (ptr)
	{
	if (Flags & (FLAG_DEBUG1 | FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"  %s ",Fmt);
		Curr=GetNextListItem(Vars);
		while (Curr)
		{
			if (
					(strncmp(Curr->Tag,"item:",5)==0) &&
					(StrLen((char *) Curr->Item))
				)
				{
					p_ItemFormat=Curr->Tag+5;

				if (
							 (! StrLen(Fmt)) ||
							 (strncmp(p_ItemFormat,Fmt,StrLen(Fmt))==0)
					)
				{
					if (Flags & (FLAG_DEBUG1 | FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"... YES!\n\n");

					//Only use it if 'RetVal' is set
					if (RetVal==-1)
					{
						SetVar(Vars,"ID",(char *) Curr->Item);
						Selected=CopyStr(Selected,Fmt);

						if (strcmp(Fmt,"yt-flv")==0) RetVal=TYPE_YOUTUBE_REF;
						else RetVal=WebsiteType;
					}

					break;
				}
				}
				Curr=GetNextListItem(Curr);
			}
			if (Flags & (FLAG_DEBUG1 | FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"... no\n",Fmt);
			ptr=GetToken(ptr,",",&Fmt,0);
	}

if (RetVal!=-1) fprintf(stderr,"Selected format %s\n",Selected);
else fprintf(stderr,"No suitable download format found from '%s'\n\n",FormatPreference);


DestroyString(Selected);
DestroyString(Tempstr);
DestroyString(Fmt);
return(RetVal);
}



// This is the main function that 'screen scrapes' a webpage looking for 
// information that it can use to get a video
void ExtractItemInfo(STREAM *S, int Type, char *Server, int Port, char *Title, int Post)
{
char *Tempstr=NULL, *Token=NULL, *VarName=NULL;
ListNode *Vars=NULL;
char *ptr, *ptr2;
int MediaCount=0, i;
//Standard file types list breaks berkely downloads
char *BerkeleyFileTypes[]={".mp4",".m4a",".mp3",NULL};

#define GENERIC_TITLE_START "<title>"
#define GENERIC_TITLE_END "</title>"



Vars=CreateEmptyList();
SetVar(Vars,"Server",Server);
if (Port==0) Port=DefaultPort;
Tempstr=FormatStr(Tempstr,"%d",Port);
SetVar(Vars,"Port",Tempstr);
SetVar(Vars,"Title",Title);
SetVar(Vars,"SelectedItem",ItemSelectionArg);

	if (Flags & (FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"\n------- DOWNLOADING DOCUMENT ------\n");
Tempstr=STREAMReadLine(Tempstr, S);
while (Tempstr)
{
	StripTrailingWhitespace(Tempstr);
	StripLeadingWhitespace(Tempstr);
	
	if (Flags & (FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"%s\n",Tempstr);
switch (Type)
{

case TYPE_YOUTUBE:
//#define YOUTUBE_PTR "new SWFObject(\"/player2.swf?"

//#define YOUTUBE_PTR "var swfArgs = {"
#define YOUTUBE_DIV "video_id\":"
#define YOUTUBE_DIV2 "fmt_url_map\": \""
#define YOUTUBE_TITLE "<meta name=\"title\" content=\""
#define YOUTUBE_TITLE2 "'VIDEO_TITLE': '"

	if (strstr(Tempstr,YOUTUBE_TITLE))
	{
		GenericExtractFromLine(Tempstr, "Title",YOUTUBE_TITLE, "\">", Vars,EXTRACT_DEQUOTE);
	}

	if (strstr(Tempstr,YOUTUBE_TITLE2))
	{
		GenericExtractFromLine(Tempstr, "Title",YOUTUBE_TITLE2, "'", Vars,EXTRACT_DEQUOTE);
	}


/*
	if (strstr(Tempstr,YOUTUBE_DIV))
	{
		GenericExtractFromLine(Tempstr, "item:flv",YOUTUBE_DIV, ",", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		GenericExtractFromLine(Tempstr, "Extra","\"t\": ",",", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
  	Tempstr=SubstituteVarsInString(Tempstr,"http://$(Server):$(Port)/get_video?video_id=$(ID)&t=$(Extra)",Vars,0);
		SetVar(Vars,"item:flv",Tempstr);
	}
	*/

	if (strstr(Tempstr,YOUTUBE_DIV2))
	{
		if (StrLen(GetVar(Vars,"item:flv"))==0) 
		{
			GenericExtractFromLine(Tempstr, "yt:url_fmt",YOUTUBE_DIV2, "\"", Vars,0);
			Tempstr=CopyStr(Tempstr,GetVar(Vars,"yt:url_fmt"));
			DecodeYouTubeFormats(Tempstr,Vars);
		}
	}

break;

case TYPE_METACAFE:
#define METACAFE_ITEM "var itemID = "
#define METACAFE_ITEM2 "so.addParam(\"flashvars\", \'itemID="
//#define METACAFE_ITEM_FINAL "<item "
#define METACAFE_ITEM_FINAL ");"
#define METACAFE_EXTRA "var LEID = "
#define METACAFE_OVER_18 "allowAdultContent=1&submit=Continue+-+I%27m+over+18"
#define METACAFE_JS_REDIR "<script type=\"text/javascript\">document.location = "

	if (strncmp(Tempstr,METACAFE_JS_REDIR,StrLen(METACAFE_JS_REDIR))==0)
	{
		ptr=GetToken(Tempstr+StrLen(METACAFE_JS_REDIR),";</script>",&Token,0);
		StripQuotes(Token);
		SetVar(Vars,"item:flv",Token);
//		GenericExtractFromLine(Tempstr, "item:flv",METACAFE_JS_REDIR,";</script>", Vars);
		Type=TYPE_METACAFE_JS_REDIR;
	}
	else
	{
	if (strstr(Tempstr,GENERIC_TITLE_START))
	{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END, Vars,EXTRACT_DEQUOTE);
	}


#define METACAFE_MEDIA_URL "mediaURL="
	if (
				(strstr(Tempstr,METACAFE_MEDIA_URL))
		)
			
	{
		Token=ExtractMetacafeMediaURL(Token,Tempstr,METACAFE_MEDIA_URL,"&");
		SetVar(Vars,"item:flv",Token);
	}
	}
break;

case TYPE_METACAFE_FINAL:
	if (strstr(Tempstr," url="))
	{
		GenericExtractFromLine(Tempstr, "item:flv"," url=","\\S", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
break;


case TYPE_GOOGLE_VIDEO:
#define GOOGLE_VID_ITEM "If the download does not start automatically, right-click <a href="


	if (strstr(Tempstr,GENERIC_TITLE_START))
	{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END, Vars,EXTRACT_DEQUOTE);
	}

	if (strstr(Tempstr,GOOGLE_VID_ITEM))
	{
		GenericExtractFromLine(Tempstr, "item:flv",GOOGLE_VID_ITEM,">",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
break;

case TYPE_BLIP_TV:
#define BLIP_TV_ITEM "player.setPrimaryMediaUrl(\""
	ptr=strstr(Tempstr,BLIP_TV_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",BLIP_TV_ITEM,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
break;

case TYPE_3GPDB:
#define _3GPDB_FRONTPAGE "Download This Video Mp4 3GP and Flv Format"
#define _3GPDB_MP4 "Download MP4 of Video</a>"
#define _3GPDB_3GP "Download 3GP of Video</a>"

	ptr=strstr(Tempstr,_3GPDB_FRONTPAGE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "ID","href=\"","\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

//Fall through

case TYPE_3GPDB_STAGE2:
	ptr=strstr(Tempstr,_3GPDB_MP4);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:mp4","href=\"","\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	Type=TYPE_3GPDB_STAGE2;
		AddToFormatsList(Vars,"mp4 ");
	}

	ptr=strstr(Tempstr,_3GPDB_3GP);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:3gp","href=\"","\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		Type=TYPE_3GPDB_STAGE2;
		AddToFormatsList(Vars,"3gp ");
	}


	ptr=strstr(Tempstr,"<title>");
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title","<title>","</title>",Vars,EXTRACT_DEQUOTE);
	}

break;


case TYPE_BREAK_COM:
#define BREAK_ITEM "sGlobalFileName='"
#define BREAK_EXTRA "flashVars.icon = \""
#define BREAK_WMV "+sGlobalContentFilePath+'/'+sGlobalFileName+'.wmv"
#define BREAK_FLV "+sGlobalContentFilePath+'/'+sGlobalFileName+'.flv"
#define BREAK_TITLE "id=\"vid_title\" content=\""


	ptr=strstr(Tempstr,BREAK_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",BREAK_ITEM,"'",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,"item:flv");
		if (ptr) ptr=CatStr(ptr,".flv");
	}

	ptr=strstr(Tempstr,BREAK_EXTRA);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Extra",BREAK_EXTRA,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
	
	ptr=strstr(Tempstr,BREAK_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",BREAK_TITLE,"\"",Vars,0);
	}


break;



case TYPE_EHOW:
#define EHOW_ITEM "flashvars=&quot;id="
#define EHOW_ITEM_END "&"
#define EHOW_TITLE "<h1 class=\"Heading1a\">"
#define EHOW_TITLE_END "</h1>"

	ptr=strstr(Tempstr,EHOW_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",EHOW_ITEM,EHOW_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,EHOW_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",EHOW_TITLE,EHOW_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}
break;

case TYPE_EHOW_STAGE2:
#define EHOW_STAGE2_ITEM "source="
#define EHOW_STAGE2_ITEM_END "&"

	ptr=strstr(Tempstr,EHOW_STAGE2_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",EHOW_STAGE2_ITEM,EHOW_STAGE2_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,EHOW_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",EHOW_TITLE,EHOW_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}
break;



case TYPE_VIMEO:
#define VIMEO_ITEM "clip_id="
#define VIMEO_TITLE "<meta name=\"title\" content=\""
#define VIMEO_TITLE_END "\""

	ptr=strstr(Tempstr,VIMEO_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "ID",VIMEO_ITEM,"&",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,VIMEO_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",VIMEO_TITLE,VIMEO_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}	
break;


case TYPE_VIMEO_STAGE2:
#define VIMEO_STAGE2_ITEM "<nodeId>"
#define VIMEO_STAGE2_ITEM_END "</nodeId>"
#define VIMEO_STAGE2_REQ_SIG "<request_signature>"
#define VIMEO_STAGE2_REQ_SIG_END "</request_signature>"
#define VIMEO_STAGE2_REQ_SIG_EX "<request_signature_expires>"
#define VIMEO_STAGE2_REQ_SIG_EX_END "</request_signature_expires>"

	if (StrLen(GetVar(Vars,"item:flv"))==0)
	{
	ptr=strstr(Tempstr,VIMEO_STAGE2_ITEM);
	if (ptr) GenericExtractFromLine(Tempstr, "item:flv",VIMEO_STAGE2_ITEM,VIMEO_STAGE2_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,VIMEO_STAGE2_REQ_SIG);
	if (ptr)
	{
	if (ptr) GenericExtractFromLine(Tempstr, "Extra",VIMEO_STAGE2_REQ_SIG,VIMEO_STAGE2_REQ_SIG_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,VIMEO_STAGE2_REQ_SIG_EX);
	if (ptr)
	{
	if (ptr) GenericExtractFromLine(Tempstr, "Extra2",VIMEO_STAGE2_REQ_SIG_EX,VIMEO_STAGE2_REQ_SIG_EX_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
break;


case TYPE_ALMOST_KILLED:
#define AK_ITEM "file=http://"

	ptr=strstr(Tempstr,AK_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",AK_ITEM,"&",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

break;

case TYPE_FIVE_MIN:
#define FIVE_MIN_ITEM "videoUrl="
#define FIVE_MIN_TITLE "<h1 class=\"videoTitle\">"
#define FIVE_MIN_TITLE_END "</h1>"

	ptr=strstr(Tempstr,FIVE_MIN_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",FIVE_MIN_ITEM,"&",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,FIVE_MIN_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",FIVE_MIN_TITLE,FIVE_MIN_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}
break;

case TYPE_IGN:
#define IGN_ITEM "downloadURL=http://"
#define IGN_ITEM_END "[/ignvideo]"
#define IGN_ITEM_2 "mediaIdHash[\""
#define IGN_ITEM_2_END "\"]"
#define IGN_ITEM_LOW "vp_lowResFlash = '"
#define IGN_ITEM_HIGH "vp_hiResFlash = '"

	ptr=strstr(Tempstr,IGN_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",IGN_ITEM,IGN_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,IGN_ITEM_2);
	if (ptr && (StrLen(GetVar(Vars,"item:flv"))==0))
	{
		GenericExtractFromLine(Tempstr, "item:flv",IGN_ITEM_2,IGN_ITEM_2_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}


	ptr=strstr(Tempstr,IGN_ITEM_LOW);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv:lowq",IGN_ITEM_LOW,"'",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,IGN_ITEM_HIGH);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv:highq",IGN_ITEM_HIGH,"'",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
 break;


case TYPE_DAILYMOTION:
#define DAILYMOTION_ITEM "addVariable(\"video\", \""
#define DAILYMOTION_ITEM_END "&"
#define DAILYMOTION_TITLE_START "<h1 class=\"dmco_title\"><span class=\"title\" title=\""
#define DAILYMOTION_TITLE_END "\""
#define DAILYMOTION_EXTRA "addVariable(\"uid\", \""


	if (strstr(Tempstr,DAILYMOTION_TITLE_START))
	{
		GenericExtractFromLine(Tempstr, "Title",DAILYMOTION_TITLE_START,DAILYMOTION_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}


	if (strstr(Tempstr,DAILYMOTION_ITEM))
	{
		GenericExtractFromLine(Tempstr, "DailyMotionItems",DAILYMOTION_ITEM,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,"DailyMotionItems");
		DecodeDailyMotionFormats(ptr,Vars);
	}

	if (strstr(Tempstr,DAILYMOTION_EXTRA))
	{
		GenericExtractFromLine(Tempstr, "Extra",DAILYMOTION_EXTRA,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
break;


case TYPE_VBOX7:
#define VBOX7_ITEM "so.addVariable(\"vid\", \""
#define VBOX7_ITEM_END "\");"

if (strstr(Tempstr,VBOX7_ITEM))
{
		GenericExtractFromLine(Tempstr, "item:flv",VBOX7_ITEM,VBOX7_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}

if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}
break;

case TYPE_TED:
#define TED_HIGH_ITEM_START "hs:\""
#define TED_MED_ITEM_START "ms:\""
#define TED_LOW_ITEM_START "ls:\""
#define TED_ITEM_END "\""
if (strstr(Tempstr,TED_HIGH_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv:highq",TED_HIGH_ITEM_START,TED_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}

if (strstr(Tempstr,TED_MED_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv:medq",TED_MED_ITEM_START,TED_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		GenericExtractFromLine(Tempstr, "item:flv",TED_MED_ITEM_START,TED_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}

if (strstr(Tempstr,TED_LOW_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv:lowq",TED_LOW_ITEM_START,TED_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}

if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}
break;

case TYPE_MYVIDEO:
#define MYVIDEO_URL_START "link rel='image_src' href='"
#define MYVIDEO_URL_END "/thumbs"
#define MYVIDEO_VIDID_END "_"

if (strstr(Tempstr,MYVIDEO_URL_START))
{
		ptr=GenericExtractFromLine(Tempstr, "MyVidURL",MYVIDEO_URL_START,MYVIDEO_URL_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GenericExtractFromLine(ptr, "item:flv","/",MYVIDEO_VIDID_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}
break;



case TYPE_BERKELEY:
#define BERKELEY_PTR "PIPPlay"
#define BERKELEY_ITEM_START "file="
#define BERKELEY_ITEM_END "&"
#define BERKELEY_TITLE_START "caption="
#define BERKELEY_TITLE_END "')"

if (strstr(Tempstr,BERKELEY_PTR))
{
		for (i=0; BerkeleyFileTypes[i] !=NULL; i++)
		{
			//a messy check. 'Format preferences' contains format specifiers like 
			//'flv' or 'mp4'. The file types come as '.flv', so we have to jump 
			//one char ahead before checking that they are in the users specified 
			//format list
			if (strstr(FormatPreference,BerkeleyFileTypes[i] +1) && strstr(Tempstr,BerkeleyFileTypes[i]))
			{
			VarName=FormatStr(VarName,"ID:%d",MediaCount);
			GenericExtractFromLine(Tempstr, VarName,BERKELEY_ITEM_START,BerkeleyFileTypes[i],Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
			Token=CopyStr(Token,GetVar(Vars,VarName));
			if (StrLen(Token) > 0) 
			{
				Token=CatStr(Token,BerkeleyFileTypes[i]);
				SetVar(Vars,VarName,Token);
				MediaCount++;
				if (MediaCount==1) SetVar(Vars,"ID",Token);
				Token=FormatStr(Token,"Title:%d",MediaCount);
				GenericExtractFromLine(Tempstr, Token, BERKELEY_TITLE_START,BERKELEY_TITLE_END,Vars,EXTRACT_DEQUOTE);
			}
			}
		}

		Token=FormatStr(Token,"%d",MediaCount);
		SetVar(Vars,"MediaCount",Token);
}
break;

case TYPE_YALE:
#define YALE_ITEM_START "openVideoWindowMEDIUM('"
#define YALE_ITEM_END "')"

if (strstr(Tempstr,YALE_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "ID",YALE_ITEM_START,YALE_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;

case TYPE_REDBALCONY:
#define REDBALCONY_ITEM_START "send_mobile_content?vid="
#define REDBALCONY_ITEM_END ".flv"

if (strstr(Tempstr,REDBALCONY_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv",REDBALCONY_ITEM_START,REDBALCONY_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		Token=CopyStr(Token,GetVar(Vars,"item:flv"));
		Token=CatStr(Token,".flv");
		SetVar(Vars,"item:flv",Token);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;

case TYPE_MOBANGO:
#define MOBANGO_ITEM_START "so.addVariable('flvfile','"
#define MOBANGO_ITEM_END "'"

#define MOBANGO_TITLE_START "name=\"flname\" value=\""
#define MOBANGO_TITLE_END "\""

if (strstr(Tempstr,MOBANGO_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv",MOBANGO_ITEM_START,MOBANGO_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}


if (strstr(Tempstr,MOBANGO_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",MOBANGO_TITLE_START,MOBANGO_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;


#define IZLESE_ITEM_START "file=http"
#define IZLESE_ITEM_END "&cid="
#define IZLESE_TITLE_START "<h1>"
#define IZLESE_TITLE_END "</h1>"
#define IZLESE_TITLE2_START "<title>"
#define IZLESE_TITLE2_END "</title>"

case TYPE_IZLESE:
case TYPE_IZLES:
case TYPE_IZLES_ORG:


if (strstr(Tempstr,IZLESE_ITEM_START) && strstr(Tempstr,IZLESE_ITEM_END))
{
		GenericExtractFromLine(Tempstr, "item:flv",IZLESE_ITEM_START,IZLESE_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
ptr=GetVar(Vars,"item:flv");
//put 'http' back on
Tempstr=MCopyStr(Tempstr,"http",ptr,NULL);
if (strstr(ptr,"youtube"))
{
		SetVar(Vars,"item:flv","");
		SetVar(Vars,"item:yt-flv",Tempstr);
}
else SetVar(Vars,"item:flv",Tempstr);
}


if (strstr(Tempstr,IZLESE_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",IZLESE_TITLE_START,IZLESE_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

if (strstr(Tempstr,IZLESE_TITLE2_START))
{
		GenericExtractFromLine(Tempstr, "Title",IZLESE_TITLE2_START,IZLESE_TITLE2_END,Vars,EXTRACT_DEQUOTE);
}


break;


case TYPE_CLIPSHACK:
#define CLIPSHACK_ITEM_START "playerconfig.aspx?key="
#define CLIPSHACK_TITLE_START "<span id=\"lblTitle\">"
#define CLIPSHACK_TITLE_END "</span>"

if (strstr(Tempstr,CLIPSHACK_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "ID",CLIPSHACK_ITEM_START,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}


if (strstr(Tempstr,CLIPSHACK_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",CLIPSHACK_TITLE_START,CLIPSHACK_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;

case TYPE_CLIPSHACK_STAGE2:
#define CLIPSHACK_STAGE2_START "<file>"
#define CLIPSHACK_STAGE2_END "</file>"
if (strstr(Tempstr,CLIPSHACK_STAGE2_START))
{
		GenericExtractFromLine(Tempstr, "ID",CLIPSHACK_STAGE2_START,CLIPSHACK_STAGE2_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}
break;

case TYPE_CLIPSHACK_STAGE3:
#define CLIPSHACK_STAGE3_START "<location>"
#define CLIPSHACK_STAGE3_END "</location>"
if (strstr(Tempstr,CLIPSHACK_STAGE3_START))
{
		GenericExtractFromLine(Tempstr, "item:flv",CLIPSHACK_STAGE3_START,CLIPSHACK_STAGE3_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}
break;


#define REUTERS_FLV_ITEM_START "'flv':"
#define REUTERS_MP4_ITEM_START "'mpeg':"
#define REUTERS_TITLE "'headline':"

case TYPE_REUTERS:
	ptr=strstr(Tempstr,REUTERS_FLV_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",REUTERS_FLV_ITEM_START,",",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
		AddToFormatsList(Vars,"flv ");
	}

	ptr=strstr(Tempstr,REUTERS_MP4_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:mp4",REUTERS_MP4_ITEM_START,",",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
		AddToFormatsList(Vars,"mp4 ");
	}


	ptr=strstr(Tempstr,REUTERS_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",REUTERS_TITLE,",",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
	}
break;


#define AE_FLV_ITEM_START "flashVars.flvURL = "
#define AE_YT_ITEM_START "flashVars.ytID = "

case TYPE_ACADEMIC_EARTH:
	ptr=strstr(Tempstr,AE_FLV_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",AE_FLV_ITEM_START,";",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
		AddToFormatsList(Vars,"flv ");
	}

	ptr=strstr(Tempstr,AE_YT_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:yt-flv",AE_YT_ITEM_START,";",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
		Tempstr=MCopyStr(Tempstr,"http://youtube.com/watch?v=",GetVar(Vars,"item:yt-flv"),NULL);
		SetVar(Vars,"item:yt-flv",Tempstr);
		AddToFormatsList(Vars,"yt-flv ");
	}

break;


#define PHOTOBUCKET_START "flashvars=&quot;file="
#define PHOTOBUCKET_END "&quot;"

case TYPE_PHOTOBUCKET:
	ptr=strstr(Tempstr,PHOTOBUCKET_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:mp4",PHOTOBUCKET_START,PHOTOBUCKET_END,Vars, EXTRACT_DEQUOTE);
		AddToFormatsList(Vars,"mp4 ");
	}
break;


#define CLIPFISH_ITEM_START "&url="
case TYPE_CLIPFISH_DE:
	ptr=strstr(Tempstr,CLIPFISH_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",CLIPFISH_ITEM_START,"&",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
	}
break;

#define LIVELEAK_ITEM_START "'config','"
#define LIVELEAK_ITEM_END "');"
case TYPE_LIVELEAK:
	ptr=strstr(Tempstr,LIVELEAK_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "ID",LIVELEAK_ITEM_START,LIVELEAK_ITEM_END,Vars,EXTRACT_DEQUOTE);
	}
break;


#define LIVELEAK2_ITEM_START "<file>"
#define LIVELEAK2_ITEM_END "'</"
case TYPE_LIVELEAK_STAGE2:
	ptr=strstr(Tempstr,LIVELEAK_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",LIVELEAK2_ITEM_START,LIVELEAK2_ITEM_END,Vars,EXTRACT_DEQUOTE);
	}
break;

#define VIDEOEMO_ITEM_START "href=\"http://www.vidoevo.com/videodownload.php?e="
#define VIDEOEMO_ITEM_END "\""

case TYPE_VIDEOEMO:
	if (strstr(Tempstr,VIDEOEMO_ITEM_START))
	{
		GenericExtractFromLine(Tempstr, "ID","href=\"",VIDEOEMO_ITEM_END,Vars,EXTRACT_DEQUOTE);
	}

//Fall through, we might be on the second page already!
//break;

#define VIDEOEMO2_ITEM_START "Format of Video"
#define VIDEOEMO2_TITLE_START "<h2 class=\"title\">"
#define VIDEOEMO2_TITLE_END "</h2>"

case TYPE_VIDEOEMO_STAGE2:
	if (strstr(Tempstr,VIDEOEMO2_ITEM_START))
	{
		if (strstr(Tempstr,"flv")) GenericExtractFromLine(Tempstr, "item:flv","href=",">",Vars,EXTRACT_DEQUOTE);
		if (strstr(Tempstr,"mp4")) GenericExtractFromLine(Tempstr, "item:mp4","href=",">",Vars,EXTRACT_DEQUOTE);
		if (strstr(Tempstr,"3gp")) GenericExtractFromLine(Tempstr, "item:3gp","href=",">",Vars,EXTRACT_DEQUOTE);

		//if we found a download link, then we are at stage2 already!
		Type=TYPE_VIDEOEMO_STAGE2;
	}

if (strstr(Tempstr,VIDEOEMO2_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",VIDEOEMO2_TITLE_START,VIDEOEMO2_TITLE_END,Vars,EXTRACT_DEQUOTE);
}


break;

#define VIDEOSFACEBOOK_ITEM_START "<link rel=\"video_src\" href=\"http://www.videosfacebook.net/js/jw/player.swf?file="
#define VIDEOSFACEBOOK_ITEM_END "\""
#define VIDEOSFACEBOOK_TITLE_START "<meta property=\"og:title\" content=\""
#define VIDEOSFACEBOOK_TITLE_END "\""

case TYPE_VIDEOSFACEBOOK:
	if (strstr(Tempstr,VIDEOSFACEBOOK_ITEM_START))
	{
		GenericExtractFromLine(Tempstr, "ID",VIDEOSFACEBOOK_ITEM_START,VIDEOSFACEBOOK_ITEM_END,Vars,EXTRACT_DEQUOTE);
	}

	if (strstr(Tempstr,VIDEOSFACEBOOK_TITLE_START))
	{
		GenericExtractFromLine(Tempstr, "Title",VIDEOSFACEBOOK_TITLE_START,VIDEOSFACEBOOK_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}


break;



case TYPE_CRAZYMOTION:
case TYPE_MYTOPCLIP:
case TYPE_SDNHM:
case TYPE_UCHANNEL:
case TYPE_PRINCETON:
case TYPE_UCSDTV:
case TYPE_GENERIC:
case TYPE_ALJAZEERA:

//some site are actually just frontends to youtube
#define YOUTUBE_REFERENCE1 "http://www.youtube.com/watch?v="
#define YOUTUBE_REFERENCE2 "http://www.youtube.com/v/"
if (strstr(Tempstr,YOUTUBE_REFERENCE1))
{
		GenericExtractFromLine(Tempstr, "yt-tmp",YOUTUBE_REFERENCE1,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,"yt-tmp");
		if (StrLen(ptr)) Token=MCopyStr(Token,YOUTUBE_REFERENCE1,ptr,NULL);
		SetVar(Vars,"item:yt-flv",Token);
		Token=CopyStr(Token,""); //So as later stages of this process don't
														 //pick up on it
}

if (strstr(Tempstr,YOUTUBE_REFERENCE2))
{
		GenericExtractFromLine(Tempstr, "yt-tmp",YOUTUBE_REFERENCE2,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,"yt-tmp");
		if (StrLen(ptr)) Token=MCopyStr(Token,YOUTUBE_REFERENCE2,ptr,NULL);
		SetVar(Vars,"item:yt-flv",Token);
		Token=CopyStr(Token,""); //So as later stages of this process don't
														 //pick up on it
}


for (i=0; FileTypes[i] !=NULL; i++)
{
	if (strstr(Tempstr,FileTypes[i]))
	{
		VarName=MCopyStr(VarName,"item:",FileTypes[i]+1,NULL);
		GenericExtractFromLine(Tempstr, VarName,"http://",FileTypes[i],Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,VarName);
		if (StrLen(ptr)) Token=MCopyStr(Token,"http://",ptr,NULL);
		else
		{
			GenericExtractFromLine(Tempstr, VarName,"href=",FileTypes[i],Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
			Token=CopyStr(Token,GetVar(Vars,VarName));
			if (StrLen(Token) && (strncasecmp(Token,"http://",7) !=0))
			{
				Token=MCopyStr(Token,"http://",GetVar(Vars,"Server"),"/",NULL);
				Token=CatStr(Token,GetVar(Vars,VarName));
			}
		}
		
		if (StrLen(Token)) 
		{
			Token=CatStr(Token,FileTypes[i]);
			SetVar(Vars,VarName,Token);
		}
	}
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;


}

  Tempstr=STREAMReadLine(Tempstr,S);
}

	if (Flags & (FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"\n------- END DOCUMENT ------\n\n");


if ((Type != TYPE_BERKELEY) && StrLen(GetVar(Vars,"ID"))==0)
{
 Type=SelectDownloadFormat(Vars,FormatPreference,Type);
}


ptr=GetVar(Vars,"ID");
if (ptr) GetNextURL(Type, Server, Post, Vars);
else if (! (Flags & FLAG_QUIET))
{
	fprintf(stderr,"FAILED TO GET DOCUMENT REFERENCE!\n");
	fprintf(stderr,"Maybe you need to change the format prefernces, or else the url you supplied is wrong\n");
	fprintf(stderr,"Format Preference: %s\n\n",FormatPreference);
}

DestroyList(Vars,DestroyString);

DestroyString(VarName);
DestroyString(Token);
DestroyString(Tempstr);
}


void DownloadPage(char *Path, int Type, char *Title, int Post)
{
char *ptr, *Server=NULL, *Doc=NULL, *Args=NULL;
char *Tempstr=NULL, *Token=NULL;
int Port;
STREAM *S;

if (Flags & (FLAG_DEBUG1 | FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"Next URL: %s\n",Path);
ptr=HTTPParseURL(Path,&Tempstr,&Server,&Port,NULL,NULL);
Doc=CopyStr(Doc,ptr);
if (Port==0) Port=DefaultPort;

S=ConnectAndSendHeaders(Server, Doc, Port, Post,0);

if (S)
{
  ExtractItemInfo(S, Type, Server, Port, Title, Post);
}
else if (! (Flags & FLAG_QUIET)) fprintf(stderr,"ERROR: failed to Connect to %s\n",Server);

DestroyString(Tempstr);
DestroyString(Token);
DestroyString(Server);
DestroyString(Doc);
}


void PrintUsage()
{
int i;

fprintf(stderr,"\nMovgrab: version %s\n",Version);
fprintf(stderr,"Author: Colum Paget\n");
fprintf(stderr,"Email: colums.projects@gmail.com\n");
fprintf(stderr,"Blogs: \n");
fprintf(stderr,"	tech: http://idratherhack.blogspot.com \n");
fprintf(stderr,"	rants: http://thesingularitysucks.blogspot.com \n");
fprintf(stderr,"\n");
fprintf(stderr,"Usage: movgrab [-t <type>] [-p http://username:password@x.x.x.x:80 ] [-b] [-x] [-q] [-f <format list>] [-v] [-o <output file> url\n");
fprintf(stderr,"\n'-v'		increases verbosity/debug level\n");
fprintf(stderr,"'-v -v'		prints out all webpages encountered\n");
fprintf(stderr,"'-v -v -v'	maximum debugging\n");
fprintf(stderr,"'-T'			Test mode, don't do final download\n");
fprintf(stderr,"'-a'		Authentication info in Username:Password format.\n");
fprintf(stderr,"'-q'		QUIET. No progress/informative output.\n");
fprintf(stderr,"'-b'		Background. Fork into background and nohup\n");
fprintf(stderr,"'-p'		address of HTTP proxy server in URL format.\n");
fprintf(stderr,"'-w'		Wait for addresses to be entered on stdin.\n");
fprintf(stderr,"'-t'		specifies website type.\n");
fprintf(stderr,"'-f'		specifies preferred video/audio formats for sites that offer more than one\n");
fprintf(stderr,"		example: flv:640:480,flv,mp4,mp3\n");
fprintf(stderr,"		specifying 'none' with debug turned on will get a list of what the site offers\n");
fprintf(stderr,"'-o'		specifies output file ( '-' for stdout)\n");
fprintf(stderr,"'-x'		try to avoid 'family filter' on some sites\n");
fprintf(stderr,"'-n'		For pages with multiple movies (not movie formats, but movies) specifes item selection for download. Argument has the form:\n");
fprintf(stderr,"			-n all		Download all\n");
fprintf(stderr,"			-n 2		Download 2nd item\n");
fprintf(stderr,"			-n 2-5		Download 2nd to 5th item\n");
fprintf(stderr,"			-n 2-		Download from 2nd item onwards\n");
fprintf(stderr,"			-n -8		Download from 1st to 8th item\n");

fprintf(stderr,"\nThe '-t' argument is optional, Movgrab will try to guess the download type from the provided url.\n\n");

fprintf(stderr,"The url should be that of the page that the movie would normally appear on. Movgrab examines the page for embed tags and other information that would normally cause the movie to play on that page.\n");

fprintf(stderr,"\nDownload types are:\n");
for (i=1; DownloadTypes[i] !=NULL; i++) fprintf(stderr,"%- 20s %s\n",DownloadTypes[i],DownloadNames[i]);

fprintf(stderr,"\nIf a website is not in the list, try 'movgrab -t generic <url>'\n");
fprintf(stderr,"\nFeel free to email me and tell me if you've used this software!\n");

//If you want to watch quite a good youtube movie, try 'SPIN', "movgrab http://www.youtube.com/watch?v=oP59tQf_njc"
}


int ParseType(char *Type)
{
int i, result=TYPE_NONE;

for (i=0; DownloadTypes[i] !=NULL; i++)
{
	if (strcasecmp(Type,DownloadTypes[i])==0) result=i;
}
return(result);
}


void ParseCommandLine(int argc, char *argv[], ListNode *DL_List, int *OverrideType)
{
int i, DebugLevel=0;
char *ptr;

ProgName=CopyStr(ProgName,argv[0]);
CmdLine=argv[0];

for (i=1; i < argc; i++)
{
	if (strcmp(argv[i],"-p")==0)
	{
		HTTPSetProxy(argv[++i]);
	}
	else if (strcmp(argv[i],"-a")==0)
	{
			ptr=GetToken(argv[++i],":",&Username,NULL);
			ptr=GetToken(ptr,":",&Password,NULL);
	}
	else if (strcmp(argv[i],"-v")==0)
	{
		DebugLevel++;
	}
	else if (strcmp(argv[i],"-o")==0)
	{
		SaveFilePath=CopyStr(SaveFilePath,argv[++i]);
	}
	else if (strcmp(argv[i],"-n")==0)
	{
		ItemSelectionArg=CopyStr(ItemSelectionArg,argv[++i]);
	}
	else if (strcmp(argv[i],"-t")==0) *OverrideType=ParseType(argv[++i]);
	else if (strcmp(argv[i],"-f")==0) FormatPreference=CopyStr(FormatPreference,argv[++i]);
	else if (strcmp(argv[i],"-q")==0) Flags |= FLAG_QUIET;
	else if (strcmp(argv[i],"-b")==0) Flags |= FLAG_BACKGROUND;
	else if (strcmp(argv[i],"-x")==0) Flags |= FLAG_PORN;
	else if (strcmp(argv[i],"-T")==0) Flags |= FLAG_TEST;
	else if (strcmp(argv[i],"-?")==0) Flags |= FLAG_PRINT_USAGE;
	else if (strcmp(argv[i],"--help")==0) Flags |= FLAG_PRINT_USAGE;
	else
	{
		AddItemToList(DL_List,CopyStr(NULL,argv[i]));
	}	

}

if (Flags & FLAG_BACKGROUND) 
{
Flags |= FLAG_QUIET;
demonize();
}

if (Flags & FLAG_QUIET) DebugLevel=0;

if (DebugLevel==1) Flags |= FLAG_DEBUG1;
if (DebugLevel==2) Flags |= FLAG_DEBUG2;
if (DebugLevel > 2) Flags |= FLAG_DEBUG3;
}


int IdentifyServiceType(char *Server)
{
int Type=0;

if (
	(strstr(Server,"youtube")) 
   ) Type=TYPE_YOUTUBE;

if (strcmp(Server,"www.metacafe.com")==0)
{
 Type=TYPE_METACAFE;
}
else if (strncmp(Server,"video.google.",13)==0)
{
 Type=TYPE_GOOGLE_VIDEO;
}
else if (strcmp(Server,"www.break.com")==0)
{
 Type=TYPE_BREAK_COM;
}
else if (strcmp(Server,"www.3gpdb.com")==0)
{
 Type=TYPE_3GPDB;
}
else if (strcmp(Server,"www.ehow.com")==0)
{
 Type=TYPE_EHOW;
}
else if (strcmp(Server,"www.almostkilled.com")==0)
{
 Type=TYPE_ALMOST_KILLED;
}
else if (strcmp(Server,"www.dailymotion.com")==0)
{
 Type=TYPE_DAILYMOTION;
}
else if (strcmp(Server,"www.5min.com")==0)
{
 Type=TYPE_FIVE_MIN;
}

else if (strcmp(Server,"www.ted.com")==0)
{
 Type=TYPE_TED;
}
else if (strcmp(Server,"webcast.berkeley.edu")==0)
{
 Type=TYPE_BERKELEY;
}
else if (strcmp(Server,"oyc.yale.edu")==0)
{
 Type=TYPE_YALE;
}
else if (strcmp(Server,"www.sdnhm.org")==0)
{
 Type=TYPE_SDNHM;
}
else if (strcmp(Server,"uc.princeton.edu")==0)
{
 Type=TYPE_UCHANNEL;
}
else if (strcmp(Server,"www.princeton.edu")==0)
{
 Type=TYPE_PRINCETON;
}
else if (strcmp(Server,"www.academicearth.org")==0)
{
 Type=TYPE_ACADEMIC_EARTH;
}
else if (strstr(Server,"photobucket.com"))
{
 Type=TYPE_PHOTOBUCKET;
}
else if (strstr(Server,"ign.com"))
{
 Type=TYPE_IGN;
}
else if (strstr(Server,"vbox7.com"))
{
 Type=TYPE_VBOX7;
}
else if (strstr(Server,"myvideo"))
{
 Type=TYPE_MYVIDEO;
}
else if (strstr(Server,"redbalcony.com"))
{
 Type=TYPE_REDBALCONY;
}
else if (strstr(Server,"mobango.com"))
{
 Type=TYPE_MOBANGO;
}
else if (strstr(Server,"izlese.org"))
{
 Type=TYPE_IZLESE;
}
else if (strstr(Server,"izles.net"))
{
 Type=TYPE_IZLES;
}
else if (strstr(Server,"izles.org"))
{
 Type=TYPE_IZLES_ORG;
}
else if (strstr(Server,"crazymotion.net"))
{
 Type=TYPE_CRAZYMOTION;
}
else if (strstr(Server,"clipshack.com"))
{
 Type=TYPE_CLIPSHACK;
}
else if (strstr(Server,"mytopclip.com"))
{
 Type=TYPE_MYTOPCLIP;
}
else if (strstr(Server,"liveleak"))
{
 Type=TYPE_LIVELEAK;
}
else if (strstr(Server,"blip.tv"))
{
 Type=TYPE_BLIP_TV;
}
else if (strstr(Server,"vimeo.com"))
{
 Type=TYPE_VIMEO;
}
else if (strstr(Server,"vidoevo.com"))
{
 Type=TYPE_VIDEOEMO;
}
else if (strstr(Server,"videosfacebook.net"))
{
 Type=TYPE_VIDEOSFACEBOOK;
}
else if (strstr(Server,"reuters"))
{
 Type=TYPE_REUTERS;
}
else if (strstr(Server,"clipfish.de"))
{
 Type=TYPE_CLIPFISH_DE;
}
else if (strstr(Server,"ucsd.tv"))
{
 Type=TYPE_UCSDTV;
}
else if (strstr(Server,"aljazeera.net"))
{
 Type=TYPE_ALJAZEERA;
}

return(Type);
}


main(int argc, char *argv[])
{
ListNode *Curr, *Next;
int OverrideType=TYPE_NONE;
char *Tempstr=NULL;

StdIn=STREAMFromFD(0);
STREAMSetTimeout(StdIn,0);

DownloadQueue=CreateEmptyList();
Tempstr=MCopyStr(Tempstr,"Movgrab ",Version,NULL);
HTTPSetUserAgent(Tempstr);
FormatPreference=CopyStr(FormatPreference,"mp4,flv,mov,mpg,mpeg,wmv,avi,3gp,yt-flv,mp3,m4a,wma");

ParseCommandLine(argc, argv, DownloadQueue, &OverrideType);

if (Flags & FLAG_PRINT_USAGE) PrintUsage();
else if (! (Flags & FLAG_STDIN))
{
if (CountItemsInList(DownloadQueue)==0) PrintUsage();
}


while (1)
{
	if ((Flags & FLAG_STDIN) && (CountItemsInList(DownloadQueue)==0) )
	{
		Tempstr=STREAMReadLine(Tempstr,StdIn);
		StripTrailingWhitespace(StdIn);
		AddItemToList(DownloadQueue,CopyStr(NULL,Tempstr));
	}

	Curr=GetNextListItem(DownloadQueue);
	while (Curr)
	{
		GrabMovie((char *) Curr->Item,OverrideType);
		Next=GetNextListItem(Curr);
		DeleteNodeFromList(Curr);
		Curr=Next;
	}

if (! (Flags & FLAG_STDIN)) break;
}

}
