#ifndef PPU_HPP
#define PPU_HPP
#include <array>
#include <bitset>
#include <queue>
#include <SDL2/SDL.h>

class Ppu {
    private:
        // FIFO and fetcher which load pixels to the screen
        std::array<uint8_t, 8192> vram;

        // Modes for different timings
        enum Mode {HBLANK, VBLANK, SCANLINE_OAM, SCANLINE_VRAM} mode;

        // Totals number of cycles from CPU to determine mode
        int mode_clock;

        // Pixel
        // A pixel is 2-bits in size, so there are 4 possible color values
        enum Pixel {ZERO, ONE, TWO, THREE};

        // Tile set
        // A tile is made up of 8 * 8 pixels
        typedef std::array<std::array<Pixel, 8>, 8> Tile;
        std::array<Tile, 384> tileset;

        // Render one scanline
        void render();

        // Draw to screen
        void draw();

    public:
        // Registers
        bool bg_switch;
        bool bg_map;
        bool bg_tile;
        bool lcd_switch;
        uint8_t scy;
        uint8_t scx;
        uint8_t scanline;
        uint8_t palette;

        int cycles;
        
        // LCD screen
        std::array<uint32_t, 160 * 144> lcd;

        Ppu();
        uint8_t read_vram(uint16_t);
        void write_vram(uint16_t, uint8_t);
        void step_clock();
};

#endif // PPU_HPP