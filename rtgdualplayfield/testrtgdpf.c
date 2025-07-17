#include "rtgdpfs_factory.h"
#include "datatypebm8b.h"
#include <stdio.h>
#include <stdlib.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <exec/ports.h>
#include <hardware/intbits.h>

struct rtg_dpf_screen* DualScreen = NULL;

DtBm someBitmap;
PLANEPTR someMaskPlane=NULL;
UBYTE   *somePalette=NULL;
void exitclose();

// - - - use a vertical blank interuption each 50 or 60Hz to manage some animation.
struct Task				*myTask;
//int __interrupt __saveds VBlankInterface( void );

int VBlankInterface();
struct Interrupt VertBlank =
{	NULL,NULL,NT_INTERRUPT,-60,"DPF VBlank",		/* node, pri = -60 */
	NULL,										/* data ptr, same as inputevent */
	(void *)VBlankInterface						/* code ptr */
};
int vblank_ok=0;

int main(int argc, char** argv)
{
	myTask = FindTask(NULL);
	atexit(&exitclose);
	// this creates a 8color + 8 color dpf or 16 color + 16 color dpf.
	DualScreen = rtgdpfs_createBestImplementation(320+80,256+64,320+80,256+64);
	if (!DualScreen)
	{
		printf("fail to find screen\n");
		return 1;
	}

    someBitmap.bm = NULL;
    int res = LoadDataTypeToBm8b("woot.png",&someBitmap,&someMaskPlane,&somePalette,/*DualScreen->_screen*/NULL);
    printf("loadbm: %d BM: %08x\n",res,(int)someBitmap.bm);

    // set palette
    if(somePalette)
    {
        someBitmap.nbColors = 8;
        DualScreen->setPalette(DualScreen,somePalette,someBitmap.nbColors,0); // pf1
        DualScreen->setPalette(DualScreen,somePalette,someBitmap.nbColors,1); // pf2
    }

    //copy bm
    if(someBitmap.bm)
    {
        printf("w:%d h:%d d:%d \n",(int)someBitmap.bm->BytesPerRow<<3,someBitmap.bm->Rows,(int)someBitmap.bm->Depth);

        BltBitMapRastPort( someBitmap.bm, //source bm
                    0, 0, // source x,y
                    DualScreen->_pf1rp,  // dest rp
                 8, 8, // dest x,y
                 someBitmap.width,  someBitmap.height,//size
                (ABC|ABNC|ANBC)//, // copy with mask minterm
               // someMaskPlane //bltmask
                 );

        BltMaskBitMapRastPort( someBitmap.bm, //source bm
                    0, 0, // source x,y
                    DualScreen->_pf1rp,  // dest rp
                 8+20, 8+12, // dest x,y
                 someBitmap.width,  someBitmap.height,//size
                (ABC|ABNC|ANBC), // copy with mask minterm
                someMaskPlane //bltmask
                 );

       for(int i=0;i<16;i++)
       {
           SetAPen(DualScreen->_pf1rp,i);
           RectFill(DualScreen->_pf1rp,i*8 + 32,120,32+8+i*8,128);
       }


        BltMaskBitMapRastPort( someBitmap.bm, //source bm
                    0, 0, // source x,y
                    DualScreen->_pf2rp,  // dest rp
                 30, 40, // dest x,y
                 someBitmap.width,  someBitmap.height,//size
             (ABC|ABNC|ANBC), // copy minterm
                someMaskPlane //bltmask
                 );
       for(int i=0;i<16;i++)
       {
           SetAPen(DualScreen->_pf2rp,i);
           RectFill(DualScreen->_pf2rp,i*8 + 32,140,32+8+i*8,148);
       }

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


	AddIntServer(INTB_VERTB, &VertBlank);
	vblank_ok = TRUE;

    // - - - -
    {
    int iquit=0;

    while(!iquit)
    {
        struct IntuiMessage *im;
        int doDraw=0;
        ULONG bitsToWait = (1 << (DualScreen->_userPort->mp_SigBit)) | SIGBREAKF_CTRL_F ;
        ULONG signals = Wait(bitsToWait);

        if(signals & SIGBREAKF_CTRL_F)
        {
            // vertb timer
            static int itimer =0;
            itimer++;
            int dx =   itimer & 63;
            if(dx>32) dx = 63-dx;
            DualScreen->setscroll(DualScreen,32-dx,0,32+dx,32+dx);
        }
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
    if(vblank_ok) {
     RemIntServer(INTB_VERTB, &VertBlank);
    }
    closeDataTypeBm(&someBitmap);
	if(DualScreen) DualScreen->close(DualScreen);
	DualScreen = NULL;
}

int VBlankInterface( void )
{	Signal(myTask,SIGBREAKF_CTRL_F);
	return 0; /* server chain continues		*/
}
