#include "DataProcessing.h"


#ifdef HAVE_LIBSSL

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/evp.h>


typedef struct
{
char *Key;
int KeyLen;
char *InputVector;
int InputVectorLen;
int BlockSize;
const EVP_CIPHER *Cipher;
EVP_CIPHER_CTX *enc_ctx;
EVP_CIPHER_CTX *dec_ctx;
} libCryptoProcessorData;
#endif


#ifdef HAVE_CRYPTODEV

#include <sys/ioctl.h>
#include <linux/cryptodev.h>

typedef struct
{
char *Key;
int KeyLen;
char *InputVector;
int InputVectorLen;
struct session_op sess;
struct crypt_op cryp;
int fd;
char *EncryptBuff;
int EncryptBuffLen;
char *DecryptBuff;
int DecryptBuffLen;
} CryptoDevProcessorData;

#endif


void DestroyProcessingModule(TProcessingModule *Mod)
{
if (Mod)
{
   DestroyString(Mod->Name);
   DestroyString(Mod->Args);
   DestroyString(Mod->Buffer);
	 DestroyList(Mod->Values,DestroyString);
   free(Mod);
}
}



char *DataProcessorGetValue(TProcessingModule *M, const char *Name)
{
char *ptr;
ListNode *Curr;

if (! M->Values) return(NULL);
Curr=ListFindNamedItem(M->Values,Name);
if (Curr) return(Curr->Item);
return(NULL);
}


void DataProcessorSetValue(TProcessingModule *M, const char *Name, const char *Value)
{
char *ptr;
ListNode *Curr;

if (! M->Values) M->Values=CreateEmptyList();
Curr=ListFindNamedItem(M->Values,Name);
if (Curr) Curr->Item = (void *) CopyStr( (char *) Curr->Item, Value);
else AddNamedItemToList(M->Values,Name,CopyStr(NULL,Value));
}


void InitialiseEncryptionComponents(const char *Args, char **Cipher, char **InputVector, int *IVLen,  char **Key, int *KeyLen, int *Flags)
{
char *TmpKey=NULL, *Tempstr=NULL;
int klen=0, slen=0;
char *Name=NULL, *Value=NULL, *ptr;
char *Salt=NULL;

*IVLen=0;
fprintf(stderr,"IEC1: [%s] \n",Args);
ptr=GetNameValuePair(Args,"\\S","=",&Name,&Value);
while (ptr)
{
fprintf(stderr,"IEC: [%s] [%s]\n",Name,Value);
  if (StrLen(Name))
  {
  if (strcasecmp(Name,"Cipher")==0)
  {
	  *Cipher=CopyStr(*Cipher,Value);
  }


  if (strcasecmp(Name,"Key")==0)
  {
	  TmpKey=CopyStr(TmpKey,Value);
	  klen=StrLen(TmpKey);
  }

  if (strcasecmp(Name,"Salt")==0)
  {
	  Salt=CopyStr(Salt,Value);
	  slen=StrLen(Salt);
  }



  if (
	(strcasecmp(Name,"iv")==0) ||
  	(strcasecmp(Name,"InputVector")==0)
    )
  {
    *InputVector=CopyStr(*InputVector,Value);
    *IVLen=StrLen(*InputVector);
  }

  if (strcasecmp(Name,"HexKey")==0)
  {
    klen=HexStrToBytes(&TmpKey, Value);
  }

  if (
	(strcasecmp(Name,"HexIV")==0) ||
  	(strcasecmp(Name,"HexInputVector")==0)
    )

  {
    *IVLen=HexStrToBytes(InputVector,Value);
  }

  if (strcasecmp(Name,"PadBlock")==0) 
  {
    if (strcasecmp(Value,"N")==0) *Flags |= DPM_NOPAD_DATA;
  }

  }

ptr=GetNameValuePair(ptr,"\\S","=",&Name,&Value);
}


Tempstr=SetStrLen(Tempstr,klen+slen);
memcpy(Tempstr,Salt,slen);
memcpy(Tempstr+slen,TmpKey,klen);

*Key=HashMD5(*Key,Tempstr,slen+klen,0);
*KeyLen=StrLen(*Key);


DestroyString(Name);
DestroyString(Value);
DestroyString(Tempstr);
DestroyString(TmpKey);
DestroyString(Salt);
}


