#ifndef native_aclass_Am_Ui_ViewContextGraphics_c
#define native_aclass_Am_Ui_ViewContextGraphics_c

#include <libc/core.h>
#include <Am/Ui/ViewContextGraphics.h>
#include <Am/Ui/Graphics.h>
#include <Am/Ui/ViewContext.h>
#include <Am/Ui/ClipRect.h>
#include <macos-arm/Am/Ui/Window.h>
#include <macos-arm/Am/Ui/Bitmap.h>
#include <macos-arm/Am/Ui/Font.h>
#include <macos-arm/Am/Ui/Screen.h>
#include <Am/Ui/Bitmap.h>
#include <Am/Ui/Font.h>
#include <Am/Imaging/Image.h>

#include <SDL2/SDL.h>
#ifdef AM_UI_LINUX_SDL_TTF
#  include <SDL2/SDL_ttf.h>
#endif

#include <libc/core_inline_functions.h>

// Linux Am.Ui.ViewContextGraphics
//
// This is the per-view "default graphics" — every View.paint(graphics)
// gets one of these. On AmigaOS it draws directly on the parent
// Window's RastPort; on Linux it draws to the SDL_Renderer of the
// containing Window, looked up via ViewContext → Window. The drawing
// primitives are identical to LayerGraphics.c — kept as a separate
// file for native-class symbol routing.
//
// State is stored in the AmLang Graphics base class properties
// (xOffset / yOffset etc.) plus a small data struct for SDL state.

typedef struct {
    SDL_Renderer *renderer;       // borrowed from owning Window
    SDL_Color     foreground;
    SDL_Color     background;
    SDL_Rect      clip_rect;
    bool          clip_active;
    void         *current_font;   // TTF_Font *
    int           current_font_height;
    const Uint32 *pen_palette;
    int           pen_palette_count;
} Am_Ui_ViewContextGraphics_data;

static Am_Ui_ViewContextGraphics_data *vcg_data(aobject *const this)
{
    return (Am_Ui_ViewContextGraphics_data *) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
}

static void vcg_ensure_data(aobject *const this)
{
    if (vcg_data(this) != NULL) return;
    Am_Ui_ViewContextGraphics_data *d = (Am_Ui_ViewContextGraphics_data *) calloc(1, sizeof(*d));
    if (d != NULL) {
        d->foreground = (SDL_Color) { 255, 255, 255, 255 };
        d->background = (SDL_Color) { 0, 0, 0, 255 };
        d->pen_palette = am_ui_linux_screen_palette();
        d->pen_palette_count = am_ui_linux_screen_palette_count();
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = d;
    }
}

// Pull the SDL_Renderer for the paint surface. Am.Ui.ViewContext is
// an interface, so we can't walk its `window` field directly from
// native C without unwrapping the iface_reference union — the same
// dance that bit the JS bridges. Sidestep by reading the process-
// global "most recently opened" renderer that Window.c publishes.
// Single-Window scope is fine for the IDE's bring-up; multi-window
// support is a P2 task that needs a per-Window registry.
static SDL_Renderer *vcg_renderer(aobject *const this)
{
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (d == NULL) return NULL;
    if (d->renderer == NULL) d->renderer = am_ui_linux_primary_renderer();
    return d->renderer;
}

// Graphics-base xOffset / yOffset are properties on Graphics; we read
// them per primitive to handle translate() correctly without our own
// shadow state.
static short get_xoff(aobject *const this) { return __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_xOffset].nullable_value.value.short_value; }
static short get_yoff(aobject *const this) { return __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_yOffset].nullable_value.value.short_value; }

static SDL_Color argb_to_sdl(Uint32 argb)
{
    SDL_Color c;
    c.a = (Uint8) ((argb >> 24) & 0xFF);
    c.r = (Uint8) ((argb >> 16) & 0xFF);
    c.g = (Uint8) ((argb >>  8) & 0xFF);
    c.b = (Uint8) ( argb        & 0xFF);
    if (c.a == 0) c.a = 255;
    return c;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

function_result Am_Ui_ViewContextGraphics__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    vcg_ensure_data(this);
    __decrease_reference_count(this);
    return __result;
}
function_result Am_Ui_ViewContextGraphics__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}
function_result Am_Ui_ViewContextGraphics__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (d != NULL) {
        free(d);
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }
    return __result;
}

// ---------------------------------------------------------------------------
// Paint cycle (beginPainting / endPainting set the clip; the actual
// render-target binding is the Window's job — we always draw to the
// default backbuffer).
// ---------------------------------------------------------------------------

function_result Am_Ui_ViewContextGraphics_beginPainting_0(aobject *const this, struct Am_Ui_ClipRect *clipRect)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    SDL_Renderer *r = vcg_renderer(this);
    if (r != NULL && clipRect != NULL) {
        Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
        d->clip_rect.x = clipRect->x;
        d->clip_rect.y = clipRect->y;
        d->clip_rect.w = (clipRect->width  > 0) ? clipRect->width  : 0;
        d->clip_rect.h = (clipRect->height > 0) ? clipRect->height : 0;
        d->clip_active = (d->clip_rect.w > 0 && d->clip_rect.h > 0);
        SDL_RenderSetClipRect(r, d->clip_active ? &d->clip_rect : NULL);
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_ViewContextGraphics_endPainting_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    SDL_Renderer *r = vcg_renderer(this);
    if (r != NULL) SDL_RenderSetClipRect(r, NULL);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (d != NULL) d->clip_active = false;
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_ViewContextGraphics_setClipRect_0(aobject *const this, struct Am_Ui_ClipRect *clipRect)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    SDL_Renderer *r = vcg_renderer(this);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (r != NULL && d != NULL && clipRect != NULL) {
        d->clip_rect.x = clipRect->x;
        d->clip_rect.y = clipRect->y;
        d->clip_rect.w = (clipRect->width  > 0) ? clipRect->width  : 0;
        d->clip_rect.h = (clipRect->height > 0) ? clipRect->height : 0;
        // A pushed clip is always active, even when zero-sized — see
        // the matching comment in LayerGraphics.c. Treating empty as
        // NULL (no clip) is the bug, not the fix.
        d->clip_active = true;
        SDL_RenderSetClipRect(r, &d->clip_rect);
    }
    __decrease_reference_count(this);
    return __result;
}
function_result Am_Ui_ViewContextGraphics_clearClipRect_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    SDL_Renderer *r = vcg_renderer(this);
    if (r != NULL) SDL_RenderSetClipRect(r, NULL);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (d != NULL) d->clip_active = false;
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Pen / colour state
// ---------------------------------------------------------------------------

function_result Am_Ui_ViewContextGraphics_setForegroundPen_0(aobject *const this, unsigned char pen)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (d != NULL) {
        const Uint32 *pal = d->pen_palette ? d->pen_palette : am_ui_linux_screen_palette();
        int n = d->pen_palette_count > 0 ? d->pen_palette_count : am_ui_linux_screen_palette_count();
        Uint32 argb = (pen < n) ? pal[pen] : 0xFFFFFFFFu;
        d->foreground = argb_to_sdl(argb);
    }
    __decrease_reference_count(this);
    return __result;
}
function_result Am_Ui_ViewContextGraphics_setBackgroundPen_0(aobject *const this, unsigned char pen)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (d != NULL) {
        const Uint32 *pal = d->pen_palette ? d->pen_palette : am_ui_linux_screen_palette();
        int n = d->pen_palette_count > 0 ? d->pen_palette_count : am_ui_linux_screen_palette_count();
        Uint32 argb = (pen < n) ? pal[pen] : 0xFF000000u;
        d->background = argb_to_sdl(argb);
    }
    __decrease_reference_count(this);
    return __result;
}
function_result Am_Ui_ViewContextGraphics_setForegroundColor_0(aobject *const this, unsigned int argb)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (d != NULL) d->foreground = argb_to_sdl(argb);
    __decrease_reference_count(this);
    return __result;
}
function_result Am_Ui_ViewContextGraphics_setBackgroundColor_0(aobject *const this, unsigned int argb)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (d != NULL) d->background = argb_to_sdl(argb);
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Primitives
// ---------------------------------------------------------------------------

function_result Am_Ui_ViewContextGraphics_fillRect_0(aobject *const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    SDL_Renderer *r = vcg_renderer(this);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (r != NULL && d != NULL) {
        SDL_SetRenderDrawColor(r, d->foreground.r, d->foreground.g, d->foreground.b, d->foreground.a);
        SDL_Rect rc;
        rc.x = x + get_xoff(this); rc.y = y + get_yoff(this);
        rc.w = (x2 - x) + 1; rc.h = (y2 - y) + 1;
        if (rc.w > 0 && rc.h > 0) SDL_RenderFillRect(r, &rc);
    }
    __decrease_reference_count(this);
    return __result;
}
function_result Am_Ui_ViewContextGraphics_eraseRect_0(aobject *const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    SDL_Renderer *r = vcg_renderer(this);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (r != NULL && d != NULL) {
        SDL_SetRenderDrawColor(r, d->background.r, d->background.g, d->background.b, d->background.a);
        SDL_Rect rc;
        rc.x = x + get_xoff(this); rc.y = y + get_yoff(this);
        rc.w = (x2 - x) + 1; rc.h = (y2 - y) + 1;
        if (rc.w > 0 && rc.h > 0) SDL_RenderFillRect(r, &rc);
    }
    __decrease_reference_count(this);
    return __result;
}
function_result Am_Ui_ViewContextGraphics_drawLine_0(aobject *const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    SDL_Renderer *r = vcg_renderer(this);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (r != NULL && d != NULL) {
        SDL_SetRenderDrawColor(r, d->foreground.r, d->foreground.g, d->foreground.b, d->foreground.a);
        short ox = get_xoff(this), oy = get_yoff(this);
        SDL_RenderDrawLine(r, x + ox, y + oy, x2 + ox, y2 + oy);
    }
    __decrease_reference_count(this);
    return __result;
}
function_result Am_Ui_ViewContextGraphics_scrollRect_0(aobject *const this, short x, short y, unsigned short w, unsigned short h, short dx, short dy, unsigned char fillPen)
{
    function_result __result = { .has_return_value = false };
    (void) this; (void) x; (void) y; (void) w; (void) h; (void) dx; (void) dy; (void) fillPen;
    // TODO(linux): SDL_RenderCopy via a readback texture for fast scroll.
    return __result;
}

