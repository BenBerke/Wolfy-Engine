cmake_minimum_required(VERSION 4.1)

project(Wolfy_Engine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Keep both executables in the same output folder.
# Useful if the launcher starts Wolfy_Engine.exe.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

find_package(OpenGL REQUIRED)
find_package(SDL3 REQUIRED CONFIG COMPONENTS SDL3 Headers)
find_package(SDL3_image REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)
find_package(Freetype REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

option(WOLFY_USE_IMGUI "Enable Dear ImGui editor UI" ON)

# -------------------------
# GLAD
# -------------------------

add_library(glad STATIC
        glad/src/glad.c
)

target_include_directories(glad PUBLIC
        glad/include
)

target_link_libraries(glad PUBLIC
        OpenGL::GL
)

# -------------------------
# Dear ImGui
# -------------------------

if (WOLFY_USE_IMGUI AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/External/imgui/imgui.h")
    add_library(imgui STATIC
            External/imgui/imgui.cpp
            External/imgui/imgui_draw.cpp
            External/imgui/imgui_tables.cpp
            External/imgui/imgui_widgets.cpp
            External/imgui/imgui_demo.cpp

            External/imgui/backends/imgui_impl_sdl3.cpp
            External/imgui/backends/imgui_impl_sdlrenderer3.cpp
            External/imgui/backends/imgui_impl_opengl3.cpp
    )

    target_include_directories(imgui PUBLIC
            External/imgui
            External/imgui/backends
    )

    target_link_libraries(imgui PUBLIC
            SDL3::SDL3
            SDL3::Headers
            OpenGL::GL
    )
endif()

# -------------------------
# Engine sources
# -------------------------

set(WOLFY_ENGINE_SOURCES
        src/Renderer/TextureManager.cpp

        src/MapEditor/MapEditor.cpp
        src/MapEditor/MapEditorState.cpp
        src/MapEditor/MapEditorGeometry.cpp
        src/MapEditor/MapEditorDrawing.cpp
        src/MapEditor/MapEditorInput.cpp
        src/MapEditor/MapEditorUI.cpp
        src/MapEditor/MapEditorSaveLoad.cpp

        src/Engine/GameTime.cpp
        src/Engine/InputManager.cpp
        src/Engine/Local/Local.cpp

        src/Objects/Player.cpp

        src/Renderer/Renderer/RendererState.cpp
        src/Renderer/Renderer/RendererMath.cpp
        src/Renderer/Renderer/RendererInit.cpp
        src/Renderer/Renderer/RendererText.cpp
        src/Renderer/Renderer/RendererMapBuild.cpp
        src/Renderer/Renderer/RendererSprites.cpp
        src/Renderer/Renderer/RendererDebug.cpp
        src/Renderer/Renderer/RendererUpdate.cpp
        src/Renderer/Renderer/RendererDestroy.cpp
        src/Renderer/Renderer/RendererDecals.cpp
        src/Renderer/Renderer/RendererSectors.cpp
        src/Renderer/Renderer/RenderBackground.cpp
        src/Renderer/Renderer/RendererUI.cpp

        src/Math/Geometry/Geometry.cpp

        src/Map/MapQueries.cpp
        src/Map/LevelManager.cpp

        src/Project/ProjectManager.cpp

        config.h
        Headers/Objects/Entity.hpp
        Headers/Objects/Level.hpp
        Headers/Objects/Components.hpp
        Headers/Objects/Sector.hpp
        Headers/Objects/Wall.hpp
        Headers/Map/MapQueries.hpp
        Headers/Map/LevelManager.hpp
        Headers/Math/Geometry/Geometry.h
        Headers/Engine/Local/Local.hpp
        Headers/Project/ProjectManager.hpp
)

# -------------------------
# Engine target
# Configuration 2: Engine
# Builds engine files only and runs Wolfy_Engine.
# -------------------------

add_executable(Wolfy_Engine
        src/EngineApp/EngineMain.cpp
        ${WOLFY_ENGINE_SOURCES}
)

target_precompile_headers(Wolfy_Engine PRIVATE
        <vector>
        <string>
        <array>
        <memory>
        <map>
        <algorithm>
        <cmath>
        <fstream>
        <filesystem>
        <SDL3/SDL.h>
        <nlohmann/json.hpp>
)

target_include_directories(Wolfy_Engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        src/MapEditor
)

target_link_libraries(Wolfy_Engine PRIVATE
        SDL3::SDL3
        SDL3::Headers
        $<IF:$<TARGET_EXISTS:SDL3_image::SDL3_image-shared>,SDL3_image::SDL3_image-shared,SDL3_image::SDL3_image-static>
        SDL3_ttf::SDL3_ttf
        Freetype::Freetype
        glad
        nlohmann_json::nlohmann_json
)

target_compile_definitions(Wolfy_Engine PRIVATE
        WOLFY_CONTENT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
)

if (WOLFY_USE_IMGUI AND TARGET imgui)
    target_link_libraries(Wolfy_Engine PRIVATE imgui)
    target_compile_definitions(Wolfy_Engine PRIVATE WOLFY_USE_IMGUI=1)
else()
    target_compile_definitions(Wolfy_Engine PRIVATE WOLFY_USE_IMGUI=0)
endif()

# -------------------------
# Launcher target
# Configuration 1: Launcher
# Builds launcher files only and runs Wolfy_Launcher.
# -------------------------

add_executable(Wolfy_Launcher
        src/Launcher/LauncherMain.cpp
        Headers/Project/ProjectManager.hpp
)

target_include_directories(Wolfy_Launcher PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Wolfy_Launcher PRIVATE
        nlohmann_json::nlohmann_json
)

target_compile_definitions(Wolfy_Launcher PRIVATE
        WOLFY_CONTENT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
)

# -------------------------
# All executable
# Configuration 3: All
# Builds engine + launcher-style executable, then runs Wolfy_All.
# -------------------------

add_executable(Wolfy_All
        src/Launcher/LauncherMain.cpp
        Headers/Project/ProjectManager.hpp
)

target_include_directories(Wolfy_All PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Wolfy_All PRIVATE
        nlohmann_json::nlohmann_json
)

target_compile_definitions(Wolfy_All PRIVATE
        WOLFY_CONTENT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
)

add_dependencies(Wolfy_All Wolfy_Engine)