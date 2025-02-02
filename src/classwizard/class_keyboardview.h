#ifndef _CLASS_KEYBOARDVIEW_H_
#define _CLASS_KEYBOARDVIEW_H_
/**
 * This is the file that can be released
 */

#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>

#define VERSION_KEYBOARDVIEW 1

#define KeyboardView_SUPERCLASS_ID "gadgetclass"
#define KeyboardView_CLASS_ID_Base "keyboardview.gadget"

// KeyboardView_CLASS_ID is the identifier for this class.

#ifdef KEYBOARDVIEW_STATICLINK
    // just use this one once to init class, will use a atexit() to close
    extern char KeyboardView_CLASS_ID_Static[];
    #define KeyboardView_CLASS_ID     KeyboardView_CLASS_ID_Static
    extern void KeyboardView_static_class_init(void);
#else
    #define KeyboardView_CLASS_ID KeyboardView_CLASS_ID_Base
#endif

#endif
