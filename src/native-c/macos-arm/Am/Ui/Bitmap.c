#ifndef native_aclass_Am_Ui_Bitmap_c
#define native_aclass_Am_Ui_Bitmap_c

#include <libc/core.h>
#include <Am/Ui/Bitmap.h>
#include <macos-arm/Am/Ui/Bitmap.h>
#include <macos-arm/Am/Ui/Window.h>
#include <Am/Imaging/Image.h>

#include <SDL2/SDL.h>
#include <libc/core_inline_functions.h>

// Linux Am.Ui.Bitmap.
//
// Two flavours:
//   - source bitmap: backed by an SDL_Surface in ARGB8888 format.
//     Created from an Am.Imaging.Image via createFromImage /
//     createFromImageWithMask. Lazy-uploaded to a renderer-bound
//     SDL_Texture on first drawBitmap.
//   - empty bitmap: pixel canvas. We store an SDL_Surface so the
//     pixel data is CPU-readable; drawBitmap re-uploads on use.

static Am_Ui_Bitmap_data *bm_data(aobject *const this)
{
    return (Am_Ui_Bitmap_data *) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
}

static void bm_ensure_data(aobject *const this)
{
    if (bm_data(this) != NULL) return;
    Am_Ui_Bitmap_data *d = (Am_Ui_Bitmap_data *) calloc(1, sizeof(Am_Ui_Bitmap_data));
    if (d != NULL) {
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = d;
    }
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

function_result Am_Ui_Bitmap__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    bm_ensure_data(this);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Bitmap__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}

function_result Am_Ui_Bitmap__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    Am_Ui_Bitmap_data *d = bm_data(this);
    if (d != NULL) {
        if (d->texture != NULL) { SDL_DestroyTexture(d->texture); d->texture = NULL; }
        if (d->surface != NULL) { SDL_FreeSurface(d->surface);     d->surface = NULL; }
        free(d);
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }
    return __result;
}

// ---------------------------------------------------------------------------
// createEmpty(width, height, window)
//
// Allocate an ARGB surface big enough to be re-uploaded as a texture
// on first paint. The amigaos peer uses the window for friend-bitmap
// matching; SDL handles pixel format internally so the window arg is
// just ignored here.
// ---------------------------------------------------------------------------

function_result Am_Ui_Bitmap_createEmpty_0(aobject *const this, unsigned short width, unsigned short height, aobject *window)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (window != NULL) __increase_reference_count(window);

    bm_ensure_data(this);
    Am_Ui_Bitmap_data *d = bm_data(this);
    if (d != NULL && width > 0 && height > 0) {
        d->width = width;
        d->height = height;
        d->mask_enabled = false;
        // An "empty" Bitmap is the off-screen render target used by
        // Window.getOffscreen(). Create a TARGET-access texture bound
        // to the Window's renderer up-front so RenderableBitmap can
        // hand it to LayerGraphics without a deferred fixup. Without
        // this, the off-screen LayerGraphics ends up with NULL renderer
        // and silently drops every fillRect / drawString — the visible
        // window stays black.
        SDL_Renderer *r = NULL;
        if (window != NULL) {
            Am_Ui_Window_data *wd = (Am_Ui_Window_data *) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
            if (wd != NULL) r = wd->renderer;
        }
        if (r == NULL) r = am_ui_linux_primary_renderer();
        if (r != NULL) {
            d->texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width, height);
            d->bound_renderer = r;
            d->is_render_target = (d->texture != NULL);
            d->surface = NULL;
            // Newly-created TARGET textures have undefined contents —
            // any region the AmLang side doesn't repaint leaks raw GPU
            // memory through the blit. One-shot clear-to-black here
            // makes "I only painted half of it" still look sane.
            if (d->texture != NULL) {
                SDL_Texture *prev = SDL_GetRenderTarget(r);
                SDL_SetRenderTarget(r, d->texture);
                SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
                SDL_RenderClear(r);
                SDL_SetRenderTarget(r, prev);
            }
        } else {
            // No renderer yet — fall back to a CPU surface so the bitmap
            // is at least storable. LayerGraphics will fail to attach
            // until a renderer appears, but the data is preserved.
            d->surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_ARGB8888);
            d->is_render_target = false;
            d->bound_renderer = NULL;
        }
    }

    if (window != NULL) __decrease_reference_count(window);
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// createFromImage / createFromImageWithMask
//
// Pull the Image's pixel array (ARGB Uint32 row-major) and copy into a
// fresh SDL_Surface. We don't reference-count the source Image, so the
// surface owns its pixels.
//
// `withMask` toggles the texture's blend mode to BLEND when we upload
// it — that's what makes a transparent-pixel source actually punch
// through on later drawBitmap calls. The amigaos backend builds a
// separate 1-bit plane; SDL gives it for free via per-pixel alpha.
// ---------------------------------------------------------------------------

