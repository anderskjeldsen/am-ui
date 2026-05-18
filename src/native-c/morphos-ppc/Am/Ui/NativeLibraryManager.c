#include <libc/core.h>
#include <Am/Ui/NativeLibraryManager.h>
#include <morphos-ppc/Am/Ui/NativeLibraryManager.h>
#include <libc/core_inline_functions.h>

// On MorphOS CyberGraphX is part of graphics.library and opened
// implicitly, so we don't have to mirror the amigaos NativeLibraryManager
// that opens cybergraphics.library manually. The lifecycle hooks still
// need to exist so the linker is happy.

function_result Am_Ui_NativeLibraryManager__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_NativeLibraryManager__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_NativeLibraryManager__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}
