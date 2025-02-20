#include "rtgdpfs_factory.h"
//#include "rtgdpfs_AGA.h"
#include "rtgdpfs_OCS.h"
#include "rtgdpfs_P96.h"

struct rtg_dpf_screen* rtgdpfs_createBestImplementation()
{
    // todo test p96
   // return Create_dualplayfield_screen_P96();
    return Create_dualplayfield_screen_OCS();
}


