
#include <proto/exec.h>
#include <proto/intuition.h>
//#include <proto/utility.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>

#include "class_keyboardview.h"

#include "asmmacros.h"

Class   *KeyboardViewClassPtr=NULL;
const char *KeyboardViewClassID="krb.keyboardview";
const char *KeyboardViewSuperClassID="";

#define KEYBOARDVIEW_STATICLINK 1
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

      SysBase=(APTR)LongMem[1];

//        return(ex_OpenLibs(0, "MPEditor", 0,0,0, MyLibs));
    }

    void KeyboardView_CloseLibs(void)
    {
//      ex_CloseLibs(MyLibs);
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
void KeyboardView_LibInit_static()
{

}
#endif
