#ifndef native_aclass_Am_Ui_ScreenModeRequester_c
#define native_aclass_Am_Ui_ScreenModeRequester_c
#include <libc/core.h>
#include <morphos-ppc/morphos.h>
#include <Am/Ui/ScreenModeRequester.h>
#include <Am/Lang/Object.h>
#include <Am/Ui/ScreenMode.h>
#include <Am/Lang/Int.h>

#include <exec/types.h>

#include <proto/exec.h>
#include <proto/asl.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>

#include <libc/core_inline_functions.h>


typedef struct _Am_Ui_ScreenModeRequester_data Am_Ui_ScreenModeRequester_data;
struct _Am_Ui_ScreenModeRequester_data {
	struct ScreenModeRequester * screenModeRequester;
	ULONG min_width;
	ULONG min_height;
	ULONG min_depth;

	ULONG max_width;
	ULONG max_height;
	ULONG max_depth;

	ULONG desired_width;
	ULONG desired_height;
	ULONG desired_depth;
};

function_result Am_Ui_ScreenModeRequester__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
	printf("TODO: implement native function Am_Ui_ScreenModeRequester__native_init_0\n");

	SysBase = *((struct ExecBase **)4UL);
	printf("Asl base: %d\n", (unsigned int) IntuitionBase);
	if (AslBase == NULL) {
		AslBase = (struct Library *) __ensure_library("asl.library", 0L);
	}

	// TODO: replace [0] with a way to get a named property
	aobject * title = this->object_properties.class_object_properties.properties[0].nullable_value.value.object_value;
	string_holder *title_holder = title->object_properties.class_object_properties.object_data.value.custom_value;

	struct TagItem reqTypes[] = 
	{
		ASLSM_Window, (ULONG) NULL,
		ASLSM_PubScreenName, (ULONG) NULL,
		ASLSM_Screen, (ULONG) NULL,
		ASLSM_PrivateIDCMP, FALSE,
		ASLSM_TitleText, (ULONG) title_holder->string_value,
		ASLSM_InitialWidth, 256,
		ASLSM_InitialHeight, 200,
		ASLSM_InitialDisplayWidth, 0,
		ASLSM_InitialDisplayHeight, 0,
		ASLSM_InitialDisplayDepth, 0,
		ASLSM_DoWidth, TRUE,	
		ASLSM_DoHeight, TRUE,
		ASLSM_DoDepth, TRUE,
		ASLSM_MinWidth, 0,
		ASLSM_MinHeight, 0,
		ASLSM_MinDepth, 0,
		ASLSM_MaxWidth, 0,
		ASLSM_MaxHeight, 0,
		ASLSM_MaxDepth, 0,
		TAG_DONE,
	};

	Am_Ui_ScreenModeRequester_data * data = malloc(sizeof(Am_Ui_ScreenModeRequester_data));
	this->object_properties.class_object_properties.object_data.value.custom_value = data;

	data->screenModeRequester = (struct ScreenModeRequester *) AllocAslRequest(ASL_ScreenModeRequest, reqTypes);
	if (data->screenModeRequester == NULL) {
		printf("Unable to open ScreenModeRequester");
	}
__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_ScreenModeRequester__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	printf("TODO: implement native function Am_Ui_ScreenModeRequester__native_release_0\n");

	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	FreeAslRequest((APTR)data->screenModeRequester);
	free(data);

	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;

__exit:
	return __result;
};

function_result Am_Ui_ScreenModeRequester_setMin_0(aobject * const this, int width, int height, int depth)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
	printf("Am_Ui_ScreenModeRequester_setMin_0: %d x %d x %d\n", width, height, depth);

	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	data->min_width = width;
	data->min_height = height;
	data->min_depth = depth;

__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_ScreenModeRequester_setDesired_0(aobject * const this, int width, int height, int depth)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
//	printf("Am_Ui_ScreenModeRequester_setDesired_0: %dx%d\n", width, height);

	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	data->desired_width = width;
	data->desired_height = height;
	data->desired_depth = depth;

__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_ScreenModeRequester_setMax_0(aobject * const this, int width, int height, int depth)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
//	printf("Am_Ui_ScreenModeRequester_setMax_0: %dx%d\n", width, height);

	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	data->max_width = width;
	data->max_height = height;
	data->max_depth = depth;

__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_ScreenModeRequester_open_0(aobject * const this)
{
	function_result __result = { .has_return_value = 1 };
	char __returning = 0;
	__increase_reference_count(this);
	printf("TODO: implement native function Am_Ui_ScreenModeRequester_open_0\n");

	Am_Ui_ScreenModeRequester_data * data = (Am_Ui_ScreenModeRequester_data *) this->object_properties.class_object_properties.object_data.value.custom_value;

	struct TagItem reqTypes[] = 
	{
		ASLSM_InitialDisplayWidth, data->desired_width,
		ASLSM_InitialDisplayHeight, data->desired_width,
		ASLSM_InitialDisplayDepth, data->desired_width,
		ASLSM_MinWidth, data->min_width,
		ASLSM_MinHeight, data->min_width,
		ASLSM_MinDepth, data->min_width,
		ASLSM_MaxWidth, data->max_width,
		ASLSM_MaxHeight, data->max_width,
		ASLSM_MaxDepth, data->max_width,
		TAG_DONE, TAG_DONE
	};

	if ( AslRequest((APTR)data->screenModeRequester, reqTypes) )
	{
		printf("  createScreenMode %d\n", data->screenModeRequester->sm_DisplayWidth);
		__result = Am_Ui_ScreenModeRequester_createScreenMode_0(this, data->screenModeRequester->sm_DisplayWidth, data->screenModeRequester->sm_DisplayHeight, data->screenModeRequester->sm_DisplayDepth, data->screenModeRequester->sm_DisplayID);
	}
	else {
	
		__result.return_value.value.object_value = NULL;
	}

__exit:
	__decrease_reference_count(this);
	return __result;
};

#endif
