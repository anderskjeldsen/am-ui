#ifndef native_amigaos_aclass_Am_Ui_LayerGraphics_h
#define native_amigaos_aclass_Am_Ui_LayerGraphics_h

#include <libc/core.h>
#include <Am/Ui/LayerGraphics.h>
#include <graphics/layers.h>

// Native data for LayerGraphics on AmigaOS.
//
// fg_color / bg_color + the matching _active flags hold the
// "direct RGB" mode setForegroundColor / setBackgroundColor put
// the graphics into. When _active is FALSE the rastport's
// APen / BPen are authoritative (the classic pen path); when
// TRUE the drawing primitives substitute the cached 24-bit
// ARGB via CGFX's FillPixelArray / WritePixelColor instead of
// going through the colormap — that's what lets us push more
// than the 16 palette pens the IDE reserved into a sprite
// editor.
typedef struct _Am_Ui_LayerGraphics_data Am_Ui_LayerGraphics_data;
struct _Am_Ui_LayerGraphics_data {
    struct Region *clip_region;
    struct RastPort *rastport;
    struct Layer *layer;
    ULONG fg_color;
    ULONG bg_color;
    BOOL  fg_color_active;
    BOOL  bg_color_active;
};

#endif
