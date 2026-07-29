#include <libc/core.h>
#include <Am/Ui/LayerGraphics.h>
#include <morphos-ppc/Am/Ui/LayerGraphics.h>
#include <Am/Ui/RenderableBitmap.h>
#include <morphos-ppc/Am/Ui/RenderableBitmap.h>
#include <Am/Ui/Window.h>
#include <morphos-ppc/Am/Ui/Window.h>
#include <Am/Ui/Bitmap.h>
#include <morphos-ppc/Am/Ui/Bitmap.h>
#include <Am/Ui/Font.h>
#include <Am/Ui/ClipRect.h>
#include <Am/Imaging/Image.h>
#include <libc/Am/Lang/String.h>

#include <string.h>
#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/layers.h>
#include <graphics/rastport.h>
#include <graphics/rpattr.h>
#include <graphics/scale.h>
#include <graphics/view.h>
#include <graphics/text.h>
#include <cybergraphx/cybergraphics.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/cybergraphics.h>

#include <libc/core_inline_functions.h>

short lg_translated_x(aobject *g, short x) {
    short tx = __unwrap(g)->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_xOffset].nullable_value.value.short_value;
    return tx + x;
}

short lg_translated_y(aobject *g, short y) {
    short ty = __unwrap(g)->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_yOffset].nullable_value.value.short_value;
    return ty + y;
}

// Screen ViewPort of the (single) app screen — cached the first time a
// window graphics binds. The off-screen double-buffer's LayerGraphics
// reuses it so colour-mode text/lines can program the scratch pens
// (SetRGB32 lives on the ViewPort, and the offscreen bitmap shares the
// screen's colormap as a friend bitmap). Avoids threading a Window
// reference through RenderableBitmap. The IDE only ever opens one
// screen; if that ever changes, bind_window_target keeps this pointing
// at the most-recently-bound window's screen.
static struct ViewPort *g_screen_vp = NULL;

static Am_Ui_LayerGraphics_data *get_layer_graphics_data(aobject *this)
{
    if (this == NULL) return NULL;
    return (Am_Ui_LayerGraphics_data *)__unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
}

static struct RastPort *get_rp(aobject *this)
{
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    if (data == NULL || data->rastport == NULL || data->rastport->BitMap == NULL) return NULL;
    return data->rastport;
}

static struct Layer *get_layer(aobject *this)
{
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    if (data == NULL) return NULL;
    return data->layer;
}

static void clear_target(Am_Ui_LayerGraphics_data *data)
{
    if (data == NULL) return;
    data->rastport = NULL;
    data->layer = NULL;
    data->screen_vp = NULL;
}

static void bind_renderable_bitmap_target(aobject *this)
{
    if (this == NULL) return;
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    if (data == NULL) return;

    aobject *rb = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_LayerGraphics_P_renderableBitmap]
        .nullable_value.value.object_value;
    if (rb == NULL) {
        clear_target(data);
        return;
    }

    Am_Ui_RenderableBitmap_data *rbData =
        (Am_Ui_RenderableBitmap_data *)__unwrap(rb)->object_properties.class_object_properties.object_data.value.custom_value;
    if (rbData == NULL || rbData->layer == NULL || rbData->rastport.BitMap == NULL) {
        clear_target(data);
        return;
    }

    data->rastport = &rbData->rastport;
    data->layer = rbData->layer;
    // The IDE paints every view into this off-screen buffer, so the
    // scratch-pen colour scheme (pens for Text/Draw) must work here.
    // The offscreen bitmap is a friend of the screen bitmap → shares
    // its colormap, so the screen's ViewPort (cached when the window
    // graphics bound) is the right one to program.
    data->screen_vp = g_screen_vp;
}

static void bind_window_target(aobject *this)
{
    if (this == NULL) return;
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    if (data == NULL) return;

    aobject *windowObj = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_LayerGraphics_P_window]
        .nullable_value.value.object_value;
    if (windowObj == NULL) {
        clear_target(data);
        return;
    }

    Am_Ui_Window_data *windowData =
        (Am_Ui_Window_data *)__unwrap(windowObj)->object_properties.class_object_properties.object_data.value.custom_value;
    if (windowData == NULL || windowData->window == NULL || windowData->window->RPort == NULL || windowData->window->WLayer == NULL) {
        clear_target(data);
        return;
    }

    data->rastport = windowData->window->RPort;
    data->layer = windowData->window->WLayer;
    data->screen_vp = NULL;
    if (windowData->window->WScreen != NULL) {
        data->screen_vp = &windowData->window->WScreen->ViewPort;
        // Cache for the off-screen buffer's LayerGraphics (single-screen app).
        g_screen_vp = data->screen_vp;
    }
}

