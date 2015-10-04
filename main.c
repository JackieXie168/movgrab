//Movgrab. A downloader for movie/media websites.
//Written by Colum Paget.
//Copyright 2010 Colum Paget.


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


#include "servicetypes.h"
#include "containerfiles.h"
#include "download.h"
#include "outputfiles.h"
#include "youtube.h"
#include "dailymotion.h"
#include "cbsnews.h"
#include "ehow.h"




int Flags=0;
char *ItemSelectionArg=NULL;
char *ProgName=NULL, *CmdLine=NULL;
char *FormatPreference=NULL;
ListNode *DownloadQueue=NULL;
STREAM *StdIn=NULL;
char *Username=NULL, *Password=NULL;
int STREAMTimeout=30;
int Type=TYPE_NONE, DefaultPort=80;

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
    ListAddItem(DownloadQueue,CopyStr(NULL,Tempstr));
    if (! (Flags & FLAG_QUIET)) fprintf(stderr,"\r\nQUEUED: %s\n",Tempstr);
    result=TRUE;
  }
}
DestroyString(Tempstr);

return(result);
}





//-------- Go through the processes involved in getting a video file
int GrabMovie(char *Path, int MovType)
{
int i;
char *Proto=NULL, *Server=NULL, *Doc=NULL, *Tempstr=NULL, *Title=NULL;
char *NextPath=NULL;
char *ptr, *Token=NULL;
int Port;
int RetVal=FALSE;

if (!StrLen(Path)) return(FALSE);

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

if (Type==TYPE_NONE) 
{
if (GetContainerFileType(Path) != -1) Type=TYPE_CONTAINERFILE;
else Type=IdentifyServiceType(Server);
}


if (Type==TYPE_NONE)
{
if (! (Flags & FLAG_QUIET)) fprintf(stderr,"Unrecognized url type. Falling Back to 'generic youtube frontend'.\n Try using the -t option to force the service type ( \"movgrab -?\" for more details )\n");
Type=TYPE_GENERIC;
}

if (Type==TYPE_CONTAINERFILE) RetVal=DownloadContainer(NextPath,Title,Flags);
else
{
NextPath=SiteSpecificPreprocessing(NextPath, Path, Proto, Server, Port, Doc, Type, &Title, &Flags);

if (DownloadPage(NextPath, Type,Title,Flags)) RetVal=TRUE;
}

DestroyString(Tempstr);
DestroyString(Server);
DestroyString(Doc);
DestroyString(NextPath);
DestroyString(Token);
DestroyString(Title);
DestroyString(Proto);

return(RetVal);
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
			fprintf(stderr,"	% 4d: %4s  %s\n",i,ptr,Var);
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
 		ListAddNamedItem(Results,GetVar(Vars,Tempstr),CopyStr(NULL,Var));
		}

DestroyString(Tempstr);
DestroyString(Var);
}


char *GatherMatchingFormats(char *Buffer, char *Type, ListNode *Vars)
{
ListNode *Curr;
char *Tempstr=NULL;

Tempstr=CopyStr(Buffer,"");
Curr=ListGetNext(Vars);
while (Curr)
{
if (strncmp(Curr->Tag,"item:",5)==0)
{
if ((StrLen(Type)==0) || (strncmp(Curr->Tag,Type,StrLen(Type))==0)) Tempstr=MCatStr(Tempstr,Curr->Tag+5," ",NULL);
}

Curr=ListGetNext(Curr);
}

return(Tempstr);
}

int FmtIDMatches(char *FmtID, char *CurrItem, char *ItemData)
{
				if (strncmp(CurrItem,"item:",5) !=0) return(FALSE);
				if (StrLen(FmtID)==0) return(TRUE);
				if (strcmp(FmtID,"item:*")==0) return(TRUE);
				if ((strncmp(CurrItem,FmtID,StrLen(FmtID))==0) && (StrLen(ItemData))) return(TRUE);
			return(FALSE);
}


