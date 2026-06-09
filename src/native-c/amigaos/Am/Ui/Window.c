#include <libc/core.h>

#include <Am/Ui/Window.h>
#include <Am/Ui/WindowProperties.h>
#include <Am/Ui/MenuItem.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>
#include <Am/Ui/Screen.h>
#include <Am/Lang/Int.h>

#include <amigaos/amiga.h>
#include <amigaos/Am/Ui/Window.h>
#include <amigaos/Am/Ui/Screen.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <devices/keymap.h>
#include <devices/console.h>
#include <devices/inputevent.h>
#include <devices/clipboard.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/keymap.h>
#include <proto/layers.h>

#include <libc/core_inline_functions.h>

static void do_full_close(aobject * const this) {
	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	if ( data != NULL ) {
		bool releasing = this->reference_count == 0; // diiiirty
		if (releasing) {
			this->reference_count = -100;
		}
		Am_Ui_Window_f_setRootView_0(this, NULL);
		if (releasing) {
			this->reference_count = 0;
		}

		if (data->window != NULL) {
			if (data->menu_strip != NULL) {
				ClearMenuStrip(data->window);
			}
			CloseWindow(data->window);
			data->window = NULL;
		}

		if (data->menu_strip != NULL) {
			FreeMenus(data->menu_strip);
			data->menu_strip = NULL;
		}
		if (data->menu_new_menus != NULL) {
			for (int i = 0; data->menu_new_menus[i].nm_Type != NM_END; i++) {
				if (data->menu_new_menus[i].nm_Label != NULL && data->menu_new_menus[i].nm_Label != NM_BARLABEL) {
					free((void *) data->menu_new_menus[i].nm_Label);
				}
				if (data->menu_new_menus[i].nm_CommKey != NULL) {
					free((void *) data->menu_new_menus[i].nm_CommKey);
				}
			}
			free(data->menu_new_menus);
			data->menu_new_menus = NULL;
		}
		for (int i = 0; i < data->menu_item_count; i++) {
			if (data->menu_item_aobjects[i] != NULL) {
				__decrease_reference_count(data->menu_item_aobjects[i]);
				data->menu_item_aobjects[i] = NULL;
			}
		}
		data->menu_item_count = 0;
		if (data->menu_builder != NULL) {
			free(data->menu_builder);
			data->menu_builder = NULL;
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

		if (data->clip_region != NULL) {
			DisposeRegion(data->clip_region);
			data->clip_region = NULL;
		}
	}

	free(this->object_properties.class_object_properties.object_data.value.custom_value);
	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;

}

void close_window_native(aobject * const this) {
	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data == NULL) {
		return;
	}
	if (data->inside_handle_input) {
		// Inside Intuition's message dispatch — closing now would free the
		// window before the current msg gets ReplyMsg'd, corrupting state.
		// Defer to handleInput's post-loop tail.
		data->deferred_full_close = true;
		return;
	}
	do_full_close(this);
}

function_result Am_Ui_Window__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	// Add reference count for this in Window._native_init
	if (this != NULL) {
		__increase_reference_count(this);
	}
	// TODO: implement native function Am_Ui_Window__native_init_0
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

__exit: ;
	return __result;
};

function_result Am_Ui_Window__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

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


