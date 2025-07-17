#include "rtgdpfs_OCS.h"
#include "extrarastport.h"
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>
//#include <proto/alib.h>
#include <intuition/screens.h>
#include <graphics/displayinfo.h>
#include <graphics/modeid.h>
#include <stdio.h>
#include <stdlib.h>



struct rtg_dpf_screen_ocs {

    struct rtg_dpf_screen _super;

    struct Window *_win_backdrop;

    ULONG _modeid;
    ULONG _width,_height;

    struct ExtraRastPortAndBm _pf2rp;
    struct RasInfo *_rasinfo2;
    // tricky private things
    ULONG _initialDepth;
    ULONG _fp2_paletteShift;
    ULONG _maxNbColsPerDpf;
};

static void ocs_close(struct rtg_dpf_screen_ocs *pthis)
{
    if(!pthis) return;

    if(pthis->_win_backdrop) CloseWindow(pthis->_win_backdrop);

    if(pthis->_super._screen)
    {
        // uninstall pf2 - undocumented by example, but looks more clean like that ?
        {
            ScreenToBack(pthis->_super._screen); // less glitches
            Forbid();
                /* Install rinfo for viewport's second playfield */
                pthis->_super._screen->ViewPort.RasInfo->Next = NULL;
                pthis->_super._screen->ViewPort.Modes &= ~DUALPF;

                pthis->_super._screen->RastPort.BitMap->Depth = pthis->_initialDepth; // pfioooooou
                pthis->_super._screen->BitMap.Depth = pthis->_initialDepth;

            Permit();
            /* Put viewport change into effect */
            MakeScreen(pthis->_super._screen);
            RethinkDisplay();
        }

        CloseScreen(pthis->_super._screen);
    }
    CloseExtraRastPortAndBm(&pthis->_pf2rp);

    if(pthis->_rasinfo2) FreeVec(pthis->_rasinfo2);
    FreeVec(pthis);
}

void ocs_setPalette(struct rtg_dpf_screen_ocs* pthis,const UBYTE *ppalette, int nbcolors, int iPlayfield)
{
    struct Screen *pscreen = NULL;
    if(!pthis || pthis->_super._screen==NULL) return;
    pscreen = pthis->_super._screen;

    if(nbcolors>pthis->_maxNbColsPerDpf) nbcolors=pthis->_maxNbColsPerDpf; // max supported for a playfield.

 //   int ishiftpal = (iPlayfield)?pthis->_fp2_paletteShift:0;


    int nbc = (int)pscreen->ViewPort.ColorMap->Count;

    int ipalshift = (iPlayfield)?pthis->_fp2_paletteShift:0;
    printf("nbcolors:%d ipalshift:%d nbctoset:%d\n",nbc,ipalshift,nbcolors);
    for(int i=0;i<nbcolors ; i++)
    {
         ULONG r = ((ULONG)*ppalette++)<<24;
          ULONG g = ((ULONG)*ppalette++)<<24;
         ULONG b = ((ULONG)*ppalette++)<<24;
        if((i+ipalshift)<nbc) // or crash
        {
            SetRGB32( &(pscreen->ViewPort),i+ipalshift,  r,  g,  b );
        }
    }




}


static void ocs_setscroll(struct rtg_dpf_screen_ocs* pthis, WORD scrollx1, WORD scrolly1, WORD scrollx2, WORD scrolly2)
{
    struct Screen *pscreen = NULL;
    if(!pthis || pthis->_super._screen==NULL) return;
    pscreen = pthis->_super._screen;

    // ScrollVPort could be heavy for some drivers
    if( scrollx1 != pscreen->ViewPort.DxOffset ||
        scrolly1 != pscreen->ViewPort.DyOffset ||

        scrollx2 != pthis->_rasinfo2->RxOffset ||
        scrolly2 != pthis->_rasinfo2->RyOffset
        )
       {
            pscreen->ViewPort.DxOffset = scrollx1;
            pscreen->ViewPort.DyOffset = scrolly1;

            pthis->_rasinfo2->RxOffset = scrollx2;
            pthis->_rasinfo2->RyOffset = scrolly2;

            ScrollVPort(&(pscreen->ViewPort));
       }

}

//static void makeDualPF(struct rtg_dpf_screen_ocs *pthis)
//{
//struct Screen   *myscreen;
//struct RasInfo  *rinfo2;
//struct BitMap   *bmap2;
//struct RastPort *rport2;

//myscreen = win->WScreen;   /* Find the window's screen */

