#include "amiga_retroarch_audio.h"

// amiga
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/alib.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/ahi.h>
#include <proto/utility.h>


    #include "exec/types.h"
    #include <dos/dos.h>
    #include <dos/dostags.h>
    #include <exec/memory.h>
    #include <dos/dosextens.h>
    #include <dos/dostags.h>

#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

#define INPUTFORMAT_16B 1

// MAME106 would have INT32 has internal format
#ifdef INPUTFORMAT_16B
typedef signed short ismpl_t;
#else
typedef signed int ismpl_t;
#endif

//  globals
struct Library *AHIBase=NULL;

// private, describe a sound buffer to write
// used by the mixer between caller and AHI.
struct sSoundToWrite
{	// write: stereo Writing on leftright buffer (signed short*2)table:
	WORD 	    *m_pBuffer;
	WORD 	    *m_pPrevBuffer; // previous buffer, for tricks.
	// write: AHI Volume multiplier. should be 0x00010000; or do not touch.
	//ULONG 	    m_Volume;
	// read: the amount of data to write in pBuffer. *2 for stereo.
	ULONG m_nbSampleToFill;
	// read: play frequency (22050,44100,...) should be the one given with AHIS_Init()
	ULONG	m_PlayFrequency;
	ULONG         m_stereo;
	// Total Amount of sample played from the begining.
	// seconds should be found with:  m_TotalSampleDone/m_PlayFrequency.
	//unsigned long long	m_TotalSampleDone;
};

// round buffer shared between threads.
struct SampleFrame
{
    ismpl_t *_mix; // left/rightb interlaced if stereo.
    // mix thread set read state, main caller process set write state.
    ULONG _read; // amount of sample currently read in the frame.
    ULONG _written;  // amount of sample currently written in the frame.
    USHORT _readlock;
    USHORT _writelock;

};


typedef enum {
	eAHIS_ok=0,
	eAHIS_NotInited, // on purpose.
	eAHIS_Init, // special private value
	eAHIS_DeviceError,
	eAHIS_NotEnoughMemory,
	eAHIS_ThreadError,
   eAHIS_StreamEnd,
	eAHIS_NumberOfError // used to extend the error list by other libs.
} eAHIError;

#define nbSampleFrame 8
#define nbSampleFrameMask 7
// https://sintonen.fi/src/ipc/ipctest.c

// the actual object returned by amiga_audio_init(),
// also shared between process, so it's MEMF_PUBLIC.
struct amiga_audio_internal
{
    // - - - threads thing
    struct Process *m_hMainProcess;
    struct Process *m_hThread;
    struct MsgPort *m_MainProcessReplyPort; // could be transient, but factorise this.
    // - - - - - messages
    // main to thread:
    char m_useAHI;
    char m_askedtoplay; // 1=play, 0 stop
    char m_askThreadEnd;
    // thread to main:
    char m_isplaying;
    // - - - - - audio options
    // main to thread
    ULONG m_freq; // in hz, likely 22050...
    ULONG m_sampleUpdateLength; // computed from freq, then rounded.
    ULONG m_mixdivcte;
    ULONG m_stereo;
    // - - - - AHI
	struct MsgPort 	*m_AHImp;
	struct AHIRequest  *m_AHIio;
	struct AHIRequest  *m_AHIio2;
	struct AHIRequest  *m_join;
    int m_ahi_error;
    // - - - - Paula

    // - - - - sound buffers
    // buffers of audio thread, passed to hardware or AHI
    // is SHORT* or BYTE *
	SHORT *m_pSBuffAlloc,*m_pSBuff1, *m_pSBuff2;

    // round buffers that are written by caller process,
    // and read and passed to hardware by thread.
    ULONG   m_iFrame_written;
    ULONG   m_iFrame_read;
    struct SampleFrame m_SampleFrames[8];
};

// this struct may be actually copy-passed to thread.
struct threadParamMsg
{
	struct Message msg; // we're also a message.
    struct amiga_audio_internal *p;
};