int CryptoDevProcessorInit(TProcessingModule *ProcMod, const char *Args)
{
int result=FALSE;
#ifdef HAVE_CRYPTODEV
CryptoDevProcessorData *Data;
char *Cipher=NULL;
int val;

val=open("/dev/crypto",O_RDWR);

if (! val) return(FALSE);
Data=(CryptoDevProcessorData *) calloc(1,sizeof(CryptoDevProcessorData));
Data->fd=val;

InitialiseEncryptionComponents(Args, &Cipher, & Data->Key, &Data->KeyLen, &Data->InputVector, &Data->InputVectorLen,&ProcMod->Flags);

if (StrLen(ProcMod->Name)==0) ProcMod->Name=CopyStr(ProcMod->Name,Cipher);

Data->sess.cipher=CRYPTO_CIPHER_NAME | CRYPTO_FLAG_CBC;
Data->sess.alg_name=ProcMod->Name;
Data->sess.alg_namelen=StrLen(ProcMod->Name);
Data->sess.key=Data->Key;
Data->sess.keylen=Data->KeyLen;
ProcMod->Data=Data;

if (ioctl(Data->fd,CIOCGSESSION, &Data->sess) != -1)
{
  result=TRUE;
}


DestroyString(Cipher);
#endif
return(result);
}


int CryptoDevProcessorWrite(TProcessingModule *ProcMod, const char *InData, int InLen, char *OutData, int OutLen)
{
int wrote=0;

#ifdef HAVE_CRYPTODEV
char *ptr;
CryptoDevProcessorData *Data;
int NoOfBlocks=0, OutBlocks=0, val;


Data=(CryptoDevProcessorData *) ProcMod->Data;

Data->cryp.len=0;

if (InLen==0)
{
//Pad up to a block boundary
while ((Data->EncryptBuffLen % Data->sess.blocksize) !=0)
{
Data->EncryptBuff=SetStrLen(Data->EncryptBuff,Data->EncryptBuffLen+1);
*(Data->EncryptBuff+Data->EncryptBuffLen)='\0';
Data->EncryptBuffLen++;
}
}
else
{
Data->EncryptBuff=SetStrLen(Data->EncryptBuff,Data->EncryptBuffLen+InLen);
memcpy(Data->EncryptBuff+Data->EncryptBuffLen,InData,InLen);
Data->EncryptBuffLen+=InLen;
}

NoOfBlocks=Data->EncryptBuffLen / Data->sess.blocksize;
OutBlocks=OutLen / Data->sess.blocksize;
if (OutBlocks < NoOfBlocks) NoOfBlocks=OutBlocks;

if (NoOfBlocks > 0)
{
val=NoOfBlocks * Data->sess.blocksize;
Data->cryp.ses=Data->sess.ses;
Data->cryp.src=Data->EncryptBuff;
Data->cryp.len=val;
Data->cryp.dst=OutData;
Data->cryp.iv=Data->InputVector;
Data->cryp.op=COP_ENCRYPT;


if (ioctl(Data->fd,CIOCCRYPT,&Data->cryp))
{
return(FALSE);
}

Data->EncryptBuffLen-=val;
if (Data->EncryptBuffLen > 0) memmove(Data->EncryptBuff,Data->EncryptBuff+val, Data->EncryptBuffLen);
}

wrote=Data->cryp.len;

#endif
return(wrote);
}


