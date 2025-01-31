/**
 * This file contains lib inits that create the class
 * and would OpenLibrary() for other dependencies.
 * word XXX_STATICLINK decides if this is used as the header for a shared .class file
 * or if it is statically linked.
 */
 #define USE_DEBUG_FILE 1
#include <exec/alerts.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/utility.h>
#include <proto/layers.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#ifdef USE_DEBUG_FILE
    #include <libraries/dos.h>
#endif
#include "class_keyboardview_private.h"



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
struct IntuitionBase  *IntuitionBase=NULL;
struct Library        *LayersBase=NULL;

struct DosLibrary     *DOSBase=NULL;

// just used for callHooks
struct Library        *UtilityBase=NULL;



#endif
#ifdef USE_DEBUG_FILE
    BPTR DebugFile=NULL;
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

// this is the only global writtable we should see in the whole class binary !
struct IClass   *KeyboardViewClassPtr=NULL;
// this 2 strings are also linked to the asm startup header ( in .gadget mode)
// note: (const char *str="") would make str be a (char **) to the linker. so char str[] is linkable to asm startup
const char KeyboardViewClassID[]= KeyboardView_CLASS_ID;
const char KeyboardViewSuperClassID[]=KeyboardView_SUPERCLASS_ID;
const char *KeyboardViewVersionString = "keyboardview.gadget 1.0 "; // add date

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
      // if here, sysbase is already acquired from LibInit.
      //NO: if(!SysBase) SysBase = *(( struct ExecBase **)4);
       if(!DOSBase)  DOSBase = (struct DosLibrary *)OpenLibrary("dos.library",1);
       if(!IntuitionBase)  IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library",39);
       if(!GfxBase) GfxBase = (struct GfxBase *) OpenLibrary("graphics.library",39);
       if(!UtilityBase) UtilityBase = OpenLibrary("utility.library",39);
       if(!LayersBase) LayersBase = OpenLibrary("layers.library",39);
#ifdef USE_DEBUG_FILE
    DebugFile =  Open("rewoot.txt",MODE_NEWFILE);
    if(DebugFile) Write(DebugFile,"ohoh",4);
#endif
        return TRUE;
    }

    void KeyboardView_CloseLibs(void)
    {
#ifdef USE_DEBUG_FILE
    if(DebugFile) Close(DebugFile);
    DebugFile = NULL;
#endif
        if(LayersBase) CloseLibrary(LayersBase);
        if(DOSBase) CloseLibrary((struct Library *)DOSBase);
        if(UtilityBase) CloseLibrary(UtilityBase);
        if(GfxBase) CloseLibrary((struct Library *)GfxBase);
        if(IntuitionBase) CloseLibrary((struct Library *)IntuitionBase);
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
  return(KeyboardView_Dispatcher(Cl,(struct Gadget *)Obj,M,(struct Library *)Cl->cl_Dispatcher.h_Data));
}

int F_SAVED KeyboardView_CreateClass(REG(struct ExtClassLib *LibBase,a6))
{
  if(LibBase) SysBase = LibBase->cb_SysBase;
  if(KeyboardView_OpenLibs())
  {
    if(KeyboardViewClassPtr=MakeClass(KeyboardViewClassID,KeyboardViewSuperClassID,0,sizeof(KeyboardView),0))
    {
     if(LibBase) LibBase->cl_Class = KeyboardViewClassPtr;
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
void F_SAVED KeyboardView_DestroyClass( REG(struct ExtClassLib *LibBase,a6) )
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


