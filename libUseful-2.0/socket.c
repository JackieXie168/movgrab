#include "includes.h"

#include <arpa/inet.h>

#ifdef HAVE_LIBSSL
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#endif

char *HopTypes[]={"none","direct","http_proxy","ssh","shell","telnet",NULL};


int IsSockConnected(int sock)
{
struct sockaddr_in sa;
int salen, result;

if (sock==-1) return(FALSE);
salen=sizeof(sa);
result=getpeername(sock,(struct sockaddr *) &sa, &salen);
if (result==0) return(TRUE);
if (errno==ENOTCONN) return(SOCK_CONNECTING);
return(FALSE);
}


int InitServerSock(char *Address, int Port)
{
int sock;
struct sockaddr_in sa;
int salen;
int result;

sock=socket(AF_INET,SOCK_STREAM,0);
if (sock <0) return(-1);

result=1;
salen=sizeof(result);
setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&result,salen);

sa.sin_port=htons(Port);
sa.sin_family=AF_INET;
if (StrLen(Address) > 0) sa.sin_addr.s_addr=StrtoIP(Address);
else sa.sin_addr.s_addr=INADDR_ANY;

salen=sizeof(struct sockaddr_in);
result=bind(sock,(struct sockaddr *) &sa, salen);

if (result==0)
{
 result=listen(sock,10);
}

if (result==0) return(sock);
else 
{
close(sock);
return(-1);
}
}



int InitUnixServerSock(char *Path)
{
int sock;
struct sockaddr_un sa;
int salen;
int result;

sock=socket(AF_UNIX,SOCK_STREAM,0);
if (sock <0) return(-1);
result=1;
salen=sizeof(result);
strcpy(sa.sun_path,Path);
sa.sun_family=AF_UNIX;
salen=sizeof(struct sockaddr_un);
result=bind(sock,(struct sockaddr *) &sa, salen);

if (result==0)
{
 result=listen(sock,10);
}

if (result==0) return(sock);
else 
{
close(sock);
return(-1);
}
}


int TCPServerSockAccept(int ServerSock, int *Addr)
{
struct sockaddr_in sa;
int salen;
int sock;

salen=sizeof(sa);
sock=accept(ServerSock,(struct sockaddr *) &sa,&salen);
*Addr=sa.sin_addr.s_addr;
return(sock);
}

int UnixServerSockAccept(int ServerSock)
{
struct sockaddr_un sa;
int salen;
int sock;

salen=sizeof(sa);
sock=accept(ServerSock,(struct sockaddr *) &sa,&salen);
return(sock);
}





int IsAddress(char *Str)
{
int len,count;
len=StrLen(Str);
if (len <1) return(FALSE);
for (count=0; count < len; count++)
{
 if ((! isdigit(Str[count])) && (Str[count] !='.')) 
 {
	return(FALSE);
 }
}

return(TRUE);
}


int GetSockDetails(int sock, char **LocalAddress, int *LocalPort, char **RemoteAddress, int *RemotePort)
{
int salen, result;
struct sockaddr_in sa;

*LocalPort=0;
*RemotePort=0;
*LocalAddress=CopyStr(*LocalAddress,"");
*RemoteAddress=CopyStr(*RemoteAddress,"");

salen=sizeof(struct sockaddr_in);
result=getsockname(sock, (struct sockaddr *) &sa, &salen);

if (result==0)
{
	*LocalAddress=CopyStr(*LocalAddress,IPtoStr(sa.sin_addr.s_addr));
	*LocalPort=ntohs(sa.sin_port);

	//Set Address to be the same as control sock, as it might not be INADDR_ANY
	result=getpeername(sock, (struct sockaddr *) &sa, &salen);

	if (result==0)
	{
		*RemoteAddress=CopyStr(*RemoteAddress,IPtoStr(sa.sin_addr.s_addr));
		*RemotePort=sa.sin_port;
	}

	//We've got the local sock, so lets still call it a success
	result=0;
}

if (result==0) return(TRUE);
return(FALSE);
}