// - - - - - - -- - - - - - - - - - -- - -
#ifdef ARA_DEBUGTRACE
int nbmirror=0;
int nbexact = 0;
int nbfar=0;
#endif
// return how much done.
static inline ULONG soundMixOnThread16b(struct amiga_audio_internal *p ,struct sSoundToWrite *pSoundToWrite)
{
    WORD *ps = pSoundToWrite->m_pBuffer;
    int icurrent;
    WORD nToStillDo;

    /*
        MAMEMinimix has something here to watch if emulator cycles are running
        since the last position, to detect "forced pause" and freeze states like wb left menu
        or moving window, and pause the sound in that case.
        You won't receive a message or interupt in those cases !
    */
    // note: mame frames and AHI buffers are meant to have the exact same size.
    /*
     *     cycles_t delta = (osd_cycles()-lastSoundFrameUpdate);
    if(delta>(1000000LL>>2)) // 0.25 sec
    {
        (buffer cleaning)
        return pSoundToWrite->m_nbSampleToFill;
    }
        idea: if too much continuous mirroring cases (0.5s) , fall in sort of pause mode.
    */


    // - - - continuity at all cost. - -  - -
    // emulated frames round buffers are now 8
    nToStillDo = (WORD) pSoundToWrite->m_nbSampleToFill;
    // last written available is m_iFrame_written-1
    LONG iframewritten = (LONG) p->m_iFrame_written; // consider state frozen now.
    LONG framedelta = iframewritten - (LONG)p->m_iFrame_read;

    if(framedelta<=0 )
    {
        // too slow, mirror technique
#ifdef ARA_DEBUGTRACE
    nbmirror++;
#endif
        // just mirror alternate buffer if late.
        UWORD lh = ((UWORD) pSoundToWrite->m_nbSampleToFill)>>1;
        // point end of prev buffer

       if(pSoundToWrite->m_stereo)
       {
           WORD *pr = pSoundToWrite->m_pPrevBuffer+(pSoundToWrite->m_nbSampleToFill*2);
           LONG *psl = (LONG *)ps; // copy 4 bytes
           LONG *prl = (LONG *)pr;
           for(UWORD i=0; i<lh ; i++ )
           {
               *psl++ = *--prl;
           }
           for(UWORD i=0; i<lh ; i++ )
           {
               *psl++ = *prl++;
           }
       } else
       {
           WORD *pr = pSoundToWrite->m_pPrevBuffer+(pSoundToWrite->m_nbSampleToFill);
           // mono
           for(UWORD i=0; i<lh ; i++ )
           {
               *ps++ = *--pr;
           }
           for(UWORD i=0; i<lh ; i++ )
           {
               *ps++ = *pr++;
           }
       }
        return pSoundToWrite->m_nbSampleToFill;
    }
    if(framedelta <= 3) // 2 available,but in that case treat one. may sync.
    {
#ifdef ARA_DEBUGTRACE
nbexact++;
#endif
        // ideal, same speed
        struct SampleFrame *pFrame =  &p->m_SampleFrames[(p->m_iFrame_read)&nbSampleFrameMask];

        const ismpl_t *mix = pFrame->_mix;

        WORD ntodo2 = nToStillDo;
        if(pSoundToWrite->m_stereo) ntodo2*=2;

        for(WORD i=0; i<ntodo2 ; i++ )
        {
            *ps++ = *mix++;
        }
        pFrame->_written = 0;

        p->m_iFrame_read++;  // +1 in that case
        return pSoundToWrite->m_nbSampleToFill;
    }
    //
#ifdef ARA_DEBUGTRACE
nbfar++;
#endif
    // framedelta>1 sound written too fast, mix 2 frames

    // mix from p->m_iFrame_read +1 to iwritten.
    struct SampleFrame *pFrame_a =  &p->m_SampleFrames[(p->m_iFrame_read)&nbSampleFrameMask];
    struct SampleFrame *pFrame_b =  &p->m_SampleFrames[(iframewritten-1)&nbSampleFrameMask];
    const ismpl_t *mixa = pFrame_a->_mix;
    const ismpl_t *mixb = pFrame_b->_mix;
    // first quarter as a
    {
        WORD ntodo2 = nToStillDo>>2;
        if(pSoundToWrite->m_stereo) ntodo2*=2;
        mixb += ntodo2;
        for(WORD i=0; i<ntodo2 ; i++ )
        {
            *ps++ = *mixa++;
        }
    }
    // middle  mix ->b
    {
        WORD ntodo2 = nToStillDo>>1;
        ULONG addx= p->m_mixdivcte; //0x10000/ntodo2; // totally constant.
        ULONG w=0;
        if(pSoundToWrite->m_stereo)
        {
            for(WORD i=0; i<ntodo2 ; i++ )
            {
                LONG w2 = w>>8;
                LONG iw = 0x100 - w2;
                *ps++ = ((*mixa++ * iw) +(*mixb++ * w2))>>8;
                *ps++ = ((*mixa++ * iw) +(*mixb++ * w2))>>8;
                w += addx;
            }
        } else
        { // mono
            for(WORD i=0; i<ntodo2 ; i++ )
            {
                LONG w2 = w>>8;
                LONG iw = 0x100 - w2;
                *ps++ = ((*mixa++ * iw) +(*mixb++ * w2))>>8;
                w += addx;
            }

        }

    } // end middle mix
    // last quarter as b
    {
        WORD ntodo2 = nToStillDo>>2;
        if(pSoundToWrite->m_stereo) ntodo2*=2;

        for(WORD i=0; i<ntodo2 ; i++ )
        {
            *ps++ = *mixb++;
        }
    }
// - - - - -
    p->m_iFrame_read = iframewritten; // +more than1

    return pSoundToWrite->m_nbSampleToFill;
}


