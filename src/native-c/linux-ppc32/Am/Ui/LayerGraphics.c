#ifndef native_aclass_Am_Ui_LayerGraphics_c
#define native_aclass_Am_Ui_LayerGraphics_c

#include <libc/core.h>
#include <Am/Ui/LayerGraphics.h>
#include <linux-x64/Am/Ui/LayerGraphics.h>
#include <linux-x64/Am/Ui/Window.h>
#include <linux-x64/Am/Ui/Bitmap.h>
#include <linux-x64/Am/Ui/RenderableBitmap.h>
#include <linux-x64/Am/Ui/Font.h>
#include <linux-x64/Am/Ui/Screen.h>
#include <Am/Ui/RenderableBitmap.h>
#include <Am/Ui/Window.h>
#include <Am/Ui/Bitmap.h>
#include <Am/Ui/Font.h>
#include <Am/Ui/ClipRect.h>
#include <Am/Imaging/Image.h>

#include <SDL2/SDL.h>
#ifdef AM_UI_LINUX_SDL_TTF
#  include <SDL2/SDL_ttf.h>
#endif

#include <libc/core_inline_functions.h>

// Linux Am.Ui.LayerGraphics — SDL2 paint primitives.
//
// The struct itself is in linux/Am/Ui/LayerGraphics.h; this file just
// implements every drawing primitive against the borrowed SDL_Renderer.
// We don't own the renderer — the Window or RenderableBitmap whose
// graphics we are does. begin/endPainting parks the renderer's target
// when we're a render-to-texture path; window paths leave the target
// as the default backbuffer.

static Am_Ui_LayerGraphics_data *lg_data(aobject *const this)
{
    return (Am_Ui_LayerGraphics_data *) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
}

static void lg_ensure_data(aobject *const this)
{
    if (lg_data(this) != NULL) return;
    Am_Ui_LayerGraphics_data *d = (Am_Ui_LayerGraphics_data *) calloc(1, sizeof(Am_Ui_LayerGraphics_data));
    if (d != NULL) {
        d->foreground = (SDL_Color) { 255, 255, 255, 255 };
        d->background = (SDL_Color) { 0, 0, 0, 255 };
        d->pen_palette = am_ui_linux_screen_palette();
        d->pen_palette_count = am_ui_linux_screen_palette_count();
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = d;
    }
}

// Translate an ARGB Uint32 (0xAARRGGBB) into SDL_Color.
static SDL_Color argb_to_sdl(Uint32 argb)
{
    SDL_Color c;
    c.a = (Uint8) ((argb >> 24) & 0xFF);
    c.r = (Uint8) ((argb >> 16) & 0xFF);
    c.g = (Uint8) ((argb >>  8) & 0xFF);
    c.b = (Uint8) ( argb        & 0xFF);
    if (c.a == 0) c.a = 255;   // treat 0x00RRGGBB as opaque
    return c;
}

static void apply_foreground(Am_Ui_LayerGraphics_data *d)
{
    SDL_SetRenderDrawColor(d->renderer, d->foreground.r, d->foreground.g, d->foreground.b, d->foreground.a);
}

// Ensure the renderer's current target matches what this LayerGraphics
// expects (the off-screen TARGET texture, or NULL for the window). The
// AmLang side never calls beginPainting/endPainting — both Window's
// on-screen graphics and Window.getOffscreenGraphics() share the same
// SDL renderer, and draws can interleave (e.g. off-screen fillRect →
// on-screen blitBitmapRect → off-screen fillRect again). Setting the
// target on every draw is the cheapest correctness fix; SDL no-ops the
// call when the target is already what you ask for.
// Cache the last target we asked SDL to bind, per-renderer. SDL's
// SDL_GetRenderTarget round-trips into the backend on the OpenGL path
// and over hundreds of calls per paint cycle that adds up enough to
// freeze the IDE's main repaint.
typedef struct {
    SDL_Renderer *renderer;
    SDL_Texture  *target;
} target_cache_entry;
#define APPLY_TARGET_CACHE 8
static target_cache_entry g_target_cache[APPLY_TARGET_CACHE];

