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

//// private, describe a sound buffer to write
//// used by the mixer between caller and AHI.
//struct sSoundToWrite
//{	// write: stereo Writing on leftright buffer (signed short*2)table:
//	WORD 	    *m_pBuffer;
//	WORD 	    *m_pPrevBuffer; // previous buffer, for tricks.
//	// write: AHI Volume multiplier. should be 0x00010000; or do not touch.
//	//ULONG 	    m_Volume;
//	// read: the amount of data to write in pBuffer. *2 for stereo.
//	ULONG m_nbSampleToFill;
//	// read: play frequency (22050,44100,...) should be the one given with AHIS_Init()
//	ULONG	m_PlayFrequency;
//	ULONG         m_stereo;
//	// Total Amount of sample played from the begining.
//	// seconds should be found with:  m_TotalSampleDone/m_PlayFrequency.
//	//unsigned long long	m_TotalSampleDone;
//};


//// return how much done.
//ULONG soundMixOnThread( struct sSoundToWrite *pSoundToWrite);



//typedef enum {
//	eAHIS_ok=0,
//	eAHIS_NotInited, // on purpose.
//	eAHIS_Init, // special private value
//	eAHIS_DeviceError,
//	eAHIS_NotEnoughMemory,
//	eAHIS_ThreadError,
//    eAHIS_StreamEnd,
//	eAHIS_NumberOfError // used to extend the error list by other libs.
//} eAHIError;



//// Use  AHIS_Create()  AHIS_Delete() functions below to manage it:
//// private struct returned as handler !
//struct sAHISoundServer
//{
//	// private system terms for AHI:
//	struct MsgPort 	*m_AHImp;
//	struct AHIRequest  *m_AHIio;
//	struct AHIRequest  *m_AHIio2;
//	struct AHIRequest  *m_join;
//	SHORT *m_pSBuffAlloc,*m_pSBuff1, *m_pSBuff2;

//    unsigned int    m_nextSamples;
//    unsigned int    m_stereo; // set at init from machine.
//	unsigned long long	m_TotalSampleDone;
//	int	m_Error;
//};



//static void AHIS_Delete(struct amiga_audio_internal *p)
//{

////    printf(" AHIS_Delete:%08x\n",(int)_pThread);
//    if(!p->_pThread) return;
//    if(p->_pThread->m_Error == eAHIS_ok ) // if thread running
//    {
////    printf(" set signal\n");
//        ULONG oldSignals = SetSignal(0L, SIGF_SINGLE);        /* Use SIGF_SINGLE only after */
//        p->_pThread->m_AskThreadDeath = 1;                                   /* clearing it.               */
////    printf(" wait, oldSignals:%08x\n",oldSignals);
//        Wait(SIGF_SINGLE);     /* Only use SIGF_SINGLE for Wait()ing and */
//        SetSignal(oldSignals, oldSignals);
//    }
//	FreeVec(p->_pThread);
//    p->_pThread = NULL;
//}


//// used by thread itself
//static void AHISStaticThread_Close( struct sAHISoundServer *pAHIS )
//{
//    //printf("AHISStaticThread_Close\n");
//    int k;
//    	// close ahi
//	if(pAHIS->m_join) {

//        if (!CheckIO((struct IORequest *)(pAHIS->m_join))) {
//               AbortIO((struct IORequest *)(pAHIS->m_join));
//           }
//		WaitIO((struct IORequest *)(pAHIS->m_join));
//	}
//    //printf("AHISStaticThread_Close 2\n");
//	if(pAHIS->m_AHIio){
//		CloseDevice((struct IORequest *)(pAHIS->m_AHIio));
// 		DeleteExtIO((struct IORequest *)(pAHIS->m_AHIio));
//        pAHIS->m_AHIio= NULL;
//		}
//    //printf("AHISStaticThread_Close 3\n");
//	if(pAHIS->m_AHIio2){
//        DeleteExtIO((struct IORequest *)(pAHIS->m_AHIio2));
// 		pAHIS->m_AHIio2= NULL;
//		}
//	if(pAHIS->m_AHImp){
//        DeletePort(pAHIS->m_AHImp);
// 		pAHIS->m_AHImp= NULL;
//		}
//    //printf("AHISStaticThread_Close 4\n");
//    // then close buffers
//	if(pAHIS->m_pSBuffAlloc){ FreeVec(pAHIS->m_pSBuffAlloc); pAHIS->m_pSBuffAlloc=NULL; }
//    //printf("AHISStaticThread_Close 5\n");
//    pAHIS->m_AskThreadDeath = 0;
//	pAHIS->m_hThread = 0; // really has to be last.
//}
//static LONG AHISStaticThread_Open( struct sAHISoundServer *pAHIS )
//{
//    if(pAHIS==NULL || !mainprocess) return 1;

