#ifndef native_aclass_Am_Ui_Screen_c
#define native_aclass_Am_Ui_Screen_c
#include <libc/core.h>
#include <amigaos/amiga.h>
#include <Am/Ui/Screen.h>
#include <Am/Ui/ScreenMode.h>
#include <amigaos/Am/Ui/Screen.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>
#include <Am/Lang/Int.h>

#include <exec/types.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <graphics/displayinfo.h>
#include <graphics/view.h>
#include <cybergraphx/cybergraphics.h>

#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/cybergraphics.h>

#include <libc/core_inline_functions.h>


function_result Am_Ui_Screen__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;
	__increase_reference_count(this);
	printf("TODO: implement native function Am_Ui_Screen__native_init_0\n");
__exit:
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_Screen__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	char __returning = 0;

	Am_Ui_Screen_close_0(this);

__exit:
	return __result;
};

function_result Am_Ui_Screen__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_Screen_open_0(aobject * const this, int width, int height, int depth, int displayId, aobject * title, aobject * systemPens, aobject * paletteColors)
{
	function_result __result = { .has_return_value = 0 };
	__increase_reference_count(this);
	if (title != NULL) {
		__increase_reference_count(title);
	}
	if (systemPens != NULL) {
		__increase_reference_count(systemPens);
	}
	if (paletteColors != NULL) {
		__increase_reference_count(paletteColors);
	}

	SysBase = *((struct ExecBase **)4UL);
	if (IntuitionBase == NULL) {
		IntuitionBase = (struct IntuitionBase *) __ensure_library("intuition.library", 0L);
	}

	// Extract the AmLang string into a C string pointer. NULL means
	// "no SA_Title tag" — Intuition will leave the title bar empty.
	const char * title_cstr = NULL;
	if (title != NULL) {
		string_holder * sh = (string_holder *) (title + 1);
		if (sh != NULL && sh->string_value != NULL) {
			title_cstr = sh->string_value;
		}
	}

	// SA_Pens wants a UWORD array terminated by ~0. The AmLang side
	// passes a UByte[] (one pen index per DRI role) — we widen each
	// entry and append the terminator.
	UWORD pen_array[16];
	UWORD * pens_ptr = NULL;
	if (systemPens != NULL) {
		array_holder * a = (array_holder *) &systemPens[1];
		unsigned int src_len = a->size;
		unsigned char * src = (unsigned char *) a->array_data;
		unsigned int max = src_len > 15 ? 15 : src_len;
		for (unsigned int i = 0; i < max; i++) {
			pen_array[i] = (UWORD) src[i];
		}
		pen_array[max] = (UWORD) ~0;
		pens_ptr = pen_array;
	}

	// Build SA_Colors32 table from a UInt[] of packed 0x00RRGGBB. Format
	// is: { count<<16 | startIdx, r0_32, g0_32, b0_32, r1_32, ..., 0 }.
	// 8-bit channels are widened with the 0x01010101 trick so 0xFF maps
	// to full brightness.
	ULONG * colors32 = NULL;
	unsigned int color_count = 0;
	if (paletteColors != NULL) {
		array_holder * a = (array_holder *) &paletteColors[1];
		color_count = a->size;
		if (color_count > 256) color_count = 256;
		unsigned int * src = (unsigned int *) a->array_data;
		colors32 = (ULONG *) malloc(sizeof(ULONG) * (1 + 3 * color_count + 1));
		if (colors32 != NULL) {
			colors32[0] = ((ULONG) color_count << 16) | 0;
			for (unsigned int i = 0; i < color_count; i++) {
				unsigned int packed = src[i];
				unsigned char r = (packed >> 16) & 0xFF;
				unsigned char g = (packed >> 8)  & 0xFF;
				unsigned char b =  packed        & 0xFF;
				colors32[1 + 3*i + 0] = ((ULONG) r) * 0x01010101UL;
				colors32[1 + 3*i + 1] = ((ULONG) g) * 0x01010101UL;
				colors32[1 + 3*i + 2] = ((ULONG) b) * 0x01010101UL;
			}
			colors32[1 + 3 * color_count] = 0;
		}
	}

	printf("Screen open: %dx%dx%d id=0x%lx title=%s pens=%s colors=%u\n", width, height, depth,
		(unsigned long) displayId, title_cstr != NULL ? title_cstr : "(none)",
		pens_ptr != NULL ? "custom" : "default", color_count);
	if (pens_ptr != NULL) {
		printf("  SA_Pens: ");
		for (int i = 0; pens_ptr[i] != (UWORD) ~0; i++) {
			printf("%d=%u ", i, (unsigned) pens_ptr[i]);
		}
		printf("\n");
	}

	struct TagItem screenTags[] = {
		SA_Left, 0,
		SA_Top, 0,
		SA_Width, width,
		SA_Height, height,
 		SA_Depth, depth,
		SA_DisplayID, displayId,
		SA_Type, PUBLICSCREEN,
		SA_ShowTitle, TRUE,
		SA_Title, (ULONG) title_cstr,
		SA_Pens, (ULONG) pens_ptr,
		SA_Colors32, (ULONG) colors32,
		TAG_DONE, TAG_DONE,
	};

	struct Screen * screen = OpenScreenTagList(NULL, screenTags);

	if ( screen == NULL ) {
		printf("Unable to open screen\n");
	} else {
		// On Picasso96 the bar can be drawn before SA_Colors32 reaches the
		// display, leaving the screen title with the driver's default RGB
		// (and effectively invisible against our dark BARBLOCKPEN). Toggle
		// ShowTitle to force a fresh render with the now-correct palette.
		ShowTitle(screen, FALSE);
		ShowTitle(screen, TRUE);

		struct DrawInfo * dri = GetScreenDrawInfo(screen);
		if (dri != NULL) {
			printf("  Screen DRI: version=%u numPens=%u depth=%u barHeight=%d ",
				(unsigned) dri->dri_Version, (unsigned) dri->dri_NumPens,
				(unsigned) dri->dri_Depth, (int) screen->BarHeight);
			printf("pens: ");
			for (int i = 0; i < dri->dri_NumPens; i++) {
				printf("%d=%u ", i, (unsigned) dri->dri_Pens[i]);
			}
			printf("\n");
			FreeScreenDrawInfo(screen, dri);
		} else {
			printf("  Screen DRI: NULL\n");
		}
	}

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) malloc(sizeof(Am_Ui_Screen_data));
	data->screen = screen;
	this->object_properties.class_object_properties.object_data.value.custom_value = data;

