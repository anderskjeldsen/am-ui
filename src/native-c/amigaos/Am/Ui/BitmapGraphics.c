#include <libc/core.h>
#include <Am/Ui/BitmapGraphics.h>
#include <amigaos/Am/Ui/BitmapGraphics.h>
#include <Am/Ui/Bitmap.h>
#include <amigaos/Am/Ui/Bitmap.h>
#include <Am/Ui/Font.h>
#include <Am/Imaging/Image.h>
#include <libc/Am/Lang/String.h>
#include <amigaos/amiga.h>

#include <exec/types.h>
#include <graphics/gfx.h>
#include <graphics/rastport.h>
#include <graphics/scale.h>
#include <graphics/text.h>
#include <cybergraphics/cybergraphics.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>

#include <libc/core_inline_functions.h>

// ---------------------------------------------------------------------------
// Helpers: read xOffset/yOffset from the Graphics base
// (same defines as ViewContextGraphics uses: Am_Ui_Graphics_P_xOffset/yOffset)
// ---------------------------------------------------------------------------

static short bg_translated_x(aobject *g, short x)
{
    short tx = g->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_xOffset].nullable_value.value.short_value;
    return tx + x;
}

static short bg_translated_y(aobject *g, short y)
{
    short ty = g->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_yOffset].nullable_value.value.short_value;
    return ty + y;
}

// Retrieve the native RastPort from this BitmapGraphics object.
static struct RastPort *get_rp(aobject *this)
{
    Am_Ui_BitmapGraphics_data *data =
        (Am_Ui_BitmapGraphics_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) return NULL;
    return &data->rastport;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

function_result Am_Ui_BitmapGraphics__native_init_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }

    Am_Ui_BitmapGraphics_data *data = AllocVec(sizeof(Am_Ui_BitmapGraphics_data), MEMF_CLEAR | MEMF_ANY);
    if (data == NULL) {
        __throw_simple_exception("AllocVec failed in Am_Ui_BitmapGraphics__native_init_0",
                                 "in Am_Ui_BitmapGraphics__native_init_0", &__result);
        goto __exit;
    }
    this->object_properties.class_object_properties.object_data.value.custom_value = data;

__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

function_result Am_Ui_BitmapGraphics__native_release_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;

    Am_Ui_BitmapGraphics_data *data =
        (Am_Ui_BitmapGraphics_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data != NULL) {
        FreeVec(data);
        this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }

__exit: ;
    return __result;
}

function_result Am_Ui_BitmapGraphics__native_mark_children_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

// ---------------------------------------------------------------------------
// attach(): wire the RastPort to the Bitmap
// Called once after construction (by RenderableBitmap or directly).
// ---------------------------------------------------------------------------

function_result Am_Ui_BitmapGraphics_attach_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (this != NULL) {
        __increase_reference_count(this);
    }

    Am_Ui_BitmapGraphics_data *data =
        (Am_Ui_BitmapGraphics_data *)this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) goto __exit;

    // bitmap is property 4 of BitmapGraphics (Graphics has 0-3)
    aobject *bitmapObj = this->object_properties.class_object_properties.properties[Am_Ui_BitmapGraphics_P_bitmap].nullable_value.value.object_value;
    if (bitmapObj == NULL) goto __exit;

    Am_Ui_Bitmap_data *bitmapData =
        (Am_Ui_Bitmap_data *)bitmapObj->object_properties.class_object_properties.object_data.value.custom_value;
    if (bitmapData == NULL || bitmapData->bitmap == NULL) goto __exit;

    if (CyberGfxBase == NULL)
        CyberGfxBase = (struct Library *)__ensure_library("cybergraphics.library", 40L);

    InitRastPort(&data->rastport);
    data->rastport.BitMap = bitmapData->bitmap;

__exit: ;
    if (this != NULL) {
        __decrease_reference_count(this);
    }
    return __result;
}

// ---------------------------------------------------------------------------
// Drawing primitives
// ---------------------------------------------------------------------------

function_result Am_Ui_BitmapGraphics_setForegroundPen_0(aobject * const this, unsigned char pen)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    struct RastPort *rp = get_rp(this);
    if (rp != NULL) SetAPen(rp, pen);
__exit: ;
    return __result;
}

function_result Am_Ui_BitmapGraphics_setBackgroundPen_0(aobject * const this, unsigned char pen)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    struct RastPort *rp = get_rp(this);
    if (rp != NULL) SetBPen(rp, pen);
__exit: ;
    return __result;
}

function_result Am_Ui_BitmapGraphics_drawLine_0(aobject * const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    struct RastPort *rp = get_rp(this);
    if (rp == NULL) goto __exit;
    Move(rp, bg_translated_x(this, x), bg_translated_y(this, y));
    Draw(rp, bg_translated_x(this, x2), bg_translated_y(this, y2));
__exit: ;
    return __result;
}

function_result Am_Ui_BitmapGraphics_fillRect_0(aobject * const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    struct RastPort *rp = get_rp(this);
    if (rp == NULL) goto __exit;
    RectFill(rp, bg_translated_x(this, x), bg_translated_y(this, y),
                 bg_translated_x(this, x2), bg_translated_y(this, y2));
__exit: ;
    return __result;
}