int CryptoDevProcessorRead(TProcessingModule *ProcMod, const char *InData, int InLen, char *OutData, int OutLen)
{
int wrote=0;
#ifdef HAVE_CRYPTODEV
CryptoDevProcessorData *Data;
int NoOfBlocks=0, OutBlocks=0, val;


Data=(CryptoDevProcessorData *) ProcMod->Data;

Data->cryp.len=0;
Data->DecryptBuff=SetStrLen(Data->DecryptBuff,Data->DecryptBuffLen+InLen);
memcpy(Data->DecryptBuff+Data->DecryptBuffLen,InData,InLen);
Data->DecryptBuffLen+=InLen;

NoOfBlocks=Data->DecryptBuffLen / Data->sess.blocksize;
OutBlocks=OutLen / Data->sess.blocksize;
if (OutBlocks < NoOfBlocks) NoOfBlocks=OutBlocks;

if (NoOfBlocks > 0)
{
val=NoOfBlocks * Data->sess.blocksize;
Data->cryp.ses=Data->sess.ses;
Data->cryp.len=val;
Data->cryp.src=Data->DecryptBuff;
Data->cryp.dst=OutData;
Data->cryp.iv=Data->InputVector;
Data->cryp.op=COP_DECRYPT;

if (ioctl(Data->fd,CIOCCRYPT,&Data->cryp))return(FALSE);
Data->DecryptBuffLen-=val;
memmove(Data->DecryptBuff,Data->DecryptBuff+val, Data->DecryptBuffLen);
}

wrote=Data->cryp.len;

#endif
return(wrote);
}



int CryptoDevProcessorClose(TProcessingModule *ProcMod)
{
#ifdef HAVE_CRYPTODEV
	if (ProcMod->Data)
	{
		free(ProcMod->Data);
		ProcMod->Data=NULL;
	}
#endif
return(TRUE);
}



#ifdef HAVE_LIBCRYPTO

int libCryptoProcessorInit(TProcessingModule *ProcMod, const char *Args)
{
int result=FALSE;

#ifdef HAVE_LIBSSL
libCryptoProcessorData *Data;
EVP_CIPHER_CTX *ctx;
char *CipherList[]={"blowfish","rc2","rc4","rc5","des","desx","cast","idea","aes","aes-256",NULL};
typedef enum {CI_BLOWFISH, CI_RC2, CI_RC4, CI_RC5, CI_DES, CI_DESX, CI_CAST,CI_IDEA,CI_AES, CI_AES_256} LIBUSEFUL_CRYPT_CIPHERS;
int val;
char *Tempstr=NULL;

Data=(libCryptoProcessorData *) calloc(1,sizeof(libCryptoProcessorData));

//Tempstr here holds the cipher name
InitialiseEncryptionComponents(Args, &Tempstr, &Data->InputVector, &Data->InputVectorLen, & Data->Key, &Data->KeyLen,&ProcMod->Flags);

if (StrLen(ProcMod->Name)==0) ProcMod->Name=CopyStr(ProcMod->Name,Tempstr);

val=MatchTokenFromList(ProcMod->Name,CipherList,0);

switch(val)
{
/*
	case CI_NONE:
	Data->Cipher=EVP_enc_null();
	break;
*/

	case CI_BLOWFISH:
		Data->Cipher=EVP_bf_cbc();
		break;

	case CI_RC2:
		Data->Cipher=EVP_rc2_cbc();
		break;

	case CI_RC4:
		Data->Cipher=EVP_rc4();
		break;

	case CI_RC5:
		//Data->Cipher=EVP_rc5_32_12_16_cbc();
		break;

	case CI_DES:
		Data->Cipher=EVP_des_cbc();
		break;

	case CI_DESX:
		Data->Cipher=EVP_desx_cbc();
		break;

	case CI_CAST:
		Data->Cipher=EVP_cast5_cbc();
		break;

	case CI_IDEA:
		Data->Cipher=EVP_idea_cbc();
		break;

	case CI_AES:
		Data->Cipher=EVP_aes_128_cbc();
		break;

	case CI_AES_256:
		Data->Cipher=EVP_aes_256_cbc();
		break;
}


if (Data->Cipher)
{
Data->enc_ctx=(EVP_CIPHER_CTX *) calloc(1,sizeof(EVP_CIPHER_CTX));
Data->dec_ctx=(EVP_CIPHER_CTX *) calloc(1,sizeof(EVP_CIPHER_CTX));
EVP_CIPHER_CTX_init(Data->enc_ctx);
EVP_CIPHER_CTX_init(Data->dec_ctx);
Data->BlockSize=EVP_CIPHER_block_size(Data->Cipher);

EVP_EncryptInit_ex(Data->enc_ctx,Data->Cipher,NULL,Data->Key,Data->InputVector);
EVP_DecryptInit_ex(Data->dec_ctx,Data->Cipher,NULL,Data->Key,Data->InputVector);

if (ProcMod->Flags & DPM_NOPAD_DATA) EVP_CIPHER_CTX_set_padding(Data->enc_ctx,FALSE);

ProcMod->Data=Data;
result=TRUE;

DataProcessorSetValue(ProcMod,"Cipher",Tempstr);
Tempstr=FormatStr(Tempstr,"%d",Data->BlockSize);
DataProcessorSetValue(ProcMod,"BlockSize",Tempstr);
}

DestroyString(Tempstr);
#endif
return(result);
}


