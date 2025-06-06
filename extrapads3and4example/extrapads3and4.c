
   /***********************************************************************
   *                                                                      *
   *                            COPYRIGHTS                                *
   *                                                                      *
   *   Copyright (c) 1990  Commodore-Amiga, Inc.  All Rights Reserved.    *
   *                                                                      *
   ***********************************************************************/

#include "app.h"
#include <devices/inputevent.h>
//#include <clib/commodities_protos.h>
/* An input expression to match any RAWKEY event with the
 * CAPSLOCK qualifier bit set
 */
IX myix = {
   IX_VERSION,             /* required                           */
   IECLASS_RAWKEY,

   0,                      /* Code: won't care                     */
   0,                      /* CodeMask: 0 means don't care about Code   */

   IEQUALIFIER_CAPSLOCK,   /* qualifier I am interested in            */
   IEQUALIFIER_CAPSLOCK,   /* and it's the only qualifier of interest   */
   0                       /* synonyms irrelevant                  */
};

BOOL setupNoCapsLock(void);
void middlebaction(CxMsg *,CxObj *);
void nocapsaction(CxMsg *cxm,CxObj *co);
BOOL setupNoCapsLock()
{
   char   **tt;
   int      exitval = 0;

   CxObj   *filter;

   filter = CxFilter(NULL);
   if (!filter)
      return(0);

   SetFilterIX(filter, &myix);

   /** DEBUG **/
   D( AttachCxObj(filter, CxDebug( 0xDEADFACE)) );

   AttachCxObj(filter, CxCustom( nocapsaction, 0L));

   if (CxObjError(filter))
   {
      D( printf("nocapslock: filter error %lx\n", CxObjError(filter) ) );
      DeleteCxObjAll(filter);
      return(0);
   }

   AttachCxObj(broker, filter);
   return(TRUE);
}
void nocapsaction(CxMsg *cxm,CxObj *co)
{
   register struct InputEvent *ie;

   D( kprintf("nocapsaction\n") );

   /* i KNOW that all messages getting this far are CXM_IEVENT   */
   ie = (struct InputEvent *) CxMsgData(cxm);

   ie->ie_Qualifier &= ~IEQUALIFIER_CAPSLOCK;
}
