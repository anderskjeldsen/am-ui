#include <libc/core.h>

#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Ui/Screen.h>
#include <Am/Lang/Int.h>

#include <amigaos/amiga.h>
#include <amigaos/Am/Ui/Window.h>
#include <amigaos/Am/Ui/Screen.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>

#include <libc/core_inline_functions.h>

void close_window_native(aobject * const this) {
	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	if ( data != NULL ) {		
		bool releasing = this->reference_count == 0; // diiiirty
		if (releasing) {
			this->reference_count = -100;
		}
		Am_Ui_Window_setRootView_0(this, NULL);
		if (releasing) {
			this->reference_count = 0;
		}

		if (data->window != NULL) {
			CloseWindow(data->window);
			data->window = NULL;
		}

		if (data->context_gadget != NULL) {
			FreeGadgets(data->context_gadget);
		}

		if (data->visual_info != NULL) {
			FreeVisualInfo(data->visual_info);
			data->visual_info = NULL;
		}

		if (data->locked_screen != NULL) {
			UnlockPubScreen(NULL, data->locked_screen);
			data->locked_screen = NULL;
		}
	}

	free(this->object_properties.class_object_properties.object_data.value.custom_value);
	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;

}

function_result Am_Ui_Window__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	// Add reference count for this in Window._native_init
	printf("Add reference count for this in Window._native_init\n");
	if (this != NULL) {
		__increase_reference_count(this);
	}
	// TODO: implement native function Am_Ui_Window__native_init_0
	printf("TODO: implement native function Am_Ui_Window__native_init_0\n");
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_Window__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;

	close_window_native(this);
/*
	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;

	if ( data != NULL && data->window != NULL ) {
		CloseWindow(data->window);
	}

	free(this->object_properties.class_object_properties.object_data.value.custom_value);
	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
*/
	printf("TODO: implement native function Am_Ui_Window__native_release_0\n");
__exit: ;
	return __result;
};

function_result Am_Ui_Window_open_0(aobject * const this, USHORT width, USHORT height, aobject * screen) //, aobject * screen)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	// Add reference count for this in Window.open
	printf("Add reference count for this in Window.open\n");
	if (this != NULL) {
		__increase_reference_count(this);
	}

	SysBase = *((struct ExecBase **)4UL);
	printf("Intuition base: %d\n", (unsigned int) IntuitionBase);
	if (IntuitionBase == NULL) {
		IntuitionBase = (struct IntuitionBase *) __ensure_library("intuition.library", 0L);
	}

	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) calloc(1, sizeof(Am_Ui_Window_data));
	this->object_properties.class_object_properties.object_data.value.custom_value = data;

	struct Screen *amiga_screen = NULL;
	struct VisualInfo *visual_info = NULL;

	if (screen != NULL) {
		Am_Ui_Screen_data *screen_data = (Am_Ui_Screen_data *) screen->object_properties.class_object_properties.object_data.value.custom_value;
		amiga_screen = screen_data->screen;
	} else {
		amiga_screen = LockPubScreen(NULL);
		data->locked_screen = amiga_screen;

		if (amiga_screen == NULL) {
			__throw_simple_exception("Unable lock public screen", "Am_Ui_Window_open_0, LockPubScreen", &__result);
			goto __exit;
		}
	}

	visual_info = GetVisualInfo(amiga_screen, TAG_DONE);
	if (visual_info == NULL) {
		__throw_simple_exception("Unable to get visual info", "Am_Ui_Window_open_0, GetVisualInfo", &__result);
		goto __exit;
	}

	data->visual_info = visual_info;

	data->context_gadget = NULL;
	CreateContext(&data->context_gadget);
	printf("context gadget %p\n", data->context_gadget);

	struct TagItem tags[] = {
		WA_Left, 0,
		WA_Top, 0,
		WA_Width, width,
		WA_Height, height,
		WA_DetailPen, 1,
		WA_BlockPen, 2,
		WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_GADGETUP | MENUPICK | MOUSEBUTTONS | REFRESHWINDOW | MOUSEMOVE,
		WA_Flags, WFLG_SMART_REFRESH | WFLG_ACTIVATE | WFLG_RMBTRAP | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET, //WFLG_BORDERLESS WFLG_BACKDROP
		WA_Gadgets, (ULONG) data->context_gadget,
		WA_Title, (ULONG) "Hello",
		WA_MinWidth, width,
		WA_MaxWidth, width,
		WA_MinHeight, height,
		WA_MaxHeight, height,
		WA_Checkmark, (ULONG) NULL,
		WA_ScreenTitle, (ULONG) NULL,
		WA_SuperBitMap, (ULONG) NULL,
		WA_CustomScreen, (ULONG) amiga_screen,
		TAG_END
	};

	struct Window * window = OpenWindowTagList(NULL, tags);

	if ( window == NULL )
	{
		__throw_simple_exception("Unable to open window", "Am_Ui_Window_open_0, OpenWindowTagList", &__result);
		goto __exit;
	}

	GT_RefreshWindow(window, NULL);

	data->window = window;

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (screen != NULL) {
		__decrease_reference_count(screen);
	}
	return __result;
};

function_result Am_Ui_Window_close_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	// Add reference count for this in Window.close
	printf("Add reference count for this in Window.close\n");
	if (this != NULL) {
		__increase_reference_count(this);
	}

	close_window_native(this);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_Window_handleInput_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	struct Window * win = window_data->window;
	ULONG sig_mask = 1L << window_data->window->UserPort->mp_SigBit;
	printf("Wait %d\n", sig_mask);
	ULONG signals = Wait(sig_mask);
	printf("Wait done %d\n", signals);

	BOOL close_window = FALSE;
//	printf("Wait done %d, %d\n", sig_mask, signals & sig_mask);
	if (TRUE) // signals & sig_mask)
	{
		struct IntuiMessage *msg;
		while ((msg = (struct IntuiMessage *)GT_GetIMsg(window_data->window->UserPort)) != NULL)
		{
			switch (msg->Class)
			{
				case IDCMP_CLOSEWINDOW:
					close_window = true;
					break;

				case IDCMP_REFRESHWINDOW:
					GT_BeginRefresh(window_data->window);
					/* custom rendering, if any, goes here */
					GT_EndRefresh(window_data->window, TRUE);
					break;
				case IDCMP_NEWSIZE:
					Am_Ui_Window_onResize_0(this, win->LeftEdge, win->TopEdge, win->Width, win->Height);
					GT_RefreshWindow(win, NULL);
					break;
			}
			GT_ReplyIMsg((struct IntuiMessage *)msg);
		}
	}

	if (close_window) {
		close_window_native(this);
	}

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_Window_isOpen_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	bool __returning = true;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	__result.return_value.value.bool_value = this->object_properties.class_object_properties.object_data.value.custom_value != NULL;
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_Window_refresh_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	GT_RefreshWindow(data->window, NULL);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};