int libCryptoProcessorClose(TProcessingModule *ProcMod)
{
#ifdef HAVE_LIBSSL
libCryptoProcessorData *Data;
EVP_CIPHER_CTX *ctx;

Data=(libCryptoProcessorData *) ProcMod->Data;
if (Data)
{
EVP_CIPHER_CTX_cleanup(Data->enc_ctx);
EVP_CIPHER_CTX_cleanup(Data->dec_ctx);

DestroyString(Data->Key);
DestroyString(Data->InputVector);
free(Data);
}
ProcMod->Data=NULL;
#endif
return(TRUE);
}







int libCryptoProcessorWrite(TProcessingModule *ProcMod, const char *InData, int InLen, char *OutData, int OutLen)
{
int wrote=0;

#ifdef HAVE_LIBSSL
int len, result, val;
libCryptoProcessorData *Data;
EVP_CIPHER_CTX *ctx;
char *ptr, *Tempstr=NULL;

//fprintf(stderr,"LCPW: %s %d\n",InData,InLen);
//if (ProcMod->Flags & DPM_WRITE_FINAL) return(0);
ptr=OutData;

Data=(libCryptoProcessorData *) ProcMod->Data;
ctx=Data->enc_ctx;

len=OutLen;

ProcMod->Flags = ProcMod->Flags & ~DPM_WRITE_FINAL;
if (ProcMod->Flags & DPM_NOPAD_DATA)
{
printf("PAD!\n");
	val=InLen % Data->BlockSize;
	Tempstr=CopyStrLen(Tempstr,InData,InLen);
	if (val !=0) 
	{
		Tempstr=SetStrLen(Tempstr,InLen + (Data->BlockSize-val));
		memset(Tempstr+InLen,' ', (Data->BlockSize-val));
		val=InLen+(Data->BlockSize-val);
	}
	else val=InLen;

	result=EVP_EncryptUpdate(ctx, ptr, &len, Tempstr, val);
fprintf(stderr,"WRITE: %s %d -> %d\n",Tempstr,val,len);
}
else 
{
result=EVP_EncryptUpdate(ctx, ptr, &len, InData, InLen);
fprintf(stderr,"WRITE: %s %d -> %d (%d)\n",InData,InLen,len,OutLen);
}


if (! result) wrote=0;
else wrote=len;

DestroyString(Tempstr);
#endif
return(wrote);
}



int libCryptoProcessorFlush(TProcessingModule *ProcMod, const char *InData, int InLen, char *OutData, int OutLen)
{
int result=0, wrote=0, len;
libCryptoProcessorData *Data;

if (ProcMod->Flags & DPM_WRITE_FINAL) return(0);
Data=(libCryptoProcessorData *) ProcMod->Data;

if (Data)
{
if (InLen > 0)
{
result=libCryptoProcessorWrite(ProcMod, InData, InLen, OutData, OutLen);
if (result > 0) return(result);
}

len=OutLen;
result=EVP_EncryptFinal_ex(Data->enc_ctx, OutData, &len);
ProcMod->Flags |= DPM_WRITE_FINAL;
}
if (! result) wrote=0;
else wrote=len;

return(wrote);
}


