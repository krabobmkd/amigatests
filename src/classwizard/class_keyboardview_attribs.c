
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>

#ifdef __SASC
//    #include "minialib.h"
    #include <clib/alib_protos.h>
#else
    // GCC
    #include "minialib.h"
#endif

#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>

#include "class_keyboardview.h"
#include "class_keyboardview_private.h"

#include <utility/tagitem.h>


ULONG KeyboardView_GetAttr(Class *C, struct Gadget *Gad, struct opGet *Get)
{
  ULONG retval=1;
  int   DoSuperCall=0;
  KeyboardView *gdata;
  ULONG *data;

  gdata=INST_DATA(C, Gad);

  data=Get->opg_Storage;

  switch(Get->opg_AttrID)
  {
    case KEYBOARDVIEW_CenterX:
        *data = (LONG)gdata->_circleCenterX;
    break;
    case KEYBOARDVIEW_CenterY:
        *data = (LONG)gdata->_circleCenterY;
    break;
    // super class gadget things
    case GA_Disabled:
        *data = (LONG)gdata->_disabled;
        DoSuperCall = 1;
    break;
    case GA_Highlight:
        *data = (LONG)gdata->_highlighted;
        DoSuperCall = 1;
    break;
    case GA_Selected:
        *data = (LONG)gdata->_selected;
        DoSuperCall = 1;
    break;
    default:
        DoSuperCall = 1;
      // everything we don't manage directly is managed by supercall.
  }
  if(DoSuperCall)  retval=DoSuperMethodA(C, (APTR)Gad, (APTR)Get);

  return(retval);
}



ULONG Redraw[]={0, GREDRAW_UPDATE, GREDRAW_REDRAW};

ULONG KeyboardView_SetAttrs(Class *C, struct Gadget *Gad, struct opSet *Set)
{
  struct TagItem *tag;
  ULONG retval=0,data;
  KeyboardView *gdata;
  ULONG redraw=0, update=0, notifCoords=0;

  gdata=INST_DATA(C, Gad);

 // set can use a list of attribs to change, so we manage this with a loop.
 // this also allows to have just one draw refresh for a set of change.
  for( tag = Set->ops_AttrList ;
        tag->ti_Tag != TAG_END ;
        tag++
   )
  {
    data=tag->ti_Data;

    switch(tag->ti_Tag)
    {
     case KEYBOARDVIEW_CenterX:
        if((UWORD)data != gdata->_circleCenterX )
        {
            gdata->_circleCenterX = (UWORD)data ;
            //update=1;
            redraw=1;
            notifCoords = 1;
        }
        break;
     case KEYBOARDVIEW_CenterY:
        if((UWORD)data != gdata->_circleCenterY )
        {
            gdata->_circleCenterY = (UWORD)data ;
            //update=1;
            redraw=1;
            notifCoords = 1;
        }
        break;
     // - - - actually we have to manage these super class attribs:
      case GA_Disabled:
        gdata->_disabled = (UBYTE)data;
        redraw=1;
        break;
      case GA_Highlight:
        gdata->_highlighted = (UBYTE)data;
        redraw=1;
        break;
      case GA_Selected:
        gdata->_selected = (UBYTE)data;
        redraw=1;
        break;
    default:
        //note: apparently super call is ont to be managed here (not sure !!!)
        break;

    } // end switch
  } // end for

  if(redraw | update)
  {
    struct RastPort *rp;

    if(rp=ObtainGIRPort(Set->ops_GInfo))
    {
        // freeze with "..." call with GCC6.5, use local struct works 100%.
        struct gpRender gpr;
        gpr.MethodID = GM_RENDER;
        gpr.gpr_GInfo = Set->ops_GInfo;
        gpr.gpr_RPort = rp;
        gpr.gpr_Redraw = (redraw?GREDRAW_REDRAW:GREDRAW_UPDATE);

        DoMethodA((Object *)Gad,(Msg)&gpr.MethodID);
        ReleaseGIRPort(rp);
    }

    if(notifCoords)
    {
        KeyboardView_NotifyCoords(C,Gad,Set->ops_GInfo);
    }
  }

  return(retval);
}


