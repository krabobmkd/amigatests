/**
 * This file contains lib inits that create the class
 * and would OpenLibrary() for other dependencies.
 * word XXX_STATICLINK decides if this is used as the header for a shared .class file
 * or if it is statically linked.
 */
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
//#include <proto/utility.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>

#include "class_keyboardview_private.h"
#include "exec/resident.h"

#ifdef KEYBOARDVIEW_STATICLINK
#include <stdlib.h>
#endif

#include "asmmacros.h"


#ifndef KEYBOARDVIEW_STATICLINK
#ifdef __GNUC__
//
// the way to lib header on gcc6.5
// when linking .class , this will be the header
int __attribute__((no_reorder)) _start()
{
        return -1;
}
extern void EndCode(void);
extern const char libName[];
extern const char libIdString[];
extern const uint32_t InitTable[];
/*
               ;STRUCTURE RT,0
     DC.W    RTC_MATCHWORD      ; UWORD RT_MATCHWORD
     DC.L    RomTag             ; APTR  RT_MATCHTAG
     DC.L    EndCode            ; APTR  RT_ENDSKIP
     DC.B    RTF_AUTOINIT       ; UBYTE RT_FLAGS
     DC.B    VERSION            ; UBYTE RT_VERSION  (defined in sample_rev.i)
     DC.B    NT_LIBRARY         ; UBYTE RT_TYPE
     DC.B    MYPRI              ; BYTE  RT_PRI
     DC.L    LibName            ; APTR  RT_NAME
     DC.L    IDString           ; APTR  RT_IDSTRING
     DC.L    InitTable          ; APTR  RT_INIT  table for InitResident()
*/
#define VERSION_KEYBOARDVIEW 1

const struct Resident RomTag __attribute__((used)) = {
    RTC_MATCHWORD, // w
    (struct Resident *)&RomTag, // l
    (APTR)&EndCode,        // hard to link, must be in same section.
    RTF_AUTOINIT,          // UBYTE RT_FLAGS
    VERSION_KEYBOARDVIEW,  // UBYTE
    1,                     // UBYTE  version
    0,                     // UBYTE  PRIORITY
    (char *)((intptr_t)&libName), // l
    (char *)((intptr_t)&libIdString), // l
    (APTR)InitTable, // l
};

const char libName[] = KeyboardView_CLASS_LIBID;
const char libIdString[] = "1.0";


//static struct VC4Base * OpenLib(REGARG(ULONG version, "d0"), REGARG(struct VC4Base *VC4Base, "a6"))
//{
//    struct ExecBase *SysBase = *(struct ExecBase **)4;
//    VC4Base->vc4_LibNode.LibBase.lib_OpenCnt++;
//    VC4Base->vc4_LibNode.LibBase.lib_Flags &= ~LIBF_DELEXP;

//   // bug("[VC] OpenLib\n");

//    return VC4Base;
//}

//static ULONG ExpungeLib(REGARG(struct VC4Base *VC4Base, "a6"))
//{
//    struct ExecBase *SysBase = VC4Base->vc4_SysBase;
//    BPTR segList = 0;

//    if (VC4Base->vc4_LibNode.LibBase.lib_OpenCnt == 0)
//    {
//        /* Free memory of mailbox request buffer */
//        FreeMem(VC4Base->vc4_RequestBase, 4*256);

//        /* Remove library from Exec's list */
//        Remove(&VC4Base->vc4_LibNode.LibBase.lib_Node);

//        /* Close all eventually opened libraries */
//        if (VC4Base->vc4_ExpansionBase != NULL)
//            CloseLibrary((struct Library *)VC4Base->vc4_ExpansionBase);
//        if (VC4Base->vc4_DOSBase != NULL)
//            CloseLibrary((struct Library *)VC4Base->vc4_DOSBase);
//        if (VC4Base->vc4_IntuitionBase != NULL)
//            CloseLibrary((struct Library *)VC4Base->vc4_IntuitionBase);