int libCryptoProcessorRead(TProcessingModule *ProcMod, const char *InData, int InLen, char *OutData, int OutLen)
{
int bytes_read=0;
#ifdef HAVE_LIBSSL
int len, ivlen, result, val;
libCryptoProcessorData *Data;
EVP_CIPHER_CTX *ctx;
char *ptr;

ptr=OutData;

Data=(libCryptoProcessorData *) ProcMod->Data;
if (!Data) return(0);

/*
if (ProcMod->Flags & DPM_READ_FINAL)
{
  if (InLen==0)	return(0);
  EVP_DecryptInit_ex(Data->dec_ctx,Data->Cipher,NULL,Data->Key,Data->InputVector);

}
*/

ctx=Data->dec_ctx;

if (InLen==0) 
{
  len=0;
  result=EVP_DecryptFinal_ex(ctx, ptr, &len);
  ProcMod->Flags |= DPM_READ_FINAL; //this so we don't try 
				    //another read
	
}
else 
{
	len=OutLen;
	result=EVP_DecryptUpdate(ctx, ptr, &len, InData, InLen);
}

if (! result) bytes_read=-1;
else bytes_read+=InLen; //should be 'len' but DecryptUpdate returns the
		   //wrong value.

#endif
return(bytes_read);
}

#endif



#ifdef HAVE_LIBZ

#include <zlib.h>

typedef struct
{
z_stream z_in;
z_stream z_out;
} zlibData;
#endif


int zlibProcessorInit(TProcessingModule *ProcMod, const char *Args)
{
int result=FALSE;

#ifdef HAVE_LIBZ
zlibData *ZData;
int CompressionLevel=5;
char *ptr, *Name=NULL, *Value=NULL;

ptr=GetNameValuePair(Args,"\\S","=",&Name,&Value);
while (ptr)
{
  if (strcasecmp(Name,"CompressionLevel")==0) CompressionLevel=atoi(Value);
  if (strcasecmp(Name,"Level")==0) CompressionLevel=atoi(Value);
ptr=GetNameValuePair(ptr,"\\S","=",&Name,&Value);
}



ZData=(zlibData *) calloc(1,sizeof(zlibData));
ZData->z_in.avail_in=0;
ZData->z_in.avail_out=0;
result=inflateInit(&ZData->z_in);

ZData->z_out.avail_in=0;
ZData->z_out.avail_out=0;
deflateInit(&ZData->z_out,5);


ProcMod->Data=(void *) ZData;
result=TRUE;

DestroyString(Name);
DestroyString(Value);

#endif
return(result);
}

int gzipProcessorInit(TProcessingModule *ProcMod, const char *Args)
{
int result=FALSE;

#ifdef HAVE_LIBZ
zlibData *ZData;
int CompressionLevel=5;
char *ptr, *Name=NULL, *Value=NULL;

ptr=GetNameValuePair(Args,"\\S","=",&Name,&Value);
while (ptr)
{
  if (strcasecmp(Name,"CompressionLevel")==0) CompressionLevel=atoi(Value);
  if (strcasecmp(Name,"Level")==0) CompressionLevel=atoi(Value);
ptr=GetNameValuePair(ptr,"\\S","=",&Name,&Value);
}



ZData=(zlibData *) calloc(1,sizeof(zlibData));
ZData->z_in.avail_in=0;
ZData->z_in.avail_out=0;
result=inflateInit2(&ZData->z_in,47);

ZData->z_out.avail_in=0;
ZData->z_out.avail_out=0;
deflateInit2(&ZData->z_out,5,Z_DEFLATED,30,8,Z_DEFAULT_STRATEGY);

ProcMod->Data=(void *) ZData;
result=TRUE;

DestroyString(Name);
DestroyString(Value);

#endif
return(result);
}

