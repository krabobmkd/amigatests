#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>

#include <graphics/layers.h>
#include <graphics/rastport.h>
#include <graphics/gels.h>
#include <stdio.h>
#include <stdlib.h>
#include <graphics/gels.h>
#include "datatypebm8b.h"
#include <hardware/intbits.h>
struct Screen *pLockedScreen=NULL;
struct Window *w=NULL;
//struct BitMap *bm=NULL;
//PLANEPTR bm_mask=NULL;

DtBm DtBitmap;
PLANEPTR BitmapMask=NULL;


// - - - use a vertical blank interuption each 50 or 60Hz to manage some animation.
struct Task				*myTask=NULL;
//int __interrupt __saveds VBlankInterface( void );

int VBlankInterface();
struct Interrupt VertBlank =
{	NULL,NULL,NT_INTERRUPT,-60,"DPF VBlank",		/* node, pri = -60 */
	NULL,										/* data ptr, same as inputevent */
	(void *)VBlankInterface						/* code ptr */
};
int vblank_ok=0;


/* Create a Bob from the information given in nBob.  Use freeBob() to free this GEL.
** A VSprite is created for this bob.  This routine properly allocates all double
** buffered information if it is required.
*/
//struct Bob *makeBob(NEWBOB *nBob)
//{
//    struct Bob         *bob;
//    struct VSprite     *vsprite;
//    NEWVSPRITE          nVSprite ;
//    LONG                rassize;

//rassize = (LONG)sizeof(UWORD) * nBob->nb_WordWidth * nBob->nb_LineHeight * nBob->nb_RasDepth;

//if (NULL != (bob = (struct Bob *)AllocMem((LONG)sizeof(struct Bob), MEMF_CLEAR)))
//        {
//        if (NULL != (bob->SaveBuffer = (WORD *)AllocMem(rassize, MEMF_CHIP)))
//            {
//            nVSprite.nvs_WordWidth  = nBob->nb_WordWidth;
//            nVSprite.nvs_LineHeight = nBob->nb_LineHeight;
//            nVSprite.nvs_ImageDepth = nBob->nb_ImageDepth;
//            nVSprite.nvs_Image      = nBob->nb_Image;
//            nVSprite.nvs_X          = nBob->nb_X;
//            nVSprite.nvs_Y          = nBob->nb_Y;
//            nVSprite.nvs_ColorSet   = NULL;
//            nVSprite.nvs_Flags      = nBob->nb_BFlags;
//            /* Push the values into the NEWVSPRITE structure for use in makeVSprite(). */
//            nVSprite.nvs_MeMask     = nBob->nb_MeMask;
//            nVSprite.nvs_HitMask    = nBob->nb_HitMask;

//            if ((vsprite = makeVSprite(&nVSprite)) != NULL)
//                {
//                vsprite->PlanePick = nBob->nb_PlanePick;
//                vsprite->PlaneOnOff = nBob->nb_PlaneOnOff;
//                vsprite->VSBob   = bob;
//                bob->BobVSprite  = vsprite;
//                bob->ImageShadow = vsprite->CollMask;
//                bob->Flags       = 0;
//                bob->Before      = NULL;
//                bob->After       = NULL;
//                bob->BobComp     = NULL;

//                if (nBob->nb_DBuf)
//                    {
//                    if (NULL != (bob->DBuffer = (struct DBufPacket *)
//                            AllocMem((LONG)sizeof(struct DBufPacket), MEMF_CLEAR)))
//                        {
//                        if (NULL != (bob->DBuffer->BufBuffer = (WORD *)AllocMem(rassize, MEMF_CHIP)))
//                            return(bob);
//                        FreeMem(bob->DBuffer, (LONG)sizeof(struct DBufPacket));
//                        }
//                    }
//                else
//                    {
//                    bob->DBuffer = NULL;
//                    return(bob);
//                    }
//                freeVSprite(vsprite);
//                }
//            FreeMem(bob->SaveBuffer, rassize);
//            }
//        FreeMem(bob, (LONG)sizeof(*bob));
//        }
//return(NULL);
//}



