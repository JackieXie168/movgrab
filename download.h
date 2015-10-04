#ifndef MOVGRAB_DOWNLOAD
#define MOVGRAB_DOWNLOAD

#include "common.h"

int DownloadItem(char *URL, char *Path, int Flags);
int DownloadPage(char *Path, int Type, char *Title, int Flags);

#endif
