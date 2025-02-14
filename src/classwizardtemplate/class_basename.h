#ifndef GADGETS_BASENAME_H
#define GADGETS_BASENAME_H
/**
 * Definitions for Gadget BaseName
 * (This is the file that can be released)
 */
#include <exec/types.h>
#include <intuition/gadgetclass.h>
#include <intuition/classes.h>

#define VERSION_BASENAME 1
#define BaseName_SUPERCLASS_ID "gadgetclass"

#ifdef BASENAME_STATICLINK
    extern int BaseName_static_class_init();
    extern void BaseName_static_class_close();
    extern struct IClass   *BaseNameClassPtr;
#else
    // BaseName_CLASS_ID is the identifier for this class, when shared.
    #define BaseName_CLASS_ID "basename.gadget"
#endif

/* Additional attributes defined by the gadget class.
 * We should also manages some attributes defined for super class gadget,
 * like GA_Disabled.
 */

#define BASENAME_Dummy			(TAG_USER+0x04110000)

#define	BASENAME_CenterX		(BASENAME_Dummy+1)

#define	BASENAME_CenterY		(BASENAME_Dummy+2)

/** DEVTODO: adds attributes definitions here and
 * manage them in class_basename_attribs.c
 */
#endif
