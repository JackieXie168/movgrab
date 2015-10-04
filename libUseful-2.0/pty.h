#ifndef LIBUSEFUL_PTY_H
#define LIBUSEFUL_PTY_H

#include "defines.h"

#define TTYFLAG_ECHO 1
#define TTYFLAG_CRLF 2
#define TTYFLAG_LFCR 4
#define TTYFLAG_NONBLOCK 8
#define TTYFLAG_CANON 16
#define TTYFLAG_HARDWARE_FLOW 32
#define TTYFLAG_SOFTWARE_FLOW 64
#define TTYFLAG_CRLF_KEEP 128
#define TTYFLAG_ISIG 256

void HangUpLine(int tty);
int OpenTTY(char *devname, int LineSpeed, int Flags);
void InitTTY(int tty, int LineSpeed, int Flags);
void ResetTTY(int tty);
int GrabPseudoTTY(int *pty, int *tty);
int PseudoTTYSpawnFunction(int *pty, BASIC_FUNC Func, void *Data);
int PseudoTTYSpawn(int *pty, const char *Command);

#endif