static void apply_clip_rect(aobject *this, struct Am_Ui_ClipRect *clipRect)
{
    if (this == NULL) return;
    if (clipRect == NULL) return;

    Am_Ui_LayerGraphics_data *gData =
        (Am_Ui_LayerGraphics_data *)__unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
    if (gData == NULL || gData->clip_region == NULL) return;

    struct Layer *layer = get_layer(this);
    if (layer == NULL) return;

    InstallClipRegion(layer, NULL);
    ClearRegion(gData->clip_region);

    // ClipRect is now a struct (value type) — read fields directly.
    struct Rectangle rect;
    rect.MinX = clipRect->x;
    rect.MinY = clipRect->y;
    rect.MaxX = clipRect->x + clipRect->width - 1;
    rect.MaxY = clipRect->y + clipRect->height - 1;

    OrRectRegion(gData->clip_region, &rect);
    InstallClipRegion(layer, gData->clip_region);
}

function_result Am_Ui_LayerGraphics__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }

    Am_Ui_LayerGraphics_data *data = AllocVec(sizeof(Am_Ui_LayerGraphics_data), MEMF_CLEAR | MEMF_ANY);
    if (data == NULL) {
        __throw_simple_exception("AllocVec failed in Am_Ui_LayerGraphics__native_init_0",
                                 "in Am_Ui_LayerGraphics__native_init_0", &__result);
        goto __exit;
    }
    data->clip_region = NewRegion();
    clear_target(data);
    __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = data;

__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

function_result Am_Ui_LayerGraphics__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;

    Am_Ui_LayerGraphics_data *data =
        (Am_Ui_LayerGraphics_data *)__unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
    if (data != NULL) {
        if (data->clip_region != NULL) {
            DisposeRegion(data->clip_region);
            data->clip_region = NULL;
        }
        FreeVec(data);
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }

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

function_result Am_Ui_LayerGraphics_attachRenderableBitmap_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    bind_renderable_bitmap_target(this);
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_attachWindow_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    bind_window_target(this);
__exit: ;
    return __result;
}

// The pen-vs-colour split: setForegroundPen / setBackgroundPen
// take the classic AmigaOS path (SetAPen / SetBPen, drawing
// uses the colormap index). setForegroundColor /
// setBackgroundColor stash a direct 24-bit ARGB on the
// graphics instance instead — fillRect / eraseRect check the
// fg_color_active / bg_color_active flag and route through
// CGFX's FillPixelArray when set, which takes the ARGB direct
// and skips the colormap entirely. That's how an extension can
// pick from more than the IDE's 16 reserved palette pens.
//
// Pen setters clear the matching colour flag so a caller
// switching back to a pen actually goes back to the pen path
// instead of inheriting whatever colour the previous
// setForegroundColor stashed.
function_result Am_Ui_LayerGraphics_setForegroundPen_0(aobject * const this, unsigned char pen)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    struct RastPort *rp = get_rp(this);
    if (data != NULL) data->fg_color_active = FALSE;
    if (rp != NULL) SetAPen(rp, pen);
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_setBackgroundPen_0(aobject * const this, unsigned char pen)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    struct RastPort *rp = get_rp(this);
    if (data != NULL) data->bg_color_active = FALSE;
    if (rp != NULL) SetBPen(rp, pen);
__exit: ;
    return __result;
}

