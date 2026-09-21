//
// Created by Carlitos on 9/16/2026.
//
using namespace std;
#ifndef CHIP_8_EMU_CHIP8_H
#define CHIP_8_EMU_CHIP8_H
#include <cstdint>
#include <chrono>
#include <random>

class Chip8 {
public:
    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t indexRegister{};
    uint16_t programCounter{};
    uint16_t stack[16]{};
    uint8_t stackPointer{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
    uint16_t inputKeys[16]{};
    uint32_t displayMemory[64*32]{};
    uint16_t opcode{};
    void loadROM(const char*filename);
    Chip8();
    default_random_engine randGen;
    uniform_int_distribution<uint8_t> randByte;
    void OP_00E0();
    void OP_00EE();
    void OP_1NNN();
    void OP_2NNN();
    void OP_3XKK();
    void OP_4XKK();
    void OP_5XY0();
    void OP_6XKK();
    void OP_7XKK();
    void OP_8XY0();
    void OP_8XY1();
    void OP_8XY2();
    void OP_8XY3();
    void OP_8XY4();
    void OP_8XY5();
    void OP_8XY6();
    void OP_8XY7();
    void OP_8XYE();
    void OP_9XY0();
    void OP_ANNN();
    void OP_BNNN();
};

#endif //CHIP_8_EMU_CHIP8_H
