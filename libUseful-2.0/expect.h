
#ifndef LIBUSEFUL_EXPECT_H
#define LIBUSEFUL_EXPECT_H

#include "file.h"

#define DIALOG_END 1
#define DIALOG_FAIL 2
#define DIALOG_OPTIONAL 4

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

int STREAMExpectAndReply(STREAM *S, char *Expect, char *Reply);
int STREAMExpectSilence(STREAM *S, int wait);

void DialogAdd(ListNode *Dialogs, char *Expect, char *Reply, int Flags);
int STREAMDialog(STREAM *S, ListNode *Dialogs);


#ifdef __cplusplus
}
#endif


#endif
