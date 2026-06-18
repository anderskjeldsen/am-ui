#include <libc/core.h>
#include <Am/Ui/PixelBufferView.h>
#include <libc/core_inline_functions.h>

// PixelBufferView — the AmigaOS native pixel-array fast path. The
// AmLang side falls back to the portable per-pixel View path when
// these stubs return without doing anything, so the IDE renders
// without the fast path until the SDL2 backend wires it.

function_result Am_Ui_PixelBufferView__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_PixelBufferView__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_PixelBufferView__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_PixelBufferView_initBuffer_0(aobject *const this, unsigned short var_width, unsigned short var_height)
{
    function_result __result = { .has_return_value = false };
    (void) this; (void) var_width; (void) var_height;
    return __result;
}
function_result Am_Ui_PixelBufferView_writePixels_0(aobject *const this, aobject *var_graphics, short var_x, short var_y)
{
    function_result __result = { .has_return_value = false };
    (void) this; (void) var_graphics; (void) var_x; (void) var_y;
    return __result;
}
