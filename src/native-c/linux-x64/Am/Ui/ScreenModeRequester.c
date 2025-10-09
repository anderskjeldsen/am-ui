#include <libc/core.h>
#include <linux-x64/linux.h>
#include <Am/Ui/ScreenModeRequester.h>
#include <linux-x64/Am/Ui/ScreenModeRequester.h>
#include <Am/Lang/Object.h>
#include <Am/Ui/ScreenMode.h>
#include <Am/Lang/Int.h>

#include <libc/core_inline_functions.h>


function_result Am_Ui_ScreenModeRequester__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
	printf("[linux-x64] Am_Ui_ScreenModeRequester__native_init_0\n");

	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) malloc(sizeof(Am_Ui_ScreenModeRequester_data));
	memset(data, 0, sizeof(Am_Ui_ScreenModeRequester_data));
	
	this->object_properties.class_object_properties.object_data.value.custom_value = data;

__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_ScreenModeRequester__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;

	printf("[linux-x64] Am_Ui_ScreenModeRequester__native_release_0\n");
	
	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	free(data);

	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;

__exit:
	return __result;
};

function_result Am_Ui_ScreenModeRequester__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Ui_ScreenModeRequester_setMin_0(aobject * const this, int width, int height, int depth)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
	printf("[linux-x64] Am_Ui_ScreenModeRequester_setMin_0: %d x %d x %d\n", width, height, depth);

	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	data->min_width = width;
	data->min_height = height;
	data->min_depth = depth;

__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_ScreenModeRequester_setMax_0(aobject * const this, int width, int height, int depth)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
	printf("[linux-x64] Am_Ui_ScreenModeRequester_setMax_0: %d x %d x %d\n", width, height, depth);

	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	data->max_width = width;
	data->max_height = height;
	data->max_depth = depth;

__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_ScreenModeRequester_setDesired_0(aobject * const this, int width, int height, int depth)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
	printf("[linux-x64] Am_Ui_ScreenModeRequester_setDesired_0: %d x %d x %d\n", width, height, depth);

	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	data->desired_width = width;
	data->desired_height = height;
	data->desired_depth = depth;

__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_ScreenModeRequester_open_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	bool __returning = false;
	__increase_reference_count(this);
	printf("[linux-x64] Am_Ui_ScreenModeRequester_open_0\n");

	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) this->object_properties.class_object_properties.object_data.value.custom_value;

	// Simulate user selection - return the desired values
	printf("[linux-x64] Simulating screen mode selection\n");
	printf("  Selected: %lux%lux%lu\n", data->desired_width, data->desired_height, data->desired_depth);

	// Create a screen mode object and return it
	aobject * screen_mode = Am_Ui_ScreenMode_new_0();
	Am_Ui_ScreenMode_setWidth_0(screen_mode, data->desired_width);
	Am_Ui_ScreenMode_setHeight_0(screen_mode, data->desired_height);
	Am_Ui_ScreenMode_setDepth_0(screen_mode, data->desired_depth);
	Am_Ui_ScreenMode_setModeId_0(screen_mode, 0);

	__result.return_value.value.object_value = screen_mode;

__exit:
	__decrease_reference_count(this);
	return __result;
};
