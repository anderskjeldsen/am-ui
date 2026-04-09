#include <libc/core.h>
#include <Am/Ui/Bitmap.h>
#include <morphos-ppc/Am/Ui/Bitmap.h>
#include <Am/Imaging/Image.h>
#include <libc/core_inline_functions.h>

// Bitmap true-colour support is AmigaOS-only for now.
// MorphOS stubs — no-ops.

function_result Am_Ui_Bitmap__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Bitmap__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Bitmap__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Bitmap_createEmpty_0(aobject * const this,
                                            unsigned short width,
                                            unsigned short height)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Bitmap_createFromImage_0(aobject * const this,
                                               aobject * image)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}
