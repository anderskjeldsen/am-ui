#include <libc/core.h>
#include <Am/Ui/Button.h>
#include <amigaos/Am/Ui/Button.h>
#include <Am/Ui/GadgetView.h>

#include <exec/types.h>
#include <intuition/intuition.h>


function_result Am_Ui_Button__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	printf("Add reference count for this in Button._native_init\n");
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
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	printf("Add reference count for this in Button.attachButton\n");
	if (this != NULL) {
		__increase_reference_count(this);
	}
	printf("Add reference count for window in Button.attachButton\n");
	if (window != NULL) {
		__increase_reference_count(window);
	}

//    aobject * const viewContext = this->object_properties.class_object_properties.properties[Am_Ui_View_P_viewContext].nullable_value.value.object_value;
//	aobject * const window = viewContext->object_properties.class_object_properties.properties[Am_Ui_ViewContext_P_window].nullable_value.value.object_value;
    Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;

    ULONG Gadgetkinds[1] = { BUTTON_KIND };
    
    struct TextAttr topaz8 = {
        (STRPTR)"topaz.font", 8, 0, 1
    };

    struct NewGadget Gadgetdata[3] = {
       63, 26, 172, 13, (UBYTE *)"Name", &topaz8, 1, PLACETEXT_LEFT, NULL, NULL, 
    };

    Gadget * const gadget = data->last_gadget = CreateGadgetA(Gadgetkinds[i], data->last_gadget, &Gadgetdata[i], (struct TagItem *)&GadgetTags[i])

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (window != NULL) {
		__increase_reference_count(window);
	}
	return __result;
};

function_result Am_Ui_Button_detachButton_0(aobject * const this, aobject * window)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	printf("Add reference count for this in Button.detachButton\n");
	if (this != NULL) {
		__increase_reference_count(this);
	}
	printf("Add reference count for window in Button.detachButton\n");
	if (window != NULL) {
		__increase_reference_count(window);
	}
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (window != NULL) {
		__increase_reference_count(window);
	}
	return __result;
};


