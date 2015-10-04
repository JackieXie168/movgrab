#include "servicetypes.h"
#include "containerfiles.h"

/*
This file and it's header (servicetypes.h) holds all the functions and data
related to individual services/sites. When adding a site you need to add to
the arrays 'DownloadTypes', 'DownloadNames' and 'TestLinks' and to the 
enumerated type in servicetypes.h tht matches to 'DownloadTypes'.

Then site specific 
*/



//Site type names used at the command line etc
char *DownloadTypes[]={"none","generic","youtube","metacafe","dailymotion","break","ehow","vimeo","5min","vbox7","blip.tv","ted","myvideo","clipshack","mytopclip","redbalcony","mobango","yale","sdnhm","princeton","reuters","clipfish.de","liveleak","academicearth","photobucket","videoemo","videosfacebook","aljazeera","mefeedia","myvido1","iviewtube","washingtonpost","cbsnews","france24","euronews","metatube","motionfeeds","escapist","guardian","redorbit","scivee","izlese","uctv.tv","royalsociety.tv","britishacademy","kitp","dotsub","astronomy.com",NULL};

//Longer names used in display
char *DownloadNames[]={"none",
"Generic: Search in page for http://*.flv, http://*.mp3, http//*.mp4 etc, etc, etc",
"YouTube: http://www.youtube.com",
"Metacafe: http://www.metacafe.com",
"Daily Motion: http://www.dailymotion.com",
"www.break.com",
"www.ehow.com",
"www.vimeo.com",
"www.5min.com",
"www.vbox7.com",
"www.blip.tv",
"www.ted.com",
"www.myvideo.de",
"www.clipshack.com",
"www.mytopclip.com",
"www.redbalcony.com",
"www.mobango.com",
"Yale University: http://oyc.yale.edu",
"San Diago Natural History Museum: http://www.sdnhm.org/webcasts/index.html",
"Princeton University: http://www.princeton.edu/WebMedia/",
"Reuters: http://www.reuters.com/",
"clipfish.de",
"Liveleak: http://www.liveleak.com",
"Academic Earth: http://www.academicearth.org",
"Photobucket: http://www.photobucket.com",
"VideoEmo: http://www.vidoevo.com/",
"Videos Facebook: http://www.videosfacebook.net",
"Aljazeera: english.aljazeera.net",
"mefeedia.com",
"myvido1.com",
"iViewTube: www.iviewtube.com",
"Washington Post: www.washingtonpost.com",
"CBS News: www.cbsnews.com",
"France24: www.france24.com",
"Euronews: www.euronews.net",
"www.metatube.com",
"www.motionfeeds.com",
"www.escapistmagazine.com",
"www.guardian.co.uk",
"www.redorbit.com",
"www.scivee.tv",
"www.izlese.org",
"University of California Television: http://www.uctv.tv/",
"http://royalsociety.org/",
"http://britac.studyserve.com",
"KAVLI INSTITUTE: http://online.itp.ucsb.edu/plecture/",
"dotsub.com",
"astronomy.com",
NULL};

//links used by the -test-sites feature to test if a download site still
//works with movgrab
char *TestLinks[]={"", "",
"http://www.youtube.com/watch?v=oP59tQf_njc",
"http://www.metacafe.com/watch/6063075/how_to_use_chopsticks/",
"http://www.dailymotion.com/video/x5790e_hubblecast-16-galaxies-gone-wild_tech",
"http://www.break.com/index/ninja-cat.html",
"http://www.ehow.com/video_6819748_creamy-thyme-scrambled-eggs-recipe.html",
"http://vimeo.com/channels/knowyourmeme",
"http://www.5min.com/Video/Learn-About-Chocolate-516948033",
"http://www.vbox7.com/play:1417ad5a",
"http://blip.tv/file/4855647/",
"http://www.ted.com/talks/janine_benyus_shares_nature_s_designs.html",
"http://www.myvideo.de/watch/8045043#featuredvideo1",
"http://www.clipshack.com/Clip.aspx?key=F9F949CE15A12B50",
"http://www.mytopclip.com/videos/32501/flip-over-card-trick-revealed",
"http://www.redbalcony.com/?vid=23083",
"http://www.mobango.com/media_details/struck_in_tsunami/%21tKpzdHIo5E%3D/",
"http://oyc.yale.edu/economics/game-theory/contents/sessions/session-1-introduction-five-first-lessons",
"http://www.sdnhm.org/webcasts/lectures/scottsampson.html",
"http://www.princeton.edu/WebMedia/flash/lectures/20100428_publect_taibbi_tett.shtml",
"http://www.reuters.com/news/video/story?videoId=193471398&videoChannel=4",
"http://www.clipfish.de/musikvideos/video/3527775/daft-punk-derezzed/",
"http://www.liveleak.com/view?i=a7b_1299633723",
"http://www.academicearth.org/lectures/interaction-with-physical-environment",
"http://gs147.photobucket.com/groups/r299/QM25Y4IJEP/?action=view&current=BlackSwan1.mp4",
"http://www.vidoevo.com/yvideo.php?i=NXhxdEU0cWuRpZktqUGc&j-majik-wickaman-feat-dee-freer-in-pieces-drum-bass-mix",
"http://www.videosfacebook.net/sueper-best-dance-songs-2-aaaa-1",
"http://english.aljazeera.net/programmes/countingthecost/2011/02/2011219142816937101.html",
"http://www.mefeedia.com/video/36135458",
"http://www.myvido1.com/QWxgGMihFZEl1aOJlTy0UP_bob-marley-don-039t-worry-be-happy",
"http://www.iviewtube.com/v/152128/insane-hail-storm-oklahoma-city",
"http://www.washingtonpost.com/business/on-leadership-modern-day-machiavellis-/2011/02/18/ABYgckH_video.html",
"http://www.cbsnews.com/video/watch/?id=7357739n",
"http://www.france24.com/en/20110219-beyond-business-cybercriminality-iphone-mobile-security",
"http://www.euronews.net/news/bulletin/",
"http://www.metatube.com/en/videos/cid2/Funny-Videos/53210/Rail-Jump-Fail/",
"http://www.motionfeeds.com/10401/helsinki-travel-guide.html",
"http://www.escapistmagazine.com/videos/view/zero-punctuation/1776-Red-Dead-Redemption",
"http://www.guardian.co.uk/world/video/2011/may/13/fukushima-radiation-dairy-farmers-video",
"http://www.redorbit.com/news/video/science_2/2598565/unlocking_mysteries_of_sharks/index.html",
"http://www.scivee.tv/node/5300",
"http://www.izlese.org/hot-girl-hot-cars-911-turbo-vs-mercedes-s550-vs-rally-car.html",
"http://www.uctv.tv/search-details.aspx?showID=20888",
"http://royalsociety.tv/rsPlayer.aspx?presentationid=474",
"broken",
"http://online.itp.ucsb.edu/plecture/bmonreal11/",
"http://dotsub.com/view/5d90ef11-d5e5-42fb-8263-a4c128fb64df",
"http://www.astronomy.com/News-Observing/Liz%20and%20Bills%20Cosmic%20Adventures/2011/02/Episode%202.aspx",
NULL};


int SelectDownloadFormat(ListNode *Vars, int WebsiteType);


