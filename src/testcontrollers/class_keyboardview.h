#ifndef _CLASS_KEYBOARDVIEW_H_
#define _CLASS_KEYBOARDVIEW_H_
/**
 * This is the file that can be released
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>

#define VERSION_KEYBOARDVIEW 1

#define KeyboardView_SUPERCLASS_ID "gadgetclass"
#define KeyboardView_CLASS_ID      "krb.keyboardview"
#define KeyboardView_CLASS_LIBID   "krb.keyboardview.class"

#ifdef KEYBOARDVIEW_STATICLINK
// just use this one once to init class, will use a atexit() to close
void KeyboardView_static_class_init();
#endif

#ifdef __cplusplus
}
#endif

#endif