// //printf("ahi init\n");
//    BYTE deviceResult;
//    pAHIS->m_AHImp = CreatePort(NULL,0);
//    pAHIS->m_AHIio = (struct AHIRequest*)CreateExtIO(pAHIS->m_AHImp ,sizeof(struct AHIRequest));

//    if (pAHIS->m_AHIio) {
//        pAHIS->m_AHIio->ahir_Version = 4;
//        // AHI_NO_UNIT crash on my conf , 0 means "default", looks better

//        deviceResult =
//            OpenDevice(AHINAME,AHI_DEFAULT_UNIT, (struct IORequest *)(pAHIS->m_AHIio), 0);
//    }
//    if (deviceResult) {
//        pAHIS->m_Error = eAHIS_DeviceError;
//        AHISStaticThread_Close(pAHIS);
//        return 1;
//    }

//    AHIBase = (struct Library *) pAHIS->m_AHIio->ahir_Std.io_Device;
//    pAHIS->m_AHIio2 = (struct AHIRequest *)CreateExtIO(pAHIS->m_AHImp ,sizeof(struct AHIRequest));

//    if(!pAHIS->m_AHIio2) {
//        pAHIS->m_Error = eAHIS_DeviceError;
//        AHISStaticThread_Close(pAHIS);
//        return 1;
//    }
//    //CopyMem(pAHIS->m_AHIio, pAHIS->m_AHIio2, sizeof(struct AHIRequest));
//    memcpy(pAHIS->m_AHIio2,pAHIS->m_AHIio,sizeof(struct AHIRequest));

//    ULONG streamBytes = pAHIS->m_nextSamples<<1; // *sizeof(SHORT);
//    if(pAHIS->m_stereo) streamBytes<<=1;

//    // then again <<1 * 2 for double buffer
//    pAHIS->m_pSBuffAlloc = (SHORT*) AllocVec(streamBytes<<1, MEMF_PUBLIC|MEMF_CLEAR);
//    if (!pAHIS->m_pSBuffAlloc) {
//        pAHIS->m_Error = eAHIS_NotEnoughMemory;
//        AHISStaticThread_Close(pAHIS);
//        return 1;
//    }

//    pAHIS->m_pSBuff1 = pAHIS->m_pSBuffAlloc;
//    pAHIS->m_pSBuff2 = pAHIS->m_pSBuffAlloc + (streamBytes>>1);

//    pAHIS->m_Error = eAHIS_ok;
//    return 0;
//	//ok for ahi init.
//}

//// note: OS4 have different signature.
//static void AHISStaticThread()
//{
//    // tell caller process it's been created and running.
//    //  calling process must be waiting if CreateNewProc() suceed.
//    if(mainprocess) Signal((struct Task *)mainprocess, SIGF_SINGLE);


//    struct amiga_audio_internal *p;


//    ULONG res = 1; // default is error.
//    if(_pThread) res = AHISStaticThread_Open(_pThread);

//    // then there could be a AHI error so we quit now.
//    if(res!=0) return;


//    sAHISoundServer *pAHIS = _pThread;
//    ULONG streamBytes = pAHIS->m_nextSamples<<1; // *sizeof(SHORT);
//    if(pAHIS->m_stereo) streamBytes<<=1;

// //    printf(" TTT pAHIS->m_nextSamples:%d stereo:%d freq:%d\n",pAHIS->m_nextSamples, pAHIS->m_stereo,pAHIS->m_freq);
//	{ // paragraph for thread loop & data
//	// prepare struct which is passed to write func:
//	sSoundToWrite soundToWrite;
//	soundToWrite.m_nbSampleToFill = _pThread->m_nextSamples; // always
//	soundToWrite.m_PlayFrequency = _pThread->m_freq; // always
//	//soundToWrite.m_TotalSampleDone = 0ULL; // 64b.
//    soundToWrite.m_stereo =  _pThread->m_stereo;

