#ifndef _EXTRARASTPORT_H_
#define _EXTRARASTPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

struct BitMap;
struct RastPort;
struct Layer_Info;
struct Layer;

struct ExtraRastPort {
    struct RastPort *_rp;
    struct Layer_Info *_layerinfo;
    struct Layer *_layer;
};

struct ExtraRastPortAndBm {
    struct ExtraRastPort _erp;
    struct BitMap *_bm;
};

/**
    Create an extra offscreen, with Bitmap + Rastport + clip layer,
    so all drawing functions from graphics would work and be clipped.
    Bitmap is created with same format as friend, if friend NULL will be planar.
    erp must be inited with 0.
    return  ok 1 error 0.
*/
int OpenExtraRastPortAndBm(struct ExtraRastPortAndBm *erp,int width, int height,int depth, struct BitMap *friend);

/* close what was opened with OpenExtraRastPort() layers, rastport and bitmap.
 */
void CloseExtraRastPortAndBm(struct ExtraRastPortAndBm *erp);


/* version that use a pre-created bitmap */
int OpenExtraRastPort(struct ExtraRastPort *erp,int width, int height,struct BitMap *bm);

/* close what was opened with OpenExtraRastPort() rastport and layers, bitmap leaved unchanged.
 */
void CloseExtraRastPort(struct ExtraRastPort *erp);



#ifdef __cplusplus
}
#endif

#endif
