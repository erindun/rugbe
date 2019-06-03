#ifndef PPU_HPP
#define PPU_HPP
#include <array>
#include <bitset>
#include <queue>
class Mmu;

class Ppu {
    private:
        std::queue<uint8_t> fifo;
        std::queue<uint8_t> fetcher_buffer;


        // Modes for different timings
        enum Mode {HBLANK, VBLANK, SCANLINE_OAM, SCANLINE_VRAM} mode;

        // Totals number of cycles from CPU to determine mode
        int mode_clock;

        // Pointer to MMU
        Mmu* mmu;

    public:
        int cycles;
        Ppu(Mmu*);
        void step_clock();
        void update_tileset();
        void fetcher(uint16_t addr);
        void update_fifo();
};

#endif // PPU_HPP