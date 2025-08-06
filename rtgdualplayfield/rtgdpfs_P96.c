#include "rtgdpfs_p96.h"
#include <proto/Picasso96.h>
#include "extrarastport.h"
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

// this way if no p96 found, it's still ok
#define P96EXPLICITLOAD 1
#ifdef P96EXPLICITLOAD
struct Library *P96Base = NULL;
static void CloseP96()
{
    if(P96Base) CloseLibrary(P96Base);
}
#endif

int initP96()
{
#ifdef P96EXPLICITLOAD
    if(P96Base == NULL)
    {
        P96Base = OpenLibrary(P96NAME,1);
        if(!P96Base) return 0;
        atexit(&CloseP96);
        return 1;
    } else return 1;
#else
    return (int)(P96Base != NULL);
#endif
}

struct rtg_dpf_screen_p96 {

    struct rtg_dpf_screen _super;
    struct Window *_win_backdrop;
    ULONG _modeid;

    // as allocated and freed
    struct BitMap *_main_bm;
    // same, tweaked for use for rtg screen.
    //struct BitMap _rtg_bm;
    // same, tweaked to be presented as pf1 rastport
     struct BitMap _pf1_bm;
    // same, tweaked to be presented as pf2 rastport
     struct BitMap _pf2_bm;
     struct ExtraRastPort _pf1_erp;
     struct ExtraRastPort _pf2_erp;

    int _pf2front;

     // pf1 colors
     UBYTE _pf1_cols[16*3];
     UBYTE _pf2_cols[16*3];



};

static void p96_close(struct rtg_dpf_screen_p96 *pthis)
{
    if(!pthis) return;
    CloseExtraRastPort(&pthis->_pf2_erp);
    CloseExtraRastPort(&pthis->_pf1_erp);

    if(pthis->_win_backdrop) CloseWindow(pthis->_win_backdrop);
    if(pthis->_super._screen)   CloseScreen(pthis->_super._screen);

    // if(pthis->_rasinfo2) FreeVec(pthis->_rasinfo2);
    FreeVec(pthis);
}

void p96_updatePalette(struct rtg_dpf_screen_p96* pthis,const UBYTE *ppalette, int nbcolors, int iPlayfield)
{
    struct Screen *pscreen = NULL;
    if(!pthis || pthis->_super._screen==NULL) return;
    pscreen = pthis->_super._screen;

    int totaldepth = pscreen->RastPort->BitMap->Depth;

    if(pthis->_pf2front)
    {
    } else
    {
        // pf1 front.

    }

    // for(int i=0;i<nbcolors ; i++)
    // {
    //      ULONG r = ((ULONG)*ppalette++)<<24;
    //       ULONG g = ((ULONG)*ppalette++)<<24;
    //      ULONG b = ((ULONG)*ppalette++)<<24;

    //     SetRGB32( &(pscreen->ViewPort),i,  r,  g,  b );
    // }

}

void p96_setPalette(struct rtg_dpf_screen_p96* pthis,const UBYTE *ppalette, int nbcolors, int iPlayfield)
{
    struct Screen *pscreen = NULL;
    if(!pthis || pthis->_super._screen==NULL) return;
    pscreen = pthis->_super._screen;

    // for(int i=0;i<nbcolors ; i++)
    // {
    //      ULONG r = ((ULONG)*ppalette++)<<24;
    //       ULONG g = ((ULONG)*ppalette++)<<24;
    //      ULONG b = ((ULONG)*ppalette++)<<24;

    //     SetRGB32( &(pscreen->ViewPort),i,  r,  g,  b );
    // }

}


