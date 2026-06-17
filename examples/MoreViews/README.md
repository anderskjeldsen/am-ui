# MoreViews Example

This example demonstrates the new UI views available in the am-ui library: ImageView, ProgressBar, TextField, CheckBox, and Separator.

## Features

- **ImageView** - Display images with scaling and aspect ratio options
- **ProgressBar** - Show progress indicators with customizable ranges
- **TextField** - Text input with placeholder support
- **CheckBox** - Interactive checkboxes with change callbacks
- **Separator** - Visual dividers for organizing UI elements

The example creates a window with all these views arranged in a vertical stack layout, demonstrating:
- Proper padding and border styling
- Event handling for interactive elements
- Layout management with VStack and HStack
- Button interactions for updating UI state

## Building

Make sure you have the amlc.jar compiler in this directory, then run:

```bash
java -jar amlc.jar . -acmd -bt amigaos_docker_unix-x64
```

Or use the Makefile:

```bash
make build
```

The binary will be created in `builds/bin/amigaos/app`.

## Running

Copy the `app` file to your Amiga system and run it. The example will create a window demonstrating all the new UI views with interactive elements.

## Dependencies

- **am-lang-core** - Core AmLang runtime and standard library
- **am-ui** - UI framework with view components
- **am-imaging** - Image processing library for ImageView support