// Direct-colour mode. Two cooperating mechanisms:
//
//  1. The 24-bit ARGB is stashed on the instance and flagged
//     active — fillRect / eraseRect route through CGFX's
//     FillPixelArray, which takes the ARGB straight and skips
//     the colormap entirely.
//  2. The SCRATCH PEN: the colour is also programmed into the
//     screen colormap at a dedicated index — 255 for the
//     foreground, 254 for the background (separate indices so
//     the two setters can't clobber each other) — and SetAPen /
//     SetBPen select it. That makes the pen-only primitives
//     (Text, Draw) render the requested colour too; before this,
//     drawString / drawLine used whatever pen the IDE chrome
//     last set (the sprite pixler's status line changed colour
//     with unrelated UI state, its grid came out white).
//
// On a truecolor RTG screen pen drawing resolves the colormap at
// draw time, so reprogramming the scratch pen doesn't repaint
// earlier pixels. Alpha is dropped — the AmigaOS palette has no
// alpha channel — and the value is masked to 24 bits so a
// 0xFFRRGGBB passed from JS doesn't sign-extend.
#define AM_UI_SCRATCH_PEN_FG 255
#define AM_UI_SCRATCH_PEN_BG 254

function_result Am_Ui_LayerGraphics_setForegroundColor_0(aobject * const this, unsigned int argb)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    struct RastPort *rp = get_rp(this);
    if (data != NULL) {
        data->fg_color = argb & 0xFFFFFFUL;
        data->fg_color_active = TRUE;
        if (data->screen_vp != NULL) {
            ULONG r = (data->fg_color >> 16) & 0xFF;
            ULONG g = (data->fg_color >> 8) & 0xFF;
            ULONG b = data->fg_color & 0xFF;
            SetRGB32(data->screen_vp, AM_UI_SCRATCH_PEN_FG,
                r * 0x01010101UL, g * 0x01010101UL, b * 0x01010101UL);
            if (rp != NULL) SetAPen(rp, AM_UI_SCRATCH_PEN_FG);
        }
    }
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_setBackgroundColor_0(aobject * const this, unsigned int argb)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    struct RastPort *rp = get_rp(this);
    if (data != NULL) {
        data->bg_color = argb & 0xFFFFFFUL;
        data->bg_color_active = TRUE;
        if (data->screen_vp != NULL) {
            ULONG r = (data->bg_color >> 16) & 0xFF;
            ULONG g = (data->bg_color >> 8) & 0xFF;
            ULONG b = data->bg_color & 0xFF;
            SetRGB32(data->screen_vp, AM_UI_SCRATCH_PEN_BG,
                r * 0x01010101UL, g * 0x01010101UL, b * 0x01010101UL);
            if (rp != NULL) SetBPen(rp, AM_UI_SCRATCH_PEN_BG);
        }
    }
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_drawLine_0(aobject * const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    struct RastPort *rp = get_rp(this);
    short tx1 = lg_translated_x(this, x), ty1 = lg_translated_y(this, y);
    short tx2 = lg_translated_x(this, x2), ty2 = lg_translated_y(this, y2);
    if (rp == NULL) goto __exit;
    // In colour mode setForegroundColor already SetAPen'd the scratch
    // pen (255) programmed to the requested RGB, so the plain pen path
    // renders the correct colour — no separate direct-colour branch is
    // needed for lines (the sprite pixler's grid draws through here).
    Move(rp, tx1, ty1);
    Draw(rp, tx2, ty2);
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_fillRect_0(aobject * const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    struct RastPort *rp = get_rp(this);
    short tx1 = lg_translated_x(this, x), ty1 = lg_translated_y(this, y);
    short tx2 = lg_translated_x(this, x2), ty2 = lg_translated_y(this, y2);
    if (rp == NULL) goto __exit;
    // Colour-mode fill — CGFX's FillPixelArray paints the
    // rectangle with the stashed 24-bit ARGB without touching
    // the colormap, which is what makes the multi-colour
    // sprite palette work. Falls back to RectFill (pen path)
    // when no setForegroundColor preceded this call.
    if (data != NULL && data->fg_color_active) {
        UWORD w = (UWORD)(tx2 - tx1 + 1);
        UWORD h = (UWORD)(ty2 - ty1 + 1);
        FillPixelArray(rp, tx1, ty1, w, h, data->fg_color);
    } else {
        RectFill(rp, tx1, ty1, tx2, ty2);
    }
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_eraseRect_0(aobject * const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    struct RastPort *rp = get_rp(this);
    short tx1 = lg_translated_x(this, x), ty1 = lg_translated_y(this, y);
    short tx2 = lg_translated_x(this, x2), ty2 = lg_translated_y(this, y2);
    if (rp == NULL) goto __exit;
    // Same colour-mode short-circuit, but for the background
    // colour. EraseRect normally paints with BPen; in colour
    // mode we substitute the cached bg_color via FillPixelArray.
    if (data != NULL && data->bg_color_active) {
        UWORD w = (UWORD)(tx2 - tx1 + 1);
        UWORD h = (UWORD)(ty2 - ty1 + 1);
        FillPixelArray(rp, tx1, ty1, w, h, data->bg_color);
    } else {
        EraseRect(rp, tx1, ty1, tx2, ty2);
    }
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_drawString_0(aobject * const this, aobject *text, short x, short y)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (text != NULL) {
        __increase_reference_count(text);
    }
    Am_Ui_LayerGraphics_data *data = get_layer_graphics_data(this);
    struct RastPort *rp = get_rp(this);
    if (rp == NULL || text == NULL) goto __exit;
    {
        string_holder *sh = __unwrap(text)->object_properties.class_object_properties.object_data.value.custom_value;
        struct TextFont *tf = rp->Font;
        short baseline = tf ? tf->tf_Baseline : 0;
        short ysize = tf ? tf->tf_YSize : 0;
        short tx = lg_translated_x(this, x);
        short ty = lg_translated_y(this, y + baseline);
        (void)ysize;
        // Colour-mode text has TWO independent needs, and they must NOT
        // share a gate:
        //
        //  * DRAW MODE — JAM2 fills every glyph cell with the B-pen,
        //    which is leaked chrome state (the pixler's status line grew
        //    a button-highlight band behind its text). Forcing JAM1
        //    (transparent glyph background — colour callers fill their
        //    own bg) kills the band. This needs NO viewport, so it is
        //    applied whenever the caller is in colour mode. That is the
        //    actual fix for the "coords panel gets the Ref background".
        //
        //  * FOREGROUND COLOUR — Text() draws glyphs in the A-pen. To get
        //    the exact requested RGB we select the scratch pen (255) that
        //    setForegroundColor programmed via SetRGB32. That DOES need a
        //    viewport; without one we leave whatever A-pen is current
        //    (the glyph colour may be off, but there is still no band).
        //
        // An explicit setBackgroundColor opts back into JAM2 with the bg
        // scratch pen (254) — only meaningful when a viewport is present.
        if (data != NULL && data->fg_color_active) {
            UBYTE oldDrMd = rp->DrawMode;
            if (data->bg_color_active && data->screen_vp != NULL) {
                SetBPen(rp, AM_UI_SCRATCH_PEN_BG);
                SetDrMd(rp, JAM2);
            } else {
                SetDrMd(rp, JAM1);
            }
            if (data->screen_vp != NULL) {
                SetAPen(rp, AM_UI_SCRATCH_PEN_FG);
            }
            Move(rp, tx, ty);
            Text(rp, sh->string_value, sh->length);
            SetDrMd(rp, oldDrMd);
        } else {
            Move(rp, tx, ty);
            Text(rp, sh->string_value, sh->length);
        }
    }
__exit: ;
    if (text != NULL) {
        __decrease_reference_count(text);
    }
    return __result;
}

function_result Am_Ui_LayerGraphics_calculateStringWidth_0(aobject * const this, aobject *text)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    if (text != NULL) {
        __increase_reference_count(text);
    }
    struct RastPort *rp = get_rp(this);
    if (rp != NULL && text != NULL) {
        string_holder *sh = __unwrap(text)->object_properties.class_object_properties.object_data.value.custom_value;
        __result.return_value.value.ushort_value = (unsigned short)TextLength(rp, sh->string_value, sh->length);
    }
__exit: ;
    if (text != NULL) {
        __decrease_reference_count(text);
    }
    return __result;
}

function_result Am_Ui_LayerGraphics_getCurrentFontSize_0(aobject * const this)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    struct RastPort *rp = get_rp(this);
    if (rp != NULL && rp->Font != NULL) {
        __result.return_value.value.uchar_value = rp->Font->tf_YSize;
    }
__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_setFont_0(aobject * const this, aobject *font)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (font != NULL) {
        __increase_reference_count(font);
    }
    struct RastPort *rp = get_rp(this);
    if (rp != NULL && font != NULL) {
        struct TextFont *tf = __unwrap(font)->object_properties.class_object_properties.object_data.value.custom_value;
        if (tf != NULL) SetFont(rp, tf);
    }
__exit: ;
    if (font != NULL) {
        __decrease_reference_count(font);
    }
    return __result;
}