//Guess service type from servername in URL
int IdentifyServiceType(char *Server)
{
int Type=0;

if (
	(strstr(Server,"youtube")) 
   ) Type=TYPE_YOUTUBE;

if (strcmp(Server,"www.metacafe.com")==0)
{
 Type=TYPE_METACAFE;
}
else if (strcmp(Server,"www.break.com")==0)
{
 Type=TYPE_BREAK_COM;
}
else if (strstr(Server,".ehow."))
{
 Type=TYPE_EHOW;
}
else if (strcmp(Server,"www.dailymotion.com")==0)
{
 Type=TYPE_DAILYMOTION;
}
else if (strcmp(Server,"www.5min.com")==0)
{
 Type=TYPE_FIVE_MIN;
}

else if (strcmp(Server,"www.ted.com")==0)
{
 Type=TYPE_TED;
}
else if (strcmp(Server,"oyc.yale.edu")==0)
{
 Type=TYPE_YALE;
}
else if (strcmp(Server,"www.sdnhm.org")==0)
{
 Type=TYPE_SDNHM;
}
else if (strcmp(Server,"www.princeton.edu")==0)
{
 Type=TYPE_PRINCETON;
}
else if (strcmp(Server,"www.academicearth.org")==0)
{
 Type=TYPE_ACADEMIC_EARTH;
}
else if (strstr(Server,"photobucket.com"))
{
 Type=TYPE_PHOTOBUCKET;
}
else if (strstr(Server,"vbox7.com"))
{
 Type=TYPE_VBOX7;
}
else if (strstr(Server,"myvideo"))
{
 Type=TYPE_MYVIDEO;
}
else if (strstr(Server,"myvido1"))
{
 Type=TYPE_MYVIDO1;
}
else if (strstr(Server,"redbalcony.com"))
{
 Type=TYPE_REDBALCONY;
}
else if (strstr(Server,"mobango.com"))
{
 Type=TYPE_MOBANGO;
}
else if (strstr(Server,"izlese.org"))
{
 Type=TYPE_IZLESE;
}
else if (strstr(Server,"clipshack.com"))
{
 Type=TYPE_CLIPSHACK;
}
else if (strstr(Server,"mytopclip.com"))
{
 Type=TYPE_MYTOPCLIP;
}
else if (strstr(Server,"liveleak"))
{
 Type=TYPE_LIVELEAK;
}
else if (strstr(Server,"blip.tv"))
{
 Type=TYPE_BLIPTV;
}
else if (strstr(Server,"vimeo.com"))
{
 Type=TYPE_VIMEO;
}
else if (strstr(Server,"vidoevo.com"))
{
 Type=TYPE_VIDEOEMO;
}
else if (strstr(Server,"videosfacebook.net"))
{
 Type=TYPE_VIDEOSFACEBOOK;
}
else if (strstr(Server,"reuters"))
{
 Type=TYPE_REUTERS;
}
else if (strstr(Server,"clipfish.de"))
{
 Type=TYPE_CLIPFISH_DE;
}
else if (strstr(Server,"mefeedia.com"))
{
 Type=TYPE_MEFEEDIA;
}
else if (strstr(Server,"ucsd.tv"))
{
 Type=TYPE_UCSDTV;
}
else if (strstr(Server,"uctv.tv"))
{
 Type=TYPE_UCSDTV;
}
else if (strstr(Server,"aljazeera.net"))
{
 Type=TYPE_ALJAZEERA;
}
else if (strstr(Server,"iviewtube.com"))
{
 Type=TYPE_IVIEWTUBE;
}
else if (strstr(Server,"washingtonpost"))
{
 Type=TYPE_WASHINGTONPOST;
}
else if (strstr(Server,"cbsnews"))
{
 Type=TYPE_CBSNEWS;
}
else if (strstr(Server,"france24"))
{
 Type=TYPE_FRANCE24;
}
else if (strstr(Server,"euronews"))
{
 Type=TYPE_EURONEWS;
}
else if (strstr(Server,"metatube"))
{
 Type=TYPE_METATUBE;
}
else if (strstr(Server,"motionfeeds"))
{
 Type=TYPE_MOTIONFEEDS;
}
else if (strstr(Server,"guardian"))
{
 Type=TYPE_GUARDIAN;
}
else if (strstr(Server,"redorbit"))
{
 Type=TYPE_REDORBIT;
}
else if (strstr(Server,"scivee"))
{
 Type=TYPE_SCIVEE;
}
else if (strstr(Server,"escapistmagazine.com"))
{
 Type=TYPE_ESCAPIST;
}
else if (strcmp(Server,"royalsociety.tv")==0)
{
 Type=TYPE_ROYALSOCIETY;
}
else if (strcmp(Server,"britac.studyserve.com")==0)
{
 Type=TYPE_BRITISHACADEMY;
}
else if (strcmp(Server,"online.itp.ucsb.edu")==0)
{
 Type=TYPE_KAVLIINSTITUTE;
}
else if (strcmp(Server,"dotsub.com")==0)
{
 Type=TYPE_DOTSUB;
}
else if (strstr(Server,"astronomy.com"))
{
 Type=TYPE_ASTRONOMYCOM;
}
return(Type);
}



//A couple of old functions related to specific sites. As these sites no longer
//work it is likely that these functions will be removed some day
int DoVBOX7(char *ID, char *Title)
{
char *Dir=NULL, *Tempstr=NULL;
int i, RetVal=FALSE;

Dir=CopyStrLen(Dir,ID,2);

for (i=1; i < 50; i++)
{
	if (i==0) Tempstr=FormatStr(Tempstr,"http://media.vbox7.com/s/%s/%s.flv",Dir,ID);
  else Tempstr=FormatStr(Tempstr,"http://media%02d.vbox7.com/s/%s/%s.flv",i,Dir,ID);
  if (DownloadItem(Tempstr, Title, Flags))
	{
		RetVal=TRUE;
		 break;
	}
}

DestroyString(Dir);
DestroyString(Tempstr);

return(RetVal);
}


char *ExtractMetacafeMediaURL(char *RetStr, char *Data, char *Start, char *End)
{
char *Tempstr=NULL, *Token=NULL, *ptr;


		ptr=strstr(Data,Start);
		ptr+=StrLen(Start);
		ptr=GetToken(ptr,End,&Token,0);
		Tempstr=HTTPUnQuote(Tempstr,Token);
		RetStr=MCopyStr(RetStr,Tempstr,"?__gda__=",NULL);
		ptr=GetToken(Data,"gdaKey=",&Token,0);
		ptr=GetToken(ptr,"&",&Token,0);
		RetStr=CatStr(RetStr,Token);
	
DestroyString(Tempstr);
DestroyString(Token);


return(RetStr);
}


// These functions used by escapist magazine, but the method might be more 
// generic than just that one side
void ParseFlashPlaylistItem(char *Data,ListNode *Vars)
{
char *Name=NULL, *Value=NULL, *URL=NULL, *Type=NULL, *ptr;

strrep(Data,'\'',' ');
ptr=GetNameValuePair(Data,",",":",&Name,&Value);
while (ptr)
{
StripTrailingWhitespace(Name);
StripLeadingWhitespace(Name);
StripQuotes(Name);

StripTrailingWhitespace(Value);
StripLeadingWhitespace(Value);
StripQuotes(Value);


if (StrLen(Name))
{
  if (strcmp(Name,"eventCategory")==0) Type=CopyStr(Type,Value);
  if (strcmp(Name,"url")==0) URL=CopyStr(URL,Value);
}

ptr=GetNameValuePair(ptr,",",":",&Name,&Value);
}

if (StrLen(Type) && (strcmp(Type,"Video")==0)) SetVar(Vars,"item:mp4",URL);

DestroyString(Name);
DestroyString(Value);
DestroyString(URL);
DestroyString(Type);

}


void ParseFlashPlaylist(char *Data, ListNode *Vars)
{
char *Tempstr=NULL, *Item=NULL,  *ptr;

//This clips the playlist structure out into Tempstr
ptr=GetToken(Data,"{'playlist':[",&Tempstr,0);
ptr=GetToken(ptr,"]",&Tempstr,0);

ptr=GetToken(Tempstr,"{",&Item,0);
while (ptr)
{
//'Item' no contains one playlist item
ptr=GetToken(ptr,"}",&Item,0);
ParseFlashPlaylistItem(Item,Vars);

ptr=GetToken(ptr,"{",&Item,0);
}

DestroyString(Tempstr);
DestroyString(Item);
}



//This function is called before we even pull the first page from a site
//it is a good place to do any site-specific stuff like rewriting the
//site URL to a form that's better for movgrab
char *SiteSpecificPreprocessing(char *RetBuff, char *Path, char *Proto, char *Server, int Port, char *Doc, int Type, char **Title, int *Flags)
{
char *Tempstr=NULL;
char *NextPath=NULL;
char *ptr, *Token=NULL;

NextPath=CopyStr(RetBuff,Path);

if (Type==TYPE_YOUTUBE)
{

	//hmm.. have we been given the http//www.youtube.com/v/ format?
	if (strncmp(Doc,"v/",2)==0) Token=CopyStr(Token,Doc+2);
	else Token=CopyStr(Token,Doc+8);

	NextPath=MCopyStr(NextPath,"http://www.youtube.com/get_video_info?&video_id=",Token,"&el=detailpage&ps=default&eurl=&gl=US&hl=enB",NULL);

	//Do we have authentication info?
	//if (StrLen(Username) && StrLen(Password)) YoutubeLogin(Username,Password);

}
else if (Type==TYPE_METACAFE)
{
	ptr=GetToken(Doc,"watch/",&Token,0);
	ptr=GetToken(ptr,"/",&Token,0);
	if (strchr(Token,'-'))
	{
		//Movie is embedded from elsewhere
		if (strncmp(Token,"yt",2)==0) 
		{
			ptr=strrchr(Token,'-');
			if (ptr) 
			{
				ptr++;
	//			Type=TYPE_NONE;
	//			Tempstr=MCopyStr(Tempstr,"http://www.youtube.com/watch?v=",ptr,NULL);
	//			GrabMovie(Tempstr);

				Type=TYPE_YOUTUBE;
				NextPath=MCopyStr(NextPath,"http://www.youtube.com/watch?v=",ptr,NULL);
	
			}
		}
		else if (! (*Flags & FLAG_QUIET)) fprintf(stderr,"Movie is not genuinely stored on metacafe, it is embedded from elsewhere, but it's not youtube, so I don't know how to download it. Sorry.\n");
	}
	else 
	{
		STREAM *S;

		if (*Flags & FLAG_PORN)
		{
		//Initial 'turn off family filter'
		Tempstr=FormatStr(Tempstr,"%s://%s:%d/f/index.php?inputType=filter&controllerGroup=user&filters=0&submit=Continue+-+I%27m+over+18",Proto,Server,Port);
		S=HTTPMethod("POST",Tempstr,"","");
		STREAMClose(S);

		//But we have to do it twice, probably something to do with cookies
		NextPath=FormatStr(NextPath,"http://%s:%d/f/index.php?inputType=filter&controllerGroup=user&filters=0&Continue=Continue+-+I%27m+over+18&prevURL=%s",Server,Port,Path);
		}
		else NextPath=CopyStr(NextPath,Path);
	}
}
else if (Type==TYPE_DAILYMOTION)
{
	if (*Flags & FLAG_PORN)
	{
		Tempstr=HTTPQuote(Tempstr,Doc);
		NextPath=FormatStr(NextPath,"http://%s:%d/family_filter?urlback=/%s&enable=false",Server,Port,Tempstr);
   *Flags |= FLAG_POST;
	}
	else NextPath=CopyStr(NextPath,Path);
}
else if (Type==TYPE_PRINCETON)
{
	ptr=strrchr(Doc,'/');
	if (ptr)
	{
		ptr++;
		*Title=CopyStr(*Title,ptr);
		ptr=strrchr(*Title,'.');
		if (ptr) *ptr='\0';
	}
 	NextPath=CopyStr(NextPath,Path);
}
else if (Type==TYPE_CLIPFISH_DE)
{
	ptr=strstr(Doc,"video/");
	if (ptr)
	{
		ptr+=6;
 		Tempstr=CopyStr(Tempstr,ptr);
		ptr=strchr(Tempstr,'/');
		if (ptr)
		{
			 *ptr='\0';
			ptr++;
			*Title=CopyStr(*Title,ptr);
			ptr=strchr(*Title,'/');
			if (ptr) *ptr='\0';
		}
		NextPath=MCopyStr(NextPath,"http://www.clipfish.de/video_n.php?p=0|DE&vid=",Tempstr,NULL);
	}
}
else
{
 NextPath=CopyStr(NextPath,Path);
}

DestroyString(Tempstr);
DestroyString(Token);

return(NextPath);
}


