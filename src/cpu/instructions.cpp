#include "cpu.hpp"
#include "../mmu/mmu.hpp"
#include <iostream>

// Push args into stack
void Cpu::push(uint8_t x, uint8_t y) {
    --sp;
    mmu->write(sp, x);
    --sp;
    mmu->write(sp, y);
}

// Pop word from stack into args
void Cpu::pop(uint8_t& x, uint8_t& y) {
    y = mmu->read(sp);
    ++sp;
    x = mmu->read(sp);
    ++sp;
}

// Rotate left
inline void Cpu::rlc(uint8_t& x) {
    reg.set_cf((reg.a() & 0b10000000) >> 7);
    x = (x << 1) | (x >> 7);

    // Set flags
    reg.calc_zf(x);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate right
inline void Cpu::rrc(uint8_t& x) {
    reg.set_cf(x & 0b00000001);
    x = (x >> 1) | (x << 7);

    // Set flags
    reg.calc_zf(x);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate left through carry
inline void Cpu::rl(uint8_t& x) {
    bool carry = reg.get_cf();
    reg.set_cf((reg.a() & 0b10000000) >> 7);
    x <<= 1;

    // Set bit0 to the value of the carry
    x += (carry ? 1 : 0);

    // Set flags
    reg.calc_zf(x);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate right through carry
inline void Cpu::rr(uint8_t& x) {
    bool carry = reg.get_cf();
    reg.set_cf(x & 0b00000001);
    x >>= 1;

    // Set bit7 to the value of the carry
    x |= (carry ? 0b10000000 : 0b00000000);

    reg.calc_zf(x);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Shift left arithmetic
inline void Cpu::sla(uint8_t& x) {
    reg.set_cf((reg.a() & 0b10000000) >> 7);

    x <<= 1;
    
    // Set flags
    reg.calc_zf(x);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Shift right arithmetic
inline void Cpu::sra(uint8_t& x) {
    reg.set_cf((reg.a() & 0b10000000) >> 7);

    x >>= 1;
    
    // Set flags
    reg.calc_zf(x);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Shift right logical TODO
inline void Cpu::srl(uint8_t& x) {
    reg.set_cf((reg.a() & 0b10000000) >> 7);

    x >>= 1;
    
    // Set flags
    reg.calc_zf(x);
    reg.set_nf(0);
    reg.set_hf(0);
}

inline void Cpu::swap(uint8_t& x) {
    uint8_t low = x & 0b00001111;
    uint8_t high = (x & 0b11110000) >> 4;
    x = (low << 4) | high;

    // Set flags
    reg.calc_zf(x);
    reg.set_nf(0);
    reg.set_hf(0);
    reg.set_cf(0);
}

// Rotate bits
inline void rotate_left(uint8_t& r) { r = (r << 1) | (r >> 7); }
inline void rotate_right(uint8_t& r) { r = (r >> 1) | (r << 7); }

// r: 8-bit register
// rr: 16-bit register
// rp: (8-bit register)
// rrp: (16-bit register)
// n: 8-bit unsigned immediate data
// nn: 16-bit unsigned immediate data
// i: 8-bit signed immediate data
// x: wildcard; 8-bit value
// xx: wildcard; 16-bit value


/******************************
 *      load/store/move
 ******************************/


void Cpu::LD_r_x(uint8_t& r1, uint8_t r2) {
    r1 = r2;
}

void Cpu::LD_xxp_x(uint16_t xx, uint8_t x) {
    mmu->write(xx, x);
}

void Cpu::LD_rr_nn(uint16_t& rr) {
    rr = get_nn();
}

// TODO: check endian-ness
void Cpu::LD_nnp_rr(uint16_t rr) {
    uint8_t highbyte = (rr >> 8 & 0xff);
    uint8_t lowbyte = rr & 0xff;

    mmu->write(get_n(), highbyte);
    mmu->write(get_n(), lowbyte);
}

void Cpu::LD_rr_rri(uint16_t rr1, uint16_t rr2) {
    int8_t i = get_i();
    reg.set_zf(0);
    reg.set_nf(0);
    reg.calc_hf(rr2, i);
    reg.calc_cf(rr2, i);

    rr2 += i;
    rr1 = rr2;
}

// Takes extra cycle
void Cpu::LD_rr_rr(uint16_t rr1, uint16_t rr2) {
    rr1 = rr2;
    cycles += 4;
}

void Cpu::LDH_np_a() {
    mmu->write(0xff00 + get_n(), reg.a());
}

void Cpu::LDH_a_np() {
    reg.a() =  mmu->read(0xff00 + get_n());
}

void Cpu::LD_cp_a() {
    mmu->write(0xff00 + reg.c(), reg.a());
}

void Cpu::LD_a_cp() {
    reg.a() = mmu->read(0xff00 + reg.c());
}

void Cpu::POP_rr(uint8_t& r1, uint8_t& r2) {
    pop(r1, r2);
}

// Takes extra cycle
// For the sake of simplicity, the 16-bit register is passed as its
// two individual 8-bit registers
void Cpu::PUSH_rr(uint8_t r1, uint8_t r2) {
    push(r1, r2);
    cycles += 4;
}


/******************************
 *         arithmetic 
 ******************************/

void Cpu::INC_r(uint8_t& r) {
    reg.set_nf(0);
    reg.calc_hf(r, 1);

    ++r;

    reg.calc_zf(r);
}

void Cpu::INC_rrp(uint16_t addr) {
    uint8_t val = mmu->read(addr);
    reg.set_nf(0);
    reg.calc_hf(val, 1);

    ++val;
    mmu->write(addr, val);

    reg.calc_zf(val);
}

void Cpu::DEC_r(uint8_t& r) {
    reg.set_nf(1);
    reg.calc_hf(r, 1);

    --r;

    reg.calc_zf(r);
}

void Cpu::DEC_rrp(uint16_t addr) {
    uint8_t val = mmu->read(addr);
    reg.set_nf(1);
    reg.calc_hf(val, 1);

    --val;
    mmu->write(addr, val);

    reg.calc_zf(val);
}

void Cpu::ADD_a_x(uint8_t x) {
    reg.set_nf(0);
    reg.calc_hf(reg.a(), x);
    reg.calc_cf(reg.a(), x);

    reg.a() += x;

    reg.calc_zf(reg.a());
}

void Cpu::SUB_a_x(uint8_t x) {
    reg.set_nf(1);
    reg.calc_hf(reg.a(), x);
    reg.calc_cf(reg.a(), x);

    reg.a() -= x;

    reg.calc_zf(reg.a());
}

void Cpu::ADC_a_x(uint8_t x) {
    reg.set_nf(0);
    reg.calc_hf(reg.a(), x);
    reg.calc_cf(reg.a(), x);

    // Add the value of the carry to the result, i.e. add 1 or 0
    int carry = reg.get_cf() ? 1 : 0;
    reg.a() += x + carry;

    reg.calc_zf(reg.a());
}

void Cpu::SBC_a_x(uint8_t x) {
    reg.set_nf(1);
    reg.calc_hf(reg.a(), x);
    reg.calc_cf(reg.a(), x);

    // Add the value of the carry to the result, i.e. add 1 or 0
    int carry = reg.get_cf() ? 1 : 0;
    reg.a() -= x + carry;

    reg.calc_zf(reg.a());
}

void Cpu::AND_a_x(uint8_t x) {
    reg.set_nf(0);
    reg.set_hf(1);
    reg.set_cf(1);

    reg.a() &= x;

    reg.calc_zf(reg.a());
}

void Cpu::XOR_a_x(uint8_t x) {
    reg.set_nf(0);
    reg.set_hf(0);
    reg.set_cf(0);

    reg.a() ^= x;

    reg.calc_zf(reg.a());
}

void Cpu::OR_a_x(uint8_t x) {
    reg.set_nf(0);
    reg.set_hf(0);
    reg.set_cf(0);

    reg.a() |= x;

    reg.calc_zf(reg.a());
}

void Cpu::CP_a_x(uint8_t x) {
    reg.set_nf(1);
    reg.calc_hf(reg.a(), x);
    reg.calc_cf(reg.a(), x);
    reg.calc_zf(reg.a() - x);
}

void Cpu::CPL() {
    reg.a() = ~reg.a();
    reg.set_nf(1);
    reg.set_hf(1);
}

void Cpu::CCF() {
    reg.set_cf(!reg.get_cf());
    reg.set_nf(0);
    reg.set_hf(0);
}

void Cpu::SCF() {
    reg.set_cf(1);
    reg.set_nf(0);
    reg.set_hf(0);
}

void Cpu::INC_rr(uint16_t& rr) {
    ++rr;
}

void Cpu::DEC_rr(uint16_t& rr) {
    --rr;
}

void Cpu::ADD_hl_rr(uint16_t rr) {
    reg.set_nf(0);
    reg.calc_hf(reg.hl(), rr);
    reg.calc_cf(reg.hl(), rr);

    reg.hl() += rr;
}

void Cpu::ADD_sp_i() {
    int8_t i = get_i();
    reg.set_nf(0);
    reg.calc_hf(reg.hl(), i);
    reg.calc_cf(reg.hl(), i);

    sp += i;

    reg.calc_zf(sp);
}


/******************************
 *       jumps/calls
 ******************************/

// May optionally take a condition paramter
// Takes 4 extra cycles, if true
void Cpu::JR_i(bool c) {
    int8_t i = get_i();

    if (c) {
        ++pc;
        pc += i;
        increment_pc = false;

        cycles += 4;
    }
}

// May optionally take a condition paramter
// Takes 4 extra cycles, if true
void Cpu::JP_nn(bool c) {
    uint16_t nn = get_nn();

    if (c) {
        pc = nn;
        increment_pc = false;
        cycles += 4;
    }
}

// Only takes 4 cycles.
void Cpu::JP_hl() {
    pc = reg.hl();
    increment_pc = false;
}

void Cpu::CALL_nn(bool c) {
    uint16_t nn = get_nn();

    if (c) {
        ++pc;
        uint8_t lowpc = pc & 0xff;
        uint8_t highpc = (pc >> 8) & 0xff;
        push(lowpc, highpc);
        pc = nn;
        increment_pc = false;
    }
}

// Takes 16 cycles
void Cpu::RET() {
    uint8_t lowpc = 0, highpc = 0;
    pop(lowpc, highpc);
    pc = (highpc << 8) | lowpc;
    increment_pc = false;
    cycles += 4;
}

// Takes 16 cycles
// TODO: Add interrupt support
void Cpu::RETI() {
    uint8_t lowpc = 0, highpc = 0;
    pop(lowpc, highpc);
    pc = (highpc << 8) | lowpc;
    increment_pc = false;
    cycles += 4;
}

// Takes 20 cycles if true, 8 if false
void Cpu::RET_c(bool c) {
    cycles += 4;

    if (c) {
        RET();
    }
}

// Takes 16 cycles
void Cpu::RST_h(int h) {
    uint8_t lowpc = pc & 0xff;
    uint8_t highpc = (pc >> 8) & 0xff;
    push(lowpc, highpc);
    pc = h;
    increment_pc = false;

    cycles += 4;
}


/*************************
 *      bit shift
 *************************/


// Rotate r left, store old bit 7 in CF. Set ZF, reset NF and HF to 0
void Cpu::RLC_r(uint8_t& r) {
    reg.set_cf((reg.a() & 0b10000000) >> 7);
    r = (r << 1) | (r >> 7);

    // Set flags
    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate r right, store old bit 0 in CF. Set ZF, reset NF and HF to 0
void Cpu::RRC_r(uint8_t& r) {
    reg.set_cf(r & 0b00000001);
    r = (r >> 1) | (r << 7);

    // Set flags
    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate r left through carry. Store old bit 7 in CF. Set ZF, reset NF and HF to 0
void Cpu::RL_r(uint8_t& r) {
    rl(r);
}

// Rotate r right through carry
void Cpu::RR_r(uint8_t& r) {
    rr(r);
}

// Rotate (HL) left
void Cpu::RLC_hlp() {
    auto hlp = get_hlp();
    auto oldhlp = hlp;

    rlc(hlp);

    mmu->write(oldhlp, hlp);
}

// Rotate (HL) right
void Cpu::RRC_hlp() {
    auto hlp = get_hlp();
    auto oldhlp = hlp;

    rrc(hlp);

    mmu->write(oldhlp, hlp);
}

// Rotate (HL) left through carry
void Cpu::RL_hlp() {
    auto hlp = get_hlp();
    auto oldhlp = hlp;

    rl(hlp);

    mmu->write(oldhlp, hlp);
}

// Rotate (HL) right through carry
void Cpu::RR_hlp() {
    auto hlp = get_hlp();
    auto oldhlp = hlp;

    rr(hlp);

    mmu->write(oldhlp, hlp);
}

// Shift r left logical
void Cpu::SLA_r(uint8_t& r) {
    sla(r);
}

// Shift r left. Bit 7 becomes CF
void Cpu::SLA_hlp() {
    auto hlp = get_hlp();
    auto oldhlp = hlp;

    sla(hlp);

    mmu->write(oldhlp, hlp);
}

// Shift u8 right. Bit 0 becomes CF
void Cpu::SRA_r(uint8_t& r) {
    sra(r);
}

// Shift u8 right. Bit 0 becomes CF
void Cpu::SRA_hlp() {
    auto hlp = get_hlp();
    auto oldhlp = hlp;

    sra(hlp);

    mmu->write(oldhlp, hlp);
}

// Swap nibbles (e.g. byte 11110000 becomes 00001111)
void Cpu::SWAP_r(uint8_t& r) {
    swap(r);
}

// Swap nibbles (e.g. byte 11110000 becomes 00001111)
void Cpu::SWAP_hlp() {
    auto hlp = get_hlp();
    auto oldhlp = hlp;

    swap(hlp);

    mmu->write(oldhlp, hlp);
}

// Shift r right logical
void Cpu::SRL_r(uint8_t& r) {
    srl(r);
}

// Shift (HL) right logical
void Cpu::SRL_hlp() {
    auto hlp = get_hlp();
    auto oldhlp = hlp;

    sra(hlp);

    mmu->write(oldhlp, hlp);
}

// Test bit N of u8 and set ZF accordingly
void Cpu::BIT_b_r(int bit, uint8_t r) {
    switch (bit) {
        case 0:
            r &= 0b00000001;
            break;
        case 1:
            r = (r & 0b00000010) >> 1;
            break;
        case 2:
            r = (r & 0b00000100) >> 2;
            break;
        case 3:
            r = (r & 0b00001000) >> 3;
            break;
        case 4:
            r = (r & 0b00010000) >> 4;
            break;
        case 5:
            r = (r & 0b00100000) >> 5;
            break;
        case 6:
            r = (r & 0b01000000) >> 6;
            break;
        case 7:
            r = (r & 0b10000000) >> 7;
            break;
    }

    // Set flags
    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(1);
}

// Test bit N of u8 and set ZF accordingly
void Cpu::BIT_b_hlp(int bit) {
    uint8_t hlp = get_hlp();
    switch (bit) {
        case 0:
            hlp &= 0b00000001;
            break;
        case 1:
            hlp = (hlp & 0b00000010) >> 1;
            break;
        case 2:
            hlp = (hlp & 0b00000100) >> 2;
            break;
        case 3:
            hlp = (hlp & 0b00001000) >> 3;
            break;
        case 4:
            hlp = (hlp & 0b00010000) >> 4;
            break;
        case 5:
            hlp = (hlp & 0b00100000) >> 5;
            break;
        case 6:
            hlp = (hlp & 0b01000000) >> 6;
            break;
        case 7:
            hlp = (hlp & 0b10000000) >> 7;
            break;
    }

    // Set flags
    reg.calc_zf(hlp);
    reg.set_nf(0);
    reg.set_hf(1);
    
    cycles += 4;
}

// Reset bit N of u8
void Cpu::RES_b_r(int bit, uint8_t r) {
    switch (bit) {
        case 0:
            if ((r & 0b00000001) == 1) {
                r -= 0b00000001;
            }
            break;
        case 1:
            if (((r & 0b00000010) >> 1) == 1) {
                r -= 0b00000010;
            }
            break;
        case 2:
            if (((r & 0b00000100) >> 2) == 1) {
                r -= 0b00000100;
            }
            break;
        case 3:
            if (((r & 0b00001000) >> 3) == 1) {
                r -= 0b00001000;
            }
            break;
        case 4:
            if (((r & 0b00010000) >> 4) == 1) {
                r -= 0b00010000;
            }
            break;
        case 5:
            if (((r & 0b00100000) >> 5) == 1) {
                r -= 0b00100000;
            }
            break;
        case 6:
            if (((r & 0b01000000) >> 6) == 1) {
                r -= 0b01000000;
            }
            break;
        case 7:
            if (((r & 0b01000000) >> 7) == 1) {
                r -= 0b10000000;
            }
            break;
    }
}

// Reset bit N of u8
void Cpu::RES_b_hlp(int bit) {
    uint8_t hlp = get_hlp();

    switch (bit) {
        case 0:
            if ((hlp & 0b00000001) == 1) {
                hlp -= 0b00000001;
            }
            break;
        case 1:
            if (((hlp & 0b00000010) >> 1) == 1) {
                hlp -= 0b00000010;
            }
            break;
        case 2:
            if (((hlp & 0b00000100) >> 2) == 1) {
                hlp -= 0b00000100;
            }
            break;
        case 3:
            if (((hlp & 0b00001000) >> 3) == 1) {
                hlp -= 0b00001000;
            }
            break;
        case 4:
            if (((hlp & 0b00010000) >> 4) == 1) {
                hlp -= 0b00010000;
            }
            break;
        case 5:
            if (((hlp & 0b00100000) >> 5) == 1) {
                hlp -= 0b00100000;
            }
            break;
        case 6:
            if (((hlp & 0b01000000) >> 6) == 1) {
                hlp -= 0b01000000;
            }
            break;
        case 7:
            if (((hlp & 0b01000000) >> 7) == 1) {
                hlp -= 0b10000000;
            }
            break;
    }

    cycles += 4;
}

// Set bit N of u8
void Cpu::SET_b_r(int bit, uint8_t r) {
    switch (bit) {
        case 0:
            if ((r & 0b00000001) == 0) {
                r += 0b00000001;
            }
            break;
        case 1:
            if (((r & 0b00000010) >> 1) == 0) {
                r += 0b00000010;
            }
            break;
        case 2:
            if (((r & 0b00000100) >> 2) == 0) {
                r += 0b00000100;
            }
            break;
        case 3:
            if (((r & 0b00001000) >> 3) == 0) {
                r += 0b00001000;
            }
            break;
        case 4:
            if (((r & 0b00010000) >> 4) == 0) {
                r += 0b00010000;
            }
            break;
        case 5:
            if (((r & 0b00100000) >> 5) == 0) {
                r += 0b00100000;
            }
            break;
        case 6:
            if (((r & 0b01000000) >> 6) == 0) {
                r += 0b01000000;
            }
            break;
        case 7:
            if (((r & 0b01000000) >> 7) == 0) {
                r += 0b10000000;
            }
            break;
    }
}

// Set bit N of u8
void Cpu::SET_b_hlp(int bit) {
    uint8_t hlp = get_hlp();

    switch (bit) {
        case 0:
            if ((hlp & 0b00000001) == 0) {
                hlp += 0b00000001;
            }
            break;
        case 1:
            if (((hlp & 0b00000010) >> 1) == 0) {
                hlp += 0b00000010;
            }
            break;
        case 2:
            if (((hlp & 0b00000100) >> 2) == 0) {
                hlp += 0b00000100;
            }
            break;
        case 3:
            if (((hlp & 0b00001000) >> 3) == 0) {
                hlp += 0b00001000;
            }
            break;
        case 4:
            if (((hlp & 0b00010000) >> 4) == 0) {
                hlp += 0b00010000;
            }
            break;
        case 5:
            if (((hlp & 0b00100000) >> 5) == 0) {
                hlp += 0b00100000;
            }
            break;
        case 6:
            if (((hlp & 0b01000000) >> 6) == 0) {
                hlp += 0b01000000;
            }
            break;
        case 7:
            if (((hlp & 0b01000000) >> 7) == 0) {
                hlp += 0b10000000;
            }
            break;
    }
    
    cycles += 4;
}