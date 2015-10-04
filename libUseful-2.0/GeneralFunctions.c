#include "includes.h"
#include "base64.h"

void WritePidFile(char *ProgName) 
{ 
char *Tempstr=NULL; 
STREAM *S; 
 
Tempstr=FormatStr(Tempstr,"/var/run/%s.pid",ProgName);
S=STREAMOpenFile(Tempstr,O_CREAT | O_WRONLY); 
fchmod(S->in_fd,0644); 
if (flock(S->in_fd,LOCK_EX|LOCK_NB) !=0) 
{ 
STREAMClose(S); 
exit(1); 
} 
 
Tempstr=FormatStr(Tempstr,"%d\n",getpid()); 
STREAMWriteLine(Tempstr,S); 
STREAMFlush(S); 
} 


void CloseOpenFiles()
{
      int i;

      for (i=3; i < 1024; i++) close(i);
}


char *BytesToHexStr(char *Buffer, char *Bytes, int len)
{
int i;
char *Str=NULL, *ptr;


Str=SetStrLen(Buffer,(len *2) +1);
ptr=Str;
for (i=0; i < len; i++)
{
	snprintf(ptr,2,"%02x",Bytes[i]);
	ptr+=2;
}
*ptr='\0';

return(Str);
}

int HexStrToBytes(char **Buffer, char *HexStr)
{
int i, len;
char *Str=NULL, *ptr;

len=StrLen(HexStr);
*Buffer=SetStrLen(*Buffer,len / 2);
ptr=*Buffer;
for (i=0; i < len; i+=2)
{
   Str=CopyStrLen(Str,HexStr+i,2);
   *ptr=strtol(Str,NULL,16);
   ptr++;
}

DestroyString(Str);
return(len / 2);
}




/* This function turns our process into a demon */
int demonize()
{
int result, i=0;

result=fork();
if (result != 0) exit(0);

/*we can only get to here if result= 0 i.e. we are the child process*/
setsid();

result=fork();
if (result !=0) exit(0);
umask(0);

/* close stdin, stdout and std error, but only if they are a tty. In some  */
/* situations (like working out of cron) we may not have been given in/out/err */
/* and thus the first files we open will be 0,1,2. If we close them, we will have */
/* closed files that we need! Alternatively, the user may have used shell redirection */
/* to send output for a file, and I'm sure they don't want us to close that file */

//for (i=0; i < 3; i++)
{
	if (isatty(i)) 
	{
		close(i);
		/* reopen to /dev/null so that any output gets thrown away */
		/* but the program still has somewhere to write to         */
		open("/dev/null",O_RDWR);
	}
}


return(1);
}


void SwitchProgram(char *CommandLine)
{
char **argv, *ptr;
char *Token=NULL;
int i;

argv=(char **) calloc(101,sizeof(char *));
ptr=CommandLine;
for (i=0; i < 100; i++)
{
ptr=GetToken(ptr,"\\S",&Token,GETTOKEN_QUOTES);
if (! ptr) break;
argv[i]=CopyStr(argv[i],Token);
}

/* we are the child so we continue */
execv(argv[0],argv);
//no point trying to free stuff here, we will no longer
//be the main program
}

int ForkWithIO(int StdIn, int StdOut, int StdErr)
{
int result, fd;

result=fork();
if (result==0)
{
	if (StdIn > -1) 
	{
		if (StdIn !=0) 
		{
			close(0);
			dup(StdIn);
		}
	}
	else
	{
		fd=open("/dev/null",O_RDONLY);
		dup(fd);
		close(fd);
	}

	if (StdOut > -1) 
	{
		if (StdOut !=1) 
		{
			close(1);
			dup(StdOut);
		}
	}
	else
	{
		fd=open("/dev/null",O_WRONLY);
		dup(fd);
		close(fd);
	}

	if (StdErr > -1) 
	{
		if (StdErr !=2) 
		{
		close(2);
		dup(StdErr);
		}
	}
}
else
{
	fd=open("/dev/null",O_WRONLY);
	dup(fd);
	close(fd);
}

return(result);
}



