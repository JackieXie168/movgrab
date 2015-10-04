#include "http.h"
#include "DataProcessing.h"

ListNode *Cookies=NULL;
char *g_UserAgent=NULL;
char *g_Proxy=NULL;
int g_Flags=0;

void HTTPAuthSet(HTTPAuthStruct *Auth, char *Logon, char *Password, int Type)
{
	Auth->Logon=CopyStr(Auth->Logon,Logon);
	Auth->Password=CopyStr(Auth->Password,Password);
	Auth->Flags |= Type;
}



void HTTPAuthDestroy(void *p_Auth)
{
HTTPAuthStruct *Auth;

if (! p_Auth) return;
Auth=(HTTPAuthStruct *) p_Auth;

DestroyString(Auth->AuthRealm);
DestroyString(Auth->AuthQOP);
DestroyString(Auth->AuthNonce);
DestroyString(Auth->AuthOpaque);
DestroyString(Auth->Logon);
DestroyString(Auth->Password);

free(Auth);
}

void HTTPInfoDestroy(void *p_Info)
{
HTTPInfoStruct *Info;

if (! p_Info) return;
Info=(HTTPInfoStruct *) p_Info;
DestroyString(Info->Host);
DestroyString(Info->Method);
DestroyString(Info->Doc);
DestroyString(Info->Destination);
DestroyString(Info->ResponseCode);
DestroyString(Info->PreviousRedirect);
DestroyString(Info->RedirectPath);
DestroyString(Info->ContentType);
DestroyString(Info->Timestamp);
DestroyString(Info->PostData);
DestroyString(Info->PostContentType);
DestroyString(Info->IfModifiedSince);
DestroyString(Info->Proxy);

DestroyList(Info->CustomSendHeaders,DestroyString);
if (Info->Authorization) HTTPAuthDestroy(Info->Authorization);
if (Info->ProxyAuthorization) HTTPAuthDestroy(Info->ProxyAuthorization);
free(Info);
}



//These functions relate to adding a 'Data processor' to the stream that
//will decode chunked HTTP transfers

typedef struct
{
char *Buffer;
int ChunkSize;
int BuffLen;
} THTTPChunk;

int HTTPChunkedInit(TProcessingModule *Mod, const char *Args)
{

Mod->Data=(THTTPChunk *) calloc(1, sizeof(THTTPChunk));

return(TRUE);
}

int HTTPChunkedRead(TProcessingModule *Mod, const char *InBuff, int InLen, char *OutBuff, int OutLen)
{
int len=0, val=0;
THTTPChunk *Chunk;
char *ptr, *vptr;

Chunk=(THTTPChunk *) Mod->Data;
len=Chunk->BuffLen+InLen;
Chunk->Buffer=SetStrLen(Chunk->Buffer,len);
memcpy(Chunk->Buffer+Chunk->BuffLen,InBuff,InLen);
Chunk->BuffLen=len;
ptr=Chunk->Buffer;

if (Chunk->ChunkSize==0)
{
	vptr=ptr;
	if (*vptr=='\r') vptr++;
	if (*vptr=='\n') vptr++;
	ptr=strchr(vptr,'\n');
	if (ptr)
	{
	  *ptr='\0';
		ptr++;
	}
	else ptr=Chunk->Buffer;
	Chunk->ChunkSize=strtol(vptr,NULL,16);

	if (Chunk->ChunkSize==0) return(0);
}

val=(Chunk->Buffer +len) - ptr;

if (ptr)
{
if (val > Chunk->ChunkSize) val=Chunk->ChunkSize;
if (val > OutLen) val=OutLen;

memcpy(OutBuff,ptr,val);
Chunk->BuffLen=(Chunk->Buffer+len) - (ptr + val);
memmove(Chunk->Buffer, ptr+val, Chunk->BuffLen);

Chunk->ChunkSize-=val;
}

if (Chunk->ChunkSize < 0) Chunk->ChunkSize=0;

return(val);
}

int HTTPChunkedClose(TProcessingModule *Mod)
{
THTTPChunk *Chunk;

Chunk=(THTTPChunk *) Mod->Data;
DestroyString(Chunk->Buffer);
free(Chunk);

return(TRUE);
}


