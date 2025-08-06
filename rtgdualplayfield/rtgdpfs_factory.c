#include "rtgdpfs_factory.h"
//#include "rtgdpfs_AGA.h"
#include "rtgdpfs_OCS.h"
#include "rtgdpfs_P96.h"


struct rtg_dpf_screen* rtgdpfs_createBestImplementation(int pf1width,int pf1height,int pf2width,int pf2height)
{
    if(initP96())
    {
        struct rtg_dpf_screen *pscreen = Create_dualplayfield_screen_P96(pf1width,pf1height,pf2width,pf2height);
        if(pscreen) return pscreen; // else fallback aga
    }

    return Create_dualplayfield_screen_OCS(pf1width,pf1height,pf2width,pf2height);
}