int SpawnWithIO(char *CommandLine, int StdIn, int StdOut, int StdErr)
{
int result, fd, i;

result=ForkWithIO(StdIn,StdOut,StdErr);
if (result==0)
{
SwitchProgram(CommandLine);
_exit(result);
}
else return(result);
}


int Spawn(char *ProgName)
{
return(SpawnWithIO(ProgName, 0,1,2));
}


/* This creates a child process that we can talk to using a couple of pipes*/
int PipeSpawnFunction(int *infd,int  *outfd,int  *errfd, BASIC_FUNC Func, void *Data )
{
int result;
int channel1[2], channel2[2], channel3[2], DevNull=-1;
int count;

if (infd) pipe(channel1);
if (outfd) pipe(channel2);
if (errfd) pipe(channel3);

result=fork();
if (result==0)
{
/* we are the child */
if (infd) close(channel1[1]);
else if (DevNull==-1) DevNull=open("/dev/null",O_RDWR);
if (outfd) close(channel2[0]);
else if (DevNull==-1) DevNull=open("/dev/null",O_RDWR);
if (errfd) close(channel3[0]);
else if (DevNull==-1) DevNull=open("/dev/null",O_RDWR);

/*close stdin, stdout and stderr*/
close(0);
close(1);
close(2);
/*channel 1 is going to be our stdin, so we close the writing side of it*/
if (infd) dup(channel1[0]);
else dup(DevNull);
/* channel 2 is stdout */
if (outfd) dup(channel2[1]);
else dup(DevNull);
/* channel 3 is stderr */
if (errfd) dup(channel3[1]);
else dup(DevNull);

Func(Data);
exit(0);
}
else 
{
/* we close the appropriate halves of the link */
if (infd) 
{
	close(channel1[0]);
	*infd=channel1[1];
}
if (outfd)
{
	close(channel2[1]);
	*outfd=channel2[0];
}
if (errfd)
{
	close(channel3[1]);
	*errfd=channel3[0];
}

return(result);
}

}


int BASIC_FUNC_EXEC_COMMAND(void *Data)
{
return(execl("/bin/sh","/bin/sh","-c",(char *) Data,NULL));
}


int PipeSpawn(int *infd,int  *outfd,int  *errfd, char *Command)
{
return(PipeSpawnFunction(infd,outfd,errfd, BASIC_FUNC_EXEC_COMMAND, Command));
}

/* This checks if a certain file exists (not if we can open it etc, just if */
/* we can stat it, this is useful for checking pid files etc).              */
int FileExists(char *FileName)
{
struct stat StatData;

if (stat(FileName,&StatData) == 0) return(1);
else return(0);
}
                                     

void ColLibDefaultSignalHandler(int sig)
{

}


/* A general 'Set Timer' function, Useful for timing out */
/* socket connections etc                                */

void SetTimeout(int timeout)
{
struct sigaction SigAct;

SigAct.sa_handler=&ColLibDefaultSignalHandler;
SigAct.sa_flags=SA_RESETHAND;
//SigAct.sa_restorer=NULL;

sigaction(SIGALRM,&SigAct,NULL);
alarm(timeout);
}



int CreateLockFile(char *FilePath, int Timeout)
{
int fd, result;

SetTimeout(Timeout);
fd=open(FilePath, O_CREAT | O_RDWR, 0600);
if (fd <0) return(-1);
result=flock(fd,LOCK_EX);
alarm(0);

if (result==-1)
{
  close(fd);
  return(-1);
}
return(fd);
}

char *GetDateStrFromSecs(char *DateFormat, time_t Secs, char *TimeZone)
{
time_t val;
struct tm *TMS;
static char *Buffer=NULL;
char *Tempstr=NULL;
#define DATE_BUFF_LEN 255

val=Secs;

if (StrLen(TimeZone))
{
if (getenv("TZ")) Tempstr=CopyStr(Tempstr,getenv("TZ"));
setenv("TZ",TimeZone,TRUE);
tzset();
}
TMS=localtime(&val);
if (StrLen(TimeZone))
{
if (! Tempstr) unsetenv("TZ");
else setenv("TZ",Tempstr,TRUE);
tzset();
}

val=StrLen(DateFormat)+ DATE_BUFF_LEN;
Buffer=SetStrLen(Buffer,val);
strftime(Buffer,val,DateFormat,TMS);

DestroyString(Tempstr);
return(Buffer);
}



