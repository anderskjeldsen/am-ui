#include <libc/core.h>
#include <Am/Ui/PixelBufferView.h>
#include <amigaos/Am/Ui/PixelBufferView.h>
#include <Am/Ui/Graphics.h>
#include <Am/Ui/LayerGraphics.h>
#include <amigaos/Am/Ui/LayerGraphics.h>
#include <Am/Ui/ViewContextGraphics.h>
#include <amigaos/Am/Ui/ViewContextGraphics.h>
#include <amigaos/Am/Ui/Window.h>
#include <Am/Lang/Array.h>
#include <Am/Lang/UInt.h>

#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>
#include <cybergraphx/cybergraphics.h>

#include <proto/cybergraphics.h>

#include <libc/core_inline_functions.h>

// Extract the RastPort from any concrete Graphics implementation.
// Handles LayerGraphics (primary path) and ViewContextGraphics (fallback).
static struct RastPort *get_rastport_from_graphics(aobject *graphics)
{
    if (graphics == NULL) return NULL;

    if (graphics->class_ptr == &Am_Ui_LayerGraphics) {
        Am_Ui_LayerGraphics_data *data = (Am_Ui_LayerGraphics_data *)
            graphics->object_properties.class_object_properties.object_data.value.custom_value;
        if (data == NULL || data->rastport == NULL) return NULL;
        return data->rastport;
    }

    if (graphics->class_ptr == &Am_Ui_ViewContextGraphics) {
        aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(graphics)
            .return_value.value.object_value;
        if (window == NULL) return NULL;
        Am_Ui_Window_data *wd = (Am_Ui_Window_data *)
            window->object_properties.class_object_properties.object_data.value.custom_value;
        if (wd == NULL || wd->window == NULL) return NULL;
        return wd->window->RPort;
    }

    return NULL;
}

// ---------------------------------------------------------------------------
// Lifecycle — no native data to allocate or free
// ---------------------------------------------------------------------------

function_result Am_Ui_PixelBufferView__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_PixelBufferView__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_PixelBufferView__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

// ---------------------------------------------------------------------------
// initBuffer: allocate the ARGB pixel array (zero-initialised)
// ---------------------------------------------------------------------------

function_result Am_Ui_PixelBufferView_initBuffer_0(
    aobject * const this,
    unsigned short width,
    unsigned short height)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    aobject *pixelsArr = NULL;

    if (this != NULL) { __increase_reference_count(this); }

    // Allocate the UInt[] pixel buffer (zero-initialised = transparent black).
    {
        unsigned int bufSize = (unsigned int)width * (unsigned int)height;
        pixelsArr = __create_array(bufSize, sizeof(unsigned int),
                                   &Am_Lang_Array_ta_Am_Lang_UInt, uint_type);
        if (pixelsArr == NULL) {
            __throw_simple_exception("__create_array failed for pixel buffer",
                                     "in Am_Ui_PixelBufferView_initBuffer_0", &__result);
            goto __exit;
        }

        nullable_value nv = (nullable_value){ .flags = 0,
                                              .value = (value){ .object_value = pixelsArr } };
        __set_property(this, Am_Ui_PixelBufferView_P_pixels, nv);
    }

    // Record buffer dimensions in the AML properties.
    this->object_properties.class_object_properties
        .properties[Am_Ui_PixelBufferView_P_bufferWidth].nullable_value.value.ushort_value = width;
    this->object_properties.class_object_properties
        .properties[Am_Ui_PixelBufferView_P_bufferHeight].nullable_value.value.ushort_value = height;

__exit: ;
    if (pixelsArr != NULL) { __decrease_reference_count(pixelsArr); }
    if (this != NULL)      { __decrease_reference_count(this); }
    return __result;
}

// ---------------------------------------------------------------------------
// writePixels: WritePixelArray directly to the window RastPort
// ---------------------------------------------------------------------------

function_result Am_Ui_PixelBufferView_writePixels_0(
    aobject * const this,
    aobject * graphics,
    short x,
    short y)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;

    if (this != NULL)     { __increase_reference_count(this); }
    if (graphics != NULL) { __increase_reference_count(graphics); }

    if (graphics == NULL) goto __exit;

    {
        // Get the pixel buffer.
        aobject *pixelsObj = this->object_properties.class_object_properties
            .properties[Am_Ui_PixelBufferView_P_pixels].nullable_value.value.object_value;
        if (pixelsObj == NULL) goto __exit;

        array_holder *ah = get_array_holder(pixelsObj);
        if (ah == NULL || ah->size == 0) goto __exit;

        unsigned int *pixels = (unsigned int *)get_array_data(ah);
        if (pixels == NULL) goto __exit;

        unsigned short width  = this->object_properties.class_object_properties
            .properties[Am_Ui_PixelBufferView_P_bufferWidth].nullable_value.value.ushort_value;
        unsigned short height = this->object_properties.class_object_properties
            .properties[Am_Ui_PixelBufferView_P_bufferHeight].nullable_value.value.ushort_value;
        if (width == 0 || height == 0) goto __exit;

        // Obtain the RastPort from the Graphics context (LayerGraphics or ViewContextGraphics).
        struct RastPort *rp = get_rastport_from_graphics(graphics);
        if (rp == NULL) goto __exit;

        // Translate view-local x/y through the graphics offset.
        // paintAll() calls graphics.translate(ix, iy) before invoking paint(),
        // so xOffset/yOffset already encode the view's screen position.
        short xOff = graphics->object_properties.class_object_properties
            .properties[Am_Ui_Graphics_P_xOffset].nullable_value.value.short_value;
        short yOff = graphics->object_properties.class_object_properties
            .properties[Am_Ui_Graphics_P_yOffset].nullable_value.value.short_value;

        WritePixelArray(
            pixels,
            0, 0,
            width * 4,      /* srcBytesPerRow: 4 bytes per ARGB pixel */
            rp,
            xOff + x,
            yOff + y,
            width, height,
            RECTFMT_ARGB
        );
    }

__exit: ;
    if (graphics != NULL) { __decrease_reference_count(graphics); }
    if (this != NULL)     { __decrease_reference_count(this); }
    return __result;
}
