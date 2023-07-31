#pragma once
#include <libc/core.h>
#include <Am/Ui/GadgetViews.h>
#include <Am/Lang/Object.h>

#include <exec/types.h>

typedef struct _gadget_view_holder gadget_view_holder;

struct _gadget_view_holder {
    struct Gadget *gadget;
    ULONG gadget_num;
};

