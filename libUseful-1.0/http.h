#ifndef LIBUSEFUL_HTTP_H
#define LIBUSEFUL_HTTP_H

#include "includes.h"
#include "defines.h"
#include "file.h"

#define HTTP_AUTH_BASIC  1
#define HTTP_AUTH_DIGEST 2
#define HTTP_SENT_AUTH 4
#define HTTP_PROXY_AUTH 8


#define HTTP_OKAY 0
#define HTTP_NOCONNECT 1
#define HTTP_NOTFOUND 2
#define HTTP_REDIRECT 4
#define HTTP_CHUNKED 8
#define HTTP_ERROR 16
#define HTTP_NOREDIRECT 32
#define HTTP_VER1_0 64
#define HTTP_KEEPALIVE 128
#define HTTP_DEBUG 256
#define HTTP_PROXY 512
#define HTTP_SSL 1024
#define HTTP_SSL_REWRITE 2048
#define HTTP_CIRCULAR_REDIRECTS 4096


typedef struct
{
int Flags;
char *AuthRealm;
char *AuthQOP;
char *AuthNonce;
char *AuthOpaque;
char *Logon;
char *Password;
} HTTPAuthStruct;


typedef struct
{
char *Host;
int Port;
char *Method;
char *Doc;
char *Destination;
char *ResponseCode;
int Flags;
char *RedirectPath;
char *PreviousRedirect;
char *ContentType;
char *Timestamp;
int ContentLength;
int Depth;
char *PostData;
char *PostContentType;
int PostContentLength;
char *IfModifiedSince;
char *Proxy;
ListNode *CustomSendHeaders;
HTTPAuthStruct *Authorization;
HTTPAuthStruct *ProxyAuthorization;
STREAM *S;
} HTTPInfoStruct;



char *HTTPQuote(char *, char*);
char *HTTPUnQuote(char *, char*);


void HTTPInfoDestroy(void *p_Info);
void HTTPInfoSetValues(HTTPInfoStruct *Info, char *Host, int Port, char *Logon, char *Password, char *Method, char *Doc, char *ContentType, int ContentLength);
HTTPInfoStruct *HTTPInfoCreate(char *Host, int Port, char *Logon, char *Password, char *Method, char *Doc, char *ContentType, int ContentLength);
STREAM *HTTPConnect(HTTPInfoStruct *Info);
STREAM *HTTPTransact(HTTPInfoStruct *Info);
STREAM *HTTPMethod(char *Method, char *URL, char *Logon, char *Password);
STREAM *HTTPGet(char *URL, char *Logon, char *Password);
int HTTPReadBytes(STREAM *Con, char **Buffer);

void HTTPCopyToSTREAM(STREAM *Con, STREAM *S);
int HTTPDownload(char *URL, char *Login, char *Password, STREAM *S);
void HTTPSetUserAgent(char *AgentName);
void HTTPSetProxy(char *Proxy);


#endif
