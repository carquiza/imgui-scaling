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

# Try to find ImGui if not already provided (e.g. by vcpkg)
if(NOT TARGET imgui AND NOT TARGET imgui::imgui)
    find_package(imgui CONFIG QUIET)
endif()

if(TARGET imgui)
    target_link_libraries(ImGuiScaling PUBLIC imgui)
    message(STATUS "ImGuiScaling: Using 'imgui' target")
elseif(TARGET imgui::imgui)
    target_link_libraries(ImGuiScaling PUBLIC imgui::imgui)
    message(STATUS "ImGuiScaling: Using 'imgui::imgui' target")
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

# =============================================================================
# Installation rules (for vcpkg and find_package support)
# =============================================================================
# Only include install rules when built as a standalone project or via vcpkg,
# not when included as a subdirectory via add_subdirectory/FetchContent.

# Check if we're the top-level project (CMake 3.21+ provides PROJECT_IS_TOP_LEVEL)
if(CMAKE_VERSION VERSION_LESS "3.21")
    get_directory_property(_has_parent PARENT_DIRECTORY)
    if(_has_parent)
        set(PROJECT_IS_TOP_LEVEL OFF)
    else()
        set(PROJECT_IS_TOP_LEVEL ON)
    endif()
endif()

if(PROJECT_IS_TOP_LEVEL OR IMGUISCALING_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)

    # Install the library
    install(TARGETS ImGuiScaling
        EXPORT ImGuiScalingTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    # Install headers
    install(DIRECTORY include/ImGuiScaling
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILES_MATCHING PATTERN "*.hpp"
    )

    # Create and install CMake config files
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/ImGuiScalingConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ImGuiScalingConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/ImGuiScalingConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImGuiScaling
    )

    install(EXPORT ImGuiScalingTargets
        FILE ImGuiScalingTargets.cmake
        NAMESPACE ImGuiScaling::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImGuiScaling
    )

    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/ImGuiScalingConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/ImGuiScalingConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ImGuiScaling
    )
endif()