static SDL_Surface *surface_from_image(aobject *image)
{
    if (image == NULL) return NULL;
    unsigned short w = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_width].nullable_value.value.ushort_value;
    unsigned short h = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_height].nullable_value.value.ushort_value;
    int pixFmt        = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelFormat].nullable_value.value.int_value;
    if (w == 0 || h == 0) return NULL;

    // PngLoader stores pixels in A,R,G,B byte order (after
    // png_set_swap_alpha). That's the in-memory layout regardless of
    // host endianness — see am-png/src/native-c/libc/Am/Imaging/Png/
    // PngLoader.c:256. SDL's pixel format names refer to the uint32
    // layout (MSB to LSB), so we need the format whose memory bytes
    // come out as A,R,G,B: that's BGRA8888 on little-endian (uint32
    // BGRA→bytes ARGB) and ARGB8888 on big-endian (uint32 ARGB→bytes
    // ARGB). Without this match, ARGB and BGR get swapped on arm64 and
    // alpha lands in the blue channel — the splash bitmap reads back as
    // mostly transparent / black.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 wire_format = SDL_PIXELFORMAT_ARGB8888;
#else
    Uint32 wire_format = SDL_PIXELFORMAT_BGRA8888;
#endif

    SDL_Surface *s = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, wire_format);
    if (s == NULL) return NULL;

    if (pixFmt == 2) { /* ARGB */
        aobject *pixelColorsObj =
            __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;
        if (pixelColorsObj != NULL) {
            array_holder *ah = (array_holder *) &pixelColorsObj[1];
            unsigned int *src = (unsigned int *) ah->array_data;
            memcpy(s->pixels, src, (size_t) w * (size_t) h * 4);
        }
    } else {
        // Palette path not handled yet — return the blank surface so
        // the caller still has a valid (transparent) texture.
        fprintf(stderr, "[am-ui/macos-arm] surface_from_image: non-ARGB pixFmt=%d not handled\n", pixFmt);
    }

    return s;
}

function_result Am_Ui_Bitmap_createFromImage_0(aobject *const this, aobject *image, aobject *window)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (image != NULL)  __increase_reference_count(image);
    if (window != NULL) __increase_reference_count(window);

    bm_ensure_data(this);
    Am_Ui_Bitmap_data *d = bm_data(this);
    if (d != NULL && image != NULL) {
        if (d->surface != NULL) { SDL_FreeSurface(d->surface); d->surface = NULL; }
        if (d->texture != NULL) { SDL_DestroyTexture(d->texture); d->texture = NULL; }
        d->surface = surface_from_image(image);
        if (d->surface != NULL) {
            d->width = (Uint16) d->surface->w;
            d->height = (Uint16) d->surface->h;
        }
        d->mask_enabled = false;
        d->is_render_target = false;
        d->bound_renderer = NULL;
    }

    if (window != NULL) __decrease_reference_count(window);
    if (image != NULL)  __decrease_reference_count(image);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Bitmap_createFromImageWithMask_0(aobject *const this, aobject *image, aobject *window)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (image != NULL)  __increase_reference_count(image);
    if (window != NULL) __increase_reference_count(window);

    bm_ensure_data(this);
    Am_Ui_Bitmap_data *d = bm_data(this);
    if (d != NULL && image != NULL) {
        if (d->surface != NULL) { SDL_FreeSurface(d->surface); d->surface = NULL; }
        if (d->texture != NULL) { SDL_DestroyTexture(d->texture); d->texture = NULL; }
        d->surface = surface_from_image(image);
        if (d->surface != NULL) {
            d->width = (Uint16) d->surface->w;
            d->height = (Uint16) d->surface->h;
        }
        // The surface already carries per-pixel alpha; flag mask
        // so LayerGraphics turns on BLEND when it uploads.
        d->mask_enabled = true;
        d->is_render_target = false;
        d->bound_renderer = NULL;
    }

    if (window != NULL) __decrease_reference_count(window);
    if (image != NULL)  __decrease_reference_count(image);
    __decrease_reference_count(this);
    return __result;
}

#endif
