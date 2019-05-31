#ifndef PPU_HPP
#define PPU_HPP
#include <array>
#include <bitset>
class Mmu;

class Ppu {
    private:
        // C++11 3D-array syntax...yikes!!
        // This is an array of 384 tiles of 8*8, 2-bit pixels.
        // Because the way pixels are stored in memory is rather convoluted,
        // the values of the tiles are parsed when writing to memory and
        // stored here so they are easier to process.
        std::array<std::array<std::array<uint8_t, 8>, 8>, 384> tileset;


        // Modes for different timings
        enum Mode {HBLANK, VBLANK, SCANLINE_OAM, SCANLINE_VRAM} mode;

        // Totals number of cycles from CPU to determine mode
        int mode_clock;

        // Pointer to MMU
        Mmu* mmu;

    public:
        Ppu(Mmu*);
        void step_clock();
        void update_tileset();
};

#endif // PPU_HPP