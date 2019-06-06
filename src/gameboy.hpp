#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP
#include "mmu/mmu.hpp"
#include "cpu/cpu.hpp"
#include "ppu/ppu.hpp"

class GameBoy {
    public :
        Mmu mmu;
        Cpu cpu;
        Ppu ppu;

        GameBoy(const char*);
        void emulate();
        void test_boot_rom();
};

#endif // GAMEBOY_HPP