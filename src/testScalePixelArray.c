#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>

#include <cybergraphx/cybergraphics.h>

#include <stdio.h>
#include <stdlib.h>

#include "datatypebmRGB.h"

struct Window *w=NULL;
RGBBitmap *bm_argb=NULL;
 struct Library * CyberGfxBase = NULL;

int init()
{

    int res = LoadDataTypeToBmRGB("rgbimage.png",&bm_argb);
    printf("loadbm: %d BM: %08x\n",res,(int)bm_argb);
    if(res != 0) exit(1);

    if(!w)
    {
    int wwidth = 256;
    int wheight = 220;

    struct Screen *pWbScreen;
    if (!(pWbScreen = LockPubScreen(NULL))
            ) return 1;
        int _widthphys = pWbScreen->Width;
        int _heightphys = pWbScreen->Height;

    // open window in center of workbench
    int xcen = (pWbScreen->Width - wwidth);
    int ycen = (pWbScreen->Height - wheight);
    if(xcen<0) xcen=0;
    xcen>>=1;
    if(ycen<0) ycen=0;
    ycen>>=1;

    w = (struct Window *)OpenWindowTags(NULL,
        WA_Left,xcen,
        WA_Top,ycen,
     //   WA_Width, _machineWidth,
     //   WA_Height, _machineHeight,
        WA_InnerWidth, wwidth,
        WA_InnerHeight, wheight,
        WA_MaxWidth,  wwidth*2,
        WA_MaxHeight, wheight*2,
        WA_MinWidth, 64,
        WA_MinHeight, 32,
        WA_RptQueue,0, // no rawkey repeat messages
        WA_IDCMP,/* IDCMP_GADGETUP | IDCMP_GADGETDOWN |*/
            IDCMP_MOUSEBUTTONS |  IDCMP_RAWKEY | IDCMP_CHANGEWINDOW | IDCMP_REFRESHWINDOW |
            IDCMP_NEWSIZE /*| IDCMP_INTUITICKS*/ | IDCMP_CLOSEWINDOW,

        WA_Flags, WFLG_SIZEGADGET /*| WFLG_SIZEBRIGHT | WFLG_SIZEBBOTTOM*/ |

            WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE /*|
            WFLG_SUPER_BITMAP*/
             | WFLG_GIMMEZEROZERO
           // | WFLG_NOCAREREFRESH
           //  | WFLG_SMART_REFRESH
            | WFLG_SIMPLE_REFRESH

            // | ((_maxzoomfactor>1)?WFLG_SIZEGADGET:0)
            ,
        WA_Title,(ULONG)"TestWindow", /* take title from version string */
        WA_PubScreen, (ULONG)pWbScreen,
        TAG_DONE
        );
//    } // end if sbm ok
    UnlockPubScreen(NULL,pWbScreen);
    if(w == NULL) return 2;

    } // end if window todo
    return 0;
}

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
    if(CyberGfxBase) CloseLibrary(CyberGfxBase);
}

int main(int argc, char **argv)
{
    atexit(exitclose);

    CyberGfxBase = OpenLibrary("cybergraphics.library",1);
    if(!CyberGfxBase)  {
        printf("CFX not found\n");
        return 1;
    }

    if(init()) return 1;

    // - - - -
    int iquit=0;
    while(!iquit) // (im = (struct IntuiMessage *) GetMsg(pMsgPort)))
    {
        ULONG bitsToWait = 1 << (w->UserPort->mp_SigBit);
        Wait(bitsToWait);

        struct IntuiMessage *im;
        while((im = (struct IntuiMessage *) GetMsg(w->UserPort)))
        {
            ULONG imclass = im->Class;
            UWORD imcode  = im->Code;
            UWORD imqual  = im->Qualifier;

            ReplyMsg((struct Message *) im); // the faster the better.
 printf("imclass:%08x\n",imclass);
            switch(imclass)
            {
                case IDCMP_RAWKEY:
                if(!(imqual & IEQUALIFIER_REPEAT) )
                {
                    // same as amiga rawkey for keyboard, then joypads are remaped.
                    // pack that to fit one byte.
                    #define IKEY_RAWMASK_CD32PADS 0x037f // rawmask has evolved with CD32 pads
                    UWORD finalkeycode = imcode & IKEY_RAWMASK_CD32PADS ; //IKEY_RAWMASK;
                   // printf("key:%04x\n",finalkeycode);
                    if(imcode & IECODE_UP_PREFIX)
                    {

                    }
                    else
                    {
                        if( finalkeycode == 0x45 ) iquit = 1;
                    }
                }
                break;
                case IDCMP_CLOSEWINDOW:
                    iquit=1;
                break;
               // case IDCMP_NEWSIZE:
                case IDCMP_REFRESHWINDOW:

                printf("IDCMP_REFRESHWINDOW %d %d \n");
                ScalePixelArray((APTR)bm_argb->_RGB,
                        bm_argb->_width,bm_argb->_height,
                        bm_argb->_width*4,
                                    w->RPort,
                                    0,0, // dest x, y
                                    w->GZZWidth,
                                    w->GZZHeight,
                                    RECTFMT_ARGB
                                    );

                    break;
                default:
                break;
            }


        } // end while message




    }

    return 0;
}