///* Allocate the second playfield's rasinfo, bitmap, and bitplane */
//rinfo2 = (struct RasInfo *) AllocMem(sizeof(struct RasInfo), MEMF_PUBLIC | MEMF_CLEAR);
//if ( rinfo2 != NULL )
//    {
//    /* Get a rastport, and set it up for rendering into bmap2 */
//    rport2 = (struct RastPort *) AllocMem(sizeof(struct RastPort), MEMF_PUBLIC );
//    if (rport2 != NULL )
//        {
//        bmap2 = (struct BitMap *) AllocMem(sizeof(struct BitMap), MEMF_PUBLIC | MEMF_CLEAR);
//        if (bmap2 != NULL )
//            {
//            InitBitMap(bmap2, 1, myscreen->Width, myscreen->Height);

//            /* extra playfield will only use one bitplane here. */
//            bmap2->Planes[0] = (PLANEPTR) AllocRaster(myscreen->Width, myscreen->Height);
//            if (bmap2->Planes[0] != NULL )
//                {
//                InitRastPort(rport2);
//                rport2->BitMap = rinfo2->BitMap = bmap2;

//                SetRast(rport2, 0);

//                if (installDualPF(myscreen,rinfo2))
//                    {
//                    /* Set foreground color; color 9 is color 1 for
//                    ** second playfield of hi-res viewport
//                    */
//                    SetRGB4(&myscreen->ViewPort, 9, 0, 0xF, 0);

//                    drawSomething(rport2);

//                    handleIDCMP(win);

//                    removeDualPF(myscreen);
//                    }
//                FreeRaster(bmap2->Planes[0], myscreen->Width, myscreen->Height);
//                }
//            FreeMem(bmap2, sizeof(struct BitMap));
//            }
//        FreeMem(rport2, sizeof(struct RastPort));
//        }
//    FreeMem(rinfo2, sizeof(struct RasInfo));
//    }
//}

struct rtg_dpf_screen* Create_dualplayfield_screen_OCS(int pf1width,int pf1height,int pf2width,int pf2height)
{
    struct rtg_dpf_screen_ocs *pthis = (struct rtg_dpf_screen_ocs *)
                    AllocVec(sizeof(struct rtg_dpf_screen_ocs), MEMF_CLEAR);
    if(!pthis) return NULL;

    pthis->_super.close = ocs_close;
   // pthis->_super.copyBm = ocs_copyBm;
    pthis->_super.setPalette = ocs_setPalette;
    pthis->_super.setscroll = ocs_setscroll;

    // - - - - -
    // 1.3: test 16b/24b depth then if fail 8b,5b,4b , for native modes AGA/OCS.
    int isAGA= 1;
    int initialDepth;
    int appliedDepth;
    if(isAGA)
    {
        initialDepth = 4;
        appliedDepth = 4;
        pthis->_fp2_paletteShift = 8;
        pthis->_maxNbColsPerDpf = 16;
    } else
    {
        //OCS/ECS
        initialDepth = 3;
        appliedDepth = 3;
        pthis->_fp2_paletteShift = 8;
        pthis->_maxNbColsPerDpf = 8;
    }
    pthis->_initialDepth = initialDepth; // used for closing.

    pthis->_modeid = BestModeID(
            BIDTAG_Depth,appliedDepth,
            BIDTAG_DIPFMustHave, DIPF_IS_DUALPF , // DIPF_IS_PF2PRI
            BIDTAG_NominalWidth,320,
            BIDTAG_NominalHeight,200,
            TAG_DONE );

    if(pthis->_modeid == INVALID_ID)
    {
        printf("got invalid id\n");
        ocs_close(pthis);
        return NULL;
    }
            printf("mode id %08x\n",pthis->_modeid);
    // get height
   {
        LONG v;
        struct DimensionInfo dims;
        v = GetDisplayInfoData(NULL, (UBYTE *) &dims, sizeof(struct DimensionInfo),
                     DTAG_DIMS, pthis->_modeid);
        if(v>0)
        {
            pthis->_width = (int)(dims.Nominal.MaxX - dims.Nominal.MinX)+1;
            pthis->_height = (int)(dims.Nominal.MaxY - dims.Nominal.MinY)+1;
        } else
        {   // shouldnt happen, fallback
            pthis->_width = 320;
            pthis->_height = 200;
        }
   }
    printf("nominal %d %d\n",pthis->_width,pthis->_height);

//	struct ColorSpec colspec[8]={ // let's do it amiga default like
//                0,  0,0,0, //black
//                1,  8,8,8,  // grey
//                // end
//                -1,0,0,0};


    // on OCS/ECS,
    // dual playfield is always lowres , 8color + 8color,
    // and those colors are always [0,7] [16,23] in a 32c palette. (mouse can be [24,31])

    // on AGA,
    // dual playfield is always lowres , 16color + 16color,
    // at hardware level it can use any of the first 16c of the 8 "32c slots".
    // to force intuition to manage the full 256c we ask 256 colors intially.

    // on P96, dualpf is tricked basically we trick a 256c planar to chunly mode. -> other code.


