cmake_minimum_required(VERSION 3.14...3.25)

project(AutoRegistry LANGUAGES CXX VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(WIN32_USE_MP ON)

include(FetchContent)
find_package(Boost COMPONENTS json)

set (ENTT_INCLUDE_HEADERS ON)
# FetchContent added in CMake 3.11, downloads during the configure step
# FetchContent_MakeAvailable was added in CMake 3.14; simpler usage
FetchContent_Declare(
    EnTT
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.10.3)
FetchContent_MakeAvailable(EnTT)

FetchContent_Declare(
    imgui-docking
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG docking
    )
FetchContent_MakeAvailable(imgui-docking)

FetchContent_Declare(
    SDL
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG release-2.26.2
)
FetchContent_MakeAvailable(SDL)

set(AutoRegistry_Sources
    ./src/main.cpp
    ./src/ComponentAutoFactory.cpp
    ./src/ComponentRegistry.cpp
    ./src/EntityArchive.cpp
    ./src/Components.cpp
    ./src/Components/ComponentExample.cpp
    ./src/Components/Name.cpp
    ./src/Components/Position.cpp
    ./src/Components/Script.cpp
    ./src/Components/Velocity.cpp
    ./src/ImguiUtils/imgui_impl.cpp
    ./src/Systems/GUISystem.cpp
    ./src/Widgets/DrawEntityWidget.cpp
    ./src/Widgets/EntityEditor.cpp
    ./src/MainWindowSDL.cpp    
)

set(AutoRegistry_Headers
    ./src/better_enums.h
    ./src/ComponentAutoFactory.h
    ./src/ComponentList.h
    ./src/Components.h
    ./src/ComponentRegistry.h
    ./src/EntityArchive.h
    ./src/Components/ComponentExample.h
    ./src/Components/Name.h
    ./src/Components/Position.h
    ./src/Components/Script.h
    ./src/Components/Velocity.h
    ./src/ImguiUtils/RAIID.h
    ./src/ImguiUtils/UIState.h
    ./src/ImguiUtils/ImGuiHelpers.h
    ./src/Systems/GUISystem.h
    ./src/Widgets/DrawEntityWidget.h
    ./src/Widgets/EntityEditor.h
    ./src/MainWindowSDL.h
)

add_executable(AutoRegistry ${AutoRegistry_Sources})
target_compile_features(AutoRegistry PUBLIC cxx_std_17)
target_include_directories(AutoRegistry PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/Components"
    "${PROJECT_SOURCE_DIR}/src/ImguiUtils"
    "${PROJECT_SOURCE_DIR}/src/Systems"
    "${PROJECT_SOURCE_DIR}/src/Widgets"
    "${entt_SOURCE_DIR}/src"
    "${imgui-docking_SOURCE_DIR}"
    "${sdl_SOURCE_DIR}/include"
)

target_compile_definitions(AutoRegistry PRIVATE ENTT_NO_ETO)

message(STATUS "Libraries: ${imgui-docking_SOURCE_DIR}")
target_link_libraries(AutoRegistry SDL2-static SDL2main)