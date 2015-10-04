#ifndef LIBUSEFUL_SPAWN_H
#define LIBUSEFUL_SPAWN_H

#include "includes.h"

#ifdef __cplusplus
extern "C" {
#endif

int ForkWithContext();
/* This function turns our process into a demon */
int demonize();
int ForkWithIO(int StdIn, int StdOut, int StdErr);
int SpawnWithIO(char *CommandLine, int StdIn, int StdOut, int StdErr);
int Spawn(char *ProgName);
/* This creates a child process that we can talk to using a couple of pipes*/
int PipeSpawnFunction(int *infd,int  *outfd,int  *errfd, BASIC_FUNC Func, void *Data );
int PipeSpawn(int *infd,int  *outfd,int  *errfd, char *Command);
int PseudoTTYSpawn(int *pty, const char *Command);
STREAM *STREAMSpawnCommand(const char *Command, int Type);

#ifdef __cplusplus
}
#endif


#endif