static void AudioThread_AHI_Close(struct amiga_audio_internal *p);
static void AudioThread_AHI_Init(struct amiga_audio_internal *p)
{
    if(p->m_isplaying) return;
    p->m_isplaying = 0;
    BYTE deviceResult;
    p->m_AHImp = CreatePort(NULL,0);
    p->m_AHIio = (struct AHIRequest*)CreateExtIO(p->m_AHImp ,sizeof(struct AHIRequest));

    if (p->m_AHIio) {
        p->m_AHIio->ahir_Version = 4;
        deviceResult =
            OpenDevice("ahi.device",AHI_DEFAULT_UNIT, (struct IORequest *)(p->m_AHIio), 0);
    }
    if (deviceResult) {
        p->m_ahi_error = eAHIS_DeviceError;
        AudioThread_AHI_Close(p);
        return;
    }

    AHIBase = (struct Library *) p->m_AHIio->ahir_Std.io_Device;
    p->m_AHIio2 = (struct AHIRequest *)CreateExtIO(p->m_AHImp ,sizeof(struct AHIRequest));

    if(!p->m_AHIio2) {
        p->m_ahi_error = eAHIS_DeviceError;
        AudioThread_AHI_Close(p);
        return;
    }
    //CopyMem(p->m_AHIio, p->m_AHIio2, sizeof(struct AHIRequest));
    memcpy(p->m_AHIio2,p->m_AHIio,sizeof(struct AHIRequest));

    int stereosizemult = (p->m_stereo)?2:1;

    // then again  * 2 for double buffer
    p->m_pSBuffAlloc = (SHORT*) AllocVec(p->m_sampleUpdateLength*sizeof(SHORT)*2*stereosizemult, MEMF_PUBLIC|MEMF_CLEAR);
    if (!p->m_pSBuffAlloc) {
        p->m_ahi_error = eAHIS_NotEnoughMemory;
        AudioThread_AHI_Close(p);
        return;
    }

    p->m_pSBuff1 = p->m_pSBuffAlloc;
    p->m_pSBuff2 = p->m_pSBuffAlloc + (p->m_sampleUpdateLength*stereosizemult);

    p->m_ahi_error = eAHIS_ok;
    p->m_isplaying = 1;
    return;
}
static void AudioThread_AHI_Loop(struct amiga_audio_internal *p)
{
    if(!p->m_isplaying) return;

    ULONG streamBytes = p->m_sampleUpdateLength<<1; // *sizeof(SHORT);
    if(p->m_stereo) streamBytes<<=1;

	{ // paragraph for thread loop & data
        // prepare struct which is passed to write func:
        struct sSoundToWrite soundToWrite;
        soundToWrite.m_nbSampleToFill = p->m_sampleUpdateLength; // always
        soundToWrite.m_PlayFrequency = p->m_freq; // always
        soundToWrite.m_stereo =  p->m_stereo;

        // loop still something ask to stop.
        p->m_join = NULL; // retain the last one to tell next request we continue this one.
      //  ULONG iloop=0;
        while(p->m_askedtoplay)
        {
            ULONG numSampleWritten;
            SHORT *p1 = p->m_pSBuff1;
            soundToWrite.m_pBuffer = p1;
            soundToWrite.m_pPrevBuffer = p->m_pSBuff2; // for tricks.

            // write the signal:
            // if(iloop<2)
            // { // would clean the buffers at start. no data ready anyway.
            //    numSampleWritten = soundToWrite.m_nbSampleToFill;
            //    memset(p1,0,streamBytes);
            //    iloop++;
            // } else
            // {
             //numSampleWritten = soundToWrite.m_nbSampleToFill;
                numSampleWritten = soundMixOnThread16b( p,&soundToWrite );
//            }

            {
                struct AHIRequest  *AHIio = p->m_AHIio;
                AHIio->ahir_Std.io_Message.mn_Node.ln_Pri = 10; //64 //127?
                AHIio->ahir_Std.io_Command = CMD_WRITE;
                AHIio->ahir_Std.io_Data = p1;
                AHIio->ahir_Std.io_Offset = 0;
                AHIio->ahir_Version = 4;
                AHIio->ahir_Frequency = p->m_freq;
                if(p->m_stereo)
                {
                   AHIio->ahir_Type = AHIST_S16S;
                   AHIio->ahir_Std.io_Length = numSampleWritten<<2; // number of bytes transfered.
                } else
                {   // mono
                   AHIio->ahir_Type = AHIST_M16S;
                   AHIio->ahir_Std.io_Length = numSampleWritten<<1;
                }
                //Workout mode to set
                AHIio->ahir_Volume = 0x010000; // max volume
                AHIio->ahir_Position = 0x8000; // stereo position to the middle, means 0.5.
                AHIio->ahir_Link = p->m_join;

                SendIO((struct IORequest *)AHIio);

                if (p->m_join) {
                    // this is where the process is waiting when playing.
                    // it's waiting to join the next buffer.
                    // it must always happens at the right moment.
                    // and that's why there is a thread.
                    WaitIO((struct IORequest *)(p->m_join));
                    }

                p->m_join = AHIio;

                // switch double buffer:
                p->m_AHIio = p->m_AHIio2;
                p->m_AHIio2 = AHIio;

                p->m_pSBuff1 = p->m_pSBuff2;
                p->m_pSBuff2 = p1;

            } // end of io paragraph
        } // end of life loop
    } // def apragraph

}
static void AudioThread_AHI_Close(struct amiga_audio_internal *p)
{
    p->m_isplaying = 0;
//    //printf("AudioThread_AHI_Close\n");

   	// close ahi
	if(p->m_join) {

       if (!CheckIO((struct IORequest *)(p->m_join))) {
              AbortIO((struct IORequest *)(p->m_join));
          }
		WaitIO((struct IORequest *)(p->m_join));
	}
   //printf("AudioThread_AHI_Close 2\n");
	if(p->m_AHIio){
		CloseDevice((struct IORequest *)(p->m_AHIio));
		DeleteExtIO((struct IORequest *)(p->m_AHIio));
       p->m_AHIio= NULL;
		}
   //printf("AudioThread_AHI_Close 3\n");
	if(p->m_AHIio2){
       DeleteExtIO((struct IORequest *)(p->m_AHIio2));
		p->m_AHIio2= NULL;
		}
	if(p->m_AHImp){
       DeletePort(p->m_AHImp);
		p->m_AHImp= NULL;
		}
   //printf("AudioThread_AHI_Close 4\n");
   // then close buffers
	if(p->m_pSBuffAlloc){ FreeVec(p->m_pSBuffAlloc); p->m_pSBuffAlloc=NULL; }

}
static void AudioThread_Paula_Init(struct amiga_audio_internal *p)
{
    //TODO
    return;
}
static void AudioThread_Paula_Loop(struct amiga_audio_internal *p)
{
    //TODO

}
static void AudioThread_Paula_Close(struct amiga_audio_internal *p)
{
    //TODO

}

