//#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>

#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <datatypes/datatypesclass.h>

#include <stdio.h>
#include <stdlib.h>

#include "datatypebm8b.h"
int LoadDataTypeToBm8b(const char *pFileName,
                         DtBm *DtBm,PLANEPTR *pmaskPlane,UBYTE **pPalette,struct Screen *pDestScreen)
{
    UBYTE                   *chunk;
    int                 chunksize,nbc;
    int             reworkedwidth;

    struct BitMapHeader     *bmhd=NULL;
    struct ColorRegister    *coloreg=NULL;

    if(pDestScreen)
    {   // remap to screen
        DtBm->obj =   NewDTObject( pFileName,
                    DTA_SourceType,         DTST_FILE,
                    DTA_GroupID,            GID_PICTURE,
                    OBP_Precision,          PRECISION_IMAGE,
                    PDTA_FreeSourceBitMap,  TRUE,
                    PDTA_Screen,            pDestScreen,
                    PDTA_Remap,             TRUE,
                   0
             );
    } else
    {
        DtBm->obj =   NewDTObject( pFileName,
                    DTA_SourceType,         DTST_FILE,
                    DTA_GroupID,            GID_PICTURE,
                    OBP_Precision,          PRECISION_IMAGE,
                   PDTA_FreeSourceBitMap,  TRUE,
                    PDTA_Remap,             TRUE,
                   0
             );
    }

  printf("NewDTObject:%lx\n",(int)DtBm->obj);

    if (DtBm->obj == NULL) return(1);

//    if( GetAttr(    PDTA_ColorRegisters,obj,(ULONG *) &coloreg  )==0L ) { DisposeDTObject( obj );    return(2); }

    if( GetAttr(    PDTA_BitMapHeader,DtBm->obj,(ULONG *) &bmhd  )==0L ) { DisposeDTObject( DtBm->obj );
    DtBm->obj = NULL;
        return(2); }

    DtBm->width = (WORD)bmhd->bmh_Width;
    DtBm->height = (WORD)bmhd->bmh_Height;
    DtBm->nbColors = 1<<bmhd->bmh_Depth;
    printf("bmh_Width:%d bmh_Height:%d d:%d\n",
(int)bmhd->bmh_Width,(int)bmhd->bmh_Height, (int)bmhd->bmh_Depth
    );

    printf("bmh_Masking:%08x \n",(int) bmhd->bmh_Masking);

    DoDTMethod( DtBm->obj,0,0, DTM_PROCLAYOUT, NULL,1, 0 );

/*
#define	mskNone			0
#define	mskHasMask		1
#define	mskHasTransparentColor	2
#define	mskLasso		3
#define	mskHasAlpha		4
*/
    if(pmaskPlane)
    {
        *pmaskPlane = NULL; // default.
         /* NULL or mask plane for use with BltMaskBitMapRastPort() (PLANEPTR) */
        GetAttr(    PDTA_MaskPlane,DtBm->obj,(ULONG *) pmaskPlane );
        printf("PDTA_MaskPlane:%08x\n",(int) *pmaskPlane);
    }
    if(pPalette)
    {
        GetAttr(    PDTA_ColorRegisters,DtBm->obj,(ULONG *) pPalette );
    }


//    if( bmhd->bmh_Depth >8 ) { DisposeDTObject( obj );    return(3); }
    GetAttr(   PDTA_DestBitMap,  DtBm->obj,    (ULONG *) &DtBm->bm );

//    if (bm == NULL) {   GetAttr(   PDTA_BitMap,   obj,    (ULONG *) &bm ); }
    if (DtBm->bm == NULL) { DisposeDTObject( DtBm->obj ); DtBm->obj = NULL;  return(4);   }

    // get number of color in the palette
    //nbc = 1<<(bmhd->bmh_Depth);

   // reworkedwidth =  (bmhd->bmh_Width+15) & 0xfffffff0 ;

// PDTA_MaskPlane -> PLANEPTR

    //if(presult) *presult = bm;

    return 0;
}
void closeDataTypeBm(DtBm *DtBm)
{
    if(DtBm->obj) DisposeDTObject( DtBm->obj );
    DtBm->obj = NULL;

}