function_result Am_Ui_Window_open_0(aobject * const this, SHORT x, SHORT y, USHORT width, USHORT height, aobject * screen, aobject * windowManager, struct Am_Ui_WindowProperties * properties)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	// Add reference count for this in Window.open
	if (this != NULL) {
		__increase_reference_count(this);
	}

	bool borderless = (properties != NULL) ? properties->borderless : false;
	// WFLG_RMBTRAP is intentionally NOT set: Intuition needs the right
	// mouse button to pop up the menu strip when one is installed.
	ULONG window_flags = borderless
		? (WFLG_ACTIVATE | WFLG_BORDERLESS | WFLG_SMART_REFRESH)
		: (WFLG_SIZEGADGET | WFLG_ACTIVATE | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_SMART_REFRESH | WFLG_SIZEBBOTTOM);

	SysBase = *((struct ExecBase **)4UL);
	if (IntuitionBase == NULL) {
		IntuitionBase = (struct IntuitionBase *) __ensure_library("intuition.library", 0L);
	}

	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) calloc(1, sizeof(Am_Ui_Window_data));
	this->object_properties.class_object_properties.object_data.value.custom_value = data;
	data->clip_region = NewRegion();

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
//	printf("context gadget %p\n", data->context_gadget);

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
		// IDCMP_EXTENDEDMOUSE (added in OS 3.5 / V45) carries
		// mouse-wheel events as IDCMP_MOUSEMOVE messages whose
		// Code field holds IMSGCODE_INTUIWHEELDATA and whose
		// IAddress points to an IntuiWheelData struct. The amiga-gcc
		// 3.2/3.5 NDK declares both — on pre-V45 systems Intuition
		// silently ignores the unknown flag, so the rest of the
		// IDCMP mask still works.
		// IDCMP_MENUVERIFY pairs with IDCMP_MENUPICK to mark when a
		// menu is active so the mouse-event branches below can
		// suppress events that fire during menu interaction (stale
		// hover, phantom clicks when the menu closes). Replying to
		// MENUVERIFY with msg->Code = MENUHOT lets the menu open.
		WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_GADGETUP | MENUPICK | MOUSEBUTTONS | REFRESHWINDOW | IDCMP_INTUITICKS | IDCMP_NEWSIZE | IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY | IDCMP_EXTENDEDMOUSE | IDCMP_MENUVERIFY,
		WA_Flags, window_flags,
		WA_Borderless, borderless ? TRUE : FALSE,
		WA_Gadgets, 0, // (ULONG) data->context_gadget,
		WA_Title, borderless ? (ULONG) NULL : (ULONG) "Hello",
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

	data->window = window;

	// if we want a 5 pixel padding around a view, then:
	// 320x256: x: 5x1, y:5x1
	// 640x256: x: 5x2, y: 5x1
	// 320x512: x: 5x1, y: 5x2

	this->object_properties.class_object_properties.properties[Am_Ui_Window_P_pixelScaleX].nullable_value.value.uchar_value = calculate_pixel_scale_x(window->WScreen->Width, window->WScreen->Height);
	this->object_properties.class_object_properties.properties[Am_Ui_Window_P_pixelScaleY].nullable_value.value.uchar_value = calculate_pixel_scale_y(window->WScreen->Width, window->WScreen->Height);

	Am_Ui_Window_f_setBorder_0(this, window->BorderLeft, window->BorderTop, window->BorderRight, window->BorderBottom);
	Am_Ui_Window_f_onResize_0(this, window->LeftEdge, window->TopEdge, window->Width, window->Height);

	GT_RefreshWindow(window, NULL);

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
//	printf("Add reference count for this in Window.close\n");
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
		case IDCMP_MENUVERIFY:
			// Intuition fires this before showing the menu. Acking
			// with MENUHOT lets the menu open; MENUCANCEL would
			// suppress it. We always accept and just flip the
			// menu_active gate so the mouse paths below stop
			// publishing events to the AmLang view tree.
			window_data->menu_active = TRUE;
			msg->Code = MENUHOT;
			break;
		case IDCMP_MENUPICK:
			// MENUPICK arrives whether the user picked an item
			// or cancelled — either way the menu is gone, so
			// clear the gate before dispatching the selection.
			window_data->menu_active = FALSE;
			{
				USHORT code = msg->Code;
				while (code != MENUNULL && window_data->menu_strip != NULL) {
					struct MenuItem *menu_item = ItemAddress(window_data->menu_strip, code);
					if (menu_item == NULL) {
						break;
					}
					aobject *aml_item = (aobject *) GTMENUITEM_USERDATA(menu_item);
					if (aml_item != NULL) {
						Am_Ui_MenuItem_f_invokeClick_0(aml_item);
					}
					code = menu_item->NextSelect;
				}
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
			// Drop button events while a menu is up — the explicit
			// menu_active flag covers MagicMenu popups; WFLG_MENUSTATE
			// covers stock Intuition. Either set means the click
			// belongs to the menu interaction, not the view tree.
			if (window_data->menu_active || (win->Flags & WFLG_MENUSTATE)) {
				break;
			}
			if (msg->Code == MENUUP) // Check for right mouse button // IECODE_RBUTTON
			{
				Am_Ui_Window_f_onMouseEvent_0(this, 2, 2, msg->MouseX, msg->MouseY);
			} else if (msg->Code == SELECTUP) {
				Am_Ui_Window_f_onMouseEvent_0(this, 2, 1, msg->MouseX, msg->MouseY);
			} else if (msg->Code == MENUDOWN) // Check for right mouse button
			{
				Am_Ui_Window_f_onMouseEvent_0(this, 3, 2, msg->MouseX, msg->MouseY);
			} else if (msg->Code == SELECTDOWN) {
				Am_Ui_Window_f_onMouseEvent_0(this, 3, 1, msg->MouseX, msg->MouseY);
			}
			break;
		case IDCMP_MOUSEMOVE:
		case IDCMP_INTUITICKS:
			// Same menu-up gate as MOUSEBUTTONS — but wheel events
			// are checked first because they're carried over
			// IDCMP_MOUSEMOVE and we want wheel scroll to keep
			// working even if a menu somehow leaks one through.
			if (msg->Code == IMSGCODE_INTUIWHEELDATA && msg->IAddress != NULL) {
				// IDCMP_EXTENDEDMOUSE delivers wheel events as
				// IDCMP_MOUSEMOVE messages tagged with
				// IMSGCODE_INTUIWHEELDATA in Code; msg->IAddress
				// points at a struct IntuiWheelData. We skip the
				// cursor-tracking path on wheel events — wheel msgs
				// can carry MouseX/Y matching the last position but
				// the axis we care about is wd->WheelY.
				struct IntuiWheelData *wd = (struct IntuiWheelData *) msg->IAddress;
				if (wd->WheelY != 0) {
					Am_Ui_Window_f_onMouseWheel_0(this, wd->WheelY, msg->MouseX, msg->MouseY);
				}
				break;
			}
			if (window_data->menu_active || (win->Flags & WFLG_MENUSTATE)) {
				break;
			}
			if (msg->MouseX != window_data->last_mouse_x || msg->MouseY != window_data->last_mouse_y) {
				window_data->last_mouse_x = msg->MouseX;
				window_data->last_mouse_y = msg->MouseY;
				Am_Ui_Window_f_onMouseEvent_0(this, 1, 0, msg->MouseX, msg->MouseY);
			}
			break;
		case IDCMP_RAWKEY:
			{
				// Check if this is a key press (bit 7 clear) or key release (bit 7 set)
				if (msg->Code & 0x80) {
					// Key release event (code + 128) - ignore for now
//					printf("onKey, key release: raw code %d (press code: %d)\n", msg->Code, msg->Code & 0x7F);
					// Optionally call keyboard event handler for key release
					// Am_Ui_Window_f_onKeyboardEvent_0(this, 2, msg->Code & 0x7F, 0); // type 2 = key release
					break;
				}
				
				// Key press event - handle special keys first, then convert others to ASCII
				UBYTE key_buffer[8];
				WORD actual_length = 0;
				UBYTE ascii_char = 0;
				
				// Check for known special keys first (AmigaOS raw key codes)
				if (msg->Code == 65) { // Backspace key on Amiga
//					printf("onKey, key press: backspace (raw code %d)\n", msg->Code);
					Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 8); // Pass ASCII 8 for backspace
				} else if (msg->Code == 70) { // Delete key on Amiga
//					printf("onKey, key press: delete (raw code %d)\n", msg->Code);
					Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 127); // Pass ASCII 127 for delete
				} else if (msg->Code == 79) { // Left arrow
//					printf("onKey, key press: left arrow (raw code %d)\n", msg->Code);
					// Check for Shift modifier (IEQUALIFIER_LSHIFT or IEQUALIFIER_RSHIFT)
					if (msg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)) {
						Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 1); // Pass 1 for Shift+Left
					} else {
						Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 0); // No ASCII equivalent
					}
				} else if (msg->Code == 78) { // Right arrow
//					printf("onKey, key press: right arrow (raw code %d)\n", msg->Code);
					// Check for Shift modifier
					if (msg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)) {
						Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 1); // Pass 1 for Shift+Right
					} else {
						Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 0); // No ASCII equivalent
					}
				} else if (msg->Code == 76) { // Up arrow
//					printf("onKey, key press: up arrow (raw code %d)\n", msg->Code);
					// Check for Shift modifier
					if (msg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)) {
						Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 1); // Pass 1 for Shift+Up
					} else {
						Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 0); // No ASCII equivalent
					}
				} else if (msg->Code == 77) { // Down arrow
//					printf("onKey, key press: down arrow (raw code %d)\n", msg->Code);
					// Check for Shift modifier
					if (msg->Qualifier & (IEQUALIFIER_LSHIFT | IEQUALIFIER_RSHIFT)) {
						Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 1); // Pass 1 for Shift+Down
					} else {
						Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 0); // No ASCII equivalent
					}
				} else {
					// Try to convert other keys to ASCII using MapRawKey
					struct InputEvent input_event;
					input_event.ie_Class = IECLASS_RAWKEY;
					input_event.ie_Code = msg->Code;
					input_event.ie_Qualifier = msg->Qualifier;
					input_event.ie_position.ie_addr = 0;
					
					actual_length = MapRawKey(&input_event, key_buffer, 8, NULL);
					
					if (actual_length > 0) {
						// We got a printable character
						ascii_char = key_buffer[0];
//						printf("onKey, key press: raw code %d -> ASCII '%c' (%d)\n", msg->Code, ascii_char, ascii_char);
						Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, ascii_char);
					} else {
						// Other special key (function keys, etc.)
//						printf("onKey, key press: special key code %d (no ASCII equivalent)\n", msg->Code);
						Am_Ui_Window_f_onKeyboardEvent_0(this, 1, msg->Code, 0);
					}
				}
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
	window_data->deferred_full_close = false;

	ULONG sig_mask = 1L << window_data->window->UserPort->mp_SigBit;
