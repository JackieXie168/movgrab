#include "includes.h"
#include "base64.h"


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
int result, i;

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

for (i=0; i < 3; i++)
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


int Spawn(char *ProgName)
{
int result;

result=fork();


if (result==0)
{
/* we are the child so we continue */
result=execl(ProgName,NULL);
return(result);
}
else return(result);

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
SigAct.sa_flags=SA_ONESHOT | SA_INTERRUPT;
SigAct.sa_restorer=NULL;

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
#define DATE_BUFF_LEN 40

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

Buffer=SetStrLen(Buffer,DATE_BUFF_LEN);
strftime(Buffer,DATE_BUFF_LEN,DateFormat,TMS);

DestroyString(Tempstr);
return(Buffer);
}


char *GetDateStr(char *DateFormat, char *TimeZone)
{
time_t Now;

time(&Now);
return(GetDateStrFromSecs(DateFormat, Now, TimeZone));
}


time_t DateStrToSecs(char *DateFormat, char *Str)
{
time_t Secs;
struct tm TMS;

if (StrLen(DateFormat)==0) return(0);
if (StrLen(Str)==0) return(0);
strptime(Str,DateFormat,&TMS);
TMS.tm_isdst=-1;
Secs=mktime(&TMS);

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

Curr=GetNextListItem(Tokens);
Tok=(ExprToken *) Curr->Item;
val=Tok->value;
Curr=GetNextListItem(Curr);
while (Curr)
{
  Tok=(ExprToken *) Curr->Item;
  Curr=GetNextListItem(Curr);
  NextTok=(ExprToken *) Curr->Item;

  
  if (Tok->operator==OP_PLUS) val+=NextTok->value;
  else if (Tok->operator==OP_MINUS) val-=NextTok->value;
  else if (Tok->operator==OP_VAL) val=Tok->value;
  Curr=GetNextListItem(Curr);
}
ClearList(Tokens,free);
Tokens->Next=NULL;
Tokens->Head=NULL;
Tok=(ExprToken *) calloc(1,sizeof(ExprToken));
Tok->operator=OP_VAL;
Tok->value=val;
AddItemToList(Tokens,Tok);
return(val);
}


double ProcessMultDiv(ListNode *Tokens)
{
ListNode *Curr;
ExprToken *Tok, *PrevTok, *NextTok;
double val=0;
int count;

Curr=GetNextListItem(Tokens);
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
	DeleteNodeFromList(Curr->Prev);
	DeleteNodeFromList(Curr->Next);
	free(PrevTok);
	free(NextTok);
	Tok->operator=OP_VAL;
	Tok->value=val;
  }
  
  Curr=GetNextListItem(Curr);
}

return(val);
}

double ProcessExpn(ListNode *Tokens)
{
ListNode *Curr;
ExprToken *Tok, *PrevTok, *NextTok;
double val=0;
int count;

Curr=GetNextListItem(Tokens);
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
	DeleteNodeFromList(Curr->Prev);
	DeleteNodeFromList(Curr->Next);
	free(PrevTok);
	free(NextTok);
	Tok->operator=OP_VAL;
	Tok->value=val;
  }
  
  Curr=GetNextListItem(Curr);
}

return(val);
}


void ProcessBrackets(ListNode *Tokens)
{
ListNode *Start=NULL, *Curr, *SubExpr=NULL, *SubCurr=NULL;
ExprToken *Tok;

Curr=GetNextListItem(Tokens);
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
	DeleteNodeFromList(Curr);
	free(Tok);
	Curr=Start;
	SubExpr=CreateEmptyList();
	SubCurr=SubExpr;
  } 
  Curr=GetNextListItem(Curr);
}

if (SubExpr) DestroyList(SubExpr,free);
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

Tokens=CreateEmptyList();
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
   AddItemToList(Tokens,Tok);
   ptr=GetMathExprToken(ptr,&Token);
}

val=ProcessMathExpression(Tokens);

DestroyList(Tokens,free);
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


#include "md5.h"
#define MD5LEN 16
char *HashMD5(char *Return, char *text, int len, int Encoding)
{
MD5_CTX context;
int count;
unsigned char *digestbuff=NULL;
char *HashStr=NULL, *Tempstr=NULL;
int i;

HashStr=CopyStr(Return,"");
digestbuff=SetStrLen(digestbuff,MD5LEN+1);
memset(digestbuff,0,MD5LEN);
MD5Init(&context);
MD5Update(&context, text, len);
MD5Final(digestbuff, &context);

switch (Encoding)
{
case ENCODE_HEX:
for (i=0; i < MD5LEN; i++)
{
Tempstr=FormatStr(Tempstr,"%02x",digestbuff[i]);
HashStr=CatStr(HashStr,Tempstr);
}
break;

case ENCODE_BASE64:
HashStr=SetStrLen(HashStr,42);
to64frombits(HashStr,digestbuff,16);
break;
}

DestroyString(digestbuff);
DestroyString(Tempstr);
return(HashStr);
}