function_result Am_Ui_LayerGraphics_drawImage_0(aobject * const this, aobject *image,
                                                 short x, short y, short width, short height)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (image != NULL) {
        __increase_reference_count(image);
    }
    struct RastPort *rp = get_rp(this);
    if (rp == NULL || image == NULL) goto __exit;
    {
        int pixFmt = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelFormat].nullable_value.value.int_value;
        if (pixFmt == 2) {
            aobject *pixelColorsObj =
                __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;
            unsigned short imgW = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_width].nullable_value.value.ushort_value;
            if (pixelColorsObj != NULL) {
                array_holder *ah = (array_holder *)&pixelColorsObj[1];
                unsigned int *pixels = (unsigned int *)(void *)&ah[1];
                WritePixelArray(pixels, 0, 0, imgW * 4,
                                rp,
                                lg_translated_x(this, x), lg_translated_y(this, y),
                                width, height,
                                RECTFMT_ARGB);
            }
        }
    }
__exit: ;
    if (image != NULL) {
        __decrease_reference_count(image);
    }
    return __result;
}

function_result Am_Ui_LayerGraphics_blitBitmapRect_0(aobject * const this, aobject *bitmap,
                                                       short srcX, short srcY,
                                                       unsigned short srcW, unsigned short srcH,
                                                       short dstX, short dstY)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (bitmap != NULL) {
        __increase_reference_count(bitmap);
    }
    struct RastPort *rp = get_rp(this);
    if (rp == NULL || bitmap == NULL) goto __exit;
    if (srcW == 0 || srcH == 0) goto __exit;
    {
        Am_Ui_Bitmap_data *bitmapData =
            (Am_Ui_Bitmap_data *)__unwrap(bitmap)->object_properties.class_object_properties.object_data.value.custom_value;
        if (bitmapData == NULL || bitmapData->bitmap == NULL) goto __exit;

        short tx = lg_translated_x(this, dstX);
        short ty = lg_translated_y(this, dstY);

        /* Mask-aware: when present, use BltMaskBitMapRastPort + 0xE0
         * so transparent pixels leave the destination intact.
         * Otherwise the original 0xC0 plain-copy. */
        if (bitmapData->mask != NULL) {
            BltMaskBitMapRastPort(bitmapData->bitmap, srcX, srcY,
                                  rp, tx, ty, srcW, srcH, 0xE0,
                                  (APTR)bitmapData->mask->Planes[0]);
        } else {
            BltBitMapRastPort(bitmapData->bitmap, srcX, srcY, rp, tx, ty, srcW, srcH, 0xC0);
        }
    }
