#ifndef native_aclass_Am_Ui_Window_c
#define native_aclass_Am_Ui_Window_c

#include <libc/core.h>
#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Ui/Screen.h>
#include <Am/Ui/RenderableBitmap.h>
#include <Am/Ui/Bitmap.h>
#include <Am/Lang/Int.h>
#include <Am/Lang/Exception.h>
#include <linux-x64/Am/Ui/Window.h>
#include <linux-x64/Am/Ui/Screen.h>
#include <linux-x64/Am/Ui/Bitmap.h>

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef AM_UI_LINUX_GTK
#include <gtk/gtk.h>
#include <gdk/gdkx.h>          // gdk_x11_window_get_xid
#include <gdk/gdkkeysyms.h>    // GDK_KEY_*
#endif

#include <libc/core_inline_functions.h>

// Defined in LayerGraphics.c — keep the apply_target cache in sync
// when we change render targets directly from outside.
extern void am_ui_macos_arm_lg_target_changed(SDL_Renderer *renderer, SDL_Texture *now);

// Linux Am.Ui.Window — SDL2 backend.
//
// One SDL_Window + SDL_Renderer per AmLang Window. The renderer is the
// resource LayerGraphics borrows during paint; Bitmap.c uploads its
// surfaces to textures bound to it.
//
// Event loop: handleInput() pumps SDL events and dispatches mouse +
// keyboard + close + resize back into the AmLang side via the
// `Am_Ui_Window_f_*` callbacks the amigaos backend uses too. The
// AmLang side then walks the View tree.
//
// What's stubbed in this file (logged once per call site, not silent):
//   - menu strip ops (nativeBeginMenuStrip etc.) — Linux menus will
//     come later as an in-canvas menu bar via View tree, not a native
//     widget.
//   - clipboard (copyToClipboard / pasteFromClipboard) — TODO with
//     SDL_SetClipboardText / SDL_GetClipboardText, not wired yet.
//   - getUserPortSigBit / getUserPortTaskPtr — AmigaOS exec.library
//     concepts that have no equivalent on Linux; return 0.
//
// External AmLang callbacks invoked from here (compiled C symbols of
// the AmLang-side methods):
//   Am_Ui_Window_f_onMouseEvent_0(this, button, type, x, y)
//      button: 1=left, 2=middle, 3=right
//      type:   0=move, 1=up, 2=down
//   Am_Ui_Window_f_onMouseWheel_0(this, dy, x, y)
//   Am_Ui_Window_f_onResize_0(this, x, y, w, h)
//   Am_Ui_Window_f_setBorder_0(this, l, t, r, b)
//   Am_Ui_Window_f_setRootView_0(this, view)

// The AmLang-side callbacks are already declared in shared/Am/Ui/Window.h
// (included transitively via Am/Ui/Window.h). Their canonical
// signatures are:
//   Am_Ui_Window_f_onMouseEvent_0(this, int type, int button, short x, short y)
//     type: 0=move, 1=up, 2=down (matches MouseEventType enum)
//     button: 1=left, 2=middle, 3=right
//   Am_Ui_Window_f_onMouseWheel_0(this, short deltaY, short x, short y)
//   Am_Ui_Window_f_onResize_0(this, short x, short y, ushort w, ushort h)
//   Am_Ui_Window_f_setBorder_0(this, short l, short t, short r, short b)
//   Am_Ui_Window_f_setRootView_0(this, aobject *view)

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

static Am_Ui_Window_data *win_data(aobject *const this)
{
    return (Am_Ui_Window_data *) __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value;
}

// Process-global "most recent renderer" pointer. Window.open_0 updates
// this so ViewContextGraphics (which can't traverse the
// AmLang ViewContext interface from native C) has a paint surface.
static SDL_Renderer *g_primary_renderer = NULL;
SDL_Renderer *am_ui_linux_primary_renderer(void) { return g_primary_renderer; }

// Map an SDL mouse button to the (1=left, 2=middle, 3=right) convention
// the AmigaOS path uses. Returns 0 for unknown buttons.
static unsigned char map_sdl_button(Uint8 b)
{
    if (b == SDL_BUTTON_LEFT)   return 1;
    if (b == SDL_BUTTON_MIDDLE) return 2;
    if (b == SDL_BUTTON_RIGHT)  return 3;
    return 0;
}

// Pull the C string out of an AmLang String aobject. NULL on null.
static const char *amlang_str(aobject *s)
{
    if (s == NULL) return NULL;
    string_holder *sh = (string_holder *) (s + 1);
    if (sh == NULL) return NULL;
    return sh->string_value;
}

#ifdef AM_UI_LINUX_GTK
// ===========================================================================
// GTK shell — a GtkWindow with a GtkMenuBar on top and a GtkDrawingArea
// below. SDL renders into the drawing area's X11 window; GTK owns the
// event loop, input, and the native menu bar. handleInput() pumps GTK
// and these signal handlers translate input into the same
// Am_Ui_Window_f_* callbacks the pure-SDL path used.
// ===========================================================================

extern function_result Am_Ui_MenuItem_f_invokeClick_0(aobject *const this);

// MenuItem property indices (mirror the Cocoa bridge's hardcoding so we
// don't take a header dependency just to read enabled / isSeparator).
#define AMUI_MENUITEM_P_enabled     2
#define AMUI_MENUITEM_P_isSeparator 5

static int g_gtk_inited = 0;

static bool item_bool(aobject *item, int idx, bool dflt) {
    if (item == NULL) return dflt;
    return __unwrap(item)->object_properties.class_object_properties.properties[idx].nullable_value.value.bool_value;
}

static unsigned char map_gdk_button(guint b) {
    if (b == 1) return 1;   // left
    if (b == 2) return 2;   // middle
    if (b == 3) return 3;   // right
    return 0;
}

// GTK runs at 1:1 so event coords are PHYSICAL; the IDE/View tree works
// in LOGICAL pixels. Divide by the scale to convert.
static int win_ui_scale(Am_Ui_Window_data *data) {
    return (data != NULL && data->ui_scale > 0) ? data->ui_scale : 1;
}

static gboolean gtk_on_button(GtkWidget *w, GdkEventButton *e, gpointer user) {
    aobject *this = (aobject *) user;
    Am_Ui_Window_data *data = win_data(this);
    if (data == NULL) return TRUE;
    unsigned char btn = map_gdk_button(e->button);
    if (btn == 0) return TRUE;
    int s = win_ui_scale(data);
    short px = (short) ((int) e->x / s);
    short py = (short) ((int) e->y / s);
    data->last_mouse_x = px;
    data->last_mouse_y = py;
    if (e->type == GDK_BUTTON_PRESS) gtk_widget_grab_focus(w);
    int type = (e->type == GDK_BUTTON_PRESS) ? 3 : 2;   // down=3, up=2
    Am_Ui_Window_f_onMouseEvent_0(this, type, btn, px, py);
    return TRUE;
}

static gboolean gtk_on_motion(GtkWidget *w, GdkEventMotion *e, gpointer user) {
    (void) w;
    aobject *this = (aobject *) user;
    Am_Ui_Window_data *data = win_data(this);
    if (data == NULL) return TRUE;
    int s = win_ui_scale(data);
    short px = (short) ((int) e->x / s);
    short py = (short) ((int) e->y / s);
    data->last_mouse_x = px;
    data->last_mouse_y = py;
    Am_Ui_Window_f_onMouseEvent_0(this, 1, 0, px, py);  // move
    return TRUE;
}

static gboolean gtk_on_scroll(GtkWidget *w, GdkEventScroll *e, gpointer user) {
    (void) w;
    aobject *this = (aobject *) user;
    Am_Ui_Window_data *data = win_data(this);
    if (data == NULL) return TRUE;
    short dy = 0;
    if (e->direction == GDK_SCROLL_UP)        dy = 1;
    else if (e->direction == GDK_SCROLL_DOWN) dy = -1;
    else if (e->direction == GDK_SCROLL_SMOOTH) dy = (e->delta_y < 0) ? 1 : (e->delta_y > 0 ? -1 : 0);
    if (dy != 0) Am_Ui_Window_f_onMouseWheel_0(this, dy, data->last_mouse_x, data->last_mouse_y);
    return TRUE;
}

static gboolean gtk_on_key(GtkWidget *w, GdkEventKey *e, gpointer user) {
    (void) w;
    aobject *this = (aobject *) user;
    int type = (e->type == GDK_KEY_PRESS) ? 1 : 2;   // down=1, up=2
    guint sym = e->keyval;
    gboolean shift = (e->state & GDK_SHIFT_MASK) != 0;
    gboolean ctrl  = (e->state & GDK_CONTROL_MASK) != 0;
    int amiga_code = 0, amiga_char = 0;
    switch (sym) {
        case GDK_KEY_BackSpace: amiga_code = 65; amiga_char = 8;   break;
        case GDK_KEY_Delete:    amiga_code = 70; amiga_char = 127; break;
        case GDK_KEY_Left:      amiga_code = 79; amiga_char = shift ? 1 : 0; break;
        case GDK_KEY_Right:     amiga_code = 78; amiga_char = shift ? 1 : 0; break;
        case GDK_KEY_Up:        amiga_code = 76; amiga_char = shift ? 1 : 0; break;
        case GDK_KEY_Down:      amiga_code = 77; amiga_char = shift ? 1 : 0; break;
        case GDK_KEY_Tab:       amiga_code = 66; amiga_char = shift ? 0 : 9;   break;
        case GDK_KEY_Return:
        case GDK_KEY_KP_Enter:  amiga_code = 68; amiga_char = 13;  break;
        case GDK_KEY_Escape:    amiga_code = 69; amiga_char = 27;  break;
        default:
            if (ctrl && sym >= GDK_KEY_a && sym <= GDK_KEY_z) {
                amiga_code = (int) sym;
                amiga_char = (int) (sym - GDK_KEY_a + 1);   // Ctrl+A=1 .. Ctrl+Z=26
            } else if (!ctrl && e->type == GDK_KEY_PRESS) {
                // Printable: mirror the SDL_TEXTINPUT path (code 0, char = unicode).
                guint32 uni = gdk_keyval_to_unicode(sym);
                if (uni >= 32 && uni <= 126) {
                    Am_Ui_Window_f_onKeyboardEvent_0(this, 1, 0, (unsigned short) uni);
                    return TRUE;
                }
            }
            break;
    }
    if (amiga_code != 0 || amiga_char != 0) {
        Am_Ui_Window_f_onKeyboardEvent_0(this, type, (unsigned short) amiga_code, (unsigned short) amiga_char);
    }
    return TRUE;
}

