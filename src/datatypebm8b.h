#ifndef _LOADDTTOBM8B_H_
#define _LOADDTTOBM8B_H_


struct BitMap;

/** LoadDataTypeToBm8b use Amiga OS datatypes to retreive a classic Amiga gfx.h Planar BitMap.
 * \param  pFileName file name (png, gif, ilbm, ...)
 * \param  presult receive an allocated struct to be freed with graphics FreeBitMap().
  */
int LoadDataTypeToBm8b(const char *pFileName, struct BitMap**presult);

#endif


