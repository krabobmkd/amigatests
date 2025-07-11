#include "rtgdpfs_factory.h"
#include "datatypebm8b.h"
#include <stdio.h>
#include <stdlib.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <exec/ports.h>

struct rtg_dpf_screen* DualScreen = NULL;

DtBm someBitmap;
PLANEPTR someMaskPlane=NULL;

void exitclose();

int main(int argc, char** argv)
{
	atexit(&exitclose);
	// this creates a 8color + 8 color dpf or 16 color + 16 color dpf.
	DualScreen = rtgdpfs_createBestImplementation(320+80,256+64,320+80,256+64);
	if (!DualScreen)
	{
		printf("fail to find screen\n");
		return 1;
	}


    someBitmap.bm = NULL;
    int res = LoadDataTypeToBm8b("woot.gif",&someBitmap,&someMaskPlane,DualScreen->_screen);
    printf("loadbm: %d BM: %08x\n",res,(int)someBitmap.bm);

    //copy bm
    if(someBitmap.bm)
    {
        printf("w:%d h:%d d:%d \n",(int)someBitmap.bm->BytesPerRow<<3,someBitmap.bm->Rows,(int)someBitmap.bm->Depth);

        BltMaskBitMapRastPort( someBitmap.bm, //source bm
                    0, 0, // source x,y
                    DualScreen->_pf1rp,  // dest rp
                 8, 8, // dest x,y
                 someBitmap.bm->BytesPerRow<<3,  someBitmap.bm->Rows,//size
                0x00c0, // copy minterm
                someMaskPlane //bltmask
                 );

        BltMaskBitMapRastPort( someBitmap.bm, //source bm
                    0, 0, // source x,y
                    DualScreen->_pf2rp,  // dest rp
                 30, 40, // dest x,y
                 someBitmap.bm->BytesPerRow<<3,  someBitmap.bm->Rows,//size
                0x00c0, // copy minterm
                someMaskPlane //bltmask
                 );


//        BltBitMapRastPort( someBitmap.bm,//CONST struct BitMap *srcBitMap,
//               0,0, //LONG xSrc, LONG ySrc,
//               DualScreen->_pf1rp,//struct RastPort *destRP,
//               8,8,//LONG xDest, LONG yDest,
//               someBitmap.bm->BytesPerRow<<3,  someBitmap.bm->Rows,
//               0x00c0//ULONG minterm  -> copy minterm.
//               );

//        BltBitMapRastPort( someBitmap.bm,//CONST struct BitMap *srcBitMap,
//               0,0, //LONG xSrc, LONG ySrc,
//               DualScreen->_pf2rp,//struct RastPort *destRP,
//               64,60,//LONG xDest, LONG yDest,
//               someBitmap.bm->BytesPerRow<<3,  someBitmap.bm->Rows,
//               0x00c0//ULONG minterm  -> copy minterm.
//               );

    }
    DualScreen->setscroll(DualScreen,0,0,16,16);

    // - - - -
    {
    int iquit=0;

    while(!iquit)
    {
        struct IntuiMessage *im;
        int doDraw=0;
        ULONG bitsToWait = 1 << (DualScreen->_userPort->mp_SigBit);
        Wait(bitsToWait);

        while((im = (struct IntuiMessage *) GetMsg(DualScreen->_userPort)))
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
                default:
                break;
            } // end switch msg class
        } // end while message



    } // end main loop
    }

	return 0;
}

void exitclose()
{
    closeDataTypeBm(&someBitmap);
	if(DualScreen) DualScreen->close(DualScreen);
	DualScreen = NULL;
}
