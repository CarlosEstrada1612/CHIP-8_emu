//
// Created by Carlitos on 11/8/2026.
//

#ifndef CHIP_8_EMU_PLATFORM_H
#define CHIP_8_EMU_PLATFORM_H
#include <cstdint>
#include <SDL3/SDL.h>

class Platform {
private:
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
public:
    Platform(char const* title, int windowWidth, int windowHeight, int textureWidth, int textureHeight);
    ~Platform();
    void Update(void const* buffer, int pitch);
    bool ProcessInput(uint8_t* keys);
};


#endif //CHIP_8_EMU_PLATFORM_H
