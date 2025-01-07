#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>

#include <graphics/layers.h>
#include <graphics/rastport.h>
#include <graphics/gels.h>
#include <stdio.h>
#include <stdlib.h>

#include "datatypebm.h"


struct Window *w=NULL;
struct BitMap *bm=NULL;

int init()
{

    int res = LoadDataTypeToBm("chunli.gif",&bm,NULL);
    printf("loadbm: %d BM: %08x\n",res,(int)bm);

    // init gels
    struct GelsInfo gelsi={};
    struct Bob  b;

    InitGels(NULL,NULL,&gelsi);


    if(!w)
    {
    int wwidth = 128;
    int wheight = 96;

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
//    printf("openWindow:_machineWidth:%d _machineHeight:%d xcen:%d ycen:%d \n",_machineWidth,_machineHeight,xcen,ycen);

    w = (struct Window *)OpenWindowTags(NULL,
        WA_Left,xcen,
        WA_Top,ycen,
     //   WA_Width, _machineWidth,
     //   WA_Height, _machineHeight,
        WA_InnerWidth, wwidth,
        WA_InnerHeight, wheight,
//        WA_MaxWidth,  wwidth,
//        WA_MaxHeight, wheight,
//        WA_MinWidth, _machineWidth,
//        WA_MinHeight, _machineHeight,
        WA_RptQueue,0, // no rawkey repeat messages
        WA_IDCMP,/* IDCMP_GADGETUP | IDCMP_GADGETDOWN |*/
            IDCMP_MOUSEBUTTONS |  IDCMP_RAWKEY | IDCMP_CHANGEWINDOW | IDCMP_REFRESHWINDOW |
            IDCMP_NEWSIZE /*| IDCMP_INTUITICKS*/ | IDCMP_CLOSEWINDOW,

        WA_Flags, /*WFLG_SIZEGADGET*/ /*| WFLG_SIZEBRIGHT | WFLG_SIZEBBOTTOM |

            */ WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_ACTIVATE /*|
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
    if(bm)
    {
        FreeBitMap(bm);
        bm=NULL;
    }
}

int main(int argc, char **argv)
{
    atexit(exitclose);
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
                case IDCMP_REFRESHWINDOW:

                    break;
                default:
                break;
            }


        } // end while message




    }

    return 0;
}
