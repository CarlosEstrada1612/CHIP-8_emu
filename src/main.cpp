#include <chrono>
#include <iostream>
#include "../include/chip8.h"
#include "../include/Platform.h"
#include <SDL3/SDL.h>
using namespace std;

int main(int argc, char** argv) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
        exit(EXIT_FAILURE);
    }
    int videoScale = stoi(argv[1]);
    int cycleDelay = stoi(argv[2]);
    char const* romFilename = argv[3];
    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale,
            VIDEO_WIDTH, VIDEO_HEIGHT);
    Chip8 chip8;
    chip8.loadROM(romFilename);
    int videoPitch = sizeof(chip8.displayMemory[0]) * VIDEO_WIDTH;
    auto lastCycleTime = chrono::high_resolution_clock::now();
    bool quit = false;
    while (!quit) {
        quit = platform.ProcessInput(chip8.inputKeys);
        auto currentTime = chrono::high_resolution_clock::now();
        float dt = chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();
        if (dt > cycleDelay) {
            lastCycleTime = currentTime;
            chip8.Cycle();
            cout << "PC: " << std::hex << chip8.programCounter << '\n';
            platform.Update(chip8.displayMemory, videoPitch);
        }
    }
    return 0;
}