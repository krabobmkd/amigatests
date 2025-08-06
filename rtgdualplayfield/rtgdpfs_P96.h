#ifndef _RTGDPFSP96_H_
#define _RTGDPFSP96_H_

#include "rtgdpfs.h"

struct rtg_dpf_screen* Create_dualplayfield_screen_P96(int pf1width,int pf1height,int pf2width,int pf2height);

// return 1 if opened, else no p96 around.
int initP96();

#endif