//	// loop still something ask to stop.
//    _pThread->m_join = NULL; // retain the last one to tell next request we continue this one.
//    ULONG iloop=0;
//	while(_pThread->m_AskThreadDeath == 0 )
//	{
//		ULONG numSampleWritten;
//        SHORT *p1 = _pThread->m_pSBuff1;

//		soundToWrite.m_pBuffer = p1;
//		soundToWrite.m_pPrevBuffer = _pThread->m_pSBuff2; // for tricks.
//		//soundToWrite.m_Volume = 0x00020000; // 0x00010000;

//		// write the signal:
//        if(iloop<2)
//        {
//            numSampleWritten = soundToWrite.m_nbSampleToFill;
//            memset(p1,0,streamBytes);
//            iloop++;
//        } else
//        {
//            numSampleWritten = soundMixOnThread( &soundToWrite );
//        }
//   //     printf(" TTT 3 numSampleWritten:%d\n",(int)numSampleWritten);

//		// return 0 means no more sound: end of the thread.
//		if(numSampleWritten == 0)
//        {
//            _pThread->m_Error = eAHIS_StreamEnd;
//            break;
//        }
//	//no use	soundToWrite.m_TotalSampleDone +=  (long long int)numSampleWritten;
//		{
//			struct AHIRequest  *AHIio = _pThread->m_AHIio;
//			AHIio->ahir_Std.io_Message.mn_Node.ln_Pri = 127; //64 //127?
//			AHIio->ahir_Std.io_Command = CMD_WRITE;
//			AHIio->ahir_Std.io_Data = p1;
//			AHIio->ahir_Std.io_Offset = 0;
//            AHIio->ahir_Version = 4;
//			AHIio->ahir_Frequency = _pThread->m_freq;
//            if(_pThread->m_stereo)
//            {
//                AHIio->ahir_Type = AHIST_S16S;
//                AHIio->ahir_Std.io_Length = numSampleWritten<<2;
//            } else
//            {   // mono
//                AHIio->ahir_Type = AHIST_M16S;
//                AHIio->ahir_Std.io_Length = numSampleWritten<<1;
//            }
//			//Workout mode to set
//            AHIio->ahir_Volume = 0x010000; // 0x010000;
//			AHIio->ahir_Position = 0x8000; // stereo position to the middle, means 0.5.
//			AHIio->ahir_Link = _pThread->m_join;
////printf(" TTT bef sendio\n");
//			SendIO((struct IORequest *)AHIio);
////printf(" TTT aft sendio\n");
//			if (_pThread->m_join) {
//				WaitIO((struct IORequest *)(_pThread->m_join));
//				}

//    			_pThread->m_join = AHIio;
////printf(" TTT doubleb switch\n");
//			// switch double buffer:
//			_pThread->m_AHIio = _pThread->m_AHIio2;
//			_pThread->m_AHIio2 = AHIio;

//			_pThread->m_pSBuff1 = _pThread->m_pSBuff2;
//			_pThread->m_pSBuff2 = p1;

//		} // end of io paragraph
//	} // end of life loop

//	}// enmainprocessd of paragraph for thread loop & data
////    //PutStr("  thread die, close AHI\n");
////    //Flush(Output());
////	// assume thread is dead if we reach here:
//   // printf(" close AHI\n");
//	 AHISStaticThread_Close(_pThread);

//   // printf("send exit signal\n");
//    if(mainprocess)
//    {
//    	Signal((struct Task *)mainprocess, SIGF_SINGLE);
//    }
//	return 0;

//}




// - - - - - - retroarch api
// device and block_frames can be ignored, latency potentially too, new_rate should at the end contain the rate which was actually used, fixing things to 22050 also is fine
//static void *amiga_audio_init_ahi(const char *device,
//	unsigned rate, unsigned latency,
//	unsigned block_frames, unsigned *new_rate)
//{


//    return (void *)p;
//}
// https://sintonen.fi/src/ipc/ipctest.c

// the actual object returned by amiga_audio_init(),
// also shared between process, so it's MEMF_PUBLIC.
struct amiga_audio_internal
{
    struct Process *m_hMainProcess;
    struct Process *m_hThread;
    // main to thread:
    char m_useAHI;
    char m_play; // 1=play, 0 stop
    char m_askThreadEnd;
    // thread to main:
    char m_isplaying;
    // main to thread
    ULONG m_freq;

