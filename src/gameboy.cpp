#include "gameboy.hpp"

GameBoy::GameBoy(const char* filepath) : mmu {&cpu}, cpu {Cpu(&mmu, &ppu)}, ppu {&mmu} {
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