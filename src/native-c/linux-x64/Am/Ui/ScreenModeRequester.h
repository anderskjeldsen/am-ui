#pragma once
#include <libc/core.h>
#include <Am/Ui/ScreenModeRequester.h>
#include <Am/Lang/Object.h>

typedef struct _Am_Ui_ScreenModeRequester_data Am_Ui_ScreenModeRequester_data;
struct _Am_Ui_ScreenModeRequester_data {
	unsigned long min_width;
	unsigned long min_height;
	unsigned long min_depth;

	unsigned long max_width;
	unsigned long max_height;
	unsigned long max_depth;

	unsigned long desired_width;
	unsigned long desired_height;
	unsigned long desired_depth;
};
