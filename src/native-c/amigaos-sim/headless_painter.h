// Headless painter for the amigaos-sim build target. Draws into a per-Window
// byte buffer instead of an OS window so the layout / paint / clip pipeline can
// be unit-tested under linux-x64 without X11 or a real AmigaOS environment.
//
// Each pixel is one byte, holding the foreground pen index that was active at
// draw time (1 = first user pen, 0 = background / never painted). Clipping is
// enforced exactly once, in headless_write_pixel — every other primitive
// (fill_rect, draw_line, draw_text_box) routes through it.

#ifndef AMIGAOS_SIM_HEADLESS_PAINTER_H
#define AMIGAOS_SIM_HEADLESS_PAINTER_H

#include <stdbool.h>
#include <stddef.h>

// 150x100 is the smallest size that fits the AppLauncher-style layouts the
// tests aim at; bumping this only costs a bigger malloc.
#define HEADLESS_SCREEN_WIDTH   150
#define HEADLESS_SCREEN_HEIGHT  100

typedef struct {
    short min_x;
    short min_y;
    short max_x; // inclusive
    short max_y; // inclusive
    bool active;  // false = no clip (everything passes)
} headless_clip_t;

typedef struct {
    unsigned char *buffer;          // width * height bytes, row-major
    unsigned short width;
    unsigned short height;
    short translate_x;              // current cumulative translate (set by Graphics.translate)
    short translate_y;
    unsigned char foreground_pen;   // current pen for fill / line / text
    unsigned char background_pen;   // currently unused; kept for API parity
    headless_clip_t clip;           // current installed clip rect (in screen coords)
    bool open;                      // Window.isOpen flips false on close
} headless_screen_t;

// Allocate / free
headless_screen_t * headless_screen_alloc(unsigned short width, unsigned short height);
void                headless_screen_free(headless_screen_t *s);

// Write a single pixel at (x, y) in screen coords. Silently no-ops if (x, y)
// falls outside the screen bounds OR outside the active clip rect.
static inline void headless_write_pixel(headless_screen_t *s, short x, short y, unsigned char pen)
{
    if (s == NULL || s->buffer == NULL) return;
    if (x < 0 || y < 0) return;
    if ((unsigned short)x >= s->width || (unsigned short)y >= s->height) return;
    if (s->clip.active) {
        if (x < s->clip.min_x || x > s->clip.max_x) return;
        if (y < s->clip.min_y || y > s->clip.max_y) return;
    }
    s->buffer[(unsigned short)y * s->width + (unsigned short)x] = pen;
}

// Read a single pixel — used by the test-only HeadlessPainter native class.
// Out-of-bounds reads return 0.
static inline unsigned char headless_read_pixel(const headless_screen_t *s, short x, short y)
{
    if (s == NULL || s->buffer == NULL) return 0;
    if (x < 0 || y < 0) return 0;
    if ((unsigned short)x >= s->width || (unsigned short)y >= s->height) return 0;
    return s->buffer[(unsigned short)y * s->width + (unsigned short)x];
}

// Filled rectangle [x1..x2] x [y1..y2] (inclusive both ends, matching
// AmigaOS RectFill). Coordinates are pre-translated by the Graphics layer.
void headless_fill_rect(headless_screen_t *s, short x1, short y1, short x2, short y2, unsigned char pen);

// Bresenham-style line from (x1, y1) to (x2, y2), inclusive.
void headless_draw_line(headless_screen_t *s, short x1, short y1, short x2, short y2, unsigned char pen);

// Fill an 8x8 block per character. The "font" in amigaos-sim is just a solid
// 8-pixel cell — enough to verify drawString hit the right bounds and pen.
void headless_draw_text_box(headless_screen_t *s, short x, short y, unsigned int char_count, unsigned char pen);

// Pull the screen out of a Window aobject's custom_value. `aobject` is an
// am-lang-core typedef; including <libc/core.h> at the call site provides it.
#include <libc/core.h>
headless_screen_t * headless_screen_for_window(aobject *window);

#endif // AMIGAOS_SIM_HEADLESS_PAINTER_H