char *EncodeBase64(char *Return, char *Text, int len)
{
char *RetStr;

RetStr=SetStrLen(Return,len *2);
to64frombits(RetStr,Text,len);

return(RetStr);
}

char *DecodeBase64(char *Return, int *len, char *Text)
{
char *RetStr;

RetStr=SetStrLen(Return,StrLen(Text) *2);
*len=from64tobits(RetStr,Text);

return(RetStr);
}



void SetVar(ListNode *Vars, char *Name, char *Data)
{
ListNode *Node;
char *Tempstr=NULL;

Tempstr=CopyStr(Tempstr,Name);
strlwr(Tempstr);
Node=ListFindNamedItem(Vars,Tempstr);
if (Node) Node->Item=(void *) CopyStr((char *) Node->Item,Data);
else AddNamedItemToList(Vars,Tempstr,CopyStr(NULL,Data));

DestroyString(Tempstr);
}

char *GetVar(ListNode *Vars, char *Name)
{
ListNode *Node;
char *Tempstr=NULL;

Tempstr=CopyStr(Tempstr,Name);
strlwr(Tempstr);
Node=ListFindNamedItem(Vars,Tempstr);
DestroyString(Tempstr);
if (Node) return((char *) Node->Item);
return(NULL);
}


void UnsetVar(ListNode *Vars,char *Name)
{
ListNode *Curr;
char *Str=NULL;
char *Tempstr=NULL;

if (Vars) return;
Tempstr=CopyStr(Tempstr,Name);
strlwr(Tempstr);
Curr=ListFindNamedItem(Vars,Tempstr);
if (Curr)
{
    Str=DeleteNodeFromList(Curr);
    DestroyString(Str);
}
DestroyString(Tempstr);
}


void ClearVars(ListNode *Vars)
{
ListNode *Curr;
char *Str;

if (! Vars) return;
Curr=GetNextListItem(Vars);
while (Curr)
{
    Str=DeleteNodeFromList(Curr);
    DestroyString(Str);
Curr=GetNextListItem(Curr);
}


}



void CopyVars(ListNode *Dest, ListNode *Source)
{
ListNode *Curr;
char *Str;

if (! Dest) return;
if (! Source) return;
Curr=GetNextListItem(Source);
while (Curr)
{
SetVar(Dest,Curr->Tag,Curr->Item);
Curr=GetNextListItem(Curr);
}

}



char *SubstituteVarsInString(char *Buffer, char *Fmt, ListNode *Vars, int Flags)
{
char *ReturnStr=NULL, *FmtPtr, *VarName=NULL;
int count, VarIsPointer=FALSE;
ListNode *Curr;
int len=0, i;

ReturnStr=CopyStr(Buffer,"");

if (! Fmt) return(ReturnStr);


FmtPtr=Fmt;
while (*FmtPtr !=0)
{
	switch (*FmtPtr)
	{

		case '\\':
			FmtPtr++;
			switch (*FmtPtr)
			{
			   case 't':
				ReturnStr=AddCharToStr(ReturnStr,' ');
				len=StrLen(ReturnStr);
				while ((len % 4) !=0)
				{
				ReturnStr=AddCharToStr(ReturnStr,' ');
				len++;
				}
			   break;

			   case 'r':
				ReturnStr=AddCharToStr(ReturnStr,'\r');
				len++;
			   break;

			   case 'n':
				ReturnStr=AddCharToStr(ReturnStr,'\n');
				len++;
			   break;

			   default:
				ReturnStr=AddCharToStr(ReturnStr,*FmtPtr);
				len++;
			}
		break;

		case '$':
  			FmtPtr++;
			if (*FmtPtr=='$')
			{
				 VarIsPointer=TRUE;
				 FmtPtr++;
			}
			if (*FmtPtr=='(') FmtPtr++;
			for (count=0; (*(FmtPtr+count) !=0) && (*(FmtPtr+count) !=')'); count++);
			if (count > 0) 
			{
				VarName=CopyStrLen(VarName,FmtPtr,count); 
				if (! (Flags & SUBS_CASE_VARNAMES)) strlwr(VarName);
   				Curr=ListFindNamedItem(Vars,VarName);
				if (Curr && (StrLen(Curr->Item) > 0))
				{
				   if (Flags & SUBS_QUOTE_VARS) ReturnStr=CatStr(ReturnStr,"'");
				   ReturnStr=CatStr(ReturnStr,(char *) Curr->Item);
				   if (Flags & SUBS_QUOTE_VARS) ReturnStr=CatStr(ReturnStr,"'");
				   len=StrLen(ReturnStr);
				}
				FmtPtr+=count;
			}
 		break;

		case '"':
			FmtPtr++;
			while (*FmtPtr && (*FmtPtr !='"')) 
			{
			 	ReturnStr=AddCharToStr(ReturnStr,*FmtPtr);
				len++;
				FmtPtr++;
			}
			break;
 
		default:
			 ReturnStr=AddCharToStr(ReturnStr,*FmtPtr);
			 len++;
	}

FmtPtr++;
}


DestroyString(VarName);
return(ReturnStr);
}





