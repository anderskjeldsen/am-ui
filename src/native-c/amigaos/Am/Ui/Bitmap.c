#include <libc/core.h>
#include <Am/Ui/Bitmap.h>
#include <amigaos/Am/Ui/Bitmap.h>
#include <Am/Imaging/Image.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <graphics/scale.h>
#include <cybergraphx/cybergraphics.h>
#include <intuition/intuition.h>
#include <stdio.h>

#include <amigaos/amiga.h>
#include <amigaos/Am/Ui/Window.h>
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

// ---------------------------------------------------------------------------
// Helper: get the friend BitMap from a window's screen
// ---------------------------------------------------------------------------
static struct BitMap *get_friend_bitmap(aobject *windowObj)
{
    if (windowObj == NULL) return NULL;
    Am_Ui_Window_data *wd =
        (Am_Ui_Window_data *)windowObj->object_properties.class_object_properties.object_data.value.custom_value;
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
        (Am_Ui_Bitmap_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
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
    this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value   = width;
    this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value  = height;
    this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_pixelFormat].nullable_value.value.int_value = 1; /* BitmapPixelFormat.ZRGB */

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
    printf("[Bitmap.createFromImage] step 01: enter (this=%p, image=%p)\n", (void *)this, (void *)image);
    fflush(stdout);

    if (this != NULL) {
        printf("[Bitmap.createFromImage] step 02: increase this ref\n");
        fflush(stdout);
        __increase_reference_count(this);
    }
    if (image != NULL) {
        printf("[Bitmap.createFromImage] step 03: increase image ref\n");
        fflush(stdout);
        __increase_reference_count(image);
    }

    printf("[Bitmap.createFromImage] step 04: fetch bitmap native data\n");
    fflush(stdout);
    Am_Ui_Bitmap_data *data =
        (Am_Ui_Bitmap_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) {
        printf("[Bitmap.createFromImage] step 05: data is NULL -> throw\n");
        fflush(stdout);
        __throw_simple_exception("Bitmap not initialized",
                                 "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
        goto __exit;

    }
    printf("[Bitmap.createFromImage] step 05: data OK (%p)\n", (void *)data);
    fflush(stdout);

    if (image == NULL) {
        printf("[Bitmap.createFromImage] step 06: image is NULL -> throw\n");
        fflush(stdout);
        __throw_simple_exception("Image argument is NULL",
                                 "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
        goto __exit;
    }
    printf("[Bitmap.createFromImage] step 06: image OK\n");
    fflush(stdout);

    // One AmLang Bitmap maps to exactly one native BitMap.
    // Do not replace an existing native bitmap once created.
    if (data->bitmap != NULL) {
        printf("[Bitmap.createFromImage] step 07: bitmap already exists (%p) -> throw\n", (void *)data->bitmap);
        fflush(stdout);
        __throw_simple_exception("Bitmap already has a native BitMap",
                                 "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
        goto __exit;
    }
    printf("[Bitmap.createFromImage] step 07: bitmap slot is empty\n");
    fflush(stdout);

    {
        printf("[Bitmap.createFromImage] step 08: read image dimensions/pixfmt\n");
        fflush(stdout);
        unsigned short width  = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_width].nullable_value.value.ushort_value;
        unsigned short height = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_height].nullable_value.value.ushort_value;
        int pixFmt = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelFormat].nullable_value.value.int_value;
        printf("[Bitmap.createFromImage] step 09: image width=%u height=%u pixFmt=%d\n", width, height, pixFmt);
        fflush(stdout);

        printf("[Bitmap.createFromImage] step 10: allocate native bitmap\n");
        fflush(stdout);
        data->bitmap = alloc_truecolor_bitmap(width, height, get_friend_bitmap(window));
        if (data->bitmap == NULL) {
            printf("[Bitmap.createFromImage] step 11: alloc_truecolor_bitmap failed -> throw\n");
            fflush(stdout);
            __throw_simple_exception("AllocBitMap failed in createFromImage",
                                     "in Am_Ui_Bitmap_f_createFromImage_0", &__result);
            goto __exit;
        }
        printf("[Bitmap.createFromImage] step 11: native bitmap allocated (%p)\n", (void *)data->bitmap);
        fflush(stdout);

        printf("[Bitmap.createFromImage] step 12: init RastPort and bind BitMap\n");
        fflush(stdout);
        struct RastPort rp;
        InitRastPort(&rp);
        rp.BitMap = data->bitmap;

        if (pixFmt == 2) { /* Am.Imaging.PixelFormat.ARGB = 2 */
            printf("[Bitmap.createFromImage] step 13: ARGB path\n");
            fflush(stdout);
            aobject *pixelColorsObj =
                image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;
            printf("[Bitmap.createFromImage] step 14: pixelColors object=%p\n", (void *)pixelColorsObj);
            fflush(stdout);

            if (pixelColorsObj != NULL) {
                printf("[Bitmap.createFromImage] step 15: resolve pixel array\n");
                fflush(stdout);
                // Array layout: aobject header | array_holder | element data
                array_holder *ah = (array_holder *)&pixelColorsObj[1];
                unsigned int *pixels = (unsigned int *)(void *)&ah[1];
                printf("[Bitmap.createFromImage] step 16: WritePixelArray begin (pixels=%p, bpr=%u)\n", (void *)pixels, width * 4);
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
                printf("[Bitmap.createFromImage] step 17: WritePixelArray done\n");
                fflush(stdout);
            } else {
                printf("[Bitmap.createFromImage] step 15: pixelColors is NULL (skip WritePixelArray)\n");
                fflush(stdout);
            }
        } else {
            printf("[Bitmap.createFromImage] step 13: non-ARGB path (pixFmt=%d), leave bitmap cleared\n", pixFmt);
            fflush(stdout);
        }
        /* Indexed images: not yet implemented — bitmap remains cleared */

        printf("[Bitmap.createFromImage] step 18: write AML bitmap properties\n");
        fflush(stdout);
        this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value   = width;
        this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value  = height;
        this->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_pixelFormat].nullable_value.value.int_value = 1; /* BitmapPixelFormat.ZRGB */
        printf("[Bitmap.createFromImage] step 19: function body complete\n");
        fflush(stdout);
    }

__exit: ;
    printf("[Bitmap.createFromImage] step 20: exit (image=%p, this=%p)\n", (void *)image, (void *)this);
    fflush(stdout);
    if (image != NULL) {
        printf("[Bitmap.createFromImage] step 21: decrease image ref\n");
        fflush(stdout);
        __decrease_reference_count(image);
    }
    if (this != NULL) {
        printf("[Bitmap.createFromImage] step 22: decrease this ref\n");
        fflush(stdout);
        __decrease_reference_count(this);
    }
    printf("[Bitmap.createFromImage] step 23: return\n");
    fflush(stdout);
    return __result;
}
