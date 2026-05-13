cmake_minimum_required(VERSION 3.20)
project(Gactorio CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile_commands.json for clangd / IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -----------------------------------------------------------------------------
# Dependencies
# -----------------------------------------------------------------------------
# SDL2 (provided by MSYS2 pacman: mingw-w64-ucrt-x86_64-SDL2)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

# Dear ImGui (downloaded automatically)
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.5
)
FetchContent_MakeAvailable(imgui)

# Build ImGui as a static library so we can link from main app
add_library(imgui STATIC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_demo.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC SDL2::SDL2 OpenGL::GL)

# -----------------------------------------------------------------------------
# Main application
# -----------------------------------------------------------------------------
add_executable(gactorio
    src/main.cpp
    src/models/Machine.cpp
    src/models/Factory.cpp
    src/views/AppUI.cpp
    src/views/FactoryPanel.cpp
)

target_include_directories(gactorio PRIVATE src)

target_link_libraries(gactorio PRIVATE
    imgui
    SDL2::SDL2main
    SDL2::SDL2
    OpenGL::GL
)

# Copy SDL2.dll next to the executable on Windows so it runs from the build dir
if(WIN32)
    add_custom_command(TARGET gactorio POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:SDL2::SDL2>
        $<TARGET_FILE_DIR:gactorio>
    )
endif()
