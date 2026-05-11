cmake_minimum_required(VERSION 4.0)

project(e_paper_emulator)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)


FetchContent_Declare(
        raylib
        GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
        GIT_TAG "master"
        GIT_PROGRESS TRUE
)

FetchContent_Declare(
        json
        GIT_REPOSITORY "https://github.com/nlohmann/json.git"
        GIT_TAG "master"
        GIT_PROGRESS TRUE
)

FetchContent_Declare(
        httplib
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
        GIT_TAG master
)

set(raylib_USE_STATIC_LIBS OFF CACHE BOOL "" FORCE)
set(json_USE_STATIC_LIBS OFF CACHE BOOL "" FORCE)
set(httplib_USE_STATIC_LIBS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(raylib json httplib)

set(PROJECT_ROOT ${CMAKE_SOURCE_DIR}/..)
set(ARDUINO_DIR ${PROJECT_ROOT}/Arduino)

set(SOURCES
        src/main.cpp
        src/util/dev/console/console.cpp
        src/util/dev/console/command/registry.cpp
        src/util/dev/console/command/auto_completion.cpp
        src/util/dev/console/command/commands/core_command.cpp
        src/manager/console_manager.cpp
        src/ui/input_field.cpp
        src/ui/button.cpp
        src/gui/gui_raylib.cpp
        src/simulate/simulate.cpp

        ${ARDUINO_DIR}/src/core/core.cpp
        ${ARDUINO_DIR}/src/canvas/gui_paint.cpp
        ${ARDUINO_DIR}/src/font/font8.cpp
        ${ARDUINO_DIR}/src/font/font12.cpp
        ${ARDUINO_DIR}/src/font/font16.cpp
        ${ARDUINO_DIR}/src/font/font20.cpp
        ${ARDUINO_DIR}/src/font/font24.cpp
        ${ARDUINO_DIR}/src/e-paper/paper_command.cpp
        ${ARDUINO_DIR}/src/util/base_64_decode.cpp
        ${ARDUINO_DIR}/src/core/command/helper.cpp
        ${ARDUINO_DIR}/src/core/command/clear.cpp
        ${ARDUINO_DIR}/src/core/command/set_rotate.cpp
        ${ARDUINO_DIR}/src/core/command/set_mirroring.cpp
        ${ARDUINO_DIR}/src/core/command/set_pixel.cpp
        ${ARDUINO_DIR}/src/core/command/clear_window.cpp
        ${ARDUINO_DIR}/src/core/command/draw_point.cpp
        ${ARDUINO_DIR}/src/core/command/draw_line.cpp
        ${ARDUINO_DIR}/src/core/command/draw_rectangle.cpp
        ${ARDUINO_DIR}/src/core/command/draw_circle.cpp
        ${ARDUINO_DIR}/src/core/command/draw_image.cpp
        ${ARDUINO_DIR}/src/core/command/draw_char.cpp
        ${ARDUINO_DIR}/src/core/command/draw_num.cpp
        ${ARDUINO_DIR}/src/core/command/draw_time.cpp
        ${ARDUINO_DIR}/src/core/command/draw_string.cpp

)
set(HEADERS
        src/gui/gui_raylib.h
        src/ui/input_field.h
        src/ui/button.h
        src/util/dev/console/console.h
        src/util/dev/console/command/registry.h
        src/util/dev/console/command/auto_completion.h
        src/util/dev/console/command/commands/core_command.h
        src/manager/console_manager.h

        ${ARDUINO_DIR}/src/debug_custom/debug.h
        ${ARDUINO_DIR}/src/core/core.h
        ${ARDUINO_DIR}/src/network/request.h
        ${ARDUINO_DIR}/src/e-paper/paper_command.h
        ${ARDUINO_DIR}/src/font/fonts.h
        ${ARDUINO_DIR}/src/canvas/gui_paint.h
        src/simulate/simulate.h

)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    add_executable(e_paper_emulator WIN32 ${SOURCES} ${HEADERS})

    if(MSVC)
        target_link_options(e_paper_emulator PRIVATE
                "/SUBSYSTEM:WINDOWS"
                "/ENTRY:mainCRTStartup"
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_link_options(e_paper_emulator PRIVATE
                "-Wl,--subsystem,windows"
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_link_options(e_paper_emulator PRIVATE
                "-Wl,--subsystem,windows"
        )
    endif()
elseif(APPLE)
    add_executable(e_paper_emulator MACOSX_BUNDLE ${SOURCES} ${HEADERS})
elseif(UNIX AND NOT APPLE)
    add_executable(e_paper_emulator ${SOURCES} ${HEADERS})
endif()

if(ARDUINO)
    target_sources(e_paper_emulator PRIVATE
            ${ARDUINO_DIR}/src/debug_custom/debug.cpp
            ${ARDUINO_DIR}/src/network/request.cpp
    )
else()
    target_sources(e_paper_emulator PRIVATE
            src/debug_custom/debug.cpp
            src/network/request.cpp
    )
endif()

if (WIN32)
    target_compile_definitions(e_paper_emulator PRIVATE
            _WIN32_WINNT=0x0A00
            WINVER=0x0A00
    )
endif()

target_include_directories(e_paper_emulator
        PUBLIC
        include
        ${ARDUINO_DIR}/src
        ${CMAKE_SOURCE_DIR}/src
)

set_property(TARGET e_paper_emulator PROPERTY C_STANDARD 17)

target_link_libraries(e_paper_emulator PRIVATE raylib nlohmann_json::nlohmann_json httplib)

target_compile_definitions(e_paper_emulator PUBLIC GAME_VERSION="1.0.0")
target_compile_definitions(e_paper_emulator PUBLIC MEMORY_RUNTIME_SAFETY=1)

if(WIN32)
    target_compile_definitions(e_paper_emulator PUBLIC PLATFORM_WINDOWS)
elseif(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Build architectures for macOS" FORCE)

    target_compile_definitions(e_paper_emulator PUBLIC PLATFORM_MACOS)

    # Link necessary macOS frameworks for raylib
    target_link_libraries(e_paper_emulator PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo" "-framework OpenGL")
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(e_paper_emulator PUBLIC PLATFORM_LINUX)
endif()