char *GetDateStr(char *DateFormat, char *TimeZone)
{
time_t Now;

time(&Now);
return(GetDateStrFromSecs(DateFormat, Now, TimeZone));
}


time_t DateStrToSecs(char *DateFormat, char *Str, char *TimeZone)
{
time_t Secs=0;
struct tm TMS;
char *Tempstr=NULL;
int val;

if (StrLen(DateFormat)==0) return(0);
if (StrLen(Str)==0) return(0);

if (StrLen(TimeZone))
{
	if (getenv("TZ")) Tempstr=CopyStr(Tempstr,getenv("TZ"));
	setenv("TZ",TimeZone,TRUE);
	tzset();
}

strptime(Str,DateFormat,&TMS);
TMS.tm_isdst=-1;
Secs=mktime(&TMS);

if (StrLen(TimeZone))
{
	if (! Tempstr) unsetenv("TZ");
	else setenv("TZ",Tempstr,TRUE);
	tzset();
}
return(Secs);
}


	
typedef struct
{
int operator;
double value;
} ExprToken;

char *OpStrings[]={"0","+","-","*","/","(",")","%","^",NULL};
typedef enum Ops {OP_VAL,OP_PLUS,OP_MINUS,OP_TIMES,OP_DIVIDE,OP_OPEN,OP_CLOSE,OP_MOD,OP_POW} LIBUSEFUL_TMATHOPS;

char *GetMathExprToken(char *String, char **Token)
{
char *ptr, *start;
int count, found=FALSE;

ptr=String;
if (! ptr) return(NULL);
if (*ptr=='\0') return(NULL);

while (isspace(*ptr)) ptr++;
start=ptr;

while (ptr)
{
	if (*ptr=='\0')
	{

		*Token=CopyStr(*Token,start);
		break;
	}

	for (count=1; OpStrings[count] !=NULL; count++)
	{
		if (strncmp(ptr,OpStrings[count],StrLen(OpStrings[count]))==0)
		{
			// we have come to an Operator at the start, so 
			// that is our token
			if (start==ptr)
			{
				*Token=CopyStr(*Token,OpStrings[count]);					ptr+=StrLen(OpStrings[count]);
			}
			else
			{
				*Token=CopyStrLen(*Token,start,ptr-start);
			}
			found=TRUE;
			break;
		}
	}
	if (found) break;
	ptr++;
}

return(ptr);
}


double ProcessMathExpression(ListNode *Tokens);

double ProcessSumExpression(ListNode *Tokens)
{
ListNode *Curr;
ExprToken *Tok, *NextTok;
double val=0;

Curr=ListGetNext(Tokens);
Tok=(ExprToken *) Curr->Item;
val=Tok->value;
Curr=ListGetNext(Curr);
while (Curr)
{
  Tok=(ExprToken *) Curr->Item;
  Curr=ListGetNext(Curr);
  NextTok=(ExprToken *) Curr->Item;

  
  if (Tok->operator==OP_PLUS) val+=NextTok->value;
  else if (Tok->operator==OP_MINUS) val-=NextTok->value;
  else if (Tok->operator==OP_VAL) val=Tok->value;
  Curr=ListGetNext(Curr);
}
ListClear(Tokens,free);
Tokens->Next=NULL;
Tokens->Head=NULL;
Tok=(ExprToken *) calloc(1,sizeof(ExprToken));
Tok->operator=OP_VAL;
Tok->value=val;
ListAddItem(Tokens,Tok);
return(val);
}


