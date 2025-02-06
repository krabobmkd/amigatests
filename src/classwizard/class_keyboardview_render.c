
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/dos.h>
#include <proto/layers.h>

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

/* The GM_DOMAIN method is used to obtain the sizing requirements of an
 * object for a class before ever creating an object. */

/* GM_DOMAIN */
//struct gpDomain
//{
//    ULONG		 MethodID;
//    struct GadgetInfo	*gpd_GInfo;
//    struct RastPort	*gpd_RPort;	/* RastPort to layout for */
//    LONG		 gpd_Which;
//    struct IBox		 gpd_Domain;	/* Resulting domain */
//    struct TagItem	*gpd_Attrs;	/* Additional attributes */
//};

ULONG KeyboardView_Domain(Class *C, struct Gadget *Gad, struct gpDomain *D)
{
  KeyboardView *gdata=0;

  if(Gad) gdata=INST_DATA(C, Gad);
// Printf("KeyboardView_Domain data:%lx\n",(int)gdata);

  D->gpd_Domain.Left=0;
  D->gpd_Domain.Top=0;

  switch(D->gpd_Which)
  {
    case GDOMAIN_NOMINAL:
//      if(gdata)
//      {
//        D->gpd_Domain.Width =sqrt(gdata->Pens) * 12 + 8;
//        D->gpd_Domain.Height=sqrt(gdata->Pens) * 12 + 8;
//      }
//      else
      {
        D->gpd_Domain.Width=100;
        D->gpd_Domain.Height=50;
      }
      break;

    case GDOMAIN_MAXIMUM:
      D->gpd_Domain.Width=16000;
      D->gpd_Domain.Height=16000;
      break;

    case GDOMAIN_MINIMUM:
    default:
//      if(gdata)
//      {
//        D->gpd_Domain.Width =sqrt(gdata->Pens) * 8 + 8;
//        D->gpd_Domain.Height=sqrt(gdata->Pens) * 8 + 8;
//      }
//      else
      {
        D->gpd_Domain.Width=  50;
        D->gpd_Domain.Height= 50;
      }
      break;

  }
  return(1);
}

/**
 * method GM_LAYOUT
 * The gadget knows its final coordinates,
 * So we may have to resize what's inside our gadget.
 */
ULONG KeyboardView_Layout(Class *C, struct Gadget *Gad, struct gpLayout *layout)
{
  KeyboardView *gdata;
  LONG rows,cols,l,topedge,leftedge,width,height;
  BOOL swap=0;

//    WORD left   =Gad->LeftEdge;
//    WORD top    =Gad->TopEdge;
//    WORD width  =Gad->Width;
//    WORD height =Gad->Height;
    gdata=INST_DATA(C, Gad);
    gdata->_updateClipRegion = 1;
//  float cfloat,aspect;
//    Printf("KeyboardView_Layout: %ld %ld\n",(int)Gad->Width,(int)Gad->Height);
//    Printf("left: %ld top: %ld width: %ld heigt: %ld\n",
//    (int)Gad->LeftEdge,(int)Gad->TopEdge,(int)Gad->Width,(int)Gad->Height);



//  DKP("GM_LAYOUT\n");
// GadgetInfo
/*
  SetAttrs(gdata->Bevel, IA_Left,       Gad->LeftEdge,
                         IA_Top,        Gad->TopEdge,
                         IA_Width,      Gad->Width,
                         IA_Height,     Gad->Height,
                         BEVEL_ColorMap,layout->gpl_GInfo->gi_Screen->ViewPort.ColorMap,
                         TAG_DONE);

  GetAttr(BEVEL_InnerTop,     gdata->Bevel, &topedge);
  GetAttr(BEVEL_InnerLeft,    gdata->Bevel, &leftedge);
  GetAttr(BEVEL_InnerWidth,   gdata->Bevel, &width);
  GetAttr(BEVEL_InnerHeight,  gdata->Bevel, &height);
*/
/*
  if(width>0 && height>0)
  {
    aspect=((float)height / (float)width);

    if(aspect<1)
    {
      aspect=((float)width / (float)height);
      swap=1;
    }

//    DKP("  aspect %ld\n",(ULONG)(aspect*1000));

    rows=sqrt(gdata->Pens * aspect) + .5;

    if(rows<1) rows=1;
    if(rows>gdata->Pens) rows=gdata->Pens;

//    DKP("  rows=%ld\n",rows);

    cfloat=gdata->Pens / rows;
    cols=cfloat;

    while(rows>1 && (cfloat * rows)!=gdata->Pens)
    {
      rows--;
      cfloat=gdata->Pens / rows;
      cols=cfloat;
//      DKP("   testing - rows %ld cfloat %lf cols=%ld\n",rows, cfloat, cols);
    }

//    DKP("  cols=%ld\n",cols);

    if(swap)
    {
      gdata->Rows = cols;
      gdata->Cols = rows;

      rows=gdata->Rows;
      cols=gdata->Cols;
    }
    else
    {
      gdata->Rows = rows;
      gdata->Cols = cols;
    }

    for(l=0;l<=rows;l++)
    {
      gdata->Row[l]=(LONG)height * l / (rows) + topedge;
    }

    for(l=0;l<=cols;l++)
    {
      gdata->Col[l]=(LONG)width * l / (cols)  + leftedge;
    }
  }
  else
  {
    gdata->Rows=gdata->Cols=0;
  }
  if(layout->gpl_Initial)
  {
    KeyboardView_Notify(C,Gad,layout,0);
  }
*/
//  if(layout->gpl_Initial)
//  {
//    KeyboardView_Notify(C,Gad,layout,0);
//  }
  return(1);
}


