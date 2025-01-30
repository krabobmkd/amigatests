/** little boopsi/reaction example
 * that uses the gadget to test,
 * in a layout, with a button.
 * This will use a static link of the class definition
 * when XXXXX_STATICLINK is defined,
 * (which is usefull for debugging)
 * and will search the shared .class file
 * when this is not defined, which "is the way".
 */

#include <clib/alib_protos.h>
#include <clib/reaction_lib_protos.h>

#include <intuition/icclass.h>
//#include <libraries/gadtools.h>
#include <proto/diskfont.h>
#include <proto/exec.h>
#include <proto/graphics.h>
//#include <proto/icon.h>
#include <proto/intuition.h>
#include <exec/alerts.h>
//#include <proto/utility.h>

// too generic, include planet earth.
//#include <reaction/reaction.h>

#include <proto/window.h>
#include <classes/window.h>

#include <proto/layout.h>
#include <gadgets/layout.h>

#include <proto/button.h>
#include <gadgets/button.h>

// this is the public definition of the class we test:
#include "class_keyboardview.h"


// because original reaction macros are a nightmare
// and are not GCC compatible anyway.
#include "reactioninlines.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct IntuitionBase *IntuitionBase=NULL;
struct GfxBase *GfxBase=NULL;

// boopsi classes bases:
struct Library *WindowBase=NULL;
struct Library *LayoutBase=NULL;
struct Library *BitMapBase=NULL;
struct Library *ButtonBase=NULL;
struct Library *CheckBoxBase=NULL;

#ifndef KEYBOARDVIEW_STATICLINK
struct Library *KeyBoardViewBase=NULL;
#endif

//struct TextAttr helvetica15bu = { (STRPTR)"helvetica.font", 15, FSF_UNDERLINED | FSF_BOLD, FPF_DISKFONT };
//struct TextAttr garnet16 = { (STRPTR)"garnet.font", 16, 0, FPF_DISKFONT };

struct App
{
    struct Screen *screen;

    Object *window_obj; // object logic
    struct Window *win; // current reopened instance, as intuition Window.

    struct MsgPort *app_port;

    struct Screen *lockedscreen;

    Object *mainlayout;
        Object *keyboardlayout;
        Object *llpadslayout;
        Object *bottombarlayout;

        Object *testbt;
        Object *kbdview;
};

struct App *app=NULL;


Object *reaction_createLayout(int horiz,Object *childa,Object *childb,Object *childc,Object *childd)
{
    return NewObject( LAYOUT_GetClass(), NULL,
                LAYOUT_Orientation, horiz,
                LAYOUT_EvenSize, TRUE,
                LAYOUT_HorizAlignment, LALIGN_RIGHT,
                LAYOUT_SpaceInner, FALSE,
                LAYOUT_AddChild, childa,
                ((childb)?LAYOUT_AddChild:TAG_DONE),childb,
                ((childc)?LAYOUT_AddChild:TAG_DONE),childc,
                ((childd)?LAYOUT_AddChild:TAG_DONE),childd,
                TAG_DONE);
}

void cleanexit(const char *pmessage)
{
    if(pmessage) printf("%s\n",pmessage);
    exit(0);
}
void exitclose(void);