double ProcessMultDiv(ListNode *Tokens)
{
ListNode *Curr;
ExprToken *Tok, *PrevTok, *NextTok;
double val=0;
int count;

Curr=ListGetNext(Tokens);
while (Curr)
{
  Tok=Curr->Item;
  if (
	  (Tok->operator==OP_TIMES)  || 
	  (Tok->operator==OP_DIVIDE) 
     )
  {
	PrevTok=(ExprToken *) Curr->Prev->Item;
	NextTok=(ExprToken *) Curr->Next->Item;
	if (Tok->operator==OP_TIMES) val=PrevTok->value * NextTok->value;
	if (Tok->operator==OP_DIVIDE) val=PrevTok->value / NextTok->value;
	ListDeleteNode(Curr->Prev);
	ListDeleteNode(Curr->Next);
	free(PrevTok);
	free(NextTok);
	Tok->operator=OP_VAL;
	Tok->value=val;
  }
  
  Curr=ListGetNext(Curr);
}

return(val);
}

double ProcessExpn(ListNode *Tokens)
{
ListNode *Curr;
ExprToken *Tok, *PrevTok, *NextTok;
double val=0;
int count;

Curr=ListGetNext(Tokens);
while (Curr)
{
  Tok=Curr->Item;
  if (
	  (Tok->operator==OP_MOD) ||
	  (Tok->operator==OP_POW)
     )
  {
	PrevTok=(ExprToken *) Curr->Prev->Item;
	NextTok=(ExprToken *) Curr->Next->Item;
	if (Tok->operator==OP_MOD) val=(int) PrevTok->value % (int) NextTok->value;
	if (Tok->operator==OP_POW)
	{
		val=1.0;
		for (count=0; count < NextTok->value; count++)
		{
		   val=val * PrevTok->value;
		}
	}
	ListDeleteNode(Curr->Prev);
	ListDeleteNode(Curr->Next);
	free(PrevTok);
	free(NextTok);
	Tok->operator=OP_VAL;
	Tok->value=val;
  }
  
  Curr=ListGetNext(Curr);
}

return(val);
}


void ProcessBrackets(ListNode *Tokens)
{
ListNode *Start=NULL, *Curr, *SubExpr=NULL, *SubCurr=NULL;
ExprToken *Tok;

Curr=ListGetNext(Tokens);
while (Curr)
{
  Tok=(ExprToken *) Curr->Item;
  if (Start)
  {
	if (Tok->operator==OP_CLOSE)
	{
		SubCurr->Next=NULL;
		//replace end brace with val
		Tok->operator=OP_VAL;
		Tok->value=ProcessMathExpression(SubExpr);
		Start->Next=Curr;
		Curr->Prev=Start;
		Start=NULL;
	}
	else 
	{
		SubCurr->Next=Curr;
		Curr->Prev=SubCurr;
		SubCurr=SubCurr->Next;
	}
  }
  else if (Tok->operator==OP_OPEN)
  {
	Start=Curr->Prev;
	//get rid of pesky leading brace
	ListDeleteNode(Curr);
	free(Tok);
	Curr=Start;
	SubExpr=ListCreate();
	SubCurr=SubExpr;
  } 
  Curr=ListGetNext(Curr);
}

if (SubExpr) ListDestroy(SubExpr,free);
}

double ProcessMathExpression(ListNode *Tokens)
{
double val;

ProcessBrackets(Tokens);
ProcessExpn(Tokens);
ProcessMultDiv(Tokens);
val=ProcessSumExpression(Tokens);
return(val);
}


double EvaluateMathStr(char *String)
{
double val;
char *ptr, *Token=NULL;
int operator;
ListNode *Tokens, *Curr;
ExprToken *Tok;

Tokens=ListCreate();
ptr=GetMathExprToken(String,&Token);
while (ptr)
{
   operator=MatchTokenFromList(Token,OpStrings,0);
   if (operator==-1) operator=OP_VAL;
   Tok=(ExprToken *) calloc(1,sizeof(ExprToken));
   Tok->operator=operator;
   if (operator==OP_VAL) 
   {
	   Tok->value=atof(Token);
   }
   ListAddItem(Tokens,Tok);
   ptr=GetMathExprToken(ptr,&Token);
}

val=ProcessMathExpression(Tokens);

ListDestroy(Tokens,free);
DestroyString(Token);

return(val);
}



