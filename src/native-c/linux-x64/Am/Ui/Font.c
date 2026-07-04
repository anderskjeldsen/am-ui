#ifndef native_aclass_Am_Ui_Font_c
#define native_aclass_Am_Ui_Font_c

#include <libc/core.h>
#include <Am/Ui/Font.h>
#include <linux-x64/Am/Ui/Font.h>

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <libc/core_inline_functions.h>

#ifdef AM_UI_LINUX_SDL_TTF
#  include <SDL2/SDL_ttf.h>
#endif

// Linux Am.Ui.Font — SDL_ttf wrapper.
//
// Two builds:
//   - AM_UI_LINUX_SDL_TTF defined: TTF_OpenFont + cached per-Font
//     data. Real font rendering, used by drawString in LayerGraphics.
//   - undefined: no-op stubs. The IDE still runs; drawString just
//     paints nothing and calculateStringWidth returns 0. Useful when
//     the host doesn't have libsdl2-ttf installed.
//
// Font name resolution: if the AmLang `name` looks like a path, we
// pass it through to TTF_OpenFont as-is. Otherwise we fall back to
// am_ui_linux_font_fallback_path() which probes a handful of
// well-known monospace TTFs from common distros.

static Am_Ui_Font_data *fn_data(aobject *const this)
{
    return (Am_Ui_Font_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
}

static void fn_ensure_data(aobject *const this)
{
    if (fn_data(this) != NULL) return;
    Am_Ui_Font_data *d = (Am_Ui_Font_data *) calloc(1, sizeof(Am_Ui_Font_data));
    if (d != NULL) {
        this->object_properties.class_object_properties.object_data.value.custom_value = d;
    }
}

// ---------------------------------------------------------------------------
// Fallback font lookup
// ---------------------------------------------------------------------------

static int file_exists(const char *path)
{
    if (path == NULL) return 0;
    struct stat st;
    return stat(path, &st) == 0 ? 1 : 0;
}

// Lookup the host font for a logical AmLang font name. AmLang's
// "topaz" / "helvetica" / "courier" come from the AmigaOS world and
// don't exist on macOS/Linux disk — map them to the closest local
// face. Anything else (including null) falls through to the generic
// monospace fallback list.
//
// macOS hits are listed first because that's where we're running for
// now; the Linux paths still get checked so the same backend lights
// up on a real Linux host without changes.
static const char *fallback_for_logical(const char *logical)
{
    static const char *generic_mono[] = {
        // macOS — SF Mono is Apple's modern coding font (same one Xcode
        // and Terminal default to). Ships with every recent macOS and
        // reads much cleaner at editor sizes than Menlo / Monaco.
        "/System/Library/Fonts/SFNSMono.ttf",
        "/System/Library/Fonts/Menlo.ttc",
        "/System/Library/Fonts/Monaco.ttf",
        "/System/Library/Fonts/Courier.ttc",
        "/System/Library/Fonts/Supplemental/Andale Mono.ttf",
        "/System/Library/Fonts/Supplemental/Courier New.ttf",
        // Linux distros.
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
        "/usr/share/fonts/TTF/DejaVuSansMono.ttf",
        "/usr/share/fonts/dejavu/DejaVuSansMono.ttf",
        "/usr/share/fonts/liberation/LiberationMono-Regular.ttf",
        "/usr/share/fonts/inconsolata/Inconsolata-Regular.ttf",
        NULL
    };
    static const char *generic_sans[] = {
        "/System/Library/Fonts/Helvetica.ttc",
        "/System/Library/Fonts/HelveticaNeue.ttc",
        "/System/Library/Fonts/Geneva.ttf",
        "/System/Library/Fonts/Supplemental/Arial.ttf",
        // Linux
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        NULL
    };

    const char *const *list = generic_mono;
    if (logical != NULL) {
        // Case-insensitive prefix match on the family. Anything that
        // doesn't list as a sans family lands on the mono list because
        // that's what the IDE expects for code views (and topaz reads
        // mono on a real Amiga too).
        if (strncasecmp(logical, "helvetica", 9) == 0 ||
            strncasecmp(logical, "geneva", 6)    == 0 ||
            strncasecmp(logical, "arial", 5)     == 0 ||
            strncasecmp(logical, "sans", 4)      == 0) {
            list = generic_sans;
        }
    }
    for (int i = 0; list[i] != NULL; i++) {
        if (file_exists(list[i])) return list[i];
    }
    // Final mono fallback in case the caller asked for sans but no
    // sans face exists. Better an ugly mono render than nothing.
    if (list == generic_sans) {
        for (int i = 0; generic_mono[i] != NULL; i++) {
            if (file_exists(generic_mono[i])) return generic_mono[i];
        }
    }
    return NULL;
}

const char *am_ui_linux_font_fallback_path(void)
{
    static const char *cached = NULL;
    if (cached != NULL) return cached;
    cached = fallback_for_logical(NULL);
    return cached;
}

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

function_result Am_Ui_Font__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    fn_ensure_data(this);

#ifdef AM_UI_LINUX_SDL_TTF
    Am_Ui_Font_data *d = fn_data(this);
    if (d == NULL) goto __exit;

    if (!TTF_WasInit() && TTF_Init() != 0) {
        fprintf(stderr, "[am-ui/macos-arm] TTF_Init failed: %s\n", TTF_GetError());
        goto __exit;
    }

    // The AmLang Font constructor stores name + size as ordinary
    // properties before _native_init_0 fires — mirror the amigaos
    // peer and pull them straight off `this`.
    aobject *name      = this->object_properties.class_object_properties.properties[Am_Ui_Font_P_name].nullable_value.value.object_value;
    unsigned char size = this->object_properties.class_object_properties.properties[Am_Ui_Font_P_size].nullable_value.value.uchar_value;

    const char *requested = NULL;
    if (name != NULL) {
        string_holder *sh = (string_holder *) (name + 1);
        if (sh != NULL) requested = sh->string_value;
    }
    // Absolute path → pass through. Otherwise map the logical AmLang
    // family (topaz, helvetica, …) onto a host TTF.
    const char *path = (requested != NULL && requested[0] == '/') ? requested : fallback_for_logical(requested);
    if (path == NULL) {
        fprintf(stderr, "[am-ui/macos-arm] no host font matched '%s'; drawString will paint nothing\n", requested ? requested : "(null)");
        goto __exit;
    }
    // Scale up tiny Amiga-pixel sizes for modern displays. AmIde asks
    // for Topaz/8, which is microscopic on Retina; map small sizes
    // through a fudge factor and enforce a sensible floor. Anything ≥
    // 14 the caller asked for is honoured verbatim.
    int px = (size > 0) ? (int) size : 13;
    if (px < 14) {
        px = px * 2 - 2;  // 8 → 14, 10 → 18, 12 → 22
    }
    if (px < 13) px = 13;

    TTF_Font *f = TTF_OpenFont(path, px);
    if (f == NULL) {
        fprintf(stderr, "[am-ui/macos-arm] TTF_OpenFont(%s, %d) for '%s' failed: %s\n", path, px, requested ? requested : "(null)", TTF_GetError());
        goto __exit;
    }
    int real_height = TTF_FontHeight(f);
    int line_skip   = TTF_FontLineSkip(f);
    fprintf(stderr, "[am-ui/macos-arm] font '%s' size %d -> %s (TTF_FontHeight=%d ascent=%d lineSkip=%d)\n",
        requested ? requested : "(null)", px, path, real_height, TTF_FontAscent(f), line_skip);
    d->ttf_font   = f;
    d->height     = real_height;
    d->ascent     = TTF_FontAscent(f);
    d->line_skip  = line_skip;

    // Write the natural-line-height value onto the AmLang Font's
    // `size` field. We use TTF_FontLineSkip rather than
    // TTF_FontHeight: line-skip is the recommended spacing between
    // baselines and matches the visual "row height" of the font
    // (typically a few px taller than FontHeight, which is only the
    // max glyph extent). CliView's cursor box is `lineHeight() =
    // font.size + 1` and was visibly shorter than the rendered text
    // when sized from FontHeight alone — using FontLineSkip makes
    // the cursor cover the full line. Label / TextEditor inherit
    // the same change which is the visual expectation anyway.
    int chosen = line_skip > real_height ? line_skip : real_height;
    int stored = chosen > 255 ? 255 : chosen;
    nullable_value scaled = { .flags = PRIMITIVE_UBYTE, .value = { .uchar_value = (unsigned char) stored } };
    this->object_properties.class_object_properties.properties[Am_Ui_Font_P_size].nullable_value = scaled;

__exit:
#endif

    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Font__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}

