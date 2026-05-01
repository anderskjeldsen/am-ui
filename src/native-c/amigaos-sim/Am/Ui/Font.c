#include <libc/core.h>
#include <Am/Ui/Font.h>
#include <Am/Lang/UShort.h>
#include <Am/Lang/String.h>
#include <libc/core_inline_functions.h>

function_result Am_Ui_Font__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) __increase_reference_count(this);
__exit: ;
    if (this != NULL) __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Font__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Font__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_Font_calculateStringWidth_0(aobject * const this, aobject *text)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    if (text != NULL) __increase_reference_count(text);
    unsigned short width = 0;
    if (text != NULL) {
        string_holder *sh = (string_holder *) text->object_properties.class_object_properties.object_data.value.custom_value;
        if (sh != NULL) width = (unsigned short)(sh->length * 8);
    }
    __result.return_value.value.ushort_value = width;
__exit: ;
    if (text != NULL) __decrease_reference_count(text);
    return __result;
}
