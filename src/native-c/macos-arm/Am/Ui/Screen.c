#ifndef native_aclass_Am_Ui_Screen_c
#define native_aclass_Am_Ui_Screen_c

#include <libc/core.h>
#include <Am/Ui/Screen.h>
#include <Am/Ui/ScreenMode.h>
#include <macos-arm/Am/Ui/Screen.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/String.h>
#include <Am/Lang/Int.h>

#include <SDL2/SDL.h>
#include <libc/core_inline_functions.h>

// Linux Am.Ui.Screen
//
// One process-singleton palette + display-bounds metadata cache.
// AmigaOS's Screen opens a custom display via OpenScreenTagList;
// Linux WMs don't expose that API at all — am-ide opens one
// window in the user's existing desktop, the "Screen" is just the
// place we hang the palette table off and the cached
// display dimensions. No SDL_Window is created here; that's
// Window.c's job.

// ---------------------------------------------------------------------------
// Process-global palette pointer.
//
// LayerGraphics needs to resolve pen indices into ARGB without a back-
// reference to the Window's Screen on every call. We expose a single
// shared pointer + count via am_ui_linux_screen_palette() that
// LayerGraphics borrows. When no Screen has been opened yet we point
// at the static default below; once openScreen runs, we re-point at
// the live Screen's data->palette array.
// ---------------------------------------------------------------------------

// Default 32-entry palette — overwritten by Program.aml's
// buildPalette() at startup. Picked to make a smoke-test build
// readable even if the AmLang side never calls setColor: pen 0 =
// dark editor bg, pen 1 = light text, pen 7 = a cyan-ish
// "selection" so the IDE chrome isn't all monochrome before the
// real palette pushes in.
static const Uint32 default_palette[AM_UI_LINUX_SCREEN_PALETTE_SIZE] = {
    0xFF1E1E1E, // 0  editorBg
    0xFFD4D4D4, // 1  text
    0xFFFFFFFF, // 2  textBright
    0xFF4C909C, // 3  selection
    0xFF1E1E1E, // 4  editorBg (alias)
    0xFFD4D4D4, // 5  text (alias)
    0xFFFFFFFF, // 6  textBright (alias)
    0xFF4C909C, // 7  selection (alias)
    0xFF252526, // 8  sidebarBg
    0xFF007ACC, // 9  accent
    0xFF2D2D30, // 10 statusBg
    0xFF858585, // 11 muted
    0xFF3F3F46, // 12 border
    0xFFCCCCCC, // 13 textDim
    0xFF4EC95A, // 14 gitGreen
    0xFFE68A1A, // 15 linkOrange
    // Pens 16-31 left as middle-gray so a sprite that doesn't set
    // a colour at this index doesn't render as solid black.
    0xFF606060, 0xFF606060, 0xFF606060, 0xFF606060,
    0xFF606060, 0xFF606060, 0xFF606060, 0xFF606060,
    0xFF606060, 0xFF606060, 0xFF606060, 0xFF606060,
    0xFF606060, 0xFF606060, 0xFF606060, 0xFF606060,
};

static const Uint32 *g_palette = default_palette;
static int          g_palette_count = AM_UI_LINUX_SCREEN_PALETTE_SIZE;

const Uint32 *am_ui_linux_screen_palette(void)        { return g_palette; }
int           am_ui_linux_screen_palette_count(void)  { return g_palette_count; }

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

function_result Am_Ui_Screen__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);

    Am_Ui_Screen_data *data = (Am_Ui_Screen_data *) calloc(1, sizeof(Am_Ui_Screen_data));
    if (data != NULL) {
        for (int i = 0; i < AM_UI_LINUX_SCREEN_PALETTE_SIZE; i++) {
            data->palette[i] = default_palette[i];
        }
        data->display_w = 0;
        data->display_h = 0;
        this->object_properties.class_object_properties.object_data.value.custom_value = data;
    }

    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Screen__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}

