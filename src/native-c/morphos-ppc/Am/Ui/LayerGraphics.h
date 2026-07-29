#ifndef native_morphos_ppc_aclass_Am_Ui_LayerGraphics_h
#define native_morphos_ppc_aclass_Am_Ui_LayerGraphics_h

#include <libc/core.h>
#include <Am/Ui/LayerGraphics.h>
#include <graphics/layers.h>

// Native data for LayerGraphics on MorphOS. Mirrors the AmigaOS layout
// since the Intuition / graphics / layers / cybergraphx APIs are shared —
// including the fg_color / bg_color "direct RGB" mode flipped on by
// setForegroundColor / setBackgroundColor (see the amigaos sibling for
// the full rationale).
typedef struct _Am_Ui_LayerGraphics_data Am_Ui_LayerGraphics_data;
struct _Am_Ui_LayerGraphics_data {
    struct Region *clip_region;
    struct RastPort *rastport;
    struct Layer *layer;
    // ViewPort of the bound window's screen. setForegroundColor /
    // setBackgroundColor program the scratch pens (255 fg / 254 bg)
    // into its colormap via SetRGB32 so pen-based primitives
    // (Text, Draw) render the requested colour too. NULL when the
    // target is an off-screen RenderableBitmap.
    struct ViewPort *screen_vp;
    ULONG fg_color;
    ULONG bg_color;
    BOOL  fg_color_active;
    BOOL  bg_color_active;
};

#endif
