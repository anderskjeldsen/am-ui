// Test-only buffer accessor for the amigaos-sim platform.

#include <libc/core.h>
#include <Am/Ui/Tests/HeadlessPainter.h>
#include <Am/Lang/UByte.h>
#include <libc/core_inline_functions.h>

#include <amigaos-sim/headless_painter.h>

#include <string.h>

function_result Am_Ui_Tests_HeadlessPainter__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) __increase_reference_count(this);
__exit: ;
    if (this != NULL) __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Tests_HeadlessPainter__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Tests_HeadlessPainter__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Tests_HeadlessPainter_getPixelAt_0(aobject *window, short x, short y)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    if (window != NULL) __increase_reference_count(window);
    headless_screen_t *s = headless_screen_for_window(window);
    __result.return_value.value.uchar_value = headless_read_pixel(s, x, y);
__exit: ;
    if (window != NULL) __decrease_reference_count(window);
    return __result;
}

function_result Am_Ui_Tests_HeadlessPainter_clear_0(aobject *window)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (window != NULL) __increase_reference_count(window);
    headless_screen_t *s = headless_screen_for_window(window);
    if (s != NULL && s->buffer != NULL) {
        memset(s->buffer, 0, (size_t) s->width * (size_t) s->height);
    }
__exit: ;
    if (window != NULL) __decrease_reference_count(window);
    return __result;
}
