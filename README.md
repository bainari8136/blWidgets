# blWidgets 

Minimal retained-mode C++ UI framework prototype using Blend2D for UI rendering and GLFW for window/input handling.

## Features

- Window creation and event loop via GLFW.
- Full UI rasterization into a Blend2D framebuffer (`BLImage` + `BLContext`).
- OpenGL texture presentation bridge (uploads framebuffer and draws fullscreen quad).
- Widget hierarchy with `blWidget` base class for extensible UI components.
- Sizer-based automatic layout system (`blBoxSizer`) for dynamic widget positioning and resizing.
- Event dispatching system (`blEventDispatcher`) for mouse and keyboard input handling.
- CSS-like styling system (`blStyle`) for customizable widget appearance.
- Animation framework (`blAnimation`) for smooth UI transitions.
- Layout invalidation and constraint tracking to ensure proper layout updates on widget changes.
- Interactive widgets: Button and Panel (with internal layout support).

## Architecture

- **Core Components**: Application management (`blApp`), window handling (`blWindow`), rendering context (`blRenderContext`).
- **UI System**: Root widget manager (`blUiRoot`), widget base class (`blWidget`), layout sizers (`blSizer`, `blBoxSizer`).
- **Rendering**: Blend2D-based drawing with OpenGL presentation.
- **Event Handling**: Mouse and keyboard events propagated through the widget hierarchy.

## Build (Windows, CMake + MSVC)

```powershell
cmake -S . -B build
cmake --build build --config Release
```

Executable:

- `build/Release/bl_widgets.exe`

## Run

```powershell
.\build\Release\bl_widgets.exe
```

Expected runtime behavior:

- Window opens with a panel containing a button.
- Widgets automatically resize and reposition on window resize.
- Hover changes button color.
- Mouse down changes button to pressed color.
- Click (press + release inside) prints `blButton clicked: Click Me` to stdout.

## Recent Updates

- Implemented automatic layout system with sizer-based positioning to replace manual layout.
- Added layout invalidation API and constraint change tracking to fix stacking and desync bugs.
- Enhanced Panel widget with internal sizer support for proper child layout management.
- Integrated styling and animation frameworks for enhanced UI capabilities.

## Notes

- All UI drawing is done in Blend2D; OpenGL is used only to present the framebuffer in the GLFW window.
- CMake will try to find installed `glfw3`; if unavailable, it fetches GLFW 3.4 automatically.
- CMake will try to find Blend2D via package config; if not found, it falls back to:
  - `third_party/blend2d`
  - `../blend2d`
  - `../../blend2d`
