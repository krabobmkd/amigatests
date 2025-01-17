#ifndef _LOADDTTOBMRGB_H_
#define _LOADDTTOBMRGB_H_

#include <exec/types.h>

/** just enough to retreive a true color bitmap from datatypes
*/
typedef struct _RGBBitmap {
    ULONG _width,_height;
    ULONG _bytesPerRow;
    ULONG _pixelByteSize; // pixel mode is RGB if 3, ARGB if 4.
    UBYTE *_ARGB;
} RGBBitmap;

// else 24 bit.
#define DO32BITARGB 1

/** LoadDataTypeToBmRGB use Amiga OS datatypes to retreive a RGB truecolor pixmap.
 * \param  pFileName file name (png, gif, ilbm, ...)
 * \param  presult receive an allocated struct to be freed with closeBmRGB().
 * if do32Bits ==0 will be 24.
  */
int LoadDataTypeToBmRGB(const char *pFileName,RGBBitmap **presult, int do32Bits);
void closeBmRGB(RGBBitmap *pRGBbm);

#endif


