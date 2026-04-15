# blWidgets Coding Agent Prompts

## System Prompt

You are a senior C++ systems engineer tasked with building a prototype UI framework called **blWidgets**.

### Project Vision
blWidgets is a cross-platform C++ UI framework inspired by:
- wxWidgets architecture (object-oriented widgets, event system, sizers)
- Flutter rendering philosophy (custom rendering, no native widgets)
- Blend2D rendering engine (all UI drawn manually)

### Requirements
- Use C++17 or later
- Use Blend2D for all rendering
- Do NOT use native OS widgets (only for window creation)
- Follow a retained-mode UI architecture

### Architecture
Layers:
- Core → application lifecycle
- Platform → OS window + input
- Rendering → Blend2D integration
- UI → widgets, layout, events

### Rendering Flow
1. Poll OS events
2. Convert to internal events
3. Dispatch to widget tree
4. Layout pass (if needed)
5. Render pass using Blend2D
6. Present frame

### Blend2D Rules
- Use BLImage as framebuffer
- Use BLContext for drawing
- No OS drawing APIs

### Core Classes
- blApp
- blWindow
- blRenderContext
- blWidget
- blEvent
- blEventDispatcher
- blSizer (later)

### Deliverables
- CMake project
- Working window
- Blend2D rendering
- Interactive widget

---

## Task Prompt

Build the first working prototype of **blWidgets** using Blend2D.

### Objective
Create a minimal UI framework with:
- Window creation
- Blend2D rendering
- Widget system
- One interactive button

### Project Structure
/core  
/platform  
/rendering  
/ui  
/app  

### Components

#### blWindow
- Create window (GLFW preferred)
- Handle input
- Manage render loop

#### blRenderContext
- Wrap BLImage and BLContext
- Clear and manage drawing

#### blWidget
```cpp
class blWidget {
public:
    virtual void Paint(BLContext& ctx) = 0;
    virtual void OnEvent(const blEvent& event);
};
```

#### blEvent
Types:
- MouseDown
- MouseUp
- MouseMove

#### blButton
```cpp
class blButton : public blWidget {
public:
    blButton(int x, int y, int w, int h);
    void Paint(BLContext& ctx) override;
    void OnEvent(const blEvent& event) override;
};
```

### Behavior
- Draw rectangle
- Change color on hover
- Change color on click
- Print message on click

### Rendering Loop
1. Clear framebuffer
2. Paint widgets
3. End context
4. Present frame

### Expected Result
- Window opens
- Button renders
- Hover + click works


---

## Phase 2: Layout System (Sizer-Based)

### Objective
Extend blWidgets with a basic layout system inspired by wxWidgets sizers.

### Goals
- Remove manual positioning
- Introduce automatic layout
- Support vertical and horizontal stacking

---

### New Components

#### blSizer (Base Class)
```cpp
class blSizer {
public:
    virtual void Layout(blWidget* parent) = 0;
    virtual void Add(std::shared_ptr<blWidget> child, int proportion = 0) = 0;
    virtual ~blSizer() = default;
};
```

---

#### blBoxSizer
```cpp
class blBoxSizer : public blSizer {
public:
    enum Orientation {
        Horizontal,
        Vertical
    };

    blBoxSizer(Orientation orient);

    void Layout(blWidget* parent) override;
    void Add(std::shared_ptr<blWidget> child, int proportion = 0) override;

private:
    Orientation orientation;
    std::vector<std::shared_ptr<blWidget>> children;
};
```

---

### Layout Behavior

#### Vertical Layout
- Stack widgets top to bottom
- Each widget gets:
  - Full width of parent
  - Height based on preferred size

#### Horizontal Layout
- Stack widgets left to right
- Each widget gets:
  - Full height of parent
  - Width based on preferred size

---

### Widget Updates