/* draw yourself, in the appropriate state */
ULONG KeyboardView_Render(Class *C, struct Gadget *Gad, struct gpRender *Render, ULONG update)
{
  KeyboardView *gdata;
  struct RastPort *rp;
  LONG l,left,top,width,height,right,bottom;
  ULONG retval=1;

  gdata=INST_DATA(C, Gad);

  if(Render->MethodID==GM_RENDER)
  {
    rp=Render->gpr_RPort;
    update=Render->gpr_Redraw;
  }
  else
  {
    rp = ObtainGIRPort(Render->gpr_GInfo);
  }

  if(rp)
  {
    int penbg=1,penb=2,penc=3;
//  struct Rectangle gadgetrec;
    struct Region *oldClipRegion;
      WORD left   =Gad->LeftEdge;
      WORD top    =Gad->TopEdge;
      WORD width  =Gad->Width;
      WORD height =Gad->Height;

    if(Gad->Flags & GFLG_DISABLED) // if disabled, draw background with another color.
    {
        penbg = 0;
    }
    //
//    gadgetrec.MinX = left;
//    gadgetrec.MinY = top;
//    gadgetrec.MaxX = left + width  -1;
//    gadgetrec.MaxY = top  + height -1;
// In OM_NEW : gdata->_clipRegion = NewRegion(); , OM_DISPOSE DisposeRegion(gdata->_clipRegion);
#ifdef USE_REGION_CLIPPING
    if(1){   // must be done each time ?
        struct Rectangle gadgetrec;
        ClearRegion(gdata->_clipRegion);
        gadgetrec.MinX = Gad->LeftEdge;
        gadgetrec.MinY = Gad->TopEdge;
        gadgetrec.MaxX = Gad->LeftEdge + Gad->Width  -1;
        gadgetrec.MaxY = Gad->TopEdge  + Gad->Height -1;
        OrRectRegion(gdata->_clipRegion, &gadgetrec);
        gdata->_updateClipRegion = 0;
    }
    oldClipRegion = InstallClipRegion( rp->Layer, gdata->_clipRegion);
#endif
      SetDrMd(rp,JAM1);
      SetAPen(rp,penbg);
      RectFill(rp,left,
                  top,
                  left + width  -1,
                  top  + height -1) ;
    {
        UWORD xc = left + ((width*gdata->_circleCenterX)>>16);
        UWORD yc = top + ((height*gdata->_circleCenterY)>>16);
        SetAPen(rp,penb);
        DrawEllipse(rp,xc,yc,width>>1,height>>1);
        SetAPen(rp,penc);
        DrawEllipse(rp,xc,yc,width>>2,height>>2);
    }
#ifdef USE_REGION_CLIPPING
    InstallClipRegion( rp->Layer,oldClipRegion); // important to pass NULL if oldClipRegion is NULL.
#endif
    if (Render->MethodID != GM_RENDER)
      ReleaseGIRPort(rp);
  }
  return(retval);
}

//void i_RenderColorBox(Class *C, struct Gadget *Gad, struct GadgetInfo *gi, struct RastPort *rp, ULONG Pen)
//{
//  struct DrawInfo *di;
//  struct GadData *gdata;
//  ULONG row,col,
//          left,top,
//          width,height,
//          bottom,right;

//  gdata=INST_DATA(C, Gad);

//  di=gi->gi_DrInfo;

