#include <libc/core.h>
#include <Am/Ui/Startup.h>
#include <linux-x64/Am/Ui/Startup.h>
#include <Am/Lang/Object.h>
#include <libc/core_inline_functions.h>
#include <SDL2/SDL.h>

function_result Am_Ui_Startup_init_0()
{
function_result __result = { .has_return_value = false };
bool __returning = false;

printf("[linux-x64] Am_Ui_Startup_init_0\n");

// Initialize SDL2
if (SDL_Init(SDL_INIT_VIDEO) < 0) {
printf("[linux-x64] SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
__throw_simple_exception("SDL_Init failed", "Am_Ui_Startup_init_0", &__result);
goto __exit;
}

printf("[linux-x64] UI system initialized for Linux x64 with SDL2\n");

__exit: ;
return __result;
};

function_result Am_Ui_Startup_shutdown_0()
{
function_result __result = { .has_return_value = false };
bool __returning = false;

printf("[linux-x64] Am_Ui_Startup_shutdown_0\n");

// Quit SDL2
SDL_Quit();

printf("[linux-x64] UI system shut down\n");

__exit: ;
return __result;
};