void HTTPAddChunkedProcessor(STREAM *S)
{
TProcessingModule *Mod=NULL;

   Mod=(TProcessingModule *) calloc(1,sizeof(TProcessingModule));
   Mod->Name=CopyStr(Mod->Name,"HTTP:Chunked");
   Mod->Init=HTTPChunkedInit;
   Mod->Read=HTTPChunkedRead;
   Mod->Close=HTTPChunkedClose;

   Mod->Init(Mod, "");
	 STREAMAddDataProcessor(S,Mod,"");
}



char *HTTPUnQuote(char *RetBuff, char *Str)
{
char *RetStr=NULL, *Token=NULL, *ptr;
int olen=0, ilen;

RetStr=CopyStr(RetStr,"");
ilen=StrLen(Str);

for (ptr=Str; ptr < (Str+ilen); ptr++)
{
switch (*ptr)
{
	case '+': 
		RetStr=AddCharToBuffer(RetStr,olen,' '); 
		olen++; 
		break;
		
	case '%':
		  ptr++;
		  Token=CopyStrLen(Token,ptr,2); 
		  ptr++; //not +=2, as we will increment again
		  RetStr=AddCharToBuffer(RetStr,olen,strtol(Token,NULL,16) & 0xFF); 
		  olen++; 
		  break;

	default:
		  RetStr=AddCharToBuffer(RetStr,olen,*ptr); 
		  olen++;
		  break;
}

}

DestroyString(Token);
return(RetStr);
}

char *HTTPQuote(char *RetBuff, char *Str)
{
char *RetStr=NULL, *Token=NULL, *ptr;
int olen=0, ilen;

RetStr=CopyStr(RetStr,"");
ilen=StrLen(Str);

for (ptr=Str; ptr < (Str+ilen); ptr++)
{
switch (*ptr)
{

		case ' ':
		case '(':
		case ')':
		case '[':
		case ']':
		case '{':
		case '}':
		case '\t':
		case '?':
		case '&':
		case '!':
		case ',':
		case '+':
		case '\'':
		case ':':
		case '/':
		Token=FormatStr(Token,"%%%02X",*ptr); 
		RetStr=CatStr(RetStr,Token);
		olen+=StrLen(Token);
		break;

		default:
	//	 RetStr=AddCharToBuffer(RetStr,olen,*ptr); 
		 RetStr=AddCharToStr(RetStr,*ptr); 
		 olen++;
		break;
}

}

RetStr[olen]='\0';
DestroyString(Token);
return(RetStr);
}


void HTTPInfoSetValues(HTTPInfoStruct *Info, char *Host, int Port, char *Logon, char *Password, char *Method, char *Doc, char *ContentType, int ContentLength)
{

Info->Host=CopyStr(Info->Host,Host);
if (Port > 0) Info->Port=Port;
else Info->Port=80;
Info->Method=CopyStr(Info->Method,Method);
Info->Doc=CopyStr(Info->Doc,Doc);
Info->PostContentType=CopyStr(Info->PostContentType,ContentType);
Info->PostContentLength=ContentLength;

if (StrLen(Logon) || StrLen(Password))
{
if (! Info->Authorization) Info->Authorization=(HTTPAuthStruct *) calloc(1,sizeof(HTTPAuthStruct));
	HTTPAuthSet(Info->Authorization,Logon, Password, HTTP_AUTH_BASIC);
}

}




HTTPInfoStruct *HTTPInfoCreate(char *Host, int Port, char *Logon, char *Password, char *Method, char *Doc, char *ContentType, int ContentLength)
{
HTTPInfoStruct *Info;

Info=(HTTPInfoStruct *) calloc(1,sizeof(HTTPInfoStruct));
HTTPInfoSetValues(Info, Host, Port, Logon, Password, Method, Doc, ContentType, ContentLength);

Info->CustomSendHeaders=CreateEmptyList();
if (StrLen(g_Proxy)) Info->Proxy=CopyStr(Info->Proxy,g_Proxy);
if (g_Flags) Info->Flags=g_Flags;

return(Info);
}




