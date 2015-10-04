#ifndef MOVGRAB_DOWNLOAD
#define MOVGRAB_DOWNLOAD

#include "common.h"

int DownloadItem(char *URL, char *Format, char *Path, int Flags);
int DownloadPage(char *Path, int Type, char *Title, int Flags);

extern char *Player;
extern int PlayerLaunchPercent;

#endif