void DisplayAvailableFormats(ListNode *Vars, char *Formats)
{
char *Token=NULL, *TokenID=NULL, *Tempstr=NULL, *ptr;
STREAM *S;
double val;

fprintf(stderr, "\nFormats available for this Movie:");

ptr=GetToken(Formats," ",&Token,0);
while (ptr)
{
if (StrLen(Token)) TokenID=MCopyStr(TokenID,"item:",Token,NULL);

Tempstr=CopyStr(Tempstr,GetVar(Vars,TokenID));

if (strcmp(Token,"reference") !=0)
{
	S=HTTPMethod("HEAD",Tempstr,NULL,NULL);
	fprintf(stderr,"%s",Token);
	if (S)
	{
		Tempstr=CopyStr(Tempstr,STREAMGetValue(S,"HTTP:Content-length"));
		val=atof(Tempstr);
		fprintf(stderr, " (%s)",GetHumanReadableDataQty(val,TRUE));
		STREAMClose(S);
	}
}
fprintf(stderr,", ");

ptr=GetToken(ptr," ",&Token,0);
}


fprintf(stderr,"\n\n",Tempstr);
}

//this function compares the video formats found on the page to the list of
//preferences expressed by the user with the '-f' flag, and contained in the
//global variable 'FormatPreference'
int SelectDownloadFormat(ListNode *Vars, int WebsiteType)
{
ListNode *Curr;
char *ptr, *Tempstr=NULL, *Fmt=NULL, *FmtID=NULL, *Selected=NULL, *p_ItemFormat;
int RetVal=-1, FoundMatch=FALSE;

Tempstr=GatherMatchingFormats(Tempstr,"",Vars);
if (! (Flags & FLAG_QUIET)) DisplayAvailableFormats(Vars, Tempstr);

	ptr=GetToken(FormatPreference,",",&Fmt,0);
	while (ptr)
	{
	if (StrLen(Fmt)) FmtID=MCopyStr(FmtID,"item:",Fmt,NULL);
	else FmtID=CopyStr(FmtID,"");

	if (Flags & FLAG_DEBUG) fprintf(stderr,"  %s ",Fmt);

		FoundMatch=FALSE;
		Curr=ListGetNext(Vars);
		while (Curr)
		{

			if (FmtIDMatches(FmtID,Curr->Tag, (char *) Curr->Item))
				{
					if (Flags & (FLAG_DEBUG)) 
					{
						Tempstr=GatherMatchingFormats(Tempstr,FmtID,Vars);
						fprintf(stderr,"... YES! %s\n",Tempstr);
					}

					FoundMatch=TRUE;
					//where available a local filetype overrides a reference
					if ((RetVal==-1) || (RetVal==TYPE_REFERENCE))
					{
					Selected=CopyStr(Selected,Curr->Tag);
					SetVar(Vars,"ID",(char *) Curr->Item);
					}
					break;
				}
				Curr=ListGetNext(Curr);
			}
			if ((! FoundMatch) && (Flags & (FLAG_DEBUG))) fprintf(stderr,"... no\n",Fmt);
			ptr=GetToken(ptr,",",&Fmt,0);
	}


if (StrLen(Selected))
{
	if (strcmp(Selected,"item:reference")==0) RetVal=TYPE_REFERENCE;
	else RetVal=WebsiteType;
}

if (! (Flags & FLAG_TEST_SITES))
{
if (RetVal==-1) fprintf(stderr,"No suitable download format found from '%s'\n\n",FormatPreference);
else if (RetVal==TYPE_REFERENCE) fprintf(stderr,"Reference to another site: %s\n",GetVar(Vars,"ID"));
else fprintf(stderr,"Selected format %s\n",Selected);
}


//+5 to get past leading 'item:' in variable name
if (StrLen(Selected)) SetVar(Vars,"DownloadFormat",Selected+5);

DestroyString(Selected);
DestroyString(Tempstr);
DestroyString(Fmt);
return(RetVal);
}





