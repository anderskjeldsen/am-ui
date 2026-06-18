#ifndef native_linux_aclass_Am_Ui_RenderableBitmap_h
#define native_linux_aclass_Am_Ui_RenderableBitmap_h

#include <libc/core.h>
#include <Am/Ui/RenderableBitmap.h>

#include <SDL2/SDL.h>

// Off-screen render target. Backed by an SDL_Texture with
// SDL_TEXTUREACCESS_TARGET and a renderer borrowed from the
// owning Window so SDL_SetRenderTarget can park drawing into
// the texture for the LayerGraphics returned by createGraphics().
//
// We don't allocate a dedicated SDL_Renderer per RenderableBitmap
// — render targets are renderer-scoped, so a texture made from
// renderer A can only ever be drawn into via renderer A. That's
// fine for the IDE's pattern (one main window's renderer for all
// editor-side bitmaps).
typedef struct _Am_Ui_RenderableBitmap_data Am_Ui_RenderableBitmap_data;
struct _Am_Ui_RenderableBitmap_data {
    SDL_Renderer *renderer;  // borrowed from the owning Window
    SDL_Texture  *texture;   // SDL_TEXTUREACCESS_TARGET
    Uint16        width;
    Uint16        height;
};

#endif
