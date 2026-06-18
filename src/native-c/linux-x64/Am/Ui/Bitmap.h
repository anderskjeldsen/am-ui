#ifndef native_linux_aclass_Am_Ui_Bitmap_h
#define native_linux_aclass_Am_Ui_Bitmap_h

#include <libc/core.h>
#include <Am/Ui/Bitmap.h>

#include <SDL2/SDL.h>
#include <stdbool.h>

// Native data for Am.Ui.Bitmap on Linux.
//
// Bitmaps come in two flavours:
//   - source-only: backed by an SDL_Surface. fromImage* path
//     produces these; LayerGraphics.drawBitmap uploads them on
//     demand and caches the resulting texture. Has ARGB pixels
//     visible to e.g. mask-aware blits.
//   - render-target: backed by an SDL_Texture with
//     SDL_TEXTUREACCESS_TARGET. RenderableBitmap.empty produces
//     these; LayerGraphics may set the renderer's target to this
//     texture and draw straight into it.
//
// `mask_enabled` mirrors the amigaos backend's "fromImageWithMask
// produced a 1-bit mask" semantics. On SDL the mask just toggles
// SDL_BLENDMODE_BLEND on the texture; the source surface's alpha
// channel does the cutout.
typedef struct _Am_Ui_Bitmap_data Am_Ui_Bitmap_data;
struct _Am_Ui_Bitmap_data {
    SDL_Surface *surface;        // non-NULL for source bitmaps
    SDL_Texture *texture;        // lazy-uploaded; or render-target
    Uint16       width;
    Uint16       height;
    bool         mask_enabled;
    bool         is_render_target;
    // The renderer the texture is bound to. SDL textures are
    // renderer-specific, so caching the binding lets the upload
    // happen against the right one and a cross-renderer draw
    // fails loud instead of producing a black square.
    SDL_Renderer *bound_renderer;
};

#endif
