#include <libc/core.h>
#include <Am/Ui/ViewContextGraphics.h>
#include <morphos-ppc/Am/Ui/ViewContextGraphics.h>
#include <Am/Ui/Graphics.h>
#include <Am/Ui/ViewContext.h>
#include <Am/Ui/ClipRect.h>
#include <Am/Lang/UByte.h>
#include <Am/Lang/Short.h>

#include <morphos-ppc/morphos.h>
#include <morphos-ppc/Am/Ui/Window.h>
#include <morphos-ppc/Am/Ui/Screen.h>
#include <morphos-ppc/Am/Ui/Bitmap.h>
#include <Am/Ui/Bitmap.h>
#include <libc/Am/Lang/String.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <graphics/gfx.h>
#include <graphics/scale.h>
#include <cybergraphx/cybergraphics.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>


#include <libc/core_inline_functions.h>

// Forward declaration for AmigaOS layers function (not in standard headers)
struct Region *InstallClipRegion(struct Layer *layer, struct Region *region);

short translated_x(aobject *g, short x) {
	short tx = __unwrap(g)->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_xOffset].nullable_value.value.short_value;
	return tx + x;
}

short translated_y(aobject *g, short y) {
	short ty = __unwrap(g)->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_yOffset].nullable_value.value.short_value;
	return ty + y;
}

function_result Am_Ui_ViewContextGraphics__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
};

function_result Am_Ui_ViewContextGraphics__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_ViewContextGraphics_setForegroundPen_0(aobject * const this, unsigned char pen)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	SetAPen(rp, pen);

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_setBackgroundPen_0(aobject * const this, unsigned char pen)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	SetBPen(rp, pen);
__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}

	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_drawLine_0(aobject * const this, short x, short y, short x2, short y2)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);
	short tx2 = translated_x(this, x2);
	short ty2 = translated_y(this, y2);