//	printf("Wait %d\n", sig_mask);
	ULONG signals = Wait(sig_mask);
//	printf("Wait done %d\n", signals);

//	printf("Wait done %d, %d\n", sig_mask, signals & sig_mask);
	window_data->inside_handle_input = true;
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
	window_data->inside_handle_input = false;

	if (window_data->deferred_full_close) {
		// A click handler asked to close the window mid-dispatch. Now that
		// every message has been ReplyMsg'd, it's safe to actually tear
		// down. Bail out before touching window_data — it's freed below.
		do_full_close(this);
		goto __exit;
	}

//	printf("Handling done %d\n", signals);

	if (window_data->pending_resize) {
//		window_data->old_clip_region = InstallClipRegion(win->WLayer, NULL);
		Am_Ui_Window_f_onResize_0(this, win->LeftEdge, win->TopEdge, win->Width, win->Height);
		window_data->pending_full_refresh = TRUE;
		//		InstallClipRegion(win->WLayer, window_data->old_clip_region);
	}

	if (window_data->pending_full_refresh) {
		WaitTOF();  // Wait for VBL to avoid raster beam interference
		WaitTOF();
		window_data->old_clip_region = InstallClipRegion(win->WLayer, NULL);
		Am_Ui_Window_f_paint_0(this);
		InstallClipRegion(win->WLayer, window_data->old_clip_region);
	}
	else if (window_data->pending_refresh) {
//		printf("handle pending refresh\n");

		WaitTOF();
		WaitTOF();
//		LockLayerInfo(&win->WScreen->LayerInfo);
		window_data->old_clip_region = InstallClipRegion(win->WLayer, NULL);	
		BeginRefresh(window_data->window);
		Am_Ui_Window_f_paint_0(this);
		EndRefresh(window_data->window, TRUE);
		InstallClipRegion(win->WLayer, window_data->old_clip_region);	

//		UnlockLayerInfo(&win->WScreen->LayerInfo);
	}

