#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include "file.h"
#include "GeneralFunctions.h"
#include "string.h"
#include "pty.h"

void HangUpLine(int tty)
{
int result;
struct termios tty_data, oldtty_data;


result=tcgetattr(tty,&oldtty_data);
result=tcgetattr(tty,&tty_data);
result=cfsetispeed(&tty_data,B0);
result=cfsetospeed(&tty_data,B0);

result=tcsetattr(tty,TCSANOW,&tty_data);

sleep(5);
tcsetattr(tty,TCSANOW,&oldtty_data);
}


void InitTTY(int tty, int LineSpeed, int Flags)
{
struct termios tty_data;
int result;

tcgetattr(tty,&tty_data);
tty_data.c_iflag=IGNBRK | IGNPAR;
if (Flags & TTYFLAG_CRLF) tty_data.c_iflag |= ICRNL;
if (Flags & TTYFLAG_LFCR) tty_data.c_iflag |= INLCR;
tty_data.c_oflag=0 ;
tty_data.c_cflag=CREAD | CS8 | HUPCL;
if (Flags & TTYFLAG_SOFTWARE_FLOW) 
{
tty_data.c_iflag |= IXON | IXOFF;
}
if (Flags & TTYFLAG_HARDWARE_FLOW) tty_data.c_cflag |=CRTSCTS;
if (Flags & TTYFLAG_ECHO) tty_data.c_cflag |= ECHO;
tty_data.c_lflag=0;
tty_data.c_cc[VMIN]=1;
tty_data.c_cc[VTIME]=0;

if (LineSpeed > 0)
{
cfsetispeed(&tty_data,LineSpeed);
cfsetospeed(&tty_data,LineSpeed);
}

tcflush(tty,TCIFLUSH);
result=tcsetattr(tty,TCSANOW,&tty_data);
}


int OpenTTY(char *devname, int LineSpeed, int Flags)
{
int tty;

if (Flags & TTYFLAG_NONBLOCK) tty=open(devname,O_RDWR | O_NOCTTY | O_NDELAY);
else tty=open(devname,O_RDWR | O_NOCTTY);

if ( tty <0) return(-1);
InitTTY(tty, LineSpeed, Flags);
return(tty);
}



int GrabPseudoTTY(int *pty, int *tty)
{
char c1,c2;
char *Buffer=NULL;

//first try unix98 style
*pty=open("/dev/ptmx",O_RDWR);
if (*pty > -1)
{
	 grantpt(*pty);
	 unlockpt(*pty);
   if ( (*tty=open((char *) ptsname(*pty),O_RDWR)) >-1)
		{
			InitTTY(*tty,0,0);
		  return(1);
		}
	
}

//if unix98 fails, try old BSD style

for (c1='p'; c1 < 's'; c1++)
{
  for (c2='5'; c2 <='9'; c2++)
  {
   Buffer=FormatStr(Buffer,"/dev/pty%c%c",c1,c2);
   if ( (*pty=open(Buffer,O_RDWR)) >-1)
   {
      Buffer=FormatStr(Buffer,"/dev/tty%c%c",c1,c2);
      if ( (*tty=OpenTTY(Buffer,0,0)) >-1)
      {
				DestroyString(Buffer);
        return(1);
      }
      else close(*pty);
   }

  }

}

DestroyString(Buffer);
return(0);
}



int PseudoTTYSpawnFunction(int *ret_pty, BASIC_FUNC Func, void *Data)
{
int tty, pty, result, i;
STREAM *S;
char *Tempstr=NULL;

if (GrabPseudoTTY(&pty,&tty))
{
result=fork();
if (result==0)
{
for (i=0; i < 4; i++) close(i);
close(pty);

setsid();
dup(tty);
dup(tty);
dup(tty);
ioctl(tty,TIOCSCTTY,0);
Func((char *) Data);
_exit(0);
}

close(tty);
}

*ret_pty=pty;
return(result);
}


int PseudoTTYSpawn(int *pty, const char *Command)
{
return(PseudoTTYSpawnFunction(pty, BASIC_FUNC_EXEC_COMMAND, (void *) Command));
}
