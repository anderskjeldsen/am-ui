#ifndef native_amigaos_aclass_Am_Ui_Window_h
#define native_amigaos_aclass_Am_Ui_Window_h
#include <libc/core.h>
#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/Int.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

#define AM_UI_WINDOW_MENU_MAX_MENUS 16
#define AM_UI_WINDOW_MENU_MAX_ITEMS 128

typedef struct _Am_Ui_Window_menu_builder Am_Ui_Window_menu_builder;
struct _Am_Ui_Window_menu_builder {
    int num_menus;
    int num_items;
    char * menu_titles[AM_UI_WINDOW_MENU_MAX_MENUS];
    int item_menu_indices[AM_UI_WINDOW_MENU_MAX_ITEMS];
    char * item_labels[AM_UI_WINDOW_MENU_MAX_ITEMS];
    aobject * item_aobjects[AM_UI_WINDOW_MENU_MAX_ITEMS];
};

typedef struct _Am_Ui_Window_data Am_Ui_Window_data;
struct _Am_Ui_Window_data {
    struct Screen * locked_screen;
	struct Window * window;
    struct VisualInfo * visual_info;
    struct IntuiMessage refresh_msg;
//    struct Gadget *first_gadget;
    struct Gadget *context_gadget;
    struct Region *clip_region;
    struct Region *old_clip_region;
    bool pending_close;
    bool pending_refresh;
    bool pending_full_refresh;
    bool pending_resize;
    bool inside_handle_input;
    bool deferred_full_close;
    short last_mouse_x;
    short last_mouse_y;
    Am_Ui_Window_menu_builder * menu_builder;
    struct NewMenu * menu_new_menus;
    struct Menu * menu_strip;
    aobject * menu_item_aobjects[AM_UI_WINDOW_MENU_MAX_ITEMS];
    int menu_item_count;
};

#endif
