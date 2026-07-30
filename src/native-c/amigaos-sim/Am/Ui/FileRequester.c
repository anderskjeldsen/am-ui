#include <libc/core.h>
#include <Am/Ui/FileRequester.h>
#include <amigaos-sim/Am/Ui/FileRequester.h>
#include <Am/Lang/ClassRef.h>
#include <Am/Lang/String.h>
#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <libc/core_inline_functions.h>

// amigaos-sim has no native ASL — file picking is a no-op stub — file picking is a no-op stub that returns
// null (treated as "cancelled" by the AmLang side).

function_result Am_Ui_FileRequester__native_init_0(aobject * const this)
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

function_result Am_Ui_FileRequester__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_FileRequester__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_FileRequester_requestFile_0(aobject * title, aobject * initialPath, aobject * parent)
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
	__result.return_value.value.object_value = NULL;
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
