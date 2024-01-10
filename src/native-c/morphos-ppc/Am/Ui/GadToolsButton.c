#include <libc/core.h>
#include <Am/Ui/GadToolsButton.h>
#include <morphos-ppc/Am/Ui/GadToolsButton.h>
#include <Am/Ui/GadToolsBase.h>
#include <Am/Ui/Window.h>
#include <Am/Ui/ViewContext.h>
#include <libc/core_inline_functions.h>

#include <morphos-ppc/morphos.h>
#include <morphos-ppc/Am/Ui/Window.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>

#include <morphos-ppc/Am/Ui/GadgetViews.h>

#include <libc/core_inline_functions.h>


function_result Am_Ui_GadToolsButton__native_init_0(aobject * const this)
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

function_result Am_Ui_GadToolsButton__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Ui_GadToolsButton__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Ui_GadToolsButton_attachButton_0(aobject * const this, aobject * window)
{
	printf("Attach button\n");
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (window != NULL) {
		__increase_reference_count(window);
	}

    Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
  
    struct TextAttr topaz8 = {
        "topaz.font", 8, 0, 0
    };

    struct NewGadget new_gadget = {
       10, 10, 100, 20, "Name", &topaz8, 10, PLACETEXT_IN, NULL, NULL,
    };

	new_gadget.ng_VisualInfo = window_data->visual_info;

	struct TagItem gadget_tags[] = {
		(TAG_END), (TAG_END)
	};
//
    struct Gadget * const gadget = CreateGadgetA(BUTTON_KIND, last_gadget(window_data->context_gadget), &new_gadget, (struct TagItem *)&gadget_tags[0]);

	gadget_view_holder *gvh = malloc(sizeof(gadget_view_holder));
	gvh->gadget = gadget;
	gvh->gadget_num = gadget_count(gadget);

	this->object_properties.class_object_properties.object_data.value.custom_value = gvh;

//	AddGList(window_data->window, gadget, -1, 1, NULL);
	RefreshGList(gadget, window_data->window, NULL, gvh->gadget_num);
	GT_RefreshWindow(window_data->window, NULL);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	return __result;
};

function_result Am_Ui_GadToolsButton_detachButton_0(aobject * const this, aobject * window)
{
	printf("Detach button\n");
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (window != NULL) {
		__increase_reference_count(window);
	}

	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
	gadget_view_holder *gvh = (gadget_view_holder *) this->object_properties.class_object_properties.object_data.value.custom_value;
//	printf("RemoveGadget %p %p %p\n", gvh->gadget, window_data, window_data->window);
	UWORD pos = RemoveGList(window_data->window, gvh->gadget, gvh->gadget_num);
	if (pos >= 0) {
//		gvh->gadget->NextGadget = NULL;
		FreeGadgets(gvh->gadget);
	}
	free(gvh);

	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	return __result;
};
