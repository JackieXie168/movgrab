#ifndef LIBUSEFUL_MATCHPATTERN
#define LIBUSEFUL_MATCHPATTERN

#define PMATCH_SUBSTR 1
#define PMATCH_NOWILDCARDS 2
#define PMATCH_NOCASE 4
#define PMATCH_NOEXTRACT 8
#define PMATCH_NEWLINEEND 16
#define PMATCH_NOTSTART 32
#define PMATCH_NOTEND 64

#include "includes.h"

#ifdef __cplusplus
extern "C" {
#endif

int pmatch(char *Pattern, char *String, char **Start, char **End, int Flags);

#ifdef __cplusplus
}
#endif

#endif
