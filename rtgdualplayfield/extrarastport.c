#include "extrarastport.h"

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/layers.h>

void CloseExtraRastPort(struct ExtraRastPort *erp)
{
    if(!erp) return;
    if(erp->_layer) DeleteLayer (0,erp->_layer);
    erp->_layer = NULL;

    if(erp->_layerinfo) DisposeLayerInfo(erp->_layerinfo);
    erp->_layerinfo = NULL;
    erp->_rp = NULL;


}

int OpenExtraRastPort(struct ExtraRastPort *erp,int width, int height,struct BitMap *bm)
{
    if(!erp) return 0;

    erp->_layerinfo = NewLayerInfo();
    if(!erp->_layerinfo) {  CloseExtraRastPort(erp); return 0; }

    erp->_layer = CreateUpfrontLayer(erp->_layerinfo, bm, 0, 0, width - 1, height - 1, 0, NULL);
    if(!erp->_layer) {  CloseExtraRastPort(erp); return 0; }

    erp->_rp = erp->_layer->rp;
    //ok
    return 1;
}
void CloseExtraRastPortAndBm(struct ExtraRastPortAndBm *erp)
{
    if(!erp) return;
    CloseExtraRastPort(&erp->_erp);
    if(erp->_bm) FreeBitMap(erp->_bm);
    erp->_bm = NULL;
}
int OpenExtraRastPortAndBm(struct ExtraRastPortAndBm *erp,int width, int height,int depth, struct BitMap *friend)
{
    if(!erp) return 0;
    erp->_bm = AllocBitMap(width,height,depth,BMF_CLEAR,friend);
    if(!erp->_bm) return 0;

    return OpenExtraRastPort(&erp->_erp,width,height,erp->_bm);
}
