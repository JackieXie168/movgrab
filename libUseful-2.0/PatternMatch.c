#include "PatternMatch.h"

typedef enum {MATCH_FAIL, MATCH_FOUND, MATCH_ONE, MATCH_MANY, MATCH_CONT, MATCH_QUOT, MATCH_START, MATCH_CHARLIST, MATCH_HEX, MATCH_OCTAL, MATCH_SWITCH_ON, MATCH_SWITCH_OFF} TPMatchElements;

//Gets Called recursively
int pmatch_char(char **P_PtrPtr, char **S_PtrPtr, int *Flags);



int pmatch_ascii(char *P_Ptr,char S_Char,int Type)
{
char ValStr[4];
int P_Char=-1;

if (Type==MATCH_HEX) 
{
	strncpy(ValStr,P_Ptr,2);
	P_Char=strtol(P_Ptr,NULL,16);
}

if (Type==MATCH_OCTAL) 
{
	strncpy(ValStr,P_Ptr,3);
	P_Char=strtol(P_Ptr,NULL,8);
}

if (P_Char==-1) return(FALSE);

if (P_Char==S_Char) return(TRUE);

return(FALSE);
}


void pmatch_switch(char SwitchType, char SwitchOnOrOff, int *Flags)
{
int NewFlag=0, OnOrOff=FALSE;

if (SwitchOnOrOff==MATCH_SWITCH_ON) OnOrOff=TRUE;
else if (SwitchOnOrOff==MATCH_SWITCH_OFF) OnOrOff=FALSE; 
else return;

switch (SwitchType)
{
//These switches have the opposite meaning to the flags they control.
//+C turns case sensitivity on, but the flag is 'PMATCH_NOCASE' that
//turns it off, so we need to invert the sense of 'OnOrOff'
case 'C': NewFlag=PMATCH_NOCASE; OnOrOff= !OnOrOff; break;
case 'W': NewFlag=PMATCH_NOWILDCARDS; OnOrOff= !OnOrOff; break;
case 'X': NewFlag=PMATCH_NOEXTRACT; OnOrOff= !OnOrOff; break;

case 'N': NewFlag=PMATCH_NEWLINEEND; break;
}

if (OnOrOff) *Flags |= NewFlag;
else *Flags &= ~NewFlag;
}



int pmatch_quot(char **P_PtrPtr, char **S_PtrPtr, int *Flags)
{
int result=MATCH_FAIL, OldFlags;
char P_Char, S_Char, *OldPos;

P_Char=**P_PtrPtr;
S_Char=**S_PtrPtr;

switch (P_Char)
{
	case 'b': if (S_Char=='\b') result=MATCH_ONE; break;
	case 'e': if (S_Char==27) result=MATCH_ONE; break; //escape
	case 'n': if (S_Char=='\n') result=MATCH_ONE; break;
	case 'r': if (S_Char=='\r') result=MATCH_ONE; break;
	case 't': if (S_Char=='	') result=MATCH_ONE; break;
	case 'l': if (islower(S_Char)) result=MATCH_ONE; break;
	case 'x': result=MATCH_HEX; break;
	case 'A': if (isalpha(S_Char)) result=MATCH_ONE; break;
	case 'B': if (isalnum(S_Char)) result=MATCH_ONE; break;
	case 'D': if (isdigit(S_Char)) result=MATCH_ONE; break;
	case 'S': if (isspace(S_Char)) result=MATCH_ONE; break;
	case 'P': if (ispunct(S_Char)) result=MATCH_ONE; break;
	case 'X': if (isxdigit(S_Char)) result=MATCH_ONE; break;
	case 'U': if (isupper(S_Char)) result=MATCH_ONE; break;
	case '+': result=MATCH_SWITCH_ON; break;
	case '-': result=MATCH_SWITCH_OFF; break;
	
	default: if (S_Char==P_Char) result=MATCH_ONE; break;
}

switch (result)
{
	case MATCH_ONE:
	(*P_PtrPtr)++;
	break;

	case MATCH_HEX:
		if (! pmatch_ascii((*P_PtrPtr)+1,S_Char,MATCH_HEX)) return(MATCH_FAIL);
		(*P_PtrPtr)+=2;
	break;

	case MATCH_OCTAL:
		if (! pmatch_ascii((*P_PtrPtr)+1,S_Char,MATCH_OCTAL)) return(MATCH_FAIL);
		(*P_PtrPtr)+=3;
	break;

	case MATCH_SWITCH_ON:
	case MATCH_SWITCH_OFF:


		//some switches need to be applied in order for a pattern to match 
		//(like the case-insensitive switch) others should only be applied if
		//it matches. So we apply the switch, but if the subsequent pmatch_char fails
		//we unapply it
		OldFlags=*Flags;
		OldPos=*P_PtrPtr;
		(*P_PtrPtr)++; //go past the + or - to the actual type
		pmatch_switch(**P_PtrPtr, result, Flags);
		(*P_PtrPtr)++;
		result=pmatch_char(P_PtrPtr, S_PtrPtr, Flags);

		if ((result==MATCH_FAIL) || (result==MATCH_CONT))
		{
			*P_PtrPtr=OldPos;
			*Flags=OldFlags;
		}
		return(result);
	break;

	case MATCH_FAIL:
		if (*Flags & PMATCH_SUBSTR) return(MATCH_CONT);
		return(MATCH_FAIL);
	break;
}

return(MATCH_ONE);
}


