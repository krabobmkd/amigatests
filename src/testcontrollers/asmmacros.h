#ifndef _ASMMACROS_H_
#define _ASMMACROS_H_

#if defined(__AMIGA__)
    #if defined(__GNUC__)
        #define REG(def,reg) def __asm(#reg)
        #define F_SAVED
        #define F_ASM
    #elif defined(__SASC__)
    // register __a6 struct Library *LibBase
        #define REG(def,reg) register __reg def
        #define F_SAVED __saveds __asm
        #define F_ASM __asm
    #else
        #define REG(def,reg) def
        #define F_SAVED
        #define F_ASM
    #endif
#else
    #define REG(def,reg) def
    #define F_SAVED
    #define F_ASM
#endif

#endif
