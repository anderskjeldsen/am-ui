#ifndef native_linux_aclass_Am_Ui_Font_h
#define native_linux_aclass_Am_Ui_Font_h

#include <libc/core.h>
#include <Am/Ui/Font.h>

#include <stdbool.h>

// Native data for Am.Ui.Font on Linux.
//
// We hold the SDL_ttf TTF_Font* as `void *` so this header doesn't
// pull SDL_ttf into every consumer's translation unit. The cast is
// done inside Font.c.
//
// The size field is what calculateStringWidth and the LayerGraphics
// "current font size" query report when a Window's currentFontSize
// is requested.
typedef struct _Am_Ui_Font_data Am_Ui_Font_data;
struct _Am_Ui_Font_data {
    void *ttf_font;          // TTF_Font *
    int   height;            // in pixels
    int   ascent;
    int   line_skip;
};

// Look up a system fallback font path. Tries a handful of well-known
// monospace TTFs (DejaVu, Liberation, Inconsolata) and returns the
// first one that exists, or NULL. Caller does not free.
const char * am_ui_linux_font_fallback_path(void);

#endif
