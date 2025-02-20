#ifndef _RTGDPFS_H_
#define _RTGDPFS_H_

#include <exec/types.h>

struct BitMap;
struct UserPort;

// abstract interface to be implemented
struct rtg_dpf_screen {
//replaced by create_xxx:	int (*open)(struct rtgdpfs *pthis,int pf1bmw, int pf1bmh, int pf2bmw, int pf2bmh);
	void (*close)(struct rtg_dpf_screen* pthis);
	void (*copyBm)(struct rtg_dpf_screen* pthis,int ipf,int x,int y,struct BitMap *bm);

	void (*setscroll)(struct rtg_dpf_screen* pthis, int scrollx1, int scrolly1, int scrollx2, int scrolly2);

	struct UserPort *userPort;
};


#endif
