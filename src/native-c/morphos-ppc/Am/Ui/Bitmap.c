#include <libc/core.h>
#include <Am/Ui/Bitmap.h>
#include <morphos-ppc/Am/Ui/Bitmap.h>
#include <Am/Imaging/Image.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <graphics/scale.h>
#include <cybergraphx/cybergraphics.h>
#include <intuition/intuition.h>
#include <stdio.h>

#include <morphos-ppc/morphos.h>
#include <morphos-ppc/Am/Ui/Window.h>
#include <Am/Ui/Window.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>

#include <libc/core_inline_functions.h>

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

function_result Am_Ui_Bitmap__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }

    Am_Ui_Bitmap_data *data = AllocVec(sizeof(Am_Ui_Bitmap_data), MEMF_CLEAR | MEMF_ANY);
    if (data == NULL) {
        __throw_simple_exception("AllocVec failed in Am_Ui_Bitmap__native_init_0",
                                 "in Am_Ui_Bitmap__native_init_0", &__result);
        goto __exit;
    }
    __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = data;

__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

function_result Am_Ui_Bitmap__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;

    Am_Ui_Bitmap_data *data =
        (Am_Ui_Bitmap_data *)__unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
    if (data != NULL) {
        if (data->bitmap != NULL) {
            FreeBitMap(data->bitmap);
            data->bitmap = NULL;
        }
        if (data->mask != NULL) {
            FreeBitMap(data->mask);
            data->mask = NULL;
        }
        FreeVec(data);
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }

__exit: ;
    return __result;
}

function_result Am_Ui_Bitmap__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

// ---------------------------------------------------------------------------
// Helper: get the friend BitMap from a window's screen
// ---------------------------------------------------------------------------
static struct BitMap *get_friend_bitmap(aobject *windowObj)
{
    if (windowObj == NULL) return NULL;
    Am_Ui_Window_data *wd =
        (Am_Ui_Window_data *)__unwrap(windowObj)->object_properties.class_object_properties.object_data.value.custom_value;
    if (wd == NULL) return NULL;
    // Prefer the window's own screen; fall back to locked_screen.
    struct Screen *scr = wd->window ? wd->window->WScreen : wd->locked_screen;
    return scr ? scr->RastPort.BitMap : NULL;
}

// ---------------------------------------------------------------------------
// Helper: allocate a CyberGraphX true-colour BitMap
// ---------------------------------------------------------------------------
static struct BitMap *alloc_truecolor_bitmap(ULONG width, ULONG height, struct BitMap *friendBitMap)
{
    // BMF_MINPLANES + a CGX friend bitmap ensures we get a chunky true-colour
    // bitmap in the same pixel format as the display hardware.
    return AllocBitMap(width, height, 24, BMF_CLEAR | BMF_MINPLANES, friendBitMap);
}

// ---------------------------------------------------------------------------
// createEmpty(width, height)
// ---------------------------------------------------------------------------

function_result Am_Ui_Bitmap_createEmpty_0(aobject * const this,
                                            unsigned short width,
                                            unsigned short height,
                                            aobject *window)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }

    Am_Ui_Bitmap_data *data =
        (Am_Ui_Bitmap_data *)__unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) goto __exit;

    // One AmLang Bitmap maps to exactly one native BitMap.
    // Do not replace an existing native bitmap once created.
    if (data->bitmap != NULL) {
        __throw_simple_exception("Bitmap already has a native BitMap",
                                 "in Am_Ui_Bitmap_f_createEmpty_0", &__result);
        goto __exit;
    }

    data->bitmap = alloc_truecolor_bitmap(width, height, get_friend_bitmap(window));
    if (data->bitmap == NULL) {
        __throw_simple_exception("AllocBitMap failed in createEmpty",
                                 "in Am_Ui_Bitmap_f_createEmpty_0", &__result);
        goto __exit;
    }

    // Store dimensions on the AML-side fields so getWidth()/getHeight() work
    __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value   = width;
    __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value  = height;
    __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_pixelFormat].nullable_value.value.int_value = 1; /* BitmapPixelFormat.ZRGB */

