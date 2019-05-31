#include "ppu.hpp"
#include "../mmu/mmu.hpp"
#include "../cpu/cpu.hpp"

// Create pointer to CPU
Ppu::Ppu(Mmu* mmu) : mmu {mmu} {}

void Ppu::step_clock() {
    // Clock to determine PPU mode
    mode_clock += Cpu::cycles;

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
                // TODO: Write to framebuffer
            }
            break;

        // After last hblank, push data to screen
        case HBLANK:
            if (mode_clock >= 204) {
                mode_clock = 0;
                ++line;

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

void Ppu::update_tileset() {
    
}