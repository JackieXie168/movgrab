
#ifndef MOVGRAB_OUTPUT_FILES
#define MOVGRAB_OUTPUT_FILES

#include "common.h"

STREAM *OpenSaveFile(char *Title, char *URL, int *FileSize);
STREAM *OpenCacheFile(char *Title, char *URL);
char *OutputFilesGetFilePath();
void OpenOutputFiles(char *Title, char *URL, int *FileSize);
void WriteOutputFiles(char *Data, int Len);
void AddOutputFile(char *Path, int SingleOutput);
void CloseOutputFiles();

#endif
