#include <libc/core.h>

#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Ui/Screen.h>
#include <Am/Lang/Int.h>

#include <linux-x64/linux.h>
#include <linux-x64/Am/Ui/Window.h>
#include <linux-x64/Am/Ui/Screen.h>

#include <libc/core_inline_functions.h>

void close_window_native(aobject * const this) {
Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
if ( data != NULL ) {
bool releasing = this->reference_count == 0;
if (releasing) {
this->reference_count = -100;
}
Am_Ui_Window_setRootView_0(this, NULL);
if (releasing) {
this->reference_count = 0;
}

printf("[linux-x64] Closing window %dx%d\n", data->width, data->height);

// Destroy SDL2 resources
if (data->sdl_renderer != NULL) {
SDL_DestroyRenderer(data->sdl_renderer);
data->sdl_renderer = NULL;
}
if (data->sdl_window != NULL) {
SDL_DestroyWindow(data->sdl_window);
data->sdl_window = NULL;
}

data->is_open = false;
}

free(this->object_properties.class_object_properties.object_data.value.custom_value);
this->object_properties.class_object_properties.object_data.value.custom_value = NULL;
}

function_result Am_Ui_Window__native_init_0(aobject * const this)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}
printf("[linux-x64] Am_Ui_Window__native_init_0\n");
__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};

function_result Am_Ui_Window__native_release_0(aobject * const this)
{
function_result __result = { .has_return_value = false };
bool __returning = false;

close_window_native(this);

printf("[linux-x64] Am_Ui_Window__native_release_0\n");
__exit: ;
return __result;
};

function_result Am_Ui_Window__native_mark_children_0(aobject * const this)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
__exit: ;
return __result;
};


function_result Am_Ui_Window_open_0(aobject * const this, short x, short y, unsigned short width, unsigned short height, aobject * screen, aobject * windowManager)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}

printf("[linux-x64] Am_Ui_Window_open_0\n");
printf("  Position: %d, %d\n", x, y);
printf("  Size: %dx%d\n", width, height);

Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) calloc(1, sizeof(Am_Ui_Window_data));
this->object_properties.class_object_properties.object_data.value.custom_value = data;

data->x = x;
data->y = y;
data->width = width;
data->height = height;
data->is_open = false;
data->pending_close = false;
data->pending_refresh = false;
data->last_mouse_x = 0;
data->last_mouse_y = 0;
data->fg_pen = 1;  // Default foreground color (white)
data->bg_pen = 0;  // Default background color (black)

// Create SDL2 window
data->sdl_window = SDL_CreateWindow(
"Am UI Window",
x == -1 ? SDL_WINDOWPOS_CENTERED : x,
y == -1 ? SDL_WINDOWPOS_CENTERED : y,
width, height,
SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
);

if (data->sdl_window == NULL) {
printf("[linux-x64] SDL_CreateWindow failed! SDL_Error: %s\n", SDL_GetError());
__throw_simple_exception("SDL_CreateWindow failed", "Am_Ui_Window_open_0", &__result);
goto __exit;
}

// Create SDL2 renderer
data->sdl_renderer = SDL_CreateRenderer(data->sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
if (data->sdl_renderer == NULL) {
printf("[linux-x64] SDL_CreateRenderer failed! SDL_Error: %s\n", SDL_GetError());
SDL_DestroyWindow(data->sdl_window);
data->sdl_window = NULL;
__throw_simple_exception("SDL_CreateRenderer failed", "Am_Ui_Window_open_0", &__result);
goto __exit;
}

// Set initial background color (black)
SDL_SetRenderDrawColor(data->sdl_renderer, 0, 0, 0, 255);
SDL_RenderClear(data->sdl_renderer);
SDL_RenderPresent(data->sdl_renderer);

data->is_open = true;

// Set pixel scale to 1:1 for Linux
this->object_properties.class_object_properties.properties[Am_Ui_Window_P_pixelScaleX].nullable_value.value.uchar_value = 1;
this->object_properties.class_object_properties.properties[Am_Ui_Window_P_pixelScaleY].nullable_value.value.uchar_value = 1;

// Set border to 0 for simplicity
Am_Ui_Window_setBorder_0(this, 0, 0, 0, 0);
Am_Ui_Window_onResize_0(this, x, y, width, height);

printf("[linux-x64] SDL2 window opened successfully\n");

__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
if (screen != NULL) {
__decrease_reference_count(screen);
}
return __result;
};