/* Users will probably only use this function if they want to reconnect   */
/* a broken connection, or reuse a socket for multiple connections, hence */
/* the name... */
int ReconnectSock(int sock, char *Host, int Port, int Flags)
{
int salen, result;
struct sockaddr_in sa;
struct hostent *hostdata;

sa.sin_family=AF_INET;
sa.sin_port=htons(Port);

if (IsAddress(Host))
{
inet_aton(Host, (struct in_addr *) &sa.sin_addr);
}
else 
{ 
   hostdata=gethostbyname(Host);
   if (!hostdata) 
   {
     return(-1);
   }
sa.sin_addr=*(struct in_addr *) *hostdata->h_addr_list;
}

salen=sizeof(sa);
if (Flags & CONNECT_NONBLOCK) 
{
fcntl(sock,F_SETFL,O_NONBLOCK);
}

result=connect(sock,(struct sockaddr *)&sa, salen);
if (result==0) result=TRUE;
if ((result==-1) && (Flags & CONNECT_NONBLOCK) && (errno == EINPROGRESS)) result=FALSE;
return(result);
}


int ConnectToHost(char *Host, int Port,int Flags)
{
int sock, result;

sock=socket(AF_INET,SOCK_STREAM,0);
if (sock <0) return(-1);
result=ReconnectSock(sock,Host,Port,Flags);
if (result==-1)
{
close(sock);
return(-1);
}

return(sock);

}




int CheckForTerm(DownloadContext *CTX, char inchar)
{

    if (inchar == CTX->TermStr[CTX->TermPos]) 
    {
        CTX->TermPos++;
        if (CTX->TermPos >=strlen(CTX->TermStr)) 
        {
           CTX->TermPos=0;
           return(TRUE);
        }
    }
    else
    {
      if (CTX->TermPos >0) 
      {
         STREAMWriteBytes(CTX->Output, CTX->TermStr,CTX->TermPos);
         CTX->TermPos=0;
         return(CheckForTerm(CTX,inchar));
      }
    }
return(FALSE); 
}



int  ProcessIncommingBytes(DownloadContext *CTX)
{
int inchar, FoundTerm=FALSE, err;

 inchar=STREAMReadChar(CTX->Input);

 if (inchar==EOF) return(TRUE);

 FoundTerm=CheckForTerm(CTX,(char) inchar);
 while ((inchar !=EOF) && (! FoundTerm))
 {
     if (CTX->TermPos==0) STREAMWriteChar(CTX->Output, (char) inchar);
     inchar=STREAMReadChar(CTX->Input);
     err=errno;
     FoundTerm=CheckForTerm(CTX, (char) inchar);
 }
if (inchar==EOF) return(TRUE);
if (FoundTerm) return(TRUE);
return(FALSE);
}


int DownloadToTermStr2(STREAM *Connection, STREAM *SaveFile, char *TermStr)
{
DownloadContext CTX;

CTX.TermStr=CopyStr(NULL,TermStr);
CTX.Input=Connection;
CTX.Output=SaveFile;
CTX.TermPos=0;

while(ProcessIncommingBytes(&CTX) !=TRUE);

DestroyString(CTX.TermStr);
return(TRUE);
}

int DownloadToDot(STREAM *Connection, STREAM *SaveFile)
{
DownloadToTermStr2(Connection,SaveFile,"\r\n.\r\n");
}


int DownloadToTermStr(STREAM *Connection, STREAM *SaveFile, char *TermStr)
{
char *Tempstr=NULL;

Tempstr=STREAMReadLine(Tempstr,Connection);
while (Tempstr)
{
  if (strcmp(Tempstr,TermStr)==0)
{
 break;
}
  STREAMWriteLine(Tempstr,SaveFile);
  Tempstr=STREAMReadLine(Tempstr,Connection);
}
return(TRUE);
}

char *LookupHostIP(char *Host)
{
struct hostent *hostdata;

   hostdata=gethostbyname(Host);
   if (!hostdata) 
   {
     return(NULL);
   }

//inet_ntoa shouldn't need this cast to 'char *', but it emitts a warning
//without it
return((char *) inet_ntoa(*(struct in_addr *) *hostdata->h_addr_list));
}


char *GetRemoteIP(int sock)
{
struct sockaddr_in sa;
int salen, result;

salen=sizeof(struct sockaddr_in);
result=getpeername(sock,(struct sockaddr *) &sa, &salen);
if  (result==-1)  
{
if (errno==ENOTSOCK)  return("127.0.0.1");
else return("0.0.0.0");
}

return((char *) inet_ntoa(sa.sin_addr));
}


