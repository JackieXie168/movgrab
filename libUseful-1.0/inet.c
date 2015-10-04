#include "inet.h"
#include "http.h"
#include "GeneralFunctions.h"

char *ExtractFromWebpage(char *RetStr, char *URL, char *ExtractStr, int MinLength)
{
STREAM *S;
char *Tempstr=NULL, *ptr;
ListNode *Vars;

Vars=CreateEmptyList();

S=HTTPGet(URL,"","");
if (S)
{
Tempstr=STREAMReadLine(Tempstr,S);
while (Tempstr)
{
StripTrailingWhitespace(Tempstr);
StripLeadingWhitespace(Tempstr);

printf("GOT: %d %s\n",StrLen(Tempstr),Tempstr);
if (StrLen(Tempstr) >=MinLength)
{
	if (! StrLen(ExtractStr)) RetStr=CopyStr(RetStr,Tempstr);
	else 
	{
		ExtractVarsFromString(Tempstr,ExtractStr,Vars);
		ptr=GetVar(Vars,"extract_item");
		if (StrLen(ptr)) RetStr=CopyStr(RetStr,ptr);
	}
}
Tempstr=STREAMReadLine(Tempstr,S);
}
STREAMClose(S);
}

DestroyList(Vars,(LIST_ITEM_DESTROY_FUNC) DestroyString);
DestroyString(Tempstr);

StripTrailingWhitespace(RetStr);
StripLeadingWhitespace(RetStr);

return(RetStr);
}


char *GetExternalIP(char *RetStr)
{
STREAM *S;

RetStr=CopyStr(RetStr,"");
RetStr=ExtractFromWebpage(RetStr,"http://icanhazip.com/", "",4);
if (! StrLen(RetStr)) RetStr=ExtractFromWebpage(RetStr,"http://ip.appspot.com/", "",4);
if (! StrLen(RetStr)) RetStr=ExtractFromWebpage(RetStr,"http://checkip.dyndns.org", "Current IP Address: $(extract_item)</body>",4);

return(RetStr);
}


int IPGeoLocate(char *IP, ListNode *Vars)
{
STREAM *S=NULL;
char *ptr, *TagType=NULL, *TagData=NULL, *Tempstr=NULL, *Token=NULL;
char *DesiredTags[]={"CountryCode","CountryName","City","RegionName","Latitude","Longitude","TimeZone",NULL};

if (! IsIPAddress(IP)) Tempstr=MCopyStr(Tempstr,"http://ipinfodb.com/ip_query.php?ip=",LookupHostIP(IP),NULL);
else Tempstr=MCopyStr(Tempstr,"http://ipinfodb.com/ip_query.php?ip=",IP,NULL);
S=HTTPGet(Tempstr, "", "");
if (S)
{
	Tempstr=STREAMReadLine(Tempstr,S);
	while (Tempstr)
	{
		ptr=HtmlGetTag(Tempstr,&TagType,&TagData);
		while (ptr)
		{
		if (MatchTokenFromList(TagType,DesiredTags,0) > -1)
		{
		//we can't re-use 'TagType', we still need it
		ptr=HtmlGetTag(ptr,&Token,&TagData);
		SetVar(Vars,TagType,TagData);
		}
		ptr=HtmlGetTag(ptr,&TagType,&TagData);
		}
		Tempstr=STREAMReadLine(Tempstr,S);
	}
}

STREAMClose(S);

DestroyString(Tempstr);
DestroyString(Token);
DestroyString(TagType);
DestroyString(TagData);
}