//Called after the first page has been downloaded from the site
//Decides what to do next (Download another page, download the actual 
//media item, give up, etc.

int GetNextURL(int Type, char *Server, int Flags, ListNode *Vars)
{
char *Tempstr=NULL, *Title=NULL, *Fmt=NULL, *ptr;
int RetVal=FALSE;

Title=CopyStr(Title,GetVar(Vars,"Title"));
Fmt=CopyStr(Fmt,GetVar(Vars,"DownloadFormat"));

switch (Type)
{
case TYPE_REFERENCE:
	Tempstr=CopyStr(Tempstr,GetVar(Vars,"ID"));
  if (GetContainerFileType(Tempstr) != -1)	RetVal=DownloadContainer(Tempstr, Title, Flags);
	else RetVal=GrabMovie(Tempstr,TYPE_NONE);
break;

case TYPE_YOUTUBE:
	Tempstr=CopyStr(Tempstr,GetVar(Vars,"ID"));
  	RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;

case TYPE_BREAK_COM:
  Tempstr=SubstituteVarsInString(Tempstr,"$(ID)?$(EXTRA)",Vars,0);
  	RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;

case TYPE_EHOW:
	ptr=GetVar(Vars,"ID");
	if (strncmp(ptr,"http:",5)==0) RetVal=DownloadItem(ptr,Title, Fmt, Flags);
	else
	{
		Tempstr=SubstituteVarsInString(Tempstr,"http://$(Server):$(Port)/embedvars.aspx?isEhow=true&show_related=true&id=$(ID)",Vars,0);
  	RetVal=DownloadPage(Tempstr,TYPE_EHOW_STAGE2, Title,Flags);
	}
break;

case TYPE_EHOW_STAGE2:
  	RetVal=DownloadItem(GetVar(Vars,"ID"), Title, Fmt, Flags);
break;


case TYPE_METACAFE:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)&$(METACAFE_OVER_18)",Vars,0);
  	RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;


case TYPE_METACAFE_JS_REDIR:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)&$(METACAFE_OVER_18)",Vars,0);
 	RetVal=DownloadPage(Tempstr,TYPE_METACAFE,Title,Flags);
break;


case TYPE_METACAFE_FINAL:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)&$(METACAFE_OVER_18)",Vars,0);
  	RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;

case TYPE_DAILYMOTION:
	Flags &= ~FLAG_POST;
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)&allowFullScreen=true&allowScriptAccess=always&callback=player_proxy&lang=en&autoplay=1&uid=$(Extra)",Vars,0);
  	RetVal=DownloadItem(GetVar(Vars,"ID"), Title, Fmt, Flags);
break;

case TYPE_MOBANGO:
 Tempstr=SubstituteVarsInString(Tempstr,"http://media.mobango.com/$(ID)",Vars,0);
  	RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;

case TYPE_VIDEOEMO:
 	RetVal=DownloadPage(GetVar(Vars,"ID"),TYPE_VIDEOEMO_STAGE2,Title,Flags);
break;

case TYPE_WASHINGTONPOST_JSON:
 	RetVal=DownloadPage(GetVar(Vars,"ID"),TYPE_WASHINGTONPOST_STAGE2,Title,Flags);
break;

case TYPE_WASHINGTONPOST_STAGE2:
	Tempstr=SubstituteVarsInString(Tempstr,"$(server)$(flvurl)",Vars,0);
 	RetVal=DownloadItem(Tempstr,Title, Fmt, Flags);
break;

case TYPE_TED:
 Tempstr=SubstituteVarsInString(Tempstr,"http://video.ted.com/$(ID)",Vars,0);
  	RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;

case TYPE_MYVIDEO:
		Tempstr=SubstituteVarsInString(Tempstr,"$(MyVidURL)/$(ID).flv",Vars,0);
  	RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;

case TYPE_VBOX7:
	RetVal=DoVBOX7(GetVar(Vars,"ID"), Title);
break;

case TYPE_MYVIDO1:
 	RetVal=DownloadPage(GetVar(Vars,"ID"),TYPE_MYVIDO1_STAGE2,Title,Flags | FLAG_RETRY_DOWNLOAD);
break;

case TYPE_VIMEO:
 Tempstr=SubstituteVarsInString(Tempstr,"http://$(Server):$(Port)/moogaloop/load/clip:$(ID)/embed?param_fullscreen=1&param_clip_id=$(ID)&param_show_byline=0&param_server=vimeo.com&param_color=cc6600&param_show_portrait=0&param_show_title=1",Vars,0);
 	RetVal=DownloadPage(Tempstr,TYPE_VIMEO_STAGE2,Title,FLAG_POST);
break;

case TYPE_VIMEO_STAGE2:
 Tempstr=SubstituteVarsInString(Tempstr,"http://$(Server):$(Port)/moogaloop/play/clip:$(ID)/$(Extra)/$(Extra2)/?q=sd&type=embed",Vars,0);
 	RetVal=DownloadItem(Tempstr,Title, Fmt, Flags);
break;


case TYPE_YALE:
 Tempstr=SubstituteVarsInString(Tempstr,"http://openmedia.yale.edu/cgi-bin/open_yale/media_downloader.cgi?file=$(ID)",Vars,0);
 	RetVal=DownloadItem(Tempstr,Title, Fmt, Flags);
break;

case TYPE_CLIPSHACK:
 Tempstr=SubstituteVarsInString(Tempstr,"http://$(Server):$(Port)/playerconfig.aspx?key=$(ID)",Vars,0);
  	RetVal=DownloadPage(Tempstr,TYPE_CLIPSHACK_STAGE2, Title,Flags);
break;

case TYPE_CLIPSHACK_STAGE2:
		Tempstr=CopyStr(Tempstr,GetVar(Vars,"ID"));
  	RetVal=DownloadPage(Tempstr,TYPE_CLIPSHACK_STAGE3, Title,Flags);
break;

case TYPE_CLIPSHACK_STAGE3:
 Tempstr=CopyStr(Tempstr,GetVar(Vars,"ID"));
 	RetVal=DownloadItem(Tempstr,Title, Fmt, Flags);
break;

case TYPE_VIDEOEMO_STAGE2:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
 	RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;

case TYPE_BLIPTV:
 		Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
  	RetVal=DownloadPage(Tempstr,TYPE_BLIPTV_STAGE2, Title,Flags);
break;

case TYPE_BLIPTV_STAGE2:
 		Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
  	RetVal=DownloadPage(Tempstr,TYPE_BLIPTV_STAGE3, Title,Flags);
break;

case TYPE_ESCAPIST:
 		Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
  	RetVal=DownloadPage(Tempstr,TYPE_ESCAPIST_STAGE2, Title,Flags);
break;

case TYPE_REDBALCONY:
 		Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
  	RetVal=DownloadPage(Tempstr,TYPE_REDBALCONY_STAGE2, Title,Flags);
break;


case TYPE_REDORBIT:
 		Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
  	RetVal=DownloadPage(Tempstr,TYPE_REDORBIT_STAGE2, Title,Flags);
break;

case TYPE_ROYALSOCIETY:
 		Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
  	RetVal=DownloadM3U(Tempstr, Title, Flags);
break;

case TYPE_BRITISHACADEMY:
 		Tempstr=SubstituteVarsInString(Tempstr,"http://britac.studyserve.com/home/$(ID)",Vars,0);
  	RetVal=DownloadPage(Tempstr,TYPE_BRITISHACADEMY_STAGE2, Title,Flags);
break;