static gboolean gtk_on_configure(GtkWidget *w, GdkEventConfigure *e, gpointer user) {
    (void) w;
    aobject *this = (aobject *) user;
    Am_Ui_Window_data *data = win_data(this);
    if (data == NULL || data->renderer == NULL) return FALSE;
    int s = win_ui_scale(data);
    int lw = e->width / s, lh = e->height / s;   // physical -> logical
    if (lw != data->last_paint_w || lh != data->last_paint_h) {
        data->last_paint_w = lw;
        data->last_paint_h = lh;
        Am_Ui_Window_f_onResize_0(this, 0, 0, (unsigned short) lw, (unsigned short) lh);
        data->pending_refresh = true;
    }
    return FALSE;
}

// Display the SDL offscreen through Cairo. Views render into the SDL
// offscreen TARGET texture exactly as on macOS/amigaos; here we read those
// pixels back and paint them into the GtkDrawingArea's Cairo context. GTK
// hands us a `cr` already translated to the drawing area's visible top-left
// (below the menu bar) and at the right HiDPI device scale, so content lands
// exactly where input expects it — no offset / no manual scaling juggling
// against a foreign SDL X window.
static gboolean gtk_on_draw(GtkWidget *w, cairo_t *cr, gpointer user) {
    (void) w;
    aobject *this = (aobject *) user;
    Am_Ui_Window_data *data = win_data(this);
    if (data == NULL || data->renderer == NULL) return TRUE;

    aobject *off_rb = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Window_P_offscreen].nullable_value.value.object_value;
    if (off_rb == NULL) return TRUE;
    aobject *off_bm = __unwrap(off_rb)->object_properties.class_object_properties.properties[Am_Ui_RenderableBitmap_P_bitmap].nullable_value.value.object_value;
    if (off_bm == NULL) return TRUE;
    Am_Ui_Bitmap_data *bd = (Am_Ui_Bitmap_data *) __unwrap(off_bm)->object_properties.class_object_properties.object_data.value.custom_value;
    if (bd == NULL || bd->texture == NULL) return TRUE;

    int tw = 0, th = 0;
    SDL_QueryTexture(bd->texture, NULL, NULL, &tw, &th);
    if (tw <= 0 || th <= 0) return TRUE;

    int stride = cairo_format_stride_for_width(CAIRO_FORMAT_RGB24, tw);
    size_t need = (size_t) stride * th;
    if (data->cairo_buf == NULL || data->cairo_buf_sz < need) {
        free(data->cairo_buf);
        data->cairo_buf = (unsigned char *) malloc(need);
        data->cairo_buf_sz = data->cairo_buf ? need : 0;
    }
    if (data->cairo_buf == NULL) return TRUE;

    // Read the offscreen texture into the CPU buffer. SDL ARGB8888 is, on
    // little-endian, byte order B,G,R,A — identical to Cairo's RGB24/ARGB32
    // memory layout, so no swizzle is needed.
    SDL_SetRenderTarget(data->renderer, bd->texture);
    am_ui_macos_arm_lg_target_changed(data->renderer, bd->texture);
    int rc = SDL_RenderReadPixels(data->renderer, NULL, SDL_PIXELFORMAT_ARGB8888, data->cairo_buf, stride);
    SDL_SetRenderTarget(data->renderer, NULL);
    am_ui_macos_arm_lg_target_changed(data->renderer, NULL);
    {
        // Log per-window so we can tell whether the main IDE's gtk_on_draw
        // is firing at all (vs only the splash). Cap each window at 3 lines
        // so the log doesn't drown in repeats.
        static struct { void *win; int n; } per[8];
        int slot = -1;
        for (int k = 0; k < 8; k++) {
            if (per[k].win == this) { slot = k; break; }
            if (per[k].win == NULL && slot < 0) slot = k;
        }
        if (slot >= 0 && per[slot].win == NULL) per[slot].win = this;
        if (slot >= 0) {
            // Sample every 5th draw so we can watch the offscreen evolve
            // (not just the first three frames before content hits it).
            // Also sample more pixels (start, middle, and a row 100 down)
            // so we can see if the offscreen has any non-uniform content.
            if ((per[slot].n % 5) == 0 && per[slot].n < 60) {
                unsigned char *p = data->cairo_buf;
                unsigned char *q = data->cairo_buf + 100 * stride + 300 * 4;   // ~row 100, col 300
                fprintf(stderr, "[am-ui/cairo-blit win=%p n=%d] tl=%02x%02x%02x%02x %02x%02x%02x%02x  mid=%02x%02x%02x%02x %02x%02x%02x%02x\n",
                    (void *) this, per[slot].n,
                    p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7],
                    q[0], q[1], q[2], q[3], q[4], q[5], q[6], q[7]);
            }
            per[slot].n++;
        }
    }
    if (rc != 0) return TRUE;

    cairo_surface_t *surf = cairo_image_surface_create_for_data(
        data->cairo_buf, CAIRO_FORMAT_RGB24, tw, th, stride);
    if (cairo_surface_status(surf) != CAIRO_STATUS_SUCCESS) { cairo_surface_destroy(surf); return TRUE; }
    {
        // Diagnostic: dump the first paint's cairo source to PNG so we
        // can confirm cairo is reading our SDL bytes the way we think.
        // If /tmp/amui-cairo-source.png looks dark gray → cairo reads
        // correctly, bug is in paint. If white → format/endian mismatch.
        static int once = 0;
        if (!once) {
            cairo_status_t st = cairo_surface_write_to_png(surf, "/tmp/amui-cairo-source.png");
            fprintf(stderr, "[am-ui/cairo-png] wrote /tmp/amui-cairo-source.png status=%d (CAIRO_STATUS_SUCCESS=%d)\n",
                (int) st, (int) CAIRO_STATUS_SUCCESS);
            once = 1;
        }
    }

    int s = win_ui_scale(data);
    cairo_save(cr);
    cairo_scale(cr, s, s);                       // offscreen is logical; da cr is physical
    cairo_set_source_surface(cr, surf, 0, 0);
    cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
    cairo_paint(cr);
    cairo_restore(cr);

    // Diagnostic: overlay a 40x40 solid red rect at (10, 10) on top of
    // the SDL-blit. If we can see this in the running window the cairo
    // paint path is fine and the bug is upstream (offscreen contents).
    // If we can't see this either, GTK is overpainting our work.
    cairo_save(cr);
    cairo_set_source_rgb(cr, 1.0, 0.0, 0.0);
    cairo_rectangle(cr, 10, 10, 40, 40);
    cairo_fill(cr);
    cairo_restore(cr);

    cairo_surface_destroy(surf);
    return TRUE;
}

static gboolean gtk_on_delete(GtkWidget *w, GdkEvent *e, gpointer user) {
    (void) w; (void) e;
    aobject *this = (aobject *) user;
    Am_Ui_Window_data *data = win_data(this);
    if (data != NULL) data->pending_close = true;
    return TRUE;   // teardown happens via close_0; don't let GTK destroy now
}

// GtkMenuItem "activate" → MenuItem.invokeClick(). The MenuItem aobject*
// is stashed on the widget by nativeAddMenuItem.
static void gtk_on_menu_activate(GtkMenuItem *mi, gpointer user) {
    (void) user;
    aobject *item = (aobject *) g_object_get_data(G_OBJECT(mi), "amlang_item");
    fprintf(stderr, "[am-ui/linux-gtk] menu activate: item=%p\n", (void *) item);
    if (item == NULL) return;
    function_result fr = Am_Ui_MenuItem_f_invokeClick_0(item);
    if (fr.exception != NULL) {
        aobject *msg_obj = __unwrap(fr.exception)->object_properties.class_object_properties.properties[Am_Lang_Exception_P_message].nullable_value.value.object_value;
        const char *msg = "(no msg)";
        if (msg_obj != NULL) {
            string_holder *sh = (string_holder *) (msg_obj + 1);
            if (sh != NULL && sh->string_value != NULL) msg = sh->string_value;
        }
        fprintf(stderr, "[am-ui/linux-gtk] menu click threw: %s\n", msg);
        aobject *st = __unwrap(fr.exception)->object_properties.class_object_properties.properties[Am_Lang_Exception_P_stackTrace].nullable_value.value.object_value;
        if (st != NULL) {
            function_result n = Am_Collections_List_ta_Am_Lang_String_f_getSize_0(st);
            if (n.exception == NULL) {
                int cnt = (int) n.return_value.value.int_value;
                for (int i = 0; i < cnt && i < 20; i++) {
                    function_result f = Am_Collections_List_ta_Am_Lang_String_f_get_0(st, i);
                    if (f.exception != NULL) { __decrease_reference_count(f.exception); continue; }
                    aobject *frame = f.return_value.value.object_value;
                    const char *fs = "(null)";
                    if (frame != NULL) { string_holder *fh = (string_holder *)(frame + 1); if (fh && fh->string_value) fs = fh->string_value; }
                    fprintf(stderr, "[am-ui/linux-gtk]   #%d %s\n", i, fs);
                }
            } else { __decrease_reference_count(n.exception); }
        }
        __decrease_reference_count(fr.exception);
    }
    // A menu pick almost always changes the UI (opens an app, a dialog,
    // toggles state) — ask the Window to repaint on the next pump.
    {
        aobject *win_this = (aobject *) g_object_get_data(G_OBJECT(mi), "amlang_window");
        if (win_this != NULL) {
            Am_Ui_Window_data *d = win_data(win_this);
            if (d != NULL) d->pending_refresh = true;
        }
    }
}

// Strip GTK/GIO/pixbuf module env vars that point into a foreign snap
// sandbox (most commonly VS Code's snap, whose integrated terminal
// exports GTK_PATH=/snap/code/.../gtk-3.0, GIO_MODULE_DIR=~/snap/code/…,
// GDK_PIXBUF_MODULEDIR=/snap/code/…). If left set, gtk_init loads those
// snap GTK modules, which drag /snap/core20's libpthread (glibc 2.31)
// into our process alongside the system glibc and crash with
// "undefined symbol: __libc_pthread_init, version GLIBC_PRIVATE".
// Unsetting them makes GTK fall back to the system module paths. Only
// vars whose value smells of snap are touched, so a normal launch is
// left alone.
static void am_ui_gtk_sanitize_env(void) {
    static const char *vars[] = {
        "GTK_PATH", "GTK_EXE_PREFIX", "GIO_MODULE_DIR",
        "GDK_PIXBUF_MODULEDIR", "GDK_PIXBUF_MODULE_FILE",
        "GTK_IM_MODULE_FILE", "LOCPATH", NULL
    };
    for (int i = 0; vars[i] != NULL; i++) {
        const char *v = getenv(vars[i]);
        if (v != NULL && strstr(v, "snap") != NULL) {
            fprintf(stderr, "[am-ui/linux-gtk] unsetting snap-tainted %s\n", vars[i]);
            unsetenv(vars[i]);
        }
    }
    // Pin GTK to 1:1. We do HiDPI scaling MANUALLY (window x scale,
    // content offscreen stretched x scale) because GTK's own auto-scale
    // is applied inconsistently here — gdk_monitor_get_scale_factor isn't
    // settled when getHostScreenWidth runs, so window/content sizes
    // disagreed. With GDK_SCALE=1, GTK logical == physical and everything
    // is predictable; the scale comes from am_ui_gtk_scale_factor().
    setenv("GDK_SCALE", "1", 1);
}

