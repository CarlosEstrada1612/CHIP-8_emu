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
};

#endif //CHIP_8_EMU_CHIP8_H
