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
    uint8_t op = read_mmu(pc);

    // Print current instruction
    disassemble_op();

    switch (op) {
        case 0x00: break; // NOP
        case 0x01: LD_rr_nn(reg.bc()); break;
        case 0x02: LD_xxp_x(reg.bc(), reg.a()); break;
        case 0x03: INC_rr(reg.bc()); break;
        case 0x04: INC_r(reg.b()); break;
        case 0x05: DEC_r(reg.b()); break;
        case 0x06: LD_r_x(reg.b(), get_n()); break;
        case 0x07: break; // TODO: RLCA
        case 0x08: LD_nnp_rr(sp); break;
        case 0x09: ADD_hl_rr(reg.bc()); break;
        case 0x0a: LD_r_x(reg.a(), read_mmu(reg.bc())); break;
        case 0x0b: DEC_rr(reg.bc()); break;
        case 0x0c: INC_r(reg.c()); break;
        case 0x0d: DEC_r(reg.c()); break;
        case 0x0e: LD_r_x(reg.c(), get_n()); break;
        case 0x0f: break; // TODO: RRCA
        case 0x10: break; // TODO: STOP
        case 0x11: LD_rr_nn(reg.de()); break;
        case 0x12: LD_xxp_x(reg.de(), reg.a()); break;
        case 0x13: INC_rr(reg.de()); break;
        case 0x14: INC_r(reg.d()); break;
        case 0x15: DEC_r(reg.d()); break;
        case 0x16: LD_r_x(reg.d(), get_n()); break;
        case 0x17: break; // TODO: RLA
        case 0x18: JR_i(); break;
        case 0x19: ADD_hl_rr(reg.de()); break;
        case 0x1a: LD_r_x(reg.a(), read_mmu(reg.de())); break;
        case 0x1b: DEC_rr(reg.de()); break;
        case 0x1c: INC_r(reg.e()); break;
        case 0x1d: DEC_r(reg.e()); break;
        case 0x1e: LD_r_x(reg.e(), get_n()); break;
        case 0x1f: break; // TODO: RRA
        case 0x20: JR_i(!reg.get_zf());
        case 0x21: LD_rr_nn(reg.hl()); break;
        case 0x22: LD_xxp_x(reg.hl(), reg.a()); break; // TODO INC
        case 0x23: INC_rr(reg.hl()); break;
        case 0x24: INC_r(reg.h()); break;
        case 0x25: DEC_r(reg.h()); break;
        case 0x26: LD_r_x(reg.h(), get_n()); break;
        case 0x27: break; // TODO: DAA
        case 0x28: JR_i(reg.get_zf()); break;
        case 0x29: ADD_hl_rr(reg.hl()); break;
        case 0x2a: LD_r_x(reg.a(), read_mmu(reg.hl())); break; // TODO INC
        case 0x2b: DEC_rr(reg.hl()); break;
        case 0x2c: INC_r(reg.l()); break;
        case 0x2d: DEC_r(reg.l()); break;
        case 0x2e: LD_r_x(reg.l(), get_n()); break;
        case 0x2f: break; // TODO: CPL 
        case 0x30: JR_i(!reg.get_cf());
        case 0x31: LD_rr_nn(sp); break;
        case 0x32: LD_xxp_x(reg.hl(), reg.a()); break; // TODO DEC
        case 0x33: INC_rr(sp); break;
        case 0x34: INC_rrp(reg.hl()); break;
        case 0x35: DEC_rrp(reg.hl()); break;
        case 0x36: LD_xxp_x(reg.hl(), get_n());
        case 0x37: break; // TODO: DAA
        case 0x38: JR_i(reg.get_cf()); break;
        case 0x39: ADD_hl_rr(sp); break;
        case 0x3a: LD_r_x(reg.a(), read_mmu(reg.hl())); break; // TODO DEC
        case 0x3b: DEC_rr(sp); break;
        case 0x3c: INC_r(reg.a()); break;
        case 0x3d: DEC_r(reg.a()); break;
        case 0x3e: LD_r_x(reg.a(), get_n()); break;
        case 0x3f: break; // TODO: CCF
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
        case 0x70: LD_xxp_x(reg.hl(), reg.b()); break;
        case 0x71: LD_xxp_x(reg.hl(), reg.c()); break;
        case 0x72: LD_xxp_x(reg.hl(), reg.d()); break;
        case 0x73: LD_xxp_x(reg.hl(), reg.e()); break;
        case 0x74: LD_xxp_x(reg.hl(), reg.h()); break;
        case 0x75: LD_xxp_x(reg.hl(), reg.l()); break;
        case 0x76: break; // TODO: HALT
        case 0x77: LD_xxp_x(reg.hl(), reg.a()); break;
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
        case 0xc0: RET_c(!reg.get_zf()); break;
        case 0xc1: POP_xx(reg.b(), reg.c()); break;
        case 0xc2: JP_nn(!reg.get_zf()); break;
        case 0xc3: JP_nn(); break;
        case 0xc4: CALL_nn(!reg.get_zf()); break;
        case 0xc5: PUSH_xx(reg.bc()); break;
        case 0xc6: ADD_a_x(get_n()); break;
        case 0xc7: RST_h(0); break;
        case 0xc8: RET_c(reg.get_zf()); break;
        case 0xc9: RET(); break;
        case 0xca: JP_nn(reg.get_zf()); break;
        case 0xcb: 
            ++pc;
            switch (op) {}
            break;
        case 0xcc: CALL_nn(reg.get_zf()); break;
        case 0xcd: CALL_nn(); break;
        case 0xce: ADC_a_x(get_n()); break;
        case 0xcf: RST_h(8); break;
        case 0xd0: RET_c(!reg.get_cf()); break;
        case 0xd1: POP_xx(reg.d(), reg.e()); break;
        case 0xd2: JP_nn(!reg.get_cf()); break;
        case 0xd3: break;
        case 0xd4: CALL_nn(!reg.get_cf()); break;
        case 0xd5: PUSH_xx(reg.de()); break;
        case 0xd6: SUB_a_x(get_n()); break;
        case 0xd7: RST_h(10); break;
        case 0xd8: RET_c(reg.get_cf()); break;
        case 0xd9: RETI(); break;
        case 0xda: JP_nn(reg.get_zf()); break;
        case 0xdb: break;
        case 0xdc: CALL_nn(reg.get_cf()); break;
        case 0xdd: break;
        case 0xde: SBC_a_x(get_n()); break;
        case 0xdf: RST_h(18); break;
        case 0xe0: LDH_np_a(); break;
        case 0xe1: POP_xx(reg.h(), reg.l()); break;
        case 0xe2: LD_cp_a(); break;
        case 0xe3: break;
        case 0xe4: break;
        case 0xe5: PUSH_xx(reg.hl()); break;
        case 0xe6: AND_a_x(get_n()); break;
        case 0xe7: RST_h(20); break;
        case 0xe8: ADD_sp_i(); break;
        case 0xe9: JP_hl(); break;
        case 0xea: LD_xxp_x(get_nn(), reg.a()); break;
        case 0xeb: break;
        case 0xec: break;
        case 0xed: break;
        case 0xee: XOR_a_x(get_n()); break;
        case 0xef: RST_h(28); break;
        case 0xf0: LDH_a_np(); break;
        case 0xf1: POP_xx(reg.a(), reg.f()); break;
        case 0xf2: LD_a_cp(); break;
        case 0xf3: break; // TODO: DI
        case 0xf4: break;
        case 0xf5: PUSH_xx(reg.af()); break;
        case 0xf6: OR_a_x(get_n()); break;
        case 0xf7: RST_h(30); break;
        case 0xf8: LD_rr_rri(reg.hl(), sp); break;
        case 0xf9: LD_rr_rr(sp, reg.hl());
        case 0xfa: LD_r_x(reg.a(), read_mmu(get_nn())); break;
        case 0xfb: break; // TODO: EI
        case 0xfc: break;
        case 0xfd: break;
        case 0xfe: CP_a_x(get_n()); break;
        case 0xff: RST_h(38); break;
    }
    ++pc;
}