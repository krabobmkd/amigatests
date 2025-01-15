#ifndef _LOADDTTOBMRGB_H_
#define _LOADDTTOBMRGB_H_

#include <exec/types.h>

/** just enough to be read by datatypes to RGB. */
typedef struct _RGBBitmap {
    int _width,_height;
    UBYTE *_RGB;
} RGBBitmap;

int LoadDataTypeToBmRGB(const char *pFileName,RGBBitmap **presult);
void closeBmRGB(RGBBitmap *pRGBbm);

#endif


