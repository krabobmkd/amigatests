#include "amiga_retroarch_audio.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <hardware/intbits.h>

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
WORD *buffer=NULL;

static void closemain()
{
    if(buffer) FreeVec(buffer);
    if(vblank_ok) {
     RemIntServer(INTB_VERTB, &VertBlank);
    }	
	printf("stop audio api and quit\n");
	
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
	
	printf("starts replay\n");
	amiga_audio_start(RAAudio,FALSE);

	printf("main process enter loop\n");
	size_t subsamplelength = (rate/60); // 16 align
	printf("subsamplelength:%d\n",subsamplelength);
    buffer = AllocVec(subsamplelength*4,MEMF_CLEAR);

	unsigned isample=0;
    while(1) 
    {
		// wait quit signal or timer
        ULONG signals = Wait(SIGBREAKF_CTRL_C | SIGBREAKF_CTRL_F);
		if((signals & SIGBREAKF_CTRL_C)!=0) break;
		SetSignal(0,SIGBREAKF_CTRL_F);


		// write some 440Hz sinus as audio
		 float freqdividerL = (440.0f*M_PI*2.0f)/(float)(rate);
		 float freqdividerR = (220.0f*M_PI*2.0f)/(float)(rate);
		 for(unsigned i=0;i<(subsamplelength);i++)
		 {
		 	buffer[i*2] = (WORD)(sinf(((float)(i+isample))*freqdividerL)*16000.0f);
		 	buffer[i*2+1] = (WORD)(sinf(((float)(i+isample))*freqdividerR)*25000.0f);



		 }
		 isample += subsamplelength;
		
		 size_t done = amiga_audio_write(RAAudio, (const void *)buffer,subsamplelength);
		
	}
	printf("main process out of the loop, ask stop\n");
	amiga_audio_stop(RAAudio); // Stops replay
// is_shutdown

	
	
	// go to closemain() in all cases.
    return 0;
}