__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

// ---------------------------------------------------------------------------
// createFromImage(image)
// Reads Am.Imaging.Image pixel data (ARGB UInt[] array) into the bitmap via
// CyberGraphX WritePixelArray with RECTFMT_ARGB (0xAARRGGBB).
// ---------------------------------------------------------------------------

function_result Am_Ui_Bitmap_createFromImage_0(aobject * const this,
                                               aobject * image,
                                               aobject * window)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    // printf("[Bitmap.createFromImage] step 01: enter (this=%p, image=%p)\n", (void *)this, (void *)image);
    fflush(stdout);

    if (this != NULL) {
        // printf("[Bitmap.createFromImage] step 02: increase this ref\n");
        fflush(stdout);
        __increase_reference_count(this);
    }
    if (image != NULL) {
        // printf("[Bitmap.createFromImage] step 03: increase image ref\n");
        fflush(stdout);
        __increase_reference_count(image);
    }

    // printf("[Bitmap.createFromImage] step 04: fetch bitmap native data\n");
    fflush(stdout);
    Am_Ui_Bitmap_data *data =
        (Am_Ui_Bitmap_data *)__unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) {
        // printf("[Bitmap.createFromImage] step 05: data is NULL -> throw\n");
        fflush(stdout);
        __throw_simple_exception("Bitmap not initialized",
                                 "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
        goto __exit;

    }
    // printf("[Bitmap.createFromImage] step 05: data OK (%p)\n", (void *)data);
    fflush(stdout);

    if (image == NULL) {
        // printf("[Bitmap.createFromImage] step 06: image is NULL -> throw\n");
        fflush(stdout);
        __throw_simple_exception("Image argument is NULL",
                                 "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
        goto __exit;
    }
    // printf("[Bitmap.createFromImage] step 06: image OK\n");
    fflush(stdout);

    // One AmLang Bitmap maps to exactly one native BitMap.
    // Do not replace an existing native bitmap once created.
    if (data->bitmap != NULL) {
        // printf("[Bitmap.createFromImage] step 07: bitmap already exists (%p) -> throw\n", (void *)data->bitmap);
        fflush(stdout);
        __throw_simple_exception("Bitmap already has a native BitMap",
                                 "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
        goto __exit;
    }
    // printf("[Bitmap.createFromImage] step 07: bitmap slot is empty\n");
    fflush(stdout);

    {
        // printf("[Bitmap.createFromImage] step 08: read image dimensions/pixfmt\n");
        fflush(stdout);
        unsigned short width  = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_width].nullable_value.value.ushort_value;
        unsigned short height = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_height].nullable_value.value.ushort_value;
        int pixFmt = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelFormat].nullable_value.value.int_value;
        // printf("[Bitmap.createFromImage] step 09: image width=%u height=%u pixFmt=%d\n", width, height, pixFmt);
        fflush(stdout);

        // printf("[Bitmap.createFromImage] step 10: allocate native bitmap\n");
        fflush(stdout);
        data->bitmap = alloc_truecolor_bitmap(width, height, get_friend_bitmap(window));
        if (data->bitmap == NULL) {
            // printf("[Bitmap.createFromImage] step 11: alloc_truecolor_bitmap failed -> throw\n");
            fflush(stdout);
            __throw_simple_exception("AllocBitMap failed in createFromImage",
                                     "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
            goto __exit;
        }
        // printf("[Bitmap.createFromImage] step 11: native bitmap allocated (%p)\n", (void *)data->bitmap);
        fflush(stdout);

        // printf("[Bitmap.createFromImage] step 12: init RastPort and bind BitMap\n");
        fflush(stdout);
        struct RastPort rp;
        InitRastPort(&rp);
        rp.BitMap = data->bitmap;

        if (pixFmt == 2) { /* Am.Imaging.PixelFormat.ARGB = 2 */
            // printf("[Bitmap.createFromImage] step 13: ARGB path\n");
            fflush(stdout);
            aobject *pixelColorsObj =
                __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;
            // printf("[Bitmap.createFromImage] step 14: pixelColors object=%p\n", (void *)pixelColorsObj);
            fflush(stdout);

            if (pixelColorsObj != NULL) {
                // printf("[Bitmap.createFromImage] step 15: resolve pixel array\n");
                fflush(stdout);
                // Array layout: aobject header | array_holder | element data
                array_holder *ah = (array_holder *)&pixelColorsObj[1];
                unsigned int *pixels = (unsigned int *)(void *)&ah[1];
                // printf("[Bitmap.createFromImage] step 16: WritePixelArray begin (pixels=%p, bpr=%u)\n", (void *)pixels, width * 4);
                fflush(stdout);

                WritePixelArray(
                    pixels,              /* source buffer */
                    0, 0,               /* srcX, srcY */
                    width * 4,          /* bytes per row in source */
                    &rp,                /* destination RastPort */
                    0, 0,               /* destX, destY */
                    width, height,      /* width, height */
                    RECTFMT_ARGB        /* pixel format: 0xAARRGGBB */
                );
                // printf("[Bitmap.createFromImage] step 17: WritePixelArray done\n");
                fflush(stdout);
            } else {
                // printf("[Bitmap.createFromImage] step 15: pixelColors is NULL (skip WritePixelArray)\n");
                fflush(stdout);
            }
        } else {
            // printf("[Bitmap.createFromImage] step 13: non-ARGB path (pixFmt=%d), leave bitmap cleared\n", pixFmt);
            fflush(stdout);
        }
        /* Indexed images: not yet implemented — bitmap remains cleared */

        // printf("[Bitmap.createFromImage] step 18: write AML bitmap properties\n");
        fflush(stdout);
        __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value   = width;
        __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value  = height;
        __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_pixelFormat].nullable_value.value.int_value = 1; /* BitmapPixelFormat.ZRGB */
        // printf("[Bitmap.createFromImage] step 19: function body complete\n");
        fflush(stdout);
    }

