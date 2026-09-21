//
// Created by Carlitos on 9/16/2026.
//
#include "../include/chip8.h"
#include <fstream>
#include <cstdint>
using namespace std;
const unsigned int START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;    //0x50 because

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
    if (registers[10]==opcode) programCounter=programCounter+2;
}
void Chip8::OP_4XKK() {
    uint8_t Vx=(opcode & 0x0F00u)>>8u;
    uint8_t byte = opcode & 0x00FFu;
    if (registers[10]!=opcode) programCounter=programCounter+2;
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
    registers[Vx] ^= registers[Vy];
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
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy = (opcode & 0x00F0)>>4u;
    registers[Vx] = registers[Vy]>>1u;
    uint8_t bit = registers[Vy] & 0b00000001;
    registers[0x0F] =bit;
}
void Chip8::OP_8XY7() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy = (opcode & 0x00F0)>>4u;
    if (registers[Vx]>registers[Vy]) {
        registers[0xF] = 0x00;
    } else {
        registers[0xF] = 0x01;
    }
    registers[Vx] = registers[Vy]-registers[Vx];
}
void Chip8::OP_8XYE() {
    uint8_t Vx = (opcode & 0x0F00u)>>8u;
    uint8_t Vy = (opcode & 0x00F0)>>4u;
    registers[Vx] = registers[Vy]<<1u;
    uint8_t bit = (registers[Vy] & 0b10000000)>>7u;
    registers[0x0F] = bit;
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