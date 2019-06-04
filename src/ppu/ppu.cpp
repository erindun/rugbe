#include "ppu.hpp"
#include "../mmu/mmu.hpp"
#include "../cpu/cpu.hpp"

// Create pointer to CPU
Ppu::Ppu(Mmu* mmu) : mmu {mmu}, tile_addr {0x9800},
                     mode {SCANLINE_OAM}, mode_clock {0}
{}

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
                fetcher();
                update_fifo(); 
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

void Ppu::fetcher() {
    if (fetcher_buffer.empty()) {
        if (get_bg_map()) {
            tile_addr = 0x9800;
        } else {
            tile_addr = 0x9c00;
        }


        // Read value of tile row (2 bytes) from memory
        uint8_t tile = mmu->read(tile_addr);
        uint8_t tile_data0 = mmu->read(tile);
        uint8_t tile_data1 = mmu->read(tile + 1);

        // Push value of each pixel to buffer
        for (int i = 0; i < 8; ++i) {
            // Find each bit of pixel
            bool bit0 = (tile_data0 >> i) & 1;
            bool bit1 = (tile_data1 >> i) & 1;

            // Determine pixel value
            uint8_t pixel = 0;
            bit0 ? pixel += 1 : pixel += 0;
            bit1 ? pixel += 2 : pixel += 0;
            fetcher_buffer.push(pixel);

        }
    }
}

void Ppu::update_fifo() {
    // Push byte to FIFO, if possible
    if (fifo.size() <= 8) {
        for (int i = 0; i < 8; ++i) {
            fifo.push(fetcher_buffer.front());
            fetcher_buffer.pop();
        }
    }

    // Push pixel to LCD
    for (int i = 0; i < 2; ++i) {
    }
}