function_result Am_Ui_Screen__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };

    Am_Ui_Screen_data *data = (Am_Ui_Screen_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data != NULL) {
        if (g_palette == data->palette) {
            // We were the live palette source; revert to the static
            // default so any straggling paint after the Screen is
            // gone still resolves something.
            g_palette = default_palette;
            g_palette_count = AM_UI_LINUX_SCREEN_PALETTE_SIZE;
        }
        free(data);
        this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }

    return __result;
}

// ---------------------------------------------------------------------------
// open / close
//
// On AmigaOS this is OpenScreenTagList. On Linux it's a metadata cache:
// pull SDL_GetDisplayBounds(0) for the user's primary display so callers
// that ask getWidth / getHeight have something to layout against.
// SDL_Init(SDL_INIT_VIDEO) is invoked by Startup.c — by the time any
// AmLang code opens a Screen the subsystem is already up.
// ---------------------------------------------------------------------------

function_result Am_Ui_Screen_open_0(aobject *const this,
                                    int width, int height, int depth, int displayId,
                                    aobject *title, aobject *systemPens, aobject *paletteColors)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (title != NULL)         __increase_reference_count(title);
    if (systemPens != NULL)    __increase_reference_count(systemPens);
    if (paletteColors != NULL) __increase_reference_count(paletteColors);

    Am_Ui_Screen_data *data = (Am_Ui_Screen_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data == NULL) goto __exit;

    // Capture the user's primary display geometry. SDL_GetDisplayBounds
    // returns 0 on success; on failure we fall back to the requested
    // (width, height) so layout has something to chew on.
    SDL_Rect bounds = { 0, 0, 0, 0 };
    if (SDL_GetDisplayBounds(0, &bounds) != 0) {
        bounds.w = (width  > 0) ? width  : 1280;
        bounds.h = (height > 0) ? height : 720;
    }
    data->display_w = bounds.w;
    data->display_h = bounds.h;
    (void) depth;
    (void) displayId;

    // If the caller passed a palette colour array, copy it into the
    // Screen's palette table. AmigaOS reads this back to SetRGB32; we
    // just store it for paint-time lookup.
    if (paletteColors != NULL) {
        array_holder *ah = (array_holder *) &paletteColors[1];
        unsigned int *src = (unsigned int *) ah->array_data;
        int n = (int) ah->size;
        if (n > AM_UI_LINUX_SCREEN_PALETTE_SIZE) n = AM_UI_LINUX_SCREEN_PALETTE_SIZE;
        for (int i = 0; i < n; i++) {
            // AmLang sends 0x00RRGGBB; we store 0xFFRRGGBB so an SDL
            // SDL_SetRenderDrawColor at paint time gets a non-zero
            // alpha and actually draws.
            Uint32 c = src[i];
            if ((c & 0xFF000000u) == 0) c |= 0xFF000000u;
            data->palette[i] = c;
        }
    }
    // systemPens is the host-DRI pen INDEX array (UByte). On Linux
    // there's no host DRI so we ignore it — the IDE will overwrite
    // every chrome pen via setColor() anyway.
    (void) systemPens;
    (void) title;

    // Publish this Screen's palette as the global lookup. The next
    // Window opened will resolve pens against it.
    g_palette = data->palette;
    g_palette_count = AM_UI_LINUX_SCREEN_PALETTE_SIZE;

__exit:
    if (paletteColors != NULL) __decrease_reference_count(paletteColors);
    if (systemPens != NULL)    __decrease_reference_count(systemPens);
    if (title != NULL)         __decrease_reference_count(title);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Screen_close_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    // Nothing native to tear down — palette stays in the data struct
    // until _native_release_0 frees it. We deliberately don't reset
    // the global g_palette here in case a Window is still painting.
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Geometry accessors
// ---------------------------------------------------------------------------

