#include <libc/core.h>
#include <Am/Ui/Bitmap.h>
#include <amigaos/Am/Ui/Bitmap.h>
#include <Am/Imaging/Image.h>

#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfx.h>
#include <graphics/scale.h>
#include <cybergraphics/cybergraphics.h>

#include <amigaos/amiga.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>

#include <libc/core_inline_functions.h>

// CyberGfxBase must be defined in exactly one translation unit.
// It is declared as 'extern' by proto/cybergraphics.h everywhere else.
struct Library *CyberGfxBase = NULL;

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
// Helper: allocate a CyberGraphX true-colour BitMap and wire up a RastPort
// ---------------------------------------------------------------------------
static struct BitMap *alloc_truecolor_bitmap(ULONG width, ULONG height)
{
    if (CyberGfxBase == NULL)
        CyberGfxBase = (struct Library *)__ensure_library("cybergraphics.library", 40L);

    // Request a CGX-capable 24-bit bitmap; BMF_MINPLANES lets the system
    // choose the real depth.  Passing NULL as the friend bitmap means we get
    // a system-default true-colour format.
    struct BitMap *bm = AllocBitMap(width, height, 24,
                                    BMF_CLEAR | BMF_MINPLANES, NULL);
    return bm;
}

// ---------------------------------------------------------------------------
// createEmpty(width, height)
// ---------------------------------------------------------------------------

function_result Am_Ui_Bitmap_createEmpty_0(aobject * const this,
                                            unsigned short width,
                                            unsigned short height)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }

    Am_Ui_Bitmap_data *data =
        (Am_Ui_Bitmap_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) goto __exit;

    // Free any previously allocated bitmap
    if (data->bitmap != NULL) {
        FreeBitMap(data->bitmap);
        data->bitmap = NULL;
    }

    data->bitmap = alloc_truecolor_bitmap(width, height);
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
                                               aobject * image)
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
    if (data == NULL || image == NULL) goto __exit;

    // Free any previously allocated bitmap
    if (data->bitmap != NULL) {
        FreeBitMap(data->bitmap);
        data->bitmap = NULL;
    }

    {
        unsigned short width  = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_width].nullable_value.value.ushort_value;
        unsigned short height = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_height].nullable_value.value.ushort_value;
        int pixFmt = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelFormat].nullable_value.value.int_value;

        data->bitmap = alloc_truecolor_bitmap(width, height);
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
                // Array layout: aobject header | array_holder | element data
                array_holder *ah = (array_holder *)&pixelColorsObj[1];
                unsigned int *pixels = (unsigned int *)(void *)&ah[1];

                WritePixelArray(
                    pixels,              /* source buffer */
                    0, 0,               /* srcX, srcY */
                    width * 4,          /* bytes per row in source */
                    &rp,                /* destination RastPort */
                    0, 0,               /* destX, destY */
                    width, height,      /* width, height */
                    RECTFMT_ARGB        /* pixel format: 0xAARRGGBB */
                );
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
