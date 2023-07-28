#ifndef native_amigaos_aclass_Am_Ui_Window_h
#define native_amigaos_aclass_Am_Ui_Window_h
#include <libc/core.h>
#include <Am/Ui/Window.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/Int.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>

typedef struct _Am_Ui_Window_data Am_Ui_Window_data;
struct _Am_Ui_Window_data {
    struct Screen * locked_screen;
	struct Window * window;
    struct VisualInfo * visual_info;
//    struct Gadget *first_gadget;
    struct Gadget *context_gadget;
};

#endif