function_result Am_Ui_Screen_getBarHeight_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    // WM owns the title bar — content area already excludes it.
    __result.return_value.value.ushort_value = (unsigned short) 0;
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Screen_getWidth_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    Am_Ui_Screen_data *data = (Am_Ui_Screen_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
    __result.return_value.value.ushort_value = (unsigned short) (data ? data->display_w : 0);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Screen_getHeight_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    Am_Ui_Screen_data *data = (Am_Ui_Screen_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
    __result.return_value.value.ushort_value = (unsigned short) (data ? data->display_h : 0);
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Screen-mode metadata
// ---------------------------------------------------------------------------

function_result Am_Ui_Screen_bestModeId_0(int width, int height, int depth)
{
    function_result __result = { .has_return_value = true };
    (void) width; (void) height; (void) depth;
    // No mode enumeration on Linux WMs — always returns 0.
    __result.return_value.value.int_value = 0;
    return __result;
}

function_result Am_Ui_Screen_fillDefaultScreenMode_0(aobject *mode)
{
    function_result __result = { .has_return_value = false };
    if (mode != NULL) __increase_reference_count(mode);

    SDL_Rect bounds = { 0, 0, 1280, 720 };
    SDL_GetDisplayBounds(0, &bounds);

    if (mode != NULL) {
        mode->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_width].nullable_value.value.int_value     = bounds.w;
        mode->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_height].nullable_value.value.int_value    = bounds.h;
        mode->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_depth].nullable_value.value.int_value     = 32;
        mode->object_properties.class_object_properties.properties[Am_Ui_ScreenMode_P_displayId].nullable_value.value.int_value = 0;
    }

    if (mode != NULL) __decrease_reference_count(mode);
    return __result;
}

// ---------------------------------------------------------------------------
// Palette read / write
// ---------------------------------------------------------------------------

function_result Am_Ui_Screen_setColor_0(aobject *const this, int index, unsigned char r, unsigned char g, unsigned char b)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);

    Am_Ui_Screen_data *data = (Am_Ui_Screen_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data != NULL && index >= 0 && index < AM_UI_LINUX_SCREEN_PALETTE_SIZE) {
        data->palette[index] = 0xFF000000u
                             | ((Uint32) r << 16)
                             | ((Uint32) g << 8)
                             |  (Uint32) b;
    }

    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Screen_copyHostPens_0(aobject *const this, int count)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);

    Am_Ui_Screen_data *data = (Am_Ui_Screen_data *) this->object_properties.class_object_properties.object_data.value.custom_value;
    if (data != NULL && count > 0) {
        if (count > AM_UI_LINUX_SCREEN_PALETTE_SIZE) count = AM_UI_LINUX_SCREEN_PALETTE_SIZE;
        for (int i = 0; i < count; i++) {
            data->palette[i] = default_palette[i];
        }
    }

    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Screen_fillHostPaletteColors_0(aobject *out, int count)
{
    function_result __result = { .has_return_value = false };
    if (out != NULL) __increase_reference_count(out);

    if (out == NULL || count <= 0) goto __exit;

    array_holder *ah = (array_holder *) &out[1];
    unsigned int *dst = (unsigned int *) ah->array_data;
    int cap = (int) ah->size;
    if (count > cap) count = cap;
    if (count > AM_UI_LINUX_SCREEN_PALETTE_SIZE) count = AM_UI_LINUX_SCREEN_PALETTE_SIZE;

    for (int i = 0; i < count; i++) {
        // AmLang side expects 0x00RRGGBB on this read path (see
        // Program.aml — it copies these into Palette.colors).
        dst[i] = default_palette[i] & 0x00FFFFFFu;
    }

__exit:
    if (out != NULL) __decrease_reference_count(out);
    return __result;
}

function_result Am_Ui_Screen_fillHostDrawInfoPens_0(aobject *out, int count)
{
    function_result __result = { .has_return_value = false };
    if (out != NULL) __increase_reference_count(out);

    if (out == NULL || count <= 0) goto __exit;

    array_holder *ah = (array_holder *) &out[1];
    unsigned char *dst = (unsigned char *) ah->array_data;
    int cap = (int) ah->size;
    if (count > cap) count = cap;

    // Identity mapping — pen role i = palette index i. AmigaOS samples
    // the host DRI here so a new screen inherits the user's Workbench
    // pen role mapping; on Linux there's no role-mapping concept, so
    // we hand back 0..count-1 and let the IDE chrome's per-role
    // assignments in Program.buildPalette do the real work.
    for (int i = 0; i < count; i++) {
        dst[i] = (unsigned char) i;
    }

__exit:
    if (out != NULL) __decrease_reference_count(out);
    return __result;
}

#endif