__exit: ;
    // printf("[Bitmap.createFromImage] step 20: exit (image=%p, this=%p)\n", (void *)image, (void *)this);
    fflush(stdout);
    if (image != NULL) {
        // printf("[Bitmap.createFromImage] step 21: decrease image ref\n");
        fflush(stdout);
        __decrease_reference_count(image);
    }
    if (this != NULL) {
        // printf("[Bitmap.createFromImage] step 22: decrease this ref\n");
        fflush(stdout);
        __decrease_reference_count(this);
    }
    // printf("[Bitmap.createFromImage] step 23: return\n");
    fflush(stdout);
    return __result;
}

// ---------------------------------------------------------------------------
// createFromImageWithMask(image, window)
//
// Same colour-data path as createFromImage_0 plus a 1-bit BitMap whose
// plane 0 acts as the bltMask for BltMaskBitMapRastPort. A source
// pixel with alpha >= 128 sets the corresponding mask bit to 1 (this
// pixel is opaque, copy through); alpha < 128 leaves the bit 0 (let
// the destination show through). Indexed-source images don't carry an
// alpha channel — for those we just leave the mask out (caller still
// gets an opaque blit, matching the no-mask path).
// ---------------------------------------------------------------------------

function_result Am_Ui_Bitmap_createFromImageWithMask_0(aobject * const this,
                                                       aobject *image,
                                                       aobject *window)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }
    if (image != NULL) {
        __increase_reference_count(image);
    }

    Am_Ui_Bitmap_data *data =
        (Am_Ui_Bitmap_data *)__unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) goto __exit;
    if (image == NULL) {
        __throw_simple_exception("image is NULL in createFromImageWithMask",
                                 "in Am_Ui_Bitmap_f_createFromImageWithMask_0", &__result);
        goto __exit;
    }
    if (data->bitmap != NULL) {
        __throw_simple_exception("Bitmap already has a native BitMap; replacement not allowed",
                                 "in Am_Ui_Bitmap_f_createFromImageWithMask_0", &__result);
        goto __exit;
    }

    {
        UWORD width  = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_width].nullable_value.value.ushort_value;
        UWORD height = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_height].nullable_value.value.ushort_value;
        WORD  pixFmt = __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelFormat].nullable_value.value.int_value;

        data->bitmap = alloc_truecolor_bitmap(width, height, get_friend_bitmap(window));
        if (data->bitmap == NULL) {
            __throw_simple_exception("AllocBitMap failed in createFromImageWithMask",
                                     "in Am_Ui_Bitmap_f_createFromImageWithMask_0", &__result);
            goto __exit;
        }

        struct RastPort rp;
        InitRastPort(&rp);
        rp.BitMap = data->bitmap;

        if (pixFmt == 2) { /* ARGB */
            aobject *pixelColorsObj =
                __unwrap(image)->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;
            if (pixelColorsObj != NULL) {
                array_holder *ah = (array_holder *)&pixelColorsObj[1];
                unsigned int *pixels = (unsigned int *)(void *)&ah[1];

                /* Colour path: write ARGB pixels to the screen-format
                 * bitmap exactly like createFromImage. Alpha bits are
                 * dropped by the WritePixelArray format conversion —
                 * they're encoded into the separate mask below. */
                WritePixelArray(pixels, 0, 0, width * 4, &rp, 0, 0,
                                width, height, RECTFMT_ARGB);

                /* Allocate the 1-bit mask BitMap. Plane 0 is what
                 * BltMaskBitMapRastPort wants as `bltMask`. BMF_CLEAR
                 * starts every bit at 0; we'll flip bits on for opaque
                 * source pixels.
                 *
                 * No friend bitmap on purpose — the mask is just raw
                 * bits, no need for a screen-format match, and the
                 * blitter handles either chip or fast memory. */
                data->mask = AllocBitMap(width, height, 1, BMF_CLEAR, NULL);
                if (data->mask == NULL) {
                    /* Mask alloc is best-effort: if it fails we end up
                     * with a fully-opaque bitmap (the same behaviour
                     * as the no-mask factory). No exception — better
                     * to render the icon as an opaque square than to
                     * lose the whole panel. */
                    // printf("[Bitmap.createFromImageWithMask] mask AllocBitMap failed; opaque fallback\n");
                    fflush(stdout);
                } else {
                    UBYTE *maskPlane = (UBYTE *)data->mask->Planes[0];
                    UWORD bytesPerRow = data->mask->BytesPerRow;
                    UWORD y, x;
                    /* Threshold: alpha > MASK_ALPHA_THRESHOLD -> opaque.
                     * 64 (~25%) keeps anti-aliased edges of box-filtered
                     * pixel art visible. A higher value (128 = 50%) made
                     * a 16->8 downsampled "+" disappear because the
                     * arms landed at alpha 127. Anything dimmer than
                     * 25% is still cut as transparent so isolated
                     * stray pixels don't show up as faint dots. */
                    for (y = 0; y < height; y++) {
                        UBYTE *row = maskPlane + (ULONG)y * bytesPerRow;
                        unsigned int *srcRow = pixels + (ULONG)y * width;
                        for (x = 0; x < width; x++) {
                            unsigned int argb = srcRow[x];
                            UBYTE alpha = (UBYTE)((argb >> 24) & 0xFF);
                            if (alpha > 64) {
                                /* MSB-first within each byte — bit 7
                                 * of byte 0 covers pixel 0 of the row,
                                 * matching the AmigaOS blitter's bit
                                 * order on a bitplane. */
                                row[x >> 3] |= (UBYTE)(0x80 >> (x & 7));
                            }
                        }
                    }
                }
            }
        }
        /* Indexed images: leave mask NULL; drawBitmap falls back to
         * the vanilla copy and the bitmap is fully opaque. Adding
         * alpha-aware decoding for indexed PNGs would mean reading
         * the tRNS chunk through to here — out of scope for now. */

        __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value   = width;
        __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value  = height;
        __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_pixelFormat].nullable_value.value.int_value = 1; /* ZRGB */
    }

__exit: ;
    if (image != NULL) {
        __decrease_reference_count(image);
    }
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}
