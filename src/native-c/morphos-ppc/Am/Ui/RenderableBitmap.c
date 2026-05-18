#include <libc/core.h>
#include <Am/Ui/RenderableBitmap.h>
#include <morphos-ppc/Am/Ui/RenderableBitmap.h>
#include <Am/Ui/Bitmap.h>
#include <morphos-ppc/Am/Ui/Bitmap.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <graphics/layers.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/layers.h>

#include <libc/core_inline_functions.h>

// MorphOS port — same Intuition / graphics / layers calls as AmigaOS.

function_result Am_Ui_RenderableBitmap__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }

    Am_Ui_RenderableBitmap_data *data = AllocVec(sizeof(Am_Ui_RenderableBitmap_data), MEMF_CLEAR | MEMF_ANY);
    if (data == NULL) {
        __throw_simple_exception("AllocVec failed in Am_Ui_RenderableBitmap__native_init_0",
                                 "in Am_Ui_RenderableBitmap__native_init_0", &__result);
        goto __exit;
    }
    this->object_properties.class_object_properties.object_data.value.custom_value = data;

__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

function_result Am_Ui_RenderableBitmap__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;

    Am_Ui_RenderableBitmap_data *data =
        (Am_Ui_RenderableBitmap_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data != NULL) {
        if (data->layer != NULL) {
            DeleteLayer(0, data->layer);
            data->layer = NULL;
        }
        if (data->layer_info != NULL) {
            DisposeLayerInfo(data->layer_info);
            data->layer_info = NULL;
        }
        FreeVec(data);
        this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }

__exit: ;
    return __result;
}

function_result Am_Ui_RenderableBitmap__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_RenderableBitmap_attachLayer_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }

    Am_Ui_RenderableBitmap_data *data =
        (Am_Ui_RenderableBitmap_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) {
        __throw_simple_exception("RenderableBitmap not initialized",
                                 "in Am_Ui_RenderableBitmap_f_attachLayer_0", &__result);
        goto __exit;
    }

    aobject *bitmapObj =
        this->object_properties.class_object_properties.properties[Am_Ui_RenderableBitmap_P_bitmap].nullable_value.value.object_value;
    if (bitmapObj == NULL) {
        __throw_simple_exception("RenderableBitmap has no bitmap",
                                 "in Am_Ui_RenderableBitmap_f_attachLayer_0", &__result);
        goto __exit;
    }

    Am_Ui_Bitmap_data *bitmapData =
        (Am_Ui_Bitmap_data *)bitmapObj->object_properties.class_object_properties.object_data.value.custom_value;
    if (bitmapData == NULL || bitmapData->bitmap == NULL) {
        __throw_simple_exception("RenderableBitmap bitmap has no native BitMap",
                                 "in Am_Ui_RenderableBitmap_f_attachLayer_0", &__result);
        goto __exit;
    }

    unsigned short width = bitmapObj->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value;
    unsigned short height = bitmapObj->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value;

    if (data->layer != NULL) {
        goto __exit;
    }

    data->layer_info = NewLayerInfo();
    if (data->layer_info == NULL) {
        __throw_simple_exception("NewLayerInfo failed",
                                 "in Am_Ui_RenderableBitmap_f_attachLayer_0", &__result);
        goto __exit;
    }

    data->layer = CreateUpfrontLayer(
        data->layer_info,
        bitmapData->bitmap,
        0, 0,
        (LONG)width - 1, (LONG)height - 1,
        LAYERSIMPLE,
        NULL
    );
    if (data->layer == NULL) {
        __throw_simple_exception("CreateUpfrontLayer failed",
                                 "in Am_Ui_RenderableBitmap_f_attachLayer_0", &__result);
        goto __exit;
    }

    InitRastPort(&data->rastport);
    data->rastport.BitMap = bitmapData->bitmap;
    data->rastport.Layer = data->layer;

__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}