int initWindow(struct Screen *pScreen)
{

    // init gels
//    struct GelsInfo gelsi={};
//    struct Bob  b;
//    InitGels(NULL,NULL,&gelsi);


    if(!w)
    {
        int wwidth = 128;
        int wheight = 96;

        int _widthphys = pScreen->Width;
        int _heightphys = pScreen->Height;

    // open window in center of workbench
    int xcen = (pScreen->Width - wwidth);
    int ycen = (pScreen->Height - wheight);
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
        WA_PubScreen, (ULONG)pScreen,
        TAG_DONE
        );
//    } // end if sbm ok

    if(w == NULL) return 2;

    } // end if window todo

    return 0;
}

void exitclose()
{
    if(vblank_ok) {
     RemIntServer(INTB_VERTB, &VertBlank);
    }

    if(pLockedScreen)
    {
        UnlockPubScreen(NULL,pLockedScreen);
        pLockedScreen = NULL;
    }

    if(w) {
        CloseWindow(w);
        w = NULL;
    }
    closeDataTypeBm(&DtBitmap);

}
void traceGels(struct  GelsInfo *gi)
{
    printf(" **** Gels: box left:%d right:%d top:%d bottom:%d\n",
                (int)gi->leftmost, (int)gi->rightmost,(int)gi->topmost, (int)gi->bottommost);

    struct VSprite *gel = gi->gelHead;
    while(gel)
    {
        printf("gel:\n\tDrawPath:%08x ClearPath:%08x\n",(int)gel->DrawPath,(int)gel->ClearPath);
        printf("\tWidth:%d Height:%d d:%d\n",(int)gel->Width,(int)gel->Height,(int)gel->Depth);
       printf("\tImageData:%08x\n",(int)gel->ImageData);

        gel = gel->NextVSprite;
    }
    printf("*** gels end\n");
}
// icone wb: 4,49,1

struct VSprite g_gelcaught={0};
int g_w=0,g_h=0,g_i=-1;
struct  GelsInfo *g_gi=NULL;
void getGelsSize(struct  GelsInfo *gi)
{
    struct VSprite *gel = gi->gelHead;
    int ig=0;
    while(gel)
    {
        if(gel->Width !=0 || gel->Height !=0)
        {
            if(g_w ==0 && g_h==0)
            {
                g_gelcaught = *gel;
                  g_i = ig;
            }
        }
        gel = gel->NextVSprite;
        ig++;
    }

}




int main(int argc, char **argv)
{
    atexit(exitclose);

    if (!(pLockedScreen = LockPubScreen(NULL))
            ) return 1;

    if(initWindow(pLockedScreen)) return 1;

	myTask = FindTask(NULL);
	AddIntServer(INTB_VERTB, &VertBlank);
	vblank_ok = TRUE;

    int res = LoadDataTypeToBm8b("woot.png",&DtBitmap,&BitmapMask,NULL,pLockedScreen);
    printf("loadbm: %d BM: %08x\n",res,(int)DtBitmap.bm);

    // - - - -
    int iquit=0;
    while(!iquit) // (im = (struct IntuiMessage *) GetMsg(pMsgPort)))
    {
        ULONG bitsToWait = (1 << (w->UserPort->mp_SigBit)) | SIGBREAKF_CTRL_F;
        ULONG signals = Wait(bitsToWait);

g_gi = pLockedScreen->RastPort.GelsInfo;
        if(signals & SIGBREAKF_CTRL_F)
        {
            // vertb timer
            static int itimer =0;
            itimer++;
            if(((itimer>>1) & 63) == 0)
            {
                printf("w:%d h:%d i:%d\n",g_gelcaught.,g_h,g_i);
//                struct  GelsInfo *gi = pLockedScreen->RastPort.GelsInfo;
//                if(gi)
//                {
//                    traceGels(gi);
//                }
            }
        }

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

    } // end while no quit.

    return 0;
}

int VBlankInterface( void )
{	Signal(myTask,SIGBREAKF_CTRL_F);

    if(g_gi) getGelsSize(g_gi);
	return 0; /* server chain continues		*/
}


/* tests....
 perlin noise cache
*/
ULONG seed=0x8781e35a;
UBYTE basenoise(UWORD y,UWORD x)
{

}

// 16bit UWORD domain circular

UBYTE pnoise(UWORD y,UWORD x)
{
    ULONG acc=0;
    UWORD ymask=0x8000;
    while(ymask)
    {

        ymask>>=1;
    }
}
