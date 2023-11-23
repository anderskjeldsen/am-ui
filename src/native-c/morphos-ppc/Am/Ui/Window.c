#include <libc/core.h>

#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Ui/Screen.h>
#include <Am/Lang/Int.h>

#include <morphos-ppc/morphos.h>
#include <morphos-ppc/Am/Ui/Window.h>
#include <morphos-ppc/Am/Ui/Screen.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>

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

UBYTE calculate_pixel_scale_x(USHORT screen_width, USHORT screen_height) {
	USHORT xs = screen_width / screen_height;
	if (xs == 0) {
		xs = 1;
	}
	return (UBYTE) xs;
}

UBYTE calculate_pixel_scale_y(USHORT screen_width, USHORT screen_height) {
	USHORT ys = screen_height / screen_width;
	if (ys == 0) {
		ys = 1;
	}
	return (UBYTE) ys;
}


function_result Am_Ui_Window_open_0(aobject * const this, SHORT x, SHORT y, USHORT width, USHORT height, aobject * screen, aobject * windowManager) //, aobject * screen)
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

/*
	ULONG user_port = (ULONG) NULL;
	if (windowManager != NULL) {
		struct MsgPort * msg_port = windowManager->object_properties.class_object_properties.object_data.value.custom_value;
		user_port = (ULONG) msg_port;
	}
*/
	struct TagItem tags[] = {
		WA_Left, x,
		WA_Top, y,
		WA_Width, width,
		WA_Height, height,
		WA_DetailPen, 1,
		WA_BlockPen, 2,
		WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_GADGETUP | MENUPICK | MOUSEBUTTONS | REFRESHWINDOW | IDCMP_INTUITICKS | IDCMP_NEWSIZE | IDCMP_MOUSEBUTTONS,
		WA_Flags, WFLG_SIZEGADGET | WFLG_ACTIVATE | WFLG_RMBTRAP | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_SIMPLE_REFRESH | WFLG_SIZEBBOTTOM, //WFLG_BORDERLESS WFLG_BACKDROP
		WA_Gadgets, 0, // (ULONG) data->context_gadget,
		WA_Title, (ULONG) "Hello",
		WA_MinWidth, 0,
		WA_MaxWidth, 1000,
		WA_MinHeight, 0,
		WA_MaxHeight, 1000,
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

	// if we want a 5 pixel padding around a view, then:
	// 320x256: x: 5x1, y:5x1
	// 640x256: x: 5x2, y: 5x1
	// 320x512: x: 5x1, y: 5x2

	this->object_properties.class_object_properties.properties[Am_Ui_Window_P_pixelScaleX].nullable_value.value.uchar_value = calculate_pixel_scale_x(window->WScreen->Width, window->WScreen->Height);
	this->object_properties.class_object_properties.properties[Am_Ui_Window_P_pixelScaleY].nullable_value.value.uchar_value = calculate_pixel_scale_y(window->WScreen->Width, window->WScreen->Height);

	Am_Ui_Window_setBorder_0(this, window->BorderLeft, window->BorderTop, window->BorderRight, window->BorderBottom);
	Am_Ui_Window_onResize_0(this, window->LeftEdge, window->TopEdge, window->Width, window->Height);

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

void handle_message(aobject * this, struct IntuiMessage * msg) {
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	struct Window * win = window_data->window;

//	printf("Handle message %d\n", msg->Class);

	switch (msg->Class)
	{
		case IDCMP_CLOSEWINDOW:
			window_data->pending_close = TRUE;
			break;

		case IDCMP_REFRESHWINDOW:
//			printf("handle refresh window\n");
			if (msg->Code == 1) {
				window_data->pending_full_refresh = TRUE;
			} else {
				window_data->pending_refresh = TRUE;
			}
			break;
		case IDCMP_NEWSIZE:
//			printf("Resize %dx%d\n", win->Width, win->Height);
	//					Am_Ui_Window_setBorder_0(this, win->BorderLeft, win->BorderTop, win->BorderRight, win->BorderBottom);
			window_data->pending_resize= TRUE;
//			Am_Ui_Window_onResize_0(this, win->LeftEdge, win->TopEdge, win->Width, win->Height);
	//					GT_RefreshWindow(win, NULL);
			break;
		case IDCMP_MOUSEBUTTONS:
//			printf("Mouse click %d\n", msg->Code);
			if (msg->Code == MENUUP) // Check for right mouse button // IECODE_RBUTTON
			{
				Am_Ui_Window_onMouseEvent_0(this, 2, 2, msg->MouseX, msg->MouseY);
			} else if (msg->Code == SELECTUP) {
				Am_Ui_Window_onMouseEvent_0(this, 2, 1, msg->MouseX, msg->MouseY);
			} else if (msg->Code == MENUDOWN) // Check for right mouse button
			{
				Am_Ui_Window_onMouseEvent_0(this, 3, 2, msg->MouseX, msg->MouseY);
			} else if (msg->Code == SELECTDOWN) {
				Am_Ui_Window_onMouseEvent_0(this, 3, 1, msg->MouseX, msg->MouseY);
			}
			break;
		case IDCMP_MOUSEMOVE:
		case IDCMP_INTUITICKS:
			if (msg->MouseX != window_data->last_mouse_x && msg->MouseY != window_data->last_mouse_y) {
				window_data->last_mouse_x = msg->MouseX;
				window_data->last_mouse_y = msg->MouseY;
				Am_Ui_Window_onMouseEvent_0(this, 1, 0, msg->MouseX, msg->MouseY);
			}
			break;
	}
}

function_result Am_Ui_Window_handleInput_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	struct Window * win = window_data->window;

	window_data->pending_close = FALSE;
	window_data->pending_refresh = FALSE;
	window_data->pending_full_refresh = FALSE;
	window_data->pending_resize = FALSE;

	ULONG sig_mask = 1L << window_data->window->UserPort->mp_SigBit;
//	printf("Wait %d\n", sig_mask);
	ULONG signals = Wait(sig_mask);
//	printf("Wait done %d\n", signals);

//	printf("Wait done %d, %d\n", sig_mask, signals & sig_mask);
	if (signals & sig_mask ) // )
	{
		struct IntuiMessage *msg;
		while ((msg = (struct IntuiMessage *)GetMsg(window_data->window->UserPort)) != NULL)
		{
//			printf("Handle msg %d %p\n", msg->Class, msg);

			handle_message(this, msg);

			ReplyMsg((struct Message *) msg);				
		}
	}

//	printf("Handling done %d\n", signals);

	if (window_data->pending_resize) {
		Am_Ui_Window_onResize_0(this, win->LeftEdge, win->TopEdge, win->Width, win->Height);
	}

	if (window_data->pending_full_refresh) {
		Am_Ui_Window_paint_0(this);
	}
	else if (window_data->pending_refresh) {
		printf("handle pending refresh\n");

		BeginRefresh(window_data->window);
		Am_Ui_Window_paint_0(this);
		EndRefresh(window_data->window, TRUE);
	}

/*
	if (window_data->pending_repaint) {
		Am_Ui_Window_paint_0(this);
	}
*/
	if (window_data->pending_close) {
		Am_Ui_Window_onCloseButtonClick_0(this);
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
	struct Window *window = data->window;
//	EraseRect(window->RPort, 0, 0, window->Width - 1, window->Height - 1); // Erase old content

//    struct IntuiMessage refreshMsg;
	Forbid();
	struct IntuiMessage *refresh_msg = &data->refresh_msg;
	memset(refresh_msg, 0, sizeof(struct IntuiMessage));
    refresh_msg->Class = IDCMP_REFRESHWINDOW;
    refresh_msg->Code = 1;
    PutMsg(window->UserPort, (struct Message *)refresh_msg);
	Permit();

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};