void HTTPParseCookie(HTTPInfoStruct *Info, char *Str)
{
	char *startptr, *endptr;
	char *Tempstr=NULL;
	ListNode *Curr;
	int len;

	startptr=Str;
	while (*startptr==' ') startptr++;

	endptr=strchr(startptr,';');
	if (endptr==NULL) endptr=startptr+strlen(Str);
	if (( *endptr==';') || (*endptr=='\r') ) endptr--;

	Tempstr=CopyStrLen(Tempstr,startptr,endptr-startptr);


	Curr=GetNextListItem(Cookies);
	endptr=strchr(Tempstr,'=');
	len=endptr-Tempstr;
	len--;

	while (Curr !=NULL)
	{
		if (strncmp(Curr->Item,Tempstr,len)==0)
		{
			Curr->Item=CopyStr(Curr->Item,Tempstr);
			DestroyString(Tempstr);
			return;
		}
		Curr=GetNextListItem(Curr);
	}

	if (! Cookies) Cookies=CreateEmptyList();
	AddItemToList(Cookies,(void *)CopyStr(NULL,Tempstr));

DestroyString(Tempstr);
}



char *AppendCookies(char *InStr, ListNode *CookieList)
{
	ListNode *Curr;
	int count=0;
	char *Tempstr=NULL;

	Tempstr=InStr;
	Curr=GetNextListItem(CookieList);

	if (Curr) 
	{
		Tempstr=CatStr(Tempstr,"Cookie: ");
		while ( Curr )
		{
		Tempstr=MCatStr(Tempstr," ",(char *)Curr->Item,NULL);
		Curr=GetNextListItem(Curr);
		if (Curr) Tempstr=CatStr(Tempstr, ";");
		}

		Tempstr=CatStr(Tempstr,"\r\n");
	}

return(Tempstr);
}


void HTTPHandleWWWAuthenticate(HTTPInfoStruct *Info, char *Line)
{
char *ptr, *ptr2, *Token=NULL, *Name=NULL, *Value=NULL;
char *AuthTypeStrings[]={"Basic","Digest",NULL};
typedef enum {AUTH_BASIC, AUTH_DIGEST} TAuthTypes;
int result;

if (! Info->Authorization) Info->Authorization=(HTTPAuthStruct *) calloc(1,sizeof(HTTPAuthStruct));
ptr=Line;
while (isspace(*ptr)) ptr++;
ptr=GetToken(ptr," ",&Token,0);

result=MatchTokenFromList(Token,AuthTypeStrings,0);
if (result==AUTH_BASIC) Info->Authorization->Flags |=HTTP_AUTH_BASIC;
if (result==AUTH_DIGEST) Info->Authorization->Flags |=HTTP_AUTH_DIGEST;

while (ptr)
{
ptr=GetToken(ptr,",",&Token,GETTOKEN_QUOTES);
StripLeadingWhitespace(Token);
StripTrailingWhitespace(Token);
ptr2=GetToken(Token,"=",&Name,GETTOKEN_QUOTES);
ptr2=GetToken(ptr2,"=",&Value,GETTOKEN_QUOTES);


if (strcasecmp(Name,"realm")==0) Info->Authorization->AuthRealm=CopyStr(Info->Authorization->AuthRealm,Value);
if (strcasecmp(Name,"qop")==0)  Info->Authorization->AuthQOP=CopyStr(Info->Authorization->AuthQOP,Value);
if (strcasecmp(Name,"nonce")==0) Info->Authorization->AuthNonce=CopyStr(Info->Authorization->AuthNonce,Value);
if (strcasecmp(Name,"opaque")==0) Info->Authorization->AuthOpaque=CopyStr(Info->Authorization->AuthOpaque,Value);
}

DestroyString(Token);
DestroyString(Name);
DestroyString(Value);
}




