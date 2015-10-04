#ifndef LIBUSEFUL_VARS_H
#define LIBUSEFUL_VARS_H

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

void SetVar(ListNode *Vars, char *Name, char *Data);
char *GetVar(ListNode *Vars, char *Name);
void UnsetVar(ListNode *Vars,char *Name);
void ClearVars(ListNode *Vars);
void CopyVars(ListNode *Dest, ListNode *Source);
char *SubstituteVarsInString(char *Buffer, char *Fmt, ListNode *Vars, int Flags);
int ExtractVarsFromString(char *Data, char *FormatStr, ListNode *Vars);


#ifdef __cplusplus
}
#endif

#endif
