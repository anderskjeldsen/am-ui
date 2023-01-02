#ifndef native_libc_aclass_Am_Ui_Startup_c
#define native_libc_aclass_Am_Ui_Startup_c
#include <libc/core.h>
#include <Am/Ui/Startup.h>
#include <libc/Am/Ui/Startup.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>

function_result Am_Ui_Startup__native_init_0(aobject * const this)
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

function_result Am_Ui_Startup__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

#endif
