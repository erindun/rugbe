#ifndef PPU_HPP
#define PPU_HPP
#include <array>
#include <bitset>
#include <queue>
class Mmu;

class Ppu {
    private:
        // Pointer to MMU
        Mmu* mmu;

        // FIFO and fetcher which load pixels to the screen
        std::queue<uint8_t> fifo;
        std::queue<uint8_t> fetcher_buffer;
        uint16_t tile_addr;

        // Modes for different timings
        enum Mode {HBLANK, VBLANK, SCANLINE_OAM, SCANLINE_VRAM} mode;

        // Totals number of cycles from CPU to determine mode
        int mode_clock;

        // Functions to get PPU register values
        bool get_bg_switch();
        bool get_bg_map();
        bool get_bg_tile();
        bool get_lcd_switch();

    public:
        int cycles;
        Ppu(Mmu*);
        void step_clock();
        void update_tileset();
        void fetcher();
        void update_fifo();
};

#endif // PPU_HPP