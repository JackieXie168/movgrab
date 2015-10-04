#include "libUseful.h"

main()
{
STREAM *In, *S;
int i, result;
char *Buffer[4096];

S=STREAMOpenFile("/tmp/lu.test",O_CREAT | O_TRUNC | O_WRONLY);
STREAMResizeBuffer(S,4096 * 3);
STREAMSetFlushType(S,FLUSH_BLOCK,4096);

In=STREAMOpenFile("/home/metacosm89/barbarabouchet.jpg",O_RDONLY);
result=STREAMReadBytes(In,Buffer,4096);
while (result > 0)
{
STREAMWriteBytes(S,Buffer,result);
result=STREAMReadBytes(In,Buffer,4096);
}

STREAMClose(S);
}
