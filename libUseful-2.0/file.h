#ifndef LIBUSEFUL_FILE_H
#define LIBUSEFUL_FILE_H

#include <fcntl.h>
#include "list.h"


#define STREAM_CLOSED -1
#define STREAM_NODATA -2
#define STREAM_TIMEOUT -3
#define STREAM_DATA_ERROR -4

//Flags that alter stream behavior
#define FLUSH_FULL 0
#define FLUSH_LINE 1
#define FLUSH_BLOCK 2
#define FLUSH_ALWAYS 4
#define SF_RDONLY 8
#define SF_WRONLY 16
#define SF_RDWR 32
#define SF_NONBLOCK 65
#define SF_SYMLINK_OK 128
#define SF_SSL 256
#define SF_AUTH 512

//Stream state values
#define SS_CONNECTING 1
#define SS_CONNECTED 2
#define SS_HANDSHAKE_DONE 4
#define SS_DATA_ERROR 8
#define SS_WRITE_ERROR 16
#define SS_AUTH 8192

#define STREAM_TYPE_FILE 0
#define STREAM_TYPE_UNIX 1
#define STREAM_TYPE_UNIX_DGRAM 2
#define STREAM_TYPE_TCP 3
#define STREAM_TYPE_UDP 4
#define STREAM_TYPE_SSL 5
#define STREAM_TYPE_HTTP 6
#define STREAM_TYPE_CHUNKED_HTTP 7

#define O_LOCK O_NOCTTY

#define COMMS_BY_PIPE 0
#define COMMS_BY_PTY 1

#define SELECT_READ 1
#define SELECT_WRITE 2	

typedef struct
{
int Type;
int in_fd, out_fd;
unsigned int Flags;
unsigned int State;
unsigned int Timeout;
unsigned int BlockSize;
unsigned int BuffSize;

unsigned int InStart, InEnd;
unsigned int OutEnd;
char *InputBuff;
char *OutputBuff;

unsigned int BytesRead;
unsigned int BytesWritten;
char *Path;
ListNode *ProcessingModules;
ListNode *Values;
ListNode *Items;
} STREAM;


#ifdef __cplusplus
extern "C" {
#endif


int FDSelect(int fd, int Flags, struct timeval *tv);
int FDIsWritable(int);
int FDCheckForBytes(int);

void STREAMSetNonBlock(STREAM *S, int val);
void STREAMSetTimeout(STREAM *, int);
void STREAMSetFlushType(STREAM *Stream, int Type, int val);


STREAM *STREAMCreate();
STREAM *STREAMOpenFile(const char *Path, int Mode);
STREAM *STREAMClose(STREAM *Stream);
int STREAMLock(STREAM *S, int val);
int STREAMFlush(STREAM *Stream);
void STREAMClear(STREAM *Stream);
double STREAMTell(STREAM *Stream);
double STREAMSeek(STREAM *Stream, double, int whence);
void STREAMResizeBuffer(STREAM *, int);
int STREAMReadChar(STREAM *);
int STREAMWriteChar(STREAM *, char);
char* STREAMReadLine(char *Buffer, STREAM *);
int ReadBytesToTerm(STREAM *S, char *Buffer, int BuffSize, char Term);
char* STREAMReadToTerminator(char *Buffer, STREAM *, char Term);
char* STREAMReadToMultiTerminator(char *Buffer, STREAM *, char *Terms);
int STREAMWriteString(const char *Buffer, STREAM *);
int STREAMWriteLine(const char *Buffer, STREAM *);
STREAM *STREAMFromFD(int fd);
STREAM *STREAMFromDualFD(int in_fd, int out_fd);
STREAM *STREAMSpawnCommand(const char *Command, int type);

int STREAMDisassociateFromFD(STREAM *Stream);
int STREAMPeekChar(STREAM *);

int STREAMReadBytes(STREAM *, char *Buffer, int Bytes);
int STREAMWriteBytes(STREAM *, const char *Buffer, int Bytes);
int STREAMCheckForBytes(STREAM *);
int STREAMCheckForWaitingChar(STREAM *S, char check_char);
int STREAMCountWaitingBytes(STREAM *);
STREAM *STREAMSelect(ListNode *Streams);

void STREAMSetValue(STREAM *S, const char *Name, const char *Value);
char *STREAMGetValue(STREAM *S, const char *Name);
void STREAMSetItem(STREAM *S, const char *Name, void *Item);
void *STREAMGetItem(STREAM *S, const char *Name);


#ifdef __cplusplus
}
#endif



#endif