int zlibProcessorInternalWrite(TProcessingModule *ProcMod, const char *InData, int InLen, char *OutData, int OutLen, int Flush)
{
int wrote=0;
#ifdef HAVE_LIBZ

zlibData *ZData;

if (ProcMod->Flags & DPM_WRITE_FINAL) return(0);
ZData=(zlibData *) ProcMod->Data;

ZData->z_out.avail_in=InLen;
ZData->z_out.next_in=(char *) InData;

ZData->z_out.avail_out=OutLen;
ZData->z_out.next_out=OutData;

if (Flush)
{
	deflate(& ZData->z_out, Z_FINISH);
	ProcMod->Flags |= DPM_WRITE_FINAL;
}
else deflate(& ZData->z_out, Z_NO_FLUSH);

wrote=OutLen-ZData->z_out.avail_out;
#endif
return(wrote);
}



int zlibProcessorWrite(TProcessingModule *ProcMod, const char *InData, int InLen, char *OutData, int OutLen)
{
return(zlibProcessorInternalWrite(ProcMod, InData, InLen, OutData, OutLen, FALSE));
}

int zlibProcessorFlush(TProcessingModule *ProcMod, const char *InData, int InLen, char *OutData, int OutLen)
{
return(zlibProcessorInternalWrite(ProcMod, InData, InLen, OutData, OutLen, TRUE));
}




int zlibProcessorRead(TProcessingModule *ProcMod, const char *InData, int InLen, char *OutData, int OutLen)
{
int wrote=0, result;
#ifdef HAVE_LIBZ
zlibData *ZData;
int len;

ZData=(zlibData *) ProcMod->Data;

ProcMod->Buffer=SetStrLen(ProcMod->Buffer,ProcMod->BuffSize+InLen);
memcpy(ProcMod->Buffer+ProcMod->BuffSize,InData,InLen);
ZData->z_in.next_in=ProcMod->Buffer;
ProcMod->BuffSize+=InLen;
ZData->z_in.avail_in=ProcMod->BuffSize;

ZData->z_in.avail_out=OutLen;
ZData->z_in.next_out=OutData;

if (InLen==0) result=inflate(& ZData->z_in, Z_FINISH);
else result=inflate(& ZData->z_in, Z_NO_FLUSH);
if (result==Z_DATA_ERROR) inflateSync(&ZData->z_in);

if (ZData->z_in.avail_in > 0) 
{
memmove(ProcMod->Buffer,ZData->z_in.next_in,ZData->z_in.avail_in);
ProcMod->BuffSize=ZData->z_in.avail_in;
}
else ProcMod->BuffSize=0;

wrote=OutLen-ZData->z_in.avail_out;

#endif
return(wrote);
}



int zlibProcessorClose(TProcessingModule *ProcMod)
{
#ifdef HAVE_LIBZ
zlibData *ZData;

ZData=(zlibData *) ProcMod->Data;
if (ZData)
{
inflateEnd(&ZData->z_in);
deflateEnd(&ZData->z_out);

free(ZData);
ProcMod->Data=NULL;
}
#endif
return(TRUE);
}



