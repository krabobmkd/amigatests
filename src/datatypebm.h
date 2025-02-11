#ifndef _LOADDTTOBM_H_
#define _LOADDTTOBM_H_

#include <exec/types.h>

struct BitMap;

int LoadDataTypeToBm(const char *pFileName,
                        struct BitMap **presult,
                        UBYTE   **pmaskplane,struct Screen *pDestScreen);

#endif