struct AudioDriver{
    // start resource
    void (*init)(struct amiga_audio_internal *p);
    // manage audio double buffer, must quit as soon as possible when p->m_askedtoplay==0
    void (*loop)(struct amiga_audio_internal *p);
    // free resource
    void (*close)(struct amiga_audio_internal *p);
};

static void AudioThread(void)
{
    struct amiga_audio_internal *p;
    struct threadParamMsg *msg;
    struct Process *pThread = (struct Process *)FindTask(NULL);

	WaitPort(&pThread->pr_MsgPort);
	msg = (struct threadParamMsg *) GetMsg(&pThread->pr_MsgPort);
    p = msg->p;
    ReplyMsg((APTR) msg);
#ifdef ARA_DEBUGTRACE
 printf("Hello, it's audio thread %d samplepartlength:%d\n",p->m_freq,p->m_sampleUpdateLength);
#endif
    struct AudioDriver ad;
    if(p->m_useAHI) {
        ad.init = &AudioThread_AHI_Init;
        ad.loop = &AudioThread_AHI_Loop;
        ad.close = &AudioThread_AHI_Close;
    } else {
        ad.init = &AudioThread_Paula_Init;
        ad.loop = &AudioThread_Paula_Loop;
        ad.close = &AudioThread_Paula_Close;
    }

    while(1) // do a loop for each start/stop.
    {
        // receiving message just means state of m_play or m_askThreadEnd changed.
        // else thread takes 0 cpu.
        WaitPort(&pThread->pr_MsgPort);
        msg = (struct threadParamMsg *) GetMsg(&pThread->pr_MsgPort);
        if(p->m_askThreadEnd)
        {
            ReplyMsg((APTR) msg); // asap
            return;
        }

        if(!p->m_askedtoplay)
        {
            ReplyMsg((APTR) msg);
            continue; // stop state return to wait.
        }
        // we're here if we were asked to play.
        // reply msg only after init result,

        ad.init(p);
#ifdef ARA_DEBUGTRACE
 printf("After init p->m_isplaying:%d\n",p->m_isplaying);
#endif
        ReplyMsg((APTR) msg);
        if(p->m_isplaying)
        {
            ad.loop(p);
            ad.close(p);
        }

    }

}

