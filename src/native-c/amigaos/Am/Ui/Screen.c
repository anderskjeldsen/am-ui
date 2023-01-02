#ifndef native_aclass_Am_Ui_Screen_c
#define native_aclass_Am_Ui_Screen_c
#include <libc/core.h>
#include <amigaos/amiga.h>
#include <Am/Ui/Screen.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/Int.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <cybergraphics/cybergraphics.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/cybergraphics.h>

typedef struct _Am_Ui_Screen_data Am_Ui_Screen_data;
struct _Am_Ui_Screen_data {
	struct Screen * screen;	
};

function_result Am_Ui_Screen__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
	printf("TODO: implement native function Am_Ui_Screen__native_init_0\n");
__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_Screen__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;

	Am_Ui_Screen_close_0(this);

__exit:
	return __result;
};

/*
function_result const Am_Ui_Screen__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };

	__increase_reference_count(this);

	__allocate_object_data(this, sizeof(Am_Ui_Screen_data));	

	__decrease_reference_count(this);
exit:
	return __result;
};
*/


function_result Am_Ui_Screen_open_0(aobject * const this, int width, int height, int depth, int displayId)
{
	function_result __result = { .has_return_value = 0 };
	__increase_reference_count(this);

	SysBase = *((struct ExecBase **)4UL);
	printf("Intuition base: %d\n", (unsigned int) IntuitionBase);
	if (IntuitionBase == NULL) {
		IntuitionBase = (struct IntuitionBase *) __ensure_library("intuition.library", 0L);
	}


	// if (CyberGfxBase == NULL)
	// {
	// 	CyberGfxBase = (struct Library *) __ensure_library("cybergraphics.library", 0L);
	// }

	// struct TagItem cyberTags[] = {
	// 	CYBRBIDTG_Depth, depth,
	// 	CYBRBIDTG_NominalWidth, width,
	// 	CYBRBIDTG_NominalHeight, height,
	// 	TAG_DONE, TAG_DONE,
	// };

	// int displayID = BestCModeIDTagList( cyberTags );

	printf("Width: %d\n", width);
	printf("Height: %d\n", height);
	printf("Depth: %d\n", depth);

	struct TagItem screenTags[] = {
		SA_Left, 0,
		SA_Top, 0,
		SA_Width, width,
		SA_Height, height,
 		SA_Depth, depth,
//		SA_Title, (ULONG) "Hello",
		SA_DisplayID, displayId,
		SA_Type, PUBLICSCREEN,
		TAG_DONE, TAG_DONE,
	};

	struct Screen * screen = OpenScreenTagList(NULL, screenTags);

	if ( screen == NULL ) {
		printf("Unable to open screen\n");
	}

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) malloc(sizeof(Am_Ui_Screen_data));
	data->screen = screen;
	this->object_properties.class_object_properties.object_data.value.custom_value = data;

	// TODO: Implement here
exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_Screen_close_0(aobject * const this)
{
	printf("TODO: implement native function Am_Ui_Screen_close_0\n");
	function_result __result = { .has_return_value = 0 };
	__increase_reference_count(this);

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;

	if ( data != NULL && data->screen != NULL ) {
		CloseScreen(data->screen);
	}

	free(this->object_properties.class_object_properties.object_data.value.custom_value);
	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;

exit:
	__decrease_reference_count(this);
	return __result;
};

#endif