exit:
	if (colors32 != NULL) {
		// Intuition copies SA_Colors32 into the screen's colour map, so we
		// can free the staging buffer right after OpenScreenTagList.
		free(colors32);
	}
	if (title != NULL) {
		__decrease_reference_count(title);
	}
	if (systemPens != NULL) {
		__decrease_reference_count(systemPens);
	}
	if (paletteColors != NULL) {
		__decrease_reference_count(paletteColors);
	}
	__decrease_reference_count(this);
	return __result;
};

function_result Am_Ui_Screen_close_0(aobject * const this)
{
	function_result __result = { .has_return_value = 0 };
	__increase_reference_count(this);

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;

	if ( data != NULL && data->screen != NULL ) {
		CloseScreen(data->screen);
	}

	free(this->object_properties.class_object_properties.object_data.value.custom_value);
	this->object_properties.class_object_properties.object_data.value.custom_value = NULL;

exit:
	__decrease_reference_count(this);
	return __result;
};

// Pick a display id matching the requested mode. Tries CGX first (so RTG
// resolutions like 800x600 actually resolve), then falls back to
// graphics.library's BestModeID. Returns INVALID_ID (-1 as int) on failure.
function_result Am_Ui_Screen_bestModeId_0(int width, int height, int depth)
{
	function_result __result = { .has_return_value = true };
	ULONG modeId = INVALID_ID;

	if (CyberGfxBase != NULL) {
		struct TagItem cgxTags[] = {
			CYBRBIDTG_NominalWidth,  width,
			CYBRBIDTG_NominalHeight, height,
			CYBRBIDTG_Depth,         depth,
			TAG_DONE,                0,
		};
		modeId = BestCModeIDTagList(cgxTags);
	}

	if (modeId == INVALID_ID) {
		struct TagItem gfxTags[] = {
			BIDTAG_NominalWidth,  width,
			BIDTAG_NominalHeight, height,
			BIDTAG_Depth,         depth,
			TAG_DONE,             0,
		};
		modeId = BestModeIDA(gfxTags);
	}

	__result.return_value.value.int_value = (int) modeId;
	return __result;
}

