cmake_minimum_required(VERSION 4.3)
project(CHIP_8_emu)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(CHIP_8_emu
        src/main.cpp
        src/chip8.cpp
        include/chip8.h
        src/Platform.cpp
        include/Platform.h)

target_include_directories(CHIP_8_emu PRIVATE include)

target_link_libraries(CHIP_8_emu PRIVATE SDL3::SDL3)