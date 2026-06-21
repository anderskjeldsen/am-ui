# am-ui Linux backend (SDL2)

Phase-1 native backend for am-ui on Linux. Targets a single `libsdl2`
package install (+ `libsdl2-ttf` for fonts); same code lights up macOS
and Windows builds as a side effect.

See [`am-ui/LINUX_BACKEND.md`](../../../LINUX_BACKEND.md) for the
overall design rationale.

## What's wired in this directory

| File | Status | Notes |
|------|--------|-------|
| `Screen.c` | working | Palette table (32 ARGB entries) + display bounds from `SDL_GetDisplayBounds`. Default palette mirrors am-ide's `AppPen` defaults so a smoke-test build is legible before `Program.buildPalette` runs. |
| `Window.c` | working (P1) | One `SDL_Window` + `SDL_Renderer` per AmLang Window. `handleInput` pumps SDL events and dispatches mouse + resize + close to the AmLang side. Title via `SDL_SetWindowTitle`. Clipboard via `SDL_SetClipboardText` (paste is TODO; needs AmLang String allocation). Keyboard / text-input events received but not yet routed — wait for the AmLang side to settle the `onKeyboardEvent` signature. |
| `LayerGraphics.c` | working (P1) | `setForegroundPen/Color`, `setBackgroundPen/Color`, `fillRect`, `eraseRect`, `drawLine`, `drawBitmap`, `blitBitmapRect`, `drawString` (requires `AM_UI_LINUX_SDL_TTF`), `calculateStringWidth`, `setFont`, `setClipRect`, `clearClipRect`, `beginPainting`, `endPainting`, `translate`, `attachWindow`, `attachRenderableBitmap`. `drawImage` and `scrollRect` are stubs. |
| `ViewContextGraphics.c` | working (P1) | Same paint primitives as `LayerGraphics.c` but the renderer is borrowed from the owning Window's `ViewContext`. Used by every `View.paint`. |
| `Bitmap.c` | working (P1) | `createEmpty`, `createFromImage`, `createFromImageWithMask`. ARGB only — indexed-source PNGs (rare for IDE assets) fall back to the no-op path. Texture upload is lazy on first `drawBitmap`. |
| `RenderableBitmap.c` | working (P1) | SDL_Texture with `SDL_TEXTUREACCESS_TARGET`; binding happens when a `LayerGraphics` attaches and supplies its renderer. |
| `Font.c` | working when SDL_ttf available | `Am_Ui_Font_open_0` + `calculateStringWidth_0`. Falls back to a hardcoded list of system monospace TTF paths (DejaVu / Liberation / Inconsolata / Menlo) when no path is passed. Compile-out cleanly via `-UAM_UI_LINUX_SDL_TTF` if libsdl2-ttf isn't installed — the IDE still runs, `drawString` just paints nothing. |
| `WindowManager.c`, `Button.c`, `GadgetViews.c`, `GadToolsButton.c`, `NativeLibraryManager.c`, `PixelBufferView.c`, `ScreenModeRequester.c` | stub | No-op implementations — these classes either have no Linux equivalent (GadTools / iconification) or render purely via the AmLang side (Button / PixelBufferView). |
| `FolderRequester.c` | stub (TODO) | Returns NULL (user-cancelled). Pulling in GTK/Qt for one dialog is the wrong shape; the long-term plan is an in-canvas picker built from am-ui's existing View widgets. |

## Building

Install SDL2:

```bash
# Debian / Ubuntu
sudo apt install libsdl2-dev libsdl2-ttf-dev

# Arch
sudo pacman -S sdl2 sdl2_ttf

# macOS (also runs the same backend)
brew install sdl2 sdl2_ttf
```

Build am-ide for Linux:

```bash
cd am-ide
make build-linux
```

The output binary lands at `am-ide/builds/bin/linux/app`. Run it from
that directory so relative `.studio/` paths resolve:

```bash
cd am-ide/builds/bin/linux
./app
```

To stage the IDE's extension catalog (sprite-pixler, ChatGpt, etc.):

```bash
make seed-linux-studio
```

## Known limitations (P1)

* **No menus.** AmigaOS menu strip APIs are no-op'd. The IDE's File / Edit
  menus won't appear; the workaround is to use keyboard shortcuts and
  the sidebar apps.
* **No native folder picker.** Project open is text-input only until
  the in-canvas picker lands.
* **No clipboard paste.** Copy works (`SDL_SetClipboardText`); paste
  needs AmLang `String` allocation from the native side, deferred to
  P2.
* **No keyboard events.** `SDL_KEYDOWN` / `SDL_KEYUP` arrive but aren't
  routed to the AmLang side yet. TextField won't accept input.
* **No HiDPI scaling.** Renderer treats pixel coords 1:1 with view
  coords. Looks correct on standard-DPI displays; tiny on Retina /
  4K. SDL's `SDL_WINDOW_ALLOW_HIGHDPI` is set; the pipeline-side fix
  is to read `SDL_GetWindowPixelDensity` and feed it into the View
  layout's pixel scale.
* **No `drawImage`.** Image-direct paints (rare — `ImageButton` already
  goes through Bitmap) silently no-op.
* **No `scrollRect`.** The TextEditor's fast-scroll path will fall back
  to full repaints; legible but slower than AmigaOS.
* **No native PixelBufferView fast path.** AmLang exposes a portable
  surface that works correctly; sprite-pixler-style cell drawing will
  be slower per frame than on AmigaOS until the SDL_UpdateTexture
  fast path is wired.

## Adding new native methods

Each AmLang `native fun foo(...)` on a `#implementationPlatforms` class
that includes `"linux"` becomes an extern C `Am_Ui_X_foo_N` symbol the
linker resolves to one of the files in this directory. Mirror the
amigaos peer's signature exactly (a typo in the parameter list is a
silent link error that hides behind "TODO not implemented" stub
output). The amigaos peer is the reference impl for what the AmLang
side actually expects.