// The desired UI scale, derived from the font DPI. 96 dpi -> 1, 192 -> 2.
// Read from GtkSettings' gtk-xft-dpi (1024ths of a point), which reflects
// the desktop's Xft.dpi and is populated at gtk_init — so it's stable
// from the very first call (getHostScreenWidth runs before any window).
// gdk_screen_get_resolution() was unreliable here: it returned -1 early,
// giving scale 1 in getHostScreen but 2 later, so the window came out
// the wrong size. Cached so every caller agrees.
static int g_ui_scale_cached = 0;
static int am_ui_gtk_scale_factor(void) {
    if (g_ui_scale_cached > 0) return g_ui_scale_cached;
    if (!g_gtk_inited) {
        am_ui_gtk_sanitize_env();
        if (!gtk_init_check(NULL, NULL)) return 1;
        g_gtk_inited = 1;
    }
    int s = 1;
    GtkSettings *st = gtk_settings_get_default();
    if (st != NULL) {
        int dpi1024 = 0;
        g_object_get(st, "gtk-xft-dpi", &dpi1024, NULL);
        if (dpi1024 > 0) {
            double dpi = dpi1024 / 1024.0;
            s = (int) ((dpi / 96.0) + 0.5);
        }
    }
    if (s < 1) s = 1;
    g_ui_scale_cached = s;
    return s;
}

// Build the GTK window + menubar + drawing area. Returns the drawing
// area's X11 window id for SDL_CreateWindowFrom, or 0 on failure.
static unsigned long am_ui_gtk_build_shell(aobject *this, Am_Ui_Window_data *data,
                                           const char *title, int x, int y, int w, int h) {
    if (!g_gtk_inited) {
        am_ui_gtk_sanitize_env();
        if (!gtk_init_check(NULL, NULL)) {
            fprintf(stderr, "[am-ui/linux-gtk] gtk_init_check failed (no display?)\n");
            return 0;
        }
        g_gtk_inited = 1;
    }

    // HiDPI model: the AmLang side works entirely in LOGICAL pixels
    // (getHostScreenWidth/Height already return logical), so `w`/`h` here
    // are logical. GTK widget sizes are logical too, so pass them straight
    // through; GTK makes the backing X window physical = logical * scale.
    // SDL then renders at that physical size and the paint tail stretches
    // the logical-sized offscreen up to it — giving 2x-density content +
    // menu while the window stays screen-sized.
    // `w`/`h` are LOGICAL (getHostScreen already divided by the scale).
    // GTK runs at 1:1, so to make the physical window screen-sized we
    // multiply back up. The SDL render surface is this physical size; the
    // logical-sized offscreen is stretched onto it in the paint tail, and
    // the menu bar font is scaled up via CSS — so both content and chrome
    // come out at the chosen density on a screen-sized window.
    data->ui_scale = am_ui_gtk_scale_factor();
    int lw = w * data->ui_scale;
    int lh = h * data->ui_scale;

    // GDK_SCALE=1 makes GTK render its chrome at 1x (tiny menu on 4K).
    // Bring the menu/font density back up to match the scaled content by
    // setting the font DPI to scale*96 (gtk-xft-dpi is in 1024ths of a
    // point). scale 2 -> 192 dpi -> 2x menu text.
    GtkSettings *settings = gtk_settings_get_default();
    if (settings != NULL) {
        g_object_set(settings, "gtk-xft-dpi", data->ui_scale * 96 * 1024, NULL);
    }

    GtkWidget *win     = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), title ? title : "amStudio");
    gtk_window_set_default_size(GTK_WINDOW(win), lw, lh);

    // Decide whether this is a "fill the screen" window (the IDE main
    // window, requested at roughly the monitor size) or a small one (the
    // splash). A main window sized at the full screen height plus a menu
    // bar plus the WM title bar overflows the display — the bottom scrolls
    // off. Maximizing instead hands sizing to the window manager, which
    // fits decorations + menu + drawing area into the work area, so the
    // bottom stays visible (and the window remains user-resizable). Small
    // windows open at their requested size.
    int should_maximize = 0;
    GdkMonitor *fit_mon = NULL;
    GdkRectangle fit_workarea = {0, 0, 0, 0};
    {
        GdkDisplay *disp = gdk_display_get_default();
        GdkMonitor *mon  = disp ? gdk_display_get_primary_monitor(disp) : NULL;
        if (!mon && disp) mon = gdk_display_get_monitor(disp, 0);
        GdkRectangle geo = {0, 0, 0, 0};
        if (mon) { gdk_monitor_get_geometry(mon, &geo); gdk_monitor_get_workarea(mon, &fit_workarea); }
        // geo is physical pixels (we force GDK_SCALE=1); lw/lh are physical
        // too. Treat "≥ 85% of the monitor in either axis" as fullscreen.
        if (geo.height > 0 && geo.width > 0 &&
            (lh >= geo.height * 85 / 100 || lw >= geo.width * 85 / 100)) {
            should_maximize = 1;
            fit_mon = mon;
        }
    }

    GtkWidget *vbox    = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    GtkWidget *menubar = gtk_menu_bar_new();
    GtkWidget *da      = gtk_drawing_area_new();
    // The menu items are populated later by the AmLang side (nativeAddMenu*),
    // so the bar is empty here — and an empty GtkMenuBar requests only ~1px of
    // height. If we sized/grabbed the drawing area now, it would sit at y=0
    // (full height) and the menu bar, once populated, would grow downward and
    // overdraw the top line of the SDL content ("welcome tab too far up").
    // Add a throwaway item so the bar realizes at its true height; we pin that
    // height and drop the probe before grabbing the XID, so the drawing area
    // is positioned below the real menu bar from the start.
    GtkWidget *menu_probe = gtk_menu_item_new_with_label("Xj");
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), menu_probe);
    // Only a small *minimum* so the window stays resizable and the
    // window+menubar+titlebar don't overflow the screen (a size_request
    // at the full size both pinned the window non-resizable and made it
    // taller than the display once the menu/title bar were added). The
    // drawing area gets its real size by filling the window (box expand),
    // and we wait for that allocation before grabbing the SDL XID below.
    gtk_widget_set_size_request(da, 200, 150);
    gtk_widget_set_can_focus(da, TRUE);
    // gtk_widget_set_double_buffered was the pre-3.14 way to tell GTK
    // not to back-buffer this widget — since 3.14 the call is a no-op
    // (GTK always cairo-back-buffers and we cairo-blit our SDL offscreen
    // into it from gtk_on_draw, which is the correct modern pattern).
    gtk_widget_add_events(da, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK
        | GDK_POINTER_MOTION_MASK | GDK_SCROLL_MASK | GDK_STRUCTURE_MASK
        | GDK_EXPOSURE_MASK);

    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), da,      TRUE,  TRUE,  0);
    gtk_container_add(GTK_CONTAINER(win), vbox);

    g_signal_connect(da,  "button-press-event",   G_CALLBACK(gtk_on_button),    this);
    g_signal_connect(da,  "button-release-event", G_CALLBACK(gtk_on_button),    this);
    g_signal_connect(da,  "motion-notify-event",  G_CALLBACK(gtk_on_motion),    this);
    g_signal_connect(da,  "scroll-event",         G_CALLBACK(gtk_on_scroll),    this);
    g_signal_connect(win, "key-press-event",      G_CALLBACK(gtk_on_key),       this);
    g_signal_connect(win, "key-release-event",    G_CALLBACK(gtk_on_key),       this);
    g_signal_connect(da,  "configure-event",      G_CALLBACK(gtk_on_configure), this);
    g_signal_connect(da,  "draw",                 G_CALLBACK(gtk_on_draw),      this);
    g_signal_connect(win, "delete-event",         G_CALLBACK(gtk_on_delete),    this);

    // Maximize a fullscreen-class window so the WM fits title bar + menu +
    // drawing area into the work area (bottom stays visible, window stays
    // resizable). Let the WM own placement — manually moving the window
    // pushed the title bar off the top of the screen and rode the content too
    // high. (void) the unused work-area we gathered for the decision above.
    (void) fit_mon; (void) fit_workarea;
    if (should_maximize) gtk_window_maximize(GTK_WINDOW(win));

    gtk_widget_show_all(win);
    // Position the window at the requested logical coords (×scale = physical).
    // gtk_window_move uses root-window (screen) coords; call after show_all so
    // the window is realized and the WM hint takes effect.
    if (!should_maximize && (x > 0 || y > 0)) {
        gtk_window_move(GTK_WINDOW(win), x * data->ui_scale, y * data->ui_scale);
    }

    gtk_widget_grab_focus(da);

    // GTK3 uses *client-side* (non-native) windows by default: most widgets
    // share the toplevel's single X11 window and are only drawn into it. So
    // gtk_widget_get_window(da) would hand back the TOPLEVEL's window — whose
    // origin sits at the title bar, behind the menu bar — and SDL would then
    // render from there, hiding the top of the content under the menu. Force
    // the drawing area to get its own real X11 child window, correctly
    // positioned below the menu bar, so SDL_CreateWindowFrom wraps exactly
    // the drawing area's rectangle.
    {
        GdkWindow *daw = gtk_widget_get_window(da);
        if (daw != NULL) gdk_window_ensure_native(daw);
        while (gtk_events_pending()) gtk_main_iteration_do(FALSE);
    }

    // SDL_CreateWindowFrom snapshots the X window size and never tracks later
    // GTK resizes, so the drawing area must already be at its final size before
    // we grab its XID — otherwise SDL renders into a stale viewport (content in
    // a corner, or spilling off the bottom). The resize-to-fit above is applied
    // asynchronously by the WM, so pump the loop until the allocation settles
    // (holds steady on both axes for a few readings).
    int floor_w = should_maximize ? 300 : (lw - 8);
    int prev_w = -1, prev_h = -1, stable = 0;
    for (int i = 0; i < 600; i++) {
        while (gtk_events_pending()) gtk_main_iteration_do(FALSE);
        int aw = gtk_widget_get_allocated_width(da);
        int ah = gtk_widget_get_allocated_height(da);
        if (aw >= floor_w) {
            if (aw == prev_w && ah == prev_h) { if (++stable >= 3) break; }
            else { stable = 0; prev_w = aw; prev_h = ah; }
        }
        g_usleep(5000);
    }

    // Lock in the menu bar's realized height, then drop the probe item so the
    // drawing area's position/size stays put when the real menus are installed.
    {
        int mbh = gtk_widget_get_allocated_height(menubar);
        if (mbh > 1) gtk_widget_set_size_request(menubar, -1, mbh);
        gtk_widget_destroy(menu_probe);
        while (gtk_events_pending()) gtk_main_iteration_do(FALSE);
    }

    GdkWindow *gw = gtk_widget_get_window(da);
    if (gw == NULL) { fprintf(stderr, "[am-ui/linux-gtk] drawing area not realized\n"); return 0; }
    unsigned long xid = (unsigned long) gdk_x11_window_get_xid(gw);

    data->gtk_window    = win;
    data->gtk_menubar   = menubar;
    data->gtk_draw_area = da;
    data->gtk_menu_count = 0;
    return xid;
}
#endif // AM_UI_LINUX_GTK

