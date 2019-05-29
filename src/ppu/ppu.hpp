#ifndef PPU_HPP
#define PPU_HPP
#include <array>
#include "../cpu/cpu.hpp"

class Ppu {
    private:
        // LCD is made up of 4-bit pixels
        struct Pixel {
            bool bit0;
            bool bit1;
            bool bit2;
            bool bit3;
        };
        std::array<Pixel, 160 * 144> lcd;

        // Modes for different timings
        enum Mode {HBLANK, VBLANK, SCANLINE_OAM, SCANLINE_VRAM} mode;

        // Totals number of cycles from CPU to determine mode
        int mode_clock;

        // Pointer to CPU
        Cpu* cpu;

    public:
        Ppu(Cpu*);
        void step_clock();
};

#endif // PPU_HPP