#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>
#include "mmu.hpp"

#include "../cpu/cpu.hpp"
#include "../ppu/ppu.hpp"

Mmu::Mmu(Cpu* cpu, Ppu* ppu) : cpu {cpu}, ppu {ppu} { mmu.fill(0); }

// Read a byte from memory
uint8_t Mmu::read(uint16_t addr) {
    cpu->cycles += 4;

    switch  (addr & 0xf000) {
        case 0x8000: case 0x9000:
            return ppu->read_vram(addr);

        case 0xff40:
            return (ppu->bg_switch  ? 0x01 : 0x00) |
                   (ppu->bg_map     ? 0x08 : 0x00) |
                   (ppu->bg_tile    ? 0x10 : 0x00) |
                   (ppu->lcd_switch ? 0x80 : 0x00);

        case 0xff42:
            return ppu->scy;

        case 0xff43:
            return ppu->scx;

        case 0xff44:
            return ppu->scanline;

        default:
            return mmu.at(addr);
    }
}

// Write a byte into memory. If it is written to an address that is
// emulated externally (e.g. VRAM) write to that object's data instead.
void Mmu::write(uint16_t addr, uint8_t data) {
    // Add cycles to CPU
    cpu->cycles += 4;

    switch (addr & 0xf000) {
        // If value is written to VRAM, update the PPU's internal data
        case 0x8000: case 0x9000:
            ppu->write_vram(addr, data);
            break;

        // LCD control register
        case 0xff40:
            ppu->bg_switch  = (addr & 0x1)  ? 1 : 0;
            ppu->bg_map     = (addr & 0x8)  ? 1 : 0;
            ppu->bg_tile    = (addr & 0x10) ? 1 : 0;
            ppu->lcd_switch = (addr & 0x80) ? 1 : 0;
            break;
        
        // Scroll Y
        case 0xff42:
            ppu->scy = data;
            break;

        // Scroll X
        case 0xff43:
            ppu->scx = data;
            break;

        // Current scanline
        case 0xff47:
            ppu->palette = data;
            break;

        default:
            mmu.at(addr) = data;
            break;
    }
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