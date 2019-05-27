#include "cpu.hpp"

uint8_t Cpu::get_n() {
    ++pc;
    return read_mmu(pc);
}

uint16_t Cpu::get_nn() {
    uint16_t lobyte = get_n();
    uint16_t hibyte = get_n() << 8;
    return hibyte | lobyte; 
}

int8_t Cpu::get_i() {
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


void Cpu::LD_r_r(uint8_t& x, uint8_t y) {
    x = y;
}

void Cpu::LD_r_rrp(uint8_t& x, uint8_t y) {
    x = read_mmu(y);
}

void Cpu::LD_rrp_r(uint8_t& x, uint8_t y) {
    write_mmu(x, y);
}

void Cpu::LD_rr_nn(uint16_t& x) {
    x = get_nn();
}

void Cpu::LD_nnp_rr(uint16_t x) {
    uint8_t hibyte = x & 0b11110000;
    uint8_t lobyte = x & 0b00001111;

    write_mmu(get_n(), lobyte);
    write_mmu(get_n(), hibyte);
}

void Cpu::LD_rr_rri(uint16_t x, uint16_t y) {
    y += get_i();
    x = y;
    // TODO set flags
}

// Takes extra cycle
void Cpu::LD_rr_rr(uint16_t x, uint16_t y) {
    x = y;
    cycles += 4;
}

// TODO: make individual (HL-) and (HL+) functions?

// For the sake of simplicity, the 16-bit register is passed as its
// two individual 8-bit registers
void Cpu::POP_rr(uint8_t x, uint8_t y) {
    x = read_mmu(sp);
    ++sp;
    y = read_mmu(sp);
    ++sp;
}

// Takes extra cycle
void Cpu::PUSH_rr(uint16_t x) {
    uint8_t hibyte = x & 0b11110000;
    uint8_t lobyte = x & 0b00001111;
    --sp;
    write_mmu(sp, hibyte);
    --sp;
    write_mmu(sp, lobyte);
    cycles += 4;
}


/******************************
 *         arithmetic 
 ******************************/


void Cpu::INC_rr(uint16_t& x) {
    ++x;
}

void Cpu::DEC_rr(uint16_t& x) {
    --x;
}