// Geometry accessors — read directly off the struct Screen so callers
// can size a borderless window that sits below the title bar.
function_result Am_Ui_Screen_getBarHeight_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	__increase_reference_count(this);
	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	unsigned short v = 0;
	if (data != NULL && data->screen != NULL) {
		// BarHeight is the index of the bottom row of the bar; +1 makes
		// it the actual pixel height so window y = barHeight clears it.
		v = (unsigned short) (data->screen->BarHeight + 1);
	}
	__result.return_value.value.ushort_value = v;
	__decrease_reference_count(this);
	return __result;
}

function_result Am_Ui_Screen_getWidth_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	__increase_reference_count(this);
	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	unsigned short v = 0;
	if (data != NULL && data->screen != NULL) {
		v = (unsigned short) data->screen->Width;
	}
	__result.return_value.value.ushort_value = v;
	__decrease_reference_count(this);
	return __result;
}

function_result Am_Ui_Screen_getHeight_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	__increase_reference_count(this);
	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	unsigned short v = 0;
	if (data != NULL && data->screen != NULL) {
		v = (unsigned short) data->screen->Height;
	}
	__result.return_value.value.ushort_value = v;
	__decrease_reference_count(this);
	return __result;
}

// Populate a caller-provided ScreenMode from the current default public
// screen (Workbench). Uses LockPubScreen so the screen can't go away
// while we read its viewport / bitmap. Sets all four ScreenMode props
// in place; on failure leaves them at whatever the caller initialised.
function_result Am_Ui_Screen_fillDefaultScreenMode_0(aobject * mode)
{
	function_result __result = { .has_return_value = false };
	if (mode != NULL) {
		__increase_reference_count(mode);
	}

	if (IntuitionBase == NULL) {
		IntuitionBase = (struct IntuitionBase *) __ensure_library("intuition.library", 0L);
	}

	struct Screen * pub = LockPubScreen(NULL);
	if (pub == NULL) {
		printf("fillDefaultScreenMode: LockPubScreen returned NULL\n");
		goto __exit;
	}

	ULONG modeId = GetVPModeID(&pub->ViewPort);
	int width  = pub->Width;
	int height = pub->Height;
	int depth  = GetBitMapAttr(pub->RastPort.BitMap, BMA_DEPTH);

	UnlockPubScreen(NULL, pub);

	mode->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_width].nullable_value.value.int_value     = width;
	mode->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_height].nullable_value.value.int_value    = height;
	mode->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_depth].nullable_value.value.int_value     = depth;
	mode->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_displayId].nullable_value.value.int_value = (int) modeId;

	printf("fillDefaultScreenMode: %dx%dx%d id=0x%lx\n", width, height, depth, (unsigned long) modeId);

__exit: ;
	if (mode != NULL) {
		__decrease_reference_count(mode);
	}
	return __result;
}

// SetRGB32 wants 32-bit fixed-point channels; multiplying the 8-bit value
// by 0x01010101 replicates it through all four bytes so 0x00 maps to
// 0x00000000 and 0xFF maps to 0xFFFFFFFF.
function_result Am_Ui_Screen_setColor_0(aobject * const this, int index, unsigned char r, unsigned char g, unsigned char b)
{
	function_result __result = { .has_return_value = false };
	__increase_reference_count(this);

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	if (data == NULL || data->screen == NULL) {
		goto __exit;
	}

	printf("Screen setColor: pen=%d rgb=%02x%02x%02x\n", index, (unsigned) r, (unsigned) g, (unsigned) b);
	SetRGB32(&data->screen->ViewPort, (ULONG) index,
		((ULONG) r) * 0x01010101UL,
		((ULONG) g) * 0x01010101UL,
		((ULONG) b) * 0x01010101UL);

__exit: ;
	__decrease_reference_count(this);
	return __result;
}

#endif
