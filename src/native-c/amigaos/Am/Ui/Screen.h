#ifndef native_amigaos_aclass_Am_Ui_Screen_h
#define native_amigaos_aclass_Am_Ui_Screen_h
#include <libc/core.h>
#include <Am/Ui/Screen.h>
#include <Am/Lang/Object.h>
#include <Am/Lang/Int.h>

typedef struct _Am_Ui_Screen_data Am_Ui_Screen_data;
struct _Am_Ui_Screen_data {
	struct Screen * screen;	
	struct VisualInfo * visual_info;
};

#endif
