#include "oauth.h"
#include "http.h"

void ParseTagData(char *TagName,char *TagData,char **RType,char **RName,char **RValue)
{
char *Name=NULL, *Value=NULL, *ptr;

ptr=GetNameValuePair(TagData," ","=",&Name,&Value);
while (ptr)
{
if (strcasecmp(Name,"type")==0) *RType=HtmlDeQuote(*RType,Value);
if (strcasecmp(Name,"name")==0) *RName=HtmlDeQuote(*RName,Value);
if (strcasecmp(Name,"value")==0) *RValue=HtmlDeQuote(*RValue,Value);
if (strcasecmp(Name,"method")==0) *RType=HtmlDeQuote(*RType,Value);
if (strcasecmp(Name,"action")==0) *RValue=HtmlDeQuote(*RValue,Value);

ptr=GetNameValuePair(ptr," ","=",&Name,&Value);
}

DestroyString(Name);
DestroyString(Value);
}


void OAuthParseForm(char *HTML, char *SubmitType, char **SubmitURL, ListNode *HiddenVals, ListNode *QueryVals)
{
char *TagName=NULL, *TagData=NULL, *Type=NULL, *Name=NULL, *Value=NULL, *ptr;

ptr=HtmlGetTag(HTML,&TagName,&TagData);
while (ptr)
{
if (strcmp(TagName,"input")==0)	
{
	ParseTagData(TagName,TagData,&Type,&Name,&Value);

  if (strcasecmp(Type,"hidden")==0) SetVar(HiddenVals,Name,Value);
  if (strcasecmp(Type,"submit")==0) SetVar(HiddenVals,Name,Value);
  if (strcasecmp(Type,"text")==0) SetVar(QueryVals,Name,Value);
  if (strcasecmp(Type,"password")==0) SetVar(QueryVals,Name,Value);
}

if (strcmp(TagName,"form")==0)	
{
	ParseTagData(TagName,TagData,&Type,&Name,SubmitURL);
}


ptr=HtmlGetTag(ptr,&TagName,&TagData);
}

DestroyString(TagName);
DestroyString(TagData);
DestroyString(Type);
DestroyString(Name);
DestroyString(Value);
}



/*
  "device_code" : "4/vGOwLaVXdW45sYlHS2oqNJWu0fAP",
  "user_code" : "2ega9a7p",
  "verification_url" : "http://www.google.com/device",
  "expires_in" : 1800,
  "interval" : 5
*/


void OAuthParseJSON(char *JSON, ListNode *Vars)
{
char *ptr, *dptr, *Name=NULL,*Value=NULL;

ptr=JSON+StrLen(JSON)-1;
if (*ptr==',') *ptr='\0';

ptr=GetToken(JSON," :",&Name,GETTOKEN_QUOTES);
ptr=GetToken(ptr," :",&Value,GETTOKEN_QUOTES);

SetVar(Vars,Name,Value);

DestroyString(Value);
DestroyString(Name);
}



void OAuthDeviceLogin(char *LoginURL, char *ClientID, char *Scope, char **DeviceCode, char **UserCode, char **NextURL)
{
char *Tempstr=NULL, *Encode=NULL;
ListNode *Vars=NULL;
STREAM *S;

Vars=ListCreate();

Encode=HTTPQuote(Encode,ClientID);
Tempstr=MCopyStr(Tempstr,LoginURL,"?client_id=",Encode,NULL);
Encode=HTTPQuote(Encode,Scope);
Tempstr=MCatStr(Tempstr,"&scope=",Encode,NULL);

S=HTTPMethod("POST",Tempstr,"","");

if (S)
{
Tempstr=STREAMReadLine(Tempstr,S);
while (Tempstr)
{
StripTrailingWhitespace(Tempstr);
OAuthParseJSON(Tempstr, Vars);
Tempstr=STREAMReadLine(Tempstr,S);
}

*NextURL=CopyStr(*NextURL,GetVar(Vars,"verification_url"));
*DeviceCode=CopyStr(*DeviceCode,GetVar(Vars,"device_code"));
*UserCode=CopyStr(*UserCode,GetVar(Vars,"user_code"));
}


ListDestroy(Vars,DestroyString);
DestroyString(Tempstr);
DestroyString(Encode);
STREAMClose(S);
}


/*
 POST /o/oauth2/token HTTP/1.1
 Host: accounts.google.com
 Content-Type: application/x-www-form-urlencoded

 client_id=812741506391-h38jh0j4fv0ce1krdkiq0hfvt6n5amrf.apps.googleusercontent.com&
 client_secret={clientSecret}&
 code=4/L9fTtLrhY96442SEuf1Rl3KLFg3y&
 grant_type=http://oauth.net/grant_type/device/1.0
*/