//	printf("Draw line from %d, %d\n", tx, ty);
//	printf("x %d => %d\n", x, tx);
//	printf("x2 %d => %d\n", x2, tx2);
//	printf("y %d => %d\n", y, ty);
//	printf("y2 %d => %d\n", y2, ty2);

    Move(rp, tx, ty);
    Draw(rp, tx2, ty2);

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}

	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_eraseRect_0(aobject * const this, short x, short y, short x2, short y2)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);
	short tx2 = translated_x(this, x2);
	short ty2 = translated_y(this, y2);

	EraseRect(rp, tx, ty, tx2, ty2);

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}

	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_fillRect_0(aobject * const this, short x, short y, short x2, short y2)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);
	short tx2 = translated_x(this, x2);
	short ty2 = translated_y(this, y2);

	RectFill(rp, tx, ty, tx2, ty2);

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}

	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_drawString_0(aobject * const this, aobject * text, short x, short y)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (text != NULL) {
		__increase_reference_count(text);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;
	string_holder *sh = __unwrap(text)->object_properties.class_object_properties.object_data.value.custom_value;
    struct TextFont *textFont = rp->Font;

	short tx = translated_x(this, x);
	short ty = translated_y(this, y + textFont->tf_Baseline);

    Move(rp, tx, ty);
	Text(rp, sh->string_value, sh->length);
__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}

	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (text != NULL) {
		__decrease_reference_count(text);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_calculateStringWidth_0(aobject * const this, aobject * text)
{
	function_result __result = { .has_return_value = true };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (text != NULL) {
		__increase_reference_count(text);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;
	string_holder *sh = __unwrap(text)->object_properties.class_object_properties.object_data.value.custom_value;

	ULONG width = TextLength(rp, sh->string_value, sh->length);

	__result.return_value.value.ushort_value = width;

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (text != NULL) {
		__decrease_reference_count(text);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_getCurrentFontSize_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

    struct TextFont *textFont = rp->Font; // this->object_properties.class_object_properties.object_data.value.custom_value;
	if (textFont != NULL) {
		__result.return_value.value.uchar_value = textFont->tf_YSize;		
	} else {
		__result.return_value.value.uchar_value = 0;
	}

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}

	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_setFont_0(aobject * const this, aobject * font)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (font != NULL) {
		__increase_reference_count(font);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

    struct TextFont *textFont = __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;

	SetFont(rp, textFont);
__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}

	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (font != NULL) {
		__decrease_reference_count(font);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_drawImage_0(aobject * const this, aobject * image, short x, short y, short width, short height)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (image != NULL) {
		__increase_reference_count(image);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	struct RastPort *rp = window_data->window->RPort;

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);

	// TODO: Implement actual image drawing using Am.Imaging.Image data
	// For now, just draw a placeholder rectangle
	SetAPen(rp, 2); // Use pen 2 for image placeholder
	RectFill(rp, tx, ty, tx + width - 1, ty + height - 1);

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (image != NULL) {
		__decrease_reference_count(image);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_setClipRect_0(aobject * const this, struct Am_Ui_ClipRect * clipRect)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	// clipRect is a struct value-type (not ARC-tracked); no ref count adjustments needed.

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;

	// Add the current clipRect to the master region
	if (clipRect != NULL) {
		InstallClipRegion(window_data->window->WLayer, NULL);
		ClearRegion(window_data->clip_region);

		// ClipRect is a struct (value type) — read fields directly.
		struct Rectangle rect;
		rect.MinX = clipRect->x;
		rect.MinY = clipRect->y;
		rect.MaxX = clipRect->x + clipRect->width - 1;
		rect.MaxY = clipRect->y + clipRect->height - 1;

		OrRectRegion(window_data->clip_region, &rect);
		InstallClipRegion(window_data->window->WLayer, window_data->clip_region);
//		BeginRefresh(window_data->window);
	}

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_clearClipRect_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;

	InstallClipRegion(window_data->window->WLayer, NULL);

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_ViewContextGraphics_beginPainting_0(aobject * const this, struct Am_Ui_ClipRect * clipRect) {
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	// clipRect is a struct value-type (not ARC-tracked); no ref count adjustments needed.

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;

	// Add the current clipRect to the master region
	if (clipRect != NULL) {
		InstallClipRegion(window_data->window->WLayer, NULL);
		ClearRegion(window_data->clip_region);

		// ClipRect is a struct (value type) — read fields directly.
		struct Rectangle rect;
		rect.MinX = clipRect->x;
		rect.MinY = clipRect->y;
		rect.MaxX = clipRect->x + clipRect->width - 1;
		rect.MaxY = clipRect->y + clipRect->height - 1;

		OrRectRegion(window_data->clip_region, &rect);
		InstallClipRegion(window_data->window->WLayer, window_data->clip_region);
//		BeginRefresh(window_data->window);
	}

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
}

function_result Am_Ui_ViewContextGraphics_endPainting_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;

	InstallClipRegion(window_data->window->WLayer, NULL);
	// Clear the region
//	ClearRegion(window_data->clip_region);

//	EndRefresh(window_data->window, FALSE);

__exit: ;
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

// ---------------------------------------------------------------------------
// drawBitmap: blit a Bitmap to the window RastPort, scaling if necessary.
//
// When source size == dest size: BltBitMapRastPort (fast, no copy).
// When scaling is needed: allocate a temporary bitmap, BitMapScale into it,
//   then BltBitMapRastPort the result, then free the temp bitmap.
// ---------------------------------------------------------------------------

function_result Am_Ui_ViewContextGraphics_drawBitmap_0(aobject * const this,
                                                         aobject * bitmap,
                                                         short x, short y,
                                                         short destWidth, short destHeight)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (bitmap != NULL) {
		__increase_reference_count(bitmap);
	}

	if (bitmap == NULL) goto __exit;
	if (destWidth <= 0 || destHeight <= 0) goto __exit;

	{
		Am_Ui_Bitmap_data * const bitmapData =
			(Am_Ui_Bitmap_data *)__unwrap(bitmap)->object_properties.class_object_properties.object_data.value.custom_value;
		if (bitmapData == NULL || bitmapData->bitmap == NULL) goto __exit;

		aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
		Am_Ui_Window_data * const window_data =
			(Am_Ui_Window_data * const)__unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
		struct RastPort * const rp = window_data->window->RPort;

		short tx = translated_x(this, x);
		short ty = translated_y(this, y);

		unsigned short srcW = __unwrap(bitmap)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_width].nullable_value.value.ushort_value;
		unsigned short srcH = __unwrap(bitmap)->object_properties.class_object_properties.properties[Am_Ui_Bitmap_P_height].nullable_value.value.ushort_value;

		if (destWidth == (short)srcW && destHeight == (short)srcH) {
			/* 1:1 blit — no scaling needed.
			 * Mask-aware: when the source carries a 1-bit transparency
			 * mask (built by createFromImageWithMask),
			 * BltMaskBitMapRastPort with minterm 0xE0 means
			 * D = (S AND M) OR (D AND NOT M) — destination pixels under
			 * cleared mask bits survive. Falls back to vanilla 0xC0
			 * when no mask is configured. */
			if (bitmapData->mask != NULL) {
				BltMaskBitMapRastPort(bitmapData->bitmap, 0, 0,
				                      rp, tx, ty,
				                      srcW, srcH, 0xE0,
				                      (APTR)bitmapData->mask->Planes[0]);
			} else {
				BltBitMapRastPort(bitmapData->bitmap, 0, 0,
				                  rp, tx, ty,
				                  srcW, srcH, 0xC0);
			}
		} else {
			/* Scale into a temporary bitmap, then blit to screen.
			 * Mask is NOT propagated through the scale path — scaled
			 * masked bitmaps render fully opaque. The current consumer
			 * (16x16 icon strip) never scales, so this is acceptable
			 * for now. */
			ULONG depth = GetBitMapAttr(bitmapData->bitmap, BMA_DEPTH);
			struct BitMap *scaledBitmap =
				AllocBitMap(destWidth, destHeight, depth,
				            BMF_CLEAR | BMF_MINPLANES, bitmapData->bitmap);
			if (scaledBitmap != NULL) {
				struct BitScaleArgs bsa;
				memset(&bsa, 0, sizeof(bsa));
				bsa.bsa_SrcBitMap    = bitmapData->bitmap;
				bsa.bsa_DestBitMap   = scaledBitmap;
				bsa.bsa_SrcX         = 0;
				bsa.bsa_SrcY         = 0;
				bsa.bsa_SrcWidth     = srcW;
				bsa.bsa_SrcHeight    = srcH;
				bsa.bsa_XSrcFactor   = srcW;
				bsa.bsa_YSrcFactor   = srcH;
				bsa.bsa_DestX        = 0;
				bsa.bsa_DestY        = 0;
				bsa.bsa_DestWidth    = destWidth;
				bsa.bsa_DestHeight   = destHeight;
				bsa.bsa_XDestFactor  = destWidth;
				bsa.bsa_YDestFactor  = destHeight;
				bsa.bsa_Flags        = 0;
				BitMapScale(&bsa);
				BltBitMapRastPort(scaledBitmap, 0, 0,
				                  rp, tx, ty,
				                  destWidth, destHeight, 0xC0);
				FreeBitMap(scaledBitmap);
			} else {
				/* Fallback: blit unscaled if we ran out of memory */
				BltBitMapRastPort(bitmapData->bitmap, 0, 0,
				                  rp, tx, ty,
				                  srcW, srcH, 0xC0);
			}
		}

		if (window != NULL) {
			__decrease_reference_count(window);
		}
	}

__exit: ;
	if (bitmap != NULL) {
		__decrease_reference_count(bitmap);
	}
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};




function_result Am_Ui_ViewContextGraphics_scrollRect_0(aobject * const this,
                                                        short x, short y,
                                                        unsigned short w, unsigned short h,
                                                        short dx, short dy,
                                                        unsigned char fillPen)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

	aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
	if (window == NULL || w == 0 || h == 0) goto __exit;
	if (dx == 0 && dy == 0) goto __exit;
	Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
	if (window_data == NULL || window_data->window == NULL || window_data->window->RPort == NULL) goto __exit;
	struct RastPort *rp = window_data->window->RPort;

	short tx = translated_x(this, x);
	short ty = translated_y(this, y);
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
	if (window != NULL) {
		__decrease_reference_count(window);
	}
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