char *IPStrToHostName(char *IPAddr)
{
struct sockaddr_in sa;
struct hostent *hostdata=NULL;

inet_aton(IPAddr,& sa.sin_addr);
hostdata=gethostbyaddr(&sa.sin_addr.s_addr,sizeof((sa.sin_addr.s_addr)),AF_INET);
if (hostdata) return(hostdata->h_name);
else return("");
}




char *IPtoStr(unsigned long Address)
{
struct sockaddr_in sa;
sa.sin_addr.s_addr=Address;
return((char *) inet_ntoa(sa.sin_addr));

}

unsigned long StrtoIP(char *Str)
{
struct sockaddr_in sa;
if (inet_aton(Str,&sa.sin_addr)) return(sa.sin_addr.s_addr);
return(0);
}


int STREAMIsConnected(STREAM *S)
{
int result=FALSE;

if (! S) return(FALSE);
result=IsSockConnected(S->in_fd);
if (result==TRUE)
{
	if (S->Flags & SF_CONNECTING)
	{
		S->Flags |= SF_CONNECTED;
		S->Flags &= (~SF_CONNECTING);
	}
}
if ((result==SOCK_CONNECTING) && (! (S->Flags & SF_CONNECTING))) result=FALSE;
return(result);
}


int DoHTTPProxyTunnel(STREAM *S, char *Host, int Port, int Flags)
{
char *Tempstr=NULL, *Token=NULL, *ptr=NULL;
int result=FALSE;

	if (Flags & CONNECT_SSL) Tempstr=FormatStr(Tempstr,"CONNECT https://%s:%d HTTP/1.1\r\n\r\n",Host,Port);
	else Tempstr=FormatStr(Tempstr,"CONNECT http://%s:%d HTTP/1.1\r\n\r\n",Host,Port);
	STREAMWriteLine(Tempstr,S);
	STREAMFlush(S);
	
	Tempstr=STREAMReadLine(Tempstr,S);
	StripTrailingWhitespace(Tempstr);

	ptr=GetToken(Tempstr," ",&Token,0);
	ptr=GetToken(ptr," ",&Token,0);

	if (*Token==2) result=TRUE;
	while (StrLen(Tempstr))
	{
		Tempstr=STREAMReadLine(Tempstr,S);
		StripTrailingWhitespace(Tempstr);
	}

DestroyString(Tempstr);
DestroyString(Token);

return(result);
}


int STREAMDoPostConnect(STREAM *S, int Flags)
{
int result=TRUE;
char *ptr;

//if (Flags & CONNECT_SOCKS_PROXY) result=DoSocksProxyTunnel(S);
if (Flags & CONNECT_SSL) DoSSLClientNegotiation(S, Flags);

ptr=GetRemoteIP(S->in_fd);
if (ptr) STREAMSetValue(S,"PeerIP",ptr);

return(result);
}


int STREAMInternalConnect(STREAM *S, char *Host, int Port,int Flags)
{
int val, result=FALSE;

S->in_fd=ConnectToHost(Host,Port,Flags);
S->out_fd=S->in_fd;

if (Flags & CONNECT_NONBLOCK) S->Flags |= SF_NONBLOCK;


if (S->in_fd > -1)
{
S->Type=STREAM_TYPE_TCP;
result=TRUE;
STREAMSetFlushType(S,FLUSH_LINE,0);
}

return(result);
}



int SendPublicKeyToRemote(STREAM *S, char *KeyFile, char *LocalPath)
{
char *Tempstr=NULL, *RetStr=NULL, *Line=NULL;
STREAM *LocalFile;


Tempstr=FormatStr(Tempstr,"rm -f %s ; touch %s; chmod 0600 %s\n",KeyFile,KeyFile,KeyFile);
STREAMWriteLine(Tempstr,S);
LocalFile=STREAMOpenFile(LocalPath,O_RDONLY);
if (LocalFile)
{
Line=STREAMReadLine(Line,LocalFile);
while (Line)
{
StripTrailingWhitespace(Line);
Tempstr=FormatStr(Tempstr,"echo '%s' >> %s\n",Line,KeyFile);
STREAMWriteLine(Tempstr,S);
Line=STREAMReadLine(Line,LocalFile);
}
STREAMClose(LocalFile);
}

return(TRUE);
}