// ---------------------------------------------------------------------------
// Lifecycle
// ---------------------------------------------------------------------------

function_result Am_Ui_Window__native_init_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);

    Am_Ui_Window_data *data = (Am_Ui_Window_data *) calloc(1, sizeof(Am_Ui_Window_data));
    if (data != NULL) {
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = data;
    }

    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Window__native_mark_children_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    (void) this;
    return __result;
}

function_result Am_Ui_Window__native_release_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };

    Am_Ui_Window_data *data = win_data(this);
    if (data != NULL) {
        if (data->renderer != NULL) { SDL_DestroyRenderer(data->renderer); data->renderer = NULL; }
        if (data->window != NULL)   { SDL_DestroyWindow(data->window);     data->window = NULL; }
#ifdef AM_UI_LINUX_GTK
        if (data->gtk_window != NULL) { gtk_widget_destroy(GTK_WIDGET(data->gtk_window)); data->gtk_window = NULL; data->gtk_menubar = NULL; data->gtk_draw_area = NULL; }
#endif
        free(data);
        __unwrap(this)->object_properties.class_object_properties.object_data.value.custom_value = NULL;
    }

    return __result;
}

// ---------------------------------------------------------------------------
// open / close
// ---------------------------------------------------------------------------

function_result Am_Ui_Window_open_0(aobject *const this,
                                    short x, short y, unsigned short width, unsigned short height,
                                    aobject *screen, aobject *windowManager, struct Am_Ui_WindowProperties *properties)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (screen != NULL)        __increase_reference_count(screen);
    if (windowManager != NULL) __increase_reference_count(windowManager);
    (void) properties;

    Am_Ui_Window_data *data = win_data(this);
    if (data == NULL) goto __exit;

#ifdef AM_UI_LINUX_GTK
    // GTK shell owns the top-level window (frame + menu bar); SDL renders
    // into the embedded GtkDrawingArea's X11 window.
    {
        // SDL_CreateWindowFrom needs the video subsystem up. The pure-SDL
        // path got that via SDL_CreateWindow's implicit init; here we ask
        // explicitly (idempotent if Startup already did it).
        if (!SDL_WasInit(SDL_INIT_VIDEO)) {
            if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
                fprintf(stderr, "[am-ui/linux-gtk] SDL video init failed: %s\n", SDL_GetError());
            }
        }
        unsigned long xid = am_ui_gtk_build_shell(this, data, "amStudio", (int) x, (int) y, (int) width, (int) height);
        if (xid == 0) goto __exit;
        data->window = SDL_CreateWindowFrom((const void *) xid);
    }
#else
    // Position: AmigaOS uses absolute screen coords; on Linux let the
    // WM place it unless (x, y) is explicitly non-(-1, -1).
    int sdl_x = (x <= 0) ? SDL_WINDOWPOS_UNDEFINED : x;
    int sdl_y = (y <= 0) ? SDL_WINDOWPOS_UNDEFINED : y;

    Uint32 flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    data->window = SDL_CreateWindow(
        "amStudio",                  // title; setTitleNative overwrites later
        sdl_x, sdl_y,
        (int) width, (int) height,
        flags
    );
#endif
    if (data->window == NULL) {
        fprintf(stderr, "[am-ui/linux] window create failed: %s\n", SDL_GetError());
        goto __exit;
    }

    data->window_id = SDL_GetWindowID(data->window);
    // Renderer-driver pick. Two-phase logic:
    //   1. Honour SDL_RENDER_DRIVER env var if set (override priority on
    //      the hint so SDL_SetHintWithPriority can't be undercut).
    //   2. Otherwise, on builds where we know the only available
    //      accelerated backend has broken FBO readback (Adélie ppc/ppc64
    //      where Mesa GLES2 rejects glFramebufferTexture2D for our
    //      ARGB8888 offscreens — every cairo blit comes back all zeros),
    //      hint SDL to software. On linux-x64 / macos / amigaos-sim the
    //      OpenGL backend works fine.
    //
    // The old "OpenGL on macOS, default elsewhere" comment is below;
    // it's about Metal's broken read-after-write, still relevant on Apple
    // Silicon.
    {
        const char *env = getenv("SDL_RENDER_DRIVER");
        if (env != NULL && env[0] != '\0') {
            SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, env, SDL_HINT_OVERRIDE);
        } else {
#if defined(__powerpc__) || defined(__powerpc64__) || defined(__PPC__) || defined(__PPC64__)
            SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "software", SDL_HINT_OVERRIDE);
#else
            // Non-PPC: pin to opengl (matters on Apple Silicon — Metal's
            // render-to-target / sample-from-target same-frame races and
            // leaves the read all zeros).
            SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