// Toggle via env var AMUI_CLIP_LOG=1. Lazy-init the first time it is
// queried so the env-var lookup is one-shot and doesn't depend on
// constructor-attribute survival through the linker.
bool am_ui_macos_arm_clip_log_enabled = false;
static bool clip_log_inited = false;
static inline bool clip_log_on(void) {
    if (!clip_log_inited) {
        const char *e = getenv("AMUI_CLIP_LOG");
        am_ui_macos_arm_clip_log_enabled = (e != NULL && e[0] == '1');
        clip_log_inited = true;
    }
    return am_ui_macos_arm_clip_log_enabled;
}

static inline void apply_target(Am_Ui_LayerGraphics_data *d)
{
    if (d == NULL || d->renderer == NULL) return;
    SDL_Texture *want = d->target_texture; /* NULL == window */

    int empty = -1;
    for (int i = 0; i < APPLY_TARGET_CACHE; i++) {
        if (g_target_cache[i].renderer == d->renderer) {
            if (g_target_cache[i].target == want) return;
            SDL_SetRenderTarget(d->renderer, want);
            g_target_cache[i].target = want;
            return;
        }
        if (empty < 0 && g_target_cache[i].renderer == NULL) empty = i;
    }
    SDL_SetRenderTarget(d->renderer, want);
    if (empty >= 0) {
        g_target_cache[empty].renderer = d->renderer;
        g_target_cache[empty].target   = want;
    }
}

// Re-stamp the SDL clip rect on the current target. SDL is meant to
// preserve per-target clip state across SetRenderTarget, but the
// OpenGL backend's clip occasionally goes stale after a
// SDL_CreateTextureFromSurface (drawString allocates a per-glyph
// texture) or a target ping. Forcing the AmLang-side clip back into
// SDL before every primitive keeps the scroll list inside its
// viewport at the cost of one extra SDL call per draw — measured
// negligible on the IDE's paint cadence.
static inline void restamp_clip(Am_Ui_LayerGraphics_data *d)
{
    if (d == NULL || d->renderer == NULL) return;
    SDL_RenderSetClipRect(d->renderer, d->clip_active ? &d->clip_rect : NULL);
}

// Called from outside (e.g. Window.handleInput) after a direct
// SDL_SetRenderTarget call so the cache doesn't go stale and start
// suppressing legitimate target switches on subsequent draws.
void am_ui_macos_arm_lg_target_changed(SDL_Renderer *renderer, SDL_Texture *now)
{
    if (renderer == NULL) return;
    for (int i = 0; i < APPLY_TARGET_CACHE; i++) {
        if (g_target_cache[i].renderer == renderer) {
            g_target_cache[i].target = now;
            return;
        }
    }
}

// `Graphics.translate(x, y)` is a regular AmLang fun (defined in
// Am/Ui/Graphics.aml) that mutates the `xOffset` / `yOffset` fields
// directly — no native dispatch — so the LayerGraphics_data's own
// translate_x / translate_y mirror is stale by the time any draw
// primitive fires. Read the canonical values straight off the AmLang
// object every draw. Cheap (two field reads) and keeps the offset
// math always in sync with the AmLang-side push/pop translate stack.
static inline int tx_of(aobject *const this, int x)
{
    short xoff = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_xOffset].nullable_value.value.short_value;
    return x + (int) xoff;
}
static inline int ty_of(aobject *const this, int y)
{
    short yoff = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_yOffset].nullable_value.value.short_value;
    return y + (int) yoff;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

function_result Am_Ui_LayerGraphics__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    lg_ensure_data(this);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}

function_result Am_Ui_LayerGraphics__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL) {
        free(d);
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }
    return __result;
}

// ---------------------------------------------------------------------------
// attachWindow / attachRenderableBitmap
//
// These are called by AmLang when a LayerGraphics is wired up to its
// target. We pull the SDL_Renderer out of the Window or
// RenderableBitmap and stash it on our data so subsequent paint calls
// know where to draw.
// ---------------------------------------------------------------------------