function_result Am_Ui_Font__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    Am_Ui_Font_data *d = fn_data(this);
    if (d != NULL) {
#ifdef AM_UI_LINUX_SDL_TTF
        if (d->ttf_font != NULL) TTF_CloseFont((TTF_Font *) d->ttf_font);
#endif
        free(d);
        this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }
    return __result;
}

// ---------------------------------------------------------------------------
// open / load
//
// The AmLang Font surface has open(name, size) — but the exact signature
// depends on the .aml class. We mirror the amigaos peer's
// Am_Ui_Font_open_0 which takes (this, nameStr, size:Int).
// ---------------------------------------------------------------------------

function_result Am_Ui_Font_open_0(aobject *const this, aobject *name, int size)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (name != NULL) __increase_reference_count(name);

#ifdef AM_UI_LINUX_SDL_TTF
    fn_ensure_data(this);
    Am_Ui_Font_data *d = fn_data(this);
    if (d == NULL) goto __exit;

    if (!TTF_WasInit() && TTF_Init() != 0) {
        fprintf(stderr, "[am-ui/linux] TTF_Init failed: %s\n", TTF_GetError());
        goto __exit;
    }

    const char *requested = NULL;
    if (name != NULL) {
        string_holder *sh = (string_holder *) (name + 1);
        if (sh != NULL) requested = sh->string_value;
    }
    // If the requested name looks like an absolute path, try it
    // verbatim; otherwise fall back to the system probe.
    const char *path = (requested != NULL && requested[0] == '/') ? requested : am_ui_linux_font_fallback_path();
    if (path == NULL) {
        fprintf(stderr, "[am-ui/linux] no system TTF available; drawString will paint nothing\n");
        goto __exit;
    }
    int px = (size > 0) ? size : 13;

    TTF_Font *f = TTF_OpenFont(path, px);
    if (f == NULL) {
        fprintf(stderr, "[am-ui/linux] TTF_OpenFont(%s, %d) failed: %s\n", path, px, TTF_GetError());
        goto __exit;
    }
    d->ttf_font   = f;
    d->height     = TTF_FontHeight(f);
    d->ascent     = TTF_FontAscent(f);
    d->line_skip  = TTF_FontLineSkip(f);

__exit:
#else
    (void) this; (void) name; (void) size;
#endif

    if (name != NULL) __decrease_reference_count(name);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Font_close_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_Font_data *d = fn_data(this);
    if (d != NULL) {
#ifdef AM_UI_LINUX_SDL_TTF
        if (d->ttf_font != NULL) { TTF_CloseFont((TTF_Font *) d->ttf_font); d->ttf_font = NULL; }
#endif
        d->height = 0;
        d->ascent = 0;
    }
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Font_calculateStringWidth_0(aobject *const this, aobject *text)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    if (text != NULL) __increase_reference_count(text);

    int width = 0;
#ifdef AM_UI_LINUX_SDL_TTF
    Am_Ui_Font_data *d = fn_data(this);
    if (d != NULL && d->ttf_font != NULL && text != NULL) {
        string_holder *sh = (string_holder *) (text + 1);
        if (sh != NULL && sh->string_value != NULL) {
            int h = 0;
            TTF_SizeUTF8((TTF_Font *) d->ttf_font, sh->string_value, &width, &h);
        }
    }
#endif
    __result.return_value.value.ushort_value = (unsigned short) width;

    if (text != NULL) __decrease_reference_count(text);
    __decrease_reference_count(this);
    return __result;
}

#endif
