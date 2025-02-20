#include "rtgdpfs_factory.h"
#include <stdio.h>
#include <stdlib.h>
struct rtg_dpf_screen* DualScreen = NULL;

void exitclose();

int main(int argc, char** argv)
{
	atexit(&exitcloses);
	// this creates a 8color + 8 color dpf or 16 color + 16 color dpf.
	DualScreen = rtgdpfs_createBestImplementation();
	if (!DualScreen)
	{
		printf("fail to find screen");
		return 1;
	}

   // - - - -
    {
    int iquit=0;

    while(!iquit)
    {
        struct IntuiMessage *im;
        int doDraw=0;
        ULONG bitsToWait = 1 << (DualScreen->userPort->mp_SigBit);
        Wait(bitsToWait);

        while((im = (struct IntuiMessage *) GetMsg(DualScreen->userPort)))
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

        //if(doDraw) drawScalePixelArray(w);

    } // end main loop
    }

	return 0;
}

void exitclose()
{
	if(DualScreen) DualScreen->close(DualScreen);
	DualScreen = NULL;
}
