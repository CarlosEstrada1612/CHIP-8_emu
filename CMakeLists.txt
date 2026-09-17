cmake_minimum_required(VERSION 4.3)
project(CHIP_8_emu)

set(CMAKE_CXX_STANDARD 20)

add_executable(CHIP_8_emu src/main.cpp
        src/chip8.cpp
        include/chip8.h)
