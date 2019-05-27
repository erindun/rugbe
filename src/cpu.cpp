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
        case 0x40: LD_r_x(reg.b(), reg.b()); break;
        case 0x41: LD_r_x(reg.b(), reg.c()); break;
        case 0x42: LD_r_x(reg.b(), reg.d()); break;
        case 0x43: LD_r_x(reg.b(), reg.e()); break;
        case 0x44: LD_r_x(reg.b(), reg.h()); break;
        case 0x45: LD_r_x(reg.b(), reg.l()); break;
        case 0x46: LD_r_x(reg.b(), read_mmu(reg.hl())); break;
        case 0x47: LD_r_x(reg.b(), reg.a()); break;
        case 0x48: LD_r_x(reg.c(), reg.b()); break;
        case 0x49: LD_r_x(reg.c(), reg.c()); break;
        case 0x4a: LD_r_x(reg.c(), reg.d()); break;
        case 0x4b: LD_r_x(reg.c(), reg.e()); break;
        case 0x4c: LD_r_x(reg.c(), reg.h()); break;
        case 0x4d: LD_r_x(reg.c(), reg.l()); break;
        case 0x4e: LD_r_x(reg.c(), read_mmu(reg.hl())); break;
        case 0x4f: LD_r_x(reg.c(), reg.a()); break;
        case 0x50: LD_r_x(reg.d(), reg.b()); break;
        case 0x51: LD_r_x(reg.d(), reg.c()); break;
        case 0x52: LD_r_x(reg.d(), reg.d()); break;
        case 0x53: LD_r_x(reg.d(), reg.e()); break;
        case 0x54: LD_r_x(reg.d(), reg.h()); break;
        case 0x55: LD_r_x(reg.d(), reg.l()); break;
        case 0x56: LD_r_x(reg.d(), read_mmu(reg.hl())); break;
        case 0x57: LD_r_x(reg.d(), reg.a()); break;
        case 0x58: LD_r_x(reg.e(), reg.b()); break;
        case 0x59: LD_r_x(reg.e(), reg.c()); break;
        case 0x5a: LD_r_x(reg.e(), reg.d()); break;
        case 0x5b: LD_r_x(reg.e(), reg.e()); break;
        case 0x5c: LD_r_x(reg.e(), reg.h()); break;
        case 0x5d: LD_r_x(reg.e(), reg.l()); break;
        case 0x5e: LD_r_x(reg.e(), read_mmu(reg.hl())); break;
        case 0x5f: LD_r_x(reg.e(), reg.a()); break;
        case 0x60: LD_r_x(reg.h(), reg.b()); break;
        case 0x61: LD_r_x(reg.h(), reg.c()); break;
        case 0x62: LD_r_x(reg.h(), reg.d()); break;
        case 0x63: LD_r_x(reg.h(), reg.e()); break;
        case 0x64: LD_r_x(reg.h(), reg.h()); break;
        case 0x65: LD_r_x(reg.h(), reg.l()); break;
        case 0x66: LD_r_x(reg.h(), read_mmu(reg.hl())); break;
        case 0x67: LD_r_x(reg.h(), reg.a()); break;
        case 0x68: LD_r_x(reg.l(), reg.b()); break;
        case 0x69: LD_r_x(reg.l(), reg.c()); break;
        case 0x6a: LD_r_x(reg.l(), reg.d()); break;
        case 0x6b: LD_r_x(reg.l(), reg.e()); break;
        case 0x6c: LD_r_x(reg.l(), reg.h()); break;
        case 0x6d: LD_r_x(reg.l(), reg.l()); break;
        case 0x6e: LD_r_x(reg.l(), read_mmu(reg.hl())); break;
        case 0x6f: LD_r_x(reg.l(), reg.a()); break;
        case 0x70: LD_rrp_r(reg.hl(), reg.b()); break;
        case 0x71: LD_rrp_r(reg.hl(), reg.c()); break;
        case 0x72: LD_rrp_r(reg.hl(), reg.d()); break;
        case 0x73: LD_rrp_r(reg.hl(), reg.e()); break;
        case 0x74: LD_rrp_r(reg.hl(), reg.h()); break;
        case 0x75: LD_rrp_r(reg.hl(), reg.l()); break;
        case 0x76: break; // TODO: HALT
        case 0x77: LD_rrp_r(reg.hl(), reg.a()); break;
        case 0x78: LD_r_x(reg.a(), reg.b()); break;
        case 0x79: LD_r_x(reg.a(), reg.c()); break;
        case 0x7a: LD_r_x(reg.a(), reg.d()); break;
        case 0x7b: LD_r_x(reg.a(), reg.e()); break;
        case 0x7c: LD_r_x(reg.a(), reg.h()); break;
        case 0x7d: LD_r_x(reg.a(), reg.l()); break;
        case 0x7e: LD_r_x(reg.a(), read_mmu(reg.hl())); break;
        case 0x7f: LD_r_x(reg.a(), reg.a()); break;
        case 0x80: ADD_a_x(reg.b()); break;
        case 0x81: ADD_a_x(reg.c()); break;
        case 0x82: ADD_a_x(reg.d()); break;
        case 0x83: ADD_a_x(reg.e()); break;
        case 0x84: ADD_a_x(reg.h()); break;
        case 0x85: ADD_a_x(reg.l()); break;
        case 0x86: ADD_a_x(read_mmu(reg.hl())); break;
        case 0x87: ADD_a_x(reg.a()); break;
        case 0x88: ADC_a_x(reg.b()); break;
        case 0x89: ADC_a_x(reg.c()); break;
        case 0x8a: ADC_a_x(reg.d()); break;
        case 0x8b: ADC_a_x(reg.e()); break;
        case 0x8c: ADC_a_x(reg.h()); break;
        case 0x8d: ADC_a_x(reg.l()); break;
        case 0x8e: ADC_a_x(read_mmu(reg.hl())); break;
        case 0x8f: ADC_a_x(reg.a()); break;
        case 0x90: SUB_a_x(reg.b()); break;
        case 0x91: SUB_a_x(reg.c()); break;
        case 0x92: SUB_a_x(reg.d()); break;
        case 0x93: SUB_a_x(reg.e()); break;
        case 0x94: SUB_a_x(reg.h()); break;
        case 0x95: SUB_a_x(reg.l()); break;
        case 0x96: SUB_a_x(read_mmu(reg.hl())); break;
        case 0x97: SUB_a_x(reg.a()); break;
        case 0x98: SBC_a_x(reg.b()); break;
        case 0x99: SBC_a_x(reg.c()); break;
        case 0x9a: SBC_a_x(reg.d()); break;
        case 0x9b: SBC_a_x(reg.e()); break;
        case 0x9c: SBC_a_x(reg.h()); break;
        case 0x9d: SBC_a_x(reg.l()); break;
        case 0x9e: SBC_a_x(read_mmu(reg.hl())); break;
        case 0x9f: SBC_a_x(reg.a()); break;
        case 0xa0: AND_a_x(reg.b()); break;
        case 0xa1: AND_a_x(reg.c()); break;
        case 0xa2: AND_a_x(reg.d()); break;
        case 0xa3: AND_a_x(reg.e()); break;
        case 0xa4: AND_a_x(reg.h()); break;
        case 0xa5: AND_a_x(reg.l()); break;
        case 0xa6: AND_a_x(read_mmu(reg.hl())); break;
        case 0xa7: AND_a_x(reg.a()); break;
        case 0xa8: XOR_a_x(reg.b()); break;
        case 0xa9: XOR_a_x(reg.c()); break;
        case 0xaa: XOR_a_x(reg.d()); break;
        case 0xab: XOR_a_x(reg.e()); break;
        case 0xac: XOR_a_x(reg.h()); break;
        case 0xad: XOR_a_x(reg.l()); break;
        case 0xae: XOR_a_x(read_mmu(reg.hl())); break;
        case 0xaf: XOR_a_x(reg.a()); break;
        case 0xb0: OR_a_x(reg.b()); break;
        case 0xb1: OR_a_x(reg.c()); break;
        case 0xb2: OR_a_x(reg.d()); break;
        case 0xb3: OR_a_x(reg.e()); break;
        case 0xb4: OR_a_x(reg.h()); break;
        case 0xb5: OR_a_x(reg.l()); break;
        case 0xb6: OR_a_x(read_mmu(reg.hl())); break;
        case 0xb7: OR_a_x(reg.a()); break;
        case 0xb8: CP_a_x(reg.b()); break;
        case 0xb9: CP_a_x(reg.c()); break;
        case 0xba: CP_a_x(reg.d()); break;
        case 0xbb: CP_a_x(reg.e()); break;
        case 0xbc: CP_a_x(reg.h()); break;
        case 0xbd: CP_a_x(reg.l()); break;
        case 0xbe: CP_a_x(read_mmu(reg.hl())); break;
        case 0xbf: CP_a_x(reg.a()); break;
    }
}