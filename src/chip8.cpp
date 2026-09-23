//
// Created by Carlitos on 9/16/2026.
//
#include "../include/chip8.h"
#include <fstream>
#include <cstdint>
using namespace std;
const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

void Chip8::loadROM(const char*filename) {
    // Open the file as a stream of binary and move the file pointer to the end
    ifstream file(filename,ios::binary | ios::ate);
    if (file.is_open()) {
        // Get size of file and allocate a buffer to hold the contents
        streampos size = file.tellg();
        char* buffer = new char[size];
        // Go back to the beginning of the file and fill the buffer
        file.seekg(0,ios::beg);
        file.read(buffer,size);
        file.close();
        // Load the ROM contents into the Chip8's memory, starting at 0x200
        for (long i=0;i<size;i++) {
            memory[START_ADDRESS + i] = buffer[i];
        }
        delete[] buffer;
    }
}
uint8_t fontset[FONTSET_SIZE] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};
Chip8::Chip8()
    : randGen(chrono::system_clock::now().time_since_epoch().count()){
    //constructor: initializing PC
    programCounter = START_ADDRESS;
    //loads fonts into memory
    for (unsigned int i=0;i<FONTSET_SIZE;i++) {
        memory[FONTSET_START_ADDRESS+i]=fontset[i];
    }
    // Initialize RNG
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);
}
void Chip8::OP_00E0(){
    for (int i=0;i<64*32;i++) displayMemory[i]=0;
}
void Chip8::OP_00EE() {
    programCounter = stack[stackPointer-1];
    stackPointer--;
}
void Chip8::OP_1NNN() {
    uint16_t address = opcode & 0x0FFFu;
    programCounter = address;
}
void Chip8::OP_2NNN() {
    stack[stackPointer] = programCounter;
    stackPointer++;
    uint16_t address = opcode & 0x0FFFu;
    programCounter = address;
}
void Chip8::OP_3XKK() {
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t byte = opcode & 0x00FFu;
    if (registers[Vx]==byte) programCounter=programCounter+2;
}
void Chip8::OP_4XKK() {
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t byte = opcode & 0x00FFu;
    if (registers[Vx]!=byte) programCounter=programCounter+2;
}
void Chip8::OP_5XY0() {
    uint8_t Vx= (opcode & 0x0F00)>>8u;
    uint8_t Vy= (opcode & 0x00F0)>>4u;
    if (registers[Vx]==registers[Vy]) programCounter=programCounter+2;
}
void Chip8::OP_6XKK() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] = byte;
}
void Chip8::OP_7XKK() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] = byte+registers[Vx];
}
void Chip8::OP_8XY0() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy= (opcode & 0x00F0)>>4u;
    registers[Vx] = registers[Vy];
}
void Chip8::OP_8XY1() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy= (opcode & 0x00F0)>>4u;
    registers[Vx] |= registers[Vy];
}
void Chip8::OP_8XY2() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy= (opcode & 0x00F0)>>4u;
    registers[Vx] &= registers[Vy];
}
void Chip8::OP_8XY3() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy= (opcode & 0x00F0)>>4u;
    registers[Vx] = registers[Vx] ^ registers[Vy];
}
void Chip8::OP_8XY4() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy = (opcode & 0x00F0)>>4u;
    uint16_t sum = registers[Vx] + registers[Vy];
    if (sum>0xFF) registers[0xF] = 0x01;
    else registers[0xF] = 0;
    registers[Vx] = sum;
}
void Chip8::OP_8XY5() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy = (opcode & 0x00F0)>>4u;
    if (registers[Vx]>registers[Vy]) {
        registers[0xF] = 0x01;
    } else {
        registers[0xF] = 0x00;
    }
    registers[Vx] -= registers[Vy];
}
void Chip8::OP_8XY6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[0xF] = registers[Vx] & 0x01;
    registers[Vx] >>= 1u;
}
void Chip8::OP_8XY7() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy = (opcode & 0x00F0)>>4u;
    if (registers[Vy]>registers[Vx]) {
        registers[0xF] = 0x01;
    } else {
        registers[0xF] = 0x00;
    }
    registers[Vx] = registers[Vy]-registers[Vx];
}
void Chip8::OP_8XYE() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;
    registers[Vx] <<= 1u;
}
void Chip8::OP_9XY0() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy = (opcode & 0x00F0)>>4u;
    if (registers[Vx]!=registers[Vy]) {
        programCounter+=2;
    }
}
void Chip8::OP_ANNN() {
    uint16_t nnn = opcode & 0x0FFFu;
    indexRegister = nnn;
}
void Chip8::OP_BNNN() {
    uint16_t nnn = opcode & 0x0FFFu;
    programCounter = registers[0]+nnn;
}
void Chip8::OP_CXKK() {
    uint8_t Vx = opcode & 0x0F00u;
    uint8_t Byte = opcode & 0x00FFu;
    registers[Vx] = randByte(randGen)+Byte;
}
void Chip8::OP_DXYN() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy = (opcode & 0x00F0)>>4u;
    uint8_t height = opcode & 0x000Fu;
    uint8_t xPos = registers[Vx]%VIDEO_WIDTH;
    uint8_t yPos = registers[Vy]%VIDEO_HEIGHT;
    registers[0xF] = 0;
    for (unsigned int row=0; row<height; row++) {
        uint8_t spriteByte = memory[indexRegister+row];
        for (unsigned int col=0; col<8; col++) {
            uint8_t spritePixel = spriteByte & (0x80 >> col);
            uint32_t *screenPixel = &displayMemory[(yPos + row) * VIDEO_WIDTH + (xPos + col)];
            if (screenPixel) {
                if (*screenPixel==0xFFFFFFFF) registers[0xF] = 1;
                *screenPixel ^= 0xFFFFFFFF;
            }
        }
    }
}
void Chip8::OP_EX9E() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t key = registers[Vx];
    if (inputKeys[key]) programCounter+=2;
}
void Chip8::OP_EXA1() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t key = registers[Vx];
    if (!inputKeys[key]) programCounter+=2;
}
void Chip8::OP_FX07() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    registers[Vx] = delayTimer;
}
void Chip8::OP_FX0A() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    if (inputKeys[0]) registers[Vx] = 0;
    else if (inputKeys[1]) registers[Vx] = 1;
    else if (inputKeys[2]) registers[Vx] = 2;
    else if (inputKeys[3]) registers[Vx] = 3;
    else if (inputKeys[4]) registers[Vx] = 4;
    else if (inputKeys[5]) registers[Vx] = 5;
    else if (inputKeys[6]) registers[Vx] = 6;
    else if (inputKeys[7]) registers[Vx] = 7;
    else if (inputKeys[8]) registers[Vx] = 8;
    else if (inputKeys[9]) registers[Vx] = 9;
    else if (inputKeys[10]) registers[Vx] = 10;
    else if (inputKeys[11]) registers[Vx] = 11;
    else if (inputKeys[12]) registers[Vx] = 12;
    else if (inputKeys[13]) registers[Vx] = 13;
    else if (inputKeys[14]) registers[Vx] = 14;
    else if (inputKeys[15]) registers[Vx] = 15;
    else programCounter-=2;
}
void Chip8::OP_FX15() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    delayTimer = registers[Vx];
}
void Chip8::OP_FX18() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    soundTimer = registers[Vx];
}
void Chip8::OP_FX1E() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    indexRegister += registers[Vx];
}
void Chip8::OP_FX29() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t digit = registers[Vx];
    indexRegister = FONTSET_START_ADDRESS+digit*5;
}
void Chip8::OP_FX33() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    int value = registers[Vx];
    memory[indexRegister+2] = value%10;
    value/=10;
    memory[indexRegister+1] = value%10;
    value/=10;
    memory[indexRegister+0] = value%10;
}
void Chip8::OP_FX55(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i <= Vx; ++i) memory[indexRegister + i] = registers[i];
}
void Chip8::OP_FX65(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for (uint8_t i = 0; i <= Vx; ++i) registers[i] = memory[indexRegister + i];
}

