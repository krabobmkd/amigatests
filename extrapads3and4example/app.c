
   /***********************************************************************
   *                                                                      *
   *                            COPYRIGHTS                                *
   *                                                                      *
   *   Copyright (c) 1990  Commodore-Amiga, Inc.  All Rights Reserved.    *
   *                                                                      *
   ***********************************************************************/

/* app.c This file contains the custom code for a commodity */
/* you should be able to write a new commodity by changing only */
/* app.c and app.h */

#include "app.h"

#if WINDOW

#define V(x) ((VOID *)x)

struct TextAttr mydesiredfont =
   {
      "topaz.font",  /*  Name */
      8,             /*  YSize */
      0,             /*  Style */
      0,             /*  Flags */
   };

VOID setupCustomGadgets(gad)
struct Gadget **gad;
{
   struct NewGadget ng;

   ng.ng_TopEdge    = topborder+0;
   ng.ng_LeftEdge   = 10;
   ng.ng_Width      = 40;
   ng.ng_Height     = 12;
   ng.ng_GadgetText = "Hide";
   ng.ng_TextAttr   = &mydesiredfont;
   ng.ng_GadgetID   = GAD_HIDE;
   ng.ng_Flags      = NULL;
   ng.ng_VisualInfo = vi;
   *gad = CreateButtonGadget(*gad, &ng,TAG_DONE);

   ng.ng_TopEdge    = topborder+15;
   ng.ng_LeftEdge   = 10;
   ng.ng_Width      = 40;
   ng.ng_Height     = 12;
   ng.ng_GadgetText = "Die";
   ng.ng_TextAttr   = &mydesiredfont;
   ng.ng_GadgetID   = GAD_DIE;
   ng.ng_Flags      = NULL;
   ng.ng_VisualInfo = vi;
   *gad = CreateButtonGadget(*gad, &ng,TAG_DONE);
}
VOID HandleGadget(gad,code)
ULONG gad,code;
{
   D( kprintf("app: HandleGadget(%lx)\n",gad); )
   switch(gad)
   {
      case GAD_HIDE:
            D( kprintf("app: HandleGadget() GAD_HIDE\n"); )
            shutdownWindow();
            break;
      case GAD_DIE:
            D( kprintf("app: HandleGadget() GAD_DIE\n"); )
            terminate();
   }
}
VOID setupCustomMenu()
{
   struct NewMenu mynewmenu [] =
      {
         {  NM_TITLE,   "Project",  0,    0, 0, 0,             },
         {   NM_ITEM,   "Hide",     "H",  0, 0, V(MENU_HIDE),   },
         {   NM_ITEM,   "Die",      "Q",  0, 0, V(MENU_DIE),    },
         {  NM_END,     0,          0,    0, 0, 0              },
      };

   menu=CreateMenus(mynewmenu,TAG_DONE);
   D( kprintf("app: CreateMenus returns menu =  %lx\n",menu); )
}
VOID handleCustomMenu(code)
UWORD code;
{
   struct MenuItem *item;
   BOOL terminated=FALSE;

   D( kprintf("app: handleCustomMenu(code=%lx)\n",code); )
   while((code!=MENUNULL)&&(!terminated))
   {
      item=ItemAddress(menu,code);
      switch((int)MENU_USERDATA(item))
      {
         case MENU_HIDE:
               shutdownWindow();
               terminated=TRUE; /* since window is gone NextSelect is invalid so...*/
               break;
         case MENU_DIE:
               terminate();
               break;
         default:
               break;
      }
      code=item->NextSelect;
      D( kprintf("app: handleCustomMenu next code=%lx\n",code); )
   }
   D( kprintf("app: handleCustomMenu exits"); )
}
VOID refreshWindow()
{
   if(window)
   {
      if(IDCMPRefresh)
         GT_BeginRefresh( window );

      SetAPen(window->RPort,1);
      SetBPen(window->RPort,2);
      SetDrMd(window->RPort,JAM2);
      SetFont(window->RPort,font);
      Move(window->RPort,50,topborder+40);
      Text(window->RPort,"Your Imagery Here",17);

      if(IDCMPRefresh)
         GT_EndRefresh( window, 1L );

      /* It is possible that the user has selected a font so large */
      /* that our imagery will fall off the bottom of the window   */
      /* we RefreshWindowFrame here incase our borders were overwritten */
      if((topborder+WINDOW_INNERHEIGHT) > window->Height)
         RefreshWindowFrame(window);
   }
   return;
}

#endif /* WINDOW */

BOOL setupCustomCX()
{
   return(setupNoCapsLock());
}
VOID shutdownCustomCX()
{
}
VOID handleCustomCXMsg(id)
ULONG id;
{
   switch(id)
   {
      case 0:
      default:
            break;
   }
}
VOID handleCustomCXCommand(id)
ULONG id;
{
   switch(id)
   {
      case 0:
      default:
            break;
   }
}
#if CSIGNAL
VOID handleCustomSignal(VOID)
{
}
#endif
