#ifndef _CLASS_KEYBOARDVIEWPRIVATE_H_
#define _CLASS_KEYBOARDVIEWPRIVATE_H_

#include "class_keyboardview.h"

// not much sense because c++ static runtime are hard to link.
#ifdef __cplusplus
extern "C" {
#endif

#include <exec/types.h>
#include <exec/libraries.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include "graphics/regions.h"
#include "asmmacros.h"

#define USE_REGION_CLIPPING 1

/**
*  this is the internal private struct that own the data of the object instances.
* an important principle of boopsi is that structure for the class is hidden to the consumers.
* the consumers will only see the public header, and will do setAtribs()/GetAttribs()/Domethod()
* DEVTODO: make this class evolve to retain the data needed to draw and interact with your widget.
*/
typedef struct IKeyboardView {

    // let's say we have coordinates of the center of the circle
    UWORD _circleCenterX,_circleCenterY;

    ULONG _MouseMode;
    ULONG _EditMode;

#ifdef USE_REGION_CLIPPING
    struct Region *_clipRegion;
#endif
} KeyboardView;

ULONG F_SAVED KeyboardView_SetAttrs(Class *C, struct Gadget *Gad, struct opSet *Set);
ULONG F_SAVED KeyboardView_GetAttr(Class *C, struct Gadget *Gad, struct opGet *Get);
ULONG F_SAVED KeyboardView_Layout(Class *C, struct Gadget *Gad, struct gpLayout *layout);
ULONG F_SAVED KeyboardView_Render(Class *C, struct Gadget *Gad, struct gpRender *Render, ULONG update);
ULONG F_SAVED KeyboardView_HandleInput(Class *C, struct Gadget *Gad, struct gpInput *Input);
ULONG F_SAVED KeyboardView_Domain(Class *C, struct Gadget *Gad, struct gpDomain *D);

int F_SAVED KeyboardView_LibInit(REG(struct Library *LibBase,a6));
void F_SAVED KeyboardView_LibCleanup( REG(struct Library *LibBase,a6) );

// - - - - -- -

/** for dispatcher, very wise use of union.
 *  each  struct also starts with MethodID.
 * and they are the very parameters for each methods.
 */
typedef union MsgUnion
{
  ULONG  MethodID;
  // from classusr.h or gadgetclass.h, all starts with MethodID.
  struct opSet        opSet;
  struct opUpdate     opUpdate;
  struct opGet        opGet;
  struct gpHitTest    gpHitTest;
  struct gpRender     gpRender;
  struct gpInput      gpInput;
  struct gpGoInactive gpGoInactive;
  struct gpLayout     gpLayout;
} *Msgs;

ULONG KeyboardView_Notify(Class *C, struct Gadget *Gad, Msg M, ULONG Flags);

/** from classes.h, not present in SASC6.5 includes.
*/
#ifdef OLDINCLUDES
struct ClassLibrary
{
    struct Library	 cl_Lib;	/* Embedded library */
    UWORD		 cl_Pad;	/* Align the structure */
    Class		*cl_Class;	/* Class pointer */
};
#endif
/** this is the struct that is the extended struct Library
 * That is created with OpenLibrary().
 * But as it just manages a BOOPSI class there are just the open/close functions.
 * which themselves only manages registering the class with MakeClass()/AddClass()
 * versioning, and closing itself. This is *not* the boopsi class definition which is up there.
 * So it doesnt have to evolve, and can keep same name for each projects.
 *
 * must correspond to equivalent in classinit.s
 */
struct ExtClassLib
{
    // (see down there)   struct ClassLibrary cb_ClassLibrary;
    // because SASC6.5 have old includes with no struct ClassLibrary

    struct Library	 cl_Lib;	/* Embedded library */
    UWORD		 cl_Pad;	/* Align the structure */
    Class		*cl_Class;	/* Class pointer */

    APTR  cb_SysBase; // this is passed as LibInit
    APTR  cb_SegList; // this is passed at OpenLib and needed at expunge.
    // note: old libraries examples adds bases for graphics/intuition/utility after this
    // but C compiler will only search then in globals...
};

#ifdef __cplusplus
}
#endif

#endif
