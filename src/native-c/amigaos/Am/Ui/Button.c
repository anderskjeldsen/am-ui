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

    struct NewGadget new_gad[1] = { 63, 26, 172, 13, (UBYTE *)"Name", &topaz8, 1, PLACETEXT_LEFT, NULL, NULL };
    struct Gadget *gadget = CreateGadgetA(BUTTON_KIND, ne, &Gadgetdata[i], (struct TagItem *)&GadgetTags[i])) {
            printf("Gadget %d created.\n", i);
         }
         else
            printf("Failed to create gadget %d.\n",i);
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