#endif
        }
    }
    // PRESENTVSYNC is nice but we don't want to block paint cycles on
    // it — the IDE explicitly drives refresh, vsync would force a
    // wait that conflicts with the dirty-region model.
    data->renderer = SDL_CreateRenderer(
        data->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
    );
    if (data->renderer == NULL) {
        fprintf(stderr, "[am-ui/linux] SDL_CreateRenderer (accelerated) failed: %s — falling back to software\n", SDL_GetError());
        data->renderer = SDL_CreateRenderer(data->window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (data->renderer != NULL) {
        SDL_RendererInfo info;
        if (SDL_GetRendererInfo(data->renderer, &info) == 0) {
            fprintf(stderr, "[am-ui/macos-arm] SDL renderer: %s (flags=0x%x)\n", info.name, info.flags);
        }
    }
    if (data->renderer == NULL) {
        fprintf(stderr, "[am-ui/linux] SDL_CreateRenderer (software) failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(data->window);
        data->window = NULL;
        goto __exit;
    }

    SDL_GetRendererOutputSize(data->renderer, &data->last_paint_w, &data->last_paint_h);
    // Report the size to the AmLang side in LOGICAL pixels (physical /
    // HiDPI scale), so the View tree + offscreen are laid out at the same
    // scale GTK uses for its chrome and for input coordinates. The paint
    // tail stretches that offscreen back up to the physical window.
    {
        int s = (data->ui_scale > 0) ? data->ui_scale : 1;
        data->last_paint_w /= s;
        data->last_paint_h /= s;
    }

    // Publish for ViewContextGraphics.
    g_primary_renderer = data->renderer;

    data->event_pump_owner = true;   // first window claims the pump
    data->pending_close   = false;
    data->pending_refresh = true;    // ask for an initial full paint
    data->pending_resize  = false;
    data->installed_menu_strip = NULL;

    // Tell the AmLang side how much of the Window the WM consumed for
    // chrome. The Cocoa menu bar lives at the top of the *screen*, not
    // inside the window, so no border reservation is needed here.
    Am_Ui_Window_f_setBorder_0(this, 0, 0, 0, 0);
    // Trigger the first layout pass at the requested size.
    Am_Ui_Window_f_onResize_0(this, 0, 0, (unsigned short) data->last_paint_w, (unsigned short) data->last_paint_h);

__exit:
    if (windowManager != NULL) __decrease_reference_count(windowManager);
    if (screen != NULL)        __decrease_reference_count(screen);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Window_close_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);

    Am_Ui_Window_data *data = win_data(this);
    if (data != NULL) {
        if (g_primary_renderer == data->renderer) g_primary_renderer = NULL;
        if (data->renderer != NULL) { SDL_DestroyRenderer(data->renderer); data->renderer = NULL; }
        if (data->window != NULL)   { SDL_DestroyWindow(data->window);     data->window = NULL; }
#ifdef AM_UI_LINUX_GTK
        if (data->gtk_window != NULL) { gtk_widget_destroy(GTK_WIDGET(data->gtk_window)); data->gtk_window = NULL; data->gtk_menubar = NULL; data->gtk_draw_area = NULL; }
#endif
        data->pending_close = true;
        Am_Ui_Window_f_setRootView_0(this, NULL);
    }

    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Window_isOpen_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    Am_Ui_Window_data *data = win_data(this);
    bool is_open = (data != NULL) && (data->window != NULL) && !data->pending_close;
    __result.return_value.value.bool_value = is_open;
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// refresh — AmLang side asks for "please repaint soon" (analogous to
// IDCMP_REFRESHWINDOW). We just flip the pending_refresh flag; the
// actual Am_Ui_Window_f_paint_0 dispatch + SDL_RenderPresent fires at
// the end of the next handleInput cycle. This matches the AmigaOS
// model where the OS owns paint timing, not the AmLang caller.
// ---------------------------------------------------------------------------

function_result Am_Ui_Window_refresh_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_Window_data *data = win_data(this);
    if (data != NULL) {
        data->pending_refresh = true;
    }
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Cocoa NSMenu bridge
//
// AmLang's Window.setMenuStrip() stashes a MenuStrip on the Window;
// every iteration of handleInput we compare it against the strip we
// last installed and, on change, rebuild NSApp.mainMenu so the user
// sees real macOS menus at the top of the screen.
//
// All Cocoa calls go through the Objective-C runtime so this stays a
// plain `.c` file (no `.m` / amlc rebuild dance). The runtime header
// gives us objc_msgSend, sel_registerName, objc_getClass etc.; we
// cast objc_msgSend to the right signature on each call. Selectors
// are cached statically — looking them up is cheap but pointless to
// repeat per item.
//
// Menu picks are dispatched to a runtime-allocated AmLangMenuTarget
// class with a single -menuPicked: method. Each NSMenuItem carries
// the AmLang MenuItem aobject* as its `tag` (NSInteger fits a 64-bit
// pointer on arm64) so the handler can call Am_Ui_MenuItem_f_invokeClick_0
// straight away without a side table. The IDE's MenuItem aobjects
// live until the strip is rebuilt — we install a fresh NSApp.mainMenu
// at that point, which drops the stale NSMenuItems and their tags.
//
// AmLang MenuItem includes a private `subItems: List<MenuItem>` and
// some items use it (single level of cascading menus). We render
// those by recursing into a fresh NSMenu and parenting it under the
// item via -setSubmenu:; deeper nesting flattens — same caveat the
// amigaos backend documents.
//
// Items with `isSeparator = true` produce +separatorItem entries.
// Disabled items get -setEnabled:NO. Items with `commKey` non-null
// produce a Command-<key> shortcut via -setKeyEquivalent:.
//
// Apple-only: the whole bridge is gated on __APPLE__. On linux/SDL
// there's no OS menu bar (the AmLang side draws menus in-window), so
// am_ui_macos_arm_install_menu_strip() is a no-op stub at the bottom
// of this #ifdef block.
// ---------------------------------------------------------------------------
#ifdef __APPLE__

#include <objc/runtime.h>
#include <objc/message.h>

// NSInteger is Foundation's signed pointer-sized integer (long on
// 64-bit Darwin). Defining it locally avoids pulling in Foundation
// headers — they're Objective-C and would push us into .m territory.
typedef long NSInteger;

// Property indices used by the bridge. Declared here instead of via
// the Am/Ui/Menu*.h includes so the bridge stays self-contained — it
// only needs to *read* the properties, not link to any Menu helpers.
#define MENU_BRIDGE_P_MenuStrip_menus    0
#define MENU_BRIDGE_P_Menu_title         0
#define MENU_BRIDGE_P_Menu_items         1
#define MENU_BRIDGE_P_MenuItem_label     0
#define MENU_BRIDGE_P_MenuItem_enabled   2
#define MENU_BRIDGE_P_MenuItem_commKey   3
#define MENU_BRIDGE_P_MenuItem_subItems  4
#define MENU_BRIDGE_P_MenuItem_isSeparator 5

// AmLang collection list `get` / `getSize` symbols. Re-declared here
// at function-pointer cast sites to avoid a header dependency.
extern function_result Am_Collections_List_ta_Am_Ui_Menu_f_getSize_0(aobject *const this);
extern function_result Am_Collections_List_ta_Am_Ui_Menu_f_get_0(aobject *const this, int idx);
extern function_result Am_Collections_List_ta_Am_Ui_MenuItem_f_getSize_0(aobject *const this);
extern function_result Am_Collections_List_ta_Am_Lang_String_f_getSize_0(aobject *const this);
extern function_result Am_Collections_List_ta_Am_Lang_String_f_get_0(aobject *const this, int idx);
extern function_result Am_Collections_List_ta_Am_Ui_MenuItem_f_get_0(aobject *const this, int idx);
extern function_result Am_Ui_MenuItem_f_invokeClick_0(aobject *const this);

// Cached Cocoa selectors / classes. Initialised on first install.
static bool         g_menu_bridge_inited = false;
static Class        g_NSApplication = NULL;
static Class        g_NSMenu        = NULL;
static Class        g_NSMenuItem    = NULL;
static Class        g_NSString      = NULL;
static Class        g_AmLangMenuTarget = NULL;
static id           g_menu_target_instance = NULL;
static SEL          g_sel_sharedApp;
static SEL          g_sel_setMainMenu;
static SEL          g_sel_alloc;
static SEL          g_sel_init;
static SEL          g_sel_initWithTitle_action_keyEquivalent;
static SEL          g_sel_stringWithUTF8String;
static SEL          g_sel_addItem;
static SEL          g_sel_setSubmenu;
static SEL          g_sel_separatorItem;
static SEL          g_sel_setEnabled;
static SEL          g_sel_setAction;
static SEL          g_sel_setAutoenablesItems;
static SEL          g_sel_setTag;
static SEL          g_sel_tag;
static SEL          g_sel_setTarget;
static SEL          g_sel_menuPicked;
static SEL          g_sel_validateMenuItem;
static SEL          g_sel_release;

// Action callback the AmLangMenuTarget class dispatches every NSMenuItem
// pick to. Recovers the AmLang MenuItem aobject* from the sender's
// NSInteger tag and fires its clickListener.
static void am_menu_picked(id self, SEL _cmd, id sender)
{
    (void) self; (void) _cmd;
    fprintf(stderr, "[am-ui/macos-arm] menu picked: sender=%p\n", (void*) sender); fflush(stderr);
    NSInteger tag = ((NSInteger (*)(id, SEL)) objc_msgSend)(sender, g_sel_tag);
    aobject *menuItem = (aobject *) (intptr_t) tag;
    fprintf(stderr, "[am-ui/macos-arm]   tag=%ld -> aobject=%p\n", (long) tag, (void*) menuItem); fflush(stderr);
    if (menuItem == NULL) return;
    function_result fr = Am_Ui_MenuItem_f_invokeClick_0(menuItem);
    if (fr.exception != NULL) {
        fprintf(stderr, "[am-ui/macos-arm] menu click handler threw\n");
        __decrease_reference_count(fr.exception);
    }
}

static signed char am_validate_menu_item(id self, SEL _cmd, id item)
{
    (void) self; (void) _cmd;
    static int n = 0;
    if (n < 10) { fprintf(stderr, "[am-ui/macos-arm] validate item=%p\n", (void*) item); fflush(stderr); n++; }
    return 1;
}

static void menu_bridge_lazy_init(void)
{
    if (g_menu_bridge_inited) return;

    g_NSApplication = objc_getClass("NSApplication");
    g_NSMenu        = objc_getClass("NSMenu");
    g_NSMenuItem    = objc_getClass("NSMenuItem");
    g_NSString      = objc_getClass("NSString");

    g_sel_sharedApp        = sel_registerName("sharedApplication");
    g_sel_setMainMenu      = sel_registerName("setMainMenu:");
    g_sel_alloc            = sel_registerName("alloc");
    g_sel_init             = sel_registerName("init");
    g_sel_initWithTitle_action_keyEquivalent = sel_registerName("initWithTitle:action:keyEquivalent:");
    g_sel_stringWithUTF8String = sel_registerName("stringWithUTF8String:");
    g_sel_addItem          = sel_registerName("addItem:");
    g_sel_setSubmenu       = sel_registerName("setSubmenu:");
    g_sel_separatorItem    = sel_registerName("separatorItem");
    g_sel_setEnabled       = sel_registerName("setEnabled:");
    g_sel_setAction        = sel_registerName("setAction:");
    g_sel_setAutoenablesItems = sel_registerName("setAutoenablesItems:");
    g_sel_setTag           = sel_registerName("setTag:");
    g_sel_tag              = sel_registerName("tag");
    g_sel_setTarget        = sel_registerName("setTarget:");
    g_sel_menuPicked       = sel_registerName("menuPicked:");
    g_sel_validateMenuItem = sel_registerName("validateMenuItem:");
    g_sel_release          = sel_registerName("release");

    // Build the AmLangMenuTarget class on the fly. NSObject parent +
    // -menuPicked: (our action) + -validateMenuItem: (so Cocoa's auto-
    // enable check doesn't grey out every item the first time the
    // menu opens).
    g_AmLangMenuTarget = objc_allocateClassPair(objc_getClass("NSObject"), "AmLangMenuTarget", 0);
    if (g_AmLangMenuTarget != NULL) {
        bool ok1 = class_addMethod(g_AmLangMenuTarget, g_sel_menuPicked,       (IMP) am_menu_picked,        "v@:@");
        bool ok2 = class_addMethod(g_AmLangMenuTarget, g_sel_validateMenuItem, (IMP) am_validate_menu_item, "c@:@");
        objc_registerClassPair(g_AmLangMenuTarget);
        id alloc = ((id (*)(Class, SEL)) objc_msgSend)(g_AmLangMenuTarget, g_sel_alloc);
        g_menu_target_instance = ((id (*)(id, SEL)) objc_msgSend)(alloc, g_sel_init);
        fprintf(stderr, "[am-ui/macos-arm] menu bridge inited: target=%p addMethod(picked)=%d addMethod(validate)=%d\n",
            (void*) g_menu_target_instance, ok1, ok2); fflush(stderr);
    } else {
        fprintf(stderr, "[am-ui/macos-arm] FAILED to allocate AmLangMenuTarget class\n"); fflush(stderr);
    }

    g_menu_bridge_inited = true;
}

static id ns_string(const char *utf8)
{
    if (utf8 == NULL) utf8 = "";
    return ((id (*)(Class, SEL, const char *)) objc_msgSend)(g_NSString, g_sel_stringWithUTF8String, utf8);
}

static id new_ns_menu(const char *title)
{
    id alloc = ((id (*)(Class, SEL)) objc_msgSend)(g_NSMenu, g_sel_alloc);
    SEL initWithTitle = sel_registerName("initWithTitle:");
    id menu = ((id (*)(id, SEL, id)) objc_msgSend)(alloc, initWithTitle, ns_string(title));
    // Disable Cocoa's auto-enable. Cocoa's default polls every item's
    // target for `validateMenuItem:` (or walks the responder chain
    // looking for someone who answers the action selector) every time
    // the menu opens, and disables anything that doesn't answer YES.
    // Our items already carry the right enabled bit; let -setEnabled:
    // be the source of truth.
    ((void (*)(id, SEL, signed char)) objc_msgSend)(menu, g_sel_setAutoenablesItems, 0);
    return menu;
}

// Build an NSMenu from an AmLang Menu (or any container with an
// `items: List<MenuItem>` at the given property index). Recurses into
// subItems via -setSubmenu:.
static void build_ns_menu_items(id ns_menu, aobject *items_container, int items_property_index);

static void add_menu_item_to(id ns_menu, aobject *amlang_item)
{
    if (amlang_item == NULL) return;

    bool is_sep = __unwrap(amlang_item)->object_properties.class_object_properties.properties[MENU_BRIDGE_P_MenuItem_isSeparator].nullable_value.value.bool_value;
    if (is_sep) {
        id sep = ((id (*)(Class, SEL)) objc_msgSend)(g_NSMenuItem, g_sel_separatorItem);
        ((void (*)(id, SEL, id)) objc_msgSend)(ns_menu, g_sel_addItem, sep);
        return;
    }

    aobject *label_obj = __unwrap(amlang_item)->object_properties.class_object_properties.properties[MENU_BRIDGE_P_MenuItem_label].nullable_value.value.object_value;
    const char *label = amlang_str(label_obj);

    aobject *commKey_obj = __unwrap(amlang_item)->object_properties.class_object_properties.properties[MENU_BRIDGE_P_MenuItem_commKey].nullable_value.value.object_value;
    const char *commKey = amlang_str(commKey_obj);
    // commKey is single-char on AmLang; if it's longer, use the first
    // char. Empty/NULL → no shortcut.
    char keq_buf[2] = { 0, 0 };
    if (commKey != NULL && commKey[0] != '\0') {
        keq_buf[0] = commKey[0];
    }

    bool enabled = __unwrap(amlang_item)->object_properties.class_object_properties.properties[MENU_BRIDGE_P_MenuItem_enabled].nullable_value.value.bool_value;

    // Check whether this item has sub-items; if so we want a parent
    // item with a submenu (no action) rather than a clickable one.
    aobject *subs = __unwrap(amlang_item)->object_properties.class_object_properties.properties[MENU_BRIDGE_P_MenuItem_subItems].nullable_value.value.object_value;
    int sub_count = 0;
    if (subs != NULL) {
        function_result fr = Am_Collections_List_ta_Am_Ui_MenuItem_f_getSize_0(subs);
        if (fr.exception != NULL) { __decrease_reference_count(fr.exception); }
        else sub_count = (int) fr.return_value.value.int_value;
    }

    id title = ns_string(label ? label : "");
    id keq = ns_string(keq_buf);

    // Init with no action — set target/action explicitly afterwards so
    // the SEL value isn't laundered through `id` casts that have bitten
    // us when passing variadic-ish parameters through objc_msgSend on
    // arm64.
    id alloc = ((id (*)(Class, SEL)) objc_msgSend)(g_NSMenuItem, g_sel_alloc);
    id item  = ((id (*)(id, SEL, id, SEL, id)) objc_msgSend)(alloc, g_sel_initWithTitle_action_keyEquivalent, title, (SEL) NULL, keq);

    if (sub_count > 0) {
        id submenu = new_ns_menu(label ? label : "");
        build_ns_menu_items(submenu, amlang_item, MENU_BRIDGE_P_MenuItem_subItems);
        ((void (*)(id, SEL, id)) objc_msgSend)(item, g_sel_setSubmenu, submenu);
    } else {
        // Stash the AmLang aobject* pointer so the click handler can
        // recover it from the sender's tag.
        ((void (*)(id, SEL, NSInteger)) objc_msgSend)(item, g_sel_setTag, (NSInteger) (intptr_t) amlang_item);
        ((void (*)(id, SEL, id)) objc_msgSend)(item, g_sel_setTarget, g_menu_target_instance);
        ((void (*)(id, SEL, SEL)) objc_msgSend)(item, g_sel_setAction, g_sel_menuPicked);
    }

    if (!enabled) {
        ((void (*)(id, SEL, signed char)) objc_msgSend)(item, g_sel_setEnabled, 0);
    }

    ((void (*)(id, SEL, id)) objc_msgSend)(ns_menu, g_sel_addItem, item);
}

static void build_ns_menu_items(id ns_menu, aobject *items_container, int items_property_index)
{
    aobject *items = __unwrap(items_container)->object_properties.class_object_properties.properties[items_property_index].nullable_value.value.object_value;
    if (items == NULL) return;
    function_result fr = Am_Collections_List_ta_Am_Ui_MenuItem_f_getSize_0(items);
    if (fr.exception != NULL) { __decrease_reference_count(fr.exception); return; }
    int n = (int) fr.return_value.value.int_value;
    for (int i = 0; i < n; i++) {
        function_result gr = Am_Collections_List_ta_Am_Ui_MenuItem_f_get_0(items, i);
        if (gr.exception != NULL) { __decrease_reference_count(gr.exception); continue; }
        aobject *amlang_item = gr.return_value.value.object_value;
        add_menu_item_to(ns_menu, amlang_item);
    }
}

void am_ui_macos_arm_install_menu_strip(aobject *menuStrip)
{
    menu_bridge_lazy_init();
    if (g_NSApplication == NULL || g_NSMenu == NULL) return;

    id app = ((id (*)(Class, SEL)) objc_msgSend)(g_NSApplication, g_sel_sharedApp);
    if (app == NULL) return;

    // A bare top-level NSMenu acts as the menu bar; each top-level
    // NSMenuItem holds its dropdown menu as a submenu. Cocoa convention
    // is to put the "application" menu first (the bold one that
    // matches the app name) — for now we just pass through the AmLang
    // menus in order; the first one becomes the app menu visually.
    id main_menu = new_ns_menu("");
    if (menuStrip != NULL) {
        aobject *menus = __unwrap(menuStrip)->object_properties.class_object_properties.properties[MENU_BRIDGE_P_MenuStrip_menus].nullable_value.value.object_value;
        if (menus != NULL) {
            function_result fr = Am_Collections_List_ta_Am_Ui_Menu_f_getSize_0(menus);
            if (fr.exception != NULL) { __decrease_reference_count(fr.exception); }
            else {
                int n = (int) fr.return_value.value.int_value;
                for (int i = 0; i < n; i++) {
                    function_result gr = Am_Collections_List_ta_Am_Ui_Menu_f_get_0(menus, i);
                    if (gr.exception != NULL) { __decrease_reference_count(gr.exception); continue; }
                    aobject *amlang_menu = gr.return_value.value.object_value;
                    if (amlang_menu == NULL) continue;

                    aobject *title_obj = __unwrap(amlang_menu)->object_properties.class_object_properties.properties[MENU_BRIDGE_P_Menu_title].nullable_value.value.object_value;
                    const char *title = amlang_str(title_obj);

                    // Top-level NSMenuItem hosting the dropdown.
                    id top_item_alloc = ((id (*)(Class, SEL)) objc_msgSend)(g_NSMenuItem, g_sel_alloc);
                    id empty_key = ns_string("");
                    id top_item = ((id (*)(id, SEL, id, SEL, id)) objc_msgSend)(top_item_alloc, g_sel_initWithTitle_action_keyEquivalent, ns_string(title ? title : ""), (SEL) NULL, empty_key);

                    id dropdown = new_ns_menu(title ? title : "");
                    build_ns_menu_items(dropdown, amlang_menu, MENU_BRIDGE_P_Menu_items);
                    ((void (*)(id, SEL, id)) objc_msgSend)(top_item, g_sel_setSubmenu, dropdown);

                    ((void (*)(id, SEL, id)) objc_msgSend)(main_menu, g_sel_addItem, top_item);
                }
            }
        }
    }

    ((void (*)(id, SEL, id)) objc_msgSend)(app, g_sel_setMainMenu, main_menu);
}

#else  /* !__APPLE__ — linux/SDL: no OS menu bar */

// No native menu bar on linux/SDL. handleInput still calls this on
// menu-strip change; it's a no-op (menus are rendered in-window by
// the AmLang side).
void am_ui_macos_arm_install_menu_strip(aobject *menuStrip) { (void) menuStrip; }

#endif  /* __APPLE__ */


// ---------------------------------------------------------------------------
// handleInput — pump SDL events and dispatch to the AmLang side.
//
// This corresponds to the IDCMP event drain in amigaos/Window.c. We
// run a SDL_PollEvent loop (non-blocking) and for each event call the
// matching Am_Ui_Window_f_* callback the AmLang side defines. When
// the user closes the window we flip pending_close so the next
// isOpen check returns false.
//
// Multi-window dispatch: SDL events carry a window_id. We only
// dispatch events whose window_id matches THIS Window; events for
// other Windows are re-queued via SDL_PushEvent so the other
// Window's handleInput picks them up. This means the IDE's
// "main + dialogs" topology works without a separate event router.
// ---------------------------------------------------------------------------

function_result Am_Ui_Window_handleInput_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    Am_Ui_Window_data *data = win_data(this);
    if (data == NULL || data->window == NULL) goto __exit;

    // Cocoa NSMenu install / refresh. Window.setMenuStrip() is a
    // plain AmLang setter on a property; we can't intercept it from
    // native code, so we poll the property here once per input
    // pump. Comparing the pointer dedupes — we only rebuild when the
    // strip actually changes (boot, workspace switch, etc.). NULL
    // strip means "no menus", which is exactly what the splash window
    // wants — the bridge clears NSApp.mainMenu in that case.
#ifndef AM_UI_LINUX_GTK
    // (pure-SDL / Cocoa path) Poll the menuStrip property and rebuild the
    // native menu on change. The GTK build instead populates a real
    // GtkMenuBar directly from setMenuStrip's nativeAddMenu* calls.
    aobject *current_strip = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Window_P_menuStrip].nullable_value.value.object_value;
    if ((void *) current_strip != data->installed_menu_strip) {
        am_ui_macos_arm_install_menu_strip(current_strip);
        data->installed_menu_strip = (void *) current_strip;
    }
#endif

#ifdef AM_UI_LINUX_GTK
    // GTK owns the event loop: pump it non-blocking. The signal handlers
    // dispatch input into the Am_Ui_Window_f_* callbacks, and menu picks
    // run here too. SDL_PollEvent is NOT used — SDL doesn't own the
    // (foreign, GTK-created) window so its event queue stays empty.
    while (gtk_events_pending()) gtk_main_iteration_do(FALSE);
#else
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        // Filter: only dispatch events for this Window's id when the
        // event carries one. SDL_QUIT has no id and is a "close
        // everything" signal — treat it as a close on this Window.
        bool for_us = true;
        Uint32 evt_window_id = 0;
        switch (ev.type) {
        case SDL_WINDOWEVENT:        evt_window_id = ev.window.windowID;  break;
        case SDL_MOUSEMOTION:        evt_window_id = ev.motion.windowID;  break;
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:      evt_window_id = ev.button.windowID;  break;
        case SDL_MOUSEWHEEL:         evt_window_id = ev.wheel.windowID;   break;
        case SDL_KEYDOWN:
        case SDL_KEYUP:              evt_window_id = ev.key.windowID;     break;
        case SDL_TEXTINPUT:          evt_window_id = ev.text.windowID;    break;
        default: break;
        }
        if (evt_window_id != 0 && evt_window_id != data->window_id) {
            for_us = false;
        }
        if (!for_us) {
            // Event is for a different Window. Re-push so that Window's
            // handleInput picks it up — unless the target window is no
            // longer alive (closed splash, etc.), in which case the
            // event would re-queue itself forever and this handleInput
            // would never drain. SDL_GetWindowFromID returns NULL for
            // a destroyed window id, so we drop those.
            if (SDL_GetWindowFromID(evt_window_id) != NULL) {
                SDL_PushEvent(&ev);
            }
            continue;
        }

        switch (ev.type) {
        case SDL_QUIT:
            data->pending_close = true;
            break;

        case SDL_WINDOWEVENT:
            if (ev.window.event == SDL_WINDOWEVENT_CLOSE) {
                data->pending_close = true;
            } else if (ev.window.event == SDL_WINDOWEVENT_RESIZED
                    || ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                int w = 0, h = 0;
                SDL_GetRendererOutputSize(data->renderer, &w, &h);
                if (w != data->last_paint_w || h != data->last_paint_h) {
                    data->last_paint_w = w;
                    data->last_paint_h = h;
                    Am_Ui_Window_f_onResize_0(this, 0, 0, (unsigned short) w, (unsigned short) h);
                }
            } else if (ev.window.event == SDL_WINDOWEVENT_EXPOSED) {
                data->pending_refresh = true;
            }
            break;

        case SDL_MOUSEMOTION: {
            data->last_mouse_x = (short) ev.motion.x;
            data->last_mouse_y = (short) ev.motion.y;
            // MouseEventType.move = 1, MouseButton.none = 0.
            Am_Ui_Window_f_onMouseEvent_0(this, 1, 0,
                (short) ev.motion.x, (short) ev.motion.y);
            break;
        }

        case SDL_MOUSEBUTTONDOWN: {
            unsigned char btn = map_sdl_button(ev.button.button);
            if (btn != 0) {
                // MouseEventType.down = 3.
                Am_Ui_Window_f_onMouseEvent_0(this, 3, btn,
                    (short) ev.button.x, (short) ev.button.y);
            }
            break;
        }
        case SDL_MOUSEBUTTONUP: {
            unsigned char btn = map_sdl_button(ev.button.button);
            if (btn != 0) {
                // MouseEventType.up = 2.
                Am_Ui_Window_f_onMouseEvent_0(this, 2, btn,
                    (short) ev.button.x, (short) ev.button.y);
            }
            break;
        }

        case SDL_MOUSEWHEEL: {
            // SDL_MOUSEWHEEL has its own x/y (the *scroll* deltas) but
            // not cursor coords; reuse the last seen cursor pos.
            Am_Ui_Window_f_onMouseWheel_0(this,
                (short) ev.wheel.y,
                data->last_mouse_x, data->last_mouse_y);
            break;
        }

        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            // Mirror AmigaOS Window.c's IDCMP_RAWKEY mapping: AmIde's
            // editor / text fields check specific AmigaOS raw key codes
            // (left=79, right=78, up=76, down=77, backspace=65, delete=70,
            // tab=66, return=68, escape=69) and ASCII control codes in
            // keyChar (Shift+arrow=1, Ctrl+letter=ctrl_code, Backspace=8,
            // Delete=127, Tab=9, Return=13, Escape=27). Cmd-combos are
            // owned by the Cocoa menu bar; skip them so a menu shortcut
            // doesn't double-fire into the editor.
            int type = (ev.type == SDL_KEYDOWN) ? 1 : 2;  // 1=down, 2=up
            SDL_Keycode sym = ev.key.keysym.sym;
            Uint16 mod = ev.key.keysym.mod;
            if (mod & KMOD_GUI) break;
            bool shift = (mod & KMOD_SHIFT) != 0;
            bool ctrl  = (mod & KMOD_CTRL)  != 0;

            int amiga_code = 0;
            int amiga_char = 0;
            switch (sym) {
                case SDLK_BACKSPACE: amiga_code = 65; amiga_char = 8;   break;
                case SDLK_DELETE:    amiga_code = 70; amiga_char = 127; break;
                case SDLK_LEFT:      amiga_code = 79; amiga_char = shift ? 1 : 0; break;
                case SDLK_RIGHT:     amiga_code = 78; amiga_char = shift ? 1 : 0; break;
                case SDLK_UP:        amiga_code = 76; amiga_char = shift ? 1 : 0; break;
                case SDLK_DOWN:      amiga_code = 77; amiga_char = shift ? 1 : 0; break;
                case SDLK_TAB:       amiga_code = 66; amiga_char = shift ? 0 : 9;   break;
                case SDLK_RETURN:    amiga_code = 68; amiga_char = 13;  break;
                case SDLK_KP_ENTER:  amiga_code = 68; amiga_char = 13;  break;
                case SDLK_ESCAPE:    amiga_code = 69; amiga_char = 27;  break;
                default:
                    // Ctrl+letter → ASCII control code (Ctrl+A=1 .. Ctrl+Z=26).
                    // Printable keys without Ctrl get handled by SDL_TEXTINPUT
                    // so the Shift / IME-applied character makes it through;
                    // dispatching here too would double-fire.
                    if (ctrl && sym >= SDLK_a && sym <= SDLK_z) {
                        amiga_code = (int) sym;
                        amiga_char = (int) (sym - SDLK_a + 1);
                    }
                    break;
            }
            if (amiga_code != 0 || amiga_char != 0) {
                Am_Ui_Window_f_onKeyboardEvent_0(this, type,
                    (unsigned short) amiga_code, (unsigned short) amiga_char);
            }
            break;
        }

        case SDL_TEXTINPUT: {
            // Carries the post-modifier character (e.g. uppercase from
            // Shift, language layout from IME). Skip if a Ctrl/Cmd combo
            // is held — those are routed through SDL_KEYDOWN (for Ctrl)
            // or the Cocoa menu (for Cmd). Multi-byte UTF-8 chars are
            // truncated to their first byte for now; AmIde's text path
            // uses ASCII-range keyChar today, broader Unicode is TODO.
            Uint16 mod = SDL_GetModState();
            if (mod & (KMOD_CTRL | KMOD_GUI)) break;
            unsigned char ch = (unsigned char) ev.text.text[0];
            if (ch >= 32 && ch <= 126) {
                Am_Ui_Window_f_onKeyboardEvent_0(this, 1, 0, (unsigned short) ch);
            }
            break;
        }

        default:
            break;
        }
    }