//        /* Save seglist */
//        segList = VC4Base->vc4_SegList;

//        /* Remove VC4Base itself - free the memory */
//        ULONG size = VC4Base->vc4_LibNode.LibBase.lib_NegSize + VC4Base->vc4_LibNode.LibBase.lib_PosSize;
//        FreeMem((APTR)((ULONG)VC4Base - VC4Base->vc4_LibNode.LibBase.lib_NegSize), size);
//    }
//    else
//    {
//        /* Library is still in use, set delayed expunge flag */
//        VC4Base->vc4_LibNode.LibBase.lib_Flags |= LIBF_DELEXP;
//    }

//    /* Return 0 or segList */
//    return segList;
//}

//static ULONG CloseLib(REGARG(struct VC4Base *VC4Base, "a6"))
//{
//    if (VC4Base->vc4_LibNode.LibBase.lib_OpenCnt != 0)
//        VC4Base->vc4_LibNode.LibBase.lib_OpenCnt--;

//    if (VC4Base->vc4_LibNode.LibBase.lib_OpenCnt == 0)
//    {
//        if (VC4Base->vc4_LibNode.LibBase.lib_Flags & LIBF_DELEXP)
//            return ExpungeLib(VC4Base);
//    }

//    return 0;
//}


static ULONG ExtFunc()
{
    return 0;
}

//struct VC4Base * vc4_Init(REGARG(struct VC4Base *base, "d0"), REGARG(BPTR seglist, "a0"), REGARG(struct ExecBase *SysBase, "a6"))
//{
//    struct VC4Base *VC4Base = base;
//    VC4Base->vc4_SegList = seglist;
//    VC4Base->vc4_SysBase = SysBase;
//    VC4Base->vc4_LibNode.LibBase.lib_Revision = VC4CARD_REVISION;
//    VC4Base->vc4_Enabled = -1;

//    return VC4Base;
//}

int F_SAVED KeyboardView_LibInit(REG(struct Library *LibBase,a6));

static ULONG vc4_functions[] = {
    (ULONG)OpenLib,
    (ULONG)CloseLib,
    (ULONG)ExpungeLib,

//    (ULONG)ExtFunc,
//    (ULONG)FindCard,
//    (ULONG)InitCard,
    -1
};

const ULONG InitTable[4] = {
    sizeof(struct VC4Base),
    (uint32_t)vc4_functions,
    0,
    (uint32_t)vc4_Init
};



// endif gcc6.5 header
#endif


#endif


#ifndef KEYBOARDVIEW_STATICLINK

struct ExecBase       *SysBase=NULL;
struct GfxBase        *GfxBase=NULL;
struct IntuitionBase  *IntuitionBase=NULL;
struct Library        *UtilityBase=NULL;
//struct LocaleBase     *LocaleBase;
//struct Library  *BevelBase,
//                *LabelBase,
//                *CyberGfxBase,
//                *DitherRectBase,
//                *KeymapBase;
#endif

//struct LocaleBase *LocaleBase;

//struct Libs MyLibs[]=
//{
//  (APTR *)&CyberGfxBase,  "cybergraphics.library",      39,     OLF_OPTIONAL,
//  (APTR *)&GfxBase,       "graphics.library",           39,     0,
//  (APTR *)&IntuitionBase, "intuition.library",          39,     0,
//  (APTR *)&LocaleBase,    "locale.library",             39,     0,
//  (APTR *)&UtilityBase,   "utility.library",            39,     0,
//  (APTR *)&KeymapBase,    "keymap.library",            39,     0,


//  (APTR *)&BevelBase,     "images/bevel.image",         44,     0,
//  (APTR *)&LabelBase,     "images/label.image",         44,     0,

//  (APTR *)&DitherRectBase,  "images/mlr_ordered.pattern",    1,     OLF_OPTIONAL,
//  0
//};


Class   *KeyboardViewClassPtr=NULL;
const char *KeyboardViewClassID= KeyboardView_CLASS_ID;
const char *KeyboardViewSuperClassID=KeyboardView_SUPERCLASS_ID;


