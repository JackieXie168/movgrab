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



char *EncodeHash(char *Buffer, char *Digest, int len, int Encoding)
{
char *Tempstr=NULL, *RetStr=NULL;
int i;

RetStr=SetStrLen(Buffer,128);
if (Encoding==ENCODE_BASE64) to64frombits(RetStr,Digest,len);
else
{
	for (i=0; i < len; i++)
	{
	Tempstr=FormatStr(Tempstr,"%02x",Digest[i] & 255);
	RetStr=CatStr(RetStr,Tempstr);
	}
}

DestroyString(Tempstr);
return(RetStr);
}




#include "crc32.h"

void HashUpdateCRC(THash *Hash, char *Data, int Len)
{
crc32Update((unsigned long *) &Hash->Ctx, Data, Len);
}


void HashFinishCRC(THash *Hash, int Encoding, char **HashStr)
{
unsigned long crc;

crc32Finish((unsigned long *) Hash->Ctx);
crc=htonl((unsigned long *) Hash->Ctx);

*HashStr=EncodeHash(*HashStr, (char *) &crc, sizeof(unsigned long), Encoding);
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
*HashStr=EncodeHash(*HashStr, DigestBuff, MD5LEN, Encoding);

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
else if (strcasecmp(Type,"crc32")==0) HashInitCRC(Hash);
else 
{
free(Hash);
Hash=NULL;
}

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


