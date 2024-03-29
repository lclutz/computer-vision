cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

project(computervision)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    if (MSVC)
        if(MSVC_VERSION LESS 1910)
            message(FATAL_ERROR "Version ${MSVC_VERSION} of MSVC is not supported. At least 1910 is required")
        endif()

        if(NOT CMAKE_VS_PLATFORM_NAME)
            set(CMAKE_VS_PLATFORM_NAME "x64")
        endif()

        if(NOT ("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64") OR NOT (CMAKE_SIZEOF_VOID_P STREQUAL "8"))
            message(FATAL_ERROR "${CMAKE_VS_PLATFORM_NAME} arch is not supported! We only support x64")
        endif()
    else()
        message(FATAL_ERROR "We only support MSVC for Windows builds")
    endif()
endif()

add_executable(${PROJECT_NAME}
    WIN32 MACOSX_BUNDLE
    assets/icon.rc
    src/application.cpp
    src/background_estimator.cpp
    src/gui.cpp
    src/hand_detector.cpp
    src/main.cpp
    src/motion_detector.cpp
    src/position_tracker.cpp
    src/rect_helper.cpp
    src/sdl_opencv_helper.cpp
    src/skin_tone_calibrator.cpp
    src/skin_tone_detector.cpp
    src/webcam.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

# Enable more compiler warnings
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(SDL2 PATHS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2-2.26.1/cmake" CONFIG REQUIRED)
else()
    find_package(SDL2 CONFIG REQUIRED)
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    find_package(OpenCV PATHS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/opencv-4.6.0/build" CONFIG REQUIRED)
else()
    find_package(OpenCV CONFIG REQUIRED)
endif()
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

add_subdirectory(vendor/imgui-1.89)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}> $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMAND_EXPAND_LISTS)
endif()
