cmake_minimum_required(VERSION 3.16.0 FATAL_ERROR)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_C_STANDARD 11 CACHE STRING "The C standard to use")

project(computervision)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (NOT MSVC)
        message(FATAL_ERROR "We only support MSVC for Windows builds!")
    endif()

    if(MSVC_VERSION LESS 1910)
        message(FATAL_ERROR "Version ${MSVC_VERSION} of MSVC is not supported. At least 1910 is required.")
    endif()

    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    if(NOT CMAKE_VS_PLATFORM_NAME)
        set(CMAKE_VS_PLATFORM_NAME "x64")
    endif()

    if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64") OR
       NOT (CMAKE_SIZEOF_VOID_P STREQUAL "8"))
        message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported! We only support x64")
    endif()
endif()

include_directories(include)

add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    assets/icon.rc
    src/application.cpp
    src/gui.cpp
    src/hand_detector.cpp
    src/main.cpp
    src/motion_detector.cpp
    src/skin_tone_calibrator.cpp
    src/webcam.cpp
)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

find_package(SDL2 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2-2.26.1/cmake" CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2main SDL2::SDL2)

add_compile_definitions(CV_IGNORE_DEBUG_BUILD_GUARD)
find_package(OpenCV PATHS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/opencv-4.6.0/build" CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE opencv_world)

add_subdirectory(vendor/imgui-1.89)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

add_subdirectory(vendor/nativefiledialog-116)
target_link_libraries(${PROJECT_NAME} PRIVATE nfd)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS)
endif()
