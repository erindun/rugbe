#include "cpu.hpp"
#include <iostream>


inline uint8_t Cpu::get_n() {
    ++pc;
    return read_mmu(pc);
}

inline uint16_t Cpu::get_nn() {
    uint16_t lowbyte = get_n();
    uint16_t highbyte = get_n() << 8;
    return highbyte | lowbyte; 
}

inline int8_t Cpu::get_i() {
    return static_cast<int8_t>(get_n());
}

// FIXME
inline void Cpu::push(uint8_t x, uint8_t y) {
    --sp;
    write_mmu(sp, x);
    std::cout << "pushed: " << std::hex << (int)x << std::endl;
    --sp;
    write_mmu(sp, y);
    std::cout << "pushed: " << std::hex << (int)y << std::endl;
}

// FIXME
inline void Cpu::pop(uint8_t& x, uint8_t& y) {
    y = read_mmu(sp);
    std::cout << "popped: " << std::hex << (int)y << std::endl;
    ++sp;
    x = read_mmu(sp);
    std::cout << "popped: " << std::hex << (int)x << std::endl;
    ++sp;
}

inline uint16_t construct_word(uint8_t lowbyte, uint8_t highbyte) {
    return (highbyte << 8) | lowbyte;
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
    write_mmu(xx, x);
}

void Cpu::LD_rr_nn(uint16_t& rr) {
    rr = get_nn();
}

// TODO: check endian-ness
void Cpu::LD_nnp_rr(uint16_t rr) {
    uint8_t highbyte = (rr >> 8 & 0xff);
    uint8_t lowbyte = rr & 0xff;

    write_mmu(get_n(), lowbyte);
    write_mmu(get_n(), highbyte);
}

void Cpu::LD_rr_rri(uint16_t rr1, uint16_t rr2) {
    rr2 += get_i();
    rr1 = rr2;
    // TODO set flags
}

// Takes extra cycle
void Cpu::LD_rr_rr(uint16_t rr1, uint16_t rr2) {
    rr1 = rr2;
    cycles += 4;
}

void Cpu::LDH_np_a() {
    write_mmu(0xff00 + get_n(), reg.a());
}

void Cpu::LDH_a_np() {
    reg.a() =  read_mmu(0xff00 + get_n());
}

void Cpu::LD_cp_a() {
    write_mmu(0xff00 + reg.c(), reg.a());
}

void Cpu::LD_a_cp() {
    reg.a() = read_mmu(0xff00 + reg.c());
}

// TODO: make individual (HL-) and (HL+) functions?

void Cpu::POP_rr(uint8_t r1, uint8_t r2) {
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
    uint8_t val = read_mmu(addr);
    reg.set_nf(0);
    reg.calc_hf(val, 1);

    ++val;
    write_mmu(addr, val);

    reg.calc_zf(val);
}

void Cpu::DEC_r(uint8_t& r) {
    reg.set_nf(1);
    reg.calc_hf(r, 1);

    --r;

    reg.calc_zf(r);
}

void Cpu::DEC_rrp(uint16_t addr) {
    uint8_t val = read_mmu(addr);
    reg.set_nf(1);
    reg.calc_hf(val, 1);

    --val;
    write_mmu(addr, val);

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
        // TODO something weird going on here
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
    uint8_t lowpc = 0;
    uint8_t highpc = 0;
    pop(lowpc, highpc);
    pc = construct_word(lowpc, highpc);
    std::cout << "PC: " << std::hex << (int)pc << std::endl;
    increment_pc = false;
    cycles += 4;
}

