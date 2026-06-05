// Headless Window for the amigaos-sim build target. Backs every Window with
// a HEADLESS_SCREEN_WIDTH x HEADLESS_SCREEN_HEIGHT byte buffer stored in the
// aobject's custom_value, so the .aml-side painting code thinks it has a real
// window but draws into RAM instead.

#include <libc/core.h>
#include <Am/Ui/Window.h>
#include <Am/Ui/WindowProperties.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>
#include <Am/Lang/Bool.h>
#include <Am/Ui/Screen.h>
#include <Am/Lang/Int.h>
#include <libc/core_inline_functions.h>

#include <amigaos-sim/headless_painter.h>

#include <stdlib.h>

function_result Am_Ui_Window__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }
__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

function_result Am_Ui_Window__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    headless_screen_t *s = headless_screen_for_window(this);
    if (s != NULL) {
        headless_screen_free(s);
        this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }
__exit: ;
    return __result;
}

function_result Am_Ui_Window__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Window_open_0(aobject * const this, short x, short y,
                                    unsigned short width, unsigned short height,
                                    aobject *screen, aobject *windowManager,
                                    struct Am_Ui_WindowProperties *properties)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    (void)properties;

    // Width / height parameters from the .aml side are honored, but capped to
    // the headless screen size so a paint never escapes our buffer.
    unsigned short w = width  > HEADLESS_SCREEN_WIDTH  ? HEADLESS_SCREEN_WIDTH  : width;
    unsigned short h = height > HEADLESS_SCREEN_HEIGHT ? HEADLESS_SCREEN_HEIGHT : height;
    if (w == 0) w = HEADLESS_SCREEN_WIDTH;
    if (h == 0) h = HEADLESS_SCREEN_HEIGHT;

    headless_screen_t *s = headless_screen_alloc(w, h);
    this->object_properties.class_object_properties.object_data.value.custom_value = s;

__exit: ;
    return __result;
}

function_result Am_Ui_Window_close_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    headless_screen_t *s = headless_screen_for_window(this);
    if (s != NULL) {
        s->open = false;
    }
__exit: ;
    return __result;
}

function_result Am_Ui_Window_isOpen_0(aobject * const this)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    headless_screen_t *s = headless_screen_for_window(this);
    __result.return_value.value.bool_value = (s != NULL && s->open);
__exit: ;
    return __result;
}

function_result Am_Ui_Window_handleInput_0(aobject * const this)
{
    // No event source in the headless build — return immediately so test
    // loops can opt to call this once and then check pixels.
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Window_refresh_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Window_setTitleNative_0(aobject * const this, aobject *windowTitle, aobject *screenTitle)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (windowTitle != NULL) __increase_reference_count(windowTitle);
    if (screenTitle != NULL) __increase_reference_count(screenTitle);
__exit: ;
    if (windowTitle != NULL) __decrease_reference_count(windowTitle);
    if (screenTitle != NULL) __decrease_reference_count(screenTitle);
    return __result;
}

function_result Am_Ui_Window_copyToClipboard_0(aobject * const this, aobject *text)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (text != NULL) __increase_reference_count(text);
__exit: ;
    if (text != NULL) __decrease_reference_count(text);
    return __result;
}

function_result Am_Ui_Window_pasteFromClipboard_0(aobject * const this)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    __result.return_value.value.object_value = NULL;
__exit: ;
    return __result;
}

function_result Am_Ui_Window_nativeBeginMenuStrip_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    (void)this;
    return __result;
}

function_result Am_Ui_Window_nativeAddMenu_0(aobject * const this, aobject * const title)
{
    function_result __result = { .has_return_value = false };
    (void)this; (void)title;
    return __result;
}

function_result Am_Ui_Window_nativeAddMenuItem_0(aobject * const this, int menuIndex, aobject * const item, aobject * const label)
{
    function_result __result = { .has_return_value = false };
    (void)this; (void)menuIndex; (void)item; (void)label;
    return __result;
}

function_result Am_Ui_Window_nativeAddMenuSubItem_0(aobject * const this, int menuIndex, aobject * const item, aobject * const label)
{
    function_result __result = { .has_return_value = false };
    (void)this; (void)menuIndex; (void)item; (void)label;
    return __result;
}

function_result Am_Ui_Window_nativeFinalizeMenuStrip_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    (void)this;
    return __result;
}

function_result Am_Ui_Window_nativeClearMenuStrip_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    (void)this;
    return __result;
}