/*
	if (window_data->pending_repaint) {
		Am_Ui_Window_paint_0(this);
	}
*/
	if (window_data->pending_close) {
		Am_Ui_Window_f_onCloseButtonClick_0(this);
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

function_result Am_Ui_Window_getHostScreenWidth_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	__increase_reference_count(this);
	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	unsigned short v = 0;
	if (data != NULL && data->window != NULL && data->window->WScreen != NULL) {
		v = (unsigned short) data->window->WScreen->Width;
	}
	__result.return_value.value.ushort_value = v;
	__decrease_reference_count(this);
	return __result;
}

function_result Am_Ui_Window_getHostScreenHeight_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	__increase_reference_count(this);
	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	unsigned short v = 0;
	if (data != NULL && data->window != NULL && data->window->WScreen != NULL) {
		v = (unsigned short) data->window->WScreen->Height;
	}
	__result.return_value.value.ushort_value = v;
	__decrease_reference_count(this);
	return __result;
}

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

// Return the task pointer of the task whose Wait() is blocked on the
// window's UserPort signal bit. RunningProcess uses this to Signal()
// directly when bebbossh writes output, bypassing the AmLang polling
// cycle entirely so streams (ping, tail -f, shell prompts) appear in
// the panel as soon as bytes hit the ring buffer.
function_result Am_Ui_Window_getUserPortTaskPtr_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	if (this != NULL) __increase_reference_count(this);
	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const)
		this->object_properties.class_object_properties.object_data.value.custom_value;
	LONG ptr = 0;
	if (data != NULL && data->window != NULL && data->window->UserPort != NULL) {
		ptr = (LONG) data->window->UserPort->mp_SigTask;
	}
	__result.return_value.value.long_value = ptr;
	if (this != NULL) __decrease_reference_count(this);
	return __result;
}

