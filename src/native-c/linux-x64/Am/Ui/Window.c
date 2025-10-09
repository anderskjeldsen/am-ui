#include <libc/core.h>

#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Ui/Screen.h>
#include <Am/Lang/Int.h>

#include <linux-x64/linux.h>
#include <linux-x64/Am/Ui/Window.h>
#include <linux-x64/Am/Ui/Screen.h>

#include <libc/core_inline_functions.h>

void close_window_native(aobject * const this) {
	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	if ( data != NULL ) {		
		bool releasing = this->reference_count == 0;
		if (releasing) {
			this->reference_count = -100;
		}
		Am_Ui_Window_setRootView_0(this, NULL);
		if (releasing) {
			this->reference_count = 0;
		}

		printf("[linux-x64] Closing window %dx%d\n", data->width, data->height);
		data->is_open = false;
	}

	free(this->object_properties.class_object_properties.object_data.value.custom_value);
	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
}

function_result Am_Ui_Window__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	printf("[linux-x64] Am_Ui_Window__native_init_0\n");
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_Window__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;

	close_window_native(this);

	printf("[linux-x64] Am_Ui_Window__native_release_0\n");
__exit: ;
	return __result;
};

function_result Am_Ui_Window__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};


function_result Am_Ui_Window_open_0(aobject * const this, short x, short y, unsigned short width, unsigned short height, aobject * screen, aobject * windowManager)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	printf("[linux-x64] Am_Ui_Window_open_0\n");
	printf("  Position: %d, %d\n", x, y);
	printf("  Size: %dx%d\n", width, height);

	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) calloc(1, sizeof(Am_Ui_Window_data));
	this->object_properties.class_object_properties.object_data.value.custom_value = data;

	data->x = x;
	data->y = y;
	data->width = width;
	data->height = height;
	data->is_open = true;
	data->pending_close = false;
	data->last_mouse_x = 0;
	data->last_mouse_y = 0;

	// Set pixel scale to 1:1 for Linux
	this->object_properties.class_object_properties.properties[Am_Ui_Window_P_pixelScaleX].nullable_value.value.uchar_value = 1;
	this->object_properties.class_object_properties.properties[Am_Ui_Window_P_pixelScaleY].nullable_value.value.uchar_value = 1;

	// Set border to 0 for simplicity
	Am_Ui_Window_setBorder_0(this, 0, 0, 0, 0);
	Am_Ui_Window_onResize_0(this, x, y, width, height);

	printf("[linux-x64] Window opened successfully\n");

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (screen != NULL) {
		__decrease_reference_count(screen);
	}
	return __result;
};

function_result Am_Ui_Window_close_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	printf("[linux-x64] Am_Ui_Window_close_0\n");
	close_window_native(this);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_Window_handleInput_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;

	printf("[linux-x64] Am_Ui_Window_handleInput_0 (simulated)\n");
	
	// Simulate a simple input loop - in real implementation, this would poll for events
	// For now, just return without blocking
	
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_Window_isOpen_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	bool __returning = true;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	__result.return_value.value.bool_value = (data != NULL && data->is_open);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_Window_refresh_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	printf("[linux-x64] Am_Ui_Window_refresh_0\n");
	// Trigger a repaint
	Am_Ui_Window_paint_0(this);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};
