#ifndef native_amigaos_aclass_Am_Ui_PixelBufferView_h
#define native_amigaos_aclass_Am_Ui_PixelBufferView_h

#include <libc/core.h>
#include <Am/Ui/PixelBufferView.h>

// PixelBufferView has no AmigaOS-specific native state.
// The Bitmap object and pixels[] array are stored as AML properties;
// flush() reaches them via Am_Ui_PixelBufferView_P_* indices.

#endif
