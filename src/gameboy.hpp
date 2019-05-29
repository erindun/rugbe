#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP
#include "mmu/mmu.hpp"
#include "cpu/cpu.hpp"
#include "ppu/ppu.hpp"

class GameBoy {
    private:
        Mmu mmu;
        Cpu cpu;
        Ppu ppu;
    public:
        GameBoy(const char*);
        void emulate();
        void test_boot_rom();
};

#endif // GAMEBOY_HPP