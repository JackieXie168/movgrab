#include <sys/types.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <fcntl.h>
#include "file.h"
#include "GeneralFunctions.h"
#include "string.h"
#include "pty.h"

ListNode *TTYAttribs=NULL;

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

void ResetTTY(int tty)
{
struct termios *tty_data;
char *Tempstr=NULL;
ListNode *Curr;

Tempstr=FormatStr(Tempstr,"%d",tty);
Curr=ListFindNamedItem(TTYAttribs,Tempstr);
if (Curr)
{
	tty_data=(struct termios *) Curr->Item;
	tcsetattr(tty,TCSANOW,tty_data);
	ListDeleteNode(Curr);
	free(tty_data);
}

DestroyString(Tempstr);
}

void InitTTY(int tty, int LineSpeed, int Flags)
{
struct termios tty_data, *old_tty_data;
int result, val;
char *Tempstr=NULL;
ListNode *Curr;

Tempstr=FormatStr(Tempstr,"%d",tty);
if (! TTYAttribs) TTYAttribs=ListCreate();
Curr=ListFindNamedItem(TTYAttribs,Tempstr);

if (! Curr)
{
old_tty_data=(struct termios *) calloc(1,sizeof(struct termios));
ListAddNamedItem(TTYAttribs,Tempstr,old_tty_data);
}
else old_tty_data=(struct termios *) Curr->Item;

tcgetattr(tty,old_tty_data);
tcgetattr(tty,&tty_data);

//ignore break characters and parity errors
tty_data.c_iflag=IGNBRK | IGNPAR;

//Enable Special Characters
if (Flags & TTYFLAG_CANON) tty_data.c_iflag|= ICANON;
//else tty_data.c_iflag &= ~ICANON;


/*
if (! (Flags & TTYFLAG_CRLF_KEEP))
{
	//translate carriage-return to newline
	if (Flags & TTYFLAG_CRLF) tty_data.c_iflag |= ICRNL;
	else tty_data.c_iflag &= ~ICRNL;

	//translate newline to carriage return
	if (Flags & TTYFLAG_LFCR) 
	{
		tty_data.c_iflag |= INLCR;
	}
	else tty_data.c_iflag &= ~INLCR;

	//output flags
	tty_data.c_oflag=0;
	//postprocess and translate newline to cr-nl
	if (Flags & TTYFLAG_LFCR) tty_data.c_oflag |= ONLCR | OPOST;
}
*/

tty_data.c_cflag=CREAD | CS8 | HUPCL | CLOCAL;
if (Flags & TTYFLAG_SOFTWARE_FLOW) 
{
tty_data.c_iflag |= IXON | IXOFF;
}
if (Flags & TTYFLAG_HARDWARE_FLOW) tty_data.c_cflag |=CRTSCTS;
if (Flags & TTYFLAG_ECHO) tty_data.c_cflag |= ECHO;

tty_data.c_lflag=0;
if (Flags & TTYFLAG_ISIG) tty_data.c_lflag=ISIG;

tty_data.c_cc[VMIN]=1;
tty_data.c_cc[VTIME]=0;

if (LineSpeed > 0)
{
switch (LineSpeed)
{
case 2400: val=B2400; break;
case 4800: val=B4800; break;
case 9600: val=B9600; break;
case 19200: val=B19200; break;
case 38400: val=B38400; break;
case 57600: val=B57600; break;
case 230400: val=B230400; break;
#ifdef B460800
case 460800: val=B460800; break;
#endif
#ifdef B500000
case 500000: val=B500000; break;
#endif
#ifdef B1000000
case 10000000: val=B1000000; break;
#endif
#ifdef B1152000
case 1152000: val=B1152000; break;
#endif
#ifdef B2000000
case 2000000: val=B2000000; break;
#endif
#ifdef B4000000
case 4000000: val=B4000000; break;
#endif
default: val=B115200; break;
}
cfsetispeed(&tty_data,val);
cfsetospeed(&tty_data,val);
}

tcflush(tty,TCIFLUSH);
result=tcsetattr(tty,TCSANOW,&tty_data);

DestroyString(Tempstr);
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