__exit: ;
    if (bitmap != NULL) {
        __decrease_reference_count(bitmap);
    }
    return __result;
}

function_result Am_Ui_LayerGraphics_drawBitmap_0(aobject * const this, aobject *bitmap,
                                                  short x, short y,
                                                  short destWidth, short destHeight)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (bitmap != NULL) {
        __increase_reference_count(bitmap);
    }
    struct RastPort *rp = get_rp(this);
    if (rp == NULL || bitmap == NULL) goto __exit;
    if (destWidth <= 0 || destHeight <= 0) goto __exit;
    {
        Am_Ui_Bitmap_data *bitmapData =
            (Am_Ui_Bitmap_data *)__unwrap(bitmap)->object_properties.class_object_properties.object_data.value.custom_value;
        if (bitmapData == NULL || bitmapData->bitmap == NULL) goto __exit;

        unsigned short srcW = __unwrap(bitmap)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value;
        unsigned short srcH = __unwrap(bitmap)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value;
        short tx = lg_translated_x(this, x);
        short ty = lg_translated_y(this, y);

        if (destWidth == (short)srcW && destHeight == (short)srcH) {
            if (bitmapData->mask != NULL) {
                BltMaskBitMapRastPort(bitmapData->bitmap, 0, 0,
                                      rp, tx, ty, srcW, srcH, 0xE0,
                                      (APTR)bitmapData->mask->Planes[0]);
            } else {
                BltBitMapRastPort(bitmapData->bitmap, 0, 0, rp, tx, ty, srcW, srcH, 0xC0);
            }
        } else {
            ULONG depth = GetBitMapAttr(bitmapData->bitmap, BMA_DEPTH);
            struct BitMap *scaled = AllocBitMap(destWidth, destHeight, depth,
                                                BMF_CLEAR | BMF_MINPLANES, bitmapData->bitmap);
            if (scaled != NULL) {
                struct BitScaleArgs bsa;
                memset(&bsa, 0, sizeof(bsa));
                bsa.bsa_SrcBitMap   = bitmapData->bitmap;
                bsa.bsa_DestBitMap  = scaled;
                bsa.bsa_SrcWidth    = srcW;  bsa.bsa_SrcHeight  = srcH;
                bsa.bsa_XSrcFactor  = srcW;  bsa.bsa_YSrcFactor = srcH;
                bsa.bsa_DestWidth   = destWidth; bsa.bsa_DestHeight = destHeight;
                bsa.bsa_XDestFactor = destWidth; bsa.bsa_YDestFactor = destHeight;
                BitMapScale(&bsa);
                BltBitMapRastPort(scaled, 0, 0, rp, tx, ty, destWidth, destHeight, 0xC0);
                FreeBitMap(scaled);
            } else {
                BltBitMapRastPort(bitmapData->bitmap, 0, 0, rp, tx, ty, srcW, srcH, 0xC0);
            }
        }
    }
