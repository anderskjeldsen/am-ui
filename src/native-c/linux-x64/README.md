# Linux x64 Native Implementation

This directory contains the native C implementation for the Am UI library on Linux x64 platform.

## Overview

The linux-x64 implementation provides a platform-specific backend for the Am UI library that works on Linux x64 systems. Unlike the AmigaOS and MorphOS implementations which use Intuition and related system libraries, this implementation provides a simulated/logging-based approach.

## Implementation Details

### Architecture

The implementation follows the same structure as the AmigaOS and MorphOS platforms:

- **Screen.c/h**: Screen management and initialization
- **Window.c/h**: Window creation, management, and event handling
- **WindowManager.c/h**: Window manager for coordinating multiple windows
- **ViewContextGraphics.c/h**: Graphics context for drawing operations
- **GadgetViews.c/h**: Gadget/widget layout and management
- **ScreenModeRequester.c/h**: Screen mode selection dialog
- **Startup.c/h**: UI system initialization and shutdown
- **Button.c/h, Font.c/h, GadToolsButton.c/h**: UI component implementations

### Key Differences from AmigaOS/MorphOS

1. **No Native GUI**: This implementation does not use X11, Wayland, or any other Linux GUI toolkit. Instead, it provides a logging-based simulation of UI operations.

2. **Simplified Data Structures**: Instead of using Intuition's Window and Screen structures, we use simple structs that store window/screen properties.

3. **Event Handling**: The `handleInput` function is simplified and doesn't block waiting for real events.

4. **Graphics Operations**: Drawing operations (drawLine, fillRect, drawString) print log messages instead of rendering to a display.

## Usage

To build for linux-x64 platform, use the Am Lang compiler:

```bash
java -jar amlc-1.jar . -acmd -bt linux-x64
```

This will compile the Am Lang code and link it with the linux-x64 native implementations.

## Features

All the standard Am UI features are implemented:

- ✅ Screen opening/closing
- ✅ Window creation and management
- ✅ Graphics context operations (lines, rectangles, text)
- ✅ Gadget layout
- ✅ Screen mode selection
- ✅ UI system initialization/shutdown

## Debug Output

The implementation includes debug output prefixed with `[linux-x64]` to help trace execution:

```
[linux-x64] Am_Ui_Startup_init_0
[linux-x64] UI system initialized for Linux x64
[linux-x64] Am_Ui_Screen_open_0
  Width: 640
  Height: 480
  Depth: 8
  DisplayId: 0
[linux-x64] Screen opened successfully
```

## Future Enhancements

Possible future improvements:

- Add SDL2 or X11 backend for actual rendering
- Implement proper event handling with real input events
- Add OpenGL support for hardware-accelerated graphics
- Support for modern UI features (transparency, anti-aliasing, etc.)

## Notes

This implementation serves as a foundation for porting Am UI applications to Linux. Applications built with this backend will run on Linux x64 systems and can be used for:

- Testing Am Lang applications on Linux
- Development and debugging without requiring an Amiga emulator
- Server-side UI operations (headless mode)
- Future integration with Linux GUI toolkits
