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

    // Print current instruction
    disassemble_op();

    switch (read_mmu(pc)) {
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
        case 0x20: JR_i(!reg.get_zf()); break;
        case 0x21: LD_rr_nn(reg.hl()); break;
        case 0x22: LD_xxp_x(reg.hl(), reg.a()); ++reg.hl(); break;
        case 0x23: INC_rr(reg.hl()); break;
        case 0x24: INC_r(reg.h()); break;
        case 0x25: DEC_r(reg.h()); break;
        case 0x26: LD_r_x(reg.h(), get_n()); break;
        case 0x27: break; // TODO: DAA
        case 0x28: JR_i(reg.get_zf()); break;
        case 0x29: ADD_hl_rr(reg.hl()); break;
        case 0x2a: LD_r_x(reg.a(), read_mmu(reg.hl())); ++reg.hl(); break;
        case 0x2b: DEC_rr(reg.hl()); break;
        case 0x2c: INC_r(reg.l()); break;
        case 0x2d: DEC_r(reg.l()); break;
        case 0x2e: LD_r_x(reg.l(), get_n()); break;
        case 0x2f: break; // TODO: CPL 
        case 0x30: JR_i(!reg.get_cf()); break;
        case 0x31: LD_rr_nn(sp); break;
        case 0x32: --reg.hl(); LD_xxp_x(reg.hl(), reg.a()); break;
        case 0x33: INC_rr(sp); break;
        case 0x34: INC_rrp(reg.hl()); break;
        case 0x35: DEC_rrp(reg.hl()); break;
        case 0x36: LD_xxp_x(reg.hl(), get_n()); break;
        case 0x37: break; // TODO: DAA
        case 0x38: JR_i(reg.get_cf()); break;
        case 0x39: ADD_hl_rr(sp); break;
        case 0x3a: --reg.hl(); LD_r_x(reg.a(), read_mmu(reg.hl())); break;
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
            switch (read_mmu(pc)) {
                case 0x00: RLC_r(reg.b()); break;
                case 0x01: RLC_r(reg.c()); break;
                case 0x02: RLC_r(reg.d()); break;
                case 0x03: RLC_r(reg.e()); break;
                case 0x04: RLC_r(reg.h()); break;
                case 0x05: RLC_r(reg.h()); break;
                case 0x06: RLC_hlp(); break;
                case 0x07: RLC_r(reg.a()); break;
                case 0x08: RRC_r(reg.b()); break;
                case 0x09: RRC_r(reg.c()); break;
                case 0x0a: RRC_r(reg.d()); break;
                case 0x0b: RRC_r(reg.e()); break;
                case 0x0c: RRC_r(reg.h()); break;
                case 0x0d: RRC_r(reg.h()); break;
                case 0x0e: RRC_hlp(); break;
                case 0x0f: RRC_r(reg.a()); break;
                case 0x10: RL_r(reg.b()); break;
                case 0x11: RL_r(reg.c()); break;
                case 0x12: RL_r(reg.d()); break;
                case 0x13: RL_r(reg.e()); break;
                case 0x14: RL_r(reg.h()); break;
                case 0x15: RL_r(reg.h()); break;
                case 0x16: RL_hlp(); break;
                case 0x17: RL_r(reg.a()); break;
                case 0x18: RR_r(reg.b()); break;
                case 0x19: RR_r(reg.c()); break;
                case 0x1a: RR_r(reg.d()); break;
                case 0x1b: RR_r(reg.e()); break;
                case 0x1c: RR_r(reg.h()); break;
                case 0x1d: RR_r(reg.h()); break;
                case 0x1e: RR_hlp(); break;
                case 0x1f: RR_r(reg.a()); break;
                case 0x20: SLA_r(reg.b()); break;
                case 0x21: SLA_r(reg.c()); break;
                case 0x22: SLA_r(reg.d()); break;
                case 0x23: SLA_r(reg.e()); break;
                case 0x24: SLA_r(reg.h()); break;
                case 0x25: SLA_r(reg.h()); break;
                case 0x26: SLA_hlp(); break;
                case 0x27: SLA_r(reg.a()); break;
                case 0x28: SRA_r(reg.b()); break;
                case 0x29: SRA_r(reg.c()); break;
                case 0x2a: SRA_r(reg.d()); break;
                case 0x2b: SRA_r(reg.e()); break;
                case 0x2c: SRA_r(reg.h()); break;
                case 0x2d: SRA_r(reg.h()); break;
                case 0x2e: SRA_hlp(); break;
                case 0x2f: SRA_r(reg.a()); break;
                case 0x30: SWAP_r(reg.b()); break;
                case 0x31: SWAP_r(reg.c()); break;
                case 0x32: SWAP_r(reg.d()); break;
                case 0x33: SWAP_r(reg.e()); break;
                case 0x34: SWAP_r(reg.h()); break;
                case 0x35: SWAP_r(reg.h()); break;
                case 0x36: SWAP_hlp(); break;
                case 0x37: SWAP_r(reg.a()); break;
                case 0x38: SRL_r(reg.b()); break;
                case 0x39: SRL_r(reg.c()); break;
                case 0x3a: SRL_r(reg.d()); break;
                case 0x3b: SRL_r(reg.e()); break;
                case 0x3c: SRL_r(reg.h()); break;
                case 0x3d: SRL_r(reg.h()); break;
                case 0x3e: SRL_hlp(); break;
                case 0x3f: SRL_r(reg.a()); break;
                case 0x40: BIT_b_r(0, reg.b()); break;
                case 0x41: BIT_b_r(0, reg.c()); break;
                case 0x42: BIT_b_r(0, reg.d()); break;
                case 0x43: BIT_b_r(0, reg.e()); break;
                case 0x44: BIT_b_r(0, reg.h()); break;
                case 0x45: BIT_b_r(0, reg.l()); break;
                case 0x46: BIT_b_hlp(0); break;
                case 0x47: BIT_b_r(0, reg.a()); break;
                case 0x48: BIT_b_r(1, reg.b()); break;
                case 0x49: BIT_b_r(1, reg.c()); break;
                case 0x4a: BIT_b_r(1, reg.d()); break;
                case 0x4b: BIT_b_r(1, reg.e()); break;
                case 0x4c: BIT_b_r(1, reg.h()); break;
                case 0x4d: BIT_b_r(1, reg.l()); break;
                case 0x4e: BIT_b_hlp(1); break;
                case 0x4f: BIT_b_r(1, reg.a()); break;
                case 0x50: BIT_b_r(2, reg.b()); break;
                case 0x51: BIT_b_r(2, reg.c()); break;
                case 0x52: BIT_b_r(2, reg.d()); break;
                case 0x53: BIT_b_r(2, reg.e()); break;
                case 0x54: BIT_b_r(2, reg.h()); break;
                case 0x55: BIT_b_r(2, reg.l()); break;
                case 0x56: BIT_b_hlp(2); break;
                case 0x57: BIT_b_r(2, reg.a()); break;
                case 0x58: BIT_b_r(3, reg.b()); break;
                case 0x59: BIT_b_r(3, reg.c()); break;
                case 0x5a: BIT_b_r(3, reg.d()); break;
                case 0x5b: BIT_b_r(3, reg.e()); break;
                case 0x5c: BIT_b_r(3, reg.h()); break;
                case 0x5d: BIT_b_r(3, reg.l()); break;
                case 0x5e: BIT_b_hlp(3); break;
                case 0x5f: BIT_b_r(3, reg.a()); break;
                case 0x60: BIT_b_r(4, reg.b()); break;
                case 0x61: BIT_b_r(4, reg.c()); break;
                case 0x62: BIT_b_r(4, reg.d()); break;
                case 0x63: BIT_b_r(4, reg.e()); break;
                case 0x64: BIT_b_r(4, reg.h()); break;
                case 0x65: BIT_b_r(4, reg.l()); break;
                case 0x66: BIT_b_hlp(4); break;
                case 0x67: BIT_b_r(4, reg.a()); break;
                case 0x68: BIT_b_r(5, reg.b()); break;
                case 0x69: BIT_b_r(5, reg.c()); break;
                case 0x6a: BIT_b_r(5, reg.d()); break;
                case 0x6b: BIT_b_r(5, reg.e()); break;
                case 0x6c: BIT_b_r(5, reg.h()); break;
                case 0x6d: BIT_b_r(5, reg.l()); break;
                case 0x6e: BIT_b_hlp(5); break;
                case 0x6f: BIT_b_r(5, reg.a()); break;
                case 0x70: BIT_b_r(6, reg.b()); break;
                case 0x71: BIT_b_r(6, reg.c()); break;
                case 0x72: BIT_b_r(6, reg.d()); break;
                case 0x73: BIT_b_r(6, reg.e()); break;
                case 0x74: BIT_b_r(6, reg.h()); break;
                case 0x75: BIT_b_r(6, reg.l()); break;
                case 0x76: BIT_b_hlp(4); break;
                case 0x77: BIT_b_r(6, reg.a()); break;
                case 0x78: BIT_b_r(7, reg.b()); break;
                case 0x79: BIT_b_r(7, reg.c()); break;
                case 0x7a: BIT_b_r(7, reg.d()); break;
                case 0x7b: BIT_b_r(7, reg.e()); break;
                case 0x7c: BIT_b_r(7, reg.h()); break;
                case 0x7d: BIT_b_r(7, reg.l()); break;
                case 0x7e: BIT_b_hlp(7); break;
                case 0x7f: BIT_b_r(7, reg.a()); break;
                case 0x80: RES_b_r(0, reg.b()); break;
                case 0x81: RES_b_r(0, reg.c()); break;
                case 0x82: RES_b_r(0, reg.d()); break;
                case 0x83: RES_b_r(0, reg.e()); break;
                case 0x84: RES_b_r(0, reg.h()); break;
                case 0x85: RES_b_r(0, reg.l()); break;
                case 0x86: RES_b_hlp(0); break;
                case 0x87: RES_b_r(0, reg.a()); break;
                case 0x88: RES_b_r(1, reg.b()); break;
                case 0x89: RES_b_r(1, reg.c()); break;
                case 0x8a: RES_b_r(1, reg.d()); break;
                case 0x8b: RES_b_r(1, reg.e()); break;
                case 0x8c: RES_b_r(1, reg.h()); break;
                case 0x8d: RES_b_r(1, reg.l()); break;
                case 0x8e: RES_b_hlp(1); break;
                case 0x8f: RES_b_r(1, reg.a()); break;
                case 0x90: RES_b_r(2, reg.b()); break;
                case 0x91: RES_b_r(2, reg.c()); break;
                case 0x92: RES_b_r(2, reg.d()); break;
                case 0x93: RES_b_r(2, reg.e()); break;
                case 0x94: RES_b_r(2, reg.h()); break;
                case 0x95: RES_b_r(2, reg.l()); break;
                case 0x96: RES_b_hlp(2); break;
                case 0x97: RES_b_r(2, reg.a()); break;
                case 0x98: RES_b_r(3, reg.b()); break;
                case 0x99: RES_b_r(3, reg.c()); break;
                case 0x9a: RES_b_r(3, reg.d()); break;
                case 0x9b: RES_b_r(3, reg.e()); break;
                case 0x9c: RES_b_r(3, reg.h()); break;
                case 0x9d: RES_b_r(3, reg.l()); break;
                case 0x9e: RES_b_hlp(3); break;
                case 0x9f: RES_b_r(3, reg.a()); break;
                case 0xa0: RES_b_r(4, reg.b()); break;
                case 0xa1: RES_b_r(4, reg.c()); break;
                case 0xa2: RES_b_r(4, reg.d()); break;
                case 0xa3: RES_b_r(4, reg.e()); break;
                case 0xa4: RES_b_r(4, reg.h()); break;
                case 0xa5: RES_b_r(4, reg.l()); break;
                case 0xa6: RES_b_hlp(4); break;
                case 0xa7: RES_b_r(4, reg.a()); break;
                case 0xa8: RES_b_r(5, reg.b()); break;
                case 0xa9: RES_b_r(5, reg.c()); break;
                case 0xaa: RES_b_r(5, reg.d()); break;
                case 0xab: RES_b_r(5, reg.e()); break;
                case 0xac: RES_b_r(5, reg.h()); break;
                case 0xad: RES_b_r(5, reg.l()); break;
                case 0xae: RES_b_hlp(5); break;
                case 0xaf: RES_b_r(5, reg.a()); break;
                case 0xb0: RES_b_r(6, reg.b()); break;
                case 0xb1: RES_b_r(6, reg.c()); break;
                case 0xb2: RES_b_r(6, reg.d()); break;
                case 0xb3: RES_b_r(6, reg.e()); break;
                case 0xb4: RES_b_r(6, reg.h()); break;
                case 0xb5: RES_b_r(6, reg.l()); break;
                case 0xb6: RES_b_hlp(4); break;
                case 0xb7: RES_b_r(6, reg.a()); break;
                case 0xb8: RES_b_r(7, reg.b()); break;
                case 0xb9: RES_b_r(7, reg.c()); break;
                case 0xba: RES_b_r(7, reg.d()); break;
                case 0xbb: RES_b_r(7, reg.e()); break;
                case 0xbc: RES_b_r(7, reg.h()); break;
                case 0xbd: RES_b_r(7, reg.l()); break;
                case 0xbe: RES_b_hlp(7); break;
                case 0xbf: RES_b_r(7, reg.a()); break;
                case 0xc0: SET_b_r(0, reg.b()); break;
                case 0xc1: SET_b_r(0, reg.c()); break;
                case 0xc2: SET_b_r(0, reg.d()); break;
                case 0xc3: SET_b_r(0, reg.e()); break;
                case 0xc4: SET_b_r(0, reg.h()); break;
                case 0xc5: SET_b_r(0, reg.l()); break;
                case 0xc6: SET_b_hlp(0); break;
                case 0xc7: SET_b_r(0, reg.a()); break;
                case 0xc8: SET_b_r(1, reg.b()); break;
                case 0xc9: SET_b_r(1, reg.c()); break;
                case 0xca: SET_b_r(1, reg.d()); break;
                case 0xcb: SET_b_r(1, reg.e()); break;
                case 0xcc: SET_b_r(1, reg.h()); break;
                case 0xcd: SET_b_r(1, reg.l()); break;
                case 0xce: SET_b_hlp(1); break;
                case 0xcf: SET_b_r(1, reg.a()); break;
                case 0xd0: SET_b_r(2, reg.b()); break;
                case 0xd1: SET_b_r(2, reg.c()); break;
                case 0xd2: SET_b_r(2, reg.d()); break;
                case 0xd3: SET_b_r(2, reg.e()); break;
                case 0xd4: SET_b_r(2, reg.h()); break;
                case 0xd5: SET_b_r(2, reg.l()); break;
                case 0xd6: SET_b_hlp(2); break;
                case 0xd7: SET_b_r(2, reg.a()); break;
                case 0xd8: SET_b_r(3, reg.b()); break;
                case 0xd9: SET_b_r(3, reg.c()); break;
                case 0xda: SET_b_r(3, reg.d()); break;
                case 0xdb: SET_b_r(3, reg.e()); break;
                case 0xdc: SET_b_r(3, reg.h()); break;
                case 0xdd: SET_b_r(3, reg.l()); break;
                case 0xde: SET_b_hlp(3); break;
                case 0xdf: SET_b_r(3, reg.a()); break;
                case 0xe0: SET_b_r(4, reg.b()); break;
                case 0xe1: SET_b_r(4, reg.c()); break;
                case 0xe2: SET_b_r(4, reg.d()); break;
                case 0xe3: SET_b_r(4, reg.e()); break;
                case 0xe4: SET_b_r(4, reg.h()); break;
                case 0xe5: SET_b_r(4, reg.l()); break;
                case 0xe6: SET_b_hlp(4); break;
                case 0xe7: SET_b_r(4, reg.a()); break;
                case 0xe8: SET_b_r(5, reg.b()); break;
                case 0xe9: SET_b_r(5, reg.c()); break;
                case 0xea: SET_b_r(5, reg.d()); break;
                case 0xeb: SET_b_r(5, reg.e()); break;
                case 0xec: SET_b_r(5, reg.h()); break;
                case 0xed: SET_b_r(5, reg.l()); break;
                case 0xee: SET_b_hlp(5); break;
                case 0xef: SET_b_r(5, reg.a()); break;
                case 0xf0: SET_b_r(6, reg.b()); break;
                case 0xf1: SET_b_r(6, reg.c()); break;
                case 0xf2: SET_b_r(6, reg.d()); break;
                case 0xf3: SET_b_r(6, reg.e()); break;
                case 0xf4: SET_b_r(6, reg.h()); break;
                case 0xf5: SET_b_r(6, reg.l()); break;
                case 0xf6: SET_b_hlp(4); break;
                case 0xf7: SET_b_r(6, reg.a()); break;
                case 0xf8: SET_b_r(7, reg.b()); break;
                case 0xf9: SET_b_r(7, reg.c()); break;
                case 0xfa: SET_b_r(7, reg.d()); break;
                case 0xfb: SET_b_r(7, reg.e()); break;
                case 0xfc: SET_b_r(7, reg.h()); break;
                case 0xfd: SET_b_r(7, reg.l()); break;
                case 0xfe: SET_b_hlp(7); break;
                case 0xff: SET_b_r(7, reg.a()); break;
            }
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
        case 0xf9: LD_rr_rr(sp, reg.hl()); break;
        case 0xfa: LD_r_x(reg.a(), read_mmu(get_nn())); break;
        case 0xfb: break; // TODO: EI
        case 0xfc: break;
        case 0xfd: break;
        case 0xfe: CP_a_x(get_n()); break;
        case 0xff: RST_h(38); break;
    }
    ++pc;
}