#ifndef native_aclass_Am_Ui_Screen_c
#define native_aclass_Am_Ui_Screen_c
#include <libc/core.h>
#include <linux-x64/linux.h>
#include <Am/Ui/Screen.h>
#include <linux-x64/Am/Ui/Screen.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/Int.h>

#include <libc/core_inline_functions.h>


function_result Am_Ui_Screen__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
	printf("[linux-x64] Am_Ui_Screen__native_init_0\n");
__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_Screen__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;

	Am_Ui_Screen_close_0(this);

__exit:
	return __result;
};

function_result Am_Ui_Screen__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};


function_result Am_Ui_Screen_open_0(aobject * const this, int width, int height, int depth, int displayId)
{
	function_result __result = { .has_return_value = 0 };
	__increase_reference_count(this);

	printf("[linux-x64] Am_Ui_Screen_open_0\n");
	printf("  Width: %d\n", width);
	printf("  Height: %d\n", height);
	printf("  Depth: %d\n", depth);
	printf("  DisplayId: %d\n", displayId);

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) malloc(sizeof(Am_Ui_Screen_data));
	data->width = width;
	data->height = height;
	data->depth = depth;
	data->displayId = displayId;
	data->sdl_window = NULL;
	data->sdl_renderer = NULL;

	// Note: Screen in Amiga terms is like a display mode, not an actual window
	// So we don't create SDL window here, that's done in Window.c
	
	this->object_properties.class_object_properties.object_data.value.custom_value = data;

	printf("[linux-x64] Screen opened successfully\n");

exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_Screen_close_0(aobject * const this)
{
	printf("[linux-x64] Am_Ui_Screen_close_0\n");
	function_result __result = { .has_return_value = 0 };
	__increase_reference_count(this);

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;

	if ( data != NULL ) {
		printf("[linux-x64] Closing screen %dx%d\n", data->width, data->height);
	}

	free(this->object_properties.class_object_properties.object_data.value.custom_value);
	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;

exit:
	__decrease_reference_count(this);
	return __result;
};

#endif
