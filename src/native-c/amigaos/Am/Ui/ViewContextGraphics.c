#include <libc/core.h>
#include <Am/Ui/ViewContextGraphics.h>
#include <amigaos/Am/Ui/ViewContextGraphics.h>
#include <Am/Ui/Graphics.h>
#include <Am/Ui/ViewContext.h>
#include <Am/Lang/UByte.h>
#include <Am/Lang/Short.h>

#include <amigaos/amiga.h>
#include <amigaos/Am/Ui/Window.h>
#include <amigaos/Am/Ui/Screen.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <graphics/gfx.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>


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

	aobject *window = Am_Ui_ViewContextGraphics_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	SetAPen(rp, pen);
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

	aobject *window = Am_Ui_ViewContextGraphics_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	SetBPen(rp, pen);

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

	aobject *window = Am_Ui_ViewContextGraphics_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);
	short tx2 = translated_x(this, x2);
	short ty2 = translated_y(this, y2);
/*
	printf("x %d => %d\n", x, tx);
	printf("x2 %d => %d\n", x2, tx2);
	printf("y %d => %d\n", y, ty);
	printf("y2 %d => %d\n", y2, ty2);
*/
    Move(rp, tx, ty);
    Draw(rp, tx2, ty2);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_eraseRect_0(aobject * const this, short x, short y, short x2, short y2)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	aobject *window = Am_Ui_ViewContextGraphics_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);
	short tx2 = translated_x(this, x2);
	short ty2 = translated_y(this, y2);

	EraseRect(rp, tx, ty, tx2, ty2);
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

	aobject *window = Am_Ui_ViewContextGraphics_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);
	short tx2 = translated_x(this, x2);
	short ty2 = translated_y(this, y2);

	RectFill(rp, tx, ty, tx2, ty2);
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};