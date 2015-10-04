#include "Hash.h"
#include "string.h"

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

int HMAC(char **Return, char *Algo, char *iKey, int iKeyLen, char *iText, int iTextLen)
{
int blocksize=64, len, i;
char *Key=NULL, *Text=NULL;
int KeyLen, TextLen;
char *Tempstr=NULL, *Digest=NULL, *OpadKey=NULL, *IpadKey=NULL;

len=StrLen(iKey);

if (len > blocksize) KeyLen=HashBytes(&Key,Algo,iKey,iKeyLen,"");
else 
{
KeyLen=len;
Key=SetStrLen(Key,len);
memcpy(Key,iKey,len);
}

Key=SetStrLen(Key,blocksize);
IpadKey=SetStrLen(IpadKey,blocksize);
OpadKey=SetStrLen(OpadKey,blocksize);

for (i=0; i < blocksize; i++)
{
IpadKey[i]=Key[i] ^ 0x5c;
OpadKey[i]=Key[i] ^ 0x36;
}

len=StrLen(Text);
Tempstr=SetStrLen(Tempstr,blocksize+len);
memcpy(Tempstr,IpadKey,blocksize);
memcpy(Tempstr+blocksize,Text,len);
len=HashBytes(&Digest,Algo,Tempstr,blocksize+len,"");

Tempstr=SetStrLen(Tempstr,blocksize+len);
memcpy(Tempstr,OpadKey,blocksize);
memcpy(Tempstr+blocksize,Digest,len);
len=HashBytes(&Return,Algo,Tempstr,blocksize+len,"");

DestroyString(Digest);
DestroyString(Tempstr);
DestroyString(IpadKey);
DestroyString(OpadKey);

return(len);
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


int HashFinishCRC(THash *Hash, int Encoding, char **HashStr)
{
unsigned long crc;
int len;

len=sizeof(unsigned long);
crc32Finish((unsigned long *) Hash->Ctx);
crc=htonl((unsigned long *) Hash->Ctx);

if (Encoding > 0) 
{
*HashStr=EncodeHash(*HashStr, (char *) &crc, len, Encoding);
return(StrLen(*HashStr));
}
else
{
*HashStr=SetStrLen(*HashStr,len);
memcpy(*HashStr,&crc,len);
return(len);
}
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


int HashFinishMD5(THash *Hash, int Encoding, char **HashStr)
{
int count, len;
char *Tempstr=NULL, *DigestBuff=NULL;

DigestBuff=(char *) calloc(1,MD5LEN+1);
MD5Final(DigestBuff, (MD5_CTX *) Hash->Ctx);

if (Encoding > 0)
{
*HashStr=EncodeHash(*HashStr, DigestBuff, MD5LEN, Encoding);
len=StrLen(*HashStr);
}
else
{
len=MD5LEN;
*HashStr=SetStrLen(*HashStr,len);
memcpy(*HashStr,DigestBuff,len);
}

DestroyString(DigestBuff);
DestroyString(Tempstr);

return(len);
}


void HashInitMD5(THash *Hash)
{
Hash->Ctx=(void *) calloc(1,sizeof(MD5_CTX));
MD5Init((MD5_CTX *) Hash->Ctx);
Hash->Update=HashUpdateMD5;
Hash->Finish=HashFinishMD5;
}

#include "sha1.h"
#define SHA1LEN 20

void HashUpdateSHA1(THash *Hash, char *Data, int Len)
{
sha1_process_bytes(Data,Len,(struct sha1_ctx *) Hash->Ctx);
}


int HashFinishSHA1(THash *Hash, int Encoding, char **HashStr)
{
int count, len;
char *Tempstr=NULL, *DigestBuff=NULL;

DigestBuff=(char *) calloc(1,SHA1LEN+1);
sha1_finish_ctx((struct sha1_ctx *) Hash->Ctx, DigestBuff);
if (Encoding > 0)
{
	 *HashStr=EncodeHash(*HashStr, DigestBuff, SHA1LEN, Encoding);
	 len=StrLen(*HashStr);
}
else
{
len=SHA1LEN;
*HashStr=SetStrLen(*HashStr,len);
memcpy(*HashStr,DigestBuff,len);
}

DestroyString(DigestBuff);
DestroyString(Tempstr);

return(len);
}


void HashInitSHA1(THash *Hash)
{
Hash->Ctx=(void *) calloc(1,sizeof(struct sha1_ctx));
sha1_init_ctx((struct sha1_ctx *) Hash->Ctx);
Hash->Update=HashUpdateSHA1;
Hash->Finish=HashFinishSHA1;
}

THash *HashInit(char *Type)
{
THash *Hash=NULL;

Hash=(THash *) calloc(1,sizeof(THash));
if (strcasecmp(Type,"sha1")==0) HashInitSHA1(Hash);
else if (strcasecmp(Type,"md5")==0) HashInitMD5(Hash);
else if (strcasecmp(Type,"crc32")==0) HashInitCRC(Hash);
else 
{
free(Hash);
Hash=NULL;
}

return(Hash);
}

int HashBytes(char **Return, char *Type, char *text, int len, int Encoding)
{
THash *Hash;

Hash=HashInit(Type);
Hash->Update(Hash, text, len);
return(Hash->Finish(Hash, Encoding, Return));
}

