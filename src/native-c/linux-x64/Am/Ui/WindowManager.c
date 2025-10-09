#include <libc/core.h>
#include <Am/Ui/WindowManager.h>
#include <linux-x64/Am/Ui/WindowManager.h>
#include <Am/Lang/Object.h>
#include <libc/core_inline_functions.h>


function_result Am_Ui_WindowManager__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	printf("[linux-x64] Am_Ui_WindowManager__native_init_0\n");
	// For Linux, we don't need a MsgPort equivalent
	// Just set a placeholder value to indicate initialization
	this->object_properties.class_object_properties.object_data.value.custom_value = (void*)1;

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_WindowManager__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;

	printf("[linux-x64] Am_Ui_WindowManager__native_release_0\n");
	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;

__exit: ;
	return __result;
};

function_result Am_Ui_WindowManager__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