#define CHARLIST_NOT 1

int pmatch_charlist(char **P_PtrPtr,char S_Char, int Flags)
{
char P_Char, Prev_Char;
int result=MATCH_CONT;
int mode=0;


while (**P_PtrPtr != '\0')
{
	if (Flags & PMATCH_NOCASE) P_Char=tolower(**P_PtrPtr);
	else P_Char=**P_PtrPtr;

	if (P_Char==']') break;

	switch (P_Char)
	{
	case '\\': 
		(*P_PtrPtr)++;
		if (Flags & PMATCH_NOCASE) P_Char=tolower(**P_PtrPtr);
		else P_Char=**P_PtrPtr;
	break;

	case '-':
		(*P_PtrPtr)++;
		if (Flags & PMATCH_NOCASE) P_Char=tolower(**P_PtrPtr);
		else P_Char=**P_PtrPtr;
	
		if ((S_Char >= Prev_Char) && (S_Char <= P_Char)) result=MATCH_ONE;
	break;

	case '!':
		mode |= CHARLIST_NOT;
	break;

	default:
		if (P_Char == S_Char) result=MATCH_ONE;
	break;
	}
	
	Prev_Char=P_Char;
	(*P_PtrPtr)++;
}

//go beyond ']'
(*P_PtrPtr)++;

if (mode & CHARLIST_NOT) 
{
	if (result==MATCH_ONE) result=MATCH_CONT;
	else result=MATCH_ONE;
}

return(result);
}



int pmatch_char(char **P_PtrPtr, char **S_PtrPtr, int *Flags)
{
char P_Char, S_Char;

if (*Flags & PMATCH_NOCASE)
{
	P_Char=tolower(**P_PtrPtr);
	S_Char=tolower(**S_PtrPtr);
}
else
{
	P_Char=**P_PtrPtr;
	S_Char=**S_PtrPtr;
}

//we must still honor switches even if 'nowildcards' is set, as we may want to turn
//'nowildcards' off, or turn case or extraction features on or off
if (*Flags & PMATCH_NOWILDCARDS) 
{
	if (
				(P_Char=='\\') && 
				((*(*P_PtrPtr+1)=='+') || (*(*P_PtrPtr+1)=='-'))
		) /*This is a switch, fall through and process it */ ;
	else if (P_Char==S_Char) return(MATCH_ONE);
	else return(MATCH_FAIL);
}

switch (P_Char)
{
	case '\0': return(MATCH_FOUND); break;
	case '|': return(MATCH_FOUND); break;
	case '*': return(MATCH_MANY);break;
	case '?': (*P_PtrPtr)++; return(MATCH_ONE);break;
	case '^': (*P_PtrPtr)++; return(MATCH_START); break;
	case '$':
		if (S_Char=='\0') return(MATCH_FOUND); 
		if ((*Flags & PMATCH_NEWLINEEND) && (S_Char=='\n')) return(MATCH_FOUND);
	break;
	case '[': (*P_PtrPtr)++; return(pmatch_charlist(P_PtrPtr,S_Char,*Flags)); break;

	case '\\': 
			//results here can either be MATCH_FAIL, MATCH_CONT, MATCH_ONE 
			(*P_PtrPtr)++;
			return (pmatch_quot(P_PtrPtr, S_PtrPtr, Flags));
	break;

	default:
		(*P_PtrPtr)++;
		if (P_Char==S_Char) return(MATCH_ONE);
		if (*Flags & PMATCH_SUBSTR) return(MATCH_CONT);
		return(MATCH_FAIL);
	break;
}


return(MATCH_FAIL);
}