function_result Am_Ui_Window_getUserPortSigBit_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	if (this != NULL) __increase_reference_count(this);
	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const)
		this->object_properties.class_object_properties.object_data.value.custom_value;
	LONG bit = -1;
	if (data != NULL && data->window != NULL && data->window->UserPort != NULL) {
		bit = (LONG) data->window->UserPort->mp_SigBit;
	}
	__result.return_value.value.int_value = bit;
	if (this != NULL) __decrease_reference_count(this);
	return __result;
}

function_result Am_Ui_Window_setTitleNative_0(aobject * const this, aobject * const windowTitle, aobject * const screenTitle)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (windowTitle != NULL) {
		__increase_reference_count(windowTitle);
	}
	if (screenTitle != NULL) {
		__increase_reference_count(screenTitle);
	}

	Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data != NULL && data->window != NULL) {
		// Extract window title string
		char *windowTitleStr = NULL;
		if (windowTitle != NULL) {
			string_holder *windowTitleSh = windowTitle->object_properties.class_object_properties.object_data.value.custom_value;
			if (windowTitleSh != NULL && windowTitleSh->string_value != NULL) {
				windowTitleStr = windowTitleSh->string_value;
			}
		}

		// Extract screen title string  
		char *screenTitleStr = NULL;
		if (screenTitle != NULL) {
			string_holder *screenTitleSh = screenTitle->object_properties.class_object_properties.object_data.value.custom_value;
			if (screenTitleSh != NULL && screenTitleSh->string_value != NULL && screenTitleSh->length > 0) {
				screenTitleStr = screenTitleSh->string_value;
			}
		}

		// Set both titles using AmigaOS SetWindowTitles
		// If screenTitleStr is NULL, pass (UBYTE *)-1 to leave screen title unchanged
		SetWindowTitles(data->window, windowTitleStr, 
			screenTitleStr);
	}

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (windowTitle != NULL) {
		__decrease_reference_count(windowTitle);
	}
	if (screenTitle != NULL) {
		__decrease_reference_count(screenTitle);
	}
	return __result;
};