void ParseHostDetails(char *Data,char **Host,int *Port,char **User)
{
char *ptr=NULL;

ptr=strrchr(Data,'@');
if (ptr)
{
 *User=CopyStrLen(*User,Data,ptr-Data);
ptr++;
}
else ptr=Data;

ptr=GetToken(ptr,":",Host,0);
if (StrLen(ptr)) *Port=atoi(ptr);
}


void ParseConnectDetails(char *Str, char **Type, char **Host, int *Port, char **User, char **Pass, char **InitDir)
{
char *ptr, *ptr2, *Token=NULL, *Tmp=NULL;

ptr=GetToken(Str," ",&Token,0);
while (ptr)
{
if (strcmp(Token,"-password")==0)
{
ptr=GetToken(ptr," ",Pass,0);
}
else
{
ptr2=GetToken(Token,":",Type,0);
while (*ptr2=='/') ptr2++;
ptr2=GetToken(ptr2,"/",&Tmp,0);

ParseHostDetails(Tmp,Host,Port,User);

//Now we break Tmp up into host, port and path

if (StrLen(ptr2)) *InitDir=MCopyStr(*InitDir,"/",ptr2,NULL);
}

ptr=GetToken(ptr," ",&Token,0);
}

DestroyString(Token);
DestroyString(Tmp);
}



void ParseConnectHop(char *Line, int *Type,  char **Host, char **User, char **Password, char **KeyFile, int *Port)
{
char *ptr, *ptr2, *Token=NULL, *Trash=NULL;
int result;

ParseConnectDetails(Line, &Token, Host, Port, User, Password, &Trash);
ptr=GetToken(Line,":",&Token,GETTOKEN_QUOTES);
*Type=MatchTokenFromList(Token,HopTypes,0);

DestroyString(Token);
DestroyString(Trash);
}