Update `blWidget` to support:
```cpp
class blWidget {
public:
    virtual void Paint(BLContext& ctx) = 0;
    virtual void OnEvent(const blEvent& event);

    void SetPosition(int x, int y);
    void SetSize(int w, int h);

    int X() const;
    int Y() const;
    int Width() const;
    int Height() const;

protected:
    int x, y, width, height;
};
```

---

### Window Integration

- Add support for root sizer:
```cpp
void SetRootSizer(std::shared_ptr<blSizer> sizer);
```

- During render loop:
  1. Call `sizer->Layout(rootWidget)`
  2. Then call `Paint()`

---

### Example Usage (main.cpp)

```cpp
auto rootSizer = std::make_shared<blBoxSizer>(blBoxSizer::Vertical);

auto button1 = std::make_shared<blButton>("Button 1");
auto button2 = std::make_shared<blButton>("Button 2");

rootSizer->Add(button1);
rootSizer->Add(button2);

window->SetRootSizer(rootSizer);
```

---

### Expected Result

- Widgets automatically arranged
- No manual x/y positioning
- UI scales better with window size

---

### Notes

- Keep layout logic simple (no flex yet)
- No padding/margin system yet
- Focus on correctness over features

---

### Next Steps (Future Phases)

- Flex layout (grow/shrink)
- Alignment (center, end, space-between)
- Padding and margins
- Nested layouts


---

## Phase 3: Widget Tree & Dirty Rendering

### Objective
Introduce a proper widget tree and efficient rendering using dirty flags.

### Goals
- Support parent-child hierarchy
- Avoid full redraw every frame
- Enable partial updates

---

### Widget Tree

Update blWidget:

```cpp
class blWidget {
public:
    void AddChild(std::shared_ptr<blWidget> child);

    virtual void Paint(BLContext& ctx);
    virtual void OnEvent(const blEvent& event);

    void MarkDirty();
    bool IsDirty() const;

protected:
    std::vector<std::shared_ptr<blWidget>> children;
    bool dirty = true;
};
```

---

### Rendering Strategy

- Only repaint widgets marked dirty
- Propagate dirty state up/down tree
- Window triggers repaint only when needed

---

### Render Flow

1. If root is dirty:
2. Traverse widget tree
3. Paint only dirty widgets
4. Clear dirty flags

---

### Expected Result

- Better performance
- Scalable UI system

---

## Phase 4: Styling System (CSS-like)

### Objective
Introduce a flexible styling system for widgets.

### Goals
- Separate style from logic
- Enable themes
- Allow reuse of styles

---

### Style Object

```cpp
struct blStyle {
    uint32_t backgroundColor;
    uint32_t textColor;
    int padding;
};
```

---

### Widget Integration

```cpp
class blWidget {
public:
    void SetStyle(const blStyle& style);

protected:
    blStyle style;
};
```

---

### Features

- Default styles
- Override per widget
- Global theme support (later)

---

### Future Enhancements

- CSS-like parser
- Style inheritance
- Dynamic theming

---

## Phase 5: Animation System (Flutter-style)

### Objective
Add animation capabilities to blWidgets.

### Goals
- Smooth transitions
- State-driven animations
- Frame-based updates

---

### Core Concepts

#### Animation Class

```cpp
class blAnimation {
public:
    virtual void Update(float deltaTime) = 0;
    bool IsFinished() const;
};
```

---

### Integration

- Maintain animation list in blApp or blWindow
- Update animations each frame
- Mark affected widgets dirty

---

### Example Use Cases

- Button hover fade
- Size transitions
- Color interpolation

---

### Expected Result

- Smooth UI interactions
- Modern feel

---

## Final Vision

After completing all phases, blWidgets becomes:

- A retained-mode UI framework
- Fully custom rendered (Blend2D)
- Layout-driven
- Styleable
- Animatable

---

## Long-Term Ideas

- XML/JSON UI layouts
- DevTools (UI inspector)
- GPU backend (future replacement for Blend2D)
- Cross-platform packaging

