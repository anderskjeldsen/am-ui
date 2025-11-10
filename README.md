# am-ui

# What is am-ui?

"AmLang" is a new programming language mainly targetted at niche platforms like AmigaOS (Also playing with MorphOS, Nintendo Wii Homebrew, Raspberry Pi Pico, Atari TOS). Read more about it below.

am-ui is a UI library that one could use when programming AmLang. It let's you open a Screen or a Window, add some layouts, labels, buttons etc. Work in progress.

## Available Views

The library includes the following UI components:

### Core Views
- **View** - Base class for all UI components
- **ViewGroup** - Container for child views
- **Button** - Clickable button with text and event handling
- **Label** - Text display
- **Panel** - Basic container with background

### Layout Views  
- **HStack** - Horizontal layout container
- **VStack** - Vertical layout container
- **ScrollView** - Scrollable container
- **ListView** - List of items

### New Views (Recently Added)
- **ImageView** - Display images from Am.Imaging library with scaling options
- **ProgressBar** - Progress indicator with customizable range and styling
- **TextField** - Text input with placeholder, selection, and focus management
- **CheckBox** - Toggle control with label and change events
- **Separator** - Visual divider for layouts

### System Views
- **Window** - Top-level window container
- **Screen** - Screen management
- **Font** - Font handling

See [NEW_VIEWS.md](NEW_VIEWS.md) for detailed documentation on the recently added Views.

# Am Lang Features
AmLang is inspired by other programming languages like Kotlin, Java, C#, TypeScript, Swift etc. 

- ARC (automatic reference counting)
- classes
- interfaces
- namespaces
- suspendable functions
- lambda expressions
- native c support, 
- exceptions (try,catch,throw) 
- and a lot more. 

There are still some major features missing, like for example for-loops. I have to use while loops until I prioritize for-loops 🙂 Currently it compiles only on Mac/Linux/Windows (using Docker), but one of the ultimate goals is to re-write the compiler in its own language. The compiler doesn't generate any machine code on its own, it writes C-code and lets GCC make the machine code. Performance-wise it's not as efficient as C (obviously?), but one can easily let c/asm do the heavy lifting and use this for orchestration. 

# Requirements
- Java 11+
- Docker
- Windows, MacOS, Linux

# Quick start (for AmigaOS3):

Examples are available in the examples/ directory.

For example, to build and run the FileBrowser example:

```bash
cd examples/FileBrowser
java -jar amlc-1.jar . -acmd -bt amigaos_docker_unix-x64
```

Binary will end up in `builds/bin/amigaos/` as a file called "app". Copy the "app" file to an Amiga environment and execute it.

Available build targets:
- `amigaos_docker_unix-x64` - AmigaOS 3.x (using x64 Docker image)
- `amigaos_docker_unix-arm` - AmigaOS 3.x (using ARM Docker image)
- `morphos-ppc_docker` - MorphOS PPC

Additional build options:
- Add more runtime logging: `-rl`
- Add more comments in the code: `-rdc`

Example:
```bash
java -jar amlc-1.jar . -acmd -bt amigaos_docker_unix-x64 -rl -rdc
``` 

The option "-acmd" means that you allow the compiler to run commands from the package.yml file. 

The option "-bt" specifies which build target (from package.yml) you want to use (which platform to build for and how to build)