#endif // !AM_UI_LINUX_GTK (SDL event loop)

    // End-of-cycle paint dispatch. Mirrors the amigaos backend's
    // pending_refresh / pending_full_refresh check at the tail of
    // handleInput: if AmLang flipped the flag (via refresh() from
    // requestRepaint), call back into Window.paint() which walks the
    // View tree, lets each view draw into the off-screen LayerGraphics,
    // and flushes the dirty bounds back into this renderer. Then
    // SDL_RenderPresent makes the result visible. Without this dispatch
    // the SDL renderer fills with whatever black clear color SDL gives
    // a fresh window and stays there — no AmLang view ever paints.
    if (data->pending_refresh) {
        data->pending_refresh = false;
        function_result paint_res = Am_Ui_Window_f_paint_0(this);
        if (paint_res.exception != NULL) {
            // Log paint exceptions so we don't silently swallow View-tree
            // bugs and stare at a blank window. The message lives at the
            // canonical Am.Lang.Exception property layout — pull it out
            // safely so the print itself doesn't strlen NULL.
            aobject *msg_obj = __unwrap(paint_res.exception)->object_properties.class_object_properties.properties[Am_Lang_Exception_P_message].nullable_value.value.object_value;
            const char *msg = "(no msg)";
            if (msg_obj != NULL) {
                string_holder *sh = (string_holder *) (msg_obj + 1);
                if (sh != NULL && sh->string_value != NULL) msg = sh->string_value;
            }
            // Only print once per session — paint exceptions can fire
            // many times a second once one widget's paint() is broken,
            // and that floods the terminal AND drowns out the click
            // log that we used to diagnose menu routing.
            static int paint_throw_n = 0;
            if (paint_throw_n < 1) {
                fprintf(stderr, "[am-ui/macos-arm] f_paint_0 threw for win_id=%u: %s\n", (unsigned) data->window_id, msg);
                // Walk the stack trace list (Am.Lang.Exception holds it
                // as List<String>, not a String — casting the List
                // straight to string_holder is what segfaulted my last
                // attempt to print it). Pull each frame via the typed
                // List accessor.
                aobject *st_list = __unwrap(paint_res.exception)->object_properties.class_object_properties.properties[Am_Lang_Exception_P_stackTrace].nullable_value.value.object_value;
                if (st_list != NULL) {
                    function_result n_fr = Am_Collections_List_ta_Am_Lang_String_f_getSize_0(st_list);
                    if (n_fr.exception == NULL) {
                        int n = (int) n_fr.return_value.value.int_value;
                        for (int i = 0; i < n && i < 30; i++) {
                            function_result f_fr = Am_Collections_List_ta_Am_Lang_String_f_get_0(st_list, i);
                            if (f_fr.exception != NULL) { __decrease_reference_count(f_fr.exception); continue; }
                            aobject *frame = f_fr.return_value.value.object_value;
                            const char *fs = "(null frame)";
                            if (frame != NULL) {
                                string_holder *fh = (string_holder *) (frame + 1);
                                if (fh != NULL && fh->string_value != NULL) fs = fh->string_value;
                            }
                            fprintf(stderr, "[am-ui/macos-arm]   #%d %s\n", i, fs);
                        }
                    } else {
                        __decrease_reference_count(n_fr.exception);
                    }
                }
                paint_throw_n++;
            }
            __decrease_reference_count(paint_res.exception);
        }
        if (data->renderer != NULL) {
#ifdef AM_UI_LINUX_GTK
            // Cairo-present path: the View tree has just rendered into the SDL
            // offscreen texture. Don't blit/Present to the SDL window (that
            // foreign X window spans the whole frame and rode content under the
            // chrome). Instead ask GTK to redraw the drawing area — gtk_on_draw
            // reads the offscreen back and paints it through Cairo, so GTK owns
            // placement + HiDPI scale and content lines up with input.
            if (data->gtk_draw_area != NULL) {
                gtk_widget_queue_draw(GTK_WIDGET(data->gtk_draw_area));
                while (gtk_events_pending()) gtk_main_iteration_do(FALSE);
            }
#else
            // Pure-SDL path: re-stamp the whole offscreen onto the window back
            // buffer (it's persistent; the back buffer isn't) and present.
            SDL_SetRenderTarget(data->renderer, NULL);
            am_ui_macos_arm_lg_target_changed(data->renderer, NULL);
            SDL_RenderSetClipRect(data->renderer, NULL);
            aobject *off_rb = __unwrap(this)->object_properties.class_object_properties.properties[Am_Ui_Window_P_offscreen].nullable_value.value.object_value;
            if (off_rb != NULL) {
                aobject *off_bm = __unwrap(off_rb)->object_properties.class_object_properties.properties[Am_Ui_RenderableBitmap_P_bitmap].nullable_value.value.object_value;
                if (off_bm != NULL) {
                    Am_Ui_Bitmap_data *bd = (Am_Ui_Bitmap_data *) __unwrap(off_bm)->object_properties.class_object_properties.object_data.value.custom_value;
                    if (bd != NULL && bd->texture != NULL) {
                        int out_w = 0, out_h = 0;
                        SDL_GetRendererOutputSize(data->renderer, &out_w, &out_h);
                        int tex_w = out_w, tex_h = out_h;
                        SDL_QueryTexture(bd->texture, NULL, NULL, &tex_w, &tex_h);
                        int src_w = (data->ui_scale > 1) ? (out_w / data->ui_scale) : tex_w;
                        int src_h = (data->ui_scale > 1) ? (out_h / data->ui_scale) : tex_h;
                        SDL_Rect src = { 0, 0, src_w, src_h };
                        SDL_Rect dst = { 0, 0, out_w, out_h };
                        SDL_RenderCopy(data->renderer, bd->texture, &src, &dst);
                    }
                }
            }
            SDL_RenderPresent(data->renderer);
#endif
        }
    }

