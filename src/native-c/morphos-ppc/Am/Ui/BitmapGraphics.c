#include <libc/core.h>
#include <Am/Ui/BitmapGraphics.h>
#include <morphos-ppc/Am/Ui/BitmapGraphics.h>
#include <Am/Ui/Bitmap.h>
#include <Am/Ui/Font.h>
#include <Am/Imaging/Image.h>
#include <Am/Ui/ClipRect.h>
#include <libc/core_inline_functions.h>

// BitmapGraphics is AmigaOS-only — all functions are stubs on MorphOS.

#define STUB_VOID(name) \
function_result name { \
    function_result __result = { .has_return_value = false }; \
    bool __returning = false; \
__exit: ; \
    return __result; \
}

STUB_VOID(Am_Ui_BitmapGraphics__native_init_0(aobject * const this))
STUB_VOID(Am_Ui_BitmapGraphics__native_release_0(aobject * const this))
STUB_VOID(Am_Ui_BitmapGraphics__native_mark_children_0(aobject * const this))
STUB_VOID(Am_Ui_BitmapGraphics_attach_0(aobject * const this))
STUB_VOID(Am_Ui_BitmapGraphics_setForegroundPen_0(aobject * const this, unsigned char pen))
STUB_VOID(Am_Ui_BitmapGraphics_setBackgroundPen_0(aobject * const this, unsigned char pen))
STUB_VOID(Am_Ui_BitmapGraphics_drawLine_0(aobject * const this, short x, short y, short x2, short y2))
STUB_VOID(Am_Ui_BitmapGraphics_fillRect_0(aobject * const this, short x, short y, short x2, short y2))
STUB_VOID(Am_Ui_BitmapGraphics_eraseRect_0(aobject * const this, short x, short y, short x2, short y2))
STUB_VOID(Am_Ui_BitmapGraphics_drawString_0(aobject * const this, aobject *text, short x, short y))
STUB_VOID(Am_Ui_BitmapGraphics_drawImage_0(aobject * const this, aobject *image, short x, short y, short width, short height))
STUB_VOID(Am_Ui_BitmapGraphics_drawBitmap_0(aobject * const this, aobject *bitmap, short x, short y, short destWidth, short destHeight))
STUB_VOID(Am_Ui_BitmapGraphics_setClipRect_0(aobject * const this, aobject * const clipRect))
STUB_VOID(Am_Ui_BitmapGraphics_clearClipRect_0(aobject * const this))
STUB_VOID(Am_Ui_BitmapGraphics_beginPainting_0(aobject * const this, aobject * const clipRect))
STUB_VOID(Am_Ui_BitmapGraphics_endPainting_0(aobject * const this))

function_result Am_Ui_BitmapGraphics_calculateStringWidth_0(aobject * const this, aobject *text)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    __result.return_value.value.ushort_value = 0;
__exit: ;
    return __result;
}

function_result Am_Ui_BitmapGraphics_getCurrentFontSize_0(aobject * const this)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    __result.return_value.value.uchar_value = 0;
__exit: ;
    return __result;
}

STUB_VOID(Am_Ui_BitmapGraphics_setFont_0(aobject * const this, aobject *font))
