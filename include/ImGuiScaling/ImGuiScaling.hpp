// ImGuiScaling.hpp
// Unified scaling system for ImGui widgets
// https://github.com/user/imgui-scaling

#pragma once

namespace ImGuiScaling {

/**
 * @brief Scale configuration for DPI and user preferences
 *
 * Combines platform DPI scaling with user preference scaling
 * to produce an effective scale factor for UI elements.
 */
struct ScaleConfig {
    float dpiScale = 1.0f;      ///< From platform (glfwGetWindowContentScale, etc.)
    float userScale = 1.0f;     ///< From user preference (Ctrl+Plus/Minus, settings)

    /// Get combined effective scale factor
    float GetEffectiveScale() const { return dpiScale * userScale; }

    /// Create config with specific effective scale
    static ScaleConfig FromEffective(float effective) {
        ScaleConfig config;
        config.dpiScale = 1.0f;
        config.userScale = effective;
        return config;
    }

    /// Create config with DPI and user scales
    static ScaleConfig Create(float dpi, float user) {
        ScaleConfig config;
        config.dpiScale = dpi;
        config.userScale = user;
        return config;
    }
};

/**
 * @brief Common base sizes for ImGui widgets (at 1.0x scale)
 *
 * These constants define standard UI element sizes. Multiply by
 * the effective scale factor when using in your widgets.
 *
 * Usage:
 * @code
 * float buttonHeight = ImGuiScaling::BaseSize::BUTTON_HEIGHT * scale;
 * if (ImGui::Button("OK", ImVec2(0, buttonHeight))) { ... }
 * @endcode
 */
namespace BaseSize {
    // Common widget sizes (desktop mode)
    constexpr float BUTTON_HEIGHT = 28.0f;
    constexpr float BUTTON_WIDTH = 80.0f;
    constexpr float BUTTON_WIDTH_SMALL = 60.0f;
    constexpr float BUTTON_WIDTH_LARGE = 120.0f;

    constexpr float INPUT_HEIGHT = 26.0f;
    constexpr float ROW_HEIGHT = 24.0f;
    constexpr float ICON_SIZE = 18.0f;
    constexpr float FONT_SIZE = 14.0f;

    // Window/dialog sizes
    constexpr float DIALOG_WIDTH = 400.0f;
    constexpr float DIALOG_HEIGHT = 300.0f;
    constexpr float DIALOG_WIDTH_LARGE = 650.0f;
    constexpr float DIALOG_HEIGHT_LARGE = 450.0f;
    constexpr float DIALOG_MIN_WIDTH = 300.0f;
    constexpr float DIALOG_MIN_HEIGHT = 200.0f;

    // Spacing
    constexpr float SPACING = 8.0f;
    constexpr float SPACING_SMALL = 4.0f;
    constexpr float SPACING_LARGE = 16.0f;
    constexpr float PADDING = 8.0f;

    // Scrollbar
    constexpr float SCROLLBAR_WIDTH = 16.0f;
    constexpr float GRAB_MIN_SIZE = 16.0f;

    // Touch mode sizes (larger for finger-friendly targets)
    namespace Touch {
        constexpr float BUTTON_HEIGHT = 48.0f;
        constexpr float BUTTON_WIDTH = 120.0f;
        constexpr float INPUT_HEIGHT = 48.0f;
        constexpr float ROW_HEIGHT = 52.0f;
        constexpr float ICON_SIZE = 28.0f;
        constexpr float FONT_SIZE = 16.0f;
        constexpr float SCROLLBAR_WIDTH = 40.0f;
        constexpr float GRAB_MIN_SIZE = 40.0f;
    }
}

/**
 * @brief Mixin interface for scalable widgets
 *
 * Inherit from this to add standard scaling support to your widget.
 *
 * Usage:
 * @code
 * class MyWidget : public ImGuiScaling::Scalable {
 * public:
 *     void Render() {
 *         float btnHeight = BaseSize::BUTTON_HEIGHT * GetScale();
 *         // ...
 *     }
 * };
 * @endcode
 */
class Scalable {
public:
    virtual ~Scalable() = default;

    /// Set the UI scale factor (effective scale = dpi * user)
    void SetScale(float scale) {
        if (scale > 0.0f && scale != m_scale) {
            m_prevScale = m_scale;
            m_scale = scale;
            OnScaleChanged();
        }
    }

    /// Get the current UI scale factor
    float GetScale() const { return m_scale; }

    /// Check if scale changed since last frame (call at start of Render)
    bool HasScaleChanged() const { return m_scale != m_prevScale; }

    /// Acknowledge scale change (call after handling in Render)
    void AcknowledgeScaleChange() { m_prevScale = m_scale; }

protected:
    /// Override to handle scale changes (e.g., recalculate sizes)
    virtual void OnScaleChanged() {}

    /// Helper to scale a value
    float Scaled(float value) const { return value * m_scale; }

    /// Helper to scale for touch mode
    float ScaledTouch(float desktopValue, float touchValue, bool touchMode) const {
        return (touchMode ? touchValue : desktopValue) * m_scale;
    }

private:
    float m_scale = 1.0f;
    float m_prevScale = 1.0f;
};

/**
 * @brief Helper to scale a value
 */
inline float Scale(float v, float scale) {
    return v * scale;
}

/**
 * @brief Get the global user scale value
 * @return Current user scale (persisted to imgui.ini)
 */
float GetUserScale();

/**
 * @brief Set the global user scale value
 * @param scale New user scale (will be persisted to imgui.ini)
 */
void SetUserScale(float scale);

/**
 * @brief Register ImGui settings handler for scale persistence
 *
 * Call this once after ImGui::CreateContext() to enable automatic
 * saving/loading of the user scale to imgui.ini.
 *
 * Usage:
 * @code
 * ImGui::CreateContext();
 * ImGuiScaling::RegisterSettingsHandler();
 * // ... rest of initialization
 * @endcode
 */
void RegisterSettingsHandler();

} // namespace ImGuiScaling

// Optional: Include this after imgui.h for ImVec2 helpers
#ifdef IMGUI_VERSION
namespace ImGuiScaling {

/**
 * @brief Helper to scale ImVec2 (requires imgui.h to be included first)
 */
inline ImVec2 Scale(const ImVec2& v, float scale) {
    return ImVec2(v.x * scale, v.y * scale);
}

} // namespace ImGuiScaling
#endif
