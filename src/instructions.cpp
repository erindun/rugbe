#include "cpu.hpp"

inline uint8_t Cpu::get_n() {
    ++pc;
    return read_mmu(pc);
}

// TODO check endian-ness
inline uint16_t Cpu::get_nn() {
    uint16_t lobyte = get_n();
    uint16_t hibyte = get_n() << 8;
    return hibyte | lobyte; 
}

inline int8_t Cpu::get_i() {
    return static_cast<int8_t>(get_n());
}

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

void Cpu::LD_rrp_x(uint16_t addr, uint8_t r) {
    write_mmu(addr, r);
}

void Cpu::LD_rr_nn(uint16_t& rr) {
    rr = get_nn();
}

// TODO: check endian-ness
void Cpu::LD_nnp_rr(uint16_t rr) {
    uint8_t hibyte = rr & 0b11110000;
    uint8_t lobyte = rr & 0b00001111;

    write_mmu(get_n(), lobyte);
    write_mmu(get_n(), hibyte);
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

// TODO: make individual (HL-) and (HL+) functions?

// For the sake of simplicity, the 16-bit register is passed as its
// two individual 8-bit registers
// TODO: check endian-ness
void Cpu::POP_xx(uint8_t r1, uint8_t r2) {
    r1 = read_mmu(sp);
    ++sp;
    r2 = read_mmu(sp);
    ++sp;
}

// Takes extra cycle
// TODO: check endian-ness
void Cpu::PUSH_xx(uint16_t xx) {
    uint8_t hibyte = xx & 0b11110000;
    uint8_t lobyte = xx & 0b00001111;
    --sp;
    write_mmu(sp, hibyte);
    --sp;
    write_mmu(sp, lobyte);
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
        pc += i;
        cycles += 4;
    }
}

// May optionally take a condition paramter
// Takes 4 extra cycles, if true
void Cpu::JP_nn(bool c) {
    uint16_t nn = get_nn();

    if (c) {
        pc = nn;
        cycles += 4;
    }
}

// Only takes 4 cycles.
void Cpu::JP_hlp() {
    pc = reg.hl();
}

void Cpu::CALL_nn(bool c) {
    uint16_t nn = get_nn();

    if (c) {
        // I am cheating a bit using the PUSH_xx function. Because there
        // technically isn't a call to the PUSH instruction here, take off
        // 4 cycles for the ones added by PUSH_xx.
        cycles -= 4;
        // TODO: Should I push pc + 1?
        PUSH_xx(pc);
        pc = nn;
    }
}

// Takes 16 cycles
void Cpu::RET() {
    ++sp;
    uint8_t lobyte = read_mmu(sp);
    uint8_t hibyte = (read_mmu(sp)) << 8;
    pc = hibyte | lobyte;
    cycles += 4;
}

// Takes 16 cycles
// TODO: Add interrupt support
void Cpu::RETI() {
    ++sp;
    uint8_t lobyte = read_mmu(sp);
    uint8_t hibyte = (read_mmu(sp)) << 8;
    pc = hibyte | lobyte;
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
    PUSH_xx(pc);
    pc = h;
}