#ifndef _LOADDTTOBM_H_
#define _LOADDTTOBM_H_

#include <exec/types.h>

struct BitMap;

typedef struct {
    void *obj ;
    struct BitMap *bm;
} DtBm;

int LoadDataTypeToBm8b(const char *pFileName,
                        DtBm *DtBm,PLANEPTR *maskPlane,struct Screen *pDestScreen);
void closeDataTypeBm(DtBm *DtBm);
#endif


