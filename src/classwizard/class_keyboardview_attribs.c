
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>

#ifdef __SASC
    #include <clib/alib_protos.h>
#else
    #ifdef KEYBOARDVIEW_STATICLINK
        #include <proto/alib.h>
    #else
        #include "minialib.h"
    #endif
#endif

#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>

#include "class_keyboardview.h"
#include "class_keyboardview_private.h"

#include "asmmacros.h"

#include <utility/tagitem.h>


ULONG F_SAVED KeyboardView_GetAttr(Class *C, struct Gadget *Gad, struct opGet *Get)
{
  ULONG retval=1;
  KeyboardView *gdata;
  ULONG *data;

  gdata=INST_DATA(C, Gad);

//  DKP("SetAttrs()\n");

  data=Get->opg_Storage;

  switch(Get->opg_AttrID)
  {
  /*
    case TCPALETTE_SelectedColor:
      *data=(ULONG)gdata->ActivePen;
      break;

    case TCPALETTE_RGBPalette:
      {
        ULONG l;
        struct TCPaletteRGB *rgb;

        rgb=(APTR)data;

        for(l=0;l<gdata->Pens;l++)
        {
          rgb[l]=gdata->Palette[l];
        }
      }
      break;

     case TCPALETTE_LRGBPalette:
      {
        ULONG l;

        for(l=0;l<gdata->Pens;l++)
        {
          data[l]=PACKRGB(gdata->Palette[l]);
        }
      }
      break;

    case TCPALETTE_SelectedRGB:
      {
        struct TCPaletteRGB *rgb;

        rgb=(APTR)data;

        *rgb=gdata->Palette[gdata->ActivePen];
      }
      break;

    case TCPALETTE_SelectedLRGB:
      {
        data[0]=PACKRGB(gdata->Palette[gdata->ActivePen]);
      }
      break;

     case TCPALETTE_NumColors:
      *data=gdata->Pens;
      break;

    case TCPALETTE_ShowSelected:
      *data=gdata->ShowSelected;
      break;

    case TCPALETTE_SelectedRed:
      *data=gdata->Palette[gdata->ActivePen].R>>(32-gdata->Precision);
      break;

    case TCPALETTE_SelectedGreen:
      *data=gdata->Palette[gdata->ActivePen].G>>(32-gdata->Precision);
      break;

    case TCPALETTE_SelectedBlue:
      *data=gdata->Palette[gdata->ActivePen].B>>(32-gdata->Precision);
      break;

    case TCPALETTE_Precision:
      *data=gdata->Precision;
      break;

    case TCPALETTE_Orientation:
      *data=gdata->Orientation;
      break;

    case TCPALETTE_NoUndo:
      *data=(gdata->UndoLength?0:1); // returns 0 if there is undo
      break;
*/
    default:
      retval=DoSuperMethodA(C, (APTR)Gad, (APTR)Get);
  }

  return(retval);
}



ULONG Redraw[]={0, GREDRAW_UPDATE, GREDRAW_REDRAW};

ULONG F_SAVED KeyboardView_SetAttrs(Class *C, struct Gadget *Gad, struct opSet *Set)
{
  struct TagItem *tag;
  ULONG retval=0,data;
  KeyboardView *gdata;
  ULONG redraw=0, update=0;


  gdata=INST_DATA(C, Gad);

//  DKP("SetAttrs()\n");

 // ProcessTagList(Set->ops_AttrList,tag,tstate)
  for( tag = Set->ops_AttrList ;
        tag->ti_Tag != TAG_END ;
        tag++
   )
  {
    data=tag->ti_Data;
// Printf("SetAttr:%lx\n",tag->ti_Tag);

    switch(tag->ti_Tag)
    {
    /*
      case TCPALETTE_SelectedColor:
        data=min(data,gdata->Pens);
        data=max(0,data);
        gdata->ActivePen=data;
        update=1;

        break;

      case TCPALETTE_RGBPalette:
        {
          ULONG l;
          struct TCPaletteRGB *rgb;

          rgb=(APTR)data;

          gdata->UndoLength=0;
          gdata->UndoPen=-1;

          for(l=0;l<gdata->Pens;l++)
          {
            gdata->Palette[l]=rgb[l];
          }
          redraw=1;
        }
        break;

      case TCPALETTE_LRGBPalette:
        {
          ULONG l;
          struct TCPaletteLRGB *rgb;

          rgb=(APTR)data;

          gdata->UndoLength=0;
          gdata->UndoPen=-1;

          for(l=0;l<gdata->Pens;l++)
          {
            gdata->Palette[l].R=rgb[l].R * 0x01010101;
            gdata->Palette[l].G=rgb[l].G * 0x01010101;
            gdata->Palette[l].B=rgb[l].B * 0x01010101;
          }
          redraw=1;
        }
        break;


      case TCPALETTE_SelectedRGB:
          i_StoreUndoIfNeeded(C, Gad, (Msg)Set);
          gdata->Palette[gdata->ActivePen]=*((struct TCPaletteRGB *)data);
          update=1;
        break;

      case TCPALETTE_SelectedLRGB:
        {
          struct TCPaletteLRGB *rgb;


          i_StoreUndoIfNeeded(C, Gad, (Msg)Set);
          rgb=(APTR)&data;

          gdata->Palette[gdata->ActivePen].R=rgb->R * 0x01010101;
          gdata->Palette[gdata->ActivePen].G=rgb->G * 0x01010101;
          gdata->Palette[gdata->ActivePen].B=rgb->B * 0x01010101;
          update=1;
        }
        break;

      case TCPALETTE_NumColors:
        gdata->Pens=min(data,256);
        gdata->Pens=max(1,gdata->Pens);
        redraw=1;
        break;

      case TCPALETTE_ShowSelected:
        gdata->ShowSelected=(data?1:0);
        update=1;
        break;

      case TCPALETTE_SelectedRed:
        i_StoreUndoIfNeeded(C, Gad, (Msg)Set);
        gdata->Palette[gdata->ActivePen].R=Expand(data, gdata->Precision);
        update=1;
        break;
      case TCPALETTE_SelectedGreen:
        i_StoreUndoIfNeeded(C, Gad, (Msg)Set);
        gdata->Palette[gdata->ActivePen].G=Expand(data, gdata->Precision);
        update=1;
        break;
      case TCPALETTE_SelectedBlue:
        i_StoreUndoIfNeeded(C, Gad, (Msg)Set);
        gdata->Palette[gdata->ActivePen].B=Expand(data, gdata->Precision);
        update=1;
        break;
      case TCPALETTE_Precision:
        gdata->Precision=data;
        break;

      case TCPALETTE_Orientation:
        gdata->Orientation=data;
        break;

      case TCPALETTE_EditMode:
        gdata->EditMode=data;
        gdata->EMPen=gdata->ActivePen;
        update=1;
        break;

      case TCPALETTE_Undo:
        i_GetUndo(C, Gad, (Msg)Set);
        redraw=1;
        break;

      case GA_Disabled:
        gdata->Disabled=data;
        redraw=1;
        break;
        */
    }
  }

  if(redraw | update)
  {
    struct RastPort *rp;

    if(rp=ObtainGIRPort(Set->ops_GInfo))
    {
      DoMethod((Object *)Gad,GM_RENDER,Set->ops_GInfo,rp,(redraw?GREDRAW_REDRAW:GREDRAW_UPDATE));
      ReleaseGIRPort(rp);
    }

    if(redraw)
    {
      KeyboardView_Notify(C,Gad,(Msg)Set,0);
    }
  }

  return(retval);
}