__exit: ;
    if (bitmap != NULL) {
        __decrease_reference_count(bitmap);
    }
    return __result;
}

// ScrollRaster's sign convention is opposite to ours: in ScrollRaster
// positive dx/dy moves pixels toward (0,0) — i.e. left/up. We flip the
// sign so callers can use the more intuitive "positive dy scrolls
// content down" convention. The exposed strip is filled with `fillPen`
// (via temporarily setting the rastport's APen + BPen + JAM2 mode so
// either fill style ScrollRaster uses lands on the right colour).
function_result Am_Ui_LayerGraphics_scrollRect_0(aobject * const this,
                                                   short x, short y,
                                                   unsigned short w, unsigned short h,
                                                   short dx, short dy,
                                                   unsigned char fillPen)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    struct RastPort *rp = get_rp(this);
    if (rp == NULL || w == 0 || h == 0) goto __exit;
    if (dx == 0 && dy == 0) goto __exit;

    short tx = lg_translated_x(this, x);
    short ty = lg_translated_y(this, y);
    short tx2 = tx + (short)w - 1;
    short ty2 = ty + (short)h - 1;

    UBYTE prevAPen = rp->FgPen;
    UBYTE prevBPen = rp->BgPen;
    UBYTE prevDrMd = rp->DrawMode;

    SetAPen(rp, fillPen);
    SetBPen(rp, fillPen);
    SetDrMd(rp, JAM2);

    ScrollRaster(rp, -dx, -dy, tx, ty, tx2, ty2);

    SetAPen(rp, prevAPen);
    SetBPen(rp, prevBPen);
    SetDrMd(rp, prevDrMd);

__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_setClipRect_0(aobject * const this, struct Am_Ui_ClipRect * clipRect)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;

    // clipRect is a struct value-type (not ARC-tracked); no ref count adjustments needed.
    apply_clip_rect(this, clipRect);

__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_clearClipRect_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;

    struct Layer *layer = get_layer(this);
    if (layer != NULL) {
        InstallClipRegion(layer, NULL);
    }

__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_beginPainting_0(aobject * const this, struct Am_Ui_ClipRect * clipRect)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;

    // clipRect is a struct value-type (not ARC-tracked); no ref count adjustments needed.
    apply_clip_rect(this, clipRect);

__exit: ;
    return __result;
}

function_result Am_Ui_LayerGraphics_endPainting_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;

    struct Layer *layer = get_layer(this);
    if (layer != NULL) {
        InstallClipRegion(layer, NULL);
    }

__exit: ;
    return __result;
}
