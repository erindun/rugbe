#ifndef PPU_HPP
#define PPU_HPP
#include <array>
#include <SDL2/SDL.h>

// Pixel type
// A pixel is 2-bits in size, so there are 4 possible color values.
// Pixels are mapped to the appropriate SDL Uint32 color values.
enum Pixel: Uint32 {
    BLACK      = 0x000000ff,
    DARK_GRAY  = 0X606060ff,
    LIGHT_GRAY = 0xC0C0C0ff,
    WHITE      = 0xffffffff
};

class Ppu {
    private:
        // Video RAM
        std::array<uint8_t, 8192> vram;

        // Modes for different timings
        enum Mode {HBLANK, VBLANK, SCANLINE_OAM, SCANLINE_VRAM} mode;

        // Totals number of cycles from CPU to determine mode
        int mode_clock;

        // A tile is made up of 8 * 8 pixels
        typedef std::array<std::array<Pixel, 8>, 8> Tile;

        // Tile set contains 384 pixels
        // Tiles 0-255 may be indexed with unsigned ints. (0 - 255)
        // Tiles 128-384 may be indexed with signed ints. (-128 - 127)
        // Tiles 128-255 may be indexed as signed or unsigned ints; the 
        // addresses are shared between the two indexing modes.
        std::array<Tile, 384> tileset;

        // Render one scanline
        void render();

    public:
        // Registers
        // When the CPU reads/writes to these registers, the MMU
        // redirects the value to/from these variables.
        bool bg_switch;
        bool bg_map;
        bool bg_tile;
        bool lcd_switch;
        uint8_t scy;
        uint8_t scx;
        uint8_t scanline;
        uint8_t palette;

        // Cycle counter (passed from CPU by dispatcher)
        int cycles;
        
        // Framebuffer
        // Passed into SDL update functions as Uint32
        std::array<Pixel, 160 * 144> framebuffer;

        Ppu();
        uint8_t read_vram(uint16_t);
        void write_vram(uint16_t, uint8_t);
        void step_clock();
};

#endif // PPU_HPP