int STREAMProcessConnectHop(STREAM *S, char *Value, int LastHop)
{
int val, result=FALSE;
char *Token=NULL, *Token2=NULL, *ptr;
char *Tempstr=NULL;
char *User=NULL, *Host=NULL,*Pass=NULL, *KeyFile=NULL;
int Port=0;

ParseConnectHop(Value, &val,  &Host, &User, &Pass, &KeyFile, &Port);
switch (val)
{
	case CONNECT_HOP_TCP:
		if (S->in_fd==-1)
		{
			if (STREAMInternalConnect(S,Host,Port,0)) result=TRUE;
		}
		break;

	case CONNECT_HOP_HTTP_PROXY:
		result=DoHTTPProxyTunnel(S, Host, Port, 0);
		break;	

	case CONNECT_HOP_SSH:
//		if ((StrLen(KeyFile)==0) && (StrLen(Pass) > 0)) Tempstr=CopyStr(Tempstr,"ssh -2 -e none ");
//		else 

		Tempstr=CopyStr(Tempstr,"ssh -2 -T ");
		if (StrLen(KeyFile))
		{

		if (S->in_fd != -1)
		{
			Token=FormatStr(Token,".%d-%d",getpid(),time(NULL));
			SendPublicKeyToRemote(S,Token,KeyFile);
			KeyFile=CopyStr(KeyFile,Token);
		}

			Tempstr=CatStr(Tempstr,"-i ");
			Tempstr=CatStr(Tempstr,KeyFile);
			Tempstr=CatStr(Tempstr," ");

		}

		if (Port > 0)
		{
		Token=FormatStr(Token," -p %d ",Port);
		Tempstr=CatStr(Tempstr,Token);
		}

		Token=FormatStr(Token,"%s@%s",User,Host);
		Tempstr=CatStr(Tempstr,Token);

		if (S->in_fd==-1) 
		{
			Tempstr=CatStr(Tempstr, " 2> /dev/null");
			PseudoTTYSpawn(&S->in_fd,Tempstr);
			S->out_fd=S->in_fd;
			if (S->in_fd > -1)
			{
				result=TRUE;
				STREAMSetFlushType(S,FLUSH_LINE,0);
			}
		}
		else 
		{
			if (StrLen(KeyFile))
			{
			Tempstr=CatStr(Tempstr," ; rm -f ");
			Tempstr=CatStr(Tempstr,KeyFile);
			}
			Tempstr=CatStr(Tempstr,"; exit\n");
			STREAMWriteLine(Tempstr,S);
			result=TRUE;
		}

		if ((StrLen(KeyFile)==0) && (StrLen(Pass) > 0)) 
		{
			Token=CopyStr(Token,Pass);
			Token=CatStr(Token,"\n");
			for (val=0; val < 3; val++)
			{
			if (STREAMExpectAndReply(S,"assword:",Token)) break;
			}
		}
		STREAMSetTimeout(S,1);
		STREAMExpectSilence(S);
		break;

	case CONNECT_HOP_SHELL_CMD:
		break;

	case CONNECT_HOP_TELNET:
		if (Port > 0)
		{
		Tempstr=FormatStr(Tempstr,"telnet -8 %s %d ",Host, Port);
		}
		else Tempstr=FormatStr(Tempstr,"telnet -8 %s ",Host);

		if (S->in_fd==-1) 
		{
			PseudoTTYSpawn(& S->in_fd,Tempstr);
		        S->out_fd=S->in_fd;
			if (S->in_fd > -1)
			{
				result=TRUE;
				STREAMSetFlushType(S,FLUSH_LINE,0);
			}

		}
		else 
		{
			Tempstr=CatStr(Tempstr,";exit\n");
			STREAMWriteLine(Tempstr,S);
			result=TRUE;
		}
		if (StrLen(User) > 0) 
		{
			Tempstr=MCopyStr(Tempstr,User,"\n",NULL);
			STREAMExpectAndReply(S,"ogin:",Tempstr);
		}
		if (StrLen(Pass) > 0) 
		{
			Tempstr=MCopyStr(Tempstr,Pass,"\n",NULL);
			STREAMExpectAndReply(S,"assword:",Tempstr);
		}
		STREAMSetTimeout(S,2);
		STREAMExpectSilence(S);
		break;


}

DestroyString(Tempstr);
DestroyString(Token);
DestroyString(KeyFile);
DestroyString(Host);
DestroyString(User);
DestroyString(Pass);

STREAMSetTimeout(S,30);
STREAMFlush(S);
return(result);
}


/*
int STREAMInternalLastHop(STREAM *S,char *DesiredHost,int DesiredPort, char *LastHop)
{
int result, Type,Port;
char *Host=NULL, *User=NULL, *Pass=NULL, *KeyFile=NULL;

ParseConnectHop(LastHop, &Type,  &Host, &User, &Pass, &KeyFile, &Port);
switch (Type)
{

}
result=STREAMProcessConnectHop(S, Tempstr, TRUE);

DestroyString(Tempstr);
DestroyString(Host);
DestroyString(User);
DestroyString(Pass);
DestroyString(KeyFile);
return(result);
}
*/

int STREAMConnectToHost(STREAM *S, char *DesiredHost, int DesiredPort,int Flags)
{
ListNode *Curr;
char *Token=NULL, *ptr;
int result=FALSE;
int HopNo=0;
ListNode *LastHop=NULL;

//Find the last hop, used to decide what ssh command to use
Curr=ListGetNext(S->Values);
while (Curr)
{
ptr=GetToken(Curr->Tag,":",&Token,0);
if (strcasecmp(Token,"ConnectHop")==0) LastHop=Curr;
Curr=ListGetNext(Curr);
}

STREAMSetFlushType(S,FLUSH_LINE,0);
Curr=ListGetNext(S->Values);
while (Curr)
{
ptr=GetToken(Curr->Tag,":",&Token,0);

if (strcasecmp(Token,"ConnectHop")==0) result=STREAMProcessConnectHop(S, (char *) Curr->Item,Curr==LastHop);
HopNo++;

if (! result) break;
Curr=ListGetNext(Curr);
}

if (StrLen(DesiredHost))
{
//	if (LastHop) result=STREAMInternalLastHop(S,DesiredHost,DesiredPort,Flags);
//	else
 result=STREAMInternalConnect(S,DesiredHost,DesiredPort,Flags);
}

if (result==TRUE)
{
    if (Flags & CONNECT_NONBLOCK) S->Flags |=SF_CONNECTING | SF_NONBLOCK;
	else
	{
		S->Flags |=SF_CONNECTED;
		STREAMDoPostConnect(S, Flags);
	}
}


return(result);
}




