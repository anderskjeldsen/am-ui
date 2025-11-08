# Linux x64 Native Implementation with SDL2

This directory contains the native C implementation for the Am UI library on Linux x64 platform using SDL2.

## Overview

The linux-x64 implementation provides a platform-specific backend for the Am UI library that works on Linux x64 systems. It uses **SDL2 (Simple DirectMedia Layer)** to provide actual window management and graphics rendering, making it a fully functional UI system comparable to the AmigaOS and MorphOS implementations.

## Implementation Details

### Architecture

The implementation follows the same structure as the AmigaOS and MorphOS platforms:

- **Screen.c/h**: Screen management and initialization
- **Window.c/h**: Window creation, management, and event handling with SDL2
- **WindowManager.c/h**: Window manager for coordinating multiple windows
- **ViewContextGraphics.c/h**: Graphics context for drawing operations using SDL2 renderer
- **GadgetViews.c/h**: Gadget/widget layout and management
- **ScreenModeRequester.c/h**: Screen mode selection dialog
- **Startup.c/h**: UI system initialization and shutdown (initializes SDL2)
- **Button.c/h, Font.c/h, GadToolsButton.c/h**: UI component implementations

### Key Features

1. **Real Window Management**: Uses SDL2 to create actual native windows on Linux with proper window decorations and controls.

2. **Hardware-Accelerated Graphics**: SDL2 renderer provides hardware-accelerated drawing operations for optimal performance.

3. **Event Handling**: Full event processing including:
   - Mouse motion, button presses and releases
   - Window resize and expose events
   - Close button clicks
   - Real-time event polling

4. **Graphics Operations**: Actual rendering with SDL2:
   - Lines (SDL_RenderDrawLine)
   - Filled rectangles (SDL_RenderFillRect)
   - Color management with Amiga-style pen mapping
   - Text placeholder rendering

## Requirements

### SDL2 Library

The implementation requires SDL2 to be installed on your system:

```bash
# Ubuntu/Debian
sudo apt-get install libsdl2-dev

# Fedora
sudo dnf install SDL2-devel

# Arch Linux
sudo pacman -S sdl2

# macOS (via Homebrew)
brew install sdl2
```

## Usage

To build for linux-x64 platform, use the Am Lang compiler:

```bash
java -jar amlc-1.jar . -acmd -bt linux-x64
```

This will compile the Am Lang code and link it with the linux-x64 native implementations and SDL2.

Binary will end up in `builds/bin/linux-x64/app`.

## Features

All the standard Am UI features are implemented with real rendering:

- ✅ Screen opening/closing
- ✅ Window creation and management with SDL2
- ✅ Hardware-accelerated graphics rendering
- ✅ Real-time event handling (mouse, window events)
- ✅ Gadget layout
- ✅ Screen mode selection
- ✅ UI system initialization/shutdown with SDL2

## Graphics Rendering

The implementation provides actual graphics rendering:

- **Lines**: Rendered with `SDL_RenderDrawLine`
- **Rectangles**: Filled with `SDL_RenderFillRect`
- **Colors**: Amiga-style pen colors mapped to RGB
- **Text**: Placeholder rendering (boxes showing text location)
  - Note: Full text rendering would require SDL_ttf extension

### Color Mapping

Amiga pen colors are mapped to RGB values:
- Pen 0: Black (0, 0, 0)
- Pen 1: White (255, 255, 255)
- Pen 2: Light Gray (170, 170, 170)
- Pen 3: Dark Gray (85, 85, 85)
- Other pens: Generated algorithmically from pen index

## Debug Output

The implementation includes debug output prefixed with `[linux-x64]` to help trace execution:

```
[linux-x64] Am_Ui_Startup_init_0
[linux-x64] UI system initialized for Linux x64 with SDL2
[linux-x64] Am_Ui_Window_open_0
  Position: 100, 100
  Size: 640x480
[linux-x64] SDL2 window opened successfully
[linux-x64] drawLine: (10,10) -> (100,100)
[linux-x64] fillRect: (50,50) to (150,150)
```

## Example Application Flow

When an Am UI application runs on linux-x64:

1. **Initialization**: SDL2 video subsystem is initialized
2. **Window Creation**: Real SDL2 windows are created with proper decorations
3. **Graphics Operations**: Drawing commands render to SDL2 renderer
4. **Event Loop**: Events are processed in real-time via SDL_PollEvent
5. **Presentation**: Rendered content is presented to the screen with SDL_RenderPresent

## Visual Output

The implementation creates real, visible windows on Linux with:
- Window title bars
- Close/minimize/maximize buttons
- Resizable borders
- Actual rendered graphics (lines, rectangles)
- Real-time mouse interaction

## Future Enhancements

Possible future improvements:

- Add SDL_ttf for proper text rendering with fonts
- Add SDL_image for image loading support
- Implement keyboard event handling
- Add support for multiple simultaneous windows
- OpenGL rendering backend option
- Fullscreen mode support
- Hardware cursor support

## Technical Notes

### SDL2 Window Lifecycle

Windows are created with:
- `SDL_WINDOW_SHOWN`: Window is immediately visible
- `SDL_WINDOW_RESIZABLE`: User can resize the window
- Hardware-accelerated renderer with VSYNC

### Event Processing

The `handleInput` function uses non-blocking `SDL_PollEvent` to process:
- `SDL_QUIT`: Window close button
- `SDL_WINDOWEVENT`: Resize, expose, focus events
- `SDL_MOUSEMOTION`: Mouse movement
- `SDL_MOUSEBUTTONDOWN/UP`: Mouse button clicks

### Graphics Rendering

All drawing operations are batched and presented together:
1. Clear with background color
2. Execute drawing commands (lines, rects)
3. Present renderer to display

## Comparison with Other Platforms

| Feature | AmigaOS | MorphOS | Linux x64 (SDL2) |
|---------|---------|---------|------------------|
| Window System | Intuition | Intuition | SDL2 |
| Graphics | Native | Native | SDL2 Renderer |
| Acceleration | No | No | Yes (Hardware) |
| Events | IDCMP | IDCMP | SDL Events |
| Cross-platform | No | No | Yes |

## Conclusion

This implementation provides a fully functional UI system for Linux using SDL2, making it possible to:

- Run Am UI applications natively on Linux with real, visible windows
- Test and develop Am Lang applications on Linux with actual visual feedback
- Create cross-platform applications that work on both Amiga and Linux
- Use modern graphics acceleration while maintaining Amiga-style API compatibility
- Demonstrate UI concepts without requiring an Amiga emulator
