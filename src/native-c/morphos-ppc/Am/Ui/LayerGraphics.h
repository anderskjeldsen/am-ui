#ifndef native_morphos_ppc_aclass_Am_Ui_LayerGraphics_h
#define native_morphos_ppc_aclass_Am_Ui_LayerGraphics_h

#include <libc/core.h>
#include <Am/Ui/LayerGraphics.h>
#include <graphics/layers.h>

// Native data for LayerGraphics on MorphOS. Mirrors the AmigaOS layout
// since the Intuition / graphics / layers / cybergraphx APIs are shared.
typedef struct _Am_Ui_LayerGraphics_data Am_Ui_LayerGraphics_data;
struct _Am_Ui_LayerGraphics_data {
    struct Region *clip_region;
    struct RastPort *rastport;
    struct Layer *layer;
};

#endif
