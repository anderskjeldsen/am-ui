# New Views Documentation

This document describes the additional Views that have been added to the am-ui library.

## ImageView

The `ImageView` class displays images from the `Am.Imaging` library.

### Features
- Display `Am.Imaging.Image` objects (both ARGB and Indexed pixel formats)
- Automatic sizing based on image dimensions
- Optional scaling to fit view bounds
- Aspect ratio preservation option
- Integration with the existing View system

### Usage
```aml
import Am.Imaging
var image = Am.Imaging.Image.createARGB(100US, 50US)
var imageView = new ImageView(image)
imageView.scaleToFit = true
imageView.maintainAspectRatio = true
```

### Properties
- `image: Am.Imaging.Image` - The image to display
- `scaleToFit: Bool` - Whether to scale the image to fit the view bounds (default: false)
- `maintainAspectRatio: Bool` - Whether to maintain aspect ratio when scaling (default: true)

## ProgressBar

The `ProgressBar` class shows progress with a visual bar and optional percentage text.

### Features
- Customizable value range (minimum/maximum)
- Optional percentage text display
- Customizable colors for background, progress, and text
- Visual feedback for progress updates

### Usage
```aml
var progressBar = new ProgressBar(0, 100)
progressBar.setValue(75) // 75% complete
progressBar.showText = true
```

### Properties
- `minimum: Int` - Minimum value (default: 0)
- `maximum: Int` - Maximum value (default: 100)
- `showText: Bool` - Whether to show percentage text (default: true)
- `textPen: UByte` - Color for percentage text
- `progressPen: UByte` - Color for progress bar fill
- `backgroundPen: UByte` - Background color

### Methods
- `setValue(value: Int)` - Set current progress value
- `getValue(): Int` - Get current progress value
- `setRange(minimum: Int, maximum: Int)` - Set value range
- `getPercentage(): Float` - Get progress as percentage (0.0 to 1.0)

## TextField

The `TextField` class provides text input functionality with placeholder support.

### Features
- Text input with cursor positioning
- Placeholder text when empty
- Password mode (displays asterisks)
- Text selection support
- Maximum length limiting
- Read-only mode
- Focus management

### Usage
```aml
var textField = new TextField("", "Enter your text here...")
textField.maxLength = 50
textField.passwordMode = false
```

### Properties
- `text: String` - Current text value
- `placeholder: String` - Placeholder text shown when empty
- `maxLength: Int` - Maximum text length (default: 255)
- `readOnly: Bool` - Whether the field is read-only (default: false)
- `passwordMode: Bool` - Whether to hide text with asterisks (default: false)

### Methods
- `getText(): String` - Get current text
- `setText(text: String)` - Set text value
- `setFocused(focused: Bool)` - Set focus state
- `isFocused(): Bool` - Check if focused
- `selectAll()` - Select all text
- `insertText(newText: String)` - Insert text at cursor position

## CheckBox

The `CheckBox` class provides a toggle control with label and change listener.

### Features
- Toggle state (checked/unchecked)
- Text label
- Change event callback
- Visual feedback for hover and active states
- Keyboard support (Space key to toggle)

### Usage
```aml
var checkBox = new CheckBox("Enable feature", false, fun(cb: CheckBox): Bool {
    ("Checkbox toggled: " + cb.isChecked().toString()).println()
    return true
})
```

### Properties
- `text: String` - Label text
- `checked: Bool` - Current checked state
- `changeListener: (checkBox: CheckBox) => Bool` - Callback for state changes

### Methods
- `isChecked(): Bool` - Get checked state
- `setChecked(checked: Bool)` - Set checked state
- `toggle()` - Toggle the checked state

## Separator

The `Separator` class creates visual dividers in layouts.

### Features
- Horizontal or vertical orientation
- Customizable thickness and color
- Minimal space requirements
- Integration with layout containers

### Usage
```aml
var separator = new Separator(SeparatorOrientation.Horizontal)
separator.thickness = 2S
separator.separatorPen = 2UB
```

### Properties
- `orientation: SeparatorOrientation` - Horizontal or Vertical
- `separatorPen: UByte` - Color for the separator line
- `thickness: Short` - Line thickness in pixels (default: 1)

### Enums
- `SeparatorOrientation.Horizontal` - Horizontal separator
- `SeparatorOrientation.Vertical` - Vertical separator

## Example Usage

See `ExampleNewViews.aml` for a complete example demonstrating all the new Views in a working application.

## Platform Support

All new Views include:
- AmigaOS native implementation
- MorphOS native implementation  
- libc stub implementation for testing

The ImageView includes a basic placeholder implementation in the native graphics layer. Full image rendering will require integration with the am-imaging native implementations.