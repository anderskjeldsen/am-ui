#ifndef native_aclass_Am_Ui_FolderRequester_c
#define native_aclass_Am_Ui_FolderRequester_c
#include <libc/core.h>
#include <amigaos/amiga.h>
#include <Am/Ui/FolderRequester.h>
#include <Am/Ui/Window.h>
#include <amigaos/Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>

#include <exec/types.h>
#include <libraries/asl.h>
#include <intuition/intuition.h>

#include <proto/exec.h>
#include <proto/asl.h>

#include <libc/core_inline_functions.h>


function_result Am_Ui_FolderRequester__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	return __result;
}

function_result Am_Ui_FolderRequester__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	return __result;
}

function_result Am_Ui_FolderRequester__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	return __result;
}

function_result Am_Ui_FolderRequester_requestFolder_0(aobject * title, aobject * initialPath, aobject * parent)
{
	function_result __result = { .has_return_value = true };
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

	// Without ASLFR_Window pointing at a live Intuition window on our
	// screen, ASL opens its requester on the default public screen.
	struct Window * parent_window = NULL;
	if (parent != NULL) {
		Am_Ui_Window_data * wd = (Am_Ui_Window_data *) parent->object_properties.class_object_properties.object_data.value.custom_value;
		if (wd != NULL) {
			parent_window = wd->window;
		}
	}

	SysBase = *((struct ExecBase **)4UL);
	if (AslBase == NULL) {
		AslBase = (struct Library *) __ensure_library("asl.library", 0L);
	}
	if (AslBase == NULL) {
		printf("FolderRequester: asl.library not available\n");
		goto __exit;
	}

	const char * title_cstr = NULL;
	if (title != NULL) {
		string_holder * sh = (string_holder *) (title + 1);
		if (sh != NULL && sh->string_value != NULL) {
			title_cstr = sh->string_value;
		}
	}
	const char * init_cstr = NULL;
	if (initialPath != NULL) {
		string_holder * sh = (string_holder *) (initialPath + 1);
		if (sh != NULL && sh->string_value != NULL && sh->string_value[0] != '\0') {
			init_cstr = sh->string_value;
		}
	}

	struct FileRequester * req = (struct FileRequester *) AllocAslRequest(ASL_FileRequest, NULL);
	if (req == NULL) {
		printf("FolderRequester: AllocAslRequest failed\n");
		goto __exit;
	}

	struct TagItem tags[] = {
		ASLFR_Window,        (ULONG) parent_window,
		ASLFR_TitleText,     (ULONG) title_cstr,
		ASLFR_InitialDrawer, (ULONG) init_cstr,
		ASLFR_DrawersOnly,   TRUE,
		ASLFR_RejectIcons,   TRUE,
		TAG_DONE,            0,
	};

	if (AslRequest(req, tags)) {
		// fr_Drawer is the selected directory string. Copy via
		// __create_string so AmLang owns its own buffer; the request
		// struct is freed below.
		__result.return_value.value.object_value = __create_string(req->fr_Drawer, &Am_Lang_String);
	}

	FreeAslRequest((APTR) req);

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

#endif
