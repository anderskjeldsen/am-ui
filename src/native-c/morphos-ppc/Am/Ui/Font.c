#include <libc/core.h>
#include <Am/Ui/Font.h>
#include <morphos-ppc/Am/Ui/Font.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>
#include <Am/Lang/UByte.h>
#include <Am/Collections/HashMap.h>
#include <libc/Am/Lang/String.h>

#include <morphos-ppc/morphos.h>
#include <libc/Am/Lang/String.h>

#include <exec/types.h>
#include <graphics/gfx.h>

#include <proto/exec.h>
#include <proto/graphics.h>

#include <libc/core_inline_functions.h>

function_result Am_Ui_Font__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

    aobject *name = this->object_properties.class_object_properties.properties[Am_Ui_Font_P_name].nullable_value.value.object_value;
    UBYTE size = this->object_properties.class_object_properties.properties[Am_Ui_Font_P_size].nullable_value.value.uchar_value;

    string_holder *sh = name->object_properties.class_object_properties.object_data.value.custom_value;


    struct TextAttr textAttr = { sh->string_value, size, 0, 0 };

    struct TextFont *font = OpenFont(&textAttr);

    if (font == NULL) {
        __throw_simple_exception("Failed to open font", "in Am_Ui_Font__native_init_0", &__result);
		goto __exit;
    }

    this->object_properties.class_object_properties.object_data.value.custom_value = font;

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_Font__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;

    struct TextFont *font = this->object_properties.class_object_properties.object_data.value.custom_value;
    if (font != NULL) {
        CloseFont(font);
        this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }


__exit: ;
	return __result;
};

function_result Am_Ui_Font_calculateStringWidth_0(aobject * const this, aobject * text)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (text != NULL) {
		__increase_reference_count(text);
	}

    struct TextFont *font = this->object_properties.class_object_properties.object_data.value.custom_value;
    string_holder *sh = text->object_properties.class_object_properties.object_data.value.custom_value;

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (text != NULL) {
		__decrease_reference_count(text);
	}
	return __result;
};
