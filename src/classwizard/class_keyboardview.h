#ifndef GADGETS_KEYBOARDVIEW_H
#define GADGETS_KEYBOARDVIEW_H
/**
 * Definitions for Gadget KeyboardView
 * (This is the file that can be released)
 */
#include <exec/types.h>
#include <reaction/reaction.h>
#include <intuition/gadgetclass.h>
//#include <images/bevel.h>
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

/* Additional attributes defined by the gadget class.
 * We should also manages some attributes defined for super class gadget,
 * like GA_Disabled.
 */

#define KEYBOARDVIEW_Dummy			(TAG_USER+0x04000000)

#define	KEYBOARDVIEW_CenterX		(KEYBOARDVIEW_Dummy+1)

#define	KEYBOARDVIEW_CenterY		(KEYBOARDVIEW_Dummy+2)

/** DEVTODO: adds attributes definitions here and
 * manage them in class_keyboardview_attribs.c
 */
#endif