void OAuthDeviceGetToken(char *TokenURL, char *ClientID, char *ClientSecret, char *DeviceCode, char **AccessToken, char **RefreshToken)
{
char *Tempstr=NULL, *Encode=NULL;
ListNode *Vars=NULL;
STREAM *S;

Vars=ListCreate();

Encode=HTTPQuote(Encode,ClientID);
Tempstr=MCopyStr(Tempstr,TokenURL,"?client_id=",Encode,NULL);
Encode=HTTPQuote(Encode,ClientSecret);
Tempstr=MCatStr(Tempstr,"&client_secret=",Encode,NULL);
Tempstr=MCatStr(Tempstr,"&code=",DeviceCode,NULL);
Tempstr=MCatStr(Tempstr,"&grant_type=","http://oauth.net/grant_type/device/1.0",NULL);

S=HTTPMethod("POST",Tempstr,"","");
if (S)
{
Tempstr=STREAMReadLine(Tempstr,S);
while (Tempstr)
{
StripTrailingWhitespace(Tempstr);
OAuthParseJSON(Tempstr, Vars);
Tempstr=STREAMReadLine(Tempstr,S);
}
}

/*
  "access_token" : "ya29.AHES6ZR-OrkfuNOV_npDtM8EzICJgIfIWnHB8Dz-gU0Zp7zAXW2M",
  "token_type" : "Bearer",
  "expires_in" : 3600,
  "id_token" : "eyJhbGciOiJSUzI1NiJ9.eyJpc3MiOiJhY2NvdW50cy5nb29nbGUuY29tIiwiYXVkIjoiNDMzNTAyNDEyMDU2LmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwiY2lkIjoiNDMzNTAyNDEyMDU2LmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwiaWQiOiIxMTA2NDEwMTM2MDkyMTQzNDQ0NDIiLCJ0b2tlbl9oYXNoIjoiOG9tVjRrOEs5NHJ5ZnUycldsZ014USIsImlhdCI6MTM0MjEyMjIzMiwiZXhwIjoxMzQyMTI2MTMyfQ.xghQf20eKCKngwyt0a6rt0mX0R8L_6RscALNBw2vz9Hm86mFPzGJzFPWpt7jWiYI5N8De0cJ09U242TVqnkTo9mstH4u9IsgZizaAoXk7pxwLcq38Ul1GYx21LIVbP5aCee-oEmZtGFnKJTsR4LfYkzgDBKWvBCYwba-Mf27zvI",
  "refresh_token" : "1/JHmIOQx7reU8Q5PUIGn6JHAZFN8QdbeGThX-ZoW4ZO0"
*/


*AccessToken=CopyStr(*AccessToken,GetVar(Vars,"access_token"));
*RefreshToken=CopyStr(*RefreshToken,GetVar(Vars,"refresh_token"));

ListDestroy(Vars,DestroyString);
DestroyString(Tempstr);
DestroyString(Encode);
}



void OAuthDeviceRefreshToken(char *TokenURL, char *ClientID, char *ClientSecret, char *RefreshToken, char **AccessToken)
{
char *Tempstr=NULL, *Encode=NULL;
ListNode *Vars=NULL;
STREAM *S;

Vars=ListCreate();

Encode=HTTPQuote(Encode,ClientID);
Tempstr=MCopyStr(Tempstr,TokenURL,"?client_id=",Encode,NULL);
Encode=HTTPQuote(Encode,ClientSecret);
Tempstr=MCatStr(Tempstr,"&client_secret=",Encode,NULL);
Tempstr=MCatStr(Tempstr,"&refresh_token=",RefreshToken,NULL);
Tempstr=MCatStr(Tempstr,"&grant_type=","refresh_token",NULL);

S=HTTPMethod("POST",Tempstr,"","");
if (S)
{
Tempstr=STREAMReadLine(Tempstr,S);
while (Tempstr)
{
StripTrailingWhitespace(Tempstr);
OAuthParseJSON(Tempstr, Vars);

Tempstr=STREAMReadLine(Tempstr,S);
}
} 

/*
  "access_token" : "ya29.AHES6ZR-OrkfuNOV_npDtM8EzICJgIfIWnHB8Dz-gU0Zp7zAXW2M",
  "token_type" : "Bearer",
  "expires_in" : 3600,
  "id_token" : "eyJhbGciOiJSUzI1NiJ9.eyJpc3MiOiJhY2NvdW50cy5nb29nbGUuY29tIiwiYXVkIjoiNDMzNTAyNDEyMDU2LmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwiY2lkIjoiNDMzNTAyNDEyMDU2LmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tIiwiaWQiOiIxMTA2NDEwMTM2MDkyMTQzNDQ0NDIiLCJ0b2tlbl9oYXNoIjoiOG9tVjRrOEs5NHJ5ZnUycldsZ014USIsImlhdCI6MTM0MjEyMjIzMiwiZXhwIjoxMzQyMTI2MTMyfQ.xghQf20eKCKngwyt0a6rt0mX0R8L_6RscALNBw2vz9Hm86mFPzGJzFPWpt7jWiYI5N8De0cJ09U242TVqnkTo9mstH4u9IsgZizaAoXk7pxwLcq38Ul1GYx21LIVbP5aCee-oEmZtGFnKJTsR4LfYkzgDBKWvBCYwba-Mf27zvI",
  "refresh_token" : "1/JHmIOQx7reU8Q5PUIGn6JHAZFN8QdbeGThX-ZoW4ZO0"
*/


*AccessToken=CopyStr(*AccessToken,GetVar(Vars,"access_token"));
//*RefreshToken=CopyStr(*RefreshToken,GetVar(Vars,"refresh_token"));

ListDestroy(Vars,DestroyString);
DestroyString(Tempstr);
DestroyString(Encode);
}





