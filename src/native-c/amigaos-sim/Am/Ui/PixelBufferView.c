#include <libc/core.h>
#include <Am/Ui/PixelBufferView.h>
#include <amigaos-sim/Am/Ui/PixelBufferView.h>
#include <Am/Lang/ClassRef.h>
#include <Am/Ui/View.h>
#include <Am/Lang/UShort.h>
#include <Am/Lang/UInt.h>
#include <Am/Lang/Array.h>
#include <Am/Ui/Alignment.h>
#include <Am/Ui/Graphics.h>
#include <Am/Lang/Short.h>
#include <Am/Lang/Bool.h>
#include <libc/core_inline_functions.h>

function_result Am_Ui_PixelBufferView__native_init_0(aobject * const this)
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

function_result Am_Ui_PixelBufferView__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_PixelBufferView__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_PixelBufferView_initBuffer_0(aobject * const this, unsigned short width, unsigned short height)
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

function_result Am_Ui_PixelBufferView_writePixels_0(aobject * const this, aobject * graphics, short x, short y)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (graphics != NULL) {
		__increase_reference_count(graphics);
	}
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (graphics != NULL) {
		__decrease_reference_count(graphics);
	}
	return __result;
}