case TYPE_BRITISHACADEMY_STAGE2:
 		Tempstr=SubstituteVarsInString(Tempstr,"http://britac.studyserve.com/home/lectures/$(ID)",Vars,0);
  	RetVal=DownloadASX(Tempstr, Title,Flags);
break;

case TYPE_KAVLIINSTITUTE:
 		Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
  	RetVal=DownloadPage(Tempstr,TYPE_KAVLIINSTITUTE_STAGE2, Title,Flags);
break;

case TYPE_EURONEWS:
 Tempstr=SubstituteVarsInString(Tempstr,"http://video.euronews.net/$(ID).flv",Vars,0);
 RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;

case TYPE_DOTSUB:
 Tempstr=SubstituteVarsInString(Tempstr,"$(SERV)$(ID)",Vars,0);
 RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;

case TYPE_ASTRONOMYCOM:
 	Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);
  RetVal=DownloadPage(Tempstr,TYPE_ASTRONOMYCOM_STAGE2, Title,Flags);
break;


case TYPE_KAVLIINSTITUTE_STAGE2:
case TYPE_SCIVEE:
case TYPE_BLIPTV_STAGE3:
case TYPE_FIVE_MIN:
case TYPE_GENERIC:
case TYPE_MYTOPCLIP:
case TYPE_PRINCETON:
case TYPE_SDNHM:
case TYPE_UCSDTV:
case TYPE_IZLESE:
case TYPE_REUTERS:
case TYPE_LIVELEAK:
case TYPE_CLIPFISH_DE:
case TYPE_ACADEMIC_EARTH:
case TYPE_PHOTOBUCKET:
case TYPE_VIDEOSFACEBOOK:
case TYPE_ALJAZEERA:
case TYPE_MYVIDO1_STAGE2:
case TYPE_IVIEWTUBE:
case TYPE_WASHINGTONPOST:
case TYPE_CBSNEWS:
case TYPE_FRANCE24:
case TYPE_METATUBE:
case TYPE_MOTIONFEEDS:
case TYPE_GUARDIAN:
case TYPE_ESCAPIST_STAGE2:
case TYPE_REDORBIT_STAGE2:
case TYPE_REDBALCONY_STAGE2:
case TYPE_ASTRONOMYCOM_STAGE2:
 Tempstr=SubstituteVarsInString(Tempstr,"$(ID)",Vars,0);

 	RetVal=DownloadItem(Tempstr, Title, Fmt, Flags);
break;
}


DestroyString(Tempstr);
DestroyString(Title);
DestroyString(Fmt);

return(RetVal);
}


