#include "datatypebmRGB.h"
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
//#include <proto/layers.h>
#include <proto/datatypes.h>

#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <datatypes/datatypesclass.h>

#include <stdio.h>
#include <stdlib.h>

typedef enum {
    er_ok=0,
    er_cantalloc,er_cantopen,
    er_cantgetinfo,er_cantgetbm,
    er_cantcgx
} eloadDtBm;

static const char *errortexts[]={
    "Ok",
    "Couldn't open image as Dataype",
    "Can't get information from datatype object",
    "Can't get Bitmap from datatype object",
    "Couldn't find CGX api"
};

int LoadDataTypeToBmRGB(const char *pFileName,RGBBitmap **presult)
{
    Object                  *obj ;

    UBYTE                   *chunk;
    int                 chunksize;
    int             reworkedwidth;

    struct BitMapHeader     *bmhd=NULL;
    struct BitMap           *bm=NULL;
    struct ColorRegister    *coloreg=NULL;

//    if(!CyberGfxBase) {
//        return((int)er_cantcgx);
//    }
    // default return NULL;
    if(presult) *presult = NULL;

    obj =   NewDTObject( pFileName,
                        DTA_SourceType,         DTST_FILE,
                        DTA_GroupID,            GID_PICTURE,
                        OBP_Precision,          PRECISION_IMAGE,
                        PDTA_FreeSourceBitMap,  TRUE,
                        PDTA_DestMode,          PMODE_V43, // me want 24b, else remaped to 8.
//                        PDTA_Screen,            pDestScreen,
//                        PDTA_Remap,             TRUE,
                       0
                 );

/* Remap the picture (BOOL); defaults to TRUE */
//#define	PDTA_Remap		(DTA_Dummy + 211)
/* Screen to remap to (struct Screen *) */
//#define	PDTA_Screen		(DTA_Dummy + 212)

    if (obj == NULL) return((int)er_cantopen);

 //   if( GetAttr(    PDTA_ColorRegisters,obj,(ULONG *) &coloreg  )==0L ) { DisposeDTObject( obj );    return(2); }
 //   DoDTMethod( obj,0,0, DTM_PROCLAYOUT, NULL,1, 0 );

    if( GetAttr(    PDTA_BitMapHeader,obj,(ULONG *) &bmhd  )==0L ) { DisposeDTObject( obj );    return((int)er_cantgetinfo); }
 printf("w:%d h:%d d:%d\n",bmhd->bmh_Width,bmhd->bmh_Height,bmhd->bmh_Depth);

  DoDTMethod( obj,0,0, DTM_PROCLAYOUT, NULL,1, 0 );
//  DoDTMethod( obj,0,0, GM_LAYOUT, NULL,1, 0 );
  // GM_LAYOUT

////    GetAttr(   PDTA_DestBitMap,  obj,    (ULONG *) &bm );
// GetAttr(   PDTA_BitMap,   obj,    (ULONG *) &bm );
////    if (bm == NULL) {   GetAttr(   PDTA_BitMap,   obj,    (ULONG *) &bm ); }
//    if (bm == NULL) { DisposeDTObject( obj );   return((int)er_cantgetbm);   }


    /*
			IIntuition->IDoMethod(dtype,
				PDTM_READPIXELARRAY,
				renderInfo.Memory,
				PBPAFMT_ARGB,
				srcBytesPerRow,
				0,
				0,
				srcimagewidth,
				srcimageheight,
				TAG_END);
*/
//    struct pdtBlitPixelArray pdtb_pixels;
//PDTM_OBTAINPIXELARRAY
 //   DoDTMethod( obj,0,0, PDTM_OBTAINPIXELARRAY, NULL,1, 0 );

//    if(1) { DisposeDTObject( obj );   return((int)er_cantalloc);   }

//    // fake render rastport
//    struct RastPort rp;

//    InitRastPort(&rp);
//    rp.BitMap = bm;

    {
        RGBBitmap *pRgbBm = AllocVec(sizeof(RGBBitmap),MEMF_CLEAR);
        if(!pRgbBm) { DisposeDTObject( obj );   return((int)er_cantalloc);   }

        pRgbBm->_RGB = AllocVec(4*bmhd->bmh_Width*bmhd->bmh_Height,MEMF_CLEAR);
        if(!pRgbBm->_RGB) { FreeVec(pRgbBm); DisposeDTObject( obj );   return((int)er_cantalloc);   }


        // get number of color in the palette
        //nbc = 1<<(bmhd->bmh_Depth);
        int res = DoDTMethod( obj, // datatype object
                NULL, // Window *
                NULL, // struct Requester * wtf ?
                //  DoMethod-Like tagitems start here, it's mapping struct pdtBlitPixelArray !!!
                PDTM_READPIXELARRAY,

                (ULONG)pRgbBm->_RGB,
                PBPAFMT_ARGB,
                4*bmhd->bmh_Width , // bpr
                0,0, // presume startx starty
                bmhd->bmh_Width,
                bmhd->bmh_Height,
                TAG_END
                 );
        printf("obtainpix:%d\n",res);

//// RECTFMT_RGB
//// RECTFMT_RGBA
//// RECTFMT_ARGB
////ULONG        ReadPixelArray(APTR, UWORD, UWORD, UWORD, struct RastPort *, UWORD,
////                            UWORD, UWORD, UWORD, UBYTE);
//        ULONG nbpix = ReadPixelArray((APTR)pRgbBm->_RGB,
//                            0, 0, // DestXY
//                            4*bmhd->bmh_Width, // bpr dest
//                            &rp,
//                            0, 0, // (SrcX,SrcY) - starting point in the RastPort
//                            bmhd->bmh_Width, bmhd->bmh_Height,  // (SizeX,SizeY) - size of the rectangle that should be transfered
//                            RECTFMT_ARGB );

        pRgbBm->_width = bmhd->bmh_Width;
        pRgbBm->_height = bmhd->bmh_Height;

        if(presult) *presult = pRgbBm;

        DisposeDTObject( obj );
    }

    return 0;
}

void closeBmRGB(RGBBitmap *pRGBbm)
{
    if(!pRGBbm) return;
    if(pRGBbm->_RGB) {
        FreeVec(pRGBbm->_RGB);
    }
    FreeVec(pRGBbm);
}
