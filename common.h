
#ifndef MOVGRAB_COMMON
#define MOVGRAB_COMMON
//This is doable through autoconf, but I'm sick of fighting with it
#define Version "1.1.0"

#include "libUseful-2.0/libUseful.h"
#include <string.h>
#include <glob.h>
#include "extract_text.h"

#define FLAG_QUIET 1
#define FLAG_BACKGROUND 2
#define FLAG_DEBUG1 4
#define FLAG_DEBUG2 8
#define FLAG_DEBUG3 16
#define FLAG_DEBUG (FLAG_DEBUG1 | FLAG_DEBUG2 | FLAG_DEBUG3)
#define FLAG_PORN 32
#define FLAG_PRINT_USAGE 64
#define FLAG_HTTPS 128
#define FLAG_TEST  256
#define FLAG_STDIN 512
#define FLAG_POST  1024
#define FLAG_TEST_SITES 4096
#define FLAG_STREAM 8192
#define FLAG_STREAMCACHE 16384
#define FLAG_RETRY_DOWNLOAD 32768


extern int Type, DefaultPort, Flags;
extern char *SaveFilePath;
extern char *FileTypes[];

#endif
