#include <SDL2/SDL.h>
#include <iostream>

#include "ppu.hpp"
#include "../mmu/mmu.hpp"
#include "../cpu/cpu.hpp"

const Uint32 WHITE      = 0xffffffff;
const Uint32 LIGHT_GRAY = 0xC0C0C0ff;
const Uint32 DARK_GRAY  = 0x606060ff;
const Uint32 BLACK      = 0x000000ff;

// SDL
SDL_Window* window;
SDL_Texture* texture;
SDL_Renderer* renderer;

Ppu::Ppu() : mode {SCANLINE_OAM},
             mode_clock {0},
             bg_switch {false},
             bg_map {false},
             bg_tile {false},
             lcd_switch {false}
{
    // Initialize SDL
    const int SCREEN_WIDTH = 640;
    const int SCREEN_HEIGHT = 576;
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "SDL initialization failure. SDL_Error: "
                  << SDL_GetError() << std::endl;
    }

    //Create window
    window = SDL_CreateWindow("rugbe", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Failed to create window. SDL_Error: " 
                  << SDL_GetError() << std::endl;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (renderer == nullptr) {
        std::cerr << "Failed to create renderer. SDL_Error: " 
                  << SDL_GetError() << std::endl;
    }

    // Create texture that stores frame buffer
    texture = SDL_CreateTexture(renderer,
                                SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_STREAMING,
                                160, 144);
    if (texture == nullptr) {
        std::cerr << "Failed to create texture. SDL_Error: " 
                  << SDL_GetError() << std::endl;
    }
}

uint8_t Ppu::read_vram(uint16_t addr) {
    // VRAM is only 8192 bytes, so and addr with $1fff
    return vram.at(addr & 0x1fff);
}

void Ppu::write_vram(uint16_t addr, uint8_t data) {
    // VRAM is only 8192 bytes, so and addr with $1fff
    addr &= 0x1fff;

    vram.at(addr) = data;

    // If value is not in the tileset, return
    if ((addr) > 0x1800) return;

    // Get first byte of tile row
    addr &= 0xfffe;

    // Two bytes in the tile row
    uint8_t byte1 = vram.at(addr);
    uint8_t byte2 = vram.at(addr + 1);

    int tile_index =  addr / 16;
    int row_index  = (addr % 16) / 2;

    // Push to tileset
    for (int i = 0; i < 8; ++i) {
        // Find each bit of pixel
        bool bit0 = (byte1 >> i) & 1;
        bool bit1 = (byte2 >> i) & 1;

        // Determine pixel value
        Pixel pixel;
        if (!bit0 && !bit1) pixel = ZERO;
        else if (bit0 && !bit1)  pixel = ONE;
        else if (!bit0 && bit1)  pixel = TWO;
        else if (bit0 && bit1)   pixel = THREE;
        else { std::cout << "ERROR" << std::endl; exit(1); }        

        tileset.at(tile_index).at(row_index).at(i) = pixel;
    }
}

void Ppu::render() {
    // Which tilemap is being used
    uint16_t map_offset = bg_map ? 0x1c00 : 0x1800;

    // Which line of tiles is being used
    map_offset += ((scanline + scy) & 0xff) >> 3;

    // Which tile to begin with
    uint8_t line_offset = scx >> 3;

    // Which line of pixels is being used
    int y = (scanline + scy) & 7;

    // Where in tile line to begin
    int x = scx & 7;

    // Where to render on the screen
    int lcd_offset = scanline * 160;

    // Get tile from background map
    uint8_t tile = vram.at(map_offset + line_offset);

    // Tiles are signed if Tile Map 1 is in use
    uint8_t tile_0  = tile;
    int8_t  tile_1  = static_cast<int8_t>(tile);

    for (int i = 0; i < 160; ++i) {
        Pixel pixel;
        
        // Determine if pixel has signed or unsigned value
        if (bg_map) {
            pixel = tileset.at(tile_1).at(y).at(x);
        } else {
            pixel = tileset.at(tile_0).at(y).at(x);
        }

        // Write pixel to LCD framebuffer
        switch (pixel) {
            case ZERO:  lcd.at(lcd_offset) = BLACK;
            case ONE:   lcd.at(lcd_offset) = DARK_GRAY;
            case TWO:   lcd.at(lcd_offset) = LIGHT_GRAY;
            case THREE: lcd.at(lcd_offset) = WHITE;
        }
        ++lcd_offset;

        // Increment tile counter and roceed to next tile, if necessary
        ++x;
        if (x == 8) {
            line_offset = (line_offset + 1) & 0x1f;
            tile = vram.at(line_offset + map_offset);
        }
    }

    std::cout << "test" <<std::endl;
    
    // Update texture
    SDL_UpdateTexture(texture, NULL, lcd.data(), 160 * sizeof(uint32_t));

    // Clear screen and render
    SDL_RenderClear(renderer);  
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void Ppu::step_clock() {
    // Clock to determine PPU mode
    mode_clock += cycles;

    // Counts current line. Ranges from 0-153, where 144-153 are
    // for the vblank period.
    int line = 0;

    switch (mode) {

        // Access sprite memory
        case SCANLINE_OAM:
            if (mode_clock >= 80) {
                mode_clock = 0;
                mode = SCANLINE_VRAM;
            }
            break;
        
        // Access video memory
        case SCANLINE_VRAM:
            if (mode_clock >= 172) {
                mode_clock = 0;
                mode = HBLANK;

                // Render scanline
                render();
            }
            break;

        // After last hblank, push data to screen
        case HBLANK:
            if (mode_clock >= 204) {
                mode_clock = 0;
                ++line;

                if (line == 143) {
                    mode = VBLANK;
                    // TODO: Render frame
                } else {
                    mode = SCANLINE_OAM;
                }
            }
            break;

        case VBLANK:
            if (mode_clock >= 456) {
                mode_clock = 0;
                ++line;

                // Return to top of screen
                if (line > 153) {
                    mode = SCANLINE_OAM;
                    line = 0;
                }
            }
            break;
    }
}