    // - - - - AHI
	struct MsgPort 	*m_AHImp;
	struct AHIRequest  *m_AHIio;
	struct AHIRequest  *m_AHIio2;
	struct AHIRequest  *m_join;
    int m_ahi_error;
    // - - - - Paula

    // - - - - buffers

//    ULONG m_AskThreadDeath;

//	// sample per seconds while playing:
//	ULONG 			m_freq;

//    struct sAHISoundServer *_pThread;
//    struct Library *AHIBase;
//    int m_ahi_error;
};

// this struct may be actually copy-passed to thread.
struct threadParamMsg
{
	struct Message msg; // we're also a message.
    struct amiga_audio_internal *p;
};

static void AudioThread_AHI_Close(truct amiga_audio_internal *p);
static void AudioThread_AHI_Init(truct amiga_audio_internal *p)
{
    BYTE deviceResult;
    p->m_AHImp = CreatePort(NULL,0);
    p->m_AHIio = (struct AHIRequest*)CreateExtIO(p->m_AHImp ,sizeof(struct AHIRequest));

    if (p->m_AHIio) {
        p->m_AHIio->ahir_Version = 4;
        deviceResult =
            OpenDevice(AHINAME,AHI_DEFAULT_UNIT, (struct IORequest *)(p->m_AHIio), 0);
    }
    if (deviceResult) {
        p->m_Error = eAHIS_DeviceError;
        AudioThread_AHI_Close(p);
        return 1;
    }

    AHIBase = (struct Library *) p->m_AHIio->ahir_Std.io_Device;
    p->m_AHIio2 = (struct AHIRequest *)CreateExtIO(p->m_AHImp ,sizeof(struct AHIRequest));

    if(!p->m_AHIio2) {
        p->m_Error = eAHIS_DeviceError;
        AudioThread_AHI_Close(p);
        return 1;
    }
    //CopyMem(p->m_AHIio, p->m_AHIio2, sizeof(struct AHIRequest));
    memcpy(p->m_AHIio2,p->m_AHIio,sizeof(struct AHIRequest));

    ULONG streamBytes = p->m_nextSamples<<1; // *sizeof(SHORT);
    if(p->m_stereo) streamBytes<<=1;

    // then again <<1 * 2 for double buffer
    p->m_pSBuffAlloc = (SHORT*) AllocVec(streamBytes<<1, MEMF_PUBLIC|MEMF_CLEAR);
    if (!p->m_pSBuffAlloc) {
        p->m_Error = eAHIS_NotEnoughMemory;
        AudioThread_AHI_Close(p);
        return 1;
    }

    p->m_pSBuff1 = p->m_pSBuffAlloc;
    p->m_pSBuff2 = p->m_pSBuffAlloc + (streamBytes>>1);

    p->m_Error = eAHIS_ok;
    return 0;

}
static void AudioThread_AHI_Loop(truct amiga_audio_internal *p)
{

}
static void AudioThread_AHI_Close(truct amiga_audio_internal *p)
{

}
static void AudioThread_Paula_Init(truct amiga_audio_internal *p)
{
    //TODO

}
static void AudioThread_Paula_Loop(truct amiga_audio_internal *p)
{
    //TODO

}
static void AudioThread_Paula_Close(truct amiga_audio_internal *p)
{
    //TODO

}
static void AudioThread(void)
{
    struct amiga_audio_internal *p;
    struct threadParamMsg *msg;
    struct Process *pThread = (struct Process *)FindTask(NULL);

	WaitPort(&pThread->pr_MsgPort);
	msg = (struct threadParamMsg *) GetMsg(&pThread->pr_MsgPort);
    p = msg.p;
    ReplyMsg((APTR) msg);

    while(1)
    {
        // receiving message just means state of m_play or m_askThreadEnd changed.
        // else thread takes 0 cpu.
        WaitPort(&pThread->pr_MsgPort);
        msg = (struct threadParamMsg *) GetMsg(&pThread->pr_MsgPort);
        char threadend = p->m_askThreadEnd;
        if(threadend)
        {
            ReplyMsg((APTR) msg); // asap
            return;
        }

        if(p->m_play)
        {
            // reply msg only after init result !
            if(p->m_useAHI) AudioThread_AHI_Init(p);
            else AudioThread_Paula_Init(p);

            ReplyMsg((APTR) msg);

            if(p->m_useAHI) AudioThread_AHI_LoopAndClose(p);
            else AudioThread_Paula_LoopAndClose(p);
        }
    }

}


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


    // - - - - - create thread the os3 way and pass params - - - - - -
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
        FreeVec(p);
        return NULL;
	}
	{
    	threadParamMsg tpm;

        tpm.msg.mn_ReplyPort = CreateMsgPort();
        tpm.msg.mn_Length    = sizeof(struct threadParamMsg);
        tpm.p = p;

        PutMsg(&p->m_hThread->pr_MsgPort, &tpm.msg);
        WaitPort(tpm.msg.mn_ReplyPort);
		(void) GetMsg(tpm.msg.mn_ReplyPort);

		DeleteMsgPort(tpm.msg.mn_ReplyPort);

    }


 //   return  amiga_audio_init_ahi(device,rate,latency,block_frames,new_rate);


