#ifndef PPU_HPP
#define PPU_HPP
#include <array>
#include <bitset>
#include <queue>

class Ppu {
    private:
        // FIFO and fetcher which load pixels to the screen
        std::array<uint8_t, 8192> vram;
        std::queue<uint8_t> fifo;
        std::queue<uint8_t> fetcher_buffer;
        uint16_t tile_addr;

        // Modes for different timings
        enum Mode {HBLANK, VBLANK, SCANLINE_OAM, SCANLINE_VRAM} mode;

        // Tile set
        enum Pixel {ZERO, ONE, TWO, THREE};
        typedef std::array<std::array<Pixel, 8>, 8> Tile;
        std::array<Tile, 384> tileset;

        // Totals number of cycles from CPU to determine mode
        int mode_clock;

        // Registers
        bool bg_switch;
        bool bg_map;
        bool bg_tile;
        bool lcd_switch;

    public:
        int cycles;
        Ppu();
        uint8_t read_vram(uint16_t);
        void write_vram(uint16_t, uint8_t);
        void step_clock();
        void update_tileset();
        void fetcher();
        void update_fifo();
};

#endif // PPU_HPP