// note: if other boopsi classes are dependenices, they need to be opened here.
#ifdef KEYBOARDVIEW_STATICLINK

    BOOL KeyboardView_OpenLibs(){
        return TRUE;
    }
    void KeyboardView_CloseLibs(){

    }
#else
    BOOL KeyboardView_OpenLibs(void)
    {
//      ULONG *LongMem=0;     
        SysBase = *(( struct ExecBase *)4.l);
        IntuitionBase = OpenLibrary("intuition.library",39);
        GfxBase = OpenLibrary("graphics.library",39);
        UtilityBase = OpenLibrary("utility.library",39);

        return TRUE;
    }

    void KeyboardView_CloseLibs(void)
    {
        if(UtilityBase) CloseLibrary(UtilityBase);
        if(GfxBase) CloseLibrary(GfxBase);
        if(IntuitionBase) CloseLibrary(IntuitionBase);
    }



#endif



//==========================================================================================

ULONG F_SAVED KeyboardView_Dispatcher(
                    REG(Class *C,a0), // register __a0 Class *C,
                    REG(struct Gadget *Gad,a2), // register __a2 struct Gadget *Gad,
                    REG(Msg M,a1), // register __a1 Msgs M,
                    REG(struct Library *LibBase,a6) // register __a6 struct Library *LibBase
                    );

//ULONG __asm KeyboardView_DispatcherStub(register __a0 Class *Cl, register __a2 Object *Obj, register __a1 Msg M)
ULONG F_ASM KeyboardView_DispatcherStub(REG(Class *Cl,a0),REG(Object *Obj,a2),REG(Msg M,a1))
{
  return(KeyboardView_Dispatcher(Cl,Obj,M,(struct Library *)Cl->cl_Dispatcher.h_Data));
}


int F_SAVED KeyboardView_LibInit(REG(struct Library *LibBase,a6))
{
  if(KeyboardView_OpenLibs())
  {

    if(KeyboardViewClassPtr=MakeClass(KeyboardViewClassID,KeyboardViewSuperClassID,0,sizeof(KeyboardView),0))
    {
//      DKP("   A4=%8lx A6=%8lx UtilityBase:&%8lx = %8lx\n",getreg(REG_A4),getreg(REG_A6),&UtilityBase,UtilityBase);
      KeyboardViewClassPtr->cl_Dispatcher.h_Data=LibBase;
      KeyboardViewClassPtr->cl_Dispatcher.h_Entry=KeyboardView_DispatcherStub;

      AddClass(KeyboardViewClassPtr);
      /* Success */
      return(0);
    }
    KeyboardView_CloseLibs();
  }
  /* Fail */
  return(-1);
}

//void __saveds __asm __KeyboardViewLibCleanup(register __a6 struct Library *LibBase)
void F_SAVED KeyboardView_LibCleanup( REG(struct Library *LibBase,a6) )
{
    if(KeyboardViewClassPtr)
    {
      RemoveClass(KeyboardViewClassPtr);
      FreeClass(KeyboardViewClassPtr);
      KeyboardViewClassPtr = NULL;
    }

  KeyboardView_CloseLibs();
}


//====================================================================================

#ifdef KEYBOARDVIEW_STATICLINK
void KeyboardView_static_class_close()
{
    if(KeyboardViewClassPtr)
    {
        KeyboardView_LibCleanup(NULL);
    }
}
// just use this one once
void KeyboardView_static_class_init()
{
    atexit(KeyboardView_static_class_close);
    KeyboardView_LibInit(NULL);

}
#endif


//   ; EndCode is a marker that show the end of your code.  Make sure it does not span
//   ; sections nor is before the rom tag in memory!  It is ok to put it right after the ROM
//   ; tag--that way you are always safe.  I put it here because it happens to be the "right"
//   ; thing to do, and I know that it is safe in this case.
#ifndef KEYBOARDVIEW_STATICLINK
void EndCode(void) {}
#endif
