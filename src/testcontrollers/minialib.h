#ifndef _MINIALIB_H_
#define _MINIALIB_H_

#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/gadgetclass.h>


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

ULONG Do_MethodA( Object *obj, Msg message );
ULONG Do_Method( Object *obj, ULONG methodID, ... );
ULONG Do_SuperMethodA( struct IClass *cl, Object *obj, Msg message );
ULONG Do_SuperMethod( struct IClass *cl, Object *obj, ULONG methodID, ... );

#endif
