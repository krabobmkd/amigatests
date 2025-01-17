#ifndef _REACTIONINLINES_H_
#define _REACTIONINLINES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <proto/intuition.h>
#include <intuition/classes.h>

//struct Window;

static inline struct Window *reaction_OpenWindow(Object *owin) {
    return  (struct Window *)DoMethod(win, WM_OPEN, NULL);
}


#ifdef __cplusplus
}
#endif

#endif
