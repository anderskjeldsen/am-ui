// Headless LayerGraphics for amigaos-sim. Routes every primitive through
// headless_write_pixel against the Window's pixel buffer. The Graphics base
// class keeps xOffset / yOffset on the .aml side; we read them via the
// generated property-index constants so translates compose correctly through
// nested paintAll calls.

#include <libc/core.h>
#include <Am/Ui/LayerGraphics.h>
#include <amigaos-sim/Am/Ui/LayerGraphics.h>
#include <Am/Ui/RenderableBitmap.h>
#include <Am/Ui/Bitmap.h>
#include <Am/Ui/Font.h>
#include <Am/Ui/ClipRect.h>
#include <Am/Imaging/Image.h>
#include <Am/Lang/UByte.h>
#include <Am/Lang/UShort.h>
#include <Am/Lang/Short.h>
#include <Am/Lang/String.h>
#include <libc/core_inline_functions.h>

#include <amigaos-sim/headless_painter.h>

// ---- helpers ----------------------------------------------------------------

static short translated_x(aobject *g, short x)
{
    short ox = g->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_xOffset].nullable_value.value.short_value;
    return (short)(ox + x);
}

static short translated_y(aobject *g, short y)
{
    short oy = g->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_yOffset].nullable_value.value.short_value;
    return (short)(oy + y);
}

static headless_screen_t * lg_screen(aobject *this)
{
    if (this == NULL) return NULL;
    aobject *window = this->object_properties.class_object_properties.properties[Am_Ui_LayerGraphics_P_window].nullable_value.value.object_value;
    return headless_screen_for_window(window);
}

// ---- lifecycle --------------------------------------------------------------

function_result Am_Ui_LayerGraphics__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) __increase_reference_count(this);
__exit: ;
    if (this != NULL) __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_attachWindow_0(aobject * const this)
{
    // Window reference is already in the LayerGraphics property; nothing to
    // cache for the headless backend — we look it up per primitive.
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_attachRenderableBitmap_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

// ---- pen state --------------------------------------------------------------

function_result Am_Ui_LayerGraphics_setForegroundPen_0(aobject * const this, unsigned char pen)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    headless_screen_t *s = lg_screen(this);
    if (s != NULL) s->foreground_pen = pen;
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_setBackgroundPen_0(aobject * const this, unsigned char pen)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    headless_screen_t *s = lg_screen(this);
    if (s != NULL) s->background_pen = pen;
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_setFont_0(aobject * const this, aobject *font)
{
    // No real font in the headless build — text cells are 8x8 regardless.
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (font != NULL) __increase_reference_count(font);
__exit: ;
    if (font != NULL) __decrease_reference_count(font);
    return __result;
}

// ---- primitives -------------------------------------------------------------

function_result Am_Ui_LayerGraphics_drawLine_0(aobject * const this, short x1, short y1, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    headless_screen_t *s = lg_screen(this);
    if (s != NULL) {
        headless_draw_line(s,
            translated_x(this, x1), translated_y(this, y1),
            translated_x(this, x2), translated_y(this, y2),
            s->foreground_pen);
    }
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_fillRect_0(aobject * const this, short x1, short y1, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    headless_screen_t *s = lg_screen(this);
    if (s != NULL) {
        headless_fill_rect(s,
            translated_x(this, x1), translated_y(this, y1),
            translated_x(this, x2), translated_y(this, y2),
            s->foreground_pen);
    }
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_eraseRect_0(aobject * const this, short x1, short y1, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    headless_screen_t *s = lg_screen(this);
    if (s != NULL) {
        headless_fill_rect(s,
            translated_x(this, x1), translated_y(this, y1),
            translated_x(this, x2), translated_y(this, y2),
            s->background_pen);
    }
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_drawString_0(aobject * const this, aobject *text, short x, short y)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (text != NULL) __increase_reference_count(text);

    headless_screen_t *s = lg_screen(this);
    if (s != NULL && text != NULL) {
        string_holder *sh = (string_holder *) text->object_properties.class_object_properties.object_data.value.custom_value;
        if (sh != NULL && sh->string_value != NULL) {
            headless_draw_text_box(s, translated_x(this, x), translated_y(this, y),
                                   (unsigned int) sh->length, s->foreground_pen);
        }
    }
__exit: ;
    if (text != NULL) __decrease_reference_count(text);
    return __result;
}

function_result Am_Ui_LayerGraphics_calculateStringWidth_0(aobject * const this, aobject *text)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    if (text != NULL) __increase_reference_count(text);

    unsigned short width = 0;
    if (text != NULL) {
        string_holder *sh = (string_holder *) text->object_properties.class_object_properties.object_data.value.custom_value;
        if (sh != NULL) width = (unsigned short)(sh->length * 8);
    }
    __result.return_value.value.ushort_value = width;

__exit: ;
    if (text != NULL) __decrease_reference_count(text);
    return __result;
}

function_result Am_Ui_LayerGraphics_getCurrentFontSize_0(aobject * const this)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    __result.return_value.value.uchar_value = 8;
__exit: ;
    return __result;
}

// ---- clip / paint markers ---------------------------------------------------

static void apply_clip(headless_screen_t *s, struct Am_Ui_ClipRect *clipRect)
{
    if (s == NULL) return;
    if (clipRect == NULL) {
        s->clip.active = false;
        return;
    }
    s->clip.min_x = clipRect->x;
    s->clip.min_y = clipRect->y;
    s->clip.max_x = (short)(clipRect->x + (short)clipRect->width  - 1);
    s->clip.max_y = (short)(clipRect->y + (short)clipRect->height - 1);
    s->clip.active = true;
}

function_result Am_Ui_LayerGraphics_setClipRect_0(aobject * const this, struct Am_Ui_ClipRect *clipRect)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    apply_clip(lg_screen(this), clipRect);
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_clearClipRect_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    headless_screen_t *s = lg_screen(this);
    if (s != NULL) s->clip.active = false;
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_beginPainting_0(aobject * const this, struct Am_Ui_ClipRect *clipRect)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    apply_clip(lg_screen(this), clipRect);
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_endPainting_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    headless_screen_t *s = lg_screen(this);
    if (s != NULL) s->clip.active = false;
__exit: ;
    return __result;
}

// ---- bitmap / image (no-op for now; tests don't exercise these) ----

function_result Am_Ui_LayerGraphics_drawImage_0(aobject * const this, aobject *image,
                                                short x, short y, short width, short height)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (image != NULL) __increase_reference_count(image);
__exit: ;
    if (image != NULL) __decrease_reference_count(image);
    return __result;
}

function_result Am_Ui_LayerGraphics_drawBitmap_0(aobject * const this, aobject *bitmap,
                                                 short x, short y, short destWidth, short destHeight)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (bitmap != NULL) __increase_reference_count(bitmap);
__exit: ;
    if (bitmap != NULL) __decrease_reference_count(bitmap);
    return __result;
}
