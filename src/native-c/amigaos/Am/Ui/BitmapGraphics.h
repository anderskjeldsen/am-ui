#ifndef native_amigaos_aclass_Am_Ui_BitmapGraphics_h
#define native_amigaos_aclass_Am_Ui_BitmapGraphics_h
#include <libc/core.h>
#include <Am/Ui/BitmapGraphics.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>

// Native data for BitmapGraphics: own RastPort wired to the Bitmap.
typedef struct _Am_Ui_BitmapGraphics_data Am_Ui_BitmapGraphics_data;
struct _Am_Ui_BitmapGraphics_data {
    struct RastPort rastport;
};

#endif
