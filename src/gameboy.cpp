#include "gameboy.hpp"

GameBoy::GameBoy(const char* filepath) : mmu {&ppu}, cpu {Cpu(&mmu)}, ppu {&mmu} {
    mmu.load_rom(filepath);
}
void GameBoy::emulate() { 
    cpu.emulate();
    ppu.step_clock();
}

void GameBoy::test_boot_rom() { mmu.test_boot_rom(); };