#include <libc/core.h>
#include <Am/Ui/GadToolsButton.h>
#include <libc/core_inline_functions.h>

// GadToolsButton — AmigaOS GadTools BUTTON_KIND wrapper. Linux uses
// am-ui's pure-AmLang Button — no native gadget. Attach/detach are
// no-ops; the AmLang side renders its own face.

function_result Am_Ui_GadToolsButton__native_init_0(aobject *const this)         { (void) this; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_GadToolsButton__native_mark_children_0(aobject *const this){ (void) this; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_GadToolsButton__native_release_0(aobject *const this)      { (void) this; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_GadToolsButton_attachButton_0(aobject *const this, aobject *window) { (void) this; (void) window; return (function_result){ .has_return_value = false }; }
function_result Am_Ui_GadToolsButton_detachButton_0(aobject *const this, aobject *window) { (void) this; (void) window; return (function_result){ .has_return_value = false }; }