function_result Am_Ui_Window_close_0(aobject * const this)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}

printf("[linux-x64] Am_Ui_Window_close_0\n");
close_window_native(this);

__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};

function_result Am_Ui_Window_handleInput_0(aobject * const this)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}

Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;

if (window_data == NULL || !window_data->is_open) {
goto __exit;
}

// Poll SDL events
SDL_Event event;
while (SDL_PollEvent(&event)) {
switch (event.type) {
case SDL_QUIT:
printf("[linux-x64] SDL_QUIT event received\n");
window_data->pending_close = true;
Am_Ui_Window_onCloseButtonClick_0(this);
break;

case SDL_WINDOWEVENT:
if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
printf("[linux-x64] Window resized to %dx%d\n", event.window.data1, event.window.data2);
window_data->width = event.window.data1;
window_data->height = event.window.data2;
Am_Ui_Window_onResize_0(this, window_data->x, window_data->y, window_data->width, window_data->height);
window_data->pending_refresh = true;
} else if (event.window.event == SDL_WINDOWEVENT_EXPOSED) {
window_data->pending_refresh = true;
}
break;

case SDL_MOUSEMOTION:
if (event.motion.x != window_data->last_mouse_x || event.motion.y != window_data->last_mouse_y) {
window_data->last_mouse_x = event.motion.x;
window_data->last_mouse_y = event.motion.y;
Am_Ui_Window_onMouseEvent_0(this, 1, 0, event.motion.x, event.motion.y);
}
break;

case SDL_MOUSEBUTTONDOWN:
if (event.button.button == SDL_BUTTON_LEFT) {
Am_Ui_Window_onMouseEvent_0(this, 3, 1, event.button.x, event.button.y);
} else if (event.button.button == SDL_BUTTON_RIGHT) {
Am_Ui_Window_onMouseEvent_0(this, 3, 2, event.button.x, event.button.y);
}
break;

case SDL_MOUSEBUTTONUP:
if (event.button.button == SDL_BUTTON_LEFT) {
Am_Ui_Window_onMouseEvent_0(this, 2, 1, event.button.x, event.button.y);
} else if (event.button.button == SDL_BUTTON_RIGHT) {
Am_Ui_Window_onMouseEvent_0(this, 2, 2, event.button.x, event.button.y);
}
break;
}
}

// Handle pending refresh
if (window_data->pending_refresh) {
window_data->pending_refresh = false;
Am_Ui_Window_paint_0(this);
}

__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};

function_result Am_Ui_Window_isOpen_0(aobject * const this)
{
function_result __result = { .has_return_value = true };
bool __returning = true;
if (this != NULL) {
__increase_reference_count(this);
}

Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
__result.return_value.value.bool_value = (data != NULL && data->is_open);

__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};

function_result Am_Ui_Window_refresh_0(aobject * const this)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}

printf("[linux-x64] Am_Ui_Window_refresh_0\n");

Am_Ui_Window_data * const data = (Am_Ui_Window_data * const) this->object_properties.class_object_properties.object_data.value.custom_value;
if (data != NULL && data->is_open) {
// Clear the screen with background color
SDL_SetRenderDrawColor(data->sdl_renderer, 0, 0, 0, 255);
SDL_RenderClear(data->sdl_renderer);

// Trigger a repaint
Am_Ui_Window_paint_0(this);

// Present the renderer
SDL_RenderPresent(data->sdl_renderer);
}

__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};
