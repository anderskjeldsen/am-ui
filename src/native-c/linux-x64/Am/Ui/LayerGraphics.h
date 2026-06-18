#ifndef native_linux_aclass_Am_Ui_LayerGraphics_h
#define native_linux_aclass_Am_Ui_LayerGraphics_h

#include <libc/core.h>
#include <Am/Ui/LayerGraphics.h>

#include <SDL2/SDL.h>
#include <stdbool.h>

// Native data for LayerGraphics on Linux (SDL2 backend).
//
// LayerGraphics is just a thin handle over an SDL_Renderer plus
// the current pen / clip / translate state. The SDL_Renderer
// itself is owned by either a Window or a RenderableBitmap and
// is borrowed for the duration of the paint — we never free it
// from here. `target_texture` is non-NULL when this LayerGraphics
// is hosting a RenderableBitmap (target texture) and the Renderer
// needs SDL_SetRenderTarget on entry / NULL on exit.
//
// pen_palette is borrowed from the owning Screen. Pen-based set
// calls resolve to an ARGB by indexing into it.
typedef struct _Am_Ui_LayerGraphics_data Am_Ui_LayerGraphics_data;
struct _Am_Ui_LayerGraphics_data {
    SDL_Renderer *renderer;       // borrowed
    SDL_Texture  *target_texture; // borrowed; NULL for window-targeted
    const Uint32 *pen_palette;    // borrowed; 32 ARGB entries
    int           pen_palette_count;
    // When true, this LayerGraphics points at a RenderableBitmap but
    // we've flattened the indirection: paints land on the window
    // directly. Used to make blitBitmapRect from this bitmap a no-op
    // so flushDirtyBounds doesn't overpaint what we already wrote.
    bool          bypass_offscreen;

    SDL_Color     foreground;     // resolved RGB+A for primitives
    SDL_Color     background;     // currently only used by drawString
    int           translate_x;
    int           translate_y;
    SDL_Rect      clip_rect;
    bool          clip_active;
    bool          painting;       // true between beginPainting / endPainting

    // Borrowed font for drawString. SDL_ttf TTF_Font *; void * to
    // keep this header SDL_ttf-agnostic (SDL_ttf is an optional
    // dependency at this point).
    void         *current_font;
    int           current_font_height;
};

#endif
