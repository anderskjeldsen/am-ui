#ifndef native_linux_x64_aclass_Am_Ui_Window_h
#define native_linux_x64_aclass_Am_Ui_Window_h
#include <libc/core.h>
#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/Int.h>
#include <SDL2/SDL.h>

typedef struct _Am_Ui_Window_data Am_Ui_Window_data;
struct _Am_Ui_Window_data {
	short x;
	short y;
	unsigned short width;
	unsigned short height;
	bool is_open;
	bool pending_close;
	bool pending_refresh;
	short last_mouse_x;
	short last_mouse_y;
	SDL_Window *sdl_window;
	SDL_Renderer *sdl_renderer;
	unsigned char fg_pen;
	unsigned char bg_pen;
};

#endif
