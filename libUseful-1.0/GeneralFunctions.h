#ifndef LIBUSEFUL_GENERAL_H
#define LIBUSEFUL_GENERAL_H

#include <stdio.h>
#include "defines.h"

#define LOGFILE_FLUSH 1
#define LOGFILE_SYSLOG 2
#define LOGFILE_LOGPID 4
#define LOGFILE_LOGUSER 8

#define ENCODE_HEX 0
#define ENCODE_BASE64 1

#ifdef __cplusplus
extern "C" {
#endif


int HexStrToBytes(char **Buffer, char *HexStr);
char *BytesToHexStr(char *Buffer, char *Bytes, int len);
int demonize();
int Spawn(char *);
int PipeSpawnFunction(int *infd, int *outfd, int *errfd, BASIC_FUNC Func, void *Data);
int PipeSpawn(int *infd, int *outfd, int *errfd, char *Command);
int FileExists(char *);
int LogToFile(char *,char *,...);
int LogFileSetValues(char *FileName, int Flags, int MaxSize, int FlushInterval);
void LogFileFlushAll(int ForceFlush);
void ColLibDefaultSignalHandler(int sig);
void SetTimeout(int timeout);
int CreateLockFile(char *FilePath,int Timeout);
char *GetDateStr(char *Format, char *Timezone);
char *GetDateStrFromSecs(char *Format, time_t Secs, char *Timezone);
time_t DateStrToSecs(char *Format, char *Str);
double EvaluateMathStr(char *String);
int MakeDirPath(char *Path, int DirMask);

int SwitchUser(char *User);
int SwitchGroup(char *Group);
char *GetCurrUserHomeDir();
char *HashMD5(char *RetStr, char *text, int len, int encoding);
char *EncodeBase64(char *RetStr, char *text, int len);
char *DecodeBase64(char *RetStr, int *len, char *text);
char *GetNameValuePair(const char *Input, const char *PairDelim, const char *NameValueDelim, char **Name, char **Value);

void SetVar(ListNode *Vars, char *Name, char *Data);
char *GetVar(ListNode *Vars, char *Name);
void UnsetVar(ListNode *Vars,char *VarName);
void ClearVars(ListNode *Vars);
void CopyVars(ListNode *Dest,ListNode *Source);

char *SubstituteVarsInString(char *Buffer, char *Fmt, ListNode *Vars, int Flags);
int ExtractVarsFromString(char *Data, char *FormatStr, ListNode *Vars);
void EncodeMD5(char *text, char *md5digest);
char *GetRandomData(char *RetBuff, int len, char *AllowedChars);
char *GetRandomHexStr(char *RetBuff, int len);
char *GetRandomAlphabetStr(char *RetBuff, int len);

void CloseOpenFiles();
int ChangeFileExtension(char *FilePath, char *NewExt);

int BASIC_FUNC_EXEC_COMMAND(void *Data);

char *HtmlGetTag(char *Input, char **TagType, char **TagData);
char *HtmlDeQuote(char *RetStr, char *Data);


double ParseHumanReadableDataQty(char *Data, int Type);
char *GetHumanReadableDataQty(double Size, int Type);



#ifdef __cplusplus
}
#endif


#endif
