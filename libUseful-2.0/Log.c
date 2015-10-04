#include "Log.h"
#include <syslog.h>


char *G_LogFilePath;


typedef struct
{
	char *Path;
	int Flags;
	int MaxSize;
	STREAM *S;
	char *Tag;
	int LogFacility;
	int LastFlushTime;
	int FlushInterval;
} TLogFile;


ListNode *LogFiles=NULL;
TLogFile *LogFileDefaults=NULL;


void LogFileSetupDefaults()
{
	LogFileDefaults=(TLogFile *) calloc(1,sizeof(TLogFile));
	LogFileDefaults->MaxSize=100000000;
	LogFileDefaults->Flags |= LOGFILE_FLUSH | LOGFILE_LOGPID | LOGFILE_LOGUSER;
	LogFileDefaults->LogFacility=LOG_USER;
}

TLogFile *LogFileGetEntry(char *FileName)
{
	ListNode *Node;
	TLogFile *LogFile=NULL;
	STREAM *S;

	if (! LogFiles) LogFiles=ListCreate();
	if (! LogFileDefaults) LogFileSetupDefaults();

	Node=ListFindNamedItem(LogFiles,FileName);
	if (Node) LogFile=(TLogFile *) Node->Item;
	else
	{
		if (strcmp(FileName,"STDOUT")==0) S=STREAMFromFD(1);
		else if (strcmp(FileName,"STDERR")==0) S=STREAMFromFD(2);
		else if (strcmp(FileName,"SYSLOG")==0) S=STREAMCreate();
		else
		{
			 S=STREAMOpenFile(FileName,O_CREAT | O_APPEND | O_WRONLY);
		}

		if (S)
		{
			LogFile=(TLogFile *) calloc(1,sizeof(TLogFile));
			LogFile->Path=CopyStr(LogFile->Path,FileName);
			LogFile->LogFacility=LogFileDefaults->LogFacility;
			LogFile->Flags=LogFileDefaults->Flags;
			LogFile->MaxSize=LogFileDefaults->MaxSize;
			LogFile->S=S;
			ListAddNamedItem(LogFiles,FileName,LogFile);
		}
	}

	return(LogFile);
}

void LogFileClose(char *Path)
{
ListNode *Node;
TLogFile *LogFile;

Node=ListFindNamedItem(LogFiles,Path);
if (Node)
{
LogFile=(TLogFile *) Node->Item;
ListDeleteNode(Node);
DestroyString(LogFile->Path);
STREAMClose(LogFile->S);
free(LogFile);
}
}

void LogFileInternalDoRotate(TLogFile *LogFile)
{
  struct stat FStat;
  char *Tempstr=NULL;

	if (! LogFile) return;
  if (LogFile->MaxSize > 0)
  {
  fstat(LogFile->S->out_fd,&FStat);
  if (FStat.st_size > LogFile->MaxSize)
  {
    Tempstr=MCopyStr(Tempstr,LogFile->Path,"-",NULL);
    rename(LogFile->Path,Tempstr);
    STREAMClose(LogFile->S);
    LogFile->S=STREAMOpenFile(LogFile->Path,O_CREAT | O_APPEND | O_WRONLY);
  }
  }

  DestroyString(Tempstr);
}



int LogFileSetValues(char *FileName, int Flags, int MaxSize, int FlushInterval)
{
	TLogFile *LogFile;

	if (! LogFileDefaults) LogFileSetupDefaults();
	if (StrLen(FileName)==0) LogFile=LogFileDefaults;
	else LogFile=LogFileGetEntry(FileName);
	if (LogFile)
	{
		 LogFile->MaxSize=MaxSize;
		  LogFile->FlushInterval=FlushInterval;
		   LogFile->Flags=Flags;
	}
	return(TRUE);
}


