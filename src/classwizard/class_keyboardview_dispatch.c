
    #include <proto/exec.h>
    #include <proto/intuition.h>
    #include <proto/dos.h>
#ifdef __SASC
    #include <clib/alib_protos.h>
//#include "minialib.h"
    #ifndef GM_DOMAIN
        #define GM_DOMAIN	(7)
    #endif
#else
    #ifdef KEYBOARDVIEW_STATICLINK
        #include <proto/alib.h>
    #else
        #include "minialib.h"
    #endif
#endif

//#include <proto/utility.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>

#include "class_keyboardview.h"
#include "class_keyboardview_private.h"
#include "asmmacros.h"


ULONG F_SAVED KeyboardView_Dispatcher(
                    REG(Class *C,a0), // register __a0 Class *C,
                    REG(struct Gadget *Gad,a2), // register __a2 struct Gadget *Gad,
                    REG(Msgs M,a1), // register __a1 Msgs M,
                    REG(struct Library *LibBase,a6) // register __a6 struct Library *LibBase
                    )
{
  KeyboardView *gdata;
  ULONG retval=0;
   // Printf("KeyboardView_Dispatcher %lx %lx\n",(int)C,(int)Gad);
  gdata=INST_DATA(C, Gad);

//  DKP("Dispatcher MethodID %08lx\n", M->MethodID);

  switch(M->MethodID)
  {
    case OM_NEW:
 //   Printf("kbd OM_NEW\n");
      if(Gad=(struct Gadget *)DoSuperMethodA(C,(Object *)Gad,(Msg)M))
      {

        gdata=INST_DATA(C, Gad);
        // DEVTODO: here you write the default values for your objects.
        gdata->_circleCenterX = 32767;
        gdata->_circleCenterY = 32767;

        // set gadget (super class) attributes for this instance like this:
   //TODO     SetSuperAttrs(C,Gad, GA_TabCycle,1,TAG_DONE);

 //   Printf("instance:%lx\n",(int)gdata);
//        SetSuperAttrs(C,Gad, GA_TabCycle,1,TAG_DONE);

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
        // means new object OK so far:
        retval=(ULONG)Gad;
      }
      break;

    case OM_UPDATE:
 //   Printf("kbd OM_UPDATE\n");
    case OM_SET:
   //     Printf("kbd OM_SET:\n");
      retval=DoSuperMethodA(C,(Object *)Gad,(Msg)M);
      KeyboardView_SetAttrs(C,Gad,(struct opSet *)M);
     break;

    case OM_GET:
  //  Printf("kbd OM_GET\n");
      KeyboardView_GetAttr(C,Gad,(struct opGet *)M);
     break;

    case OM_DISPOSE:
  //  Printf("kbd OM_DISPOSE\n");
      /*DisposeObject(gdata->Pattern);
      DisposeObject(gdata->Bevel);
      */
      retval=DoSuperMethodA(C,(Object *)Gad,(Msg)M);
      break;

    case GM_HITTEST:
 //   Printf("kbd GM_HITTEST\n");
      retval = GMR_GADGETHIT;
      break;

    case GM_GOACTIVE:
  //  Printf("kbd GM_GOACTIVE\n");
      Gad->Flags |= GFLG_SELECTED;
      retval=KeyboardView_HandleInput(C,Gad,(struct gpInput *)M);
//      gad_Render(C,Gad,(APTR)M,GREDRAW_UPDATE);
//      retval=GMR_MEACTIVE;
      break;

    case GM_GOINACTIVE:
   // Printf("kbd GM_GOINACTIVE\n");
      Gad->Flags &= ~GFLG_SELECTED;
      KeyboardView_Render(C,Gad,(APTR)M,GREDRAW_UPDATE);
      break;

    case GM_LAYOUT:
  //  Printf("kbd GM_LAYOUT\n");
     // retval=DoSuperMethodA(C,(Object *)Gad,(Msg)M);
      retval=1; //KeyboardView_Layout(C,Gad,(struct gpLayout *)M);
      break;

    case GM_RENDER:
  //  Printf("kbd GM_RENDER\n");
      retval=KeyboardView_Render(C,Gad,(struct gpRender *)M,0);
      break;

    case GM_HANDLEINPUT:
  //  Printf("kbd GM_HANDLEINPUT\n");
      retval=KeyboardView_HandleInput(C,Gad,(struct gpInput *)M);
      break;

    case GM_DOMAIN:
   // Printf("kbd GM_DOMAIN\n");
      KeyboardView_Domain(C, Gad, (APTR)M);
      retval=1;

    break;

    default:
  //  Printf(" - default unmanaged method - %lx\n",);
      retval=DoSuperMethodA(C,(Object *)Gad,(Msg)M);
      break;
  }
  return(retval);
}

