#include "dailymotion.h"

char *GetDailyMotionFormat(char *Formats, char **Val)
{
char *Token=NULL, *Fmt=NULL, *ptr;
char *ptr2;

	*Val=CopyStr(*Val,"");
	ptr=GetToken(Formats,",",&Fmt,0);

	ptr2=GetToken(Fmt,":",&Token,GETTOKEN_QUOTES);
	ptr2=GetToken(ptr2,":",&Token,GETTOKEN_QUOTES);
	if (strncmp(Token,"http",4) ==0) *Val=DeQuoteStr(*Val,Token);

DestroyString(Token);
DestroyString(Fmt);

return(ptr);
}


char *DecodeDailyMotionFormats(char *Formats, ListNode *Vars)
{
char *Token=NULL, *ptr;

	ptr=GetDailyMotionFormat(Formats,&Token);
	while (ptr)
	{
	if (strstr(Token,"FLV-320x240")) 
	{
		SetVar(Vars,"item:flv:320x240",Token);
	}
	else if (strstr(Token,"FLV-80x60")) 
	{
		SetVar(Vars,"item:flv:80x60",Token);
	}
	else if (strstr(Token,"ON2-848x480"))
	{
			SetVar(Vars,"item:on2:848x480",Token);
	}
	else if (strstr(Token,"ON2-320x240")) 
	{
			SetVar(Vars,"item:on2:320x240",Token);
	}
	else if (strstr(Token,"H264-848x480")) 
	{
		SetVar(Vars,"item:mp4-h264:848x480",Token);
	}
	else if (strstr(Token,"H264-512x384")) 
	{
			SetVar(Vars,"item:mp4-h264:512x384",Token);
	}
	else if (strstr(Token,"H264-1280x720")) 
	{
			SetVar(Vars,"item:mp4-h264:1280x720",Token);
	}
	else if (StrLen(Token)) fprintf(stderr,"Unknown DailyMotion Format: [%s]\n",Token);

	ptr=GetDailyMotionFormat(ptr,&Token);
	}

DestroyString(Token);

return(ptr);
}