// - - - - - - retroarch api


void *amiga_audio_init(const char *device,
	unsigned rate, unsigned latency,
	unsigned block_frames, unsigned *new_rate)
{

    struct amiga_audio_internal *p = AllocVec(sizeof(struct amiga_audio_internal),MEMF_CLEAR | MEMF_PUBLIC);
    if(!p) return NULL;

    p->m_hMainProcess = (struct Process *)FindTask(NULL);

    //
    p->m_useAHI = (device && strcmp(device,"ahi.device")==0);
    p->m_useAHI = 1; // for the moment that's it.

    if(rate>22050) rate=22050;
    p->m_freq = rate;
    if(new_rate) *new_rate = rate;
    p->m_stereo = 1;

    // would mean sound thread will update at 30Hz
    p->m_sampleUpdateLength = ((p->m_freq/30)+3)& 0xfffffffc; // also need 4-aligned for some reason.
    // AHI crash if too short it seems (do not go lower than 11khz)
    if(p->m_sampleUpdateLength<256) p->m_sampleUpdateLength=256;

    p->m_mixdivcte = 0x10000/(p->m_sampleUpdateLength>>1);

#ifdef ARA_DEBUGTRACE
 printf("p->m_sampleUpdateLength:%d\n",p->m_sampleUpdateLength);
#endif
    // - - - alloc round buffers, shared by both process
    {
        int i;
        // KRB note: this specific amiga implemtation assume frames
        // are fixed length, so alloc size is samples_this_frame.
        ULONG frameBytelength =  p->m_sampleUpdateLength * sizeof(ismpl_t);
        ULONG bigsize = frameBytelength*nbSampleFrame;
        ismpl_t *pMixmem;
        if(p->m_stereo) bigsize*=2;
        pMixmem = (ismpl_t *)AllocVec(bigsize,MEMF_CLEAR | MEMF_PUBLIC);
        if(!pMixmem) {
            FreeVec(p);
            return NULL;
        }
        for(i=0;i<nbSampleFrame;i++)
        {
            p->m_SampleFrames[i]._mix = pMixmem;
            pMixmem += (p->m_stereo)?(p->m_sampleUpdateLength*2):(p->m_sampleUpdateLength) ;

        }
    }
#ifdef ARA_DEBUGTRACE
 printf("create process\n");
#endif
    // - - - - - create thread the os3 way and pass params - - - - - -
    p->m_MainProcessReplyPort = CreateMsgPort(); // factorise that.

	{
		struct TagItem threadTags[] = { NP_Entry,(ULONG) &AudioThread,
                        NP_Priority,    60  , // int8; -128 .. 127 DO NOT HOG !!
                        NP_Name,(ULONG)"Audio",
                        NP_Output,(ULONG) p->m_hMainProcess->pr_COS, // allow to print on same console from thread !
                        NP_CloseOutput,FALSE,
                        NP_FreeSeglist, FALSE,
		 				TAG_DONE,0  };
        p->m_hThread = CreateNewProc( threadTags  );
	}
	if(!p->m_hThread ){
    	amiga_audio_free(p);
        return NULL;
	}
	{
    	struct threadParamMsg tpm;
#ifdef ARA_DEBUGTRACE
 printf("send init params\n");
#endif
        tpm.msg.mn_ReplyPort = p->m_MainProcessReplyPort;
        tpm.msg.mn_Length    = sizeof(struct threadParamMsg);
        tpm.p = p;

        PutMsg(&p->m_hThread->pr_MsgPort, &tpm.msg);
        WaitPort(tpm.msg.mn_ReplyPort);
		(void) GetMsg(tpm.msg.mn_ReplyPort);
#ifdef ARA_DEBUGTRACE
 printf("after send init params\n");
#endif
    }

    return (void *)p;
}