function_result Am_Ui_LayerGraphics_attachWindow_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    lg_ensure_data(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL) {
        // Pull the Window out of the AmLang-side `window` property.
        aobject *window = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_LayerGraphics_P_window].nullable_value.value.object_value;
        if (window != NULL) {
            Am_Ui_Window_data *wd = (Am_Ui_Window_data *) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
            if (wd != NULL) {
                d->renderer = wd->renderer;
                d->target_texture = NULL;
            }
        }
        // Refresh palette pointer in case Screen has come up since
        // init.
        d->pen_palette = am_ui_linux_screen_palette();
        d->pen_palette_count = am_ui_linux_screen_palette_count();
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_attachRenderableBitmap_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    lg_ensure_data(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL) {
        aobject *rb = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_LayerGraphics_P_renderableBitmap].nullable_value.value.object_value;
        if (rb != NULL) {
            Am_Ui_RenderableBitmap_data *rbd = (Am_Ui_RenderableBitmap_data *) __unwrap(rb)->object_properties.class_object_properties.object_data.value.custom_value;
            if (rbd != NULL) {
                d->renderer = rbd->renderer;
                d->target_texture = rbd->texture;
                d->bypass_offscreen = false;
            }
        }
        d->pen_palette = am_ui_linux_screen_palette();
        d->pen_palette_count = am_ui_linux_screen_palette_count();
    }
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Paint cycle
// ---------------------------------------------------------------------------

function_result Am_Ui_LayerGraphics_beginPainting_0(aobject *const this, struct Am_Ui_ClipRect *clipRect)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL && d->renderer != NULL) {
        if (d->target_texture != NULL) {
            SDL_SetRenderTarget(d->renderer, d->target_texture);
        }
        d->painting = true;
        d->translate_x = 0;
        d->translate_y = 0;
        if (clipRect != NULL) {
            d->clip_rect.x = clipRect->x;
            d->clip_rect.y = clipRect->y;
            d->clip_rect.w = (clipRect->width  > 0) ? clipRect->width  : 0;
            d->clip_rect.h = (clipRect->height > 0) ? clipRect->height : 0;
            d->clip_active = (d->clip_rect.w > 0 && d->clip_rect.h > 0);
            if (d->clip_active) {
                SDL_RenderSetClipRect(d->renderer, &d->clip_rect);
            } else {
                SDL_RenderSetClipRect(d->renderer, NULL);
            }
        } else {
            d->clip_active = false;
            SDL_RenderSetClipRect(d->renderer, NULL);
        }
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_endPainting_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL && d->renderer != NULL) {
        SDL_RenderSetClipRect(d->renderer, NULL);
        if (d->target_texture != NULL) {
            SDL_SetRenderTarget(d->renderer, NULL);
        }
        d->painting = false;
        d->clip_active = false;
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_setClipRect_0(aobject *const this, struct Am_Ui_ClipRect *clipRect)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL && d->renderer != NULL && clipRect != NULL) {
        apply_target(d);
        d->clip_rect.x = clipRect->x;
        d->clip_rect.y = clipRect->y;
        d->clip_rect.w = (clipRect->width  > 0) ? clipRect->width  : 0;
        d->clip_rect.h = (clipRect->height > 0) ? clipRect->height : 0;
        // A clip pushed by AmLang is always active, even when it's
        // zero-sized — that's the legitimate "child is entirely
        // outside the parent's clip" case (e.g. a scroll-list card
        // scrolled past the viewport, where createInnerClipRect
        // intersected to empty). Treating zero-sized as "no clip"
        // (NULL to SDL) disables clipping and lets subsequent
        // primitives draw anywhere on the target, which is the bug
        // that surfaced as scroll-list rows painting over the
        // search field above the viewport. SDL discards draws
        // against an empty clip, which is what we want.
        d->clip_active = true;
        SDL_RenderSetClipRect(d->renderer, &d->clip_rect);
        if (clip_log_on()) {
            fprintf(stderr, "[clip.set] tgt=%p x=%d y=%d w=%d h=%d active=%d trX=%d trY=%d\n",
                (void*) d->target_texture, d->clip_rect.x, d->clip_rect.y,
                d->clip_rect.w, d->clip_rect.h, d->clip_active,
                d->translate_x, d->translate_y);
        }
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_clearClipRect_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL && d->renderer != NULL) {
        apply_target(d);
        SDL_RenderSetClipRect(d->renderer, NULL);
        d->clip_active = false;
    }
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// translate — AmLang exposes this as an instance method on Graphics
// (compiled to setTranslate / addTranslate). The amigaos backend
// implements it via a single setTranslate that swaps the absolute
// offset. We match.
// ---------------------------------------------------------------------------

