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


/******************************
 *      load/store/move
 ******************************/


void Cpu::LD_r_r(uint8_t& r1, uint8_t r2) {
    r1 = r2;
}

void Cpu::LD_r_rrp(uint8_t& r, uint16_t addr) {
    r = read_mmu(addr);
}

void Cpu::LD_rrp_r(uint16_t addr, uint8_t r) {
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
void Cpu::POP_rr(uint8_t r1, uint8_t r2) {
    r1 = read_mmu(sp);
    ++sp;
    r2 = read_mmu(sp);
    ++sp;
}

// Takes extra cycle
// TODO: check endian-ness
void Cpu::PUSH_rr(uint16_t rr) {
    uint8_t hibyte = rr & 0b11110000;
    uint8_t lobyte = rr & 0b00001111;
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

void Cpu::ADD_a_r(uint8_t r) {
    reg.set_nf(0);
    reg.calc_hf(reg.a(), r);

    reg.a() += r;

    reg.calc_zf(reg.a());
}

void Cpu::SUB_a_r(uint8_t r) {
    reg.set_nf(1);
    reg.calc_hf(reg.a(), r);

    reg.a() -= r;

    reg.calc_zf(reg.a());
}

void Cpu::INC_rr(uint16_t& rr) {
    ++rr;
}

void Cpu::DEC_rr(uint16_t& rr) {
    --rr;
}