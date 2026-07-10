#ifndef native_aclass_Am_Ui_RenderableBitmap_c
#define native_aclass_Am_Ui_RenderableBitmap_c

#include <libc/core.h>
#include <Am/Ui/RenderableBitmap.h>
#include <Am/Ui/Bitmap.h>
#include <linux-x64/Am/Ui/RenderableBitmap.h>
#include <linux-x64/Am/Ui/Bitmap.h>
#include <linux-x64/Am/Ui/Window.h>

#include <SDL2/SDL.h>
#include <libc/core_inline_functions.h>

// Linux Am.Ui.RenderableBitmap — SDL_TEXTUREACCESS_TARGET-backed.

static Am_Ui_RenderableBitmap_data *rb_data(aobject *const this)
{
    return (Am_Ui_RenderableBitmap_data *) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
}

static void rb_ensure_data(aobject *const this)
{
    if (rb_data(this) != NULL) return;
    Am_Ui_RenderableBitmap_data *d = (Am_Ui_RenderableBitmap_data *) calloc(1, sizeof(Am_Ui_RenderableBitmap_data));
    if (d != NULL) {
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = d;
    }
}

function_result Am_Ui_RenderableBitmap__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    rb_ensure_data(this);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_RenderableBitmap__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}

function_result Am_Ui_RenderableBitmap__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    Am_Ui_RenderableBitmap_data *d = rb_data(this);
    if (d != NULL) {
        if (d->texture != NULL) { SDL_DestroyTexture(d->texture); d->texture = NULL; }
        free(d);
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }
    return __result;
}

// attachLayer — the AmLang side calls this after Bitmap.empty / fromImage
// to bind a layer to the bitmap. AmigaOS allocates a Layer + RastPort;
// on SDL we don't need either, but this is also where we lazily build
// the SDL_Texture if the AmLang side already knows the bitmap's size.
//
// The AmLang Bitmap property `bitmap` (the Am.Ui.Bitmap aobject)
// carries our width/height; pull it out and build the texture against
// the parent Window's renderer. If no Window context exists yet, defer
// — LayerGraphics will set the target when it attaches.
function_result Am_Ui_RenderableBitmap_attachLayer_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    rb_ensure_data(this);

    Am_Ui_RenderableBitmap_data *d = rb_data(this);
    if (d == NULL) goto __exit;

    aobject *bitmapObj = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_RenderableBitmap_P_bitmap].nullable_value.value.object_value;
    if (bitmapObj == NULL) goto __exit;

    // Pull width/height directly from the Bitmap aobject's properties.
    unsigned short w = __unwrap(bitmapObj)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value;
    unsigned short h = __unwrap(bitmapObj)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value;
    d->width  = w;
    d->height = h;

    // Pull the texture+renderer from the backing Bitmap (createEmpty
    // built them up-front using the Window's renderer). Without this,
    // any LayerGraphics that attaches to us gets renderer=NULL and
    // silently drops every paint call.
    Am_Ui_Bitmap_data *bd = (Am_Ui_Bitmap_data *) __unwrap(bitmapObj)->object_properties.class_object_properties.object_data.value.custom_value;
    if (bd != NULL) {
        d->texture  = bd->texture;
        d->renderer = bd->bound_renderer;
    }

__exit:
    __decrease_reference_count(this);
    return __result;
}

#endif