    pthis->_super._screen = OpenScreenTags( NULL,
			SA_DisplayID,pthis->_modeid,
                        SA_Title, (ULONG)"DualPF", // used as ID by promotion tools and else ?
                        SA_Width, pf1width,
                        SA_Height,pf1height,
                        SA_Depth,initialDepth, // big fake of asking for 256 colors instead of 8/16
                        SA_Behind,TRUE,    /* Open behind, less glitch at init */
//                        SA_Quiet,TRUE,     /* quiet */
//			SA_Type,CUSTOMSCREEN,
//			SA_Interleaved,FALSE, // test, may make C2P faster
//			SA_Colors,(ULONG)&colspec[0],
                        0 );

	if( pthis->_super._screen == NULL )
	{
        printf("OpenScreen fail\n");
        ocs_close(pthis);
        return NULL;
	}
        printf("OpenScreen ok\n");
	// - - - -make dual

//if ( rinfo2 != NULL )
	// alloc pf2 bitmap, use gfx drawable rastport since we're at it:
	if(OpenExtraRastPortAndBm(&pthis->_pf2rp,pf2width,pf2height,appliedDepth,
            pthis->_super._screen->RastPort.BitMap
            )==0)
	{
        printf("OpenExtraRastPort fail\n");
        ocs_close(pthis);
        return NULL;
	}
    pthis->_rasinfo2 =  (struct RasInfo *) AllocVec(sizeof(struct RasInfo), MEMF_PUBLIC | MEMF_CLEAR);
    if(pthis->_rasinfo2 == NULL)	{
	    printf("allow rasinfo fail\n");
        ocs_close(pthis);
        return NULL;
	}
/* from docs:
    The DUALPF and PFBA modes are related.  DUALPF tells the system to treat
    the raster specified by this ViewPort as the first of two independent and
    separately controllable playfields. It also modifies the manner in which
    the pixel colors are selected for this raster (see the above table).

	   Depth       Color	   Depth       Color
       (PF-1)    Registers     (PF-2)    Registers
       ------    ---------     ------    ---------
         1          0,1          1          8,9
         2          0-3          1          8,9
         2          0-3          2          8-11
         3          0-7          2          8-11
         3          0-7          3          8-15

When PFBA is specified, it indicates that the second playfield has video
priority over the first one.  Playfield relative priorities can be
controlled when the playfield is split into two overlapping regions.
Single-playfield and dual-playfield modes are discussed below in
"Advanced Topics."
*/

	pthis->_rasinfo2->BitMap = pthis->_pf2rp._bm;
        printf("install\n");
	// install pf2
    {
        Forbid();
            pthis->_super._screen->RastPort.BitMap->Depth = appliedDepth; // pfioooooou
            pthis->_super._screen->BitMap.Depth = appliedDepth;

            /* Install rinfo for viewport's second playfield */
            pthis->_super._screen->ViewPort.RasInfo->Next = pthis->_rasinfo2;
            pthis->_super._screen->ViewPort.Modes |= DUALPF;

        Permit();

        /* Put viewport change into effect */
        MakeScreen(pthis->_super._screen);
        RethinkDisplay();

        // then only open to front, less glitch
        ScreenToFront(pthis->_super._screen);
    }
        printf("install ok\n");
	// --------- open intuition fullscreen window for this screen:

    pthis->_win_backdrop = OpenWindowTags(/*&screenwin*/NULL,
        WA_CustomScreen,(ULONG)pthis->_super._screen,
                    WA_Backdrop,FALSE,
                    WA_Borderless,TRUE,
                    WA_Activate,TRUE,
                    //WA_RMBTrap,TRUE,
                    WA_ReportMouse,0,
                    WA_SizeGadget,0,
                    WA_DepthGadget,0,
                    WA_CloseGadget,0,
                    //WA_DragBar,0, WA_RptQueue,0, // empeach key repeat messages
                   // WA_GimmeZeroZero,FALSE,
                    WA_IDCMP,IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY ,
                    0 );

	if( pthis->_win_backdrop ==  NULL )
	{
        ocs_close(pthis);
        return NULL;
	}


    pthis->_super._userPort = pthis->_win_backdrop->UserPort;
    pthis->_super._pf1rp = &(pthis->_super._screen->RastPort);
    pthis->_super._pf2rp = pthis->_pf2rp._erp._rp;

    printf("pthis->_super._pf1rp->BitMap.Depth: %d\n",(int) pthis->_super._pf1rp->BitMap->Depth);
    printf("pthis->_super._pf2rp->BitMap.Depth: %d\n",(int) pthis->_super._pf2rp->BitMap->Depth);

	// ------- set invisible mouse pointer:
//	_pMouseRaster =  AllocRaster(8 ,8) ;
//	if(_pMouseRaster)
//	{
//        SetPointer( _pScreenWindow ,(UWORD *) _pMouseRaster, 0,1,0,0);
//    }

    return  &(pthis->_super);
}
