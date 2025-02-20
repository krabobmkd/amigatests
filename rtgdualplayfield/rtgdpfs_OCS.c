#include "rtgdpfs_OCS.h"
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
//#include <proto/alib.h>
#include <intuition/screens.h>

#include <stdio.h>
#include <stdlib.h>

struct rtg_dpf_screen_ocs {

    struct rtg_dpf_screen _super;

    struct Screen *_screen;
    struct Screen *_win_backdrop;
    struct BitMap *_bm_pf1;
    struct BitMap *_bm_pf2;
};

static void ocs_close(struct rtg_dpf_screen_ocs *pthis)
{
    if(pthis) FreeVec(pthis);
}
static void ocs_copyBm(struct rtg_dpf_screen_ocs* pthis,int ipf,int x,int y,struct BitMap *bm)
{

}

static void ocs_setscroll(struct rtg_dpf_screen_ocs* pthis, int scrollx1, int scrolly1, int scrollx2, int scrolly2)
{

}



struct rtg_dpf_screen* Create_dualplayfield_screen_OCS()
{
    rtg_dpf_screen_ocs *pthis = AllocVec(sizeof(struct rtg_dpf_screen_ocs), MEMF_CLEAR);
    if(!pthis) return NULL;





    pthis->_super.close = ocs_close;
    pthis->_super.copyBm = ocs_copyBm;
    pthis->_super.setscroll = ocs_setscroll;

    return  &(pthis->_super);
}
