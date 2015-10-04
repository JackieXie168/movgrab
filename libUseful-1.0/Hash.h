#ifndef LIBUSEFUL_HASH_H
#define LIBUSEFUL_HASH_H

#include "file.h"
#include "includes.h"


#define ENCODE_HEX 0
#define ENCODE_BASE64 1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct t_hash THash;

typedef void (*HASH_UPDATE)(THash *Hash, char *Data, int DataLen);
typedef void (*HASH_FINISH)(THash *Hash, int Encoding, char **RetStr);

struct t_hash
{
int Type;
void *Ctx;
HASH_UPDATE Update;
HASH_FINISH Finish;
};

THash *HashInit(char *Type);
char *EncodeBase64(char *Return, char *Text, int len);
char *DecodeBase64(char *Return, int *len, char *Text);
char *HashMD5(char *Return, char *text, int len, int Encoding);


#ifdef __cplusplus
}
#endif



#endif