// Simple clipboard storage - in real AmigaOS this would use clipboard.device
static char* stored_clipboard_text = NULL;

// Clipboard functionality for AmigaOS
function_result Am_Ui_Window_copyToClipboard_0(aobject * const this, aobject * const text)
{
	function_result __result = { .has_return_value = false };
	
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (text != NULL) {
		__increase_reference_count(text);
	}

	if (text == NULL) {
		goto __exit;
	}

	// Get the string content from the AmLang String object
	if (text->object_properties.class_object_properties.object_data.value.custom_value != NULL) {
		string_holder* holder = (string_holder*)text->object_properties.class_object_properties.object_data.value.custom_value;
		if (holder->string_value != NULL) {
			// Free previous clipboard content
			if (stored_clipboard_text != NULL) {
				free(stored_clipboard_text);
			}
			
			// Store a copy of the text
			stored_clipboard_text = strdup(holder->string_value);
			printf("Copy to clipboard: '%s'\n", stored_clipboard_text);
		}
	}
	
	// TODO: Implement actual clipboard device access
	// The AmigaOS clipboard device requires more setup

__exit:
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (text != NULL) {
		__decrease_reference_count(text);
	}
	return __result;
}

// Clipboard paste functionality for AmigaOS
function_result Am_Ui_Window_pasteFromClipboard_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	
	if (this != NULL) {
		__increase_reference_count(this);
	}

	// Return the stored clipboard text, or NULL if nothing was copied
	if (stored_clipboard_text != NULL) {
		// Create a proper AmLang String object from the stored text
		aobject* clipboard_string = __create_string_constant(stored_clipboard_text, &__string_class_alias);
		__result.return_value.value.object_value = clipboard_string;
		printf("Paste from clipboard: returning '%s'\n", stored_clipboard_text);
	} else {
		// No text was copied yet
		__result.return_value.value.object_value = NULL;
		printf("Paste from clipboard: clipboard is empty\n");
	}

__exit:
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
}

static char * extract_string_dup(aobject * str) {
	if (str == NULL) {
		return NULL;
	}
	string_holder * sh = (string_holder *) str->object_properties.class_object_properties.object_data.value.custom_value;
	if (sh == NULL || sh->string_value == NULL) {
		return NULL;
	}
	return strdup(sh->string_value);
}

static void free_menu_strip(Am_Ui_Window_data * data) {
	if (data->window != NULL && data->menu_strip != NULL) {
		ClearMenuStrip(data->window);
	}
	if (data->menu_strip != NULL) {
		FreeMenus(data->menu_strip);
		data->menu_strip = NULL;
	}
	if (data->menu_new_menus != NULL) {
		for (int i = 0; data->menu_new_menus[i].nm_Type != NM_END; i++) {
			if (data->menu_new_menus[i].nm_Label != NULL && data->menu_new_menus[i].nm_Label != NM_BARLABEL) {
				free((void *) data->menu_new_menus[i].nm_Label);
			}
		}
		free(data->menu_new_menus);
		data->menu_new_menus = NULL;
	}
	for (int i = 0; i < data->menu_item_count; i++) {
		if (data->menu_item_aobjects[i] != NULL) {
			__decrease_reference_count(data->menu_item_aobjects[i]);
			data->menu_item_aobjects[i] = NULL;
		}
	}
	data->menu_item_count = 0;
}

function_result Am_Ui_Window_nativeBeginMenuStrip_0(aobject * const this) {
	function_result __result = { .has_return_value = false };
	if (this != NULL) {
		__increase_reference_count(this);
	}

	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data == NULL) {
		goto __exit;
	}

	if (data->menu_builder != NULL) {
		for (int i = 0; i < data->menu_builder->num_menus; i++) {
			free(data->menu_builder->menu_titles[i]);
		}
		for (int i = 0; i < data->menu_builder->num_items; i++) {
			free(data->menu_builder->item_labels[i]);
			if (data->menu_builder->item_comm_keys[i] != NULL) {
				free(data->menu_builder->item_comm_keys[i]);
			}
		}
		free(data->menu_builder);
	}
	data->menu_builder = (Am_Ui_Window_menu_builder *) calloc(1, sizeof(Am_Ui_Window_menu_builder));

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
}

