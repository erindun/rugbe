#include "gameboy.hpp"

GameBoy::GameBoy(const char* filepath) : mmu {&cpu, &ppu}, cpu {Cpu(&mmu, &ppu)} {
    mmu.load_rom(filepath);
}
void GameBoy::emulate() { 
    // Reset cycle counter
    cpu.cycles = 0;

    // Emulate one frame
    while (cpu.cycles < 70224) {
        cpu.execute_instruction();
        ppu.step_clock();
    }
}

void GameBoy::test_boot_rom() { mmu.test_boot_rom(); };