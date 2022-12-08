cmake_minimum_required(VERSION 3.16)
project(imgui CXX)

set(CMAKE_DEBUG_POSTFIX d)

if(APPLE)
    set(CMAKE_CXX_STANDARD 11)
    enable_language(OBJCXX)
endif()

add_library(${PROJECT_NAME} "")
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

find_package(SDL2 CONFIG REQUIRED)

target_sources(
    ${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_demo.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_draw.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_tables.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_widgets.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_stdlib.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_sdl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/imgui_impl_sdlrenderer.cpp
)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)

target_link_libraries(${PROJECT_NAME} PUBLIC SDL2::SDL2)

if(IMGUI_USE_WCHAR32)
    target_compile_definitions(${PROJECT_NAME} PUBLIC IMGUI_USE_WCHAR32)
endif()
