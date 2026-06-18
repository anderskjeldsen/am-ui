#include <libc/core.h>
#include <Am/Ui/ScreenModeRequester.h>
#include <Am/Lang/Object.h>
#include <Am/Ui/ScreenMode.h>
#include <Am/Lang/String.h>
#include <Am/Lang/Int.h>
#include <libc/core_inline_functions.h>

// ScreenModeRequester — Linux WMs don't offer per-app mode switching,
// so the constraint setters are no-ops and `open` returns NULL.
// AmLang side treats null as "user cancelled" and keeps the default
// ScreenMode that Screen.getDefaultScreenMode already supplied.

function_result Am_Ui_ScreenModeRequester__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_ScreenModeRequester__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_ScreenModeRequester__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_ScreenModeRequester_setMin_0(aobject *const this, int var_width, int var_height, int var_depth)
{
    function_result __result = { .has_return_value = false };
    (void) this; (void) var_width; (void) var_height; (void) var_depth;
    return __result;
}
function_result Am_Ui_ScreenModeRequester_setDesired_0(aobject *const this, int var_width, int var_height, int var_depth)
{
    function_result __result = { .has_return_value = false };
    (void) this; (void) var_width; (void) var_height; (void) var_depth;
    return __result;
}
function_result Am_Ui_ScreenModeRequester_setMax_0(aobject *const this, int var_width, int var_height, int var_depth)
{
    function_result __result = { .has_return_value = false };
    (void) this; (void) var_width; (void) var_height; (void) var_depth;
    return __result;
}
function_result Am_Ui_ScreenModeRequester_open_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    (void) this;
    __result.return_value.value.object_value = NULL;
    __result.return_value.flags = 0;
    return __result;
}
