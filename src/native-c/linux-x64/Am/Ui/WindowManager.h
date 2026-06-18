#pragma once
#include <libc/core.h>
#include <Am/Ui/WindowManager.h>
#include <Am/Lang/Object.h>
// WindowManager is a process-singleton holder on Linux; no native
// state beyond what AmLang carries itself. Implementation is in
// WindowManager.c as no-op stubs.
