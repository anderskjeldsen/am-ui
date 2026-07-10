#ifndef native_aclass_Am_Ui_Screen_c
#define native_aclass_Am_Ui_Screen_c
#include <libc/core.h>
#include <morphos-ppc/morphos.h>
#include <Am/Ui/Screen.h>
#include <Am/Ui/ScreenMode.h>
#include <morphos-ppc/Am/Ui/Screen.h>
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

	// Passing SA_Pens=NULL is NOT the same as omitting the tag — Intuition
	// interprets a NULL pens pointer as "an empty array", which on V40+
	// degrades the DrawInfo mapping to only use pens 0 and 1 (you can
	// see this in the DRI dump after open: every role maps to 0 or 1).
	// That makes BARBLOCKPEN resolve to pen 1 = black on most palettes,
	// which is why the title bar showed up black. Use TAG_IGNORE on
	// optional tags whose values are NULL so Intuition treats them as
	// "tag not present" and uses its standard defaults.
	struct TagItem screenTags[] = {
		SA_Left, 0,
		SA_Top, 0,
		SA_Width, width,
		SA_Height, height,
 		SA_Depth, depth,
		SA_DisplayID, displayId,
		SA_Type, PUBLICSCREEN,
		SA_ShowTitle, TRUE,
		// SA_SysFont = 1 picks the user's "Workbench screen" font from
		// Prefs (which is also what Workbench itself uses). Without this
		// Intuition falls back to Topaz 8 and the bar ends up shorter
		// than the real Workbench bar — chrome painted by Magic Menu /
		// other patches that follow the user's font will then overflow
		// the BarHeight we report to callers.
		SA_SysFont, 1,
		title_cstr  != NULL ? SA_Title     : TAG_IGNORE, (ULONG) title_cstr,
		pens_ptr    != NULL ? SA_Pens      : TAG_IGNORE, (ULONG) pens_ptr,
		colors32    != NULL ? SA_Colors32  : TAG_IGNORE, (ULONG) colors32,
		TAG_DONE, TAG_DONE,
	};

	struct Screen * screen = OpenScreenTagList(NULL, screenTags);

	if ( screen == NULL ) {
		printf("Unable to open screen\n");
	} else {
		// MorphOS defensive: force the palette into the ColorMap with
		// LoadRGB32 even though SA_Colors32 was passed at open time.
		// On some RTG drivers (CGX/Picasso) SA_Colors32 is honoured at
		// open but the first paint can happen before the colormap
		// commit reaches the display, leaving every pen at its driver
		// default — visually that's a screen with our chrome's
		// background pen rendered as black/grey because pens 4-15
		// haven't been written yet. LoadRGB32 here is a no-op when
		// the open-time table already landed.
		if (colors32 != NULL) {
			LoadRGB32(&screen->ViewPort, colors32);
			printf("Screen open: LoadRGB32 applied %u colors\n", color_count);
		}

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

		// Log Workbench's BarHeight so we can see whether our screen's
		// bar font / chrome height matches.
		struct Screen * wb = LockPubScreen("Workbench");
		if (wb != NULL) {
			printf("  WB BarHeight=%d (ours=%d)\n",
				(int) wb->BarHeight, (int) screen->BarHeight);
			UnlockPubScreen(NULL, wb);
		}
	}

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) malloc(sizeof(Am_Ui_Screen_data));
	data->screen = screen;
	__unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = data;

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

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;

	if ( data != NULL && data->screen != NULL ) {
		CloseScreen(data->screen);
	}

	free(__unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value);
	__unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = NULL;

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
	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
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
	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
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
	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
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

	__unwrap(mode)->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_width].nullable_value.value.int_value     = width;
	__unwrap(mode)->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_height].nullable_value.value.int_value    = height;
	__unwrap(mode)->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_depth].nullable_value.value.int_value     = depth;
	__unwrap(mode)->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_displayId].nullable_value.value.int_value = (int) modeId;

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

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
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

