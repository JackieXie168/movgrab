#ifndef LIBUSEFUL_SETTINGS

#include "defines.h"
#include "includes.h"
#include "Vars.h"

ListNode *LibUsefulSettings=NULL;

ListNode *LibUsefulValuesGetHead()
{
return(LibUsefulSettings);
}

void LibUsefulSetValue(char *Name, char *Value)
{
	if (! LibUsefulSettings) LibUsefulSettings=ListCreate();
	SetVar(LibUsefulSettings,Name,Value);
}

char *LibUsefulGetValue(char *Name)
{
return(GetVar(LibUsefulSettings,Name));
}

#endif
