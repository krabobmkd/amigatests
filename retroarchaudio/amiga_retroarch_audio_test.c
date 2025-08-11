#include "amiga_retroarch_audio.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <exec/interupt.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

//  use a vertical blank interuption just to have some timer.
struct Task				*myTask=NULL;
int VBlankInterface();
struct Interrupt VertBlank =
{	NULL,NULL,NT_INTERRUPT,-60,"VBlank",		/* node, pri = -60 */
	NULL,										/* data ptr, same as inputevent */
	(void *)VBlankInterface						/* code ptr */
};
int vblank_ok=0;
int VBlankInterface( void )
{	
	Signal(myTask,SIGBREAKF_CTRL_F);   
	return 0; /* server chain continues		*/
}


void *RAAudio=NULL;

static void closemain()
{
    if(vblank_ok) {
     RemIntServer(INTB_VERTB, &VertBlank);
    }	
	printf("stop audio api\n");	
	
	if(RAAudio)
	{		
		amiga_audio_free(RAAudio);
	}
	
}

int main(int argc, char **argv)
{
	atexit(&closemain);
	
	unsigned rate=22050;
	printf("init audio, ask freq:%d\n",rate);	
	RAAudio = amiga_audio_init("ahi.device",
					rate,0, // latency unused
					0, &rate); //
	if(!RAAudio) return 1;
	
	printf("init audio ok, freq is:%d\n",rate);
	
	printf("init some timer\n");
	
	myTask = FindTask(NULL);
	AddIntServer(INTB_VERTB, &VertBlank);
	vblank_ok = TRUE;
	
	printf("enter loop\n");
	size_t subsamplelength = ((rate/50)+15) & (~15); // 16 align
	unsigned isample=0;
    while(1) 
    {
		// wait quit signal or timer
        ULONG signals = Wait(SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F);
		if((signals & SIGBREAKF_CTRL_C)!=0) break;
		
		// write some 440Hz sinus as audio
		WORD temp[subsample*2];
		float freqdivider = 440.0f/(rate*M_PI);
		for(unsigned i=0;i<(subsamplelength);i++)
		{
			WORD s = (WORD)(sinf(((float)(i+isample))*freqdivider)*32767.0f);	
			temp[i*2] = s
			temp[i*2+1] = s;
		}
		isample += subsamplelength;
		
		size_t done = amiga_audio_write(RAAudio, (const void *)&temp[0],subsamplelength); 
		
	}
	amiga_audio_stop(RAAudio); // Stops replay
// is_shutdown
	amiga_audio_start(RAAudio,TRUE); // starts replay	
	
	
	// go to closemain() in all cases.
    return 0;
}
