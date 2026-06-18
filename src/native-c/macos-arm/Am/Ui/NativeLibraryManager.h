#pragma once
#include <libc/core.h>
#include <Am/Ui/NativeLibraryManager.h>
#include <Am/Lang/Object.h>
// No native library bookkeeping on Linux — system libraries are
// linked statically into the binary at build time. All methods are
// no-op stubs.
