#ifndef native_amigaos_aclass_Am_Ui_Bitmap_h
#define native_amigaos_aclass_Am_Ui_Bitmap_h
#include <libc/core.h>
#include <Am/Ui/Bitmap.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>

// Native backing data for Am.Ui.Bitmap on AmigaOS.
// bitmap: the hardware BitMap (allocated via CyberGraphX/AllocBitMap)
// Note: Bitmap does not own a RastPort; BitmapGraphics owns the RastPort
// and wires it to this bitmap in BitmapGraphics.attach().
typedef struct _Am_Ui_Bitmap_data Am_Ui_Bitmap_data;
struct _Am_Ui_Bitmap_data {
    struct BitMap *bitmap;
};

#endif
