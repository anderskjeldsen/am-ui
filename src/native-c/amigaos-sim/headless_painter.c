#include <libc/core.h>
#include <amigaos-sim/headless_painter.h>

#include <stdlib.h>
#include <string.h>

// ---------------- screen lifecycle ----------------

headless_screen_t * headless_screen_alloc(unsigned short width, unsigned short height)
{
    headless_screen_t *s = (headless_screen_t *) calloc(1, sizeof(headless_screen_t));
    if (s == NULL) return NULL;
    s->buffer = (unsigned char *) calloc((size_t) width * (size_t) height, 1);
    if (s->buffer == NULL) {
        free(s);
        return NULL;
    }
    s->width = width;
    s->height = height;
    s->translate_x = 0;
    s->translate_y = 0;
    s->foreground_pen = 1;
    s->background_pen = 0;
    s->clip.active = false;
    s->open = true;
    return s;
}

void headless_screen_free(headless_screen_t *s)
{
    if (s == NULL) return;
    if (s->buffer) free(s->buffer);
    free(s);
}

// ---------------- pull screen out of a Window aobject ----------------
// We avoid pulling Am_Ui_Window_data here to keep this file self-contained;
// instead, callers (Window.c, LayerGraphics.c) cast custom_value to a
// headless_screen_t * directly, which is exactly what Window.c stores there.

#include <Am/Ui/Window.h>
#include <libc/core_inline_functions.h>

headless_screen_t * headless_screen_for_window(aobject *window)
{
    if (window == NULL) return NULL;
    return (headless_screen_t *) __unwrap(window)->object_properties.class_object_properties.object_data.value.custom_value;
}

// ---------------- drawing primitives ----------------

void headless_fill_rect(headless_screen_t *s, short x1, short y1, short x2, short y2, unsigned char pen)
{
    if (s == NULL) return;
    // Normalize so x1 <= x2 and y1 <= y2 (callers may emit either order).
    if (x1 > x2) { short t = x1; x1 = x2; x2 = t; }
    if (y1 > y2) { short t = y1; y1 = y2; y2 = t; }
    for (short y = y1; y <= y2; y++) {
        for (short x = x1; x <= x2; x++) {
            headless_write_pixel(s, x, y, pen);
        }
    }
}

void headless_draw_line(headless_screen_t *s, short x1, short y1, short x2, short y2, unsigned char pen)
{
    if (s == NULL) return;
    // Standard Bresenham — handles all 8 octants without branching on slope sign.
    short dx = (x2 > x1) ? (x2 - x1) : (x1 - x2);
    short dy = (y2 > y1) ? (y2 - y1) : (y1 - y2);
    short sx = (x1 < x2) ? 1 : -1;
    short sy = (y1 < y2) ? 1 : -1;
    short err = dx - dy;
    short x = x1;
    short y = y1;
    while (1) {
        headless_write_pixel(s, x, y, pen);
        if (x == x2 && y == y2) break;
        short e2 = err * 2;
        if (e2 > -dy) { err -= dy; x = (short)(x + sx); }
        if (e2 <  dx) { err += dx; y = (short)(y + sy); }
    }
}

void headless_draw_text_box(headless_screen_t *s, short x, short y, unsigned int char_count, unsigned char pen)
{
    if (s == NULL || char_count == 0) return;
    // Each "character" is a solid 8x8 block in the foreground pen.
    short total_width = (short)(char_count * 8);
    headless_fill_rect(s, x, y, (short)(x + total_width - 1), (short)(y + 7), pen);
}