void PrintUsage()
{
int i;

fprintf(stdout,"\nMovgrab: version %s\n",Version);
fprintf(stdout,"Author: Colum Paget\n");
fprintf(stdout,"Email: colums.projects@gmail.com\n");
fprintf(stdout,"Blogs: \n");
fprintf(stdout,"	tech: http://idratherhack.blogspot.com \n");
fprintf(stdout,"	rants: http://thesingularitysucks.blogspot.com \n");
fprintf(stdout,"\n");
fprintf(stdout,"Usage: movgrab [-t <type>] -a [<username>:<password>] [-p http://username:password@x.x.x.x:80 ] [-r] [-b] [-x] [-q] [-st <stream timeout>] [-f <format list>] [-v] [-P] [-Pp] [-o <output file>] [+o <extra output file>] url\n");
fprintf(stdout,"	movgrab -test-sites\n");
fprintf(stdout,"\n'-v'		increases verbosity/debug level\n");
fprintf(stdout,"'-v -v'		prints out all webpages encountered\n");
fprintf(stdout,"'-v -v -v'	maximum debugging\n");
fprintf(stdout,"'-T'		Test mode, don't do final download\n");
fprintf(stdout,"'-P <program>'		Player program (e.g. \"mplayer\")\n");
fprintf(stdout,"'-Pp'		Percent download to launch player at (default 25%)\n");
fprintf(stdout,"'-a'		Authentication info in Username:Password format.\n");
fprintf(stdout,"'-q'		QUIET. No progress/informative output.\n");
fprintf(stdout,"'-b'		Background. Fork into background and nohup\n");
fprintf(stdout,"'-p'		address of HTTP proxy server in URL format.\n");
fprintf(stdout,"'-w'		Wait for addresses to be entered on stdin.\n");
fprintf(stdout,"'-st'		Connection inactivity timeout in seconds. Set high for sites that 'throttle'\n");
fprintf(stdout,"'-t'		specifies website type.\n");
fprintf(stdout,"'-r'		Resume download (only works when writing a single file, not with +o).\n");
fprintf(stdout,"'-f'		specifies preferred video/audio formats for sites that offer more than one\n");
fprintf(stdout,"			example: flv:640x480,flv,mp4,mp3\n");
fprintf(stdout,"			Use -T to get a list of formats the site offers\n");
fprintf(stdout,"			Use * to mean 'any format' (the default)\n");
fprintf(stdout,"			example: flv:640x480,mp4:640x480,*\n");
fprintf(stdout,"'-o'		specifies output file ( '-' for stdout)\n");
fprintf(stdout,"'+o'		add an output file to list of output files ( '-' for stdout)\n");
fprintf(stdout,"'-x'		try to avoid 'family filter' on some sites\n");
fprintf(stdout,"'-n'		For pages with multiple movies (not movie formats, but movies) specifes item selection for download. Argument has the form:\n");
fprintf(stdout,"			-n all		Download all\n");
fprintf(stdout,"			-n 2		Download 2nd item\n");
fprintf(stdout,"			-n 2-5		Download 2nd to 5th item\n");
fprintf(stdout,"			-n 2-		Download from 2nd item onwards\n");
fprintf(stdout,"			-n -8		Download from 1st to 8th item\n");
fprintf(stdout,"-test-sites		Test downloading from each supported website");
fprintf(stdout,"\nThe '-t' argument is optional, Movgrab will try to guess the download type from the provided url.\n\n");

fprintf(stdout,"The url should be that of the page that the movie would normally appear on. Movgrab examines the page for embed tags and other information that would normally cause the movie to play on that page.\n");

fprintf(stdout,"\nDownload types are:\n");
for (i=1; DownloadTypes[i] !=NULL; i++) fprintf(stdout,"%-20s %s\n",DownloadTypes[i],DownloadNames[i]);

fprintf(stdout,"\nIf a website is not in the list, try 'movgrab -t generic <url>'\n");
fprintf(stdout,"\nFeel free to email me and tell me if you've used this software!\n");

fprintf(stdout,"\nIf you want to watch quite a good youtube movie, try 'SPIN', \"movgrab http://www.youtube.com/watch?v=oP59tQf_njc\"\n");

fprintf(stdout,"\nThanks for bug reports go to: Mark Gamar, Rich Kcsa, 'Rampant Badger', 'nibbles', Omair Eshkenazi, Matthias B, Ashish Disawal, Timo Juhani Lindfors, Abhisek Sanyal and others.\n");
fprintf(stdout,"\nSpecial thanks to:\n");
fprintf(stdout,"	'legatvs' for clive (http://clive.sourceforge.net) another downloader into whose code I had to look to figure out how to get youtube and daily motion working again.\n");
fprintf(stdout,"	Robert Crowley (http://tools.99k.org/) For all sorts of bug reports and advice.\n");
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
int i,j, DebugLevel=0;
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
			ptr=GetToken(argv[++i],":",&Username,0);
			ptr=GetToken(ptr,":",&Password,0);
	}
	else if (strcmp(argv[i],"-v")==0)
	{
		DebugLevel++;
	}
	else if (strcmp(argv[i],"-o")==0)
	{
		AddOutputFile(argv[++i], TRUE);
	}
	else if (strcmp(argv[i],"+o")==0)
	{
		AddOutputFile(argv[++i], FALSE);
	}
	else if (strcmp(argv[i],"-n")==0)
	{
		ItemSelectionArg=CopyStr(ItemSelectionArg,argv[++i]);
	}
	else if (strcmp(argv[i],"-t")==0) *OverrideType=ParseType(argv[++i]);
	else if (strcmp(argv[i],"-f")==0) FormatPreference=CopyStr(FormatPreference,argv[++i]);
	else if (strcmp(argv[i],"-q")==0) Flags |= FLAG_QUIET;
	else if (strcmp(argv[i],"-b")==0) Flags |= FLAG_BACKGROUND;
	else if (strcmp(argv[i],"-r")==0) Flags |= FLAG_RESUME;
	else if (strcmp(argv[i],"-x")==0) Flags |= FLAG_PORN;
	else if (strcmp(argv[i],"-T")==0) Flags |= FLAG_TEST;
	else if (strcmp(argv[i],"-w")==0) Flags |= FLAG_STDIN;
	else if (strcmp(argv[i],"-st")==0) STREAMTimeout=atoi(argv[++i]);
	else if (strcmp(argv[i],"-P")==0) Player=CopyStr(Player,argv[++i]);
	else if (strcmp(argv[i],"-Pp")==0) PlayerLaunchPercent=atoi(argv[++i]);
	else if (strcmp(argv[i],"-?")==0) Flags |= FLAG_PRINT_USAGE;
	else if (strcmp(argv[i],"-h")==0) Flags |= FLAG_PRINT_USAGE;
	else if (strcmp(argv[i],"-help")==0) Flags |= FLAG_PRINT_USAGE;
	else if (strcmp(argv[i],"--help")==0) Flags |= FLAG_PRINT_USAGE;
	else if (strcmp(argv[i],"-test-sites")==0) 
	{
		Flags |= FLAG_TEST_SITES | FLAG_QUIET;
		for (j=1; TestLinks[j] !=NULL; j++)
		{
		if (StrLen(TestLinks[j])) ListAddNamedItem(DL_List,DownloadTypes[j],CopyStr(NULL,TestLinks[j]));
			
		}
		ItemSelectionArg=CopyStr(ItemSelectionArg,"0");
	}
	else
	{
		ListAddItem(DL_List,CopyStr(NULL,argv[i]));
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


main(int argc, char *argv[])
{
ListNode *Curr, *Next;
int OverrideType=TYPE_NONE;
char *Tempstr=NULL;
int result;

StdIn=STREAMFromFD(0);
STREAMSetTimeout(StdIn,0);

Tempstr=CopyStr(Tempstr,getenv("http_proxy"));
if (StrLen(Tempstr)) HTTPSetProxy(Tempstr);

DownloadQueue=ListCreate();
Tempstr=MCopyStr(Tempstr,"Movgrab ",Version,NULL);
HTTPSetUserAgent(Tempstr);
FormatPreference=CopyStr(FormatPreference,"mp4,flv,webm,mov,mpg,mpeg,wmv,avi,3gp,reference,mp3,m4a,wma");
AddOutputFile("", TRUE);

ParseCommandLine(argc, argv, DownloadQueue, &OverrideType);

if (Flags & FLAG_PRINT_USAGE) PrintUsage();
else if (! (Flags & FLAG_STDIN))
{
	if (ListSize(DownloadQueue)==0) PrintUsage();
}


while (1)
{
	if ((Flags & FLAG_STDIN) && (ListSize(DownloadQueue)==0) )
	{
		Tempstr=STREAMReadLine(Tempstr,StdIn);
		StripTrailingWhitespace(Tempstr);
		ListAddItem(DownloadQueue,CopyStr(NULL,Tempstr));
	}

	Curr=ListGetNext(DownloadQueue);
	while (Curr)
	{
		if (Flags & FLAG_TEST_SITES)
		{
			fprintf(stderr,"Checking %-20s ",Curr->Tag);
			fflush(NULL);
		}

		result=GrabMovie((char *) Curr->Item,OverrideType);
		Next=ListGetNext(Curr); //must do this after grab movie
																//incase more items added while grabbing

			if (Flags & FLAG_TEST_SITES) 
			{
				if (result) fprintf(stderr," okay\n");
				else fprintf(stderr," BROKEN\n");
			}
			
		ListDeleteNode(Curr);

		Curr=Next;
	}

	if (Flags & FLAG_TEST_SITES) break;
	if (! (Flags & FLAG_STDIN)) break;
}

}
