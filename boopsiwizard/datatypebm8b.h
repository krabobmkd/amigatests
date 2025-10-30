#ifndef _LOADDTTOBM_H_
#define _LOADDTTOBM_H_

#include <exec/types.h>
#include <intuition/screens.h>


typedef struct {
    void *obj ;
    struct BitMap *bm;
    WORD   width; // height is bm->rows
    WORD   height;
    WORD    nbColors,d;
} DtBm;

int LoadDataTypeToBm8b(const char *pFileName,
                        DtBm *DtBm,PLANEPTR *pmaskPlane,UBYTE **pPalette, struct Screen *pDestScreen);
void closeDataTypeBm(DtBm *DtBm);
#endif


