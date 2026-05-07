#include <libc/core.h>

#include <Am/Ui/Window.h>
#include <Am/Ui/WindowProperties.h>
#include <Am/Ui/MenuItem.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>
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

static void do_full_close(aobject * const this) {
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

function_result Am_Ui_Window__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
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


function_result Am_Ui_Window_open_0(aobject * const this, SHORT x, SHORT y, USHORT width, USHORT height, aobject * screen, aobject * windowManager, struct Am_Ui_WindowProperties * properties)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	// Add reference count for this in Window.open
	printf("Add reference count for this in Window.open\n");
	if (this != NULL) {
		__increase_reference_count(this);
	}

	bool borderless = (properties != NULL) ? properties->borderless : false;
	// WFLG_RMBTRAP is intentionally NOT set: Intuition needs the right
	// mouse button to pop up the menu strip when one is installed.
	ULONG window_flags = borderless
		? (WFLG_ACTIVATE | WFLG_BORDERLESS | WFLG_SIMPLE_REFRESH)
		: (WFLG_SIZEGADGET | WFLG_ACTIVATE | WFLG_DRAGBAR | WFLG_DEPTHGADGET | WFLG_CLOSEGADGET | WFLG_SIMPLE_REFRESH | WFLG_SIZEBBOTTOM);

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
		case IDCMP_MENUPICK:
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
		do_full_close(this);
		goto __exit;
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
	if (this != NULL) __increase_reference_count(this);
	if (title != NULL) __increase_reference_count(title);
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
	if (this != NULL) __decrease_reference_count(this);
	if (title != NULL) __decrease_reference_count(title);
	return __result;
}

function_result Am_Ui_Window_nativeAddMenuItem_0(aobject * const this, int menuIndex, aobject * const item, aobject * const label) {
	function_result __result = { .has_return_value = false };
	if (this != NULL) __increase_reference_count(this);
	if (item != NULL) __increase_reference_count(item);
	if (label != NULL) __increase_reference_count(label);
	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data == NULL || data->menu_builder == NULL) {
		goto __exit;
	}
	Am_Ui_Window_menu_builder * b = data->menu_builder;
	if (b->num_items >= AM_UI_WINDOW_MENU_MAX_ITEMS) {
		printf("Window menu builder: too many items (max %d)\n", AM_UI_WINDOW_MENU_MAX_ITEMS);
		goto __exit;
	}
	b->item_menu_indices[b->num_items] = menuIndex;
	b->item_labels[b->num_items] = extract_string_dup(label);
	if (item != NULL) {
		__increase_reference_count(item);
	}
	b->item_aobjects[b->num_items] = item;
	b->num_items++;
__exit: ;
	if (this != NULL) __decrease_reference_count(this);
	if (item != NULL) __decrease_reference_count(item);
	if (label != NULL) __decrease_reference_count(label);
	return __result;
}

function_result Am_Ui_Window_nativeFinalizeMenuStrip_0(aobject * const this) {
	function_result __result = { .has_return_value = false };
	if (this != NULL) __increase_reference_count(this);
	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data == NULL || data->menu_builder == NULL || data->window == NULL) {
		goto __exit;
	}
	free_menu_strip(data);
	Am_Ui_Window_menu_builder * b = data->menu_builder;
	int total = b->num_menus + b->num_items + 1;
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
		b->menu_titles[m] = NULL;
		for (int i = 0; i < b->num_items; i++) {
			if (b->item_menu_indices[i] != m) {
				continue;
			}
			nm[n].nm_Type = NM_ITEM;
			nm[n].nm_Label = b->item_labels[i];
			nm[n].nm_CommKey = NULL;
			nm[n].nm_Flags = 0;
			nm[n].nm_MutualExclude = 0;
			nm[n].nm_UserData = (APTR) b->item_aobjects[i];
			n++;
			b->item_labels[i] = NULL;
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
	if (this != NULL) __decrease_reference_count(this);
	return __result;
}

function_result Am_Ui_Window_nativeClearMenuStrip_0(aobject * const this) {
	function_result __result = { .has_return_value = false };
	if (this != NULL) __increase_reference_count(this);
	Am_Ui_Window_data * data = (Am_Ui_Window_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data != NULL) {
		free_menu_strip(data);
	}
__exit: ;
	if (this != NULL) __decrease_reference_count(this);
	return __result;
}
