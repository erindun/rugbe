#include <iostream>
#include <fstream>
#include <cstdlib>
#include <memory>

#include "cpu.hpp"

// Initialize CPU
Cpu::Cpu() : pc {0}, sp {0xfffe}, cycles {0}
{
    mmu.fill(0);
}

// Load ROM into memory
// Currently, it loads memory into $0000 where the boot ROM begins;
// however, a game should be loaded into memory beginning at $0100.
void Cpu::load_rom(const char* filepath) {
    std::cout << "Loading ROM: " << filepath << std::endl;
    std::ifstream rom(filepath, std::ios::binary);

    // Verify that ROM opens properly
    if (!rom) {
        std::cerr << "Failed to open ROM." << std::endl;
        std::exit(1);
    }

    // Find size of ROM
    rom.seekg(0, std::ios::end);
    int rom_size = rom.tellg();
    rom.clear();
    rom.seekg(0, std::ios::beg);

    // Allocate memory for ROM
    auto rom_buffer = std::make_unique<char[]>(rom_size);

    // Copy ROM into buffer
    rom.read(rom_buffer.get(), rom_size);

    // TODO: Make sure that ROM is valid

    // Load ROM into memory
    for (int i = 0; i < rom_size; ++i) {
        mmu.at(i) = rom_buffer[i];
    }

    // Clean up
    rom.close();
}

void Cpu::emulate() {
    // 4 cycles
    uint8_t opcode = read_mmu(pc);

    switch (opcode) {
        case 0x00: break; // NOP
        case 0x01: break;
        //case 0x40: LD_r_r([REGISTER B], [REGISTER B]); break;
    }
}