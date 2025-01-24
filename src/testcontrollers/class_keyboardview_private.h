#ifndef _CLASS_KEYBOARDVIEWPRIVATE_H_
#define _CLASS_KEYBOARDVIEWPRIVATE_H_

#include "class_keyboardview.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include "asmmacros.h"

ULONG F_SAVED KeyboardView_SetAttrs(Class *C, struct Gadget *Gad, struct opSet *Set);
ULONG F_SAVED KeyboardView_GetAttr(Class *C, struct Gadget *Gad, struct opGet *Get);
ULONG F_SAVED KeyboardView_Layout(Class *C, struct Gadget *Gad, struct gpLayout *layout);
ULONG F_SAVED KeyboardView_Render(Class *C, struct Gadget *Gad, struct gpRender *Render, ULONG update);
ULONG F_SAVED KeyboardView_HandleInput(Class *C, struct Gadget *Gad, struct gpInput *Input);
ULONG F_SAVED KeyboardView_Domain(Class *C, struct Gadget *Gad, struct gpDomain *D);

// - - - - -- -

// for dispatcher, very wise.
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




#ifdef __cplusplus
}
#endif

#endif
