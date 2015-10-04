#ifndef LIBUSEFUL_INET_H
#define LIBUSEFUL_INET_H

#include "includes.h"
#include "defines.h"

char *ExtractFromWebpage(char *RetStr, char *URL, char *ExtractStr, int MinLength);
char *GetExternalIP(char *RetStr);
int IPGeoLocate(char *IP, ListNode *Vars);

#endif
