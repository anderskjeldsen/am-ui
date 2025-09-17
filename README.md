# am-ui

## What is am-ui?

am-ui is a cross-platform UI library written in AmLang that provides modern UI components for retro and niche platforms. It enables developers to create rich graphical applications for AmigaOS, MorphOS, and other classic computing platforms using familiar, modern programming patterns.

### Key Features

- **Cross-platform**: Supports AmigaOS 3.x, MorphOS, and other retro platforms
- **Modern UI Components**: Buttons, Labels, Windows, Screens, and layout containers
- **Layout System**: VStack and HStack for flexible, responsive layouts  
- **Native Integration**: Uses native Intuition/GadTools APIs for authentic platform look and feel
- **Event Handling**: Mouse and keyboard event support with callback-based interaction model

### Available UI Components

- **Window**: Main application windows with resize, close, and drag capabilities
- **Screen**: Custom screens with configurable resolution and color depth
- **Button**: Interactive buttons with click handlers and hover states
- **Label**: Text display with font and color customization
- **Panel**: Container with border and padding support
- **VStack**: Vertical layout container with spacing and growth controls
- **HStack**: Horizontal layout container with spacing and growth controls
- **ScreenModeRequester**: Native screen mode selection dialogs

## Simple Usage Example

Here's a basic example of creating a window with UI components:

```aml
// Create a window
var window = Window.openWindow(20S, 20S, 300US, 200US, null, null)

// Create a vertical layout container
var vStack = new VStack(5S) // 5 pixel spacing

// Add a label
vStack.addChild(new Label("Hello Amiga!"))

// Add an interactive button
vStack.addChild(new Button("Click Me!", (view) => {
    // Handle button click
    "Button was clicked!".println()
    return true
}).setup((button) => {
    button.setDefaultPadding()
    button.growX = 255UB // Fill horizontal space
}))

// Create a panel container with border and padding
var panel = new Panel()
panel.setDefaultBorder()
panel.setDefaultPadding()
panel.setChild(vStack)

// Set the panel as the window's root view
window.setRootView(panel)
window.layout()
window.requestRepaint()

// Main event loop
while(window.isOpen()) {
    window.handleInput()
}
```

## About AmLang

am-ui is built with AmLang, a modern programming language designed for retro and embedded platforms.
### AmLang Language Features

AmLang is inspired by other programming languages like Kotlin, Java, C#, TypeScript, Swift etc. 

- **ARC (automatic reference counting)** - Memory management without garbage collection overhead
- **Classes and interfaces** - Object-oriented programming with familiar syntax
- **Namespaces** - Code organization and modularity
- **Suspendable functions** - Asynchronous programming support
- **Lambda expressions** - Functional programming constructs
- **Native C support** - Seamless integration with existing C libraries
- **Exceptions (try, catch, throw)** - Robust error handling
- **And much more** - Modern language features adapted for retro platforms

*Note: Some features like for-loops are still in development. While loops are currently used as an alternative.*

The compiler transpiles AmLang code to C, which is then compiled with GCC for the target platform. This approach provides good performance while maintaining portability across different architectures.

## Requirements

### Development Environment
- **Java 11+** - Required for running the AmLang compiler
- **Docker** - Used for cross-compilation to target platforms  
- **Host OS**: Windows, macOS, or Linux

### Target Platforms
- **AmigaOS 3.x** - Classic Amiga systems
- **MorphOS** - PowerPC-based Amiga-compatible systems
- Other retro platforms (in development)

## Getting Started

### Building for AmigaOS 3.x

The am-ui library includes example applications that demonstrate its capabilities. To get started:

1. **Choose an example**: Look at the `Startup.aml` file which contains a file browser example
2. **Build the application**:
   ```bash
   java -jar amlc-1.jar . -acmd -bt amigaos_docker_unix-x64
   ```
3. **Find your binary**: The compiled application will be in `builds/bin/amigaos/` as a file called "app"
4. **Deploy**: Copy the "app" file to your Amiga environment and run it

### Build Options

**Enable runtime logging**:
```bash
java -jar amlc-1.jar . -acmd -bt amigaos_docker_unix-x64 -rl
```

**Add debug comments**:
```bash
java -jar amlc-1.jar . -acmd -bt amigaos_docker_unix-x64 -rdc
```

### Command Line Options

- **`-acmd`**: Allows the compiler to run commands from package.yml
- **`-bt <target>`**: Specifies the build target platform (from package.yml)
- **`-rl`**: Enables additional runtime logging
- **`-rdc`**: Adds more descriptive comments in generated code

### Available Build Targets

- `amigaos_docker_unix-x64` - AmigaOS 3.x (x86-64 host)
- `amigaos_docker_unix-arm` - AmigaOS 3.x (ARM host) 
- `morphos-ppc_docker` - MorphOS PowerPC

## Project Structure

```
src/
├── am-lang/Am/Ui/          # AmLang UI component source files
│   ├── Button.aml          # Interactive button component
│   ├── Label.aml           # Text display component  
│   ├── Window.aml          # Main window management
│   ├── Screen.aml          # Custom screen support
│   ├── VStack.aml          # Vertical layout container
│   ├── HStack.aml          # Horizontal layout container
│   ├── Panel.aml           # Container with borders/padding
│   └── Startup.aml         # Example file browser application
└── native-c/               # Platform-specific C implementations
    ├── amigaos/            # AmigaOS native code
    ├── morphos-ppc/        # MorphOS native code
    └── libc/               # Generic C implementations
```

## Contributing

am-ui is a work in progress. The library is being actively developed with new components and features being added regularly. Contributions and feedback are welcome!

