//
// Created by Carlitos on 9/16/2026.
//
using namespace std;
#ifndef CHIP_8_EMU_CHIP8_H
#define CHIP_8_EMU_CHIP8_H
#include <cstdint>
#include <chrono>
#include <random>
const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

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
    uint8_t inputKeys[16]{};
    uint32_t displayMemory[64*32]{};
    uint16_t opcode{};
    default_random_engine randGen;
    uniform_int_distribution<int> randByte;
    void loadROM(const char*filename);
    Chip8();
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
    void OP_CXKK();
    void OP_DXYN();
    void OP_EX9E();
    void OP_EXA1();
    void OP_FX07();
    void OP_FX0A();
    void OP_FX15();
    void OP_FX18();
    void OP_FX1E();
    void OP_FX29();
    void OP_FX33();
    void OP_FX55();
    void OP_FX65();
    void Table0();
    void Table8();
    void TableE();
    void TableF();
    void OP_NULL();
    typedef void (Chip8::*Chip8_Function)();
    Chip8_Function table[0xF + 1];
    Chip8_Function table0[0xE + 1];
    Chip8_Function table8[0xE + 1];
    Chip8_Function tableE[0xE + 1];
    Chip8_Function tableF[0x65 + 1];
    void Cycle();
};

#endif //CHIP_8_EMU_CHIP8_H