int pmatch_process(char *Pattern, char *String, char **Start, char **End, int Flags)
{
char *P_Ptr, *S_Ptr, *ptr;
int result;


	P_Ptr=Pattern;
	S_Ptr=String;
	if (Start) *Start=NULL;
	if (End) *End=NULL;


	result=pmatch_char(&P_Ptr, &S_Ptr, &Flags);
	while (*S_Ptr)
	{
		switch (result)
		{
		case MATCH_FAIL: return(FALSE); break;

		//Match failed, but we're looking for a substring of 'String' so continue searching for match
		case MATCH_CONT: 
			P_Ptr=Pattern; 
			if (Start) *Start=NULL;
			if (End) *End=NULL;
		break;

		case MATCH_START:
			if (S_Ptr != String) return(FALSE);
			if (Flags & PMATCH_NOTSTART) return(FALSE);
			if (! (Flags & PMATCH_NOEXTRACT))
			{
				if (Start && (! *Start)) *Start=S_Ptr;
			}

			S_Ptr--; //naughty, were are now pointing before String, but the
							 //S_Ptr++ below will correct this
		break;


		case MATCH_FOUND: 
			if (End && (! (Flags & PMATCH_NOEXTRACT))) 
			{
				*End=S_Ptr;
			}
			return(TRUE); 
		break;
		
		//Match many is a special case. We have too look ahead to see if the next char
		//Matches, and thust takes us out of 'match many' howerver, If the call to pmatch_char 
		//fails then we have to rewind the pattern pointer to match many's '*' and go round again
		case MATCH_MANY:
			ptr=P_Ptr;
			P_Ptr++;
			result=pmatch_char(&P_Ptr, &S_Ptr, &Flags);
			if (result==MATCH_FAIL) P_Ptr=ptr;
			else if (result==MATCH_CONT) P_Ptr=ptr;
			else if (! (Flags & PMATCH_NOEXTRACT))
			{
				if (Start && (! *Start)) *Start=S_Ptr;
			}
		break;

		case MATCH_ONE:
			if (! (Flags & PMATCH_NOEXTRACT))
			{
				if (Start && (! *Start)) *Start=S_Ptr;
			}
		break;
		}

		S_Ptr++;	
		if ((End) && (! (Flags & PMATCH_NOEXTRACT))) *End=S_Ptr;
		
		result=pmatch_char(&P_Ptr, &S_Ptr, &Flags);
	}

//if pattern not exhausted then we didn't get a match
if (*P_Ptr) return(FALSE);


return(TRUE);
}


int pmatch(char *Pattern, char *String, char **Start, char **End, int Flags)
{
char *ptr;

  //deal with the easy situation first
  if (StrLen(Pattern)==0)
  {
	 if (StrLen(String)==0) return(TRUE);
	 else return(FALSE);
  }

	ptr=Pattern;	
	while (ptr) 
	{
		if (pmatch_process(ptr, String, Start, End, Flags)) return(TRUE);
		ptr=strchr(ptr+1,'|');
		if (ptr) ptr++;
	}

	return(FALSE);
}
