
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/alib.h>

//#include <proto/utility.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>

#include "class_keyboardview.h"

#include "asmmacros.h"


ULONG F_SAVED KeyboardView_SetAttrs(Class *C, struct Gadget *Gad, struct opSet *Set);
ULONG F_SAVED KeyboardView_GetAttr(Class *C, struct Gadget *Gad, struct opGet *Get);
ULONG F_SAVED KeyboardView_Layout(Class *C, struct Gadget *Gad, struct gpLayout *layout);
ULONG F_SAVED KeyboardView_Render(Class *C, struct Gadget *Gad, struct gpRender *Render, ULONG update);
ULONG F_SAVED KeyboardView_HandleInput(Class *C, struct Gadget *Gad, struct gpInput *Input);
ULONG F_SAVED KeyboardView_Domain(Class *C, struct Gadget *Gad, struct gpDomain *D);

// for dispatcher
typedef union MsgUnion
{
  ULONG  MethodID;
  struct opSet        opSet;
  struct opUpdate     opUpdate;
  struct opGet        opGet;
//  struct gpHitTest    gpHitTest;
//  struct gpRender     gpRender;
//  struct gpInput      gpInput;
//  struct gpGoInactive gpGoInactive;
//  struct gpLayout     gpLayout;
} *Msgs;

ULONG F_SAVED KeyboardView_Dispatcher(
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
      KeyboardView_SetAttrs(C,Gad,(struct opSet *)M);
     break;

    case OM_GET:
      KeyboardView_GetAttr(C,Gad,(struct opGet *)M);
     break;

    case OM_DISPOSE:
      /*DisposeObject(gdata->Pattern);
      DisposeObject(gdata->Bevel);
      */
      retval=DoSuperMethodA(C,(Object *)Gad,(Msg)M);
      break;

    case GM_HITTEST:
      retval = GMR_GADGETHIT;
      break;

    case GM_GOACTIVE:
      Gad->Flags |= GFLG_SELECTED;
      retval=KeyboardView_HandleInput(C,Gad,(struct gpInput *)M);
//      gad_Render(C,Gad,(APTR)M,GREDRAW_UPDATE);
//      retval=GMR_MEACTIVE;
      break;

    case GM_GOINACTIVE:
      Gad->Flags &= ~GFLG_SELECTED;
      KeyboardView_Render(C,Gad,(APTR)M,GREDRAW_UPDATE);
      break;

    case GM_LAYOUT:
      retval=KeyboardView_Layout(C,Gad,(struct gpLayout *)M);
      break;

    case GM_RENDER:
      retval=KeyboardView_Render(C,Gad,(struct gpRender *)M,0);
      break;

    case GM_HANDLEINPUT:
      retval=KeyboardView_HandleInput(C,Gad,(struct gpInput *)M);
      break;

    case GM_DOMAIN:
      KeyboardView_Domain(C, Gad, (APTR)M);
      retval=1;
      break;

    default:
      retval=DoSuperMethodA(C,(Object *)Gad,(Msg)M);
      break;
  }
  return(retval);
}

