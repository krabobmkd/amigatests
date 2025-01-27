
#include <exec/types.h>
#include <exec/libraries.h>
#include <exec/initializers.h>
#include <exec/resident.h>




#include <intuition/classes.h>

//#include <libraries/dos.h"
//#include "utility/hooks.h"
//#include "led_rev.i"
//#include "classbase.i"


struct ClassLib
{
    // extend this
    struct ClassLibrary cb_ClassLibrary;
    UWORD cb_pad;
    ULONG cb_SysBase;
    //ULONG cb_UtilityBase;
	ULONG	cb_SegList; // must be kept for expunge

};

/*
	STRUCT	cb_ClassLibrary,ClassLibrary_SIZEOF
        UWORD	cb_Pad
	ULONG	cb_SysBase
	ULONG   cb_UtilityBase
	ULONG	cb_IntuitionBase
	ULONG	cb_GfxBase
	ULONG	cb_SegList
*/

int __attribute__((no_reorder)) _start()
{
        return -1;
}
extern void EndCode(void);
extern const char libName[];
extern const char libIdString[];
extern const uint32_t InitTable[];
/*
               ;STRUCTURE RT,0
     DC.W    RTC_MATCHWORD      ; UWORD RT_MATCHWORD
     DC.L    RomTag             ; APTR  RT_MATCHTAG
     DC.L    EndCode            ; APTR  RT_ENDSKIP
     DC.B    RTF_AUTOINIT       ; UBYTE RT_FLAGS
     DC.B    VERSION            ; UBYTE RT_VERSION  (defined in sample_rev.i)
     DC.B    NT_LIBRARY         ; UBYTE RT_TYPE
     DC.B    MYPRI              ; BYTE  RT_PRI
     DC.L    LibName            ; APTR  RT_NAME
     DC.L    IDString           ; APTR  RT_IDSTRING
     DC.L    InitTable          ; APTR  RT_INIT  table for InitResident()
*/
#define VERSION_KEYBOARDVIEW 1

const struct Resident RomTag __attribute__((used)) = {
    RTC_MATCHWORD, // w
    (struct Resident *)&RomTag, // l
    (APTR)&EndCode,        // hard to link, must be in same section.
    RTF_AUTOINIT,          // UBYTE RT_FLAGS
    VERSION_KEYBOARDVIEW,  // UBYTE
    1,                     // UBYTE  version
    0,                     // UBYTE  PRIORITY
    (char *)((intptr_t)&libName), // l
    (char *)((intptr_t)&libIdString), // l
    (APTR)InitTable, // l
};

const char libName[] = KeyboardView_CLASS_LIBID;
const char libIdString[] = "1.0";


// any ".class" must have this list of function.
static ULONG LibFuncTable[] = {
    (ULONG)OpenLib,     // would init class if 0 then +1
    (ULONG)CloseLib,   // would -1 and close class if 0
    (ULONG)ExpungeLib,
    NULL // reserved
};

const ULONG InitTable[4] = {
    sizeof(struct ClassLib),
    (uint32_t)LibFuncTable,
    0,
    (uint32_t)LibInit
};
/*

; Library Init entry point called when library is first loaded in memory
; On entry, D0 points to library base, A0 has lib seglist, A6 has SysBase
; Returns 0 for failure or the library base for success.
LibInit:
        movem.l a0/a5/d7,-(sp)
        move.l  d0,a5
        move.l  a6,cb_SysBase(a5)
        move.l  a0,cb_SegList(a5)

        move.w  #REVISION,LIB_REVISION(a5)

        move.l  #AO_GraphicsLib,d7
        lea     GfxName(pc),a1
        bsr.s   OpenLib
        move.l  d0,cb_GfxBase(a5)

        move.l  #AO_Intuition,d7
        lea     IntuitionName(pc),a1
        bsr.s   OpenLib
        move.l  d0,cb_IntuitionBase(a5)

        move.l  #AO_UtilityLib,d7
        lea     UtilityName(pc),a1
        bsr.s   OpenLib
        move.l  d0,cb_UtilityBase(a5)

        move.l  a5,d0
        movem.l (sp)+,a0/a5/d7
        rts

*/
static struct Library *lib LibInit(
            REG(struct Library *lib,d0),
             REG(struct SegList *seglist,a0),
             REG(struct ExecBase *exec,a6)
            )
{

}

