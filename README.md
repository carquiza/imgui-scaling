# ImGuiScaling

A header-only library providing a unified scaling system for ImGui widgets.

## Features

- **ScaleConfig**: Combines DPI scale and user preference scale
- **BaseSize**: Standard UI element sizes at 1.0x scale (desktop and touch modes)
- **Scalable**: Mixin class for adding scaling support to widgets
- **Scale Persistence**: Automatically saves/restores user scale preference via imgui.ini
- **Header-only**: No build required, just include

## Integration

### Option 1: FetchContent

```cmake
include(FetchContent)
FetchContent_Declare(
    ImGuiScaling
    GIT_REPOSITORY https://github.com/carquiza/imgui-scaling.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(ImGuiScaling)

target_link_libraries(YourWidget PRIVATE ImGuiScaling::ImGuiScaling)
```

### Option 2: Subdirectory

```cmake
add_subdirectory(external/imgui-scaling)
target_link_libraries(YourWidget PRIVATE ImGuiScaling::ImGuiScaling)
```

### Option 3: Copy Header

Just copy `include/ImGuiScaling/ImGuiScaling.hpp` to your project.

## Usage

### Basic Scaling

```cpp
#include <ImGuiScaling/ImGuiScaling.hpp>

// Create scale config
ImGuiScaling::ScaleConfig scaleConfig;
scaleConfig.dpiScale = 1.5f;   // From glfwGetWindowContentScale()
scaleConfig.userScale = 1.0f;  // From user preference

float scale = scaleConfig.GetEffectiveScale();  // 1.5f

// Use with base sizes
float buttonHeight = ImGuiScaling::BaseSize::BUTTON_HEIGHT * scale;  // 42px
float rowHeight = ImGuiScaling::BaseSize::ROW_HEIGHT * scale;        // 36px
```

### Scalable Widget Class

```cpp
#include "imgui.h"
#include <ImGuiScaling/ImGuiScaling.hpp>

class MyDialog : public ImGuiScaling::Scalable {
public:
    void Open(float scale) {
        SetScale(scale);
        m_isOpen = true;
    }

    void Render() {
        if (!m_isOpen) return;

        // Check and handle scale changes
        if (HasScaleChanged()) {
            // Recalculate cached sizes if needed
            AcknowledgeScaleChange();
        }

        // Use scaled sizes
        using namespace ImGuiScaling::BaseSize;

        float btnHeight = Scaled(BUTTON_HEIGHT);
        float btnWidth = Scaled(BUTTON_WIDTH);

        if (ImGui::Button("OK", ImVec2(btnWidth, btnHeight))) {
            m_isOpen = false;
        }
    }

private:
    bool m_isOpen = false;
};
```

### Touch Mode

```cpp
bool touchMode = true;
float scale = 1.5f;

// Select appropriate base size
float rowHeight = touchMode
    ? ImGuiScaling::BaseSize::Touch::ROW_HEIGHT * scale   // 78px
    : ImGuiScaling::BaseSize::ROW_HEIGHT * scale;         // 36px
```

### Using the Scaled Helper

```cpp
class MyWidget : public ImGuiScaling::Scalable {
    void Render() {
        // Use ScaledTouch for touch-aware sizing
        float rowHeight = ScaledTouch(
            BaseSize::ROW_HEIGHT,        // Desktop: 24px
            BaseSize::Touch::ROW_HEIGHT, // Touch: 52px
            m_touchMode
        );
    }
};
```

### Scale Persistence (imgui.ini)

ImGuiScaling can automatically save and restore the user's scale preference using ImGui's settings system. This requires registering a settings handler during initialization.

**Important**: The initialization order matters. You must:
1. Call `ImGui::CreateContext()`
2. Call `ImGuiScaling::RegisterSettingsHandler()`
3. Call `ImGui::LoadIniSettingsFromDisk()` to load persisted settings

```cpp
// During application initialization
IMGUI_CHECKVERSION();
ImGui::CreateContext();

// Register settings handler (must be after CreateContext, before LoadIniSettings)
ImGuiScaling::RegisterSettingsHandler();

// Explicitly load settings after handlers are registered
ImGui::LoadIniSettingsFromDisk(ImGui::GetIO().IniFilename);

// Now retrieve the persisted scale and apply it
float userScale = ImGuiScaling::GetUserScale();  // Returns 1.0f if not previously set
ImGui::GetIO().FontGlobalScale = userScale;

// Continue with rest of ImGui setup...
ImGui_ImplGlfw_InitForOpenGL(window, true);
ImGui_ImplOpenGL3_Init(glsl_version);
```

When the user changes the scale (e.g., via Ctrl+Plus/Minus), update and persist it:

```cpp
void OnUserScaleChanged(float newScale) {
    // Clamp to reasonable range
    newScale = std::clamp(newScale, 0.5f, 3.0f);

    // Persist for next session (saved to imgui.ini on shutdown)
    ImGuiScaling::SetUserScale(newScale);

    // Apply to ImGui
    ImGui::GetIO().FontGlobalScale = newScale;

    // Update any Scalable widgets
    myDialog.SetScale(newScale);
}
```

The settings are stored in a `[ImGuiScaling][Data]` section:
```ini
[ImGuiScaling][Data]
UserScale=1.500
```

### Global Scale Functions

- `GetUserScale()` - Get the current/persisted user scale (default: 1.0f)
- `SetUserScale(float)` - Set the user scale (will be persisted on shutdown)
- `RegisterSettingsHandler()` - Register ImGui settings handler for persistence

## Base Sizes Reference

### Desktop Mode (default)

| Constant | Value | Description |
|----------|-------|-------------|
| `BUTTON_HEIGHT` | 28px | Standard button height |
| `BUTTON_WIDTH` | 80px | Standard button width |
| `INPUT_HEIGHT` | 26px | Text input field height |
| `ROW_HEIGHT` | 24px | List/table row height |
| `ICON_SIZE` | 18px | Icon dimensions |
| `SPACING` | 8px | Standard spacing |
| `SCROLLBAR_WIDTH` | 16px | Scrollbar width |

### Touch Mode

| Constant | Value | Description |
|----------|-------|-------------|
| `Touch::BUTTON_HEIGHT` | 48px | Touch-friendly button |
| `Touch::ROW_HEIGHT` | 52px | Touch-friendly row |
| `Touch::ICON_SIZE` | 28px | Larger icons |
| `Touch::SCROLLBAR_WIDTH` | 40px | Wide scrollbar for fingers |

## License

MIT License - See LICENSE file for details.