static void p96_setscroll(struct rtg_dpf_screen_p96* pthis, WORD scrollx1, WORD scrolly1, WORD scrollx2, WORD scrolly2)
{
    struct Screen *pscreen = NULL;
    if(!pthis || pthis->_super._screen==NULL) return;
    pscreen = pthis->_super._screen;

    // // ScrollVPort could be heavy for some drivers
    // if( scrollx1 != pscreen->ViewPort.DxOffset ||
    //     scrolly1 != pscreen->ViewPort.DyOffset ||

    //     scrollx2 != pthis->_rasinfo2->RxOffset ||
    //     scrolly2 != pthis->_rasinfo2->RyOffset
    //     )
    //    {
    //         pscreen->ViewPort.DxOffset = scrollx1;
    //         pscreen->ViewPort.DyOffset = scrolly1;

    //         pthis->_rasinfo2->RxOffset = scrollx2;
    //         pthis->_rasinfo2->RyOffset = scrolly2;

    //         ScrollVPort(&(pscreen->ViewPort));
    //    }

}


struct rtg_dpf_screen* Create_dualplayfield_screen_P96(int pf1width,int pf1height,int pf2width,int pf2height)
{
    if(!initP96()) return NULL;

    struct rtg_dpf_screen_p96 *pthis = (struct rtg_dpf_screen_p96 *)
                    AllocVec(sizeof(struct rtg_dpf_screen_p96), MEMF_CLEAR);
    if(!pthis) return NULL;

    pthis->_super.close = p96_close;
    pthis->_super.setPalette = p96_setPalette;
    pthis->_super.setscroll = p96_setscroll;

    // - - - - -
    int depthpf1 = 4;
    int depthpf2 = 4;


    // Open screen with pre allocated bitmaps in planar format, even if target mode is chunky.
    int maxw = (pf1width>pf2width)?pf1width:pf2width;
    int maxh = (pf1height>pf2height)?pf1height:pf2height;

    // got to alloc with max
    pthis->_main_bm = AllocBitMap(maxw,maxh,depthpf1+depthpf2,BMF_CLEAR,NULL);
    if(!pthis->_main_bm)
    {
        p96_close(pthis);
        return NULL;
    }

    // create


	struct ColorSpec colspec[8]={ // let's do it amiga default like
                0,  0,0,0, //black
                1,  8,8,8,  // grey
                // end
                -1,0,0,0};


    pthis->_super._screen = OpenScreenTags( NULL,
			SA_DisplayID,pthis->_modeid,
                        SA_Title, (ULONG)"DualPF", // used as ID by promotion tools and else ?
                        SA_Width, pf1width,
                        SA_Height,pf1height,
                        SA_Depth,8,
                        SA_Type, CUSTOMBITMAP,
                        SA_BitMap,pthis->_main_bm,

                        SA_Behind,TRUE,    /* Open behind */
//                        SA_Quiet,TRUE,     /* quiet */
//			SA_Type,CUSTOMSCREEN,
//			SA_Interleaved,FALSE, // test, may make C2P faster
			SA_Colors,(ULONG)&colspec[0],
                        0 );

	if( pthis->_super._screen == NULL )
	{
        printf("OpenScreen fail\n");
        p96_close(pthis);
        return NULL;
	}
	// - -- - -tweak 2 rastports using same planes
	UWORD    BytesPerRow;
	UWORD    Rows;
	UBYTE    Flags;
	UBYTE    Depth;
	UWORD    pad;
	PLANEPTR Planes[8];

    pthis->_pf1_bm = *(pthis->_main_bm);
    pthis->_pf1_bm.Depth = depthpf1;

    pthis->_pf2_bm = *(pthis->_main_bm);
    pthis->_pf2_bm.Depth = depthpf2;
    {
        int i;
        for(i=0;i<depthpf2;i++) pthis->_pf2_bm.Planes[i] = pthis->_main_bm->Planes[i+depthpf1];
    }

	if(!OpenExtraRastPort(&pthis->_pf1_erp, maxw,maxh,&pthis->_pf1_bm)) { p96_close(pthis); return NULL; }
	if(!OpenExtraRastPort(&pthis->_pf2_erp, maxw,maxh,&pthis->_pf2_bm)) { p96_close(pthis); return NULL; }


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
        p96_close(pthis);
        return NULL;
	}

    pthis->_super._userPort = pthis->_win_backdrop->UserPort;

    pthis->_super._pf1rp = pthis->_pf1_erp._rp;
    pthis->_super._pf2rp = pthis->_pf2_erp._rp;

    // then setPalette finish the job.

    return pthis;

}
