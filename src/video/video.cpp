#include <iostream>
#include <SDL2/SDL.h>
#include "video.hpp"

SDL_Window* window = nullptr;
SDL_Texture* texture = nullptr;
SDL_Renderer* renderer = nullptr;

void setup_video() {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL initialization failure. SDL_Error: "
                  << SDL_GetError() << std::endl;
        exit(1);
    }



    //Create window
    window = SDL_CreateWindow("rugbe", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                                SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Failed to create window. SDL_Error: " 
                  << SDL_GetError() << std::endl;
        exit(1);
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (renderer == nullptr) {
        std::cerr << "Failed to create renderer. SDL_Error: " 
                  << SDL_GetError() << std::endl;
        exit(1);
    }

    // Create texture that stores frame buffer
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                160, 144);
    if (texture == nullptr) {
        std::cerr << "Failed to create texture. SDL_Error: " 
                  << SDL_GetError() << std::endl;
        exit(1);
    }
}

void draw(std::array<Pixel, 160 * 144> framebuffer) {
    std::array<Uint32, 160 * 144> fb;

    // TODO cleanup

    for (int i = 0; i < 160 * 144; ++i) {
        fb.at(i) = static_cast<Uint32>(fb.at(i));
        std::cout << "Pixel value at pixel pos " << i << ": " << fb.at(i) << std::endl;
    }



    // Update texture
    SDL_UpdateTexture(texture, NULL, fb.data(), 160 * sizeof(Uint32));

    // Clear screen and render
    SDL_RenderClear(renderer);  
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}