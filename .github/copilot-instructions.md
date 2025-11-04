# AmLang UI Library - GitHub Copilot Instructions

You are working with **AmUI**, the user interface library for AmLang applications. This library provides GUI components and layout management specifically designed for AmigaOS, MorphOS, and other retro platforms, with a modern object-oriented approach.

## Core UI Framework

### Window Management
```amlang
import Am.Ui

class MainWindow : Window {
    fun createWindow() {
        var window = new Window()
        window.setTitle("My Application")
        window.setSize(640, 480)
        window.show()
    }
}
```

### Available UI Components

#### Core View Classes
- **`View`** - Base class for all UI components
- **`ViewGroup`** - Container for child views
- **`Panel`** - Basic container panel
- **`Window`** - Top-level window container
- **`Screen`** - Screen management and display modes

#### Interactive Controls
- **`Button`** - Clickable button control
- **`Label`** - Text display component
- **`ListView`** - List of selectable items
- **`ScrollView`** - Scrollable content container

#### Layout Containers
- **`HStack`** - Horizontal stack layout
- **`VStack`** - Vertical stack layout

#### Graphics and Styling
- **`Font`** - Text font configuration
- **`Graphics`** - Drawing and graphics context
- **`BorderPens`** - Border styling and drawing
- **`Rectangle`** - Geometric rectangle
- **`Insets`** - Padding and margins

#### Input Handling
- **`MouseButton`** - Mouse button enumeration
- **`MouseEventType`** - Mouse event types
- **`KeyboardEventType`** - Keyboard event types

#### Platform Integration
- **`GadToolsBase`** - AmigaOS GadTools integration
- **`GadToolsButton`** - Native Amiga button controls
- **`GadToolsViews`** - Native Amiga view components
- **`ScreenMode`** - Display mode configuration
- **`ScreenModeRequester`** - Screen mode selection dialog
- **`PixelMetrics`** - Display metrics and measurements

#### Support Classes
- **`ViewContext`** - View rendering context
- **`ViewContextGraphics`** - Graphics rendering context
- **`ViewContextImpl`** - Context implementation
- **`ViewExtensions`** - View utility extensions
- **`WindowManager`** - Window management system

## Common UI Patterns

### Basic Application Setup
```amlang
namespace MyApp {
    class MainApp {
        import Am.Ui
        
        static fun main() {
            var screen = new Screen()
            screen.openWorkbench()
            
            var window = new Window()
            window.setTitle("AmLang UI App")
            window.setSize(400, 300)
            
            var panel = new Panel()
            var label = new Label("Hello, AmUI!")
            var button = new Button("Click Me")
            
            // Layout components
            var vstack = new VStack()
            vstack.add(label)
            vstack.add(button)
            
            panel.add(vstack)
            window.setContent(panel)
            window.show()
        }
    }
}
```

### Event Handling
```amlang
class InteractiveWindow : Window {
    import Am.Ui
    
    fun setupEvents() {
        var button = new Button("Action")
        
        button.onClick = { event ->
            "Button clicked!".println()
            handleButtonClick()
        }
        
        this.onMouseMove = { event ->
            var x = event.getX()
            var y = event.getY()
            processMouseMove(x, y)
        }
    }
    
    fun handleButtonClick() {
        // Handle button click event
    }
    
    fun processMouseMove(x: Int, y: Int) {
        // Handle mouse movement
    }
}
```

### Layout Management
```amlang
class LayoutExample : Panel {
    import Am.Ui
    
    fun createLayout() {
        // Horizontal layout
        var hstack = new HStack()
        hstack.add(new Button("Left"))
        hstack.add(new Button("Center"))
        hstack.add(new Button("Right"))
        
        // Vertical layout
        var vstack = new VStack()
        vstack.add(new Label("Title"))
        vstack.add(hstack)
        vstack.add(new Label("Status"))
        
        this.add(vstack)
    }
}
```

### Custom Drawing
```amlang
class CustomView : View {
    import Am.Ui
    
    override fun draw(graphics: Graphics) {
        var rect = new Rectangle(10, 10, 100, 50)
        graphics.drawRectangle(rect)
        
        var font = new Font("topaz", 8)
        graphics.setFont(font)
        graphics.drawText("Custom Content", 15, 25)
    }
}
```

## Platform-Specific Features

### AmigaOS Integration
```amlang
class AmigaWindow : Window {
    import Am.Ui
    
    fun setupAmigaFeatures() {
        // Use native Amiga components
        var gadButton = new GadToolsButton("Native Button")
        var gadViews = new GadToolsViews()
        
        // Configure screen mode
        var screenMode = new ScreenMode()
        screenMode.setResolution(640, 480, 16) // 16-color mode
        
        var screen = new Screen()
        screen.setScreenMode(screenMode)
    }
}
```

### Font and Graphics
```amlang
class StyledView : View {
    import Am.Ui
    
    fun applyStyles() {
        var font = new Font("helvetica", 12)
        font.setBold(true)
        
        var insets = new Insets(10, 10, 10, 10) // top, right, bottom, left
        this.setPadding(insets)
        
        var borderPens = new BorderPens()
        borderPens.setOuterColor(0x000000) // Black border
        borderPens.setInnerColor(0xFFFFFF) // White inner
        this.setBorder(borderPens)
    }
}
```

## Build Configuration

### Package Structure
```yaml
# package.yml
id: my-ui-app
version: 1.0.0
type: application
dependencies:
  - id: am-lang-core
    type: git-repo
    url: https://github.com/anderskjeldsen/am-lang-core.git
  - id: am-ui
    type: git-repo
    url: https://github.com/anderskjeldsen/am-ui.git
```

### Build Targets
```bash
# Build for AmigaOS
amlc build . -bt amigaos

# Build for MorphOS  
amlc build . -bt morphos-ppc

# Build for Linux (development)
amlc build . -bt linux-x64
```

## Common Issues and Solutions

### Memory Management
- UI components use automatic reference counting
- Always call `dispose()` on windows when closing
- Avoid circular references between parent and child views

### Platform Compatibility
- Test on target platforms (AmigaOS/MorphOS) early
- Use feature detection for platform-specific functionality
- Fallback to generic implementations when native features unavailable

### Performance Tips
- Minimize redraws by caching graphics operations
- Use appropriate data structures for large lists
- Optimize font rendering by reusing Font objects

## Best Practices

1. **Component Hierarchy**: Always follow proper parent-child relationships
2. **Event Handling**: Use lambda functions for simple event handlers
3. **Layout Management**: Prefer HStack/VStack over absolute positioning
4. **Resource Management**: Dispose of heavyweight resources (fonts, graphics)
5. **Platform Integration**: Use GadTools for authentic Amiga look and feel
6. **Testing**: Test UI on actual target hardware when possible

## Error Patterns to Avoid

- Don't create UI components without proper parent containers
- Avoid deep nesting of layout containers (performance impact)
- Don't forget to handle window close events
- Avoid hardcoded colors/fonts (use system defaults when possible)
- Don't mix pixel coordinates with layout managers

When suggesting AmUI code, prioritize:
1. **Platform authenticity** - Code should feel native to Amiga/retro systems
2. **Modern patterns** - Use object-oriented design and proper event handling  
3. **Performance** - Consider memory and CPU constraints of target platforms
4. **Maintainability** - Clear component structure and separation of concerns