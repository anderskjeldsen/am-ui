#ifndef native_amigaos_aclass_Am_Ui_LayerGraphics_h
#define native_amigaos_aclass_Am_Ui_LayerGraphics_h

#include <libc/core.h>
#include <Am/Ui/LayerGraphics.h>
#include <graphics/layers.h>

// Native data for LayerGraphics on AmigaOS.
typedef struct _Am_Ui_LayerGraphics_data Am_Ui_LayerGraphics_data;
struct _Am_Ui_LayerGraphics_data {
    struct Region *clip_region;
    struct RastPort *rastport;
    struct Layer *layer;
};

#endif
