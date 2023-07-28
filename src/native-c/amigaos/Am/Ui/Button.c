#include <libc/core.h>
#include <Am/Ui/Button.h>
#include <amigaos/Am/Ui/Button.h>
#include <Am/Ui/GadgetView.h>

#include <amigaos/amiga.h>
#include <amigaos/Am/Ui/Window.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>

#include <libc/core_inline_functions.h>


function_result Am_Ui_Button__native_init_0(aobject * const this)
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

function_result Am_Ui_Button__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Ui_Button_attachButton_0(aobject * const this, aobject * window)
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

//    ULONG gadget_kinds[1] = { BUTTON_KIND };
    
    struct TextAttr topaz8 = {
        (STRPTR)"topaz.font", 8, 0, 1
    };

    struct NewGadget new_gadget = {
       10, 10, 100, 20, (UBYTE *)"Name", &topaz8, 1, PLACETEXT_IN, NULL, NULL,
    };

	new_gadget.ng_VisualInfo = window_data->visual_info;

	ULONG gadget_tags[] = {
		GA_Text, (ULONG) "My Button", (TAG_DONE),
//	    GA_RelVerify, TRUE, (TAG_DONE),
	   	(GTST_MaxChars), 256, (TAG_DONE),
   		(GTNM_Border), TRUE, (TAG_DONE), 
		(TAG_DONE)
	};
//
    struct Gadget * const gadget = CreateGadgetA(BUTTON_KIND, window_data->context_gadget, &new_gadget, (struct TagItem *)&gadget_tags[0]);
	this->object_properties.class_object_properties.object_data.value.custom_value = gadget;

	printf("AddGList\n");
	AddGadget(window_data->window, gadget, -1);
//	AddGList(window_data->window, gadget, -1, 0, NULL);

//	AddGadget(data->window, gadget, -1, NULL);
//	ActivateWindow(data->window);
	printf("GT_RefreshWindow\n");
	GT_RefreshWindow(window_data->window, NULL);
	printf("GT_RefreshWindow done\n");

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	return __result;
};

function_result Am_Ui_Button_detachButton_0(aobject * const this, aobject * window)
{
	printf("Detach button\n");
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	printf("Add reference count for this in Button.detachButton\n");
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (window != NULL) {
		__increase_reference_count(window);
	}

	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
	struct Gadget * gadget = this->object_properties.class_object_properties.object_data.value.custom_value;
	printf("RemoveGadget %p %p %p\n", gadget, window_data, window_data->window);
	RemoveGadget(window_data->window, gadget);
	printf("FreeGadgets\n");
	FreeGadgets(gadget);
	printf("FreeGadgets done\n");

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


