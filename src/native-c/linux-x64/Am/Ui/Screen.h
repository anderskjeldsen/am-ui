#ifndef native_linux_x64_aclass_Am_Ui_Screen_h
#define native_linux_x64_aclass_Am_Ui_Screen_h
#include <libc/core.h>
#include <Am/Ui/Screen.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/Int.h>
#include <SDL2/SDL.h>

typedef struct _Am_Ui_Screen_data Am_Ui_Screen_data;
struct _Am_Ui_Screen_data {
	int width;
	int height;
	int depth;
	int displayId;
	SDL_Window *sdl_window;
	SDL_Renderer *sdl_renderer;
};

#endif
