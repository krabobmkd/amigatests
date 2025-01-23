
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/alib.h>

#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>

#include "class_keyboardview.h"

#include "asmmacros.h"

#include <utility/tagitem.h>




//ULONG pr_Mix(ULONG A,ULONG B, float Percent);

#define MRK_BUFFER_SIZE 3

ULONG F_SAVED KeyboardView_HandleInput(Class *C, struct Gadget *Gad, struct gpInput *Input)
{
  ULONG retval;
  char buffer[MRK_BUFFER_SIZE];
//  LONG  key;
  KeyboardView *gdata;
  struct InputEvent *ie;

  gdata=INST_DATA(C, Gad);
  retval = GMR_MEACTIVE;
  ie = Input->gpi_IEvent;

//  if(gdata->Disabled)
//    return(GMR_NOREUSE);

  switch(ie->ie_Class)
  {    case IECLASS_RAWKEY:
//      KP("RAW KEY CODE - %lx %8lx\n",ie->ie_Code,ie->ie_Qualifier);
//        gdata->gd_MouseMode=0;

      switch(ie->ie_Code)
      {
      /*
        case 0x4c: // UP
          {
            LONG t;

            t=gdata->ActivePen-gdata->Cols;
            if(t<0) t+=gdata->Pens;
            if(t<0) t=0; // double check!
            gdata->ActivePen=t;

            gad_Render(C,Gad,(APTR)Input,GREDRAW_UPDATE);
            i_Notify(C,Gad,(APTR)Input, 0);
          }
          break;

        case 0x4f: // LEFT
          {
            LONG t;

            t=gdata->ActivePen-1;
            if(t<0) t=gdata->Pens-1;
            gdata->ActivePen=t;

            gad_Render(C,Gad,(APTR)Input,GREDRAW_UPDATE);
            i_Notify(C,Gad,(APTR)Input, 0);
          }
          break;

        case 0x4e: // RIGHT
          {
            LONG t;

            t=gdata->ActivePen+1;
            if(t>=gdata->Pens) t=0;
            gdata->ActivePen=t;

            gad_Render(C,Gad,(APTR)Input,GREDRAW_UPDATE);
            i_Notify(C,Gad,(APTR)Input, 0);
          }
          break;

        case 0x4d: // DOWN
          {
            LONG t;

            t=gdata->ActivePen+gdata->Cols;
            if(t>=gdata->Pens) t-=gdata->Pens;
            if(t>=gdata->Pens) t=0; // double check!
            gdata->ActivePen=t;

            gad_Render(C,Gad,(APTR)Input,GREDRAW_UPDATE);
            i_Notify(C,Gad,(APTR)Input, 0);
          }
          break;

        default:
          if(MapRawKey(ie,buffer,MRK_BUFFER_SIZE,0))
          {
       //     KP("%ld %lc\n",buffer[0],buffer[0]);

            switch(buffer[0])
            {
              case 27: // Esc
                retval=GMR_NOREUSE;
                break;
              case 155: //  Shift + TAB
                retval=GMR_NOREUSE | GMR_PREVACTIVE;
                break;
              case  9:  // TAB
                retval=GMR_NOREUSE | GMR_NEXTACTIVE;
                break;
              case 0x20:
//                  step=(shifted?-1:1);
                break;
            }
          }
          break;
          */
      }  // end ie_code switch
      break;
    case IECLASS_RAWMOUSE:
      {
        LONG x,y;
        LONG r,c;
/*
//        retval = GMR_MEACTIVE;

        x=(Input->gpi_Mouse).X+Gad->LeftEdge;
        y=(Input->gpi_Mouse).Y+Gad->TopEdge;

//          DKP("RawMouse %ld %ld\n", x, y);
        if(gdata->MouseMode)
        {
          for(r=0; r<gdata->Rows; r++)
          {
            if(y>=gdata->Row[r] && y<gdata->Row[r+1])
              break;
          }

          for(c=0; c<gdata->Cols; c++)
          {
            if(x>=gdata->Col[c] && x<gdata->Col[c+1])
              break;
          }

//          DKP("  c=%ld r=%ld\n", c, r);

          if(c<gdata->Cols && r<gdata->Rows)
          {
            gdata->ActivePen=r * gdata->Cols + c;
            if(gdata->ActivePen!=gdata->LastActivePen)
            {
              i_StoreUndoIfNeeded(C,Gad,Input);
              i_Notify(C,Gad,(APTR)Input, OPUF_INTERIM);
              gad_Render(C,Gad,(APTR)Input,GREDRAW_UPDATE);
            }
          }

        }
*/
    // still in IECLASS_RAWMOUSE
        switch(ie->ie_Code)
         {
/*
          case SELECTUP:
             gdata->MouseMode=0;


            {// inside gadget
              ULONG em;

              em=gdata->EditMode;
              gdata->EditMode=0;


              switch(em)
              {
                case TCPEM_COPY:
                  i_AddUndo(gdata, gdata->ActivePen,0,0);
                  gdata->Palette[gdata->ActivePen]=gdata->Palette[gdata->EMPen];
                  gad_Render(C,Gad,(APTR)Input,GREDRAW_UPDATE);
                  break;
                case TCPEM_SPREAD:
                  {
                    ULONG l,c1,c2,c3,r1,r2,g1,g2,b1,b2;
                    float p;


//                        pr_SetUndoBuffer(PReq);

                    if(gdata->EMPen>gdata->ActivePen)
                    {
                      c1=gdata->ActivePen;
                      c2=gdata->EMPen;
                    }
                    else
                    {
                      c2=gdata->ActivePen;
                      c1=gdata->EMPen;
                    }

                    c3=c2-c1;

                    if(c3>1)
                    {
                      r1=gdata->Palette[c1].R;
                      g1=gdata->Palette[c1].G;
                      b1=gdata->Palette[c1].B;

                      r2=gdata->Palette[c2].R;
                      g2=gdata->Palette[c2].G;
                      b2=gdata->Palette[c2].B;

                      for(l=c1+1;l<c2;l++)
                      {
                        i_AddUndo(gdata, l ,0,!(l==(c1+1)));
                        p=(float)(l-c1)/(float)c3;
                        gdata->Palette[l].R       =pr_Mix(r1,r2,p);
                        gdata->Palette[l].G       =pr_Mix(g1,g2,p);
                        gdata->Palette[l].B       =pr_Mix(b1,b2,p);
                      }
                    }
                  }
                  gad_Render(C,Gad,(APTR)Input,GREDRAW_REDRAW);
                  break;
                case TCPEM_SWAP:
                  {
                    struct TCPaletteRGB dummy;

                    i_AddUndo(gdata, gdata->ActivePen ,0,0);
                    i_AddUndo(gdata, gdata->EMPen     ,0,1);

                    dummy=gdata->Palette[gdata->ActivePen];
                    gdata->Palette[gdata->ActivePen]=gdata->Palette[gdata->EMPen];
                    gdata->Palette[gdata->EMPen]=dummy;
                  }

                  gad_Render(C,Gad,(APTR)Input,GREDRAW_REDRAW);
                  break;

                default:
                  gad_Render(C,Gad,(APTR)Input,GREDRAW_UPDATE);
              }

              i_Notify(C,Gad,(APTR)Input, 0);

              retval = GMR_MEACTIVE;
//              retval = GMR_NOREUSE;
            }
            break;

          case SELECTDOWN:
             if ( ((Input->gpi_Mouse).X < 0) ||
                 ((Input->gpi_Mouse).X >= Gad->Width) ||
                 ((Input->gpi_Mouse).Y < 0) ||
                 ((Input->gpi_Mouse).Y >= Gad->Height) )
            {// outside gadget

              if(gdata->EditMode)
              {
                gdata->EditMode=0;
                gad_Render(C,Gad,(APTR)Input,GREDRAW_UPDATE);
                i_Notify(C,Gad,(APTR)Input, 0);
              }
//              retval = GMR_NOREUSE | GMR_VERIFY;
              retval = GMR_REUSE;
            }
            else
            {
              gdata->MouseMode=1;
              gad_Render(C,Gad,(APTR)Input,GREDRAW_UPDATE);
              i_Notify(C,Gad,(APTR)Input, 0);
              retval = GMR_MEACTIVE;
            }
            break;
*/

 /* The user hit the menu button. Go inactive and let      */
                                     /* Intuition reuse the menu button event so Intuition can */
                                     /* pop up the menu bar.                                   */

       /*   case MENUDOWN:
          if(gdata->EditMode)//                                                                      (44.3.1) (09/01/00)
            {//                                                                                        (44.3.1) (09/01/00)
              gdata->EditMode=0;//                                                                     (44.3.1) (09/01/00)
              gad_Render(C,Gad,(APTR)Input,GREDRAW_UPDATE);//                                          (44.3.1) (09/01/00)
              i_Notify(C,Gad,(APTR)Input, 0);//                                                        (44.3.1) (09/01/00)
            }//                                                                                        (44.3.1) (09/01/00)
            retval = GMR_REUSE;*/
                                          /* Since the gadget is going inactive, send a final   */
                                         /* notification to the ICA_TARGET.                    */
/*
            break;
            */
          default:
            retval = GMR_MEACTIVE;
        } // end of
      } // end of IECLASS_RAWMOUSE
      break;
  } // end of ieclass switch

  if(retval!=GMR_MEACTIVE)
  {
    i_Notify(C,Gad,(APTR)Input, 0);
  }

  return(retval);
}

/*
ULONG pr_Mix(ULONG A,ULONG B, float Percent)
{
  float a,b,diff;

  a=A;
  b=B;

  diff=b-a;

  a+=diff*Percent;

  return((ULONG) (a));
}*/
