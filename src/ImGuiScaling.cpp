// ImGuiScaling.cpp
// Implementation of scaling system with ImGui settings persistence

#include "ImGuiScaling/ImGuiScaling.hpp"
#include "imgui.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace ImGuiScaling {

// Global state
static float g_userScale = 1.0f;
static bool g_settingsHandlerRegistered = false;

float GetUserScale() {
    return g_userScale;
}

void SetUserScale(float scale) {
    if (scale > 0.0f) {
        g_userScale = scale;
        // Mark settings as dirty so ImGui will save on next opportunity
        if (ImGui::GetCurrentContext()) {
            ImGui::MarkIniSettingsDirty();
        }
    }
}

// ImGui settings handler callbacks
static void* SettingsHandler_ReadOpen(ImGuiContext*, ImGuiSettingsHandler*, const char* name) {
    // We only have one entry named "Data"
    if (strcmp(name, "Data") == 0) {
        return (void*)1;  // Non-null to indicate valid entry
    }
    return nullptr;
}

static void SettingsHandler_ReadLine(ImGuiContext*, ImGuiSettingsHandler*, void* entry, const char* line) {
    if (!entry) return;

    float scale;
    if (sscanf(line, "UserScale=%f", &scale) == 1) {
        if (scale > 0.0f && scale <= 10.0f) {  // Sanity check
            g_userScale = scale;
        }
    }
}

static void SettingsHandler_WriteAll(ImGuiContext*, ImGuiSettingsHandler* handler, ImGuiTextBuffer* buf) {
    buf->appendf("[%s][Data]\n", handler->TypeName);
    buf->appendf("UserScale=%.3f\n", g_userScale);
    buf->append("\n");
}

void RegisterSettingsHandler() {
    if (g_settingsHandlerRegistered) return;

    ImGuiContext* ctx = ImGui::GetCurrentContext();
    if (!ctx) return;

    ImGuiSettingsHandler handler;
    handler.TypeName = "ImGuiScaling";
    handler.TypeHash = ImHashStr("ImGuiScaling");
    handler.ReadOpenFn = SettingsHandler_ReadOpen;
    handler.ReadLineFn = SettingsHandler_ReadLine;
    handler.WriteAllFn = SettingsHandler_WriteAll;
    ctx->SettingsHandlers.push_back(handler);

    g_settingsHandlerRegistered = true;
}

} // namespace ImGuiScaling
