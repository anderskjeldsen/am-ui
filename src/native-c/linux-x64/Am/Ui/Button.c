#include <libc/core.h>
#include <Am/Ui/Button.h>
#include <libc/core_inline_functions.h>

// Button — am-ui paints its own button face from the AmLang side
// (Button.aml), so the "native attach/detach" hooks are no-ops here.

function_result Am_Ui_Button__native_init_0(aobject *const this)                          { (void) this; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_Button__native_release_0(aobject *const this)                       { (void) this; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_Button_attachButton_0(aobject *const this, aobject *window)         { (void) this; (void) window; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_Button_detachButton_0(aobject *const this, aobject *window)         { (void) this; (void) window; return (function_result){ .has_return_value = false }; }
