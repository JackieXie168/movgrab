#include "DataProcessing.h"

int CompressBytes(char **Out, char *Alg, char *In, int Len, int Level)
{
TProcessingModule *Mod=NULL;
char *Tempstr=NULL;
int result;

Tempstr=FormatStr(Tempstr,"CompressionLevel=%d",Level);
Mod=StandardDataProcessorCreate("compression",Alg,Tempstr);
if (! Mod) return(-1);

*Out=SetStrLen(*Out,Len*2);
result=Mod->Flush(Mod,In,Len,*Out,Len*2);

DestroyString(Tempstr);
DataProcessorDestroy(Mod);

return(result);
}
