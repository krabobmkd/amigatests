#ifndef GADGETS_BASENAME_H
#define GADGETS_BASENAME_H
/**
 * Definitions for Gadget BaseName
 * (This is the file that can be released)
 */
#include <exec/types.h>
#include <intuition/gadgetclass.h>
#include <intuition/classes.h>
            #include <pragmas/button_pragmas.h>
#define VERSION_BASENAME 1
#define BaseName_SUPERCLASS_ID "gadgetclass"

#ifdef BASENAME_STATICLINK
    extern int BaseName_static_class_init();
    extern void BaseName_static_class_close();
    extern Class *BASENAME_GetClass();
#else
    // BaseName_CLASS_ID is the identifier for this class, when shared.
    // NewObject() can use either BASENAME_GetClass() or BaseName_CLASS_ID.
    #define BaseName_CLASS_ID "basename.gadget"

    // the following is to define function with implicit library call for BASENAME_GetClass().
    // note it should be in includes generated from a fd files.
    Class * __stdargs BASENAME_GetClass( void );

    #ifndef _NO_INLINE
        # if defined(__GNUC__)
            #include <inline/macros.h>
            #define BASENAME_GetClass() LP0(0x1e, Class *, BASENAME_GetClass ,, BaseNameBase)
        # else
            // sasc
           #pragma libcall BaseNameBase BASENAME_GetClass 1e 00
        # endif
    #endif /* _NO_INLINE */

    #ifndef __NOLIBBASE__
      extern struct Library *
        # ifdef __CONSTLIBBASEDECL__
       __CONSTLIBBASEDECL__
        # endif /* __CONSTLIBBASEDECL__ */
      BaseNameBase;
    #endif /* !__NOLIBBASE__ */

// end if dynamic link
#endif

/**  Attributes defined by the gadget class,
 * all attribs from gadgetclass.h are also valid.
 */
// different classes may not use same base.
#define BASENAME_Dummy			(TAG_USER+0x04110000)

// coordinate from 0 to 65535
#define	BASENAME_CenterX		(BASENAME_Dummy+1)
// coordinate from 0 to 65535
#define	BASENAME_CenterY		(BASENAME_Dummy+2)

/** DEVTODO: adds attributes definitions here and
 * manage them in class_basename_attribs.c
 */
#endif
