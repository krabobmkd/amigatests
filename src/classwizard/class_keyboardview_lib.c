/**
 * This file contains lib inits that create the class
 * and would OpenLibrary() for other dependencies.
 * word XXX_STATICLINK decides if this is used as the header for a shared .class file
 * or if it is statically linked.
 */
 #define USE_DEBUG_FILE 1
#include <exec/alerts.h>
#include <proto/exec.h>
#include <proto/graphics.h>
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
#ifdef USE_DEBUG_FILE
    BPTR DebugFile=NULL;
#endif

// this is the only global writtable we should see in the whole class binary !
struct IClass   *KeyboardViewClassPtr=NULL;
// this 2 strings are also linked to the asm startup header ( in .gadget mode)
// note: (const char *str="") would make str be a (char **) to the linker. so char str[] is linkable to asm startup
#ifdef KEYBOARDVIEW_STATICLINK
    // in class is linked static, got to create a new identifier for each execution.
    char KeyboardView_CLASS_ID_Static[32]={0};
#else
const char KeyboardViewClassID[]= KeyboardView_CLASS_ID_Base;
#endif

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
    //    Printf("OK\n");
     if(LibBase) LibBase->cb_ClassLibrary.cl_Class = KeyboardViewClassPtr;
      KeyboardViewClassPtr->cl_Dispatcher.h_Data=LibBase;
      KeyboardViewClassPtr->cl_Dispatcher.h_Entry=(REHOOKFUNC)KeyboardView_Dispatcher;

      AddClass(KeyboardViewClassPtr);

    Printf("AddClass success\n");
      /* Success */
      return(0);
    }
         //   Printf("NOT\n");
    KeyboardView_CloseLibs();
  }
  /* Fail */
  return(-1);
}

//void __saveds __asm __KeyboardViewLibCleanup(register __a6 struct Library *LibBase)
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


//====================================================================================

#ifdef KEYBOARDVIEW_STATICLINK
// we can't use a public name for a class when static,
// because boopsi class are public to the whole systems.
// tag the task Id and pointer to it.
void createStaticClassId()
{
    // when static, std runtime is allowed.
    struct Task *ptask;
   ptask = FindTask(NULL);
   KeyboardView_CLASS_ID_Static[0]=0;
   sprintf(&KeyboardView_CLASS_ID_Static[0],"%s%08x",KeyboardView_CLASS_ID_Base,(int)ptask);
   // Printf("%s\n",KeyboardView_CLASS_ID_Static);
}

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
    Printf("KeyboardView_static_class_init\n");
    atexit(KeyboardView_static_class_close);
    createStaticClassId();
    KeyboardView_CreateClass(NULL);

}
#endif