function_result Am_Ui_Window_nativeAddMenu_0(aobject * const this, aobject * const title) {
	function_result __result = { .has_return_value = false };
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (title != NULL) {
		__increase_reference_count(title);
	}

	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data == NULL || data->menu_builder == NULL) {
		goto __exit;
	}
	Am_Ui_Window_menu_builder * b = data->menu_builder;
	if (b->num_menus >= AM_UI_WINDOW_MENU_MAX_MENUS) {
		printf("Window menu builder: too many menus (max %d)\n", AM_UI_WINDOW_MENU_MAX_MENUS);
		goto __exit;
	}
	b->menu_titles[b->num_menus] = extract_string_dup(title);
	b->num_menus++;

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (title != NULL) {
		__decrease_reference_count(title);
	}
	return __result;
}

// Push one entry into the menu builder. Shared between NM_ITEM
// (level 0) and NM_SUB (level 1) pushers so the bookkeeping stays
// in one place.
static void push_builder_entry(Am_Ui_Window_menu_builder * b, int menuIndex, int level, aobject * const item, aobject * const label, aobject * const commKey) {
	if (b->num_items >= AM_UI_WINDOW_MENU_MAX_ITEMS) {
		printf("Window menu builder: too many items (max %d)\n", AM_UI_WINDOW_MENU_MAX_ITEMS);
		return;
	}
	b->item_menu_indices[b->num_items] = menuIndex;
	b->item_levels[b->num_items] = level;
	b->item_labels[b->num_items] = extract_string_dup(label);
	// Empty string is treated as "no CommKey" — Intuition only honours
	// a non-empty first character, and passing "" to NewMenu can confuse
	// Magic Menu and friends.
	if (commKey != NULL) {
		char * dup = extract_string_dup(commKey);
		if (dup != NULL && dup[0] == '\0') {
			free(dup);
			dup = NULL;
		}
		b->item_comm_keys[b->num_items] = dup;
	} else {
		b->item_comm_keys[b->num_items] = NULL;
	}
	// Hold a reference to the AmLang MenuItem for the lifetime of the strip;
	// released in free_menu_strip().
	if (item != NULL) {
		__increase_reference_count(item);
	}
	b->item_aobjects[b->num_items] = item;
	b->num_items++;
}

function_result Am_Ui_Window_nativeAddMenuItem_0(aobject * const this, int menuIndex, aobject * const item, aobject * const label, aobject * const commKey) {
	function_result __result = { .has_return_value = false };
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (item != NULL) {
		__increase_reference_count(item);
	}
	if (label != NULL) {
		__increase_reference_count(label);
	}
	if (commKey != NULL) {
		__increase_reference_count(commKey);
	}

	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data == NULL || data->menu_builder == NULL) {
		goto __exit;
	}
	push_builder_entry(data->menu_builder, menuIndex, 0, item, label, commKey);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (item != NULL) {
		__decrease_reference_count(item);
	}
	if (label != NULL) {
		__decrease_reference_count(label);
	}
	if (commKey != NULL) {
		__decrease_reference_count(commKey);
	}
	return __result;
}

function_result Am_Ui_Window_nativeAddMenuSubItem_0(aobject * const this, int menuIndex, aobject * const item, aobject * const label, aobject * const commKey) {
	function_result __result = { .has_return_value = false };
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (item != NULL) {
		__increase_reference_count(item);
	}
	if (label != NULL) {
		__increase_reference_count(label);
	}
	if (commKey != NULL) {
		__increase_reference_count(commKey);
	}

	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data == NULL || data->menu_builder == NULL) {
		goto __exit;
	}
	push_builder_entry(data->menu_builder, menuIndex, 1, item, label, commKey);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (item != NULL) {
		__decrease_reference_count(item);
	}
	if (label != NULL) {
		__decrease_reference_count(label);
	}
	if (commKey != NULL) {
		__decrease_reference_count(commKey);
	}
	return __result;
}

