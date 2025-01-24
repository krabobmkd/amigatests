#ifndef _CLASS_KEYBOARDVIEW_H_
#define _CLASS_KEYBOARDVIEW_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <exec/types.h>
#include <intuition/classes.h>
//#include <intuition/classusr.h>

typedef struct IKeyboardView {
    ULONG _;
} KeyboardView;

#define KeyboardView_SUPERCLASS_ID "gadgetclass"
#define KeyboardView_CLASS_ID      "krb.keyboardview"

// allow static use
#define KEYBOARDVIEW_STATICLINK 1

#ifdef KEYBOARDVIEW_STATICLINK
// just use this one once to init class, will use a atexit() to close
void KeyboardView_static_class_init();
#endif

#ifdef __cplusplus
}
#endif

#endif
