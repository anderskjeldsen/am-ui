#include <libc/core.h>
#include <Am/Ui/BitmapGraphics.h>
#include <libc/Am/Ui/BitmapGraphics.h>
#include <Am/Ui/Bitmap.h>
#include <Am/Ui/Font.h>
#include <Am/Imaging/Image.h>
#include <Am/Ui/ClipRect.h>
#include <libc/core_inline_functions.h>

// BitmapGraphics is AmigaOS/MorphOS-only.
// All implementations live in the platform-specific native files.
// This file is intentionally empty so that platform builds do not get
// duplicate symbol errors when the amigaos/morphos-ppc files are also linked.