void HTTPParseHeader(STREAM *S, HTTPInfoStruct *Info, char *Header)
{
char *Token=NULL, *Tempstr=NULL;
int count=0;
int result;
char *ptr;

if (Info->Flags & HTTP_DEBUG) fprintf(stderr,"HEADER: %s\n",Header);
	ptr=GetToken(Header,":",&Token,0);
	while (isspace(*ptr)) ptr++;

	Tempstr=MCopyStr(Tempstr,"HTTP:",Token,NULL);
	STREAMSetValue(S,Tempstr,ptr);
	if (StrLen(Token))
	{
		if (strcasecmp(Token,"Location")==0)
		{
			if (
						(strncasecmp(ptr,"http:",5)==0) ||
						(strncasecmp(ptr,"https:",6)==0) 
					)
			{
				Info->RedirectPath=CopyStr(Info->RedirectPath,ptr);
			}
			else Info->RedirectPath=FormatStr(Info->RedirectPath,"http://%s:%d%s",Info->Host,Info->Port,ptr);
		}
		else if (strcasecmp(Token,"Content-length")==0)
		{
			Info->ContentLength=atoi(ptr);
		}
		else if (strcasecmp(Token,"Content-type")==0)
		{
			Info->ContentType=CopyStr(Info->ContentType,ptr);
		}
		else if (strcasecmp(Token,"WWW-Authenticate")==0) HTTPHandleWWWAuthenticate(Info,ptr);

		else if (strcasecmp(Token,"Set-Cookie")==0) HTTPParseCookie(Info,ptr);
		else if (strcasecmp(Token,"Date")==0) Info->Timestamp=CopyStr(Info->Timestamp,ptr);
		else if (strcasecmp(Token,"Connection")==0) 
		{
				if (strcasecmp(ptr,"Close")==0) Info->Flags &= ~HTTP_KEEPALIVE;
		}
		else if (
			(strcasecmp(Token,"Transfer-Encoding")==0)
		)
		{
			strlwr(ptr);
			if (strstr(ptr,"chunked")) 
			{
				Info->Flags |= HTTP_CHUNKED;
			}
		}
		else if (strcasecmp(Token,"Content-Encoding")==0) 
		{
			strlwr(ptr);
			if (
					(strcmp(ptr,"gzip")==0) ||
					(strcmp(ptr,"x-gzip")==0)
				)
			{
				Info->Flags |= HTTP_GZIP;
			}
			if (
					(strcmp(ptr,"deflate")==0) 
				)
			{
				Info->Flags |= HTTP_DEFLATE;
			}

		}
	}

DestroyString(Token);
DestroyString(Tempstr);
}

void HTTPParseResponseLine(STREAM *S, HTTPInfoStruct *Info, char *Header)
{
char *ptr;

	if (Info->Flags & HTTP_DEBUG) fprintf(stderr,"RESPONSE: %s\n",Header);
  if (strncmp(Header,"HTTP/",5)==0)
  {
    ptr=strchr(Header,' ');
    ptr++;

    Info->ResponseCode=CopyStrLen(Info->ResponseCode,ptr,3);
    STREAMSetValue(S,"HTTP:ResponseCode",Info->ResponseCode);
	if (StrLen(Info->ResponseCode))
	{
		if (*Info->ResponseCode=='3') 
		{
			Info->Flags |= HTTP_REDIRECT;
		}

		if (strcmp(Info->ResponseCode,"401")==0) 
		{
			if (Info->Authorization) Info->Authorization->Flags |= HTTP_AUTH_BASIC;
		}

		if (strcmp(Info->ResponseCode,"407")==0) 
		{
			if (Info->ProxyAuthorization) Info->ProxyAuthorization->Flags |= HTTP_PROXY_AUTH;
		}

	}  
  }

}


char *HTTPHeadersAppendAuth(char *RetStr, char *AuthHeader, HTTPInfoStruct *Info, HTTPAuthStruct *AuthInfo)
{
char *SendStr=NULL, *Tempstr=NULL;
char *HA1=NULL, *HA2=NULL, *ClientNonce=NULL, *Digest=NULL;
int i, AuthCounter;

if (! AuthInfo) return(RetStr);

SendStr=CatStr(RetStr,"");

  if (AuthInfo->Flags & HTTP_AUTH_DIGEST)
  {
    AuthCounter++;
    Tempstr=FormatStr(Tempstr,"%s:%s:%s",AuthInfo->Logon,AuthInfo->AuthRealm,AuthInfo->Password);
    HA1=HashMD5(HA1,Tempstr,StrLen(Tempstr),0);
    Tempstr=FormatStr(Tempstr,"%s:%s",Info->Method,Info->Doc);
    HA2=HashMD5(HA2,Tempstr,StrLen(Tempstr),0);

    for (i=0; i < 10; i++)
    {
			Tempstr=FormatStr(Tempstr,"%x",rand() % 255);
			ClientNonce=CatStr(ClientNonce,Tempstr);
    }

    Tempstr=FormatStr(Tempstr,"%s:%s:%08d:%s:auth:%s",HA1,AuthInfo->AuthNonce,AuthCounter,ClientNonce,HA2);
    Digest=HashMD5(Digest,Tempstr,StrLen(Tempstr),0);
    Tempstr=FormatStr(Tempstr,"%s: Digest username=\"%s\",realm=\"%s\",nonce=\"%s\",uri=\"%s\",qop=auth,nc=%08d,cnonce=\"%s\",response=\"%s\"\r\n",AuthHeader,AuthInfo->Logon,AuthInfo->AuthRealm,AuthInfo->AuthNonce,Info->Doc,AuthCounter,ClientNonce,Digest);
    SendStr=CatStr(SendStr,Tempstr);
    AuthInfo->Flags |= HTTP_SENT_AUTH;
  }
  else 
  {
    Tempstr=CopyStr(Tempstr,AuthInfo->Logon);
    Tempstr=CatStr(Tempstr,":");
    Tempstr=CatStr(Tempstr,AuthInfo->Password);
    Digest=SetStrLen(Digest,StrLen(Tempstr) *2);
    to64frombits(Digest,Tempstr,strlen(Tempstr));
    Tempstr=FormatStr(Tempstr,"%s: %s %s\r\n",AuthHeader,"Basic",Digest);
    SendStr=CatStr(SendStr,Tempstr);
    AuthInfo->Flags |= HTTP_SENT_AUTH;
  }

DestroyString(HA1);
DestroyString(HA2);
DestroyString(ClientNonce);
DestroyString(Digest);
DestroyString(Tempstr);

return(SendStr);
}


