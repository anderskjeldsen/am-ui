#include <libc/core.h>
#include <Am/Ui/ClipRect.h>
#include <amigaos/Am/Ui/ClipRect.h>
#include <Am/Lang/Short.h>
#include <Am/Lang/UShort.h>

#include <exec/types.h>
#include <graphics/gfx.h>
#include <proto/exec.h>
#include <proto/graphics.h>

#include <libc/core_inline_functions.h>

// Forward declaration for AmigaOS layers function
struct Region *InstallClipRegion(struct Layer *layer, struct Region *region);

function_result Am_Ui_ClipRect__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }

//    short x, short y, unsigned short width, unsigned short height
    short x = this->object_properties.class_object_properties.properties[Am_Ui_ClipRect_P_x].nullable_value.value.short_value;
    short y = this->object_properties.class_object_properties.properties[Am_Ui_ClipRect_P_y].nullable_value.value.short_value;
    unsigned short width = this->object_properties.class_object_properties.properties[Am_Ui_ClipRect_P_width].nullable_value.value.ushort_value;
    unsigned short height = this->object_properties.class_object_properties.properties[Am_Ui_ClipRect_P_height].nullable_value.value.ushort_value;

    // Create the AmigaOS region with absolute coordinates
    struct Region *region = NewRegion();
    if (region != NULL) {
        struct Rectangle rect;
        rect.MinX = x;
        rect.MinY = y;
        rect.MaxX = x + width - 1;
        rect.MaxY = y + height - 1;
        
        if (OrRectRegion(region, &rect)) {
            // Store the region directly in the object
            this->object_properties.class_object_properties.object_data.value.custom_value = region;
        } else {
            // Failed to add rectangle to region
            DisposeRegion(region);
        }
    }

__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

function_result Am_Ui_ClipRect__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    
    struct Region *region = (struct Region *)this->object_properties.class_object_properties.object_data.value.custom_value;
    if (region != NULL) {
        DisposeRegion(region);
        this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }

__exit: ;
    return __result;
}

function_result Am_Ui_ClipRect__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

