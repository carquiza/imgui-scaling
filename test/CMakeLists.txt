# Headless GTest unit tests — opt-in via IMGUISCALING_BUILD_TESTS=ON.
# gtest comes from the manifest's "tests" feature
# (configure with -DVCPKG_MANIFEST_FEATURES=tests).

find_package(GTest CONFIG REQUIRED)

add_executable(imgui_scaling_tests ScalingTests.cpp)

target_link_libraries(imgui_scaling_tests
    PRIVATE
        ImGuiScaling
        GTest::gtest
        GTest::gtest_main
)

if(MSVC)
    target_compile_options(imgui_scaling_tests PRIVATE /W4)
else()
    target_compile_options(imgui_scaling_tests PRIVATE -Wall -Wextra)
endif()

add_test(NAME imgui_scaling_tests COMMAND imgui_scaling_tests)