#ifdef HAVE_LIBSSL
void STREAM_INTERNAL_SSL_ADD_SECURE_KEYS(STREAM *S, SSL_CTX *ctx)
{
ListNode *Curr;

Curr=ListGetNext(S->Values);
while (Curr)
{
  if ((StrLen(Curr->Tag)) && (strncasecmp(Curr->Tag,"SSL_CERT_FILE:",14)==0))
  {
	  SSL_CTX_use_certificate_file(ctx,(char *) Curr->Item,SSL_FILETYPE_PEM);
  }

  if ((StrLen(Curr->Tag)) && (strncasecmp(Curr->Tag,"SSL_KEY_FILE:",13)==0))
  {
	  SSL_CTX_use_PrivateKey_file(ctx,(char *) Curr->Item,SSL_FILETYPE_PEM);
  }

  if ((StrLen(Curr->Tag)) && (strncasecmp(Curr->Tag,"SSL_VERIFY_CERTDIR",18)==0))
  {
	  SSL_CTX_load_verify_locations(ctx,NULL,(char *) Curr->Item);
  }

	if ((StrLen(Curr->Tag)) && (strncasecmp(Curr->Tag,"SSL_VERIFY_CERTFILE",19)==0))
	{
		SSL_CTX_load_verify_locations(ctx,(char *) Curr->Item,NULL);
	}

  Curr=ListGetNext(Curr);
}

}
#endif


void HandleSSLError()
{
int val;

#ifdef HAVE_LIBSSL
	  val=ERR_get_error();
	  fprintf(stderr,"Failed to create SSL_CTX: %s\n",ERR_error_string(val,NULL));
	  fflush(NULL);
#endif
}


#ifdef HAVE_LIBSSL
int INTERNAL_SSL_INIT()
{
static int InitDone=FALSE;

if (InitDone) return(TRUE);
  SSL_library_init();
#ifdef USE_OPENSSL_ADD_ALL_ALGORITHMS
  OpenSSL_add_all_algorithms();
#endif
  SSL_load_error_strings();

  InitDone=TRUE;
  return(TRUE);
}
#endif

int DoSSLClientNegotiation(STREAM *S, int Flags)
{
int result=FALSE, val;
#ifdef HAVE_LIBSSL
SSL_METHOD *Method;
SSL_CTX *ctx;
SSL *ssl;

if (S)
{
INTERNAL_SSL_INIT();
//  SSL_load_ciphers();
  Method=SSLv23_client_method();
  if (! Method) Method=SSLv2_client_method();
  ctx=SSL_CTX_new(Method);
  if (! ctx) HandleSSLError();
  else
  {
  STREAM_INTERNAL_SSL_ADD_SECURE_KEYS(S,ctx);
  ssl=SSL_new(ctx);
  SSL_set_fd(ssl,S->in_fd);
  S->Extra=ssl;
  result=SSL_connect(ssl);
  S->Flags|=SF_SSL;
  }

}

#endif
return(result);
}


int DoSSLServerNegotiation(STREAM *S, int Flags)
{
int result=FALSE;
#ifdef HAVE_LIBSSL
SSL_METHOD *Method;
SSL_CTX *ctx;
SSL *ssl;


if (S)
{
INTERNAL_SSL_INIT();
  Method=SSLv23_server_method();
  if (! Method) Method=SSLv2_server_method();
  if (Method)
  {
  ctx=SSL_CTX_new(Method);
	  
  if (ctx)
 {
  STREAM_INTERNAL_SSL_ADD_SECURE_KEYS(S,ctx);
  ssl=SSL_new(ctx);
  SSL_set_fd(ssl,S->in_fd);
  S->Extra=ssl;
  SSL_set_verify(ssl,SSL_VERIFY_NONE,NULL);
  SSL_set_accept_state(ssl);
  result=SSL_accept(ssl);
  if (result != TRUE)
  {
	 result=SSL_get_error(ssl,result);
	 result=ERR_get_error();
	 fprintf(stderr,"error: %s\n",ERR_error_string(result,NULL));
	 result=FALSE;
  }
  S->Flags|=SF_SSL;
  }
  }
}

#endif
return(result);
}