/*
; Library open entry point called every OpenLibrary()
; On entry, A6 has ClassBase, task switching is disabled
; Returns 0 for failure, or ClassBase for success.
LibOpen:
	tst.w	LIB_OPENCNT(a6)
	bne.s	1$

	bsr	_CreateClass
	tst.w	d0
	bne.s	1$

	moveq	#0,d0
	rts

1$:
        addq.w  #1,LIB_OPENCNT(a6)
        bclr    #LIBB_DELEXP,LIB_FLAGS(a6)
        move.l  a6,d0
        rts
*/

/*
 Library open entry point called every OpenLibrary()
 On entry, A6 has ClassBase, task switching is disabled
 Returns 0 for failure, or ClassBase for success.
*/
static struct Library *lib LibOpen(REG(struct Library *lib,a6))
{
    if(lib->lib_OpenCnt == 0)
    {   // 0 or 1
        int res = KeyboardView_LibInit(lib);
        if(res==0) return NULL;
    }
    lib->lib_OpenCnt++;
    lib->lib_Flags &= ~LIBF_DELEXP;  // not delayed expunge
    return lib;
}
/*
; Library close entry point called every CloseLibrary()
; On entry, A6 has ClassBase, task switching is disabled
; Returns 0 normally, or the library seglist when lib should be expunged
;   due to delayed expunge bit being set
LibClose:
	subq.w	#1,LIB_OPENCNT(a6)
	bne.s	1$			; if openers, don't remove class

	bsr	_DestroyClass		; zero openers, so try to remove class

1$:
	; if delayed expunge bit set, then try to get rid of the library
	btst	#LIBB_DELEXP,LIB_FLAGS(a6)
	bne.s	CloseExpunge

	; delayed expunge not set, so stick around
	moveq	#0,d0
	rts

CloseExpunge:
	; if no library users, then just remove the library
	tst.w	LIB_OPENCNT(a6)
	beq.s	DoExpunge

	; still some library users, so forget about flushing
	bclr	#LIBB_DELEXP,LIB_FLAGS(a6)
	moveq	#0,d0
	rts
*/

static void toolDoExpunge(REG(struct Library *lib,a6))
{
    // seglist remove
    //closelibs
}

static int LibClose(REG(struct Library *lib,a6))
{
    lib->lib_OpenCnt--;
    if(lib->lib_OpenCnt==0)
    {
        KeyboardView_LibCleanup(lib);
    }
    if(lib->lib_Flags & LIBF_DELEXP)
    {
        // CloseExpunge
       toolDoExpunge(lib);
       return 0;
    }
    lib->lib_Flags &= ~LIBF_DELEXP;
    return 0;
}

/* BOOPSI class libraries should use this structure as the base for their
 * library data.  This allows developers to obtain the class pointer for
 * performing object-less inquiries. */
//struct ClassLibrary
//{
//    struct Library	 cl_Lib;	/* Embedded library */
//    UWORD		 cl_Pad;	/* Align the structure */
//    Class		*cl_Class;	/* Class pointer */

//};
/**
 Library expunge entry point called whenever system memory is lacking
 On entry, A6 has ClassBase, task switching is disabled
 Returns the library seglist if the library open count is 0, returns 0
 otherwise and sets the delayed expunge bit.
 */
// return type of cb_SegList
static int LibExpunge(REG(struct ClassLibrary *lib,a6))
{
    if(lib->cl_Lib.lib_OpenCnt==0 || lib->cl_Class == NULL)
    {
        return (int)toolDoExpunge(lib);
    } else
    {
        lib->cl_Lib.lib_Flags |= LIBF_DELEXP;
        return 0;
    }

// move.l  cb_SegList(a5),d2
// TODO
}

/*
; Library expunge entry point called whenever system memory is lacking
; On entry, A6 has ClassBase, task switching is disabled
; Returns the library seglist if the library open count is 0, returns 0
; otherwise and sets the delayed expunge bit.
LibExpunge:
        tst.w   LIB_OPENCNT(a6)
        beq.s   DoExpunge

        tst.l   cl_Class(a6)
        beq.s   DoExpunge

        bset    #LIBB_DELEXP,LIB_FLAGS(a6)
        moveq   #0,d0
        rts

DoExpunge:
        movem.l d2/a5/a6,-(sp)
        move.l  a6,a5
        move.l  cb_SegList(a5),d2

        move.l  a5,a1
        REMOVE

        move.l  cb_SysBase(a5),a6
        bsr.s   CloseLibs

        move.l  a5,a1
        moveq   #0,d0
        move.w  LIB_NEGSIZE(a5),d0
        sub.l   d0,a1
        add.w   LIB_POSSIZE(a5),d0
        CALL    FreeMem

        move.l  d2,d0
        movem.l (sp)+,d2/a5/a6
        rts
*/
