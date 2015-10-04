
#ifndef MOVGRAB_OUTPUT_FILES
#define MOVGRAB_OUTPUT_FILES

#include "common.h"

STREAM *OpenSaveFile(char *Title, char *URL, int *FileSize);
STREAM *OpenCacheFile(char *Title, char *URL);

#endif
