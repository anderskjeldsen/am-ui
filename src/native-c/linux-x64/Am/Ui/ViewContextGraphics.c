#include <libc/core.h>
#include <Am/Ui/ViewContextGraphics.h>
#include <linux-x64/Am/Ui/ViewContextGraphics.h>
#include <Am/Ui/Graphics.h>
#include <Am/Ui/ViewContext.h>
#include <Am/Lang/UByte.h>
#include <Am/Lang/Short.h>

#include <linux-x64/linux.h>
#include <linux-x64/Am/Ui/Window.h>
#include <linux-x64/Am/Ui/Screen.h>
#include <libc/Am/Lang/String.h>

#include <libc/core_inline_functions.h>

short translated_x(aobject *g, short x) {
	short tx = g->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_xOffset].nullable_value.value.short_value;
	return tx + x;
}

short translated_y(aobject *g, short y) {
	short ty = g->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_yOffset].nullable_value.value.short_value;
	return ty + y;
}

function_result Am_Ui_ViewContextGraphics__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	printf("[linux-x64] Am_Ui_ViewContextGraphics__native_init_0\n");
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	printf("[linux-x64] Am_Ui_ViewContextGraphics__native_release_0\n");
__exit: ;
	return __result;
};

function_result Am_Ui_ViewContextGraphics__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Ui_ViewContextGraphics_setForegroundPen_0(aobject * const this, unsigned char pen)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	printf("[linux-x64] setForegroundPen: %d\n", pen);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_setBackgroundPen_0(aobject * const this, unsigned char pen)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	printf("[linux-x64] setBackgroundPen: %d\n", pen);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_drawLine_0(aobject * const this, short x, short y, short x2, short y2)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);
	short tx2 = translated_x(this, x2);
	short ty2 = translated_y(this, y2);

	printf("[linux-x64] drawLine: (%d,%d) -> (%d,%d)\n", tx, ty, tx2, ty2);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_fillRect_0(aobject * const this, short x, short y, short x2, short y2)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);
	short tx2 = translated_x(this, x2);
	short ty2 = translated_y(this, y2);

	printf("[linux-x64] fillRect: (%d,%d) to (%d,%d)\n", tx, ty, tx2, ty2);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_drawString_0(aobject * const this, aobject * text, short x, short y)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (text != NULL) {
		__increase_reference_count(text);
	}

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);

	string_holder *string_holder_value = text->object_properties.class_object_properties.object_data.value.custom_value;
	printf("[linux-x64] drawString at (%d,%d): %s\n", tx, ty, string_holder_value->string);

__exit: ;
	if (text != NULL) {
		__decrease_reference_count(text);
	}
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};
