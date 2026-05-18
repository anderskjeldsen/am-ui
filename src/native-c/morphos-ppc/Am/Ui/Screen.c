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
};


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

	const char * title_cstr = NULL;
	if (title != NULL) {
		string_holder * sh = (string_holder *) (title + 1);
		if (sh != NULL && sh->string_value != NULL) {
			title_cstr = sh->string_value;
		}
	}

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

function_result Am_Ui_Screen_getBarHeight_0(aobject * const this)
{
	function_result __result = { .has_return_value = true };
	__increase_reference_count(this);
	Am_Ui_Screen_data * const data = (Am_Ui_Screen_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
	unsigned short v = 0;
	if (data != NULL && data->screen != NULL) {
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

// On MorphOS graphics.library's BestModeIDA covers RTG resolutions too —
// CGX is unified with graphics, so we skip the CYBRBIDTG_* tag path that
// AmigaOS uses (those constants aren't exposed in the MorphOS headers).
function_result Am_Ui_Screen_bestModeId_0(int width, int height, int depth)
{
	function_result __result = { .has_return_value = true };

	struct TagItem gfxTags[] = {
		BIDTAG_NominalWidth,  width,
		BIDTAG_NominalHeight, height,
		BIDTAG_Depth,         depth,
		TAG_DONE,             0,
	};
	ULONG modeId = BestModeIDA(gfxTags);

	__result.return_value.value.int_value = (int) modeId;
	return __result;
}

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
