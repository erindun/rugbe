#include "gameboy.hpp"

GameBoy::GameBoy(const char* filepath) : cpu {Cpu(&mmu)}, ppu {&cpu} {
    mmu.load_rom(filepath);
}
void GameBoy::emulate() { 
    cpu.emulate();
    ppu.step_clock();
}

void GameBoy::test_boot_rom() { mmu.test_boot_rom(); };