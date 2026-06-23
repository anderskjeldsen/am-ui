#include <libc/core.h>
#include <Am/Ui/GadgetViews.h>
#include <libc/core_inline_functions.h>

// GadgetViews — AmigaOS-only "native gadget bag" wrapper. Linux paints
// its own controls via Button.aml / ImageButton.aml / etc., so the
// gadget layout pass is a no-op.

function_result Am_Ui_GadgetViews__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_GadgetViews__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_GadgetViews__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_GadgetViews_layoutNative_0(aobject *gadgetView, short x, short y, unsigned short width, unsigned short height)
{
    function_result __result = { .has_return_value = false };
    (void) gadgetView; (void) x; (void) y; (void) width; (void) height;
    return __result;
}
