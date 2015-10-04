#ifndef BASE_64_H
#define BASE_64_H

void to64frombits(unsigned char *out, const unsigned char *in, int inlen);
int from64tobits(char *out, const char *in);

#endif
