
#ifndef LIBUSEFUL_EXPECT_H
#define LIBUSEFUL_EXPECT_H

#include "file.h"

#define FLAG_FINAL_DIALOG 1
#define FLAG_DIALOG_FAIL 2

typedef struct
{
int Flags;
int Match;
char *Expect;
char *Reply;
} TDialog;


#ifdef __cplusplus
extern "C" {
#endif

void DialogAdd(ListNode *Dialogs, char *Expect, char *Reply, int Flags);
int STREAMExpectAndReply(STREAM *S, char *Expect, char *Reply);
int STREAMExpectSilence(STREAM *S, int wait);
int STREAMDialog(STREAM *S, ListNode *Dialogs);


#ifdef __cplusplus
}
#endif


#endif