function_result Am_Ui_LayerGraphics_translate_0(aobject *const this, short dx, short dy)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL) {
        d->translate_x += dx;
        d->translate_y += dy;
    }
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Pen + colour state
// ---------------------------------------------------------------------------

function_result Am_Ui_LayerGraphics_setForegroundPen_0(aobject *const this, unsigned char pen)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL) {
        const Uint32 *pal = d->pen_palette ? d->pen_palette : am_ui_linux_screen_palette();
        int n = d->pen_palette_count > 0 ? d->pen_palette_count : am_ui_linux_screen_palette_count();
        Uint32 argb = (pen < n) ? pal[pen] : 0xFFFFFFFFu;
        d->foreground = argb_to_sdl(argb);
        if (d->renderer != NULL) apply_foreground(d);
        static int dbg_n = 0;
        if (dbg_n < 8) {
            fprintf(stderr, "[am-ui/pen] setFgPen(%u) -> argb=0x%08x -> sdl(r=%u g=%u b=%u a=%u) pal=%p n=%d\n",
                (unsigned) pen, (unsigned) argb,
                (unsigned) d->foreground.r, (unsigned) d->foreground.g,
                (unsigned) d->foreground.b, (unsigned) d->foreground.a,
                (void *) pal, n);
            dbg_n++;
        }
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_setBackgroundPen_0(aobject *const this, unsigned char pen)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL) {
        const Uint32 *pal = d->pen_palette ? d->pen_palette : am_ui_linux_screen_palette();
        int n = d->pen_palette_count > 0 ? d->pen_palette_count : am_ui_linux_screen_palette_count();
        Uint32 argb = (pen < n) ? pal[pen] : 0xFF000000u;
        d->background = argb_to_sdl(argb);
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_setForegroundColor_0(aobject *const this, unsigned int argb)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL) {
        d->foreground = argb_to_sdl(argb);
        if (d->renderer != NULL) apply_foreground(d);
        static int dbg_n = 0;
        if (dbg_n < 12) {
            fprintf(stderr, "[am-ui/color] setFgColor(0x%08x) r=%p lg=%p -> sdl(r=%u g=%u b=%u a=%u)\n",
                (unsigned) argb, (void *) d->renderer, (void *) d,
                (unsigned) d->foreground.r, (unsigned) d->foreground.g,
                (unsigned) d->foreground.b, (unsigned) d->foreground.a);
            dbg_n++;
        }
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_setBackgroundColor_0(aobject *const this, unsigned int argb)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL) {
        d->background = argb_to_sdl(argb);
    }
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Primitives
// ---------------------------------------------------------------------------