__exit:
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Geometry queries against the host display.
// ---------------------------------------------------------------------------

function_result Am_Ui_Window_getHostScreenWidth_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    SDL_Rect bounds = { 0, 0, 0, 0 };
    SDL_GetDisplayBounds(0, &bounds);
    int wv = bounds.w;
#ifdef AM_UI_LINUX_GTK
    wv /= am_ui_gtk_scale_factor();   // report logical pixels (GTK scales up)
#endif
    __result.return_value.value.ushort_value = (unsigned short) wv;
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Window_getHostScreenHeight_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    SDL_Rect bounds = { 0, 0, 0, 0 };
    SDL_GetDisplayBounds(0, &bounds);
    int hv = bounds.h;
#ifdef AM_UI_LINUX_GTK
    hv /= am_ui_gtk_scale_factor();   // report logical pixels (GTK scales up)
#endif
    __result.return_value.value.ushort_value = (unsigned short) hv;
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// AmigaOS exec.library signal mailbox — no Linux equivalent.
// Callers (the IDE's TaskScheduler) treat 0 / NULL as "no async signal
// surface available" and fall back to polling via handleInput.
// ---------------------------------------------------------------------------

function_result Am_Ui_Window_getUserPortSigBit_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    (void) this;
    __result.return_value.value.int_value = 0;
    return __result;
}

