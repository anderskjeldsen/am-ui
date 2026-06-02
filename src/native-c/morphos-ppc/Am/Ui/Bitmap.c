#include <libc/core.h>
#include <Am/Ui/Bitmap.h>
#include <morphos-ppc/Am/Ui/Bitmap.h>
#include <Am/Imaging/Image.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
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

// MorphOS port of Bitmap. Same API surface as the AmigaOS version —
// AllocBitMap with BMF_MINPLANES on a friend bitmap gives us a chunky
// true-colour backing matching the display.

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
    this->object_properties.class_object_properties.object_data.value.custom_value = data;

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
        (Am_Ui_Bitmap_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
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
        this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
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

static struct BitMap *get_friend_bitmap(aobject *windowObj)
{
    if (windowObj == NULL) return NULL;
    Am_Ui_Window_data *wd =
        (Am_Ui_Window_data *)windowObj->object_properties.class_object_properties.object_data.value.custom_value;
    if (wd == NULL) return NULL;
    struct Screen *scr = wd->window ? wd->window->WScreen : wd->locked_screen;
    return scr ? scr->RastPort.BitMap : NULL;
}

static struct BitMap *alloc_truecolor_bitmap(ULONG width, ULONG height, struct BitMap *friendBitMap)
{
    return AllocBitMap(width, height, 24, BMF_CLEAR | BMF_MINPLANES, friendBitMap);
}

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
        (Am_Ui_Bitmap_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) goto __exit;

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

    this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value   = width;
    this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value  = height;
    this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_pixelFormat].nullable_value.value.int_value = 1; /* BitmapPixelFormat.ZRGB */

__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

function_result Am_Ui_Bitmap_createFromImage_0(aobject * const this,
                                               aobject * image,
                                               aobject * window)
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
        (Am_Ui_Bitmap_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) {
        __throw_simple_exception("Bitmap not initialized",
                                 "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
        goto __exit;
    }
    if (image == NULL) {
        __throw_simple_exception("Image argument is NULL",
                                 "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
        goto __exit;
    }
    if (data->bitmap != NULL) {
        __throw_simple_exception("Bitmap already has a native BitMap",
                                 "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
        goto __exit;
    }

    {
        unsigned short width  = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_width].nullable_value.value.ushort_value;
        unsigned short height = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_height].nullable_value.value.ushort_value;
        int pixFmt = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelFormat].nullable_value.value.int_value;

        data->bitmap = alloc_truecolor_bitmap(width, height, get_friend_bitmap(window));
        if (data->bitmap == NULL) {
            __throw_simple_exception("AllocBitMap failed in createFromImage",
                                     "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
            goto __exit;
        }

        struct RastPort rp;
        InitRastPort(&rp);
        rp.BitMap = data->bitmap;

        if (pixFmt == 2) { /* Am.Imaging.PixelFormat.ARGB = 2 */
            aobject *pixelColorsObj =
                image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;
            if (pixelColorsObj != NULL) {
                array_holder *ah = (array_holder *)&pixelColorsObj[1];
                unsigned int *pixels = (unsigned int *)(void *)&ah[1];
                WritePixelArray(pixels, 0, 0, width * 4,
                                &rp, 0, 0,
                                width, height,
                                RECTFMT_ARGB);
            }
        }
        /* Indexed images: not yet implemented — bitmap remains cleared */

        this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value   = width;
        this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value  = height;
        this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_pixelFormat].nullable_value.value.int_value = 1; /* BitmapPixelFormat.ZRGB */
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

// MorphOS port of createFromImageWithMask. See the AmigaOS version for
// the design rationale; the only platform diff is which header
// `BltMaskBitMapRastPort` comes from (graphics on both, same proto).
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
        (Am_Ui_Bitmap_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
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
        UWORD width  = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_width].nullable_value.value.ushort_value;
        UWORD height = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_height].nullable_value.value.ushort_value;
        WORD  pixFmt = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelFormat].nullable_value.value.int_value;

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
                image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;
            if (pixelColorsObj != NULL) {
                array_holder *ah = (array_holder *)&pixelColorsObj[1];
                unsigned int *pixels = (unsigned int *)(void *)&ah[1];

                WritePixelArray(pixels, 0, 0, width * 4, &rp, 0, 0,
                                width, height, RECTFMT_ARGB);

                data->mask = AllocBitMap(width, height, 1, BMF_CLEAR, NULL);
                if (data->mask != NULL) {
                    UBYTE *maskPlane = (UBYTE *)data->mask->Planes[0];
                    UWORD bytesPerRow = data->mask->BytesPerRow;
                    UWORD y, x;
                    /* Threshold 64 — see the AmigaOS version for why
                     * the obvious 50% threshold ate downscaled icons. */
                    for (y = 0; y < height; y++) {
                        UBYTE *row = maskPlane + (ULONG)y * bytesPerRow;
                        unsigned int *srcRow = pixels + (ULONG)y * width;
                        for (x = 0; x < width; x++) {
                            unsigned int argb = srcRow[x];
                            UBYTE alpha = (UBYTE)((argb >> 24) & 0xFF);
                            if (alpha > 64) {
                                row[x >> 3] |= (UBYTE)(0x80 >> (x & 7));
                            }
                        }
                    }
                }
                /* Mask alloc failure is non-fatal — caller gets opaque
                 * fallback, same as createFromImage. */
            }
        }
        /* Indexed source: mask stays NULL, drawBitmap uses vanilla
         * blit. */

        this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value   = width;
        this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value  = height;
        this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_pixelFormat].nullable_value.value.int_value = 1; /* ZRGB */
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
