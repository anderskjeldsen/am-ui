#include <libc/core.h>
#include <Am/Ui/LayerGraphics.h>
#include <morphos-ppc/Am/Ui/LayerGraphics.h>
#include <Am/Ui/RenderableBitmap.h>
#include <Am/Ui/Bitmap.h>
#include <Am/Ui/Font.h>
#include <Am/Ui/ClipRect.h>
#include <Am/Imaging/Image.h>
#include <libc/core_inline_functions.h>

// LayerGraphics is AmigaOS-only — all functions are stubs on MorphOS.

#define STUB_VOID(name) \
function_result name { \
    function_result __result = { .has_return_value = false }; \
    bool __returning = false; \
__exit: ; \
    return __result; \
}

STUB_VOID(Am_Ui_LayerGraphics__native_init_0(aobject * const this))
STUB_VOID(Am_Ui_LayerGraphics__native_release_0(aobject * const this))
STUB_VOID(Am_Ui_LayerGraphics__native_mark_children_0(aobject * const this))
STUB_VOID(Am_Ui_LayerGraphics_attachRenderableBitmap_0(aobject * const this))
STUB_VOID(Am_Ui_LayerGraphics_attachWindow_0(aobject * const this))
STUB_VOID(Am_Ui_LayerGraphics_setForegroundPen_0(aobject * const this, unsigned char pen))
STUB_VOID(Am_Ui_LayerGraphics_setBackgroundPen_0(aobject * const this, unsigned char pen))
STUB_VOID(Am_Ui_LayerGraphics_drawLine_0(aobject * const this, short x, short y, short x2, short y2))
STUB_VOID(Am_Ui_LayerGraphics_fillRect_0(aobject * const this, short x, short y, short x2, short y2))
STUB_VOID(Am_Ui_LayerGraphics_eraseRect_0(aobject * const this, short x, short y, short x2, short y2))
STUB_VOID(Am_Ui_LayerGraphics_drawString_0(aobject * const this, aobject *text, short x, short y))
STUB_VOID(Am_Ui_LayerGraphics_drawImage_0(aobject * const this, aobject *image, short x, short y, short width, short height))
STUB_VOID(Am_Ui_LayerGraphics_drawBitmap_0(aobject * const this, aobject *bitmap, short x, short y, short destWidth, short destHeight))
STUB_VOID(Am_Ui_LayerGraphics_setClipRect_0(aobject * const this, aobject * const clipRect))
STUB_VOID(Am_Ui_LayerGraphics_clearClipRect_0(aobject * const this))
STUB_VOID(Am_Ui_LayerGraphics_beginPainting_0(aobject * const this, aobject * const clipRect))
STUB_VOID(Am_Ui_LayerGraphics_endPainting_0(aobject * const this))

function_result Am_Ui_LayerGraphics_calculateStringWidth_0(aobject * const this, aobject *text)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    __result.return_value.value.ushort_value = 0;
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_getCurrentFontSize_0(aobject * const this)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    __result.return_value.value.uchar_value = 0;
__exit: ;
    return __result;
}

STUB_VOID(Am_Ui_LayerGraphics_setFont_0(aobject * const this, aobject *font))