//  col=Pen % gdata->Cols;
//  row=Pen / gdata->Cols;

//  left    =gdata->Col[col];
//  right   =gdata->Col[col+1]-1;
//  width   =right-left;
//  top     =gdata->Row[row];
//  bottom  =gdata->Row[row+1]-1;
//  height  =bottom-top;

//#define SIZE (0)


//  if(Pen==gdata->ActivePen && (((Gad->Flags & GFLG_SELECTED) && gdata->MouseMode) || gdata->ShowSelected))
//  {
//    if(gdata->EditMode)
//    {
////      SetDrPt(rp,0x0f0f);

///*      SetAPen(rp, di->dri_Pens[BACKGROUNDPEN]);

//      Move(rp,left,   bottom);
//      Draw(rp,left,   top);
//      Draw(rp,right,  top);
//      Draw(rp,right,  bottom);
//      Draw(rp,left,   bottom);

//      Move(rp,left+1,   bottom-1);
//      Draw(rp,left+1,   top+1);
//      Draw(rp,right-1,  top+1);
//      Draw(rp,right-1,  bottom-1);
//      Draw(rp,left+1,   bottom-1);
//*/
//      SetDrPt(rp,0xF0F0);
//    }

//    SetDrMd(rp,JAM2);

//    SetBPen(rp, di->dri_Pens[BACKGROUNDPEN]);

//    SetAPen(rp, di->dri_Pens[SHADOWPEN]);
//    Move(rp,left,bottom);
//    Draw(rp,left,top);
//    Draw(rp,right,top);

//    SetAPen(rp, di->dri_Pens[SHINEPEN]);
//    Draw(rp,right,bottom);
//    Draw(rp,left,bottom);

//    SetAPen(rp, di->dri_Pens[SHADOWPEN]);
//    Move(rp,left+1,bottom-1);
//    Draw(rp,left+1,top+1);
//    Draw(rp,right-1,top+1);

//    SetAPen(rp, di->dri_Pens[SHINEPEN]);
//    Draw(rp,right-1,  bottom-1);
//    Draw(rp,left+1,   bottom-1);

//    SetDrPt(rp,0xFfff);

//    SetAPen(rp, di->dri_Pens[BACKGROUNDPEN]);
//    Move(rp,left+2,bottom-2);
//    Draw(rp,left+2,top+2);
//    Draw(rp,right-2,top+2);
//    Draw(rp,right-2,  bottom-2);
//    Draw(rp,left+2,   bottom-2);



//    top+=3;
//    left+=3;
//    right-=3;
//    bottom-=3;
//  }
//  else
//  {
//    SetAPen(rp, di->dri_Pens[BACKGROUNDPEN]);
//    Move(rp,left,bottom);
//    Draw(rp,left,top);
//    Draw(rp,right,top);
//    Draw(rp,right,bottom);
//    Draw(rp,left,bottom);
///*

//    Move(rp,left+1,bottom-1);
//    Draw(rp,left+1,top+1);
//    Draw(rp,right-1,top+1);
//    Draw(rp,right-1,  bottom-1);
//    Draw(rp,left+1,   bottom-1);*/

//    top+=1;
//    left+=1;
//    right-=1;
//    bottom-=1;
//  }

//  width   =right-left+1;
//  height  =bottom-top+1;

//  if(CyberGfxBase && GetBitMapAttr(gi->gi_Screen->RastPort.BitMap, BMA_DEPTH )>8)
//  {
//    ULONG argb;

//    argb= ((gdata->Palette[Pen].R & 0xff000000) >> 8) |
//          ((gdata->Palette[Pen].G & 0xff000000) >> 16) |
//          ((gdata->Palette[Pen].B & 0xff000000) >> 24);


//    FillPixelArray(rp, left, top, width, height, argb);
//  }
//  else
//  {
//    ULONG p;
//    p=FindColor(gi->gi_Screen->ViewPort.ColorMap,
//                        gdata->Palette[Pen].R,
//                        gdata->Palette[Pen].G,
//                        gdata->Palette[Pen].B,
//                        -1);

//    SetAPen(rp, p);
//    RectFill(rp,left, top, right, bottom);
//  }

//  if(gdata->Disabled)
//  {
//    gui_GhostRect(rp, gi->gi_DrInfo->dri_Pens[TEXTPEN], left, top, right, bottom);
//  }

//  /*
//  SetAttrs(gdata->Pattern,PAT_RastPort,    rp,
//                          PAT_DitherAmt,   gdata->ActivePen * 256,
//                          TAG_DONE);
//*/

//}


