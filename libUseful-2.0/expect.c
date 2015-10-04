#include "includes.h"
#include "expect.h"


//Values for 'flags' that are not visible to the user
//These must not clash with any visible values
#define DIALOG_DONE 67108864


void DialogAdd(ListNode *Dialogs, char *Expect, char *Reply, int Flags)
{
TDialog *Dialog;

Dialog=(TDialog *) calloc(1,sizeof(TDialog));
Dialog->Expect=CopyStr(Dialog->Expect,Expect);
Dialog->Reply=CopyStr(Dialog->Reply,Reply);
Dialog->Flags=Flags;

ListAddItem(Dialogs,Dialog);
}


int STREAMDialog(STREAM *S, ListNode *Dialogs)
{
int len=0, inchar;
ListNode *Curr;
TDialog *Dialog;

inchar=STREAMReadChar(S);
while (inchar !=EOF)
{
  if (inchar > 0)
  {
    Curr=ListGetNext(Dialogs);
    while (Curr)
    {
      Dialog=(TDialog *) Curr->Item;
			if (! (Dialog->Flags & DIALOG_DONE))
			{
      //if the current value does not equal where we are in the string
      //we have to consider whether it is the first character in the string
      if (Dialog->Expect[Dialog->Match]!=inchar) Dialog->Match=0;

      if (Dialog->Expect[Dialog->Match]==inchar)
      {
        Dialog->Match++;
        if (Dialog->Expect[Dialog->Match]=='\0')
        {
          Dialog->Match=0;
					Dialog->Flags |= DIALOG_DONE;
          if (Dialog->Reply) STREAMWriteLine(Dialog->Reply,S);
          if (Dialog->Flags & DIALOG_END) return(TRUE);
          if (Dialog->Flags & DIALOG_FAIL) return(FALSE);
        }
      }

			if (! (Dialog->Flags & DIALOG_OPTIONAL)) break;
			}
      Curr=ListGetNext(Curr);
    }
  }
inchar=STREAMReadChar(S);
}

return(FALSE);
}


int STREAMExpectAndReply(STREAM *S, char *Expect, char *Reply)
{
int match=0, len=0, inchar;

len=StrLen(Expect);
inchar=STREAMReadChar(S);
while (inchar !=EOF)
{
  if (inchar > 0)
  {
  //if the current value does not equal where we are in the string
  //we have to consider whether it is the first character in the string
  if (Expect[match]!=inchar) match=0;

  if (Expect[match]==inchar)
  {
    match++;
    if (match==len)
    {
      if (Reply) STREAMWriteLine(Reply,S);
      return(TRUE);
    }
  }

  }
inchar=STREAMReadChar(S);
}

return(FALSE);
}


int STREAMExpectSilence(STREAM *S, int wait)
{
int inchar;
char *Tempstr=NULL;
int len=0, Timeout;

Timeout=S->Timeout;
S->Timeout=wait;
inchar=STREAMReadChar(S);
while (inchar  > 0)
{
  Tempstr=AddCharToBuffer(Tempstr,len,inchar);
  len++;
  inchar=STREAMReadChar(S);
}

S->Timeout=Timeout;

DestroyString(Tempstr);

return(FALSE);
}
