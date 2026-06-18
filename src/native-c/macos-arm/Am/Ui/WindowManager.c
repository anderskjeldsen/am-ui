#include <libc/core.h>
#include <Am/Ui/WindowManager.h>
#include <macos-arm/Am/Ui/WindowManager.h>
#include <Am/Lang/Object.h>
#include <libc/core_inline_functions.h>

// WindowManager — one-process, no iconify. All three lifecycle hooks
// are no-ops.

function_result Am_Ui_WindowManager__native_init_0(aobject *const this)         { (void) this; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_WindowManager__native_mark_children_0(aobject *const this){ (void) this; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_WindowManager__native_release_0(aobject *const this)      { (void) this; return (function_result){ .has_return_value = false }; }