void HTTPSendHeaders(STREAM *S, HTTPInfoStruct *Info)
{
char *SendStr=NULL, *Tempstr=NULL;
ListNode *Curr;
int count;
char *Doc=NULL;
int i;
char *ptr;
static int AuthCounter=0;

SendStr=CopyStr(SendStr,Info->Method);
SendStr=CatStr(SendStr," ");

if (StrLen(Info->Proxy)) SendStr=MCatStr(SendStr,"http://",Info->Host,Info->Doc,NULL);
else SendStr=CatStr(SendStr,Info->Doc);

if (Info->Flags & HTTP_VER1_0) SendStr=CatStr(SendStr," HTTP/1.0\r\n");
else
{
SendStr=MCatStr(SendStr," HTTP/1.1\r\n","Host: ",Info->Host,"\r\n",NULL);
}

if (StrLen(Info->PostContentType) >0)
{
Tempstr=FormatStr(Tempstr,"Content-Type: %s\r\n",Info->PostContentType);
SendStr=CatStr(SendStr,Tempstr);
}
if (Info->PostContentLength > 0) 
{
Tempstr=FormatStr(Tempstr,"Content-Length: %d\r\n",Info->PostContentLength);
SendStr=CatStr(SendStr,Tempstr);
}

if (StrLen(Info->Destination))
{
Tempstr=FormatStr(Tempstr,"Destination: %s\r\n",Info->Destination);
SendStr=CatStr(SendStr,Tempstr);
}

/* If we have authorisation details then send them */
if  (Info->Authorization) SendStr=HTTPHeadersAppendAuth(SendStr, "Authorization", Info, Info->Authorization);
if  (Info->ProxyAuthorization) SendStr=HTTPHeadersAppendAuth(SendStr, "Proxy-Authorization", Info, Info->ProxyAuthorization);


if (Info->Depth > 0)
{
Tempstr=FormatStr(Tempstr,"Depth: %d\r\n",Info->Depth);
SendStr=CatStr(SendStr,Tempstr);
}

/*
if ((PathData->Options.Restart) && (PathData->offset >0))
{
snprintf(Buffer,sizeof(Buffer),"Range: bytes=%d-\r\n",PathData->offset);
SendStr=CatStr(SendStr,Buffer);

}
*/

if (StrLen(Info->IfModifiedSince)) SendStr=MCatStr(SendStr,"If-Modified-Since: ",Info->IfModifiedSince,"\r\n",NULL);

if (
		 (strcasecmp(Info->Method,"DELETE") !=0) &&
		 (strcasecmp(Info->Method,"HEAD") !=0) &&
		 (strcasecmp(Info->Method,"PUT") !=0) 
	)
{
SendStr=CatStr(SendStr,"Accept: */*\r\n");

Tempstr=CopyStr(Tempstr,"");
if (DataProcessorAvailable("Compression","gzip")) Tempstr=CatStr(Tempstr,"gzip");
if (DataProcessorAvailable("Compression","zlib")) 
{
if (StrLen(Tempstr)) Tempstr=CatStr(Tempstr,", deflate");
else Tempstr=CatStr(Tempstr,"deflate");
}

if (StrLen(Tempstr)) SendStr=MCatStr(SendStr,"Accept-Encoding: ",Tempstr,"\r\n",NULL);
else SendStr=CatStr(SendStr,"Accept-Encoding:\r\n");
}

if (Info->Flags & HTTP_KEEPALIVE) 
{
//if (Info->Flags & HTTP_VER1_0) 
SendStr=CatStr(SendStr,"Connection: Keep-Alive\r\n");
//SendStr=CatStr(SendStr,"Content-Length: 0\r\n");
}
else
{
SendStr=CatStr(SendStr,"Connection: Close\r\n");
}

if (StrLen(g_UserAgent)) SendStr=MCatStr(SendStr,"User-Agent: ",g_UserAgent, "\r\n",NULL);

Curr=GetNextListItem(Info->CustomSendHeaders);
while (Curr)
{
SendStr=MCatStr(SendStr,Curr->Tag, ": ", (char *)  Curr->Item, "\r\n",NULL);
Curr=GetNextListItem(Curr);
}

SendStr=AppendCookies(SendStr,Cookies);
SendStr=CatStr(SendStr,"\r\n");

if (Info->Flags & HTTP_DEBUG) fprintf(stderr,"HTTPSEND: ------\n%s------\n\n",SendStr);
STREAMWriteLine(SendStr,S);
STREAMFlush(S);

DestroyString(Tempstr);
DestroyString(Doc);
DestroyString(SendStr);
}

