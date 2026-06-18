#ifndef native_linux_aclass_Am_Ui_Screen_h
#define native_linux_aclass_Am_Ui_Screen_h

#include <libc/core.h>
#include <Am/Ui/Screen.h>

#include <SDL2/SDL.h>

#define AM_UI_LINUX_SCREEN_PALETTE_SIZE 32

// Native data for Am.Ui.Screen on Linux.
//
// Linux WMs don't let an app open a custom display mode the way
// AmigaOS does, so Screen is a process-wide palette + display-
// bounds metadata cache rather than a native handle. The first
// openScreen call allocates this struct; subsequent calls return
// the same instance (the global pointer at file scope inside
// Screen.c).
//
// `palette` is the 32-entry ARGB lookup table that pen-based
// drawing resolves against. setColor / setPalette write to it;
// LayerGraphics reads it at paint time. AmigaOS pushes this to
// SetRGB32; on Linux there is no hardware register to push it
// to — the table IS the truth.
typedef struct _Am_Ui_Screen_data Am_Ui_Screen_data;
struct _Am_Ui_Screen_data {
    Uint32 palette[AM_UI_LINUX_SCREEN_PALETTE_SIZE];

    // Display bounds at openScreen-time, captured from
    // SDL_GetDisplayBounds(0). Re-queried only by an explicit
    // ScreenModeRequester call later — we don't follow a monitor
    // hot-plug, the IDE is one process / one window anyway.
    int    display_w;
    int    display_h;
};

// Accessor used by LayerGraphics to pull the palette pointer without
// knowing the struct layout. NULL when no Screen has been opened
// yet — paint paths fall back to a hardcoded default palette in
// that case.
const Uint32 * am_ui_linux_screen_palette(void);
int            am_ui_linux_screen_palette_count(void);

#endif