TProcessingModule *StandardDataProcessorCreate(const char *Class, const char *Name, const char *Args)
{
TProcessingModule *Mod=NULL;

#ifdef HAVE_LIBSSL
#ifdef HAVE_LIBCRYPTO
if (strcasecmp(Class,"crypto")==0)
{
   Mod=(TProcessingModule *) calloc(1,sizeof(TProcessingModule));
   Mod->Args=CopyStr(Mod->Args,Args);
   Mod->Name=CopyStr(Mod->Name,Name);
   Mod->Init=libCryptoProcessorInit;
   Mod->Write=libCryptoProcessorWrite;
   Mod->Flush=libCryptoProcessorFlush;
   Mod->Read=libCryptoProcessorRead;
   Mod->Close=libCryptoProcessorClose;

   if (Mod && Mod->Init(Mod, Args)) return(Mod);
   else 
	{
		DestroyProcessingModule(Mod);
		Mod=NULL;
	}
}
#endif
#endif

#ifdef HAVE_CRYPTODEV 
if (strcasecmp(Class,"cryptodev")==0)
{
   Mod=(TProcessingModule *) calloc(1,sizeof(TProcessingModule));
   Mod->Args=CopyStr(Mod->Args,Args);
   Mod->Name=CopyStr(Mod->Name,Name);
   Mod->Init=CryptoDevProcessorInit;
   Mod->Write=CryptoDevProcessorWrite;
   Mod->Read=CryptoDevProcessorRead;
   Mod->Close=CryptoDevProcessorClose;

   if (Mod && Mod->Init(Mod, Args)) return(Mod);
   else 
	{
		DestroyProcessingModule(Mod);
		Mod=NULL;
	}
}
#endif





#ifdef HAVE_LIBZ 
if (strcasecmp(Class,"compression")==0)
{
   Mod=(TProcessingModule *) calloc(1,sizeof(TProcessingModule));
   Mod->Args=CopyStr(Mod->Args,Args);
   Mod->Name=CopyStr(Mod->Name,Name);

   if (strcasecmp(Name,"zlib")==0) Mod->Init=zlibProcessorInit;
   else Mod->Init=gzipProcessorInit;
   
//fprintf(stderr,"SDP: %s %s\n",Class,Name);
   Mod->Write=zlibProcessorWrite;
   Mod->Read=zlibProcessorRead;
   Mod->Flush=zlibProcessorFlush;
   Mod->Close=zlibProcessorClose;
 
   if (Mod && Mod->Init(Mod, Args)) return(Mod);
   else 
	{
		DestroyProcessingModule(Mod);
		Mod=NULL;
	}
}
#endif


return(NULL);
}


void DataProcessorDestroy(void *In)
{
TProcessingModule *Mod;

Mod=(TProcessingModule *) In;
if (! Mod) return;
Mod->Close(Mod);
DestroyString(Mod->Name);
DestroyString(Mod->Args);
DestroyString(Mod->Buffer);
free(Mod);
}




int STREAMAddDataProcessor(STREAM *S, TProcessingModule *Mod, const char *Args)
{
ListNode *Curr;
char *Tempstr=NULL;
int len;

STREAMFlush(S);

len=S->InEnd - S->InStart;
Tempstr=SetStrLen(Tempstr,len);
memcpy(Tempstr,S->InputBuff + S->InStart,len);
STREAMResetInputBuffers(S);
if (! S->ProcessingModules) S->ProcessingModules=CreateEmptyList();

AddItemToList(S->ProcessingModules,Mod);
Curr=GetNextListItem(Mod->Values);
while (Curr)
{
	STREAMSetValue(S,Curr->Tag,(char *) Curr->Item);
	Curr=GetNextListItem(Curr);
}

STREAMReadThroughProcessors(S, Tempstr, len);

DestroyString(Tempstr);
return(TRUE);
}


int DataProcessorAvailable(const char *Class, const char *Name)
{
int result=FALSE;
TProcessingModule *Mod;

Mod=StandardDataProcessorCreate(Class,Name,"");
if (Mod) result=TRUE;

DataProcessorDestroy(Mod);
return(result);
}


int STREAMAddStandardDataProcessor(STREAM *S, const char *Class, const char *Name, const char *Args)
{
TProcessingModule *Mod=NULL;

//fprintf(stderr,"SASDP: %s\n",Args);
Mod=StandardDataProcessorCreate(Class,Name,Args);
if (Mod) 
{
STREAMAddDataProcessor(S, Mod, Args);
return(TRUE);
}

return(FALSE);
}


void STREAMClearDataProcessors(STREAM *S)
{
TProcessingModule *Mod;

STREAMFlush(S);
STREAMResetInputBuffers(S);
DestroyList(S->ProcessingModules, DataProcessorDestroy);
}