char *HTTPParseURL(char *URL, char **Proto, char **Host, int *Port, char **Login, char **Password)
{
char *ptr, *p_port;
char *Token=NULL, *AuthCred=NULL;

//if no port given
*Port=0;

ptr=URL;
if (strncmp(ptr,"http:",5)==0) ptr+=5;
if (strncmp(ptr,"https:",6)==0) ptr+=6;
if (Proto) 
{
	if (! ptr || (ptr==URL)) *Proto=CopyStr(*Proto,"http:");
	else *Proto=CopyStrLen(*Proto,URL,ptr-URL-1);
}

while (*ptr=='/') ptr++;

if (StrLen(*Proto))
{
ptr=GetToken(ptr,"/",&Token,0);

if (StrLen(Token))
{
	if (strchr(Token,'@')) 
	{
		ptr=GetToken(Token,"@",&AuthCred,0);
		memmove(Token,ptr,StrLen(ptr)+1);
	}

p_port=strchr(Token,':');
if (p_port)
{
*p_port='\0';
p_port++;
*Port=atoi(p_port);
}
*Host=CopyStr(*Host,Token);
}
}

if (StrLen(AuthCred))
{
	if (Login)
	{
		ptr=GetToken(AuthCred,":",Login,0);
		if (Password) ptr=GetToken(ptr,":",Password,0);
	}

}

DestroyString(Token);
DestroyString(AuthCred);
return(ptr);
}




void HTTPReadHeaders(STREAM *S, HTTPInfoStruct *Header)
{
char *Tempstr=NULL;


Header->ContentLength=0;
Header->RedirectPath=CopyStr(Header->RedirectPath,"");
Header->Flags &= ~(HTTP_CHUNKED | HTTP_GZIP | HTTP_DEFLATE);
Tempstr=STREAMReadLine(Tempstr,S);
if (Tempstr)
{
HTTPParseResponseLine(S, Header,Tempstr);
Tempstr=STREAMReadLine(Tempstr,S);
}

while (Tempstr)
{
StripTrailingWhitespace(Tempstr);
if (StrLen(Tempstr)==0) break;
HTTPParseHeader(S, Header,Tempstr);
Tempstr=STREAMReadLine(Tempstr,S);
}
DestroyString(Tempstr);
}



