#include <iostream>

#include "ppu.hpp"
#include "../mmu/mmu.hpp"
#include "../cpu/cpu.hpp"
#include "../video/video.hpp"

Ppu::Ppu() : mode {SCANLINE_OAM},
             mode_clock {0},
             bg_switch {false},
             bg_map {false},
             bg_tile {false},
             lcd_switch {false},
             scy {0},
             scx {0},
             scanline {0},
             palette {0}
{
    // Initialize tileset to all white pixels
    for (int i = 0; i < 384; ++i) {
        for (int y = 0; y < 8; ++y) {
            for (int x = 0; x < 8; ++x) {
                tileset.at(i).at(y).at(x) = WHITE;
            }
        }
    }

    // Initialize framebuffer to all white pixels
    framebuffer.fill(WHITE);
}

uint8_t Ppu::read_vram(uint16_t addr) {
    // VRAM is only 8192 bytes, so and addr with $1fff
    return vram.at(addr & 0x1fff);
}

void Ppu::write_vram(uint16_t addr, uint8_t data) {
    // VRAM is only 8192 bytes, so and addr with $1fff
    addr &= 0x1fff;

    vram.at(addr) = data;

    // If value is not in the tileset, exit function
    if (addr > 0x17ff) return;

    // Get the address of the first byte of tile row
    addr &= 0xfffe;

    // Get the two bytes in the tile row
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
        if (!bit0 && !bit1) pixel = BLACK;
        if (bit0 && !bit1)  pixel = DARK_GRAY;
        if (!bit0 && bit1)  pixel = LIGHT_GRAY;
        if (bit0 && bit1)   pixel = WHITE;

        tileset.at(tile_index).at(row_index).at(i) = pixel;
        std::cout << "pixel: " << tileset.at(tile_index).at(row_index).at(i) << std::endl;
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
    int screen_offset = scanline * 160;

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
        framebuffer.at(screen_offset) = pixel; 

        ++screen_offset;

        // Increment tile counter and roceed to next tile, if necessary
        ++x;
        if (x == 8) {
            line_offset = (line_offset + 1) & 0x1f;
            tile = vram.at(line_offset + map_offset);
            x = 0;
        }
    }
}

void Ppu::step_clock() {
    // Clock to determine PPU mode
    mode_clock += cycles;

    // Counts current line. Ranges from 0-153, where 144-153 are
    // for the vblank period.

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

                // Render a scanline
                render();
            }
            break;

        // After last hblank, push data to screen
        case HBLANK:
            if (mode_clock >= 204) {
                mode_clock = 0;
                ++scanline;

                if (scanline == 143) {
                    mode = VBLANK;
                    // Draw to screen
                    draw(framebuffer);
                } else {
                    mode = SCANLINE_OAM;
                }
            }
            break;

        case VBLANK:
            if (mode_clock >= 456) {
                mode_clock = 0;
                ++scanline;

                // Return to top of screen
                if (scanline > 153) {
                    mode = SCANLINE_OAM;
                    scanline = 0;
                }
            }
            break;
    }

    //std::cout << (int)scanline << std::endl;
}