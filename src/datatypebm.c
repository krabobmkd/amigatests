//#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
//#include <proto/layers.h>
#include <proto/datatypes.h>

#include <datatypes/datatypes.h>
#include <datatypes/pictureclass.h>
#include <datatypes/datatypesclass.h>
//#include <graphics/layers.h>
//#include <graphics/rastport.h>
//#include <graphics/gels.h>

#include <stdio.h>
#include <stdlib.h>


int LoadDataTypeToBm(const char *pFileName,struct BitMap **presult,struct Screen *pDestScreen)
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
                        PDTA_FreeSourceBitMap,  TRUE,
                        PDTA_Screen,            pDestScreen,
                        PDTA_Remap,             TRUE,
                       0
                 );

/* Remap the picture (BOOL); defaults to TRUE */
//#define	PDTA_Remap		(DTA_Dummy + 211)
/* Screen to remap to (struct Screen *) */
//#define	PDTA_Screen		(DTA_Dummy + 212)

    if (obj == NULL) return(1);

    if( GetAttr(    PDTA_ColorRegisters,obj,(ULONG *) &coloreg  )==0L ) { DisposeDTObject( obj );    return(2); }

    DoDTMethod( obj,0,0, DTM_PROCLAYOUT, NULL,1, 0 );

    if( GetAttr(    PDTA_BitMapHeader,obj,(ULONG *) &bmhd  )==0L ) { DisposeDTObject( obj );    return(2); }
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