int MakeDirPath(char *Path, int DirMask)
{
 char *ptr;
 char *Tempstr=NULL;
 int result;

 ptr=Path;
 if (*ptr=='/') ptr++;
 ptr=strchr(ptr, '/');
 while (ptr)
 {
   Tempstr=CopyStrLen(Tempstr,Path,ptr-Path);
   result=mkdir(Tempstr, DirMask);
   if ((result==-1) && (errno != EEXIST)) break;
   ptr=strchr(++ptr, '/');
 }
 DestroyString(Tempstr);
 if (result==0) return(TRUE);
 return(FALSE);
}


#include <pwd.h>
#include <grp.h>

int SwitchUser(char *NewUser)
{
struct passwd *pwent;

    pwent=getpwnam(NewUser);
    if (! pwent) return(FALSE);
    if (setreuid(pwent->pw_uid,pwent->pw_uid) !=0) return(FALSE);
    return(TRUE);
}


int SwitchGroup(char *NewGroup)
{
struct group *grent;
 
     grent=getgrnam(NewGroup);
     if (! grent) return(FALSE);
     if (setgid(grent->gr_gid) !=0) return(FALSE);
     return(TRUE);
}


char *GetCurrUserHomeDir()
{
struct passwd *pwent;

    pwent=getpwuid(getuid());
    if (! pwent) return(NULL);
    return(pwent->pw_dir);
}



char *GetNameValuePair(const char *Input, const char *PairDelim, const char *NameValueDelim, char **Name, char **Value)
{
char *ptr, *ptr2;
char *Token=NULL;

ptr=GetToken(Input,PairDelim,&Token,GETTOKEN_QUOTES);
if (StrLen(Token) && strstr(Token,NameValueDelim))
{
ptr2=GetToken(Token,NameValueDelim,Name,GETTOKEN_QUOTES);
ptr2=GetToken(ptr2,PairDelim,Value,GETTOKEN_QUOTES);
}

DestroyString(Token);
return(ptr);
}





char *GetRandomData(char *RetBuff, int len, char *AllowedChars)
{
int fd;
char *Tempstr=NULL, *RetStr=NULL;
int i;
uint8_t val, max_val;

srand(time(NULL));
max_val=StrLen(AllowedChars);

RetStr=CopyStr(RetBuff,"");
fd=open("/dev/urandom",O_RDONLY);
for (i=0; i < len ; i++)
{
	if (fd > -1) read(fd,&val,1);
	else val=rand();

	RetStr=AddCharToStr(RetStr,AllowedChars[val % max_val]);
}

if (fd) close(fd);

DestroyString(Tempstr);
return(RetStr);
}


char *GetRandomHexStr(char *RetBuff, int len)
{
return(GetRandomData(RetBuff,len,HEX_CHARS));
}


char *GetRandomAlphabetStr(char *RetBuff, int len)
{
return(GetRandomData(RetBuff,len,ALPHA_CHARS));
}

