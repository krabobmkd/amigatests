/**
 * This file contains lib inits that create the class
 * and would OpenLibrary() for other dependencies.
 * word XXX_STATICLINK decides if this is used as the header for a shared .class file
 * or if it is statically linked.
 */

#include <exec/alerts.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/layers.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>

#include "class_keyboardview_private.h"

typedef ULONG (*REHOOKFUNC)();

#ifdef KEYBOARDVIEW_STATICLINK
#include <stdlib.h>
#include <string.h>
#endif

#ifndef KEYBOARDVIEW_STATICLINK
struct ExecBase       *SysBase=NULL;
struct GfxBase        *GfxBase=NULL;
struct IntuitionBase  *IntuitionBase=NULL;
struct Library        *LayersBase=NULL;
struct DosLibrary     *DOSBase=NULL;
struct Library        *UtilityBase=NULL;
#endif

// this is the only global writtable we should see in the whole class binary !
struct IClass   *KeyboardViewClassPtr=NULL;
// this 2 strings are also linked to the asm startup header ( in .gadget mode)
// note: (const char *str="") would make str be a (char **) to the linker. so char str[] is linkable to asm startup
#ifndef KEYBOARDVIEW_STATICLINK
const char KeyboardViewClassID[]= KeyboardView_CLASS_ID;
#endif
const char KeyboardViewSuperClassID[]=KeyboardView_SUPERCLASS_ID;
const char *KeyboardViewVersionString = "keyboardview.gadget 1.0 "; // add date



// note: if other boopsi classes are dependences, they need to be opened here.
#ifndef KEYBOARDVIEW_STATICLINK
    BOOL KeyboardView_OpenLibs(void)
    {
      // if here, sysbase is already acquired from LibInit.
      //NO: if(!SysBase) SysBase = *(( struct ExecBase **)4);
       if(!DOSBase)  DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",1);
       if(!IntuitionBase)  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",39);
       if(!GfxBase) GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",39);
       if(!UtilityBase) UtilityBase = OpenLibrary("utility.library",39);
       if(!LayersBase) LayersBase = OpenLibrary("layers.library",39);

        return TRUE;
    }

    void KeyboardView_CloseLibs(void)
    {
        if(LayersBase) CloseLibrary(LayersBase);
        if(DOSBase) CloseLibrary((struct Library *)DOSBase);
        if(UtilityBase) CloseLibrary(UtilityBase);
        if(GfxBase) CloseLibrary((struct Library *)GfxBase);
        if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
    }

#endif

//==========================================================================================
// does not need to be exact, we just want the function pointer:
#ifdef __SASC
extern ULONG __asm __saveds KeyboardView_Dispatcher(
                    register __a0 struct IClass *C,
                    register __a2 struct Gadget *Gad,
                    register __a1 union MsgUnion *M);
#else
// GCC
extern ULONG KeyboardView_Dispatcher(
                    Class *C  __asm("a0"),
                    struct Gadget *Gad  __asm("a2"),
                    Msgs M  __asm("a1")
                    );
#endif

#ifdef __SASC
extern ULONG __asm __saveds KeyboardView_Dispatcher(
                    register __a0 struct IClass *C,
                    register __a2 struct Gadget *Gad,
                    register __a1 union MsgUnion *M);
#else
// GCC
extern ULONG KeyboardView_Dispatcher(
                    Class *C  __asm("a0"),
                    struct Gadget *Gad  __asm("a2"),
                    Msgs M  __asm("a1")
                    );
#endif

#ifndef KEYBOARDVIEW_STATICLINK
// called by shared Lib init to create class.
#ifdef __SASC
int __asm KeyboardView_CreateClass(register __a6 struct ExtClassLib *LibBase )
#else
int KeyboardView_CreateClass(struct ExtClassLib *LibBase  __asm("a6") )
#endif
{
  if(LibBase) SysBase = LibBase->cb_SysBase;
  if(KeyboardView_OpenLibs())
  {
    if(KeyboardViewClassPtr=MakeClass(KeyboardView_CLASS_ID,KeyboardViewSuperClassID,0,sizeof(KeyboardView),0))
    {
     if(LibBase) LibBase->cb_ClassLibrary.cl_Class = KeyboardViewClassPtr;
      KeyboardViewClassPtr->cl_Dispatcher.h_Data=LibBase;
      KeyboardViewClassPtr->cl_Dispatcher.h_Entry=(REHOOKFUNC)KeyboardView_Dispatcher;

      AddClass(KeyboardViewClassPtr);
      /* Success */
      return(0);
    }
    KeyboardView_CloseLibs();
  }
  /* Fail */
  return(-1);
}
// called by shared Lib expunge to dispose class.
#ifdef __SASC
void __asm KeyboardView_DestroyClass(register __a6 struct ExtClassLib *LibBase )
#else
void KeyboardView_DestroyClass(struct ExtClassLib *LibBase  __asm("a6") )
#endif
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
// end if shared class
#endif

//====================================================================================

#ifdef KEYBOARDVIEW_STATICLINK

// just use this one once when static link
int KeyboardView_static_class_init()
{ 
    if(KeyboardViewClassPtr=MakeClass(NULL,KeyboardViewSuperClassID,0,sizeof(KeyboardView),0))
    {
      KeyboardViewClassPtr->cl_Dispatcher.h_Entry=(REHOOKFUNC)KeyboardView_Dispatcher;
     // do not AddClass() when static, no need to publish, KeyboardViewClassPtr will be enough.
      /* Success */
      return(0);
    }
    return 1;
}

void KeyboardView_static_class_close()
{
    if(KeyboardViewClassPtr)
    {
      FreeClass(KeyboardViewClassPtr);
      KeyboardViewClassPtr = NULL;
    }

}

#endif