int HTTPProcessResponse(HTTPInfoStruct *HTTPInfo)
{
char *ptr, *login_ptr, *pass_ptr;
int result=HTTP_ERROR;
char *Tempstr=NULL;
int RCode;

if (HTTPInfo->ResponseCode)
{
RCode=atoi(HTTPInfo->ResponseCode);
switch (RCode)
{
	case 200:
	case 201:
	case 202:
	case 203:
	case 204:
	case 205:
	case 206:
	case 207:
	case 304:
	result=HTTP_OKAY;
  break;

	case 301:
	case 302:
	case 303:
	case 307:
	if (HTTPInfo->PreviousRedirect && (strcmp(HTTPInfo->RedirectPath,HTTPInfo->PreviousRedirect)==0)) result=HTTP_CIRCULAR_REDIRECTS;
	else
	{
	HTTPInfo->PreviousRedirect=CopyStr(HTTPInfo->PreviousRedirect,HTTPInfo->RedirectPath);
	ptr=HTTPParseURL(HTTPInfo->RedirectPath, &Tempstr, &HTTPInfo->Host, &HTTPInfo->Port,NULL,NULL);

	//if HTTP_SSL_REWRITE is set, then all redirects get forced to https
	if (HTTPInfo->Flags & HTTP_SSL_REWRITE) Tempstr=CopyStr(Tempstr,"https");
	if (strcmp(Tempstr,"https")==0) 
	{
		HTTPInfo->Flags |= HTTP_SSL;
		if (HTTPInfo->Port==0) HTTPInfo->Port=443;
	}
	else 
	{
		HTTPInfo->Flags &= ~HTTP_SSL;
		if (HTTPInfo->Port==0) HTTPInfo->Port=80;
	}

	//Redirects must be get!
	HTTPInfo->Method=CopyStr(HTTPInfo->Method,"GET");
	HTTPInfo->Doc=MCopyStr(HTTPInfo->Doc,"/",ptr,NULL);
	HTTPInfo->PostData=CopyStr(HTTPInfo->PostData,"");
	HTTPInfo->PostContentType=CopyStr(HTTPInfo->PostContentType,"");
	HTTPInfo->PostContentLength=0;
	if (! (HTTPInfo->Flags & HTTP_NOREDIRECT)) result=HTTP_REDIRECT;
	}
	break;

	//401 Means authenticate, so it's not a pure error
	case 401:
	//407 Means authenticate with a proxy
	result=HTTP_AUTH_BASIC;
	break;

	case 407:
	result=HTTP_PROXY_AUTH;
	break;

	default:
	result=HTTP_NOTFOUND;
	break;

}
}

DestroyString(Tempstr);

return(result);
}


STREAM *HTTPConnect(HTTPInfoStruct *Info)
{
STREAM *S;
char *Proto=NULL, *Host=NULL;
int Port=80, Flags=0;

S=STREAMCreate();
Flags=Info->Flags;

if (StrLen(Info->Proxy))
{
	if (! Info->ProxyAuthorization) 
	{
		Info->ProxyAuthorization=(HTTPAuthStruct *) calloc(1,sizeof(HTTPAuthStruct));
	}
	HTTPParseURL(Info->Proxy, &Proto, &Host, &Port, &Info->ProxyAuthorization->Logon, &Info->ProxyAuthorization->Password);
	
	if (strcasecmp(Proto,"https")==0) 
	{
		Flags |= CONNECT_SSL; 
		if (Port==0) Port=443;
	}
	else 
	{
		if (Port==0) Port=80;
	}
}
else
{
	Host=CopyStr(Host,Info->Host);
	Port=Info->Port;
	if (Info->Flags & HTTP_SSL) Flags |= CONNECT_SSL;
}

if (STREAMConnectToHost(S,Host,Port,Flags))
{
	HTTPSendHeaders(S,Info);
}
else
{
	STREAMClose(S);
	S=NULL;
}

DestroyString(Proto);
DestroyString(Host);
return(S);
}


STREAM *HTTPTransact(HTTPInfoStruct *Info)
{
int result=HTTP_NOCONNECT;

while (1)
{
	if (! Info->S) Info->S=HTTPConnect(Info);
	else 
	{
		HTTPSendHeaders(Info->S,Info);
	}

	if (Info->S && STREAMIsConnected(Info->S))
	{
		Info->ResponseCode=CopyStr(Info->ResponseCode,"");
		if (StrLen(Info->PostData)) 
		{
			STREAMWriteLine(Info->PostData,Info->S);
			STREAMFlush(Info->S);
		}
		HTTPReadHeaders(Info->S,Info);
		result=HTTPProcessResponse(Info);
		if (Info->Flags & HTTP_CHUNKED) HTTPAddChunkedProcessor(Info->S);
		if (Info->Flags & HTTP_GZIP) 
		{
			STREAMAddStandardDataProcessor(Info->S,"compression","gzip","");
		}
		if (Info->Flags & HTTP_DEFLATE) STREAMAddStandardDataProcessor(Info->S,"compression","zlib","");

		if (result == HTTP_OKAY) break;
		if (result == HTTP_NOTFOUND) break;
		if (result == HTTP_ERROR) break;
		if (result == HTTP_CIRCULAR_REDIRECTS) break;


		if (
					(result == HTTP_AUTH_BASIC) && 
					(
						(Info->Authorization->Flags & HTTP_SENT_AUTH) ||
						(! Info->Authorization->Logon) || 
						(StrLen(Info->Authorization->Logon)==0) 
					)
			 )
		{
			if (result == HTTP_AUTH_BASIC) break;
			if (result == HTTP_AUTH_DIGEST) break;
		}

		if (
					(result == HTTP_PROXY_AUTH) && 
					(
						(Info->ProxyAuthorization->Flags & HTTP_SENT_AUTH) ||
						(! Info->ProxyAuthorization->Logon) || 
						(StrLen(Info->ProxyAuthorization->Logon)==0) 
					)
			 )
		{
			 break;
		}

		STREAMClose(Info->S);
		Info->S=NULL;
	}
	else break;
}

if (result !=HTTP_OKAY)
{
STREAMClose(Info->S);
Info->S=NULL;
}
return(Info->S);
}

