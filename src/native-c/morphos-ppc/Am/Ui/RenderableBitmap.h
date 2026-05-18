#ifndef native_morphos_ppc_aclass_Am_Ui_RenderableBitmap_h
#define native_morphos_ppc_aclass_Am_Ui_RenderableBitmap_h

#include <libc/core.h>
#include <Am/Ui/RenderableBitmap.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>
#include <graphics/layers.h>

// Native backing data for RenderableBitmap on MorphOS — identical layout
// to the AmigaOS version (Intuition / layers / graphics APIs are shared).
typedef struct _Am_Ui_RenderableBitmap_data Am_Ui_RenderableBitmap_data;
struct _Am_Ui_RenderableBitmap_data {
    struct Layer_Info *layer_info;
    struct Layer *layer;
    struct RastPort rastport;
};

#endif