// Read pens 0..count-1 from the host pub screen into `out` as packed
// 0x00RRGGBB. Returns silently on any failure so AmLang gets a buffer of
// zeros it can detect and fall back from.
function_result Am_Ui_Screen_fillHostPaletteColors_0(aobject * out, int count)
{
	function_result __result = { .has_return_value = false };
	if (out != NULL) {
		__increase_reference_count(out);
	}

	if (out == NULL || count <= 0) goto __exit;
	if (count > 32) count = 32;

	array_holder * ah = (array_holder *) &out[1];
	unsigned int * dst = (unsigned int *) ah->array_data;
	int cap = (int) ah->size;
	if (count > cap) count = cap;
	if (count <= 0) goto __exit;

	SysBase = *((struct ExecBase **)4UL);
	if (IntuitionBase == NULL) {
		IntuitionBase = (struct IntuitionBase *) __ensure_library("intuition.library", 0L);
	}

	struct Screen * pub = LockPubScreen((STRPTR) "Workbench");
	if (pub == NULL) {
		pub = LockPubScreen(NULL);
	}
	if (pub == NULL) {
		printf("Screen fillHostPaletteColors: LockPubScreen returned NULL\n");
		goto __exit;
	}

	ULONG channels[32 * 3];
	GetRGB32(pub->ViewPort.ColorMap, 0, count, channels);

	// Diagnostic: dump the host screen's DrawInfo so we can compare its
	// pen-role mapping (SA_Pens) against the one our screen ends up with.
	// If Workbench's DRI maps roles into the same 0/1 register range,
	// the host itself is on a degraded palette mapping. If WB's DRI uses
	// pens 2/3 too, then OUR screen needs the same SA_Pens to look like
	// it.
	struct DrawInfo * pubDri = GetScreenDrawInfo(pub);
	if (pubDri != NULL) {
		printf("Screen fillHostPaletteColors: host DRI version=%u numPens=%u depth=%u ",
			(unsigned) pubDri->dri_Version, (unsigned) pubDri->dri_NumPens,
			(unsigned) pubDri->dri_Depth);
		printf("pens:");
		int d = 0;
		while (d < pubDri->dri_NumPens) {
			printf(" %d=%u", d, (unsigned) pubDri->dri_Pens[d]);
			d = d + 1;
		}
		printf("\n");
		FreeScreenDrawInfo(pub, pubDri);
	} else {
		printf("Screen fillHostPaletteColors: host DRI = NULL\n");
	}

	UnlockPubScreen(NULL, pub);

	int i = 0;
	while (i < count) {
		// GetRGB32 returns full 32-bit fixed-point channels (top byte
		// = 8-bit representation). Repack as 0x00RRGGBB for AmLang.
		unsigned int r = (channels[i * 3 + 0] >> 24) & 0xFF;
		unsigned int g = (channels[i * 3 + 1] >> 24) & 0xFF;
		unsigned int b = (channels[i * 3 + 2] >> 24) & 0xFF;
		dst[i] = (r << 16) | (g << 8) | b;
		printf("  host pen %d: r=%02x g=%02x b=%02x\n", i, r, g, b);
		i = i + 1;
	}
	printf("Screen fillHostPaletteColors: read %d host pens\n", count);

__exit: ;
	if (out != NULL) {
		__decrease_reference_count(out);
	}
	return __result;
}

// Read pens 0..count-1 of the host pub screen's DrawInfo (dri_Pens) into
// `out` as UByte pen indices. Pair with fillHostPaletteColors to give a
// new screen the same role → register mapping the user has on Workbench
// — otherwise Intuition can hand a TrueColor screen a defensive mapping
// that routes all roles into pens 0 and 1, which makes chrome look very
// wrong (e.g. BARBLOCKPEN → pen 1 = black title bar).
function_result Am_Ui_Screen_fillHostDrawInfoPens_0(aobject * out, int count)
{
	function_result __result = { .has_return_value = false };
	if (out != NULL) {
		__increase_reference_count(out);
	}

	if (out == NULL || count <= 0) goto __exit;
	if (count > 16) count = 16;

	array_holder * ah = (array_holder *) &out[1];
	unsigned char * dst = (unsigned char *) ah->array_data;
	int cap = (int) ah->size;
	if (count > cap) count = cap;
	if (count <= 0) goto __exit;

	SysBase = *((struct ExecBase **)4UL);
	if (IntuitionBase == NULL) {
		IntuitionBase = (struct IntuitionBase *) __ensure_library("intuition.library", 0L);
	}

	struct Screen * pub = LockPubScreen((STRPTR) "Workbench");
	if (pub == NULL) {
		pub = LockPubScreen(NULL);
	}
	if (pub == NULL) {
		printf("Screen fillHostDrawInfoPens: LockPubScreen returned NULL\n");
		goto __exit;
	}

	struct DrawInfo * dri = GetScreenDrawInfo(pub);
	if (dri == NULL) {
		printf("Screen fillHostDrawInfoPens: GetScreenDrawInfo returned NULL\n");
		UnlockPubScreen(NULL, pub);
		goto __exit;
	}

	int actual = (int) dri->dri_NumPens;
	if (count > actual) count = actual;

	int i = 0;
	while (i < count) {
		// dri_Pens entries are UWORDs but pen indices fit in a byte
		// on any normal palette. Clamp at 255 just in case.
		UWORD pen = dri->dri_Pens[i];
		if (pen > 255) {
			pen = 255;
		}
		dst[i] = (unsigned char) pen;
		i = i + 1;
	}

	FreeScreenDrawInfo(pub, dri);
	UnlockPubScreen(NULL, pub);
	printf("Screen fillHostDrawInfoPens: copied %d host DRI pens\n", count);

__exit: ;
	if (out != NULL) {
		__decrease_reference_count(out);
	}
	return __result;
}