HTTPInfoStruct *HTTPInfoFromURL(char *Method, char *URL)
{
HTTPInfoStruct *Info;
char *Proto=NULL, *User=NULL, *Pass=NULL;
char *ptr=NULL, *dptr, *p_port;
int Port=0;


Info=HTTPInfoCreate("", 0, "", "", Method, "", "",0);
dptr=HTTPParseURL(URL, &Proto, &Info->Host, &Info->Port, &User, &Pass);
if (StrLen(User) || StrLen(Pass))
{
	Info->Authorization=(HTTPAuthStruct *) calloc(1,sizeof(HTTPAuthStruct));
	HTTPAuthSet(Info->Authorization,User, Pass, HTTP_AUTH_BASIC);
}


if (Info->Port==0)
{
	if (strcmp(Proto,"https")==0) Info->Port=443;
	else Info->Port=80;
}
if (strcmp(Proto,"https")==0) Info->Flags |= HTTP_SSL;

Info->Doc=MCopyStr(Info->Doc,"/",dptr,NULL);
if (StrLen(dptr))
{
	if (strcmp(Method,"POST")==0)
	{
	ptr=strchr(Info->Doc,'?');
	if (ptr) 
	{
		*ptr='\0';
		ptr++;
	  if (StrLen(ptr)) 
		{
			Info->PostContentType=CopyStr(Info->PostContentType,"application/x-www-form-urlencoded");

			Info->PostData=CopyStr(Info->PostData,ptr);
			Info->PostContentLength=StrLen(ptr);
		}
	}
	}
	else 
	{
		ptr="";
	}
}


DestroyString(Proto);
return(Info);
}


STREAM *HTTPMethod(char *Method, char *URL, char *Logon, char *Password)
{
HTTPInfoStruct *Info;
STREAM *S;


Info=HTTPInfoFromURL(Method, URL);
if (StrLen(Logon) || StrLen(Password))
{
	if (! Info->Authorization) Info->Authorization=(HTTPAuthStruct *) calloc(1,sizeof(HTTPAuthStruct));
	HTTPAuthSet(Info->Authorization,Logon, Password, HTTP_AUTH_BASIC);
}
S=HTTPTransact(Info);

HTTPInfoDestroy(Info);
return(S);
}


STREAM *HTTPGet(char *URL, char *Logon, char *Password)
{
return(HTTPMethod("GET", URL, Logon, Password));
}


void HTTPCopyToSTREAM(STREAM *Con, STREAM *S)
{
char *Tempstr=NULL;
int result;

Tempstr=SetStrLen(Tempstr,BUFSIZ);
result=STREAMReadBytes(Con, Tempstr,BUFSIZ);
while (result > 0)
{
	STREAMWriteBytes(S,Tempstr,result);
	result=STREAMReadBytes(Con, Tempstr,BUFSIZ);
}

}


int HTTPDownload(char *URL,char *Login,char *Password, STREAM *S)
{
STREAM *Con;
Con=HTTPGet(URL,Login,Password);
if (! Con) return(FALSE);
HTTPCopyToSTREAM(Con, S);
return(TRUE);
}

void HTTPSetUserAgent(char *AgentName)
{
g_UserAgent=CopyStr(g_UserAgent,AgentName);
}

void HTTPSetProxy(char *Proxy)
{
g_Proxy=CopyStr(g_Proxy,Proxy);
}

void HTTPSetFlags(int Flags)
{
g_Flags=Flags;
}
