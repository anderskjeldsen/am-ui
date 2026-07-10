#include <libc/core.h>
#include <Am/Ui/Font.h>
#include <morphos-ppc/Am/Ui/Font.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>
#include <Am/Lang/UByte.h>
#include <Am/Collections/HashMap.h>
#include <libc/Am/Lang/String.h>

#include <morphos-ppc/morphos.h>
#include <libc/Am/Lang/String.h>

#include <exec/types.h>
#include <graphics/gfx.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/diskfont.h>
#include <proto/dos.h>

#include <libc/core_inline_functions.h>

/* diskfont.library isn't auto-opened by m68k-amigaos-gcc's C
 * startup (only exec / dos / intuition / graphics get that). We
 * route through __ensure_library so the open is tracked in the
 * shared lib_node list and CloseLibrary'd by the existing
 * __release_libraries destructor at process exit. proto/diskfont.h
 * declares DiskfontBase as extern; one definition lives here. */
struct Library *DiskfontBase = NULL;

static void am_font_ensure_diskfont(void) {
    if (DiskfontBase != NULL) {
        return;
    }
    DiskfontBase = (struct Library *)__ensure_library((unsigned char *)"diskfont.library", 36L);
}

function_result Am_Ui_Font__native_init_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
	if (this != NULL) {
		__increase_reference_count(this);
	}

    aobject *name = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Font_P_name].nullable_value.value.object_value;
    UBYTE size = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Font_P_size].nullable_value.value.uchar_value;

    string_holder *sh = __unwrap(name)->object_properties.class_object_properties.object_data.value.custom_value;


    struct TextAttr textAttr = { sh->string_value, size, 0, 0 };

    am_font_ensure_diskfont();
    if (DiskfontBase == NULL) {
        __throw_simple_exception("diskfont.library not available", "in Am_Ui_Font__native_init_0", &__result);
        goto __exit;
    }

    /* diskfont.library's OpenDiskFont is a superset of graphics's
     * OpenFont: it checks the in-memory font list first (so ROM
     * fonts like topaz/8 + anything previously loaded come back
     * instantly), then falls back to parsing the .font descriptor
     * under FONTS:<name>/ and loading the matching bitmap from
     * disk. Using it unconditionally means a disk-resident font
     * like helvetica.font opens the same way topaz does. */
    struct TextFont *font = OpenDiskFont(&textAttr);

    if (font == NULL) {
        __throw_simple_exception("Failed to open font", "in Am_Ui_Font__native_init_0", &__result);
		goto __exit;
    }

    __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = font;

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	return __result;
};

function_result Am_Ui_Font__native_release_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;

    struct TextFont *font = __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
    if (font != NULL) {
        CloseFont(font);
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }


__exit: ;
	return __result;
};

function_result Am_Ui_Font__native_mark_children_0(aobject * const this)
{
	function_result __result = { .has_return_value = false };
	bool __returning = false;
__exit: ;
	return __result;
}

function_result Am_Ui_Font_calculateStringWidth_0(aobject * const this, aobject * text)
{
	/* Previously this function was a no-op (no .return_value assignment,
	 * has_return_value = false), so every caller received garbage memory
	 * as a UShort. That broke every layout-time width measurement —
	 * TabView.minWidth, TextEditor.colForPixelX, and the FolderTreeView
	 * label positioning all silently received unbounded values.
	 *
	 * Proper measurement on Amiga requires a RastPort with the font set
	 * (TextLength() takes a RastPort, not a TextFont). We keep a static
	 * scratch RastPort initialised on first use; subsequent calls just
	 * re-SetFont if the font changed. */
	function_result __result = { .has_return_value = true };
	bool __returning = false;
	__result.return_value.value.ushort_value = 0;

	if (this != NULL) {
		__increase_reference_count(this);
	}
	if (text != NULL) {
		__increase_reference_count(text);
	}

	if (this == NULL || text == NULL) {
		goto __exit;
	}

	struct TextFont *font = __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
	string_holder *sh = __unwrap(text)->object_properties.class_object_properties.object_data.value.custom_value;

	if (font == NULL || sh == NULL || sh->length == 0) {
		goto __exit;
	}

	static struct RastPort scratch_rp;
	static int scratch_inited = 0;
	static struct TextFont *scratch_font = NULL;

	if (!scratch_inited) {
		InitRastPort(&scratch_rp);
		scratch_inited = 1;
	}
	if (scratch_font != font) {
		SetFont(&scratch_rp, font);
		scratch_font = font;
	}

	__result.return_value.value.ushort_value =
		(unsigned short) TextLength(&scratch_rp, sh->string_value, sh->length);

__exit: ;
	if (this != NULL) {
		__decrease_reference_count(this);
	}
	if (text != NULL) {
		__decrease_reference_count(text);
	}
	return __result;
};