/* note this is to be streamed by frame.
if we were sure "len" is always same size as m_sampleUpdateLength
it would be easy.
We have to take account of the case it uses half a sampleFrame.
or parts of 2 consecutive sampleFrames.
*/
// Assume 16 Bit Samples, stereo
size_t amiga_audio_write(void *data, const void *s, size_t len)
{    
    size_t sdone=0;
    const ismpl_t *pread = (const ismpl_t *)s;
    struct amiga_audio_internal *p = ( struct amiga_audio_internal *)data;
    struct SampleFrame *pFrame;

    if(!p || !pread || len==0) return 0;
    if(!p->m_isplaying) return 0;

    pFrame = &p->m_SampleFrames[p->m_iFrame_written & nbSampleFrameMask];
    while(len>0)
    {
        ULONG appliedlen;

        appliedlen = p->m_sampleUpdateLength - pFrame->_written;
        if(len<appliedlen) appliedlen = len;
        if(p->m_stereo)
        {
            int i;
            ismpl_t *pmix = pFrame->_mix + (pFrame->_written*2);
            // would memcpy...
            for(i=0;i<appliedlen;i++)
            {
                *pmix++ = *pread++;
                *pmix++ = *pread++;
            }

        } else
        {
            // mono
            int i;
            ismpl_t *pmix = pFrame->_mix + (pFrame->_written);
            // would memcpy...
            for(i=0;i<appliedlen;i++) *pmix++ = *pread++;
        }
        pFrame->_written += appliedlen;
        len -= appliedlen;
        sdone += appliedlen;

        if(pFrame->_written == p->m_sampleUpdateLength)
        {
            pFrame->_read = 0;
            pFrame->_writelock =0;

            p->m_iFrame_written++;           
            pFrame = &p->m_SampleFrames[p->m_iFrame_written & nbSampleFrameMask];

            pFrame->_writelock =1;
            pFrame->_written = 0;

        }

    } // end while len>0

#ifdef ARA_DEBUGTRACE
static int ifr=0;
ifr++;
if(ifr==50){
    ifr=0;
    printf("nbmirrored:%d nbexact:%d nbjumps:%d\n",nbmirror,nbexact,nbfar);
    nbmirror=0;
    nbexact=0;
    nbfar=0;

}
#endif
    return sdone;

}
// send message and wait result.
static inline void sendThreadMessage(struct amiga_audio_internal *p, char askToPlay, char askEnd )
{
    struct threadParamMsg tpm;

    if(!p->m_hThread) return;

    tpm.msg.mn_ReplyPort = p->m_MainProcessReplyPort;
    tpm.msg.mn_Length    = sizeof(struct threadParamMsg);
    tpm.p = p;
    p->m_askedtoplay = askToPlay; // when this is set to 0, play loop will quit within 1 frame.
    p->m_askThreadEnd = askEnd;
    PutMsg(&p->m_hThread->pr_MsgPort, &tpm.msg);
    WaitPort(tpm.msg.mn_ReplyPort);
    (void) GetMsg(tpm.msg.mn_ReplyPort); // flush.

}

