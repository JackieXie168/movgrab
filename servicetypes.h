
#ifndef MOVGRAB_SERVICETYPES_H
#define MOVGRAB_SERVICETYPES_H

#include "common.h"

typedef enum {TYPE_NONE, TYPE_GENERIC, TYPE_YOUTUBE, TYPE_METACAFE, TYPE_DAILYMOTION, TYPE_BREAK_COM, TYPE_EHOW,  TYPE_VIMEO, TYPE_FIVE_MIN, TYPE_VBOX7,TYPE_BLIPTV,TYPE_TED, TYPE_MYVIDEO, TYPE_CLIPSHACK, TYPE_MYTOPCLIP,TYPE_REDBALCONY, TYPE_MOBANGO, TYPE_YALE, TYPE_PRINCETON, TYPE_REUTERS, TYPE_LIVELEAK, TYPE_ACADEMIC_EARTH,TYPE_PHOTOBUCKET,TYPE_VIDEOEMO,TYPE_ALJAZEERA,TYPE_MEFEEDIA,TYPE_IVIEWTUBE,TYPE_WASHINGTONPOST,TYPE_CBSNEWS,TYPE_FRANCE24,TYPE_EURONEWS,TYPE_METATUBE,TYPE_MOTIONFEEDS,TYPE_ESCAPIST,TYPE_GUARDIAN,TYPE_REDORBIT,TYPE_SCIVEE,TYPE_IZLESE, TYPE_UCTV, TYPE_ROYALSOCIETY, TYPE_BRITISHACADEMY, TYPE_KAVLIINSTITUTE,TYPE_DOTSUB,TYPE_ASTRONOMYCOM, TYPE_TEACHERTUBE, TYPE_DISCOVERY, TYPE_BLOOMBERG,TYPE_NATGEO,TYPE_VIDEOBASH,TYPE_IBTIMES,TYPE_SMH,TYPE_PRESSTV,TYPE_VIDEOJUG,TYPE_ANIMEHERE,
/*Following ones are not real types, but used by internal processes */
TYPE_METACAFE_JS_REDIR, TYPE_METACAFE_FINAL, TYPE_DAILYMOTION_STAGE2, TYPE_DAILYMOTION_STAGE3, TYPE_VIMEO_STAGE2, TYPE_VIMEO_STAGE3, TYPE_EHOW_STAGE2, TYPE_CLIPSHACK_STAGE2, TYPE_CLIPSHACK_STAGE3, TYPE_VIDEOEMO_STAGE2,TYPE_MYVIDO1_STAGE2, TYPE_REFERENCE, TYPE_WASHINGTONPOST_JSON, TYPE_WASHINGTONPOST_STAGE2, TYPE_BLIPTV_STAGE2, TYPE_BLIPTV_STAGE3, TYPE_ESCAPIST_STAGE2, TYPE_REDORBIT_STAGE2, TYPE_REDBALCONY_STAGE2,TYPE_BRITISHACADEMY_STAGE2, TYPE_KAVLIINSTITUTE_STAGE2,TYPE_ASTRONOMYCOM_STAGE2,TYPE_CONTAINERFILE,TYPE_TEACHERTUBE_STAGE2,TYPE_VIDEOJUG_STAGE2,TYPE_VIDEOJUG_STAGE3, TYPE_ANIMEHERE_STAGE2}TDT;

extern char *DownloadTypes[], *DownloadNames[], *TestLinks[];

int IdentifyServiceType(char *Server);
char *SiteSpecificPreprocessing(char *RetBuf, char *Path, char *Proto, char *Server, int Port, char *Doc, int Type, char **Title, int *Post);
int GetNextURL(int Type, char *Server, int Post, ListNode *Vars);
int ExtractItemInfo(STREAM *S, int Type, char *URL, char *Server, int Port, char *Title, int Post);


#endif
