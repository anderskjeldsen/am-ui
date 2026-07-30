#ifndef native_aclass_Am_Ui_FileRequester_c
#define native_aclass_Am_Ui_FileRequester_c
#include <libc/core.h>
#include <amigaos/amiga.h>
#include <Am/Ui/FileRequester.h>
#include <Am/Ui/Window.h>
#include <amigaos/Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>

#include <exec/types.h>
#include <libraries/asl.h>
#include <intuition/intuition.h>

#include <proto/exec.h>
#include <proto/asl.h>

#include <string.h>

#include <libc/core_inline_functions.h>


function_result Am_Ui_FileRequester__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	return __result;
}

function_result Am_Ui_FileRequester__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	return __result;
}

function_result Am_Ui_FileRequester__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	return __result;
}

function_result Am_Ui_FileRequester_requestFile_0(aobject * title, aobject * initialPath, aobject * parent)
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

	struct Window * parent_window = NULL;
	if (parent != NULL) {
		Am_Ui_Window_data * wd = (Am_Ui_Window_data *) __unwrap(parent)->object_properties.class_object_properties.object_data.value.custom_value;
		if (wd != NULL) {
			parent_window = wd->window;
		}
	}

	SysBase = *((struct ExecBase **)4UL);
	if (AslBase == NULL) {
		AslBase = (struct Library *) __ensure_library("asl.library", 0L);
	}
	if (AslBase == NULL) {
		printf("FileRequester: asl.library not available\n");
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
		printf("FileRequester: AllocAslRequest failed\n");
		goto __exit;
	}

	// File mode: DrawersOnly FALSE so the user picks a file. Reject
	// .info icons; open (not save) mode.
	struct TagItem tags[] = {
		ASLFR_Window,        (ULONG) parent_window,
		ASLFR_TitleText,     (ULONG) title_cstr,
		ASLFR_InitialDrawer, (ULONG) init_cstr,
		ASLFR_DrawersOnly,   FALSE,
		ASLFR_DoSaveMode,    FALSE,
		ASLFR_RejectIcons,   TRUE,
		TAG_DONE,            0,
	};

	if (AslRequest(req, tags)) {
		const char * drawer = (req->fr_Drawer != NULL) ? req->fr_Drawer : "";
		const char * file   = (req->fr_File   != NULL) ? req->fr_File   : "";
		if (file[0] != '\0') {
			// Join drawer + file into a full AmigaDOS path. A drawer
			// ending in ':' or '/' already carries its separator;
			// otherwise insert a '/'.
			char full[512];
			size_t len = strlen(drawer);
			if (len >= sizeof(full)) { len = sizeof(full) - 1; }
			memcpy(full, drawer, len);
			full[len] = '\0';
			if (len > 0) {
				char last = full[len - 1];
				if (last != ':' && last != '/' && len < sizeof(full) - 1) {
					full[len] = '/';
					len = len + 1;
					full[len] = '\0';
				}
			}
			strncat(full, file, sizeof(full) - strlen(full) - 1);
			__result.return_value.value.object_value = __create_string(full, &Am_Lang_String);
		}
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