int main(int argc, char **argv)
{
    atexit(&exitclose);

    app = AllocVec(sizeof(struct App),MEMF_CLEAR);
    if(!app) return 1;

    if ( ! (IntuitionBase = (struct IntuitionBase*)OpenLibrary("intuition.library",33)))
        cleanexit("Can't open intuition.library");

    if ( ! (GfxBase = (struct GfxBase *)OpenLibrary("graphics.library",39)))
        cleanexit("Can't open graphics.library");

    if ( ! (WindowBase = OpenLibrary("window.class",44)))
        cleanexit("Can't open window.class");

    if ( ! (LayoutBase = OpenLibrary("gadgets/layout.gadget",44)))
        cleanexit("Can't open layout.gadget");

//    if ( ! (BitMapBase = OpenLibrary("images/bitmap.image",44)))
//        cleanexit("Can't open bitmap.image");

    if ( ! (ButtonBase = OpenLibrary("gadgets/button.gadget",44)))
        cleanexit("Can't open button.gadget");

#ifdef KEYBOARDVIEW_STATICLINK
    KeyboardView_static_class_init();
#else
    if ( ! (KeyBoardViewBase = OpenLibrary(
                "keyboardview.gadget"
                ,VERSION_KEYBOARDVIEW)))
        cleanexit("Can't open keyboardview.gadget");
#endif

//    if ( ! (CheckBoxBase = OpenLibrary("gadget/checkbox.gadget",44)))
//        cleanexit("Can't open checkbox.gadget");

    // = = = = = now that needed classes arte loaded
    // = = = = = creates the instances...

    app->lockedscreen = LockPubScreen(NULL);
    if (!app->lockedscreen) cleanexit("Can't lock screen");

    app->testbt = NewObject( NULL, "button.gadget",
                              //      GA_TextAttr, &garnet16,
                                    GA_Text, "B_ig Button",
                                    GA_RelVerify, TRUE, // needed
                                TAG_END);
    if(!app->testbt) cleanexit("Can't button");

    app->kbdview = NewObject( NULL, KeyboardView_CLASS_ID,
                              //      GA_TextAttr, &garnet16,
                               //     GA_Text, "B_ig Button",
                               //     GA_RelVerify, TRUE, // needed
                                TAG_END);
    if(!app->kbdview) cleanexit("Can't kbdview");


    app->keyboardlayout = reaction_createLayout(LAYOUT_ORIENT_HORIZ,
                    app->testbt,
                    app->kbdview,NULL,NULL);
    if(!app->keyboardlayout) cleanexit("Can't layout");

    {
     //   struct DrawInfo *drinfo = GetScreenDrawInfo(screen);
        app->mainlayout = NewObject( LAYOUT_GetClass(), NULL,
     //       GA_DrawInfo, drinfo,
            LAYOUT_DeferLayout, TRUE, /* Layout refreshes done on task's context (by thewindow class) */
            LAYOUT_SpaceOuter, TRUE,
            LAYOUT_BottomSpacing, 4,
            LAYOUT_HorizAlignment, LALIGN_RIGHT,
            LAYOUT_Orientation, LAYOUT_ORIENT_VERT,
            LAYOUT_AddChild, app->keyboardlayout,
            TAG_END);
        if (!app->mainlayout) cleanexit("layout class error");
    } //end if screen


    app->app_port = CreateMsgPort();

    /* Create the window object. */
    app->window_obj = NewObject( WINDOW_GetClass(), NULL,
        WA_Left, 0,
        WA_Top, app->lockedscreen->Font->ta_YSize + 3,
        WA_CustomScreen, app->lockedscreen,
        WA_IDCMP, IDCMP_CLOSEWINDOW,
        WA_Flags, WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_SIZEGADGET | WFLG_ACTIVATE | WFLG_SMART_REFRESH,
        WA_Title, "Inputs Checkup",
        WINDOW_ParentGroup, app->mainlayout,
        WINDOW_IconifyGadget, TRUE,
        WINDOW_Icon, GetDiskObject("PROGDIR:ReAction"),
        WINDOW_IconTitle, "ReAction Demo",
        WINDOW_AppPort, app->app_port,
    TAG_END);
    if(!app->window_obj) cleanexit("can't create window");


    /*  Open the window. */
    app->win = reaction_OpenWindow(app->window_obj);
    if(!app->win) cleanexit("can't open window");

    {
        ULONG signal;
        BOOL ok = TRUE;

        /* Obtain the window wait signal mask.*/
        GetAttr(WINDOW_SigMask, app->window_obj, &signal);

        /* Input Event Loop */
        while (ok)
        {
            ULONG result;

            Wait(signal | (1L << app->app_port->mp_SigBit));

            /* CA_HandleInput() returns the gadget ID of a clicked
             * gadget, or one of several pre-defined values.  For
             * this demo, we're only actually interested in a
             * close window and a couple of gadget clicks.
             */
            while ((result = DoMethod(app->window_obj, WM_HANDLEINPUT, /*code*/NULL)) != WMHI_LASTMSG)
            {

                switch(result & WMHI_CLASSMASK)
                {
                    case WMHI_CLOSEWINDOW:
                        ok = FALSE;
                        break;

                    case WMHI_GADGETUP:
 /*                       switch (result & WMHI_GADGETMASK)
                        {
                            case GAD_FORWARD:
                                GetAttr(PAGE_Current, page, &current_page);
                                if (current_page < NUM_PAGES)
                                {
                                    if (current_page == 0)
                                        if (SetGadgetAttrs(back_gad, win, NULL, GA_Disabled, FALSE, TAG_DONE))
                                            RefreshGList(layout2, win, NULL, 1);

                                    current_page++;

                                    if (current_page == NUM_PAGES)
                                        if (SetGadgetAttrs(forward_gad, win, NULL, GA_Disabled, TRUE, TAG_DONE))
                                            RefreshGList(layout2, win, NULL, 1);

                                    SetGadgetAttrs(page, win, NULL, PAGE_Current, current_page, TAG_DONE);
                                    RethinkLayout(layout1, win, NULL, TRUE);
                                }
                                break;

                            case GAD_GETFILE:
                                DoMethod((Object *)getfile_gad , GFILE_REQUEST , win);
                                break;

                            case GAD_GETFONT:
                                DoMethod((Object *)getfont_gad , GFONT_REQUEST , win);
                                break;

                            case GAD_GETSCREEN:
                                RequestScreenMode((Object *)getscreen_gad, win);
                                break;

                            case GAD_BACK:
                                GetAttr(PAGE_Current, page, &current_page);
                                if (current_page > 0)
                                {
                                    if (current_page == NUM_PAGES)
                                        if (SetGadgetAttrs(forward_gad, win, NULL, GA_Disabled, FALSE, TAG_DONE))
                                            RefreshGList(layout2, win, NULL, 1);

                                    current_page--;

                                    if (current_page == 0)
                                        if (SetGadgetAttrs(back_gad, win, NULL, GA_Disabled, TRUE, TAG_DONE))
                                            RefreshGList(layout2, win, NULL, 1);

                                    SetGadgetAttrs(page, win, NULL, PAGE_Current, current_page, TAG_DONE);
                                    RethinkLayout(layout1, win, NULL, TRUE);
                                }
                                break;

                            case GAD_QUIT:
                                ok = FALSE;
                                break;

                            default:
                                break;
                        }*/
                        break;

                    case WMHI_ICONIFY:
                        //if (RA_Iconify(window_obj)) win = NULL;
                        if(DoMethod(app->window_obj, WM_ICONIFY, NULL)) app->win = NULL;
                        break;

                    case WMHI_UNICONIFY:
                        app->win = reaction_OpenWindow(app->window_obj);
                        if (!app->win) cleanexit("can't open window");

                        break;

                    default:
                        break;
                }


            } // end while messages
        } // end while app loop
    } // loop paragraph end

    // all close done in exitclose().
    return 0;
}


void exitclose(void)
{
    if(app)
    {
        /* Disposing of the window object will also close the
         * window if it is already opened and it will dispose of
         * all objects attached to it.
         */
        if(app->window_obj) DisposeObject(app->window_obj);
        else {
            // but if not attached because mid-init fail, has to be manual.
            if(app->mainlayout)  DisposeObject(app->mainlayout);
            else {
                if(app->testbt) DisposeObject(app->testbt);
                if(app->kbdview) DisposeObject(app->kbdview);
            }
        }
        if(app->lockedscreen) UnlockPubScreen(0, app->lockedscreen);

        FreeVec(app);
    }


#ifndef KEYBOARDVIEW_STATICLINK
    if(KeyBoardViewBase) CloseLibrary(KeyBoardViewBase);
#endif
    if(CheckBoxBase) CloseLibrary(CheckBoxBase);
    if(ButtonBase) CloseLibrary(ButtonBase);
    if(BitMapBase) CloseLibrary(BitMapBase);
    if(LayoutBase) CloseLibrary(LayoutBase);
    if(WindowBase) CloseLibrary(WindowBase);

    if(GfxBase) CloseLibrary((struct Library*)GfxBase);
    if(IntuitionBase) CloseLibrary((struct Library*)IntuitionBase);

}

