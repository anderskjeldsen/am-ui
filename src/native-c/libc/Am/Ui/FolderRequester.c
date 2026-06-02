#include <libc/core.h>
#include <Am/Ui/FolderRequester.h>
#include <libc/Am/Ui/FolderRequester.h>
#include <Am/Lang/ClassRef.h>
#include <Am/Lang/String.h>
#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <libc/core_inline_functions.h>

function_result Am_Ui_FolderRequester__native_init_0(aobject * const this)
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

function_result Am_Ui_FolderRequester__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_FolderRequester__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_FolderRequester_requestFolder_0(aobject * title, aobject * initialPath, aobject * parent)
{
	function_result __result = { .has_return_value = true };
	bool __returning = false;
	if (title != NULL) {
		__increase_reference_count(title);
	}
	if (initialPath != NULL) {
		__increase_reference_count(initialPath);
	}
	if (parent != NULL) {
		__increase_reference_count(parent);
	}
__exit: ;
	if (title != NULL) {
		__decrease_reference_count(title);
	}
	if (initialPath != NULL) {
		__decrease_reference_count(initialPath);
	}
	if (parent != NULL) {
		__decrease_reference_count(parent);
	}
	return __result;
}