function_result Am_Ui_Screen_copyHostPens_0(aobject * const this, int count)
{
	function_result __result = { .has_return_value = false };
	__increase_reference_count(this);

	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
	if (data == NULL || data->screen == NULL || count <= 0) {
		printf("Screen copyHostPens: bailing (data=%p screen=%p count=%d)\n",
			(void *)data, data ? (void *)data->screen : NULL, count);
		goto __exit;
	}
	if (count > 32) {
		count = 32;
	}

	if (IntuitionBase == NULL) {
		IntuitionBase = (struct IntuitionBase *) __ensure_library("intuition.library", 0L);
	}

	// Lock the Workbench by name first — when our own screen is opened
	// with SA_Type=PUBLICSCREEN it joins the public-screen list, and
	// LockPubScreen(NULL) on some configurations returns our own
	// screen instead of Workbench. Falling back to NULL covers
	// systems where the default pub screen has a different name.
	struct Screen * pub = LockPubScreen((STRPTR) "Workbench");
	if (pub == NULL) {
		pub = LockPubScreen(NULL);
	}
	if (pub == NULL) {
		printf("Screen copyHostPens: LockPubScreen returned NULL\n");
		goto __exit;
	}

	if (pub == data->screen) {
		// We've ended up locking ourselves — nothing to copy. Log and
		// bail rather than no-op the SetRGB32 calls below.
		printf("Screen copyHostPens: pub screen IS our own screen (%p) — skipping\n", (void *)pub);
		UnlockPubScreen(NULL, pub);
		goto __exit;
	}

	printf("Screen copyHostPens: pub=%p our=%p depth=%d count=%d\n",
		(void *)pub, (void *)data->screen,
		(int)GetBitMapAttr(pub->RastPort.BitMap, BMA_DEPTH), count);

	// GetRGB32 emits three ULONG channel values per pen; 32 pens = 96
	// ULONGs = 384 bytes on the stack.
	ULONG channels[32 * 3];
	GetRGB32(pub->ViewPort.ColorMap, 0, count, channels);
	UnlockPubScreen(NULL, pub);

	int i = 0;
	while (i < count) {
		printf("  pen %d: r=%08lx g=%08lx b=%08lx\n",
			i,
			(unsigned long) channels[i * 3 + 0],
			(unsigned long) channels[i * 3 + 1],
			(unsigned long) channels[i * 3 + 2]);
		SetRGB32(&data->screen->ViewPort, (ULONG) i,
			channels[i * 3 + 0],
			channels[i * 3 + 1],
			channels[i * 3 + 2]);
		i = i + 1;
	}

	// On chunky / RTG screens (Picasso96, CGX HiColor, TrueColor) the
	// title bar pixels are stored as baked RGB, not pen indices — so
	// the SetRGB32 calls above don't repaint anything already drawn.
	// Force Intuition to re-render the bar so it picks up the new
	// pen values for the title text and depth gadget.
	ShowTitle(data->screen, FALSE);
	ShowTitle(data->screen, TRUE);

__exit: ;
	__decrease_reference_count(this);
	return __result;
}

#endif
