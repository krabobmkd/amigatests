#ifndef GADGETS_KEYBOARDVIEW_H
#define GADGETS_KEYBOARDVIEW_H
/**
 * Definitions for Gadget KeyboardView
 * (This is the file that can be released)
 */
#include <exec/types.h>
#include <intuition/gadgetclass.h>
#include <intuition/classes.h>

#define VERSION_KEYBOARDVIEW 1
#define KeyboardView_SUPERCLASS_ID "gadgetclass"

#ifdef KEYBOARDVIEW_STATICLINK
    extern int KeyboardView_static_class_init();
    extern void KeyboardView_static_class_close();
    extern struct IClass   *KeyboardViewClassPtr;
#else
    // KeyboardView_CLASS_ID is the identifier for this class, when shared.
    #define KeyboardView_CLASS_ID "keyboardview.gadget"
#endif

/* Additional attributes defined by the gadget class.
 * We should also manages some attributes defined for super class gadget,
 * like GA_Disabled.
 */

#define KEYBOARDVIEW_Dummy			(TAG_USER+0x04110000)

#define	KEYBOARDVIEW_CenterX		(KEYBOARDVIEW_Dummy+1)

#define	KEYBOARDVIEW_CenterY		(KEYBOARDVIEW_Dummy+2)

/** DEVTODO: adds attributes definitions here and
 * manage them in class_keyboardview_attribs.c
 */
#endif