function_result Am_Ui_Window_nativeFinalizeMenuStrip_0(aobject * const this) {
	function_result __result = { .has_return_value = false };
	if (this != NULL) {
		__increase_reference_count(this);
	}

	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data == NULL || data->menu_builder == NULL || data->window == NULL) {
		goto __exit;
	}

	free_menu_strip(data);

	Am_Ui_Window_menu_builder * b = data->menu_builder;
	int total = b->num_menus + b->num_items + 1; // +1 for NM_END
	struct NewMenu * nm = (struct NewMenu *) calloc(total, sizeof(struct NewMenu));
	int n = 0;
	for (int m = 0; m < b->num_menus; m++) {
		nm[n].nm_Type = NM_TITLE;
		nm[n].nm_Label = b->menu_titles[m];
		nm[n].nm_CommKey = NULL;
		nm[n].nm_Flags = 0;
		nm[n].nm_MutualExclude = 0;
		nm[n].nm_UserData = NULL;
		n++;
		// Move title ownership into NewMenu so free_menu_strip releases it.
		b->menu_titles[m] = NULL;
		// Items are walked in insertion order — Window.setMenuStrip
		// pushes every NM_ITEM followed by its NM_SUB children, so
		// preserving the order here is what makes Intuition attach
		// each sub-item to the right parent.
		for (int i = 0; i < b->num_items; i++) {
			if (b->item_menu_indices[i] != m) {
				continue;
			}
			nm[n].nm_Type = (b->item_levels[i] == 1) ? NM_SUB : NM_ITEM;
			nm[n].nm_Label = b->item_labels[i];
			nm[n].nm_CommKey = b->item_comm_keys[i];
			// MenuItem.enabled → NM_ITEMDISABLED (inverted in GadTools:
			// NewMenu items are enabled by default; setting the flag
			// disables). Reads the AmLang field directly off the
			// aobject — keeps the native binding signature stable.
			nm[n].nm_Flags = 0;
			aobject *amItem = b->item_aobjects[i];
			if (amItem != NULL) {
				bool itemEnabled = amItem->object_properties.class_object_properties
					.properties[Am_Ui_MenuItem_P_enabled].nullable_value.value.bool_value;
				if (!itemEnabled) {
					nm[n].nm_Flags |= NM_ITEMDISABLED;
				}
			}
			nm[n].nm_MutualExclude = 0;
			nm[n].nm_UserData = (APTR) b->item_aobjects[i];
			n++;
			b->item_labels[i] = NULL;
			// Ownership of the CommKey string moves into the NewMenu so
			// free_menu_strip releases it together with the label.
			b->item_comm_keys[i] = NULL;
			data->menu_item_aobjects[data->menu_item_count++] = b->item_aobjects[i];
			b->item_aobjects[i] = NULL;
		}
	}
	nm[n].nm_Type = NM_END;

	struct Menu * menu = CreateMenus(nm, TAG_END);
	if (menu == NULL) {
		printf("CreateMenus failed\n");
		free(nm);
		goto __exit;
	}
	if (!LayoutMenus(menu, data->visual_info, GTMN_NewLookMenus, TRUE, TAG_END)) {
		printf("LayoutMenus failed\n");
		FreeMenus(menu);
		free(nm);
		goto __exit;
	}
	if (!SetMenuStrip(data->window, menu)) {
		printf("SetMenuStrip failed\n");
		FreeMenus(menu);
		free(nm);
		goto __exit;
	}

	data->menu_strip = menu;
	data->menu_new_menus = nm;

	free(b);
	data->menu_builder = NULL;

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
}

function_result Am_Ui_Window_nativeClearMenuStrip_0(aobject * const this) {
	function_result __result = { .has_return_value = false };
	if (this != NULL) {
		__increase_reference_count(this);
	}

	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data != NULL) {
		free_menu_strip(data);
	}

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
}
