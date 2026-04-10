#include <libc/core.h>
#include <Am/Ui/RenderableBitmap.h>
#include <morphos-ppc/Am/Ui/RenderableBitmap.h>
#include <Am/Ui/Bitmap.h>
#include <libc/core_inline_functions.h>

// RenderableBitmap is AmigaOS-only — all functions are stubs on MorphOS.

#define STUB_VOID(name) \
function_result name { \
    function_result __result = { .has_return_value = false }; \
    bool __returning = false; \
__exit: ; \
    return __result; \
}

STUB_VOID(Am_Ui_RenderableBitmap__native_init_0(aobject * const this))
STUB_VOID(Am_Ui_RenderableBitmap__native_release_0(aobject * const this))
STUB_VOID(Am_Ui_RenderableBitmap__native_mark_children_0(aobject * const this))
STUB_VOID(Am_Ui_RenderableBitmap_attachLayer_0(aobject * const this))
