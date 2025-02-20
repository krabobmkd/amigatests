//#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>

#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <datatypes/datatypesclass.h>

#include <stdio.h>
#include <stdlib.h>


int LoadDataTypeToBm(const char *pFileName,
                        struct BitMap **presult,
                        UBYTE   **pmaskplane,struct Screen *pDestScreen)
{
    Object                  *obj ;

    UBYTE                   *chunk;
    int                 chunksize,nbc;
    int             reworkedwidth;

    struct BitMapHeader     *bmhd=NULL;
    struct BitMap           *bm=NULL;
    struct ColorRegister    *coloreg=NULL;

    obj =   NewDTObject( pFileName,
                        DTA_SourceType,         DTST_FILE,
                        DTA_GroupID,            GID_PICTURE,
                        OBP_Precision,          PRECISION_IMAGE,
                        PDTA_FreeSourceBitMap,  FALSE,
                        PDTA_Screen,            pDestScreen,
                        PDTA_Remap,             TRUE,
                       0
                 );



    if (obj == NULL) return(1);

    if( GetAttr(    PDTA_ColorRegisters,obj,(ULONG *) &coloreg  )==0L ) { DisposeDTObject( obj );    return(2); }

    DoDTMethod( obj,0,0, DTM_PROCLAYOUT, NULL,1, 0 );

    if( GetAttr(    PDTA_BitMapHeader,obj,(ULONG *) &bmhd  )==0L ) { DisposeDTObject( obj );    return(2); }

    printf("bmh_Masking:%08x ",(int) bmhd->bmh_Masking);
/*
#define	mskNone			0
#define	mskHasMask		1
#define	mskHasTransparentColor	2
#define	mskLasso		3
#define	mskHasAlpha		4
*/
    if(pmaskplane)
    {
        *pmaskplane = NULL; // default.
         /* NULL or mask plane for use with BltMaskBitMapRastPort() (PLANEPTR) */
        GetAttr(    PDTA_MaskPlane,obj,(ULONG *) &pmaskplane );
        printf("PDTA_MaskPlane:%08x ",(int) *pmaskplane);
    }


//    if( bmhd->bmh_Depth >8 ) { DisposeDTObject( obj );    return(3); }
    GetAttr(   PDTA_DestBitMap,  obj,    (ULONG *) &bm );

    if (bm == NULL) {   GetAttr(   PDTA_BitMap,   obj,    (ULONG *) &bm ); }
    if (bm == NULL) { DisposeDTObject( obj );   return(4);   }

    // get number of color in the palette
    nbc = 1<<(bmhd->bmh_Depth);

   // reworkedwidth =  (bmhd->bmh_Width+15) & 0xfffffff0 ;

// PDTA_MaskPlane -> PLANEPTR

    DisposeDTObject( obj );
    if(presult) *presult = bm;

    return 0;
}