// Takes 16 cycles
// TODO: Add interrupt support
void Cpu::RETI() {
    uint8_t lowpc = 0;
    uint8_t highpc = 0;
    pop(lowpc, highpc);
    pc = construct_word(lowpc, highpc);
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


// Rotate A left, store old bit 7 in CF. Reset ZF, NF, HF to 0
void Cpu::RLCA() {  
    reg.set_cf(reg.a() & 0b01000000);
    rotate_left(reg.a());

    reg.set_zf(0);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate A left through carry
void Cpu::RLA() {
    bool carry = reg.get_cf();
    reg.set_cf(reg.a() & 0b01000000);

    rotate_left(reg.a());
    reg.a() += (carry ? 1 : 0);

    reg.set_zf(0);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate A right, store old bit 0 in CF. Reset ZF, NF, HF to 0
void Cpu::RRCA() {
    reg.set_cf(reg.a() & 0b00000001);
    rotate_left(reg.a());

    reg.set_zf(0);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate A right through carry
void Cpu::RRA() {
    bool carry = reg.get_cf();
    reg.set_cf(reg.a() & 0b00000001);

    rotate_right(reg.a());
    reg.a() += (carry ? 1 : 0);

    reg.set_zf(0);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate u8 left, store old bit 7 in CF. Set ZF, reset NF and HF to 0
void Cpu::RLC_r(uint8_t& r) {
    reg.set_cf(r & 0b01000000);
    rotate_left(r);

    // Set flags
    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate (HL) left, store old bit 7 in CF. Set ZF, reset NF and HF to 0
void Cpu::RLC_hlp() {
    uint8_t hlp = read_mmu(reg.hl());

    reg.set_cf(hlp & 0b01000000);
    rotate_left(hlp);

    // Set flags
    reg.calc_zf(hlp);
    reg.set_nf(0);
    reg.set_hf(0);

    cycles += 4;
}   

// Rotate r right, store old bit 0 in CF. Set ZF, reset NF and HF to 0
void Cpu::RRC_r(uint8_t& r) {
    reg.set_cf(r & 0b00000001);
    rotate_right(r);

    // Set flags
    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate (HL) right, store old bit 0 in CF. Set ZF, reset NF and HF to 0
void Cpu::RRC_hlp() {
    uint8_t hlp = read_mmu(reg.hl());

    reg.set_cf(hlp & 0b00000001);
    rotate_left(hlp);

    // Set flags
    reg.calc_zf(hlp);
    reg.set_nf(0);
    reg.set_hf(0);

    cycles += 4;
}   

// Rotate r left through carry
void Cpu::RL_r(uint8_t& r) {
    bool carry = reg.get_cf();
    reg.set_cf(r & 0b01000000);

    rotate_left(r);
    r += (carry ? 1 : 0);

    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate (HL) left through carry
void Cpu::RL_hlp() {
    uint8_t hlp = read_mmu(reg.hl());
    bool carry = reg.get_cf();
    reg.set_cf(hlp & 0b01000000);

    rotate_left(hlp);
    hlp += (carry ? 1 : 0);

    reg.calc_zf(hlp);
    reg.set_nf(0);
    reg.set_hf(0);

    cycles += 4;
}

// Rotate r right through carry
void Cpu::RR_r(uint8_t& r) {
    bool carry = reg.get_cf();
    reg.set_cf(r & 0b00000001);

    rotate_right(r);
    r += (carry ? 1 : 0);

    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Rotate (HL) right through carry
void Cpu::RR_hlp() {
    uint8_t hlp = read_mmu(reg.hl());
    bool carry = reg.get_cf();
    reg.set_cf(hlp & 0b00000001);

    rotate_right(hlp);
    hlp += (carry ? 1 : 0);

    reg.calc_zf(hlp);
    reg.set_nf(0);
    reg.set_hf(0);

    cycles += 4;
}

// Shift r left. Bit 7 becomes CF
void Cpu::SLA_r(uint8_t& r) {
    reg.set_cf(r & 0b01000000);

    r <<= 1;
    
    // Set flags
    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Shift r left. Bit 7 becomes CF
void Cpu::SLA_hlp() {
    uint8_t hlp = read_mmu(reg.hl());
    reg.set_cf(hlp & 0b01000000);

    hlp <<= 1;
    
    // Set flags
    reg.calc_zf(hlp);
    reg.set_nf(0);
    reg.set_hf(0);
    
    cycles += 4;
}

// Shift u8 right. Bit 0 becomes CF
void Cpu::SRA_r(uint8_t& r) {
    reg.set_cf(r & 0b00000001);

    r >>= 1;
    
    // Set flags
    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Shift u8 right. Bit 0 becomes CF
void Cpu::SRA_hlp() {
    uint8_t hlp = read_mmu(reg.hl());
    reg.set_cf(hlp & 0b00000001);

    hlp >>= 1;
    
    // Set flags
    reg.calc_zf(hlp);
    reg.set_nf(0);
    reg.set_hf(0);

    cycles += 4;
}

// Swap nibbles (e.g. byte 11110000 becomes 00001111)
void Cpu::SWAP_r(uint8_t& r) {
    uint8_t low = r & 0b00001111;
    uint8_t high = (r >> 4) & 0b00001111;
    r = (low << 4) | high;

    // Set flags
    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
    reg.set_cf(0);
}

// Swap nibbles (e.g. byte 11110000 becomes 00001111)
void Cpu::SWAP_hlp() {
    uint8_t hlp = read_mmu(reg.hl());
    uint8_t low = hlp & 0b00001111;
    uint8_t high = (hlp >> 4) & 0b00001111;
    hlp = (low << 4) | high;

    // Set flags
    reg.calc_zf(hlp);
    reg.set_nf(0);
    reg.set_hf(0);
    reg.set_cf(0);

    cycles += 4;
}

// Shift u8 right. Bit 0 becomes CF
// TODO
void Cpu::SRL_r(uint8_t& r) {
    reg.set_cf(r & 0b00000001);

    r >>= 1;
    
    // Set flags
    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Shift u8 right. Bit 0 becomes CF
// TODO
void Cpu::SRL_hlp() {
    uint8_t hlp = read_mmu(reg.hl());
    reg.set_cf(hlp & 0b00000001);

    hlp >>= 1;
    
    // Set flags
    reg.calc_zf(hlp);
    reg.set_nf(0);
    reg.set_hf(0);

    cycles += 4;
}

// Test bit N of u8 and set ZF accordingly
void Cpu::BIT_b_r(int bit, uint8_t r) {
    switch (bit) {
        case 0:
            r &= 0b00000001;
            break;
        case 1:
            r &= 0b00000010;
            break;
        case 2:
            r &= 0b00000100;
            break;
        case 3:
            r &= 0b00001000;
            break;
        case 4:
            r &= 0b00010000;
            break;
        case 5:
            r &= 0b00100000;
            break;
        case 6:
            r &= 0b01000000;
            break;
        case 7:
            r &= 0b10000000;
            break;
    }

    // Set flags
    reg.calc_zf(r);
    reg.set_nf(0);
    reg.set_hf(0);
}

// Test bit N of u8 and set ZF accordingly
void Cpu::BIT_b_hlp(int bit) {
    uint8_t hlp = read_mmu(reg.hl());
    switch (bit) {
        case 0:
            hlp &= 0b00000001;
            break;
        case 1:
            hlp &= 0b00000010;
            break;
        case 2:
            hlp &= 0b00000100;
            break;
        case 3:
            hlp &= 0b00001000;
            break;
        case 4:
            hlp &= 0b00010000;
            break;
        case 5:
            hlp &= 0b00100000;
            break;
        case 6:
            hlp &= 0b01000000;
            break;
        case 7:
            hlp &= 0b10000000;
            break;
    }

    // Set flags
    reg.calc_zf(hlp);
    reg.set_nf(0);
    reg.set_hf(0);
    
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
            if ((r & 0b00000010) == 1) {
                r -= 0b00000010;
            }
            break;
        case 2:
            if ((r & 0b00000100) == 1) {
                r -= 0b00000100;
            }
            break;
        case 3:
            if ((r & 0b00001000) == 1) {
                r -= 0b00001000;
            }
            break;
        case 4:
            if ((r & 0b00010000) == 1) {
                r -= 0b00010000;
            }
            break;
        case 5:
            if ((r & 0b00100000) == 1) {
                r -= 0b00100000;
            }
            break;
        case 6:
            if ((r & 0b01000000) == 1) {
                r -= 0b01000000;
            }
            break;
        case 7:
            if ((r & 0b10000000) == 1) {
                r -= 0b10000000;
            }
            break;
    }
}

// Reset bit N of u8
void Cpu::RES_b_hlp(int bit) {
    uint8_t hlp = read_mmu(reg.hl());

    switch (bit) {
        case 0:
            if ((hlp & 0b00000001) == 1) {
                hlp -= 0b00000001;
            }
            break;
        case 1:
            if ((hlp & 0b00000010) == 1) {
                hlp -= 0b00000010;
            }
            break;
        case 2:
            if ((hlp & 0b00000100) == 1) {
                hlp -= 0b00000100;
            }
            break;
        case 3:
            if ((hlp & 0b00001000) == 1) {
                hlp -= 0b00001000;
            }
            break;
        case 4:
            if ((hlp & 0b00010000) == 1) {
                hlp -= 0b00010000;
            }
            break;
        case 5:
            if ((hlp & 0b00100000) == 1) {
                hlp -= 0b00100000;
            }
            break;
        case 6:
            if ((hlp & 0b01000000) == 1) {
                hlp -= 0b01000000;
            }
            break;
        case 7:
            if ((hlp & 0b10000000) == 1) {
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
            if ((r & 0b00000010) == 0) {
                r += 0b00000010;
            }
            break;
        case 2:
            if ((r & 0b00000100) == 0) {
                r += 0b00000100;
            }
            break;
        case 3:
            if ((r & 0b00001000) == 0) {
                r += 0b00001000;
            }
            break;
        case 4:
            if ((r & 0b00010000) == 0) {
                r += 0b00010000;
            }
            break;
        case 5:
            if ((r & 0b00100000) == 0) {
                r += 0b00100000;
            }
            break;
        case 6:
            if ((r & 0b01000000) == 0) {
                r += 0b01000000;
            }
            break;
        case 7:
            if ((r & 0b10000000) == 0) {
                r += 0b10000000;
            }
            break;
    }
}

// Set bit N of u8
void Cpu::SET_b_hlp(int bit) {
    uint8_t hlp = read_mmu(reg.hl());

    switch (bit) {
        case 0:
            if ((hlp & 0b00000001) == 0) {
                hlp += 0b00000001;
            }
            break;
        case 1:
            if ((hlp & 0b00000010) == 0) {
                hlp += 0b00000010;
            }
            break;
        case 2:
            if ((hlp & 0b00000100) == 0) {
                hlp += 0b00000100;
            }
            break;
        case 3:
            if ((hlp & 0b00001000) == 0) {
                hlp += 0b00001000;
            }
            break;
        case 4:
            if ((hlp & 0b00010000) == 0) {
                hlp += 0b00010000;
            }
            break;
        case 5:
            if ((hlp & 0b00100000) == 0) {
                hlp += 0b00100000;
            }
            break;
        case 6:
            if ((hlp & 0b01000000) == 0) {
                hlp += 0b01000000;
            }
            break;
        case 7:
            if ((hlp & 0b10000000) == 0) {
                hlp += 0b10000000;
            }
            break;
    }
    
    cycles += 4;
}