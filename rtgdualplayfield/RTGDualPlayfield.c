
#include "datatypebm8b.h"
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>
#include <proto/alib.h>

#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <datatypes/datatypesclass.h>

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    er_ok=0,
    er_cantalloc,er_cantopen,
    er_cantgetinfo,er_cantgetbm,
    er_cantcgx,er_dtcantrgb
} eloadDtBm;

static const char *errortexts[]={
    "Ok",
    "Couldn't open image as Dataype",
    "Can't get information from datatype object",
    "Can't get Bitmap from datatype object",
    "Couldn't find CGX api",
    "Dataype version <43"
};
// = = = = =  workaround if SDK is 3.1, this is for datatype v43.
#ifndef PDTA_DestMode
#define PDTA_DestMode		(DTA_Dummy + 251)
#define PMODE_V43 (1)	/* Extended mode */
#endif
#ifndef PDTA_SubClassRendersAll
#define PDTA_SubClassRendersAll	(DTA_Dummy + 261)
#endif

#ifndef PBPAFMT_RGB
#define PBPAFMT_RGB	0	/* 3 bytes per pixel (red, green, blue) */
#define PBPAFMT_RGBA	1	/* 4 bytes per pixel (red, green, blue, alpha channel) */
#define PBPAFMT_ARGB	2	/* 4 bytes per pixel (alpha channel, red, green, blue) */
#define PBPAFMT_LUT8	3	/* 1 byte per pixel (using a separate colour map) */
#define PBPAFMT_GREY8	4	/* 1 byte per pixel (0==black, 255==white) */
#endif
#ifndef PDTM_Dummy
#define PDTM_Dummy (DTM_Dummy + 0x60)
#endif
#ifndef PDTM_READPIXELARRAY
#define PDTM_READPIXELARRAY (PDTM_Dummy + 1)
#endif
// = = = = = =

extern struct Library * DataTypesBase;

int LoadDataTypeToBmRGB(const char *pFileName,RGBBitmap **preturn, int do32Bits)
{
    Object                  *obj ;
    struct BitMapHeader     *bmhd=NULL;
    struct BitMap           *bm=NULL;

    if(!pFileName) return((int)er_cantopen); // where did you have your head tonight ?

    // need v43 datatype to read truecolor
    //printf("DataTypes v%d\n",(int)DataTypesBase->lib_Version);
    if(DataTypesBase->lib_Version <43) {
        return((int)er_dtcantrgb);
    }

    // default return NULL;
    if(preturn) *preturn = NULL;

    obj =   NewDTObject( pFileName,
                        DTA_SourceType,         DTST_FILE,
                        DTA_GroupID,            GID_PICTURE,
                        PDTA_Remap,             FALSE,

                        PDTA_DestMode,          PMODE_V43, // me want 24b, else remaped to 8.
                        PDTA_SubClassRendersAll, TRUE, // needed ?
                       TAG_END
                 );

    if (obj == NULL) return((int)er_cantopen);

    if( GetAttr(    PDTA_BitMapHeader,obj,(ULONG *) &bmhd  )==0L ) { DisposeDTObject( obj );    return((int)er_cantgetinfo); }
   // printf("image width:%d height:%d depth:%d\n",bmhd->bmh_Width,bmhd->bmh_Height,bmhd->bmh_Depth);
    if(bmhd->bmh_Width ==0 || bmhd->bmh_Height==0) { DisposeDTObject( obj );    return((int)er_cantgetinfo); }

    if(preturn)
    {
        ULONG pixelmapsize,nbpix;
        ULONG bytesPerPixels = (do32Bits)?4:3;
        ULONG pixelMode = (do32Bits)?PBPAFMT_ARGB:PBPAFMT_RGB;        

        // RGBBitmap is my private RGB24 struct to be returned.
        RGBBitmap *pRgbBm = AllocVec(sizeof(RGBBitmap),MEMF_CLEAR);

        if(!pRgbBm) { DisposeDTObject( obj );   return((int)er_cantalloc);   }
        pixelmapsize = 4*bmhd->bmh_Width*bmhd->bmh_Height;
       // printf("pixel map size:%d\n",pixelmapsize);
        pRgbBm->_ARGB = AllocVec(pixelmapsize,MEMF_CLEAR);
        if(!pRgbBm->_ARGB) { FreeVec(pRgbBm); DisposeDTObject( obj );   return((int)er_cantalloc);   }

        pRgbBm->_width = bmhd->bmh_Width;
        pRgbBm->_height = bmhd->bmh_Height;
        pRgbBm->_bytesPerRow = bytesPerPixels*bmhd->bmh_Width;
        pRgbBm->_pixelByteSize = bytesPerPixels;
#if 0
        // FREEEZE
        printf("PDTM_READPIXELARRAY - DoDTMethod() version\n");
        // will freeze the whole system here:
        nbpix = DoDTMethod( obj, NULL,  NULL,
            // varargs start here
            (ULONG)PDTM_READPIXELARRAY,
            // following values actually maps struct pdtBlitPixelArray.
            (ULONG) pRgbBm->_ARGB,  // points pixels I have allocated
            (ULONG)pixelMode,
            (ULONG)bytesPerPixels*bmhd->bmh_Width,
            (ULONG)0,(ULONG)0,
            (ULONG)bmhd->bmh_Width,(ULONG)bmhd->bmh_Height,
            (ULONG)TAG_END // not sure if needed, but better i guess.
            );
#elif 0
        // FREEEZE
    printf("PDTM_READPIXELARRAY - DoDTMethodA() version\n");
    {   // DoDTMethodA version
        struct pdtBlitPixelArray bpa;
        bpa.MethodID = PDTM_READPIXELARRAY;
        bpa.pbpa_PixelData = (APTR) pRgbBm->_ARGB;
        bpa.pbpa_PixelFormat = pixelMode;
        bpa.pbpa_PixelArrayMod = bytesPerPixels*bmhd->bmh_Width;
        bpa.pbpa_Left = bpa.pbpa_Top = 0;
        bpa.pbpa_Width = bmhd->bmh_Width;
        bpa.pbpa_Height = bmhd->bmh_Height;

        nbpix = DoDTMethodA( obj, NULL,  NULL, &bpa  );
    }
#else
  //  printf("PDTM_READPIXELARRAY - alib's DoMethod() version\n");
    // IT WORKS !!! :)
    nbpix = DoMethod( obj,
            // varargs start here
            PDTM_READPIXELARRAY,
            // following values actually maps struct pdtBlitPixelArray.
            (ULONG) pRgbBm->_ARGB,  // points pixels I have allocated
            (ULONG)pixelMode,
            (ULONG)bytesPerPixels*bmhd->bmh_Width,
            (ULONG)0,(ULONG)0,
            (ULONG)bmhd->bmh_Width,(ULONG)bmhd->bmh_Height,
            (ULONG)TAG_END
      );

#endif
        // 1 if printf("nb pixels read:%d\n",nbpix);

        *preturn = pRgbBm;
    }
    // now that pixels are copied in pRgbBm->_ARGB, we can dispose the dataype object.
    DisposeDTObject( obj );

    return 0;
}

void closeBmRGB(RGBBitmap *pRGBbm)
{
    if(!pRGBbm) return;
    if(pRGBbm->_ARGB) {
        FreeVec(pRGBbm->_ARGB);
    }
    FreeVec(pRGBbm);
}