char *XMLGetTag(char *Input, char **Namespace, char **TagType, char **TagData)
{
char *ptr, *tptr;
int len=0, InTag=FALSE, TagHasName=FALSE;

if (! Input) return(NULL);
if (*Input=='\0') return(NULL);
ptr=Input;

//This ensures we are never dealing with nulls
if (! *TagType) *TagType=CopyStr(*TagType,"");
if (! *TagData) *TagData=CopyStr(*TagData,"");

if (*ptr=='<') 
{
	ptr++;
	while (isspace(*ptr)) ptr++;

	len=0;
	InTag=TRUE;
	TagHasName=TRUE;

	//if we start with a slash tag, then add that to the tag name
	if (*ptr=='/') 
	{
		*TagType=AddCharToBuffer(*TagType,len,*ptr);
		len++;
		ptr++;
	}

	while (InTag)
	{
		switch (*ptr)
		{
		//These all cause us to end. NONE OF THEM REQUIRE ptr++
		//ptr++ will happen when we read tag data
		case '>':
		case '\0':
		case ' ':
		case '	':
		case '\n':
			InTag=FALSE;
		break;

		//If a namespace return value is supplied, break the name up into namespace and
		//tag. Otherwise don't
		case ':':
			if (Namespace)
			{
			tptr=*TagType;
			if (*tptr=='/')
			{
			  tptr++;
			  len=1;
			}
			else len=0;
			*Namespace=CopyStr(*Namespace,tptr);
			}
			else
			{
			*TagType=AddCharToBuffer(*TagType,len,*ptr);
			len++;
			}
			ptr++;
			
		break;

		

		case '\r':
			ptr++;
		break;

		default:
			*TagType=AddCharToBuffer(*TagType,len,*ptr);
			len++;
			ptr++;
		break;
		}
		
	}
}

//End of Parse TagName. Strip any '/'
tptr=*TagType;
if ((len > 0) && (tptr[len-1]=='/')) tptr[len-1]='\0'; 
tptr[len]='\0'; 

while (isspace(*ptr)) ptr++;


len=0;
InTag=TRUE;
while (InTag) 
{
	switch (*ptr)
	{
		//End of tag, skip '>' and fall through
		case '>':
		ptr++;

		//Start of next tag or end of text
		case '<':
		case '\0':
		 InTag=FALSE;
		break;

		//Quotes!
		case '\'':
		case '\"':

		//Somewhat ugly code. If we're dealing with an actual tag, then TagHasName
		//will be set. This means we're dealing with data within a tag and quotes mean something. 
		//Otherwise we are dealing with text outside of tags and we just add the char to
		//TagData and continue
		if (TagHasName)
		{
		tptr=ptr;
		while (*tptr != *ptr)
		{
			*TagData=AddCharToBuffer(*TagData,len,*ptr);
			len++;
			ptr++;
		}
		}
		*TagData=AddCharToBuffer(*TagData,len,*ptr);
		len++;
		ptr++;
		break;

		default:
		*TagData=AddCharToBuffer(*TagData,len,*ptr);
		len++;
		ptr++;
		break;
	}

}

//End of Parse TagData. Strip any '/'
tptr=*TagData;
if ((len > 0) && (tptr[len-1]=='/')) tptr[len-1]='\0'; 
tptr[len]='\0'; 

strlwr(*TagType);
while (isspace(*ptr)) ptr++;

return(ptr);
}




char *HtmlGetTag(char *Input, char **TagType, char **TagData)
{
char *ptr;
int len=0;

if (! Input) return(NULL);
if (*Input=='\0') return(NULL);

return(XMLGetTag(Input, NULL, TagType, TagData));
}



char *HtmlDeQuote(char *RetStr, char *Data)
{
char *Output=NULL, *Token=NULL, *ptr;
int len=0;

Output=CopyStr(RetStr,Output);
ptr=Data;

while (ptr && (*ptr != '\0'))
{
	if (*ptr=='&')
	{
		ptr++;
		ptr=GetToken(ptr,";",&Token,0);

		if (*Token=='#')
		{
			Output=AddCharToBuffer(Output,len,strtol(Token+1,NULL,16));
			len++;
		}
		else if (strcmp(Token,"amp")==0)
		{
			Output=AddCharToBuffer(Output,len,'&');
			len++;
		}
		else if (strcmp(Token,"lt")==0)
		{
			Output=AddCharToBuffer(Output,len,'<');
			len++;
		}
		else if (strcmp(Token,"gt")==0)
		{
			Output=AddCharToBuffer(Output,len,'>');
			len++;
		}
		else if (strcmp(Token,"quot")==0)
		{
			Output=AddCharToBuffer(Output,len,'"');
			len++;
		}
		else if (strcmp(Token,"apos")==0)
		{
			Output=AddCharToBuffer(Output,len,'\'');
			len++;
		}
		else if (strcmp(Token,"tilde")==0)
		{
			Output=AddCharToBuffer(Output,len,'~');
			len++;
		}
		else if (strcmp(Token,"circ")==0)
		{
			Output=AddCharToBuffer(Output,len,'^');
			len++;
		}




	}
	else
	{
		Output=AddCharToBuffer(Output,len,*ptr);
		len++;
		ptr++;
	}
}

DestroyString(Token);

return(Output);
}