function_result Am_Ui_LayerGraphics_fillRect_0(aobject *const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL && d->renderer != NULL) {
        apply_target(d);
        restamp_clip(d);
        apply_foreground(d);
        SDL_Rect r;
        r.x = tx_of(this, x);
        r.y = ty_of(this, y);
        r.w = (x2 - x) + 1;
        r.h = (y2 - y) + 1;
        if (r.w > 0 && r.h > 0) {
            Uint8 sr=0, sg=0, sb=0, sa=0;
            SDL_GetRenderDrawColor(d->renderer, &sr, &sg, &sb, &sa);
            int rfr = SDL_RenderFillRect(d->renderer, &r);
            // Per-renderer log so the splash doesn't burn the whole budget.
            // Each distinct SDL_Renderer* gets its own slot; first 8 calls
            // per renderer are logged, then every 50th up to 200 total.
            static struct { void *r; int n; } per[8];
            int slot = -1;
            for (int k = 0; k < 8; k++) {
                if (per[k].r == d->renderer) { slot = k; break; }
                if (per[k].r == NULL && slot < 0) slot = k;
            }
            if (slot >= 0 && per[slot].r == NULL) per[slot].r = d->renderer;
            if (slot >= 0) {
                int n = per[slot].n;
                if (n < 8 || (n % 50 == 0 && n < 200)) {
                    fprintf(stderr, "[am-ui/fillRect r=%p n=%d] tgt=%p rect=%dx%d at (%d,%d) fg(%u,%u,%u) SDL_fg(%u,%u,%u,%u) clip=%d rc=%d\n",
                        d->renderer, n, (void*) d->target_texture, r.w, r.h, r.x, r.y,
                        (unsigned) d->foreground.r, (unsigned) d->foreground.g, (unsigned) d->foreground.b,
                        (unsigned) sr, (unsigned) sg, (unsigned) sb, (unsigned) sa,
                        (int) d->clip_active, rfr);
                }
                per[slot].n++;
            }
            if (clip_log_on()) {
                fprintf(stderr, "[fillRect] tgt=%p x=%d y=%d w=%d h=%d clip(x=%d y=%d w=%d h=%d active=%d)\n",
                    (void*) d->target_texture, r.x, r.y, r.w, r.h,
                    d->clip_rect.x, d->clip_rect.y, d->clip_rect.w, d->clip_rect.h, d->clip_active);
            }
        }
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_eraseRect_0(aobject *const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL && d->renderer != NULL) {
        apply_target(d);
        restamp_clip(d);
        // eraseRect uses the background colour, not the foreground.
        SDL_SetRenderDrawColor(d->renderer, d->background.r, d->background.g, d->background.b, d->background.a);
        SDL_Rect r;
        r.x = tx_of(this, x); r.y = ty_of(this, y);
        r.w = (x2 - x) + 1; r.h = (y2 - y) + 1;
        if (r.w > 0 && r.h > 0) SDL_RenderFillRect(d->renderer, &r);
        apply_foreground(d);
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_drawLine_0(aobject *const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL && d->renderer != NULL) {
        apply_target(d);
        restamp_clip(d);
        apply_foreground(d);
        SDL_RenderDrawLine(d->renderer, tx_of(this, x), ty_of(this, y), tx_of(this, x2), ty_of(this, y2));
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_scrollRect_0(aobject *const this, short x, short y, unsigned short w, unsigned short h, short dx, short dy, unsigned char fillPen)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    // AmigaOS ScrollRaster equivalent: shift a rectangle of the current
    // render target by (dx, dy) and clear the vacated strip to fillPen.
    // The TextEditor uses this for fast scrolled redraws — without it the
    // lines below an insertion never move and look overwritten/deleted.
    //
    // SDL can't blit a texture onto itself with overlap, so bounce the
    // region through a scratch target texture: copy source -> scratch,
    // then scratch -> target at the shifted position.
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d == NULL || d->renderer == NULL || w == 0 || h == 0) goto __exit;
    apply_target(d);
    SDL_Texture *cur = d->target_texture;   // editor renders into the offscreen
    if (cur == NULL) goto __exit;            // scrolling the window directly isn't supported

    int ax = tx_of(this, x);
    int ay = ty_of(this, y);

    SDL_Texture *scratch = SDL_CreateTexture(d->renderer, SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_TARGET, w, h);
    if (scratch == NULL) goto __exit;
    SDL_SetTextureBlendMode(scratch, SDL_BLENDMODE_NONE);

    SDL_Rect whole = { 0, 0, w, h };
    SDL_Rect srcRect = { ax, ay, w, h };
    // source region -> scratch (straight copy)
    SDL_SetRenderTarget(d->renderer, scratch);
    SDL_RenderSetClipRect(d->renderer, NULL);
    SDL_RenderCopy(d->renderer, cur, &srcRect, &whole);

    // back to the offscreen; scratch -> shifted destination (overwrite)
    SDL_SetRenderTarget(d->renderer, cur);
    am_ui_macos_arm_lg_target_changed(d->renderer, cur);
    SDL_RenderSetClipRect(d->renderer, NULL);
    SDL_Rect dstRect = { ax + dx, ay + dy, w, h };
    SDL_RenderCopy(d->renderer, scratch, &whole, &dstRect);
    SDL_DestroyTexture(scratch);

    // Clear the strip the scroll exposed (bg fill) so stale pixels don't
    // peek through before the dirty lines re-render over it.
    const Uint32 *pal = d->pen_palette ? d->pen_palette : am_ui_linux_screen_palette();
    int n = d->pen_palette_count > 0 ? d->pen_palette_count : am_ui_linux_screen_palette_count();
    Uint32 argb = ((int) fillPen < n) ? pal[fillPen] : 0xFF000000u;
    SDL_Color fc = argb_to_sdl(argb);
    SDL_SetRenderDrawColor(d->renderer, fc.r, fc.g, fc.b, fc.a);
    if (dy > 0) {
        SDL_Rect v = { ax, ay, w, dy };
        SDL_RenderFillRect(d->renderer, &v);
    } else if (dy < 0) {
        SDL_Rect v = { ax, ay + (int) h + dy, w, -dy };
        SDL_RenderFillRect(d->renderer, &v);
    }
    if (dx > 0) {
        SDL_Rect v = { ax, ay, dx, h };
        SDL_RenderFillRect(d->renderer, &v);
    } else if (dx < 0) {
        SDL_Rect v = { ax + (int) w + dx, ay, -dx, h };
        SDL_RenderFillRect(d->renderer, &v);
    }
    restamp_clip(d);
    apply_foreground(d);

__exit:
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Bitmap blits
// ---------------------------------------------------------------------------

function_result Am_Ui_LayerGraphics_drawBitmap_0(aobject *const this, aobject *bitmap, short x, short y, short destWidth, short destHeight)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (bitmap != NULL) __increase_reference_count(bitmap);

    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d == NULL || d->renderer == NULL || bitmap == NULL) goto __exit;
    if (destWidth <= 0 || destHeight <= 0) goto __exit;

    Am_Ui_Bitmap_data *bd = (Am_Ui_Bitmap_data *) __unwrap(bitmap)->object_properties.class_object_properties.object_data.value.custom_value;
    if (bd == NULL) goto __exit;
    apply_target(d);

    // Lazy texture upload for surface-backed bitmaps. The texture is
    // re-used until the surface or the renderer changes.
    if (bd->texture == NULL && bd->surface != NULL) {
        bd->texture = SDL_CreateTextureFromSurface(d->renderer, bd->surface);
        bd->bound_renderer = d->renderer;
        if (bd->texture != NULL && bd->mask_enabled) {
            SDL_SetTextureBlendMode(bd->texture, SDL_BLENDMODE_BLEND);
        }
    } else if (bd->texture != NULL && bd->bound_renderer != d->renderer && !bd->is_render_target) {
        // Renderer changed; rebind by re-uploading. Cheap for icon-
        // sized bitmaps, problematic for big ones — flag noisily so a
        // future caller knows where to optimise.
        SDL_DestroyTexture(bd->texture);
        bd->texture = NULL;
        if (bd->surface != NULL) {
            bd->texture = SDL_CreateTextureFromSurface(d->renderer, bd->surface);
            bd->bound_renderer = d->renderer;
            if (bd->texture != NULL && bd->mask_enabled) {
                SDL_SetTextureBlendMode(bd->texture, SDL_BLENDMODE_BLEND);
            }
        }
    }

    if (bd->texture != NULL) {
        restamp_clip(d);
        SDL_Rect dst;
        dst.x = tx_of(this, x);
        dst.y = ty_of(this, y);
        dst.w = destWidth;
        dst.h = destHeight;
        SDL_RenderCopy(d->renderer, bd->texture, NULL, &dst);
    }

__exit:
    if (bitmap != NULL) __decrease_reference_count(bitmap);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_blitBitmapRect_0(aobject *const this, aobject *bitmap, short srcX, short srcY, unsigned short srcW, unsigned short srcH, short dstX, short dstY)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (bitmap != NULL) __increase_reference_count(bitmap);

    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d == NULL || d->renderer == NULL || bitmap == NULL) goto __exit;
    Am_Ui_Bitmap_data *bd = (Am_Ui_Bitmap_data *) __unwrap(bitmap)->object_properties.class_object_properties.object_data.value.custom_value;
    if (bd == NULL || bd->texture == NULL) goto __exit;
    apply_target(d);
    restamp_clip(d);

    SDL_Rect src = { srcX, srcY, srcW, srcH };
    SDL_Rect dst = { tx_of(this, dstX), ty_of(this, dstY), srcW, srcH };
    SDL_RenderCopy(d->renderer, bd->texture, &src, &dst);

