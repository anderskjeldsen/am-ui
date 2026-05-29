#include <libc/core.h>
#include <Am/Ui/NativeLibraryManager.h>
#include <amigaos/Am/Ui/NativeLibraryManager.h>
#include <amigaos/amiga.h>

#include <exec/types.h>
#include <exec/libraries.h>
#include <cybergraphx/cybergraphics.h>

#include <proto/exec.h>
#include <proto/cybergraphics.h>

#include <libc/core_inline_functions.h>

// CyberGfxBase is declared extern by proto/cybergraphics.h.
// We provide the one definition here.
struct Library *CyberGfxBase = NULL;

function_result Am_Ui_NativeLibraryManager__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }

    if (CyberGfxBase == NULL) {
        CyberGfxBase = (struct Library *)__ensure_library("cybergraphics.library", 40L);
        if (CyberGfxBase == NULL) {
            __throw_simple_exception("Failed to open cybergraphics.library v40",
                                     "Am_Ui_NativeLibraryManager__native_init_0", &__result);
            goto __exit;
        }
    }

__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

function_result Am_Ui_NativeLibraryManager__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;

    // CyberGfxBase was opened via __ensure_library, so it lives in
    // the __first_lib_node list managed by amiga.c. The destructor
    // on __release_libraries owns the CloseLibrary now — calling
    // it here too would double-close the handle (singleton release
    // fires during GC sweep, then __release_libraries fires from
    // the C-runtime atexit chain). Just null the proto reference so
    // anything that re-reads it during teardown gets NULL instead
    // of a stale pointer.
    CyberGfxBase = NULL;

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