void ExtractVarsReadVar(char **Fmt, char **Msg, ListNode *Vars)
{
char *FmtPtr, *MsgPtr;
char *VarName=NULL;
int len=0;
ListNode *Node;

FmtPtr=*Fmt;

        if (*FmtPtr=='(') FmtPtr++;
	while (*FmtPtr != ')')
	{
		VarName=AddCharToBuffer(VarName,len,*FmtPtr);
		len++;
		FmtPtr++;
	}
        if (*FmtPtr==')') FmtPtr++;

MsgPtr=*Msg;
while ((*MsgPtr !=0) && (*MsgPtr != *FmtPtr)) MsgPtr++;

Node=ListFindNamedItem(Vars,VarName);
if (Node) Node->Item=(void *) CopyStrLen((char *) Node->Item, *Msg, MsgPtr-*Msg);
else AddNamedItemToList(Vars,VarName,CopyStrLen(NULL, *Msg, MsgPtr-*Msg));
*Fmt=FmtPtr;
*Msg=MsgPtr;

DestroyString(VarName);
}

char *ExtractVarsGetLiteralString(char *Buffer, char *InStr)
{
char *RetStr, *ptr;

RetStr=Buffer;

ptr=InStr;
while ((*ptr !=0) && (*ptr !='$') && (*ptr !='?') && (*ptr !='*')) ptr++;

RetStr=CopyStrLen(Buffer,InStr,ptr-InStr);
return(RetStr);
}

int ExtractVarsFromString(char *Data, char *FormatStr, ListNode *Vars)
{
char *FmtPtr, *MsgPtr, *Token=NULL;
int Match=TRUE, len;

FmtPtr=FormatStr;
if (*FmtPtr=='"') FmtPtr++;
MsgPtr=Data;

while ( (*FmtPtr !=0) && (Match))
{
   switch (*FmtPtr)
   {
      case '?':
        FmtPtr++;
        MsgPtr++;
      break;

      case '*':
        FmtPtr++;
	Token=ExtractVarsGetLiteralString(Token,FmtPtr);
	len=StrLen(Token);
        while (
                (*MsgPtr !=0) && 
                (strncmp(MsgPtr,Token,len) !=0)
              ) MsgPtr++;

      break;

      case '$':
        FmtPtr++;
        ExtractVarsReadVar(&FmtPtr, &MsgPtr, Vars);
      break;

      default:
      if (*FmtPtr != *MsgPtr)
      {
	      Match=FALSE;
      }
        FmtPtr++;
        MsgPtr++;
      break;
   }

}
DestroyString(Token);

return(Match);
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



char *HtmlGetTag(char *Input, char **TagType, char **TagData)
{
char *ptr;
int len=0;

if (! Input) return(NULL);
if (*Input=='\0') return(NULL);
ptr=Input;
*TagType=CopyStr(*TagType,"");
*TagData=CopyStr(*TagData,"");

if (*ptr != '<')
{
len=0;
while ((*ptr != '<') && (*ptr != '\0')) 
{
	*TagData=AddCharToBuffer(*TagData,len,*ptr);
	ptr++;
	len++;
}
return(ptr);
}

if (*ptr=='<') 
{
	ptr++;
	while (isspace(*ptr)) ptr++;

	len=0;
	while ((*ptr != '>') && (! isspace(*ptr)))
	{
		*TagType=AddCharToBuffer(*TagType,len,*ptr);
		len++;
		ptr++;
	}

	while (isspace(*ptr)) ptr++;

	if (*ptr !='>') 
	{
	len=0;
	while ((*ptr != '>') && (*ptr != '\0')) 
	{
		*TagData=AddCharToBuffer(*TagData,len,*ptr);
		len++;
		ptr++;
	}
	}
}

if (*ptr == '>') ptr++;
strlwr(*TagType);

return(ptr);
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
			Output=AddCharToBuffer(Output,len,strtol(Token,NULL,16));
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
			Output=AddCharToBuffer(Output,len,'"');
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
double val;
char kMGT;
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