//    struct amiga_audio_internal *p = AllocVec(sizeof(sAHISoundServer),MEMF_CLEAR);


    return (void *)p;
}

// Assume 16 Bit Samples
size_t amiga_audio_write(void *data, const void *s, size_t len)
{
    struct amiga_audio_internal *p = ( struct amiga_audio_internal *)data;

}
// Stops replay
int amiga_audio_stop(void *data)
{
    struct amiga_audio_internal *p = (struct amiga_audio_internal *)data;
    if(!p) return 0;

    if(!p->m_isplaying) return 0;

    {
    	threadParamMsg tpm;

        tpm.msg.mn_ReplyPort = CreateMsgPort();
        tpm.msg.mn_Length    = sizeof(struct threadParamMsg);
        tpm.p = p;
        p->m_play = 0;
        PutMsg(&p->m_hThread->pr_MsgPort, &tpm.msg);
        WaitPort(tpm.msg.mn_ReplyPort);
		(void) GetMsg(tpm.msg.mn_ReplyPort);

		DeleteMsgPort(tpm.msg.mn_ReplyPort);
    }

    return 0;// (int)p->m_isplaying;

    //return 0;
}

// starts replay
int amiga_audio_start(void *data, bool is_shutdown)
{
    struct amiga_audio_internal *p = (struct amiga_audio_internal *)data;
    if(!p) return 0;

    {
    	threadParamMsg tpm;

        tpm.msg.mn_ReplyPort = CreateMsgPort();
        tpm.msg.mn_Length    = sizeof(struct threadParamMsg);
        tpm.p = p;
        p->m_play = 1;
        PutMsg(&p->m_hThread->pr_MsgPort, &tpm.msg);
        WaitPort(tpm.msg.mn_ReplyPort);
		(void) GetMsg(tpm.msg.mn_ReplyPort);

		DeleteMsgPort(tpm.msg.mn_ReplyPort);
    }

    return (int)p->m_isplaying;
//// printf("osd_start_audio_stream\n");
//    if(_pThread) osd_stop_audio_stream();

//    if(!Machine) return 0; // driver and machine are already inited during this call

//    MameConfig::Audio &config = getMainConfig().audio();
//    if(config._mode != MameConfig::AudioMode::AHI)
//    {
//        m_ahi_error = eAHIS_NotInited;
//        return START_VALUE_FAIL;
//    }
//    amigamame_audio_forcemono = 0;
//    if(config._forceMono)
//    {
//        amigamame_audio_forcemono = 1;
//        stereo=0;
//    }

//    int freq =  config._freq; // Machine->sample_rate;
//    int machinefreq = Machine->sample_rate;
// // printf("config freq:%d machine freq:%d\n",freq,machinefreq);

//    if(machinefreq<freq) freq =machinefreq;
//    if(freq == 0)
//    {
//        m_ahi_error = eAHIS_NotInited;
//        return START_VALUE_FAIL;
//    }
//    int ifps = (int) Machine->drv->frames_per_second;

// // printf("osd_start_audio_stream ok to start thread\n");

//   // let's update sound 30 times per sec when 60hz...
//    /* beta 3 important note
//      For Beta 1 and 2 we used to ask the double frame length
//      which was wrong.
//      Amiga AHI doesn't support very short sound streams,
//        which happens with 11khz mono and 1sec/60 streams.
//        So we have to ask for the double.
//        But Mame wants to deliver sounds parts by video frame,
//        and we have to actually ask the right number.
//        Some PCM emulation will just consume their sample too fast.
//        So now we let the emulation fills 8 round buffers at possibly 60Hz.
//        and the AHI thread will consume 2 of them at 30Hz,
//        so AHI still use longer frames.
//    */
//    // sound thread will work at 30Hz:
//    ULONG updateLength = ((freq*2/ifps)+3)&0xfffffffc;
//    // AHI crash if too short it seems
//    if(updateLength<256) updateLength=256;

//    mainprocess = (struct Process *)FindTask(NULL);

//    _pThread = (sAHISoundServer *)AllocVec(sizeof(sAHISoundServer),
//                              //OS4 MEMF_SHARED|MEMF_CLEAR
//                              MEMF_PUBLIC|MEMF_CLEAR
//                              );
//    if(!_pThread)
//    {
//        m_ahi_error = eAHIS_DeviceError;
//        return START_VALUE_FAIL;
//    }

//   // printf("freq:%d samplelength:%d\n",freq,updateLength);
//	_pThread->m_Error = eAHIS_Init; // state in which the thread init the AHI device and requests.
//    _pThread->m_freq = freq;
//    _pThread->m_nextSamples = updateLength;
//    _pThread->m_stereo = stereo;
//// SetSignal(0L,SIGBREAKF_CTRL_C) & SIGBREAKF_CTRL_C
//    ULONG oldsignals = SetSignal(0L, SIGF_SINGLE);
//	{
//		struct TagItem threadTags[] = { NP_Entry,(ULONG) &AHISStaticThread,
//						//	NP_Child, TRUE, // os4 thread thing

//                        NP_Priority,    60  , // int8; -128 .. 127 DO NOT HOG !!
//                        NP_Name,(ULONG)"Audio",
//                        NP_Output,(ULONG) mainprocess->pr_COS, // allow to print on same console !
//                        NP_CloseOutput,FALSE,
//                        NP_FreeSeglist, FALSE,
//		 				TAG_DONE,0  };
//        	_pThread->m_hThread = CreateNewProc( threadTags  );
//	}
//    if(_pThread->m_hThread == NULL )
//    {
//        SetSignal(oldsignals, oldsignals);
//        //printf("createnewproc error\n");
//        FreeVec(_pThread);
//        _pThread = NULL;

//        m_ahi_error = eAHIS_DeviceError;
//        return START_VALUE_FAIL;
//	}
//// printf("wait for thread init oldsignals:%08x\n");
//	// wait for thread to finish AHI init
//    Wait(SIGF_SINGLE);
//    SetSignal(oldsignals, oldsignals); // old signals back
//// printf("after wait: err:%d\n",(int)_pThread->m_Error);
//	if(_pThread->m_Error != eAHIS_ok )
//	{
//    	m_ahi_error = _pThread->m_Error;
//		AHIS_Delete();
//		return START_VALUE_FAIL;
//	}
//// printf("sound thread ok, samples:%d\n",_pThread->m_nextSamples);
//    // must return samples to do next.

//    // important ask emulation to generate half of what the 30Hz thread
//    // consume:
//    return _pThread->m_nextSamples >>1; // * (stereo+1);


}

void amiga_audio_free(void *data)
{
    struct amiga_audio_internal *p = (struct amiga_audio_internal *)data;
    if(!p) return;

    if(p->m_isplaying) amiga_audio_stop(data);
    // wait end of thread
    {
    	threadParamMsg tpm;

        tpm.msg.mn_ReplyPort = CreateMsgPort();
        tpm.msg.mn_Length    = sizeof(struct threadParamMsg);
        tpm.p = p;
        p->m_play = 0;
        p->m_askThreadEnd = 1;
        PutMsg(&p->m_hThread->pr_MsgPort, &tpm.msg);
        WaitPort(tpm.msg.mn_ReplyPort);
		(void) GetMsg(tpm.msg.mn_ReplyPort);

		DeleteMsgPort(tpm.msg.mn_ReplyPort);
    }

    FreeVec(p);
}

