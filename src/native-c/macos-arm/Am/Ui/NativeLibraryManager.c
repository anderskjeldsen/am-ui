#include <libc/core.h>
#include <Am/Ui/NativeLibraryManager.h>
#include <Am/Lang/Object.h>
#include <libc/core_inline_functions.h>

// NativeLibraryManager — AmigaOS-only bookkeeping for the
// AmiSSL-style late-bound library opens. Linux links its system libs
// statically into the binary so there's nothing to manage; all hooks
// are no-ops.

function_result Am_Ui_NativeLibraryManager__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_NativeLibraryManager__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_NativeLibraryManager__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
