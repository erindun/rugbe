#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>
#include "mmu.hpp"
#include "../cpu/cpu.hpp"


// Read a byte from memory
uint8_t Mmu::read(uint16_t addr) {
    Cpu::cycles += 4;
    return mmu.at(addr);
}

void Mmu::write(uint16_t addr, uint8_t data) {
    Cpu::cycles += 4;
    mmu.at(addr) = data;
}

// Load ROM into memory
// Currently, it loads memory into $0000 where the boot ROM begins;
// however, a game should be loaded into memory beginning at $0100.
void Mmu::load_rom(const char* filepath) {
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

// Loads appopriate values into memory so that the boot ROM may be tested.
void Mmu::test_boot_rom() {
    std::array<uint8_t, 48> nintendo_logo_hexdump = {
        0xce, 0xed, 0x66, 0x66, 0xcc, 0x0d, 0x00, 0x0b, 0x03, 0x73, 0x00,
        0x83, 0x00, 0x0c, 0x00, 0x0d, 0x00, 0x08, 0x11, 0x1f, 0x88, 0x89,
        0x00, 0x0e, 0xdc, 0xcc, 0x6e, 0xe6, 0xdd, 0xdd, 0xd9, 0x99, 0xbb,
        0xbb, 0x67, 0x63, 0x6e, 0x0e, 0xec, 0xcc, 0xdd, 0xdc, 0x99, 0x9f,
        0xbb, 0xb9, 0x33, 0x3e
    };

    for (int i = 0; i < 47; ++i) {
        mmu.at(i + 0x104) = nintendo_logo_hexdump.at(i); 
    }
}