__exit:
    if (bitmap != NULL) __decrease_reference_count(bitmap);
    __decrease_reference_count(this);
    return __result;
}

// drawImage — used by views that hand us an Am.Imaging.Image directly
// (not a Bitmap). TODO(linux): wire up surface upload + cache. For
// initial bring-up this is a no-op; the IDE rarely takes this path
// (ImageButton goes through Bitmap), so the practical impact is small.
function_result Am_Ui_LayerGraphics_drawImage_0(aobject *const this, aobject *image, short x, short y, short width, short height)
{
    function_result __result = { .has_return_value = false };
    (void) this; (void) image; (void) x; (void) y; (void) width; (void) height;
    return __result;
}

// ---------------------------------------------------------------------------
// Text
// ---------------------------------------------------------------------------

function_result Am_Ui_LayerGraphics_setFont_0(aobject *const this, aobject *font)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (font != NULL) __increase_reference_count(font);

    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d != NULL) {
        if (font != NULL) {
            Am_Ui_Font_data *fd = (Am_Ui_Font_data *) __unwrap(font)->object_properties.class_object_properties.object_data.value.custom_value;
            d->current_font        = (fd != NULL) ? fd->ttf_font : NULL;
            d->current_font_height = (fd != NULL) ? fd->line_skip : 0;
        } else {
            d->current_font = NULL;
            d->current_font_height = 0;
        }
    }

    if (font != NULL) __decrease_reference_count(font);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_LayerGraphics_drawString_0(aobject *const this, aobject *text, short x, short y)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (text != NULL) __increase_reference_count(text);

