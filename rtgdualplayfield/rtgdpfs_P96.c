#include "rtgdpfs_p96.h"
#include <proto/Picasso96.h>
#include "extrarastport.h"
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>

struct rtg_dpf_screen_p96 {

    struct rtg_dpf_screen _super;

    struct Window *_win_backdrop;

    // as allocated and freed
    struct BitMap *_main_bm;
    // same, tweaked for use for rtg screen.
    struct BitMap _rtg_bm;
    // same, tweaked to be presented as pf1 rastport
     struct BitMap _pf1_bm;
    // same, tweaked to be presented as pf2 rastport
     struct BitMap _pf2_bm;

};

static void p96_close(struct rtg_dpf_screen_p96 *pthis)
{
    if(!pthis) return;
    // CloseExtraRastPort(&pthis->_pf2rp);

    // if(pthis->_win_backdrop) CloseWindow(pthis->_win_backdrop);
    // if(pthis->_super._screen)   CloseScreen(pthis->_super._screen);

    // if(pthis->_rasinfo2) FreeVec(pthis->_rasinfo2);
    FreeVec(pthis);
}


void p96_setPalette(struct rtg_dpf_screen_ocs* pthis,const UBYTE *ppalette, int nbcolors, int iPlayfield)
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


static void p96_setscroll(struct rtg_dpf_screen_ocs* pthis, WORD scrollx1, WORD scrolly1, WORD scrollx2, WORD scrolly2)
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
    struct rtg_dpf_screen_p96 *pthis = (struct rtg_dpf_screen_p96 *)
                    AllocVec(sizeof(struct rtg_dpf_screen_p96), MEMF_CLEAR);
    if(!pthis) return NULL;

    pthis->_super.close = p96_close;
    pthis->_super.setPalette = p96_setPalette;
    pthis->_super.setscroll = p96_setscroll;

    // - - - - -

    // Open screen with pre allocated bitmaps in planar format, even if target mode is chunky.
    int maxw = (pf1width>pf2width)?pf1width:pf2width;
    int maxh = (pf1height>pf2height)?pf1height:pf2height;

    // got to alloc with max
    pthis->_main_bm = AllocBitMap(maxw,maxh,8,BMF_CLEAR,NULL);
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
                        SA_BitMap,


//                        SA_Behind,TRUE,    /* Open behind */
//                        SA_Quiet,TRUE,     /* quiet */
//			SA_Type,CUSTOMSCREEN,
//			SA_Interleaved,FALSE, // test, may make C2P faster
			SA_Colors,(ULONG)&colspec[0],
                        0 );

    return pthis;

}