const char *STREAMQuerySSLCipher(STREAM *S)
{
if (! S) return(NULL);
if (! S->Extra) return(NULL);
#ifdef HAVE_LIBSSL
return(SSL_get_cipher((SSL *) S->Extra));
#else
return(NULL);
#endif
}


int STREAMIsPeerAuth(STREAM *S)
{
#ifdef HAVE_LIBSSL
if (SSL_get_verify_result((SSL *) S->Extra)==X509_V_OK)
{
  if (SSL_get_peer_certificate((SSL *) S->Extra) !=NULL) return(TRUE);
}
#endif
return(FALSE);
}


char *STREAMGetValue(STREAM *S, char *Name)
{
char *ptr;
ListNode *Curr;

if (! S->Values) return(NULL);
Curr=ListFindNamedItem(S->Values,Name);
if (Curr) return(Curr->Item);
return(NULL);
}


void STREAMSetValue(STREAM *S, char *Name, char *Value)
{
char *ptr;
ListNode *Curr;

if (! S->Values) S->Values=ListCreate();
SetVar(S->Values,Name,Value);
}



int OpenUDPSock(int Port)
{
	int result;
	struct sockaddr_in addr;
	int fd;

	addr.sin_family=AF_INET;
//	addr.sin_addr.s_addr=Interface;
	addr.sin_addr.s_addr=INADDR_ANY;
	addr.sin_port=htons(Port);

	fd=socket(AF_INET, SOCK_DGRAM,0);
	result=bind(fd,(struct sockaddr *) &addr, sizeof(addr));
        if (result !=0)
        {
		close(fd);
		return(-1);
        }
   return(fd);
}


int STREAMSendDgram(STREAM *S, char *Host, int Port, char *Bytes, int len)
{
struct sockaddr_in sa;
int salen;
struct hostent *hostdata;

sa.sin_port=htons(Port);
sa.sin_family=AF_INET;
inet_aton(Host,& sa.sin_addr);
salen=sizeof(sa);

if (IsAddress(Host))
{
   inet_aton(Host, (struct in_addr *) &sa.sin_addr);
}
else 
{ 
   hostdata=gethostbyname(Host);
   if (!hostdata) 
   {
     return(-1);
   }
sa.sin_addr=*(struct in_addr *) *hostdata->h_addr_list;
}


return(sendto(S->out_fd,Bytes,len,0,(struct sockaddr *) &sa,salen));
}



STREAM *STREAMOpenUDP(int Port,int NonBlock)
{
int fd;
STREAM *Stream;

fd=OpenUDPSock(Port);
if (fd <0) return(NULL);
Stream=STREAMFromFD(fd);
Stream->Path=FormatStr(Stream->Path,"none:%d",Port);
Stream->Type=STREAM_TYPE_UDP;
return(Stream);
}



/* This is a simple function to decide if a string is an IP address as   */
/* opposed to a host/domain name.                                        */

int IsIPAddress(char *Str)
{
int len,count;
len=strlen(Str);
if (len <1) return(FALSE);
for (count=0; count < len; count++)
   if ((! isdigit(Str[count])) && (Str[count] !='.')) return(FALSE);
 return(TRUE);
}




int STREAMAddConnectionHop(STREAM *S, char *Value)
{
char *Tempstr=NULL;
char *ConnectType=NULL;
int i;

StripTrailingWhitespace(Value);
StripLeadingWhitespace(Value);

if (! S->Values) S->Values=ListCreate();
Tempstr=FormatStr(Tempstr,"ConnectHop:%d",ListSize(S->Values));
STREAMSetValue(S,Tempstr,Value);

DestroyString(Tempstr);
return(TRUE);
}

void STREAMAddConnectionHopList(STREAM *S, char *HopList)
{
char *Hop=NULL, *ptr;

ptr=GetToken(HopList,",",&Hop,0);
while (ptr)
{
STREAMAddConnectionHop(S,Hop);
ptr=GetToken(ptr,",",&Hop,0);
}

DestroyString(Hop);
}

