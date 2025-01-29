#ifndef _MINIALIB_H_
#define _MINIALIB_H_
/** short inline replacement for proto/alib.h (amiga tool static lib)
 * To be used when linking .class file, because gcc6.5 linker
 * in all cases this is always short inlining, so not a bad idea I guess.
 */
#include "asmmacros.h"

#include <proto/utility.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>
#include <utility/hooks.h>

// ULONG  __stdargs CallHookA( struct Hook *hookPtr, Object *obj, APTR message );
// ULONG  __stdargs CallHook( struct Hook *hookPtr, Object *obj, ... );
// ULONG  __stdargs DoMethodA( Object *obj, Msg message );
// ULONG  __stdargs DoMethod( Object *obj, ULONG methodID, ... );
// ULONG  __stdargs DoSuperMethodA( struct IClass *cl, Object *obj, Msg message );
// ULONG  __stdargs DoSuperMethod( struct IClass *cl, Object *obj, ULONG methodID, ... );
// ULONG  __stdargs CoerceMethodA( struct IClass *cl, Object *obj, Msg message );
// ULONG  __stdargs CoerceMethod( struct IClass *cl, Object *obj, ULONG methodID, ... );
// ULONG  __stdargs HookEntry( struct Hook *hookPtr, Object *obj, APTR message );
// ULONG  __stdargs SetSuperAttrs( struct IClass *cl, Object *obj, ULONG tag1, ... );
#if 0
ULONG DoMethodA( Object *obj, Msg message );
ULONG DoMethod( Object *obj, ULONG methodID, ... );
ULONG DoSuperMethodA( struct IClass *cl, Object *obj, Msg message );
ULONG DoSuperMethod( struct IClass *cl, Object *obj, ULONG methodID, ... );
#else
//https://github.com/aros-development-team/AROS/blob/master/compiler/alib/domethod.c
/*
ULONG  __stdargs CallHookPkt( struct Hook *hook, APTR object, APTR paramPacket );
#define CallHookPkt(___hook, ___object, ___paramPacket) \
      LP3(0x66, ULONG, CallHookPkt , struct Hook *, ___hook, a0, APTR, ___object, a2, APTR, ___paramPacket, a1,\
      , UTILITY_BASE_NAME)
*/
//static inline ULONG CallHookPktASM( REG(struct Hook *hook,a0),
//                                     REG(APTR object,a2),
//                                     REG(APTR paramPacket,a1 ))
//{
//    ULONG r;
//	move.l	a1,-(sp)
//	move.l	a2,-(sp)
//	move.l	a0,-(sp)
//	move.l	(h_SubEntry,a0),a0
//	jsr	(a0)
//	lea	(12,sp),sp
//	return r;
//}
// CallHookPkt is rom utilitybase...
extern struct Library *UtilityBase;
static inline ULONG DoMethodA( Object *obj, Msg message ) {
    //   if (!obj || !message) return 0L;
   return CallHookPkt((struct Hook *) OCLASS(obj), obj, message);
}
static inline ULONG DoMethod( Object *obj, ULONG methodID, ... ) {
    //   if (!obj) return 0L;
   return CallHookPkt((struct Hook *) OCLASS(obj), obj, (APTR)&methodID);
}
static inline ULONG DoSuperMethodA( struct IClass *cl, Object *obj, Msg message ) {
   return CallHookPkt((struct Hook *)cl->cl_Super, obj, message);
}
static inline ULONG DoSuperMethod( struct IClass *cl, Object *obj, ULONG methodID, ... ) {
   return CallHookPkt((struct Hook *)cl->cl_Super, obj, (APTR)&methodID);
}

#endif
#endif
