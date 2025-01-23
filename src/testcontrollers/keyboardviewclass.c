#include "class_keyboardview.h"


#include <proto/exec.h>
#include <proto/intuition.h>
//#include <proto/utility.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>

#include "asmmacros.h"

Class   *KeyboardViewClassPtr=NULL;
const char *KeyboardViewClassID="krb.keyboardview";
const char *KeyboardViewSuperClassID="";

// this in .h ?
typedef struct IKeyboardView {
    ULONG _;
} KeyboardView;



#define INSTANCE_SIZE sizeof(KeyboardView)

#define KEYBOARDVIEW_STATICLINK 1
// note: if other boopsi classes are dependenices, they need to be opened here.
#ifdef KEYBOARDVIEW_STATICLINK

    void KeyboardView_OpenLibs(){

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
// =======================================================================================

Class * F_SAVED LIB_TCPALETTE_GetClass(void)
{
  return(KeyboardViewClassPtr);
}


// for dispatcher
typedef union MsgUnion
{
  ULONG  MethodID;
  struct opSet        opSet;
  struct opUpdate     opUpdate;
  struct opGet        opGet;
  struct gpHitTest    gpHitTest;
  struct gpRender     gpRender;
  struct gpInput      gpInput;
  struct gpGoInactive gpGoInactive;
  struct gpLayout     gpLayout;
} *Msgs;

ULONG F_SAVED Dispatcher(
                    REG(Class *C,a0), // register __a0 Class *C,
                    REG(struct Gadget *Gad,a2), // register __a2 struct Gadget *Gad,
                    REG(Msgs M,a1), // register __a1 Msgs M,
                    REG(struct Library *LibBase,a6) // register __a6 struct Library *LibBase
                    )
{
  KeyboardView *gdata;
  ULONG retval=0;

  gdata=INST_DATA(C, Gad);

//  DKP("Dispatcher MethodID %08lx\n", M->MethodID);

  switch(M->MethodID)
  {
    case OM_NEW:
      if(Gad=(struct Gadget *)DoSuperMethodA(C,(Object *)Gad,(Msg)M))
      {
        gdata=INST_DATA(C, Gad);

        SetSuperAttrs(C,Gad, GA_TabCycle,1,TAG_DONE);

//        gdata->Pattern=NewObject(0,(UBYTE *)"mlr_ordered.pattern", TAG_DONE);
//        {
//          if(gdata->Bevel=BevelObject, BEVEL_Style, BVS_BUTTON, BEVEL_FillPen, -1, End)
//          {
//            gdata->Precision=8;
//            gdata->ShowSelected=1;

//            gad_SetAttrs(C,Gad,(struct opSet *)M);
//            retval=(ULONG)Gad;
//          }
//        }
      }
      break;

    case OM_UPDATE:
    case OM_SET:
      retval=DoSuperMethodA(C,(Object *)Gad,(Msg)M);
      gad_SetAttrs(C,Gad,(struct opSet *)M);
     break;

    case OM_GET:
      gad_GetAttr(C,Gad,(struct opGet *)M);
     break;

    case OM_DISPOSE:
      DisposeObject(gdata->Pattern);
      DisposeObject(gdata->Bevel);
      retval=DoSuperMethodA(C,(Object *)Gad,(Msg)M);
      break;

    case GM_HITTEST:
      retval = GMR_GADGETHIT;
      break;

    case GM_GOACTIVE:
      Gad->Flags |= GFLG_SELECTED;
      retval=gad_HandleInput(C,Gad,(struct gpInput *)M);
//      gad_Render(C,Gad,(APTR)M,GREDRAW_UPDATE);
//      retval=GMR_MEACTIVE;
      break;

    case GM_GOINACTIVE:
      Gad->Flags &= ~GFLG_SELECTED;
      gad_Render(C,Gad,(APTR)M,GREDRAW_UPDATE);
      break;

    case GM_LAYOUT:
      retval=gad_Layout(C,Gad,(struct gpLayout *)M);
      break;

    case GM_RENDER:
      retval=gad_Render(C,Gad,(struct gpRender *)M,0);
      break;

    case GM_HANDLEINPUT:
      retval=gad_HandleInput(C,Gad,(struct gpInput *)M);
      break;

    case GM_DOMAIN:
      gad_Domain(C, Gad, (APTR)M);
      retval=1;
      break;

    default:
      retval=DoSuperMethodA(C,(Object *)Gad,(Msg)M);
      break;
  }
  return(retval);
}



//==========================================================================================

//ULONG __asm KeyboardView_DispatcherStub(register __a0 Class *Cl, register __a2 Object *Obj, register __a1 Msg M)
ULONG F_ASM KeyboardView_DispatcherStub(REG(Class *Cl,a0),REG(Object *Obj,a2),REG(Msg M,a1))
{
  return(Dispatcher(Cl,Obj,M,(struct Library *)Cl->cl_Dispatcher.h_Data));
}


int F_SAVED KeyboardView_LibInit(REG(struct Library *LibBase,a6))
{
  if(i_OpenLibs())
  {

    if(KeyboardViewClassPtr=MakeClass(KeyboardViewClassID,KeyboardViewSuperClassID,0,INSTANCE_SIZE,0))
    {
//      DKP("   A4=%8lx A6=%8lx UtilityBase:&%8lx = %8lx\n",getreg(REG_A4),getreg(REG_A6),&UtilityBase,UtilityBase);
      KeyboardViewClassPtr->cl_Dispatcher.h_Data=LibBase;
      KeyboardViewClassPtr->cl_Dispatcher.h_Entry=DispatcherStub;

      AddClass(KeyboardViewClassPtr);
      /* Success */
      return(0);
    }
    i_CloseLibs();
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

  i_CloseLibs();
}


//====================================================================================

#ifdef KEYBOARDVIEW_STATICLINK
void KeyboardView_LibInit_static()
{

}
#endif
