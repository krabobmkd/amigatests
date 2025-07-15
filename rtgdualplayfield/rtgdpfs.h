#ifndef _RTGDPFS_H_
#define _RTGDPFS_H_

#include <exec/types.h>

struct MsgPort;
struct Screen;
struct RastPort;

/** abstract interface to implement some sort of "Dual Playfield" (dpf),
 either for native Amiga Chip, or RTG drivers
*/
struct rtg_dpf_screen {
//replaced by create_xxx:	int (*open)(struct rtgdpfs *pthis,int pf1bmw, int pf1bmh, int pf2bmw, int pf2bmh);
	void (*close)(struct rtg_dpf_screen* pthis);
//void (*copyBm)(struct rtg_dpf_screen* pthis,int ipf,int x,int y,struct BitMap *bm);

    // screen may have been opened for 8 colors for pf1, 8 colors for pf2,
    // or 16 + 16 if AGA.
    // nbcolors max 8 or 16.
    // iPlayfield 0 pf1 1 pf2
	void (*setPalette)(struct rtg_dpf_screen* pthis,const UBYTE *ppalette, int nbcolors, int iPlayfield);

    // change scroll position of both playfield. would use graphics ScrollVPort().
	void (*setscroll)(struct rtg_dpf_screen* pthis, WORD scrollx1, WORD scrolly1, WORD scrollx2, WORD scrolly2);

    // port of the main screen window, to get keyboard and mouse events.
	struct MsgPort *_userPort;

    // the intuition screen, containing the 2 playfields. use _pf1rp instead of _screen->RastPort, to allow the rtg trick.
	struct Screen *_screen;

    // playfields as regular RastPort, usable with graphics draw functions, clipping and layers done the right way.
    // note ->BitMap will always be in planar mode even for RTG, to allow the rtg trick.
	struct RastPort *_pf1rp;
	struct RastPort *_pf2rp;

};


#endif