function_result Am_Ui_BitmapGraphics_eraseRect_0(aobject * const this, short x, short y, short x2, short y2)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    struct RastPort *rp = get_rp(this);
    if (rp == NULL) goto __exit;
    EraseRect(rp, bg_translated_x(this, x), bg_translated_y(this, y),
                  bg_translated_x(this, x2), bg_translated_y(this, y2));
__exit: ;
    return __result;
}

function_result Am_Ui_BitmapGraphics_drawString_0(aobject * const this, aobject *text, short x, short y)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (text != NULL) {
        __increase_reference_count(text);
    }
    struct RastPort *rp = get_rp(this);
    if (rp == NULL || text == NULL) goto __exit;
    {
        string_holder *sh = text->object_properties.class_object_properties.object_data.value.custom_value;
        struct TextFont *tf = rp->Font;
        short baseline = tf ? tf->tf_Baseline : 0;
        Move(rp, bg_translated_x(this, x), bg_translated_y(this, y + baseline));
        Text(rp, sh->string_value, sh->length);
    }
__exit: ;
    if (text != NULL) {
        __decrease_reference_count(text);
    }
    return __result;
}

function_result Am_Ui_BitmapGraphics_calculateStringWidth_0(aobject * const this, aobject *text)
{
    function_result __result = { .has_return_value = true };
    bool __returning = false;
    if (text != NULL) {
        __increase_reference_count(text);
    }
    struct RastPort *rp = get_rp(this);
    if (rp != NULL && text != NULL) {
        string_holder *sh = text->object_properties.class_object_properties.object_data.value.custom_value;
        __result.return_value.value.ushort_value = (unsigned short)TextLength(rp, sh->string_value, sh->length);
    }
__exit: ;
    if (text != NULL) {
        __decrease_reference_count(text);
    }
    return __result;
}

function_result Am_Ui_BitmapGraphics_getCurrentFontSize_0(aobject * const this)
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

function_result Am_Ui_BitmapGraphics_setFont_0(aobject * const this, aobject *font)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
    if (font != NULL) {
        __increase_reference_count(font);
    }
    struct RastPort *rp = get_rp(this);
    if (rp != NULL && font != NULL) {
        struct TextFont *tf = font->object_properties.class_object_properties.object_data.value.custom_value;
        if (tf != NULL) SetFont(rp, tf);
    }
__exit: ;
    if (font != NULL) {
        __decrease_reference_count(font);
    }
    return __result;
}

// ---------------------------------------------------------------------------
// drawImage: blit an Am.Imaging.Image into the bitmap via WritePixelArray
// ---------------------------------------------------------------------------

function_result Am_Ui_BitmapGraphics_drawImage_0(aobject * const this, aobject *image,
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
        int pixFmt = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelFormat].nullable_value.value.int_value;
        if (pixFmt == 2) { /* ARGB */
            aobject *pixelColorsObj =
                image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_pixelColors].nullable_value.value.object_value;
            unsigned short imgW = image->object_properties.class_object_properties.properties[Am_Imaging_Image_P_width].nullable_value.value.ushort_value;
            if (pixelColorsObj != NULL) {
                array_holder *ah = (array_holder *)&pixelColorsObj[1];
                unsigned int *pixels = (unsigned int *)(void *)&ah[1];
                WritePixelArray(pixels, 0, 0, imgW * 4,
                                rp,
                                bg_translated_x(this, x), bg_translated_y(this, y),
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

// ---------------------------------------------------------------------------
// drawBitmap: blit another Bitmap, scaling if necessary
// ---------------------------------------------------------------------------

function_result Am_Ui_BitmapGraphics_drawBitmap_0(aobject * const this, aobject *bitmap,
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
    {
        Am_Ui_Bitmap_data *bitmapData =
            (Am_Ui_Bitmap_data *)bitmap->object_properties.class_object_properties.object_data.value.custom_value;
        if (bitmapData == NULL || bitmapData->bitmap == NULL) goto __exit;

        unsigned short srcW = bitmap->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value;
        unsigned short srcH = bitmap->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value;
        short tx = bg_translated_x(this, x);
        short ty = bg_translated_y(this, y);

        if (destWidth == (short)srcW && destHeight == (short)srcH) {
            BltBitMapRastPort(bitmapData->bitmap, 0, 0, rp, tx, ty, srcW, srcH, 0xC0);
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

// ---------------------------------------------------------------------------
// Clipping / painting lifecycle — no-ops (no Layer on off-screen bitmaps)
// ---------------------------------------------------------------------------

function_result Am_Ui_BitmapGraphics_setClipRect_0(aobject * const this, aobject * const clipRect)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_BitmapGraphics_clearClipRect_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_BitmapGraphics_beginPainting_0(aobject * const this, aobject * const clipRect)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}

function_result Am_Ui_BitmapGraphics_endPainting_0(aobject * const this)
{
    function_result __result = { .has_return_value = false };
    bool __returning = false;
__exit: ;
    return __result;
}