int ChangeFileExtension(char *FilePath, char *NewExt)
{
char *ptr;
char *Tempstr=NULL;
int result;

Tempstr=CopyStr(Tempstr,FilePath);
ptr=strrchr(Tempstr,'/');
if (!ptr) ptr=Tempstr;
ptr=strrchr(ptr,'.');
if (! ptr) ptr=Tempstr+StrLen(Tempstr);
*ptr='\0';

if (*NewExt=='.') Tempstr=CatStr(Tempstr,NewExt);
else Tempstr=MCatStr(Tempstr,".",NewExt,NULL);
result=rename(FilePath,Tempstr);

DestroyString(Tempstr);
if (result==0) return(TRUE);
else return(FALSE);
}




#define KILOBYTE 1000
#define MEGABYTE 1000000
#define GIGABYTE 1000000000
#define TERABYTE 1000000000000

#define KIBIBYTE 1024
#define MEGIBYTE 1024 * 1024
#define GIGIBYTE 1024 * 1024 * 1024
#define TERIBYTE 1024 * 1024 * 1024 *1024

double ParseHumanReadableDataQty(char *Data, int Type)
{
double val;
char *ptr=NULL;
double KAY,MEG,GIG,TERA;

if (Type)
{
KAY=KILOBYTE;
MEG=MEGABYTE;
GIG=GIGABYTE;
//TERA=TERABYTE;
}
else
{
KAY=KIBIBYTE;
MEG=MEGIBYTE;
GIG=GIGIBYTE;
//TERA=TERIBYTE;
}

	val=strtod(Data,&ptr);
	while (isspace(*ptr)) ptr++;
	if (*ptr=='k') val=val * KAY;
	if (*ptr=='M') val=val * MEG;
	if (*ptr=='G') val=val * GIG;
//	if (*ptr=='T') val=val * TERA;


return(val);
}



char *GetHumanReadableDataQty(double Size, int Type)
{
static char *Str=NULL;
double val=0;
char kMGT=' ';
double KAY,MEG,GIG,TERA;

if (Type)
{
KAY=KILOBYTE;
MEG=MEGABYTE;
GIG=GIGABYTE;
//TERA=TERABYTE;
}
else
{
KAY=KIBIBYTE;
MEG=MEGIBYTE;
GIG=GIGIBYTE;
//TERA=TERIBYTE;
}
    val=Size;
    kMGT=' ';
/*    if (val > (TERA))
    {
      val=val / TERA;
      kMGT='T';
    }
    else*/
	 if (val > (GIG))
    {
      val=val / GIG;
      kMGT='G';
    }
    else if (val > (MEG))
    {
      val=val / MEG;
      kMGT='M';

    }
    else if (val > (KAY))
    {
      val=val /  KAY;
      kMGT='k';
    }

Str=FormatStr(Str,"%0.1f%c",(float) val,kMGT);
return(Str);
}


char *FindFileInPath(char *InBuff, char *File, char *Path)
{
char *Tempstr=NULL, *CurrPath=NULL, *RetStr=NULL, *ptr;

RetStr=CopyStr(InBuff,"");

ptr=GetToken(Path,":",&CurrPath,0);
while (ptr)
{
Tempstr=MCopyStr(Tempstr,CurrPath,"/",File,NULL);
if (access(Tempstr,F_OK)==0) 
{
RetStr=CopyStr(RetStr,Tempstr);
break;
}

ptr=GetToken(ptr,":",&CurrPath,0);
}

DestroyString(Tempstr);
DestroyString(CurrPath);

return(RetStr);
}
