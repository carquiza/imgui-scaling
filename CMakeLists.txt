# ImGuiScaling - Unified scaling system for ImGui widgets
# https://github.com/user/imgui-scaling

cmake_minimum_required(VERSION 3.14)
project(ImGuiScaling VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This is a header-only library
add_library(ImGuiScaling INTERFACE)
add_library(ImGuiScaling::ImGuiScaling ALIAS ImGuiScaling)

target_include_directories(ImGuiScaling INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Note: ImGui types (ImVec2) are used but imgui.h is expected to be
# included by the consuming project before ImGuiScaling headers.
# This keeps ImGuiScaling dependency-free.
