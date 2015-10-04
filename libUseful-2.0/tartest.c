#include "libUseful.h"

main()
{
STREAM *Out;

Out=STREAMOpenFile("/tmp/test.tar",O_CREAT | O_TRUNC | O_WRONLY);
//STREAMAddStandardDataProcessor(Out, "compression", "gzip", "");
TarFiles(Out,"*");

STREAMClose(Out);
}


