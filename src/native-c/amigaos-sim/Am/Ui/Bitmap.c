#include <libc/core.h>
#include <Am/Ui/Bitmap.h>
#include <amigaos-sim/Am/Ui/Bitmap.h>
#include <Am/Lang/ClassRef.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/UShort.h>
#include <Am/Ui/BitmapPixelFormat.h>
#include <Am/Ui/Window.h>
#include <Am/Imaging/Image.h>
#include <libc/core_inline_functions.h>

function_result Am_Ui_Bitmap__native_init_0(aobject * const this)
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

function_result Am_Ui_Bitmap__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_Bitmap__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_Bitmap_createEmpty_0(aobject * const this, unsigned short width, unsigned short height, aobject * window)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (window != NULL) {
		__increase_reference_count(window);
	}
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	return __result;
}

function_result Am_Ui_Bitmap_createFromImage_0(aobject * const this, aobject * image, aobject * window)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (image != NULL) {
		__increase_reference_count(image);
	}
	if (window != NULL) {
		__increase_reference_count(window);
	}
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (image != NULL) {
		__decrease_reference_count(image);
	}
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	return __result;
}

