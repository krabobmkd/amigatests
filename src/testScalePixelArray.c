#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>

#include <cybergraphx/cybergraphics.h>

#include <stdio.h>
#include <stdlib.h>

#include "datatypebmRGB.h"

struct GfxBase *GfxBase = NULL;
struct IntuitionBase *IntuitionBase = NULL;

struct Library * CyberGfxBase = NULL;
struct Library * DataTypesBase = NULL;

struct Window *w=NULL;
RGBBitmap *bm_argb=NULL;

void exitclose()
{
    if(w) {
        CloseWindow(w);
        w = NULL;
    }
    if(bm_argb)
    {
        closeBmRGB(bm_argb);
        bm_argb=NULL;
    }
    if(DataTypesBase) CloseLibrary(DataTypesBase);
    if(CyberGfxBase) CloseLibrary(CyberGfxBase);
    if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    if(GfxBase) CloseLibrary((struct Library *)GfxBase);

}

void drawScalePixelArray(struct Window *pwin)
{
    // actually clear bitmap so we see the problems on ScalePixelArray()
    SetAPen(pwin->RPort,0);
    RectFill(pwin->RPort,0,0,pwin->GZZWidth,pwin->GZZHeight);

    // cybergraphics ScalePixelArray() allow to scale and draw truecolor bitmap.

    ScalePixelArray((APTR)bm_argb->_ARGB,
            bm_argb->_width,bm_argb->_height, // source rectangle dimensions
            bm_argb->_bytesPerRow,
                pwin->RPort,
                0,0, // dest x, y
                pwin->GZZWidth,   // dest rectangle dimensions
                pwin->GZZHeight,
                ((bm_argb->_pixelByteSize==3)? RECTFMT_RGB : RECTFMT_ARGB )
                );
}

int main(int argc, char **argv)
{
    int res;
    atexit(exitclose);

    GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",39);
    if(!GfxBase)  { printf("need os3\n"); return 1; }
    IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",39);
    if(!IntuitionBase)  { printf("need os3\n"); return 1; }

    CyberGfxBase = OpenLibrary("cybergraphics.library",1);
    if(!CyberGfxBase)  { printf("cybergraphics not found\n"); return 1; }

    DataTypesBase = OpenLibrary("datatypes.library",43);
    if(!DataTypesBase)  {
        printf("Datatype version >=43 to read truecolor image not found\n");
        return 1;
    }

    res = LoadDataTypeToBmRGB("rgbimage.png",&bm_argb,DO32BITARGB);

    if(res != 0)
    {
      printf("couldn't open rgbimage.png or missing png datatype or dataype version <v43\n");
      return(1);
    }

    if(!w)
    {
        ULONG screenDepth;
        int wwidth = 256;
        int wheight = 220;
        int _widthphys,_heightphys,xcen,ycen;
        struct Screen *pWbScreen;

        if (!(pWbScreen = LockPubScreen(NULL))
            ) return 1;

        screenDepth = GetBitMapAttr(pWbScreen->RastPort.BitMap,BMA_DEPTH);
        if(screenDepth<15)
        {
            UnlockPubScreen(NULL,pWbScreen);
            printf("This example needs a  15/16/24/32 bit truecolor screen. %d\n",screenDepth);
            return 1;
        }
        _widthphys = pWbScreen->Width;
        _heightphys = pWbScreen->Height;

    // open window in center of workbench
    xcen = (pWbScreen->Width - wwidth);
    ycen = (pWbScreen->Height - wheight);
    if(xcen<0) xcen=0;
    xcen>>=1;
    if(ycen<0) ycen=0;
    ycen>>=1;

    w = (struct Window *)OpenWindowTags(NULL,
        WA_Left,xcen,
        WA_Top,ycen,
            WA_InnerWidth, wwidth,
            WA_InnerHeight, wheight,
            WA_MaxWidth,  wwidth*4,
            WA_MaxHeight, wheight*4,
            WA_MinWidth, 32,
            WA_MinHeight, 16,
        WA_RptQueue,0, // no rawkey repeat messages
        WA_IDCMP,/* IDCMP_GADGETUP | IDCMP_GADGETDOWN |*/
            IDCMP_MOUSEBUTTONS |  IDCMP_RAWKEY | IDCMP_REFRESHWINDOW |
            IDCMP_NEWSIZE  | IDCMP_CLOSEWINDOW,

        WA_Flags,
            WFLG_SIZEGADGET | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET
             | WFLG_ACTIVATE | WFLG_GIMMEZEROZERO | WFLG_SIMPLE_REFRESH
            ,
        WA_Title,(ULONG)"ScalePixelArray() RGB Test", /* take title from version string */
        WA_PubScreen, (ULONG)pWbScreen,
        TAG_DONE
        );

        UnlockPubScreen(NULL,pWbScreen);
        if(w == NULL) return 2;

    } // end if window todo

    // draw a first time is needed with WFLG_SIMPLE_REFRESH.
    drawScalePixelArray(w);

    // - - - -
    {
    int iquit=0;

    while(!iquit)
    {       
        struct IntuiMessage *im;
        int doDraw=0;
        ULONG bitsToWait = 1 << (w->UserPort->mp_SigBit);
        Wait(bitsToWait);

        while((im = (struct IntuiMessage *) GetMsg(w->UserPort)))
        {
            ULONG imclass = im->Class;
            UWORD imcode  = im->Code;
            UWORD imqual  = im->Qualifier;

            ReplyMsg((struct Message *) im); // the faster the better.
            switch(imclass)
            {
                case IDCMP_RAWKEY:
                if(!(imqual & IEQUALIFIER_REPEAT) )
                {
                    UWORD finalkeycode = imcode & 0x007f; // rawmask
                   // printf("key:%04x\n",finalkeycode);
                    if((imcode & IECODE_UP_PREFIX)==0)
                    {
                        // exit with esc key
                        if( finalkeycode == 0x45 ) iquit = 1;
                    }
                }
                break;
                case IDCMP_CLOSEWINDOW:
                    iquit=1;
                break;
                case IDCMP_NEWSIZE:
                case IDCMP_REFRESHWINDOW:
                    doDraw = 1;
                    break;
                default:
                break;
            } // end switch msg class
        } // end while message

        if(doDraw) drawScalePixelArray(w);

    } // end main loop
    }

    // close is done by atexit func.
    return 0;
}
