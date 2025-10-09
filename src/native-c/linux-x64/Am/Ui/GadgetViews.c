#include <libc/core.h>
#include <Am/Ui/GadgetViews.h>
#include <Am/Ui/View.h>
#include <linux-x64/Am/Ui/GadgetViews.h>
#include <Am/Lang/Object.h>

#include <linux-x64/linux.h>
#include <linux-x64/Am/Ui/Window.h>

#include <libc/core_inline_functions.h>

function_result Am_Ui_GadgetViews__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	printf("[linux-x64] Am_Ui_GadgetViews__native_init_0\n");
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_GadgetViews__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	printf("[linux-x64] Am_Ui_GadgetViews__native_release_0\n");
__exit: ;
	return __result;
};

function_result Am_Ui_GadgetViews__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Ui_GadgetViews_layoutNative_0(aobject * gadgetView, short x, short y, unsigned short width, unsigned short height)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (gadgetView != NULL) {
		__increase_reference_count(gadgetView);
	}

	printf("[linux-x64] Am_Ui_GadgetViews_layoutNative_0: pos=(%d,%d) size=%dx%d\n", x, y, width, height);

__exit: ;
	if (gadgetView != NULL) {
		__decrease_reference_count(gadgetView);
	}
	return __result;
};
