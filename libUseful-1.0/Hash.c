#include "Hash.h"

char *EncodeBase64(char *Return, char *Text, int len)
{
char *RetStr;

RetStr=SetStrLen(Return,len *2);
to64frombits(RetStr,Text,len);

return(RetStr);
}

char *DecodeBase64(char *Return, int *len, char *Text)
{
char *RetStr;

RetStr=SetStrLen(Return,StrLen(Text) *2);
*len=from64tobits(RetStr,Text);

return(RetStr);
}

#include "crc32.h"

void HashUpdateCRC(THash *Hash, char *Data, int Len)
{
crc32Update((unsigned long *) &Hash->Ctx, Data, Len);
}


void HashFinishCRC(THash *Hash, int Encoding, char **HashStr)
{
int count;
char *Tempstr=NULL;
int i;

crc32Finish((unsigned long *) Hash->Ctx);

if (Encoding== ENCODE_BASE64)
{
	*HashStr=SetStrLen(*HashStr,128);
	to64frombits(*HashStr,(unsigned long *) Hash->Ctx,sizeof(unsigned long));
}
else
{
	for (i=0; i < sizeof(unsigned long); i++)
	{
	Tempstr=FormatStr(Tempstr,"%02x",Tempstr[i]);
	*HashStr=CatStr(*HashStr,Tempstr);
	}
}

DestroyString(Tempstr);
}


void HashInitCRC(THash *Hash)
{
Hash->Ctx=(void *) calloc(1,sizeof(unsigned long));
crc32Init((unsigned long *) Hash->Ctx);
Hash->Update=HashUpdateCRC;
Hash->Finish=HashFinishCRC;
}


#include "md5.h"
#define MD5LEN 16

void HashUpdateMD5(THash *Hash, char *Data, int Len)
{
MD5Update((MD5_CTX *) Hash->Ctx, Data, Len);
}


void HashFinishMD5(THash *Hash, int Encoding, char **HashStr)
{
int count;
char *Tempstr=NULL, *DigestBuff=NULL;
int i;

DigestBuff=(char *) calloc(1,MD5LEN+1);
MD5Final(DigestBuff, (MD5_CTX *) Hash->Ctx);

if (Encoding==ENCODE_BASE64)
{
	*HashStr=SetStrLen(*HashStr,42);
	to64frombits(*HashStr,DigestBuff,16);
}
else
{
	for (i=0; i < MD5LEN; i++)
	{
	Tempstr=FormatStr(Tempstr,"%02x",DigestBuff[i] & 255);
	*HashStr=CatStr(*HashStr,Tempstr);
	}
}

DestroyString(DigestBuff);
DestroyString(Tempstr);
}


void HashInitMD5(THash *Hash)
{
Hash->Ctx=(void *) calloc(1,sizeof(MD5_CTX));
MD5Init((MD5_CTX *) Hash->Ctx);
Hash->Update=HashUpdateMD5;
Hash->Finish=HashFinishMD5;
}


THash *HashInit(char *Type)
{
THash *Hash;

Hash=(THash *) calloc(1,sizeof(THash));
if (strcasecmp(Type,"md5")==0) HashInitMD5(Hash);
if (strcasecmp(Type,"crc32")==0) HashInitCRC(Hash);

return(Hash);
}

char *HashMD5(char *Return, char *text, int len, int Encoding)
{
THash *Hash;
char *RetStr=NULL;

RetStr=Return;
Hash=HashInit("md5");
HashUpdateMD5(Hash, text, len);
HashFinishMD5(Hash, Encoding, &RetStr);

return(RetStr);
}


