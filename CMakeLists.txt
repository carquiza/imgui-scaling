# ImGuiScaling - Unified scaling system for ImGui widgets
# https://github.com/carquiza/imgui-scaling

cmake_minimum_required(VERSION 3.14)
project(ImGuiScaling VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Library sources
set(IMGUISCALING_SOURCES
    src/ImGuiScaling.cpp
)

set(IMGUISCALING_HEADERS
    include/ImGuiScaling/ImGuiScaling.hpp
)

# Create static library
add_library(ImGuiScaling STATIC
    ${IMGUISCALING_SOURCES}
    ${IMGUISCALING_HEADERS}
)
add_library(ImGuiScaling::ImGuiScaling ALIAS ImGuiScaling)

target_include_directories(ImGuiScaling PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# =============================================================================
# ImGui dependency
# =============================================================================
# ImGui is required for settings persistence.
# The user must provide imgui as ImGui doesn't ship with CMakeLists.txt.
# =============================================================================

if(TARGET imgui)
    target_link_libraries(ImGuiScaling PUBLIC imgui)
    message(STATUS "ImGuiScaling: Using 'imgui' target")
elseif(TARGET imgui::imgui)
    target_link_libraries(ImGuiScaling PUBLIC imgui::imgui)
    message(STATUS "ImGuiScaling: Using 'imgui::imgui' target (vcpkg)")
elseif(DEFINED IMGUI_INCLUDE_DIR)
    target_include_directories(ImGuiScaling PUBLIC ${IMGUI_INCLUDE_DIR})
    message(STATUS "ImGuiScaling: Using IMGUI_INCLUDE_DIR=${IMGUI_INCLUDE_DIR}")
else()
    message(WARNING "ImGuiScaling: No imgui target or IMGUI_INCLUDE_DIR found.")
    message(WARNING "  Make sure imgui headers are available when compiling.")
endif()

# Compiler options
if(MSVC)
    target_compile_options(ImGuiScaling PRIVATE /W4)
else()
    target_compile_options(ImGuiScaling PRIVATE -Wall -Wextra)
endif()

# Source groups for IDEs
source_group("Header Files" FILES ${IMGUISCALING_HEADERS})
source_group("Source Files" FILES ${IMGUISCALING_SOURCES})