function_result Am_Ui_ViewContextGraphics_drawBitmap_0(aobject *const this, aobject *bitmap, short x, short y, short destWidth, short destHeight)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (bitmap != NULL) __increase_reference_count(bitmap);

    SDL_Renderer *r = vcg_renderer(this);
    if (r == NULL || bitmap == NULL || destWidth <= 0 || destHeight <= 0) goto __exit;

    Am_Ui_Bitmap_data *bd = (Am_Ui_Bitmap_data *) __unwrap(bitmap)->object_properties.class_object_properties.object_data.value.custom_value;
    if (bd == NULL) goto __exit;

    if (bd->texture == NULL && bd->surface != NULL) {
        bd->texture = SDL_CreateTextureFromSurface(r, bd->surface);
        bd->bound_renderer = r;
        if (bd->texture != NULL && bd->mask_enabled) SDL_SetTextureBlendMode(bd->texture, SDL_BLENDMODE_BLEND);
    }
    if (bd->texture != NULL) {
        SDL_Rect dst = { x + get_xoff(this), y + get_yoff(this), destWidth, destHeight };
        SDL_RenderCopy(r, bd->texture, NULL, &dst);
    }

__exit:
    if (bitmap != NULL) __decrease_reference_count(bitmap);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_ViewContextGraphics_drawImage_0(aobject *const this, aobject *image, short x, short y, short width, short height)
{
    function_result __result = { .has_return_value = false };
    (void) this; (void) image; (void) x; (void) y; (void) width; (void) height;
    return __result;
}

function_result Am_Ui_ViewContextGraphics_setFont_0(aobject *const this, aobject *font)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (font != NULL) __increase_reference_count(font);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (d != NULL) {
        if (font != NULL) {
            Am_Ui_Font_data *fd = (Am_Ui_Font_data *) __unwrap(font)->object_properties.class_object_properties.object_data.value.custom_value;
            d->current_font        = (fd != NULL) ? fd->ttf_font : NULL;
            d->current_font_height = (fd != NULL) ? fd->height   : 0;
        } else {
            d->current_font = NULL;
            d->current_font_height = 0;
        }
    }
    if (font != NULL) __decrease_reference_count(font);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_ViewContextGraphics_drawString_0(aobject *const this, aobject *text, short x, short y)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (text != NULL) __increase_reference_count(text);
#ifdef AM_UI_LINUX_SDL_TTF
    SDL_Renderer *r = vcg_renderer(this);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (r == NULL || d == NULL || text == NULL || d->current_font == NULL) goto __exit;
    string_holder *sh = (string_holder *) (text + 1);
    if (sh == NULL || sh->string_value == NULL) goto __exit;
    SDL_Surface *surf = TTF_RenderUTF8_Blended((TTF_Font *) d->current_font, sh->string_value, d->foreground);
    if (surf == NULL) goto __exit;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(r, surf);
    if (tex != NULL) {
        SDL_Rect dst = { x + get_xoff(this), y + get_yoff(this), surf->w, surf->h };
        SDL_RenderCopy(r, tex, NULL, &dst);
        SDL_DestroyTexture(tex);
    }
    SDL_FreeSurface(surf);
__exit:
#else
    (void) this; (void) text; (void) x; (void) y;
#endif
    if (text != NULL) __decrease_reference_count(text);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_ViewContextGraphics_calculateStringWidth_0(aobject *const this, aobject *text)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    if (text != NULL) __increase_reference_count(text);
    int width = 0;
#ifdef AM_UI_LINUX_SDL_TTF
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    if (d != NULL && d->current_font != NULL && text != NULL) {
        string_holder *sh = (string_holder *) (text + 1);
        if (sh != NULL && sh->string_value != NULL) {
            int h = 0;
            TTF_SizeUTF8((TTF_Font *) d->current_font, sh->string_value, &width, &h);
        }
    }
#endif
    __result.return_value.value.ushort_value = (unsigned short) width;
    if (text != NULL) __decrease_reference_count(text);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_ViewContextGraphics_getCurrentFontSize_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    Am_Ui_ViewContextGraphics_data *d = vcg_data(this);
    __result.return_value.value.ushort_value = (unsigned short) ((d != NULL) ? d->current_font_height : 0);
    __decrease_reference_count(this);
    return __result;
}

#endif
