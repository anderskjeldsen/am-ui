#include <libc/core.h>
#include <Am/Ui/Startup.h>
#include <linux-x64/Am/Ui/Startup.h>
#include <Am/Lang/Object.h>
#include <libc/core_inline_functions.h>

function_result Am_Ui_Startup_init_0()
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;

	printf("[linux-x64] Am_Ui_Startup_init_0\n");
	printf("[linux-x64] UI system initialized for Linux x64\n");

__exit: ;
	return __result;
};

function_result Am_Ui_Startup_shutdown_0()
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;

	printf("[linux-x64] Am_Ui_Startup_shutdown_0\n");
	printf("[linux-x64] UI system shut down\n");

__exit: ;
	return __result;
};
