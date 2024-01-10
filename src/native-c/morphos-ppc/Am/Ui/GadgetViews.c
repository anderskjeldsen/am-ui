#include <libc/core.h>
#include <Am/Ui/GadgetViews.h>
#include <Am/Ui/View.h>
#include <morphos-ppc/Am/Ui/GadgetViews.h>
#include <Am/Lang/Object.h>

#include <morphos-ppc/morphos.h>
#include <morphos-ppc/Am/Ui/Window.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>

#include <libc/core_inline_functions.h>

function_result Am_Ui_GadgetViews__native_init_0(aobject * const this)
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

function_result Am_Ui_GadgetViews__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Ui_GadgetViews__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Ui_GadgetViews_layoutNative_0(aobject * gadgetView, short x, short y, unsigned short width, unsigned short height)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (gadgetView != NULL) {
		__increase_reference_count(gadgetView);
	}

	aobject *window = Am_Ui_View_getWindow_0(gadgetView).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
	gadget_view_holder *gvh = (gadget_view_holder *) gadgetView->object_properties.class_object_properties.object_data.value.custom_value;

/*
	struct TagItem tags[] =
	{
		{GA_Left, x},
		{GA_Top, y},
		{GA_Width, width},     // Set the new width in pixels
		{GA_Height, height},   // Set the new height in pixels
		{TAG_DONE}                // Marks the end of the tag list
	};
*/

	printf("Old gadget size %dx%d\n", gvh->gadget->Width, gvh->gadget->Height);
	printf("SetGadgetAttrsA %d,%d %dx%d\n", x, y, width, height);
	UWORD pos = RemoveGList(window_data->window, gvh->gadget, gvh->gadget_num);
	gvh->gadget->LeftEdge = x;
	gvh->gadget->TopEdge = y;
	gvh->gadget->Width = width;
	gvh->gadget->Height = height;
//	SetGadgetAttrsA(gvh->gadget, window_data->window, NULL, tags);
	printf("New gadget size %dx%d\n", gvh->gadget->Width, gvh->gadget->Height);
	AddGList(window_data->window, gvh->gadget, pos, 1, NULL);
	RefreshGList(gvh->gadget, window_data->window, NULL, gvh->gadget_num);
__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	if (gadgetView != NULL) {
		__decrease_reference_count(gadgetView);
	}
	return __result;
};

ULONG gadget_count(struct Gadget *gadget) {
	ULONG c = 0;
	while(gadget != NULL) {
		c++;
		gadget = gadget->NextGadget;
	}
	return c;
}

struct Gadget * last_gadget(struct Gadget *gadget) {
	while(gadget->NextGadget != NULL) {
		gadget = gadget->NextGadget;
	}
	return gadget;
}

void debug_gadget(struct Gadget *gadget) {
	printf("Gadget %d", gadget->GadgetID);
	if (gadget->NextGadget != NULL) {
		debug_gadget(gadget->NextGadget);
	}
}

