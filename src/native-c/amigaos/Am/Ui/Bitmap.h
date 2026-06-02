#ifndef native_amigaos_aclass_Am_Ui_Bitmap_h
#define native_amigaos_aclass_Am_Ui_Bitmap_h
#include <libc/core.h>
#include <Am/Ui/Bitmap.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>

// Native backing data for Am.Ui.Bitmap on AmigaOS.
// bitmap: the hardware BitMap (allocated via CyberGraphX/AllocBitMap)
// mask:   optional 1-bit-deep BitMap whose plane 0 is the bltMask for
//         BltMaskBitMapRastPort — non-NULL only when the bitmap was
//         built via createFromImageWithMask. drawBitmap branches on
//         this pointer: present → BltMaskBitMapRastPort with minterm
//         0xE0; absent → vanilla BltBitMapRastPort with 0xC0.
// Note: Bitmap does not own a RastPort.
// Layer-backed graphics contexts (through RenderableBitmap/LayerGraphics)
// create/wire the RastPort when needed.
typedef struct _Am_Ui_Bitmap_data Am_Ui_Bitmap_data;
struct _Am_Ui_Bitmap_data {
    struct BitMap *bitmap;
    struct BitMap *mask;
};

#endif
