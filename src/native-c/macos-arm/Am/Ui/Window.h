#ifndef native_linux_aclass_Am_Ui_Window_h
#define native_linux_aclass_Am_Ui_Window_h

#include <libc/core.h>
#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/Int.h>

#include <SDL2/SDL.h>
#include <stdbool.h>

// Native backing data for Am.Ui.Window on Linux (SDL2 backend).
//
// One SDL_Window + SDL_Renderer per AmLang Window. The renderer is
// kept around for its lifetime so LayerGraphics can draw straight
// to it without re-creating per paint cycle. `last_paint_size_*`
// caches the renderer's pixel size so a resize event can be
// distinguished from a vanilla redraw request.
//
// `event_pump_owner` is true on the Window that's responsible for
// calling SDL_PollEvent each handleInput() — first-window-opened
// claims it. Other Windows route their input by SDL_GetWindowID
// dispatch from that pump.
typedef struct _Am_Ui_Window_data Am_Ui_Window_data;
struct _Am_Ui_Window_data {
    SDL_Window   *window;
    SDL_Renderer *renderer;
    Uint32        window_id;

    // Cached event-loop state. handleInput() pulls SDL events and
    // either drives the View tree directly (mouse/keyboard) or
    // flips one of these flags for the AmLang side to react to on
    // the next refresh cycle.
    bool          pending_close;
    bool          pending_refresh;
    bool          pending_resize;

    // Last known mouse position. Mirrors AmigaOS's last_mouse_*
    // fields so a resize / focus-restore can synthesise a move
    // without having to re-query SDL.
    short         last_mouse_x;
    short         last_mouse_y;

    int           last_paint_w;
    int           last_paint_h;

    // True iff this Window owns the SDL_PollEvent loop. Set on
    // first open. Other Windows piggy-back off whoever has it.
    bool          event_pump_owner;

    // Pointer to the AmLang `menuStrip` we last installed into NSApp's
    // mainMenu — rebuilding the Cocoa menu tree is the only thing we
    // do on a strip change, so this acts as the dedupe key. NULL means
    // we've never installed one. See MenuBridge.c (Cocoa side).
    void         *installed_menu_strip;
    // Companion to installed_menu_strip: last-seen value of the AmLang
    // Window.menuStripVersion counter. AmLang ticks it on every
    // setMenuStrip() call so a re-install with the SAME strip pointer
    // (e.g. we just flipped an item's enabled flag) still forces a
    // Cocoa menu rebuild. Without this, pointer-only dedupe made
    // "same strip, updated .enabled" invisible in the bar.
    int           installed_menu_strip_version;
};

// Last opened SDL_Renderer, published so ViewContextGraphics can find
// a paint surface without having to walk through the AmLang
// ViewContext interface (Am.Ui.ViewContext is an interface, so direct
// property access from native C doesn't work). Single-window scope
// for P1 — multi-window support needs a per-Window renderer registry.
SDL_Renderer *am_ui_linux_primary_renderer(void);

#endif