// This is the main function that 'screen scrapes' a webpage looking for 
// information that it can use to get a video
int ExtractItemInfo(STREAM *S, int Type, char *URL, char *Server, int Port, char *Title, int Flags)
{
char *Tempstr=NULL, *Token=NULL, *VarName=NULL;
ListNode *Vars=NULL;
char *ptr, *ptr2;
int MediaCount=0, i;
int RetVal=FALSE;

#define GENERIC_TITLE_START "<title>"
#define GENERIC_TITLE_END "</title>"



Vars=ListCreate();
SetVar(Vars,"Server",Server);
if (Port==0) Port=DefaultPort;
Tempstr=FormatStr(Tempstr,"%d",Port);
SetVar(Vars,"Port",Tempstr);
SetVar(Vars,"Title",Title);
//SetVar(Vars,"SelectedItem",ItemSelectionArg);

	if (Flags & (FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"\n------- DOWNLOADING DOCUMENT ------\n");
Tempstr=STREAMReadLine(Tempstr, S);
while (Tempstr)
{
	StripTrailingWhitespace(Tempstr);
	StripLeadingWhitespace(Tempstr);
	
	if (Flags & (FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"%s\n",Tempstr);
switch (Type)
{

case TYPE_YOUTUBE:
//#define YOUTUBE_PTR "new SWFObject(\"/player2.swf?"

//#define YOUTUBE_PTR "var swfArgs = {"
#define YOUTUBE_DIV "url_encoded_fmt_stream_map="
#define YOUTUBE_TITLE "&title="

	if (strstr(Tempstr,YOUTUBE_TITLE))
	{
		GenericExtractFromLine(Tempstr, "Title",YOUTUBE_TITLE, "&", Vars,EXTRACT_DEQUOTE);
	}


	if (strstr(Tempstr,YOUTUBE_DIV))
	{
		if (StrLen(GetVar(Vars,"item:flv"))==0) 
		{
			GenericExtractFromLine(Tempstr, "yt:url_fmt",YOUTUBE_DIV, "&", Vars,EXTRACT_DEQUOTE);
			Tempstr=CopyStr(Tempstr,GetVar(Vars,"yt:url_fmt"));

			DecodeYouTubeFormats(Tempstr,Vars);
		}
	}

break;

case TYPE_METACAFE:
#define METACAFE_ITEM "var itemID = "
#define METACAFE_ITEM2 "so.addParam(\"flashvars\", \'itemID="
//#define METACAFE_ITEM_FINAL "<item "
#define METACAFE_ITEM_FINAL ");"
#define METACAFE_EXTRA "var LEID = "
#define METACAFE_OVER_18 "allowAdultContent=1&submit=Continue+-+I%27m+over+18"
#define METACAFE_JS_REDIR "<script type=\"text/javascript\">document.location = "

	if (strncmp(Tempstr,METACAFE_JS_REDIR,StrLen(METACAFE_JS_REDIR))==0)
	{
		ptr=GetToken(Tempstr+StrLen(METACAFE_JS_REDIR),";</script>",&Token,0);
		StripQuotes(Token);
		SetVar(Vars,"item:flv",Token);
//		GenericExtractFromLine(Tempstr, "item:flv",METACAFE_JS_REDIR,";</script>", Vars);
		Type=TYPE_METACAFE_JS_REDIR;
	}
	else
	{
	if (strstr(Tempstr,GENERIC_TITLE_START))
	{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END, Vars,EXTRACT_DEQUOTE);
	}


#define METACAFE_MEDIA_URL "mediaURL"
	if (
				(strstr(Tempstr,METACAFE_MEDIA_URL))
		)
			
	{
		Token=ExtractMetacafeMediaURL(Token,Tempstr,METACAFE_MEDIA_URL,"&");
		Tempstr=HTTPUnQuote(Tempstr,Token);

		SetVar(Vars,"item:flv",Tempstr);
	}

#define METACAFE_MEDIA_URL2 "mediaURL%22%3A%22"
	if (
				(strstr(Tempstr,METACAFE_MEDIA_URL2))
		)
			
	{
		GenericExtractFromLine(Tempstr, "metacafe:mediaurl",METACAFE_MEDIA_URL2,"&", Vars, EXTRACT_DEQUOTE);

		Token=ExtractMetacafeMediaURL(Token,Tempstr,METACAFE_MEDIA_URL2,"&");
		Tempstr=HTTPUnQuote(Tempstr,Token);

		SetVar(Vars,"item:flv",Tempstr);
	}
break;

case TYPE_METACAFE_FINAL:
	if (strstr(Tempstr," url="))
	{
		GenericExtractFromLine(Tempstr, "item:flv"," url=","\\S", Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
	}
break;


case TYPE_BLIPTV:
#define BLIPTV_ITEM "player.setPrimaryMediaUrl(\""
#define BLIPTV_ITEM1 "<meta property=\"og:video\""
#define BLIPTV_ITEM2 "content=\""
	ptr=strstr(Tempstr,BLIPTV_ITEM1);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "ID",BLIPTV_ITEM2,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
break;

case TYPE_BLIPTV_STAGE2:
	if (!MediaCount)
	{
	Tempstr=CopyStr(Tempstr,STREAMGetValue(S,"HTTP:URL"));
	GenericExtractFromLine(Tempstr, "ID","file=","&",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	MediaCount++;
	}
break;

case TYPE_BLIPTV_STAGE3:
#define BLIPTV_S3_ITEM "<media:content url=\""

	ptr=strstr(Tempstr,BLIPTV_S3_ITEM);
	if (ptr)
	{
	GenericExtractFromLine(Tempstr, "item:flv",BLIPTV_S3_ITEM,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES | EXTRACT_GUESSTYPE);
	}
break;


case TYPE_MYVIDO1:
#define MYVIDO1_FRONTPAGE "dl.php"
#define MYVIDO1_MP4 "Download MP4 Format of Video</a>"
#define MYVIDO1_3GP "Download 3GP Format of Video</a>"
#define MYVIDO1_3GP2 "Download 3GPFormat of Video</a>"
#define MYVIDO1_FLV "Download Flv Format of Video</a>"
#define MYVIDO1_TITLE_START "meta property=\"og:title\" content=\""
#define MYVIDO1_TITLE_END "\""

ptr=strstr(Tempstr,MYVIDO1_FRONTPAGE);
if (ptr)
{
	GenericExtractFromLine(Tempstr, "ID","href=\"","\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}

//Fall through

case TYPE_MYVIDO1_STAGE2:
	ptr=strstr(Tempstr,MYVIDO1_MP4);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:mp4","href=\"","\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	Type=TYPE_MYVIDO1_STAGE2;
	}

	ptr=strstr(Tempstr,MYVIDO1_3GP);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:3gp","href=\"","\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		Type=TYPE_MYVIDO1_STAGE2;
	}

	ptr=strstr(Tempstr,MYVIDO1_3GP2);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:3gp","href=\"","\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		Type=TYPE_MYVIDO1_STAGE2;
	}


	ptr=strstr(Tempstr,MYVIDO1_TITLE_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",MYVIDO1_TITLE_START,MYVIDO1_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}

break;



case TYPE_BREAK_COM:
#define BREAK_ITEM "sGlobalFileName='"
#define BREAK_EXTRA "flashVars.icon = \""
#define BREAK_WMV "+sGlobalContentFilePath+'/'+sGlobalFileName+'.wmv"
#define BREAK_FLV "+sGlobalContentFilePath+'/'+sGlobalFileName+'.flv"
#define BREAK_HD "sGlobalFileNameHD='"
#define BREAK_HDD "sGlobalFileNameHDD='"
#define BREAK_TITLE "id=\"vid_title\" content=\""


	ptr=strstr(Tempstr,BREAK_ITEM);
	if (ptr)
	{
		//'sGlobalFileName' needs .flv appended to be the true path
		GenericExtractFromLine(Tempstr, "item:flv",BREAK_ITEM,"'",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,"item:flv");
		if (ptr) ptr=CatStr(ptr,".flv");
	}

	ptr=strstr(Tempstr,BREAK_HD);
	if (ptr) GenericExtractFromLine(Tempstr, "item:mp4:medq",BREAK_HD,"'",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);

	ptr=strstr(Tempstr,BREAK_HDD);
	if (ptr) GenericExtractFromLine(Tempstr, "item:mp4:highq",BREAK_HDD,"'",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);


	ptr=strstr(Tempstr,BREAK_EXTRA);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Extra",BREAK_EXTRA,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
	
	ptr=strstr(Tempstr,BREAK_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",BREAK_TITLE,"\"",Vars,0);
	}


break;



case TYPE_EHOW:
#define EHOW_ITEM "data-video-id=\""
#define EHOW_ITEM_END "\""
#define EHOW_TITLE "<h1 class=\"Heading1a\">"
#define EHOW_TITLE_END "</h1>"
#define EHOW_ITEM2 "showPlayer({"

	ptr=strstr(Tempstr,EHOW_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",EHOW_ITEM,EHOW_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,EHOW_ITEM2);
	if (ptr)
	{
		EhowExtractShowPlayer(S,ptr,Vars);
	}


	ptr=strstr(Tempstr,EHOW_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",EHOW_TITLE,EHOW_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}
break;

case TYPE_EHOW_STAGE2:
#define EHOW_STAGE2_ITEM "source="
#define EHOW_STAGE2_ITEM_END "&"

	ptr=strstr(Tempstr,EHOW_STAGE2_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",EHOW_STAGE2_ITEM,EHOW_STAGE2_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,EHOW_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",EHOW_TITLE,EHOW_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}
break;



case TYPE_VIMEO:
#define VIMEO_ITEM "targ_clip_id:"
#define VIMEO_TITLE "<meta name=\"title\" content=\""
#define VIMEO_TITLE_END "\""

	ptr=strstr(Tempstr,VIMEO_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "ID",VIMEO_ITEM,",",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,VIMEO_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",VIMEO_TITLE,VIMEO_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}	
break;


case TYPE_VIMEO_STAGE2:
#define VIMEO_STAGE2_ITEM "<nodeId>"
#define VIMEO_STAGE2_ITEM_END "</nodeId>"
#define VIMEO_STAGE2_REQ_SIG "<request_signature>"
#define VIMEO_STAGE2_REQ_SIG_END "</request_signature>"
#define VIMEO_STAGE2_REQ_SIG_EX "<request_signature_expires>"
#define VIMEO_STAGE2_REQ_SIG_EX_END "</request_signature_expires>"

	if (StrLen(GetVar(Vars,"item:flv"))==0)
	{
	ptr=strstr(Tempstr,VIMEO_STAGE2_ITEM);
	if (ptr) GenericExtractFromLine(Tempstr, "item:flv",VIMEO_STAGE2_ITEM,VIMEO_STAGE2_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,VIMEO_STAGE2_REQ_SIG);
	if (ptr)
	{
	if (ptr) GenericExtractFromLine(Tempstr, "Extra",VIMEO_STAGE2_REQ_SIG,VIMEO_STAGE2_REQ_SIG_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,VIMEO_STAGE2_REQ_SIG_EX);
	if (ptr)
	{
	if (ptr) GenericExtractFromLine(Tempstr, "Extra2",VIMEO_STAGE2_REQ_SIG_EX,VIMEO_STAGE2_REQ_SIG_EX_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}
break;


case TYPE_FIVE_MIN:
#define FIVE_MIN_ITEM "videoUrl="
#define FIVE_MIN_TITLE "<h1 class=\"videoTitle\">"
#define FIVE_MIN_TITLE_END "</h1>"

	ptr=strstr(Tempstr,FIVE_MIN_ITEM);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",FIVE_MIN_ITEM,"&",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,FIVE_MIN_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",FIVE_MIN_TITLE,FIVE_MIN_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}
break;


case TYPE_DAILYMOTION:
#define DAILYMOTION_ITEM "addVariable(\"sequence\","
#define DAILYMOTION_ITEM_END ")"
#define DAILYMOTION_PARAMS "\"videoPluginParameters\":{"
#define DAILYMOTION_PARAMS_END "}"
#define DAILYMOTION_TITLE_START "<h1 class=\"dmco_title\"><span class=\"title\" title=\""
#define DAILYMOTION_TITLE_END "\""


	if (strstr(Tempstr,DAILYMOTION_TITLE_START))
	{
		GenericExtractFromLine(Tempstr, "Title",DAILYMOTION_TITLE_START,DAILYMOTION_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}


	if (strstr(Tempstr,DAILYMOTION_ITEM))
	{
		GenericExtractFromLine(Tempstr, "DailyMotionItems",DAILYMOTION_ITEM,DAILYMOTION_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		Tempstr=CopyStr(Tempstr,GetVar(Vars,"DailyMotionItems"));
		GenericExtractFromLine(Tempstr, "DailyMotionItems",DAILYMOTION_PARAMS,DAILYMOTION_PARAMS_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,"DailyMotionItems");
		DecodeDailyMotionFormats(ptr,Vars);
	}
break;


case TYPE_VBOX7:
#define VBOX7_ITEM "so.addVariable(\"vid\", \""
#define VBOX7_ITEM_END "\");"

if (strstr(Tempstr,VBOX7_ITEM))
{
		GenericExtractFromLine(Tempstr, "item:flv",VBOX7_ITEM,VBOX7_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}

if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}
break;

case TYPE_TED:
#define TED_HIGH_ITEM_START "hs:\""
#define TED_MED_ITEM_START "ms:\""
#define TED_LOW_ITEM_START "ls:\""
#define TED_ITEM_END "\""
if (strstr(Tempstr,TED_HIGH_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv:highq",TED_HIGH_ITEM_START,TED_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}

if (strstr(Tempstr,TED_MED_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv:medq",TED_MED_ITEM_START,TED_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		GenericExtractFromLine(Tempstr, "item:flv",TED_MED_ITEM_START,TED_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}

if (strstr(Tempstr,TED_LOW_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv:lowq",TED_LOW_ITEM_START,TED_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}

if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}
break;

case TYPE_MYVIDEO:
#define MYVIDEO_URL_START "link rel='image_src' href='"
#define MYVIDEO_URL_END "/thumbs"
#define MYVIDEO_VIDID_END "_"

if (strstr(Tempstr,MYVIDEO_URL_START))
{
		ptr=GenericExtractFromLine(Tempstr, "MyVidURL",MYVIDEO_URL_START,MYVIDEO_URL_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GenericExtractFromLine(ptr, "item:flv","/",MYVIDEO_VIDID_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}
break;



case TYPE_YALE:
#define YALE_ITEM_START "openVideoWindowMEDIUM('"
#define YALE_ITEM_END "')"

if (strstr(Tempstr,YALE_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "ID",YALE_ITEM_START,YALE_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;

case TYPE_REDBALCONY:
#define REDBALCONY_ITEM_START "so.addVariable(\"file\", \""
#define REDBALCONY_ITEM_END "\""

if (strstr(Tempstr,REDBALCONY_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "ID",REDBALCONY_ITEM_START,REDBALCONY_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES | EXTRACT_GUESSTYPE);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;

case TYPE_REDBALCONY_STAGE2:
#define REDBALCONY_S2_ITEM_START "<link><![CDATA["
#define REDBALCONY_S2_ITEM_END "]"

if (strstr(Tempstr,REDBALCONY_S2_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv",REDBALCONY_S2_ITEM_START,REDBALCONY_S2_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES | EXTRACT_GUESSTYPE);
}

break;



case TYPE_MEFEEDIA:
#define MEFEEDIA_FIND  "<li class=\"videosource\">"
#define MEFEEDIA_ITEM_START "href=\""
#define MEFEEDIA_ITEM_END "\""

if (strstr(Tempstr,MEFEEDIA_FIND))
{
		GenericExtractFromLine(Tempstr, "item:reference",MEFEEDIA_ITEM_START,MEFEEDIA_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;

case TYPE_IVIEWTUBE:
//#define IVIEWTUBE_ITEM_START "('ltas.mediaid','"
//#define IVIEWTUBE_ITEM_END "'"

#define IVIEWTUBE_TITLE_START "name=\"flname\" value=\""
#define IVIEWTUBE_TITLE_END "\""

#define IVIEWTUBE_ITEM_START "http://www.iviewtube.com/player/player.swf?file="
#define IVIEWTUBE_ITEM_END "&"

if (strstr(Tempstr,IVIEWTUBE_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv",IVIEWTUBE_ITEM_START,IVIEWTUBE_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES | EXTRACT_GUESSTYPE);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;



case TYPE_MOBANGO:
#define MOBANGO_ITEM_START "so.addVariable('flvfile','"
#define MOBANGO_ITEM_END "'"

#define MOBANGO_TITLE_START "name=\"flname\" value=\""
#define MOBANGO_TITLE_END "\""

if (strstr(Tempstr,MOBANGO_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "item:flv",MOBANGO_ITEM_START,MOBANGO_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES | EXTRACT_GUESSTYPE);
}


if (strstr(Tempstr,MOBANGO_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",MOBANGO_TITLE_START,MOBANGO_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;


#define IZLESE_ITEM_START "file=http"
#define IZLESE_ITEM_END "&"
#define IZLESE_TITLE_START "<h1>"
#define IZLESE_TITLE_END "</h1>"
#define IZLESE_TITLE2_START "<title>"
#define IZLESE_TITLE2_END "</title>"

case TYPE_IZLESE:
if (strstr(Tempstr,IZLESE_ITEM_START) && strstr(Tempstr,IZLESE_ITEM_END))
{
		GenericExtractFromLine(Tempstr, "tmp",IZLESE_ITEM_START,IZLESE_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,"tmp");
		if (ptr)
		{
			//put 'http' back on
			Tempstr=MCopyStr(Tempstr,"http",ptr,NULL);
			
		}

	if (strstr(ptr,"youtube"))
	{
		SetVar(Vars,"item:reference",Tempstr);
	}
	else 
	{
		SetVar(Vars,"item:flv",Tempstr);
	}

}


if (strstr(Tempstr,IZLESE_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",IZLESE_TITLE_START,IZLESE_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

if (strstr(Tempstr,IZLESE_TITLE2_START))
{
		GenericExtractFromLine(Tempstr, "Title",IZLESE_TITLE2_START,IZLESE_TITLE2_END,Vars,EXTRACT_DEQUOTE);
}


break;


case TYPE_CLIPSHACK:
#define CLIPSHACK_ITEM_START "playerconfig.aspx?key="
#define CLIPSHACK_TITLE_START "<span id=\"lblTitle\">"
#define CLIPSHACK_TITLE_END "</span>"

if (strstr(Tempstr,CLIPSHACK_ITEM_START))
{
		GenericExtractFromLine(Tempstr, "ID",CLIPSHACK_ITEM_START,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}


if (strstr(Tempstr,CLIPSHACK_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",CLIPSHACK_TITLE_START,CLIPSHACK_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;

case TYPE_CLIPSHACK_STAGE2:
#define CLIPSHACK_STAGE2_START "<file>"
#define CLIPSHACK_STAGE2_END "</file>"
if (strstr(Tempstr,CLIPSHACK_STAGE2_START))
{
		GenericExtractFromLine(Tempstr, "ID",CLIPSHACK_STAGE2_START,CLIPSHACK_STAGE2_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}
break;

case TYPE_CLIPSHACK_STAGE3:
#define CLIPSHACK_STAGE3_START "<location>"
#define CLIPSHACK_STAGE3_END "</location>"
if (strstr(Tempstr,CLIPSHACK_STAGE3_START))
{
		GenericExtractFromLine(Tempstr, "item:flv",CLIPSHACK_STAGE3_START,CLIPSHACK_STAGE3_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES|EXTRACT_GUESSTYPE);
}
break;


#define REUTERS_FLV_ITEM_START "'flv':"
#define REUTERS_MP4_ITEM_START "'mpeg':"
#define REUTERS_TITLE "'headline':"

case TYPE_REUTERS:
	ptr=strstr(Tempstr,REUTERS_FLV_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",REUTERS_FLV_ITEM_START,",",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,REUTERS_MP4_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:mp4",REUTERS_MP4_ITEM_START,",",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
	}


	ptr=strstr(Tempstr,REUTERS_TITLE);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "Title",REUTERS_TITLE,",",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
	}
break;


#define AE_FLV_ITEM_START "flashVars.flvURL = "
#define AE_YT_ITEM_START "flashVars.ytID = "

case TYPE_ACADEMIC_EARTH:
	ptr=strstr(Tempstr,AE_FLV_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",AE_FLV_ITEM_START,";",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
	}

	ptr=strstr(Tempstr,AE_YT_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:reference",AE_YT_ITEM_START,";",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES);
		Tempstr=MCopyStr(Tempstr,"http://youtube.com/watch?v=",GetVar(Vars,"item:reference"),NULL);
		SetVar(Vars,"item:reference",Tempstr);
	}

break;


#define PHOTOBUCKET_START "flashvars=&quot;file="
#define PHOTOBUCKET_END "&quot;"

case TYPE_PHOTOBUCKET:
	ptr=strstr(Tempstr,PHOTOBUCKET_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:mp4",PHOTOBUCKET_START,PHOTOBUCKET_END,Vars, EXTRACT_DEQUOTE| EXTRACT_GUESSTYPE);
	}
break;


#define CLIPFISH_ITEM_START "&url="
case TYPE_CLIPFISH_DE:
	ptr=strstr(Tempstr,CLIPFISH_ITEM_START);
	if (ptr)
	{
		GenericExtractFromLine(Tempstr, "item:flv",CLIPFISH_ITEM_START,"&",Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES|EXTRACT_GUESSTYPE);
	}
break;

#define LIVELEAK_ITEM_START "file: \""
#define LIVELEAK_ITEM_END "'\""
case TYPE_LIVELEAK:
ptr=strstr(Tempstr,LIVELEAK_ITEM_START);
if (ptr)
{
	GenericExtractFromLine(Tempstr, "item:flv",LIVELEAK_ITEM_START,LIVELEAK_ITEM_END,Vars,EXTRACT_WITHIN_QUOTES | EXTRACT_NOSPACES | EXTRACT_GUESSTYPE);
}

if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}
break;



#define VIDEOEMO_ITEM_START "href=\"http://www.vidoevo.com/videodownload.php?e="
#define VIDEOEMO_ITEM_END "\""

case TYPE_VIDEOEMO:
	if (strstr(Tempstr,VIDEOEMO_ITEM_START))
	{
		GenericExtractFromLine(Tempstr, "item:flv","href=\"",VIDEOEMO_ITEM_END,Vars,EXTRACT_DEQUOTE);
	}

//Fall through, we might be on the second page already!
//break;

#define VIDEOEMO2_ITEM_START "Format of Video"
#define VIDEOEMO2_TITLE_START "<h2 class=\"title\">"
#define VIDEOEMO2_TITLE_END "</h2>"

case TYPE_VIDEOEMO_STAGE2:
	if (strstr(Tempstr,VIDEOEMO2_ITEM_START))
	{
		if (strstr(Tempstr,"flv")) GenericExtractFromLine(Tempstr, "item:flv","href=",">",Vars,EXTRACT_DEQUOTE);
		if (strstr(Tempstr,"mp4")) GenericExtractFromLine(Tempstr, "item:mp4","href=",">",Vars,EXTRACT_DEQUOTE);
		if (strstr(Tempstr,"3gp")) GenericExtractFromLine(Tempstr, "item:3gp","href=",">",Vars,EXTRACT_DEQUOTE);

		//if we found a download link, then we are at stage2 already!
		Type=TYPE_VIDEOEMO_STAGE2;
	}

if (strstr(Tempstr,VIDEOEMO2_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",VIDEOEMO2_TITLE_START,VIDEOEMO2_TITLE_END,Vars,EXTRACT_DEQUOTE);
}


break;

#define VIDEOSFACEBOOK_ITEM_START "<link rel=\"video_src\" href=\"http://www.videosfacebook.net/js/jw/player.swf?file="
#define VIDEOSFACEBOOK_ITEM_END "\""
#define VIDEOSFACEBOOK_TITLE_START "<meta property=\"og:title\" content=\""
#define VIDEOSFACEBOOK_TITLE_END "\""

case TYPE_VIDEOSFACEBOOK:
	if (strstr(Tempstr,VIDEOSFACEBOOK_ITEM_START))
	{
		GenericExtractFromLine(Tempstr, "item:flv",VIDEOSFACEBOOK_ITEM_START,VIDEOSFACEBOOK_ITEM_END,Vars,EXTRACT_DEQUOTE|EXTRACT_GUESSTYPE);
	}

	if (strstr(Tempstr,VIDEOSFACEBOOK_TITLE_START))
	{
		GenericExtractFromLine(Tempstr, "Title",VIDEOSFACEBOOK_TITLE_START,VIDEOSFACEBOOK_TITLE_END,Vars,EXTRACT_DEQUOTE);
	}


break;

case TYPE_WASHINGTONPOST:
#define WASHINGTONPOST_ITEM_LINE "mediaQueryString"
#define WASHINGTONPOST_ITEM_START "flvURL="
#define WASHINGTONPOST_ITEM_END "&"

#define WASHINGTONPOST_JSON_START "jsonURL=\""
#define WASHINGTONPOST_JSON_END "\""

#define WASHINGTONPOST_JSONP_START "name=\"jsonpURL\" value=\""
#define WASHINGTONPOST_JSONP_END "\""


if (strstr(Tempstr,WASHINGTONPOST_ITEM_LINE) && (StrLen(GetVar(Vars,"item:flv"))==0))
{
		GenericExtractFromLine(Tempstr, "item:flv",WASHINGTONPOST_ITEM_START,WASHINGTONPOST_ITEM_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		Tempstr=MCopyStr(Tempstr,"http://",Server,GetVar(Vars,"item:flv"),NULL);
		SetVar(Vars,"item:flv",Tempstr);
}

if (strstr(Tempstr,WASHINGTONPOST_JSON_START))
{
		GenericExtractFromLine(Tempstr, "ID",WASHINGTONPOST_JSON_START,WASHINGTONPOST_JSON_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		Type=TYPE_WASHINGTONPOST_JSON;
}

if (strstr(Tempstr,WASHINGTONPOST_JSONP_START))
{
		GenericExtractFromLine(Tempstr, "ID",WASHINGTONPOST_JSONP_START,WASHINGTONPOST_JSONP_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		Type=TYPE_WASHINGTONPOST_JSON;
		Tempstr=MCopyStr(Tempstr,"http://",Server,GetVar(Vars,"ID"),NULL);
		SetVar(Vars,"ID",Tempstr);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;

case TYPE_WASHINGTONPOST_STAGE2:
#define WASHINGTONPOST_SERVER_START "staticServer\":\""
#define WASHINGTONPOST_SERVER_END "\""
#define WASHINGTONPOST_ITEM2_START "flvURL\": \""
#define WASHINGTONPOST_ITEM2_END "\""

if (strstr(Tempstr,WASHINGTONPOST_SERVER_START))
{
		GenericExtractFromLine(Tempstr, "server",WASHINGTONPOST_SERVER_START,WASHINGTONPOST_SERVER_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}

if (strstr(Tempstr,WASHINGTONPOST_ITEM2_START))
{
		GenericExtractFromLine(Tempstr, "flvurl",WASHINGTONPOST_ITEM2_START,WASHINGTONPOST_ITEM2_END,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
}
		Tempstr=MCopyStr(Tempstr,"http://",Server,GetVar(Vars,"item:flv"),NULL);
		SetVar(Vars,"item:flv",Tempstr);

break;



case TYPE_CBSNEWS:
ExtractCBSNews(Vars, Tempstr);
if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;


case TYPE_FRANCE24:
#define FRANCE24_ITEM_START "meta property=\"og:video\" content=\""
#define FRANCE24_ITEM_END "\""
#define FRANCE24_ITEM2_START "file="
#define FRANCE24_ITEM2_END "&"

if (strstr(Tempstr,FRANCE24_ITEM_START) )
{
		GenericExtractFromLine(Tempstr, "item:flv",FRANCE24_ITEM_START,FRANCE24_ITEM_END,Vars,EXTRACT_GUESSTYPE);
}

if (strstr(Tempstr,FRANCE24_ITEM2_START) )
{
		GenericExtractFromLine(Tempstr, "item:flv",FRANCE24_ITEM2_START,FRANCE24_ITEM2_END,Vars,EXTRACT_GUESSTYPE);
}



if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,0);
}

break;


case TYPE_EURONEWS:
#define EURONEWS_ITEM_START "videofile:\""
#define EURONEWS_BULLETIN_START "vid0:\""
#define EURONEWS_ITEM_END "\""

if (strstr(Tempstr,EURONEWS_ITEM_START) )
{
		//don't try using 'guesstype' here, as we stick .flv on later, and need
		//to be able to find 'ID', not 'item:guessed'
		GenericExtractFromLine(Tempstr, "ID",EURONEWS_ITEM_START,EURONEWS_ITEM_END,Vars,0);
}

if (strstr(Tempstr,EURONEWS_BULLETIN_START) )
{
		//don't try using 'guesstype' here, as we stick .flv on later, and need
		//to be able to find 'ID', not 'item:guessed'
		GenericExtractFromLine(Tempstr, "ID",EURONEWS_BULLETIN_START,EURONEWS_ITEM_END,Vars,0);
}

if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,0);
}

break;



case TYPE_METATUBE:
#define METATUBE_ITEM_START "flashvars.videoPath = \""
#define METATUBE_ITEM_END "\";"

if (strstr(Tempstr,METATUBE_ITEM_START) )
{
		GenericExtractFromLine(Tempstr, "item:flv",METATUBE_ITEM_START,METATUBE_ITEM_END,Vars,EXTRACT_GUESSTYPE);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,0);
}

break;

case TYPE_MOTIONFEEDS:
#define MOTIONFEEDS_ITEM_LINE "param name=\"movie\" "
#define MOTIONFEEDS_ITEM_START1 "file="
#define MOTIONFEEDS_ITEM_START2 "value="
#define MOTIONFEEDS_ITEM_END "&"

if (strstr(Tempstr,MOTIONFEEDS_ITEM_LINE) )
{
if (strstr(Tempstr,MOTIONFEEDS_ITEM_START1) ) GenericExtractFromLine(Tempstr, "item:flv",MOTIONFEEDS_ITEM_START1,MOTIONFEEDS_ITEM_END,Vars,0);
else if (strstr(Tempstr,MOTIONFEEDS_ITEM_START2) ) GenericExtractFromLine(Tempstr, "item:reference",MOTIONFEEDS_ITEM_START2,MOTIONFEEDS_ITEM_END,Vars,0);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,0);
}

break;



case TYPE_ESCAPIST:
#define ESCAPIST_ITEM_START "value=\"config="
#define ESCAPIST_ITEM_END "%"
#define ESCAPIST_TITLE_START "<meta property=\"og:title\" content=\""
#define ESCAPIST_TITLE_END "\""
if (strstr(Tempstr,ESCAPIST_ITEM_START) ) GenericExtractFromLine(Tempstr, "ID",ESCAPIST_ITEM_START,ESCAPIST_ITEM_END,Vars,0);
if (strstr(Tempstr,ESCAPIST_TITLE_START) ) 
{
GenericExtractFromLine(Tempstr, "Title",ESCAPIST_TITLE_START,ESCAPIST_TITLE_END,Vars,0);
}
break;

case TYPE_ESCAPIST_STAGE2:
/*
#define ESCAPIST_STAGE2_ITEM_START "'},{'url':'"
#define ESCAPIST_STAGE2_ITEM_END "'"
if (strstr(Tempstr,ESCAPIST_STAGE2_ITEM_START) ) GenericExtractFromLine(Tempstr, "item:mp4",ESCAPIST_STAGE2_ITEM_START,ESCAPIST_STAGE2_ITEM_END,Vars,0);

*/

ParseFlashPlaylist(Tempstr, Vars);
break;


case TYPE_GUARDIAN:
#define GUARDIAN_ITEM_START "'src': '"
#define GUARDIAN_ITEM_END "'"

if (strstr(Tempstr,GUARDIAN_ITEM_START) )
{
		GenericExtractFromLine(Tempstr, "item:mp4",GUARDIAN_ITEM_START,GUARDIAN_ITEM_END,Vars,EXTRACT_GUESSTYPE);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,0);
}


break;

case TYPE_REDORBIT:
#define REDORBIT_ITEM_START "playlist="
#define REDORBIT_ITEM_END "'"

if (strstr(Tempstr,REDORBIT_ITEM_START) )
{
		GenericExtractFromLine(Tempstr, "ID",REDORBIT_ITEM_START,REDORBIT_ITEM_END,Vars,EXTRACT_DEQUOTE);
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,0);
}


break;

case TYPE_REDORBIT_STAGE2:
#define REDORBIT2_ITEM_START "<media:content url=\""
#define REDORBIT2_ITEM_END "\""

if (strstr(Tempstr,REDORBIT2_ITEM_START) )
{
		GenericExtractFromLine(Tempstr, "item:flv",REDORBIT2_ITEM_START,REDORBIT2_ITEM_END,Vars,EXTRACT_GUESSTYPE);
}
break;

case TYPE_SCIVEE:
#define SCIVEE_ITEM_START "<param name=\"flashvars\" value=\"id="
#define SCIVEE_ITEM_END "&"
if (strstr(Tempstr,SCIVEE_ITEM_START) )
{
		GenericExtractFromLine(Tempstr, "ID",SCIVEE_ITEM_START,SCIVEE_ITEM_END,Vars,0);
	 Tempstr=MCopyStr(Tempstr,"http://www.scivee.tv/assets/video/",GetVar(Vars,"ID"),NULL);
	SetVar(Vars,"item:flv",Tempstr);
	SetVar(Vars,"ID","");

}
else if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,0);
}
break;

case TYPE_ROYALSOCIETY:
#define ROYALSOCIETY_ITEM "ipadUrl: '"

		if (strstr(Tempstr,ROYALSOCIETY_ITEM)) 
		{
			GenericExtractFromLine(Tempstr, "ID",ROYALSOCIETY_ITEM,"\'",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		}
		if (strstr(Tempstr,"</h2>")) GenericExtractFromLine(Tempstr, "Title","","</h2>",Vars,0);
break;

case TYPE_BRITISHACADEMY:
#define BRITISHACADEMY_LINEDETECT "HearLecturePicture.gif"
#define BRITISHACADEMY_ITEMSTART "javascript:launch('"
#define BRITISHACADEMY_ITEMEND "'"

		if (strstr(Tempstr,BRITISHACADEMY_LINEDETECT)) GenericExtractFromLine(Tempstr, "ID",BRITISHACADEMY_ITEMSTART,BRITISHACADEMY_ITEMEND,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
break;

case TYPE_BRITISHACADEMY_STAGE2:
#define BRITISHACADEMY_S2_ITEMSTART "<param name=\"FileName\" value=\""
#define BRITISHACADEMY_S2_ITEMEND "\""

		if (strstr(Tempstr,BRITISHACADEMY_S2_ITEMSTART)) GenericExtractFromLine(Tempstr, "ID",BRITISHACADEMY_S2_ITEMSTART,BRITISHACADEMY_S2_ITEMEND,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
break;

case TYPE_KAVLIINSTITUTE:
 if (strstr(Tempstr,"rm/flashtv.html")) 
 {
		Tempstr=MCopyStr(Tempstr,URL,"/rm/flashtv.html",NULL);
		SetVar(Vars,"ID",Tempstr);
 }
 else if (strstr(Tempstr,"rm/flash.html")) 
 {
		Tempstr=MCopyStr(Tempstr,URL,"/rm/flash.html",NULL);
		SetVar(Vars,"ID",Tempstr);
 }
break;


case TYPE_KAVLIINSTITUTE_STAGE2:
#define KAVLI_ITEMSTART "file="
#define KAVLI_ITEMEND "&"

		if (strstr(Tempstr,KAVLI_ITEMSTART)) GenericExtractFromLine(Tempstr, "ID",KAVLI_ITEMSTART,KAVLI_ITEMEND,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
break;

case TYPE_DOTSUB:
#define DOTSUB_ITEMSTART "mediaUri = '"
#define DOTSUB_ITEMEND "'"
#define DOTSUB_SERVSTART "webAppUrl = '"
#define DOTSUB_TITLESTART "og:title\" content=\""
#define DOTSUB_TITLEEND "\""

		if (strstr(Tempstr,DOTSUB_ITEMSTART)) GenericExtractFromLine(Tempstr, "ID",DOTSUB_ITEMSTART,DOTSUB_ITEMEND,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		if (strstr(Tempstr,DOTSUB_SERVSTART)) GenericExtractFromLine(Tempstr, "SERV",DOTSUB_SERVSTART,DOTSUB_ITEMEND,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		if (strstr(Tempstr,DOTSUB_TITLESTART))
		{
		GenericExtractFromLine(Tempstr, "Title",DOTSUB_TITLESTART,DOTSUB_TITLEEND,Vars,0);
		}
break;

case TYPE_ASTRONOMYCOM:
#define ASTRONOMYCOM_ITEMSTART "http://kws.astronomy.com/generator.aspx?key="
#define ASTRONOMYCOM_ITEMEND "\""
#define ASTRONOMYCOM_TITLESTART "<span class=\"currentItem\">"
#define ASTRONOMYCOM_TITLEEND "</span>"

		if (strstr(Tempstr,ASTRONOMYCOM_TITLESTART))
		{
		GenericExtractFromLine(Tempstr, "Title",ASTRONOMYCOM_TITLESTART,ASTRONOMYCOM_TITLEEND,Vars,0);
		}

		if (strstr(Tempstr,ASTRONOMYCOM_ITEMSTART))
		{
			 GenericExtractFromLine(Tempstr, "ID",ASTRONOMYCOM_ITEMSTART,ASTRONOMYCOM_ITEMEND,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
			 Tempstr=MCopyStr(Tempstr,ASTRONOMYCOM_ITEMSTART,GetVar(Vars,"ID"),NULL);
			 SetVar(Vars,"ID",Tempstr);
		}
		

break;


case TYPE_ASTRONOMYCOM_STAGE2:
#define ASTRONOMYCOM_S2_ITEMSTART "so.addVariable(\"file\", \""
#define ASTRONOMYCOM_S2_ITEMEND "\""

		if (strstr(Tempstr,ASTRONOMYCOM_S2_ITEMSTART))
		{
			 GenericExtractFromLine(Tempstr, "ID",ASTRONOMYCOM_S2_ITEMSTART,ASTRONOMYCOM_ITEMEND,Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		}
	
break;

case TYPE_MYTOPCLIP:
case TYPE_SDNHM:
case TYPE_PRINCETON:
case TYPE_UCSDTV:
case TYPE_GENERIC:
case TYPE_ALJAZEERA:

//some site are actually just frontends to youtube
#define YOUTUBE_REFERENCE1 "http://www.youtube.com/watch?v="
#define YOUTUBE_REFERENCE2 "http://www.youtube.com/v/"
if (strstr(Tempstr,YOUTUBE_REFERENCE1))
{
		GenericExtractFromLine(Tempstr, "yt-tmp",YOUTUBE_REFERENCE1,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,"yt-tmp");
		if (StrLen(ptr)) Token=MCopyStr(Token,YOUTUBE_REFERENCE1,ptr,NULL);
		SetVar(Vars,"item:reference",Token);
		Token=CopyStr(Token,""); //So as later stages of this process don't
														 //pick up on it

}

if (strstr(Tempstr,YOUTUBE_REFERENCE2))
{
		GenericExtractFromLine(Tempstr, "yt-tmp",YOUTUBE_REFERENCE2,"\"",Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,"yt-tmp");
		if (StrLen(ptr)) Token=MCopyStr(Token,YOUTUBE_REFERENCE2,ptr,NULL);
		SetVar(Vars,"item:reference",Token);
		Token=CopyStr(Token,""); //So as later stages of this process don't
														 //pick up on it
}

for (i=0; i < ContainerTypes[i]; i++)
{
if (strstr(Tempstr,ContainerTypes[i]))
{
		GenericExtractFromLine(Tempstr, "tmp","http://",ContainerTypes[i],Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,"tmp");
		if (StrLen(ptr)) Token=MCopyStr(Token,"http://",ptr,ContainerTypes[i],NULL);
		SetVar(Vars,"item:reference",Token);
}
}


//This goes searching for urls starting with 'http://' and ending with likely media file extensions
for (i=0; FileTypes[i] !=NULL; i++)
{
	if (strstr(Tempstr,FileTypes[i]))
	{
		VarName=MCopyStr(VarName,"item:",FileTypes[i]+1,NULL);
		GenericExtractFromLine(Tempstr, VarName,"http://",FileTypes[i],Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
		ptr=GetVar(Vars,VarName);
		if (StrLen(ptr)) Token=MCopyStr(Token,"http://",ptr,NULL);
		else
		{
			GenericExtractFromLine(Tempstr, VarName,"href=",FileTypes[i],Vars,EXTRACT_DEQUOTE | EXTRACT_NOSPACES);
			Token=CopyStr(Token,GetVar(Vars,VarName));
			if (StrLen(Token) && (strncasecmp(Token,"http://",7) !=0))
			{
				Token=MCopyStr(Token,"http://",GetVar(Vars,"Server"),"/",NULL);
				Token=CatStr(Token,GetVar(Vars,VarName));
			}
		}
		
		if (StrLen(Token)) 
		{
			Token=CatStr(Token,FileTypes[i]);
			SetVar(Vars,VarName,Token);
		}
	}
}


if (strstr(Tempstr,GENERIC_TITLE_START))
{
		GenericExtractFromLine(Tempstr, "Title",GENERIC_TITLE_START,GENERIC_TITLE_END,Vars,EXTRACT_DEQUOTE);
}

break;


}

  Tempstr=STREAMReadLine(Tempstr,S);
}

	if (Flags & (FLAG_DEBUG2 | FLAG_DEBUG3)) fprintf(stderr,"\n------- END DOCUMENT ------\n\n");


if (StrLen(GetVar(Vars,"ID"))==0)
{
 Type=SelectDownloadFormat(Vars,Type);
}

ptr=GetVar(Vars,"ID");
if (ptr) RetVal=GetNextURL(Type, Server, Flags, Vars);
else if (! (Flags & FLAG_QUIET))
{
	fprintf(stderr,"FAILED TO GET DOCUMENT REFERENCE!\n");
	fprintf(stderr,"Maybe you need to change the format preferences, or else the url you supplied is wrong\n");
}

ListDestroy(Vars,DestroyString);

DestroyString(VarName);
DestroyString(Token);
DestroyString(Tempstr);

return(RetVal);
}