int LogFileInternalWrite(STREAM *S,int LogLevel, int Facility, int Flags, char *Tag, char *Str)
{
	char *Tempstr=NULL, *LogStr=NULL;
	time_t Now;
	struct tm *TimeStruct;
	int result=FALSE;


	if (S)
	{
		time(&Now);
		TimeStruct=localtime(&Now);
		LogStr=SetStrLen(LogStr,40);
		strftime(LogStr,20,"%d/%m/%y %H:%M:%S ",TimeStruct);

		if (Flags & LOGFILE_LOGPID)
		{
			Tempstr=FormatStr(Tempstr,"[%d] ",getpid());
			LogStr=CatStr(LogStr,Tempstr);
		}

		if (Flags & LOGFILE_LOGUSER)
		{
			Tempstr=FormatStr(Tempstr,"user=%d ",getuid());
			LogStr=CatStr(LogStr,Tempstr);
		}
		LogStr=MCatStr(LogStr,Str,"\n",NULL);

		if (S)
		{
			STREAMLock(S,LOCK_EX);
			STREAMWriteLine(LogStr,S);
			//if (Flags & LOGFILE_FLUSH) 
			STREAMFlush(S);
			STREAMLock(S,LOCK_UN);
		}

		if (Flags & LOGFILE_SYSLOG)
		{
			if (Facility != LOG_USER)
			{
				openlog(Tag,0,Facility);
				syslog(LOG_INFO,"%s",LogStr);
				closelog();
			}
			else syslog(LOG_INFO,"%s",LogStr);
		}
		result=TRUE;
	}

	DestroyString(Tempstr);
	DestroyString(LogStr);

	return(result);
}






int LogToSTREAM(STREAM *S, int Flags, char *Str)
{
if (! S) return(FALSE);

return(LogFileInternalWrite(S, 0, 0, LOGFILE_FLUSH, "", Str));
}




void LogFileFlushAll(int Force)
{
	time_t Now;
	ListNode *Curr;
	TLogFile *Log;

	time(&Now);

	Curr=ListGetNext(LogFiles);
	while (Curr)
	{
	        Log=(TLogFile *) Curr->Item;

	        if (Force)
	        {
                STREAMFlush(Log->S);
                Log->LastFlushTime=Now;
	        }
	        else if ((Now - Log->LastFlushTime) > Log->FlushInterval)
	        {
                STREAMFlush(Log->S);
                Log->LastFlushTime=Now;
	        }
		Curr=ListGetNext(Curr);
	}
}


int LogToFile(char *FileName,char *fmt, ...)
{
	char *Tempstr=NULL;
	va_list args;
	int result=FALSE;
	TLogFile *LogFile;

	LogFile=LogFileGetEntry(FileName);
	if (LogFile)
	{
	LogFileInternalDoRotate(LogFile);

	va_start(args,fmt);
	Tempstr=VFormatStr(Tempstr,fmt,args);
	va_end(args);
	StripTrailingWhitespace(Tempstr);
	result=LogFileInternalWrite(LogFile->S,LOG_INFO, LogFile->LogFacility, LogFile->Flags, LogFile->Tag, Tempstr);
	}

DestroyString(Tempstr);
return(result);
}

int LogFileAppendTempLog(char *LogPath, char *TmpLogPath)
{
TLogFile *LogFile;
char *Tempstr=NULL;
STREAM *S;

    LogFile=LogFileGetEntry(LogPath);
    LogFileClose(TmpLogPath);
    S=STREAMOpenFile(TmpLogPath,O_RDONLY);
    if (LogFile && S)
    {

            STREAMLock(LogFile->S,LOCK_EX);
            Tempstr=STREAMReadLine(Tempstr,S);
            while(Tempstr)
            {
            STREAMWriteLine(Tempstr,LogFile->S);
            Tempstr=STREAMReadLine(Tempstr,S);
            }
            if (LogFile->Flags & LOGFILE_FLUSH) STREAMFlush(LogFile->S);
            STREAMLock(LogFile->S,LOCK_UN);
            unlink(TmpLogPath);
    }

DestroyString(Tempstr);
}

