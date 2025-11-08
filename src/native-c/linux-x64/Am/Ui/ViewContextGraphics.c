#include <libc/core.h>
#include <Am/Ui/ViewContextGraphics.h>
#include <linux-x64/Am/Ui/ViewContextGraphics.h>
#include <Am/Ui/Graphics.h>
#include <Am/Ui/ViewContext.h>
#include <Am/Lang/UByte.h>
#include <Am/Lang/Short.h>

#include <linux-x64/linux.h>
#include <linux-x64/Am/Ui/Window.h>
#include <linux-x64/Am/Ui/Screen.h>
#include <libc/Am/Lang/String.h>

#include <libc/core_inline_functions.h>

short translated_x(aobject *g, short x) {
short tx = g->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_xOffset].nullable_value.value.short_value;
return tx + x;
}

short translated_y(aobject *g, short y) {
short ty = g->object_properties.class_object_properties.properties[Am_Ui_Graphics_P_yOffset].nullable_value.value.short_value;
return ty + y;
}

// Convert Amiga pen colors to SDL RGB colors
void pen_to_rgb(unsigned char pen, Uint8 *r, Uint8 *g, Uint8 *b) {
// Simple Amiga-like palette mapping
switch (pen) {
case 0: *r = 0; *g = 0; *b = 0; break;       // Black
case 1: *r = 255; *g = 255; *b = 255; break; // White
case 2: *r = 170; *g = 170; *b = 170; break; // Light gray
case 3: *r = 85; *g = 85; *b = 85; break;    // Dark gray
default: 
// Generate colors from pen index
*r = ((pen & 0x03) * 85);
*g = (((pen >> 2) & 0x03) * 85);
*b = (((pen >> 4) & 0x03) * 85);
break;
}
}

function_result Am_Ui_ViewContextGraphics__native_init_0(aobject * const this)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}
printf("[linux-x64] Am_Ui_ViewContextGraphics__native_init_0\n");
__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};

function_result Am_Ui_ViewContextGraphics__native_release_0(aobject * const this)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
printf("[linux-x64] Am_Ui_ViewContextGraphics__native_release_0\n");
__exit: ;
return __result;
};

function_result Am_Ui_ViewContextGraphics__native_mark_children_0(aobject * const this)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
__exit: ;
return __result;
};

function_result Am_Ui_ViewContextGraphics_setForegroundPen_0(aobject * const this, unsigned char pen)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}

aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
if (window != NULL) {
Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
if (window_data != NULL) {
window_data->fg_pen = pen;
printf("[linux-x64] setForegroundPen: %d\n", pen);
}
__decrease_reference_count(window);
}

__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};

function_result Am_Ui_ViewContextGraphics_setBackgroundPen_0(aobject * const this, unsigned char pen)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}

aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
if (window != NULL) {
Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
if (window_data != NULL) {
window_data->bg_pen = pen;
printf("[linux-x64] setBackgroundPen: %d\n", pen);
}
__decrease_reference_count(window);
}

__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};

function_result Am_Ui_ViewContextGraphics_drawLine_0(aobject * const this, short x, short y, short x2, short y2)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}

short tx = translated_x(this, x);
short ty = translated_y(this, y);
short tx2 = translated_x(this, x2);
short ty2 = translated_y(this, y2);

printf("[linux-x64] drawLine: (%d,%d) -> (%d,%d)\n", tx, ty, tx2, ty2);

aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
if (window != NULL) {
Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
if (window_data != NULL && window_data->sdl_renderer != NULL) {
Uint8 r, g, b;
pen_to_rgb(window_data->fg_pen, &r, &g, &b);
SDL_SetRenderDrawColor(window_data->sdl_renderer, r, g, b, 255);
SDL_RenderDrawLine(window_data->sdl_renderer, tx, ty, tx2, ty2);
}
__decrease_reference_count(window);
}

__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};

function_result Am_Ui_ViewContextGraphics_fillRect_0(aobject * const this, short x, short y, short x2, short y2)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}

short tx = translated_x(this, x);
short ty = translated_y(this, y);
short tx2 = translated_x(this, x2);
short ty2 = translated_y(this, y2);

printf("[linux-x64] fillRect: (%d,%d) to (%d,%d)\n", tx, ty, tx2, ty2);

aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
if (window != NULL) {
Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
if (window_data != NULL && window_data->sdl_renderer != NULL) {
Uint8 r, g, b;
pen_to_rgb(window_data->fg_pen, &r, &g, &b);
SDL_SetRenderDrawColor(window_data->sdl_renderer, r, g, b, 255);

SDL_Rect rect;
rect.x = tx < tx2 ? tx : tx2;
rect.y = ty < ty2 ? ty : ty2;
rect.w = abs(tx2 - tx) + 1;
rect.h = abs(ty2 - ty) + 1;
SDL_RenderFillRect(window_data->sdl_renderer, &rect);
}
__decrease_reference_count(window);
}

__exit: ;
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};

function_result Am_Ui_ViewContextGraphics_drawString_0(aobject * const this, aobject * text, short x, short y)
{
function_result __result = { .has_return_value = false };
bool __returning = false;
if (this != NULL) {
__increase_reference_count(this);
}
if (text != NULL) {
__increase_reference_count(text);
}

short tx = translated_x(this, x);
short ty = translated_y(this, y);

string_holder *string_holder_value = text->object_properties.class_object_properties.object_data.value.custom_value;
printf("[linux-x64] drawString at (%d,%d): %s\n", tx, ty, string_holder_value->string);

// Note: SDL2 doesn't have built-in text rendering. We would need SDL_ttf for that.
// For now, we'll draw a simple rectangle where text would be to show something is being drawn.
aobject *window = Am_Ui_ViewContextGraphics_f_getWindow_0(this).return_value.value.object_value;
if (window != NULL) {
Am_Ui_Window_data * const window_data = (Am_Ui_Window_data * const) window->object_properties.class_object_properties.object_data.value.custom_value;
if (window_data != NULL && window_data->sdl_renderer != NULL) {
Uint8 r, g, b;
pen_to_rgb(window_data->fg_pen, &r, &g, &b);
SDL_SetRenderDrawColor(window_data->sdl_renderer, r, g, b, 255);

// Draw a simple rectangle to represent text
int text_len = strlen(string_holder_value->string);
SDL_Rect rect;
rect.x = tx;
rect.y = ty;
rect.w = text_len * 8;  // Approximate 8 pixels per character
rect.h = 12;            // Approximate height
SDL_RenderDrawRect(window_data->sdl_renderer, &rect);
}
__decrease_reference_count(window);
}

__exit: ;
if (text != NULL) {
__decrease_reference_count(text);
}
if (this != NULL) {
__decrease_reference_count(this);
}
return __result;
};