#ifdef AM_UI_LINUX_SDL_TTF
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    if (d == NULL || d->renderer == NULL || text == NULL || d->current_font == NULL) goto __exit;

    string_holder *sh = (string_holder *) (text + 1);
    if (sh == NULL || sh->string_value == NULL) goto __exit;

    SDL_Surface *surf = TTF_RenderUTF8_Blended((TTF_Font *) d->current_font, sh->string_value, d->foreground);
    if (surf == NULL) goto __exit;
    SDL_Texture *tex = SDL_CreateTextureFromSurface(d->renderer, surf);
    if (tex != NULL) {
        apply_target(d);
        restamp_clip(d);
        SDL_Rect dst = { tx_of(this, x), ty_of(this, y), surf->w, surf->h };
        SDL_RenderCopy(d->renderer, tex, NULL, &dst);
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

function_result Am_Ui_LayerGraphics_calculateStringWidth_0(aobject *const this, aobject *text)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    if (text != NULL) __increase_reference_count(text);

    int width = 0;
#ifdef AM_UI_LINUX_SDL_TTF
    Am_Ui_LayerGraphics_data *d = lg_data(this);
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

function_result Am_Ui_LayerGraphics_getCurrentFontSize_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    Am_Ui_LayerGraphics_data *d = lg_data(this);
    // AmLang signature returns UByte — must write into uchar_value, not
    // ushort_value. The function_result.return_value.value union shares
    // the same memory for every field, but the AmLang caller reads from
    // the field that matches the declared return type. Writing the
    // wider ushort and reading the narrower uchar takes the low byte on
    // little-endian (so x64/macOS "worked" by accident) and the *high*
    // byte (always zero for fonts ≤ 255) on big-endian PPC — which
    // collapsed every CheckBox/TextField height to fontHeight=0.
    unsigned char fh = (unsigned char) ((d != NULL && d->current_font_height > 0)
        ? (d->current_font_height > 255 ? 255 : d->current_font_height) : 0);
    __result.return_value.value.uchar_value = fh;
    __decrease_reference_count(this);
    return __result;
}

#endif
