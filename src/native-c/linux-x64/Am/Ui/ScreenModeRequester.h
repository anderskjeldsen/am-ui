#ifndef native_linux_aclass_Am_Ui_ScreenModeRequester_h
#define native_linux_aclass_Am_Ui_ScreenModeRequester_h
#include <libc/core.h>
#include <Am/Ui/ScreenModeRequester.h>
#include <Am/Lang/Object.h>
#include <Am/Ui/ScreenMode.h>
#include <Am/Lang/String.h>
#include <Am/Lang/Int.h>
// ScreenModeRequester is hidden on Linux — WMs don't expose
// switchable display modes to a non-fullscreen application. The
// AmLang surface is implemented as a no-op that returns "cancel".
#endif
