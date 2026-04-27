cmake_minimum_required(VERSION 4.0)
project(e_paper_emulator)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied emulation games


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

        src/emulation/emulation.cpp
        src/UI/input_field.cpp

        ${ARDUINO_DIR}/src/Canvas/GUI_Paint.cpp
        ${ARDUINO_DIR}/src/Font/font8.cpp
        ${ARDUINO_DIR}/src/Font/font12.cpp
        ${ARDUINO_DIR}/src/Font/font16.cpp
        ${ARDUINO_DIR}/src/Font/font20.cpp
        ${ARDUINO_DIR}/src/Font/font24.cpp
        src/network/request.cpp
        src/e-Paper/paper_command.cpp

        src/util/dev/console/console.cpp
        src/util/dev/console/command/registry.cpp
        src/util/dev/console/command/auto_completion.cpp
        src/util/dev/console/command/commands/core_command.cpp
        src/manager/console_manager.cpp
)
set(HEADERS
        src/debug.h
        src/emulation/epd_test.h
        ${ARDUINO_DIR}/src/Font/fonts.h
        ${ARDUINO_DIR}/src/Canvas/GUI_Paint.h
        src/GUI/gui_raylib.cpp
        src/GUI/gui_raylib.h
        src/UI/input_field.h
        src/UI/button.cpp
        src/UI/button.h
        src/network/request.h
        src/e-Paper/paper_command.h

        src/util/dev/console/console.h
        src/util/dev/console/command/registry.h
        src/util/dev/console/command/auto_completion.h
        src/util/dev/console/command/commands/core_command.h
        src/manager/console_manager.h
)

set(CMAKE_CXX_STANDARD 20)

if(WIN32)
    add_executable(e_paper_emulator WIN32 ${SOURCES} ${HEADERS})
    target_link_options(e_paper_emulator PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
elseif(APPLE)
    add_executable(e_paper_emulator MACOSX_BUNDLE ${SOURCES} ${HEADERS})
elseif(UNIX AND NOT APPLE)
    add_executable(e_paper_emulator ${SOURCES} ${HEADERS})
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