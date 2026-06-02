#ifndef native_morphos_ppc_aclass_Am_Ui_Bitmap_h
#define native_morphos_ppc_aclass_Am_Ui_Bitmap_h
#include <libc/core.h>
#include <Am/Ui/Bitmap.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>

// Native backing data for Am.Ui.Bitmap on MorphOS. Same shape as the
// AmigaOS version — MorphOS shares the Intuition / graphics /
// cybergraphx APIs, so the AmLang-side semantics are identical.
typedef struct _Am_Ui_Bitmap_data Am_Ui_Bitmap_data;
struct _Am_Ui_Bitmap_data {
    struct BitMap *bitmap;
    struct BitMap *mask;
};

#endif