// Stops replay
bool amiga_audio_stop(void *data)
{
    struct amiga_audio_internal *p = (struct amiga_audio_internal *)data;
    if(!p) return 0;

    if(!p->m_isplaying) return 0;


    sendThreadMessage(p,0,0); // m_askedtoplay =0, makes loop quit and go in wait message mode.

    return 0;
}

// starts replay
bool amiga_audio_start(void *data, bool is_shutdown)
{
    struct SampleFrame *pFrame;
    struct amiga_audio_internal *p = (struct amiga_audio_internal *)data;
    if(!p) return 0;

    if(p->m_askedtoplay) return 1; // doing 2 consecutive start()  would actually freeze WaitPort().

    p->m_iFrame_written = 0;
    p->m_iFrame_read = 0;
    pFrame = &p->m_SampleFrames[p->m_iFrame_written];
    pFrame->_written = 0;
    pFrame->_writelock =1;

    sendThreadMessage(p,1,0); // m_askedtoplay =1

    return (int)p->m_isplaying;
}

void amiga_audio_free(void *data)
{
    struct amiga_audio_internal *p = (struct amiga_audio_internal *)data;
    if(!p) return;

    sendThreadMessage(p,0,1); //   m_askedtoplay =0, m_askThreadEnd =1 -> quit loop, get end message.
    // at this point thread process should have died.

    if(p->m_MainProcessReplyPort) DeleteMsgPort(p->m_MainProcessReplyPort);

    // free all mem for round buffer
    if(p->m_SampleFrames[0]._mix) FreeVec(p->m_SampleFrames[0]._mix);

    FreeVec(p);
#ifdef ARA_DEBUGTRACE
    printf("final post delete, thread is dead, AHI is closed, memory is freed.\n");
#endif
}

