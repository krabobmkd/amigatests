//#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>

#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <datatypes/datatypesclass.h>

#include <stdio.h>
#include <stdlib.h>

#include "datatypebm.h"
int LoadDataTypeToBm8b(const char *pFileName,
                         DtBm *DtBm,PLANEPTR *maskPlane,struct Screen *pDestScreen)
{
    UBYTE                   *chunk;
    int                 chunksize,nbc;
    int             reworkedwidth;

    struct BitMapHeader     *bmhd=NULL;
    struct ColorRegister    *coloreg=NULL;

    DtBm->obj =   NewDTObject( pFileName,
                        DTA_SourceType,         DTST_FILE,
                        DTA_GroupID,            GID_PICTURE,
                        OBP_Precision,          PRECISION_IMAGE,
                        PDTA_FreeSourceBitMap,  TRUE,
                        PDTA_Screen,            pDestScreen,
                        PDTA_Remap,             TRUE,
                       0
                 );


  printf("NewDTObject:%lx\n",(int)DtBm->obj);

    if (DtBm->obj == NULL) return(1);

//    if( GetAttr(    PDTA_ColorRegisters,obj,(ULONG *) &coloreg  )==0L ) { DisposeDTObject( obj );    return(2); }

    if( GetAttr(    PDTA_BitMapHeader,DtBm->obj,(ULONG *) &bmhd  )==0L ) { DisposeDTObject( DtBm->obj );
    DtBm->obj = NULL;
        return(2); }

    printf("bmh_Width:%d bmh_Width:%d d:%d\n",
(int)bmhd->bmh_Width,(int)bmhd->bmh_Height, (int)bmhd->bmh_Depth
    );

    printf("bmh_Masking:%08x ",(int) bmhd->bmh_Masking);

    DoDTMethod( DtBm->obj,0,0, DTM_PROCLAYOUT, NULL,1, 0 );

/*
#define	mskNone			0
#define	mskHasMask		1
#define	mskHasTransparentColor	2
#define	mskLasso		3
#define	mskHasAlpha		4
*/
//    if(pmaskplane)
//    {
//        *pmaskplane = NULL; // default.
//         /* NULL or mask plane for use with BltMaskBitMapRastPort() (PLANEPTR) */
//        GetAttr(    PDTA_MaskPlane,obj,(ULONG *) &pmaskplane );
//        printf("PDTA_MaskPlane:%08x ",(int) *pmaskplane);
//    }


//    if( bmhd->bmh_Depth >8 ) { DisposeDTObject( obj );    return(3); }
    GetAttr(   PDTA_DestBitMap,  DtBm->obj,    (ULONG *) &DtBm->bm );

    if(maskPlane)
    {
        GetAttr(   PDTA_DestBitMap,  DtBm->obj, (ULONG *) maskPlane );
    }

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