function_result Am_Ui_Window_getUserPortTaskPtr_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    (void) this;
    __result.return_value.value.long_value = 0;
    return __result;
}

// ---------------------------------------------------------------------------
// Title — SDL_SetWindowTitle.
// ---------------------------------------------------------------------------

function_result Am_Ui_Window_setTitleNative_0(aobject *const this, aobject *windowTitle, aobject *screenTitle)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (windowTitle != NULL) __increase_reference_count(windowTitle);
    if (screenTitle != NULL) __increase_reference_count(screenTitle);

    Am_Ui_Window_data *data = win_data(this);
    const char *title = amlang_str(windowTitle);
    if (data != NULL && title != NULL) {
#ifdef AM_UI_LINUX_GTK
        if (data->gtk_window != NULL) gtk_window_set_title(GTK_WINDOW(data->gtk_window), title);
#else
        if (data->window != NULL) SDL_SetWindowTitle(data->window, title);
#endif
    }
    // screenTitle is a no-op on Linux; on AmigaOS it's the title bar
    // for the host screen the window sits in.
    (void) screenTitle;

    if (screenTitle != NULL) __decrease_reference_count(screenTitle);
    if (windowTitle != NULL) __decrease_reference_count(windowTitle);
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Clipboard — SDL2 has native primitives but pasteFromClipboard needs
// to allocate an AmLang String aobject which is more boilerplate than
// this stub session has room for. TODO(linux).
// ---------------------------------------------------------------------------

function_result Am_Ui_Window_copyToClipboard_0(aobject *const this, aobject *text)
{
    function_result __result = { .has_return_value = false };
    __increase_reference_count(this);
    if (text != NULL) __increase_reference_count(text);
    const char *t = amlang_str(text);
    if (t != NULL) {
        SDL_SetClipboardText(t);
    }
    if (text != NULL) __decrease_reference_count(text);
    __decrease_reference_count(this);
    return __result;
}

function_result Am_Ui_Window_pasteFromClipboard_0(aobject *const this)
{
    function_result __result = { .has_return_value = true };
    __increase_reference_count(this);
    // TODO(linux): build an AmLang String from SDL_GetClipboardText().
    // Returning NULL for now — callers treat null as "nothing to paste".
    __result.return_value.value.object_value = NULL;
    __decrease_reference_count(this);
    return __result;
}

// ---------------------------------------------------------------------------
// Menu strip. On the GTK build these populate the real GtkMenuBar from
// Window.setMenuStrip's nativeAddMenu* calls (item activate →
// MenuItem.invokeClick). On the pure-SDL build they're no-ops (no native
// menu bar — an in-canvas bar would be drawn by the AmLang View tree).
// ---------------------------------------------------------------------------

#ifdef AM_UI_LINUX_GTK

static GtkWidget *gtk_make_leaf(aobject *win_this, aobject *item, aobject *label)
{
    if (item_bool(item, AMUI_MENUITEM_P_isSeparator, false)) {
        return gtk_separator_menu_item_new();
    }
    const char *lbl = amlang_str(label);
    GtkWidget *mi = gtk_menu_item_new_with_label(lbl ? lbl : "");
    g_object_set_data(G_OBJECT(mi), "amlang_item", item);
    g_object_set_data(G_OBJECT(mi), "amlang_window", win_this);
    g_signal_connect(mi, "activate", G_CALLBACK(gtk_on_menu_activate), NULL);
    gtk_widget_set_sensitive(mi, item_bool(item, AMUI_MENUITEM_P_enabled, true));
    return mi;
}

static void gtk_clear_menubar(Am_Ui_Window_data *data)
{
    if (data == NULL || data->gtk_menubar == NULL) return;
    GList *kids = gtk_container_get_children(GTK_CONTAINER(data->gtk_menubar));
    for (GList *l = kids; l != NULL; l = l->next) gtk_widget_destroy(GTK_WIDGET(l->data));
    g_list_free(kids);
    data->gtk_menu_count = 0;
}

function_result Am_Ui_Window_nativeBeginMenuStrip_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    gtk_clear_menubar(win_data(this));
    return __result;
}

function_result Am_Ui_Window_nativeAddMenu_0(aobject *const this, aobject *title)
{
    function_result __result = { .has_return_value = false };
    Am_Ui_Window_data *data = win_data(this);
    if (data == NULL || data->gtk_menubar == NULL || data->gtk_menu_count >= 32) return __result;
    const char *t = amlang_str(title);
    GtkWidget *top  = gtk_menu_item_new_with_label(t ? t : "");
    GtkWidget *menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(top), menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(data->gtk_menubar), top);
    int idx = data->gtk_menu_count++;
    data->gtk_menus[idx]     = menu;
    data->gtk_last_item[idx] = NULL;
    return __result;
}

function_result Am_Ui_Window_nativeAddMenuItem_0(aobject *const this, int menuIndex, aobject *item, aobject *label, aobject *commKey)
{
    function_result __result = { .has_return_value = false };
    (void) commKey;   // TODO: Ctrl+key accelerators via a GtkAccelGroup
    Am_Ui_Window_data *data = win_data(this);
    if (data == NULL || menuIndex < 0 || menuIndex >= data->gtk_menu_count) return __result;
    GtkWidget *menu = (GtkWidget *) data->gtk_menus[menuIndex];
    if (menu == NULL) return __result;
    GtkWidget *mi = gtk_make_leaf(this, item, label);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), mi);
    data->gtk_last_item[menuIndex] = mi;
    return __result;
}

function_result Am_Ui_Window_nativeAddMenuSubItem_0(aobject *const this, int menuIndex, aobject *item, aobject *label, aobject *commKey)
{
    function_result __result = { .has_return_value = false };
    (void) commKey;
    Am_Ui_Window_data *data = win_data(this);
    if (data == NULL || menuIndex < 0 || menuIndex >= data->gtk_menu_count) return __result;
    GtkWidget *parent = (GtkWidget *) data->gtk_last_item[menuIndex];
    if (parent == NULL) return __result;
    // Lazily give the parent item a submenu the first time a sub-item lands.
    GtkWidget *submenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(parent));
    if (submenu == NULL) {
        submenu = gtk_menu_new();
        gtk_menu_item_set_submenu(GTK_MENU_ITEM(parent), submenu);
    }
    GtkWidget *mi = gtk_make_leaf(this, item, label);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), mi);
    return __result;
}

function_result Am_Ui_Window_nativeFinalizeMenuStrip_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    Am_Ui_Window_data *data = win_data(this);
    if (data != NULL && data->gtk_menubar != NULL) gtk_widget_show_all(GTK_WIDGET(data->gtk_menubar));
    return __result;
}

function_result Am_Ui_Window_nativeClearMenuStrip_0(aobject *const this)
{
    function_result __result = { .has_return_value = false };
    gtk_clear_menubar(win_data(this));
    return __result;
}

#else  // pure-SDL: no native menu bar

function_result Am_Ui_Window_nativeBeginMenuStrip_0(aobject *const this)
{ function_result r = { .has_return_value = false }; (void) this; return r; }
function_result Am_Ui_Window_nativeAddMenu_0(aobject *const this, aobject *title)
{ function_result r = { .has_return_value = false }; (void) this; (void) title; return r; }
function_result Am_Ui_Window_nativeAddMenuItem_0(aobject *const this, int menuIndex, aobject *item, aobject *label, aobject *commKey)
{ function_result r = { .has_return_value = false }; (void) this; (void) menuIndex; (void) item; (void) label; (void) commKey; return r; }
function_result Am_Ui_Window_nativeAddMenuSubItem_0(aobject *const this, int menuIndex, aobject *item, aobject *label, aobject *commKey)
{ function_result r = { .has_return_value = false }; (void) this; (void) menuIndex; (void) item; (void) label; (void) commKey; return r; }
function_result Am_Ui_Window_nativeFinalizeMenuStrip_0(aobject *const this)
{ function_result r = { .has_return_value = false }; (void) this; return r; }
function_result Am_Ui_Window_nativeClearMenuStrip_0(aobject *const this)
{ function_result r = { .has_return_value = false }; (void) this; return r; }

#endif // AM_UI_LINUX_GTK (menu strip)

#endif
