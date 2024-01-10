#include <libc/core.h>
#include <Am/Ui/WindowManager.h>
#include <morphos-ppc/Am/Ui/WindowManager.h>
#include <Am/Lang/Object.h>
#include <libc/core_inline_functions.h>

#include <exec/types.h>
#include <intuition/intuition.h>

#include <proto/exec.h>
#include <proto/intuition.h>


function_result Am_Ui_WindowManager__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

    struct MsgPort *msg_port = CreateMsgPort();
    if (msg_port == NULL) {
        __throw_simple_exception("Unable to create MsgPort", "Am_Ui_WindowManager__native_init_0, CreateMsgPort", &__result);
        goto __exit;
    }
    this->object_properties.class_object_properties.object_data.value.custom_value = msg_port;
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_WindowManager__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;

    struct MsgPort *msg_port = this->object_properties.class_object_properties.object_data.value.custom_value;
    if (msg_port != NULL) {
        DeleteMsgPort(msg_port);
        this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }
__exit: ;
	return __result;
};

function_result Am_Ui_WindowManager__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

