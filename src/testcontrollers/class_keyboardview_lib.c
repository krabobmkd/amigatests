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

// endif gcc6.5 header
#endif


#endif

#ifndef KEYBOARDVIEW_STATICLINK

struct ExecBase       *SysBase=NULL;
struct GfxBase        *GfxBase=NULL;
struct DosLibrary     *DOSBase=NULL;
struct IntuitionBase  *IntuitionBase=NULL;
struct Library        *UtilityBase=NULL;

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
        // not here because share the pointers from main exe.
        return TRUE;
    }
    void KeyboardView_CloseLibs(){

    }
#else
    BOOL KeyboardView_OpenLibs(void)
    {
//      ULONG *LongMem=0;     
       if(!SysBase) SysBase = *(( struct ExecBase **)4);
       if(!IntuitionBase)  IntuitionBase = OpenLibrary("intuition.library",39);
       if(!GfxBase) GfxBase = OpenLibrary("graphics.library",39);
       if(!UtilityBase) UtilityBase = OpenLibrary("utility.library",39);
       if(!DOSBase)  DOSBase = OpenLibrary("dos.library",33);

        return TRUE;
    }

    void KeyboardView_CloseLibs(void)
    {
        if(DOSBase) CloseLibrary(DOSBase);
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


int F_SAVED KeyboardView_CreateClass(REG(struct ClassLibrary *LibBase,a6))
{
  if(KeyboardView_OpenLibs())
  {

    if(KeyboardViewClassPtr=MakeClass(KeyboardViewClassID,KeyboardViewSuperClassID,0,sizeof(KeyboardView),0))
    {
//      DKP("   A4=%8lx A6=%8lx UtilityBase:&%8lx = %8lx\n",getreg(REG_A4),getreg(REG_A6),&UtilityBase,UtilityBase);
      KeyboardViewClassPtr->cl_Dispatcher.h_Data=LibBase;
      KeyboardViewClassPtr->cl_Dispatcher.h_Entry=KeyboardView_DispatcherStub;

      AddClass(KeyboardViewClassPtr);
      // also when shared .class (when static == NULL)
     if(LibBase) LibBase->cl_Class = KeyboardViewClassPtr;

      /* Success */
      return(0);
    }
    KeyboardView_CloseLibs();
  }
  /* Fail */
  return(-1);
}

//void __saveds __asm __KeyboardViewLibCleanup(register __a6 struct Library *LibBase)
void F_SAVED KeyboardView_DestroyClass( REG(struct ClassLibrary *LibBase,a6) )
{
    // note LibBase and KeyboardViewClassPtr should be the same
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
        KeyboardView_DestroyClass(NULL);
    }
}
// just use this one once
void KeyboardView_static_class_init()
{
    atexit(KeyboardView_static_class_close);
    KeyboardView_CreateClass(NULL);

}
#endif


