#include "cpu.hpp"

uint8_t Cpu::get_n() {
    ++pc;
    return mmu.at(pc);
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
    x = read(y);
}

void Cpu::LD_rrp_r(uint8_t& x, uint8_t y) {
    write(x, y);
}

void Cpu::LD_rr_nn(uint16_t& x) {
    x = get_nn();
}

//void Cpu::LD_nnp_rr(uint16_t& x)
//void Cpu::LD_hl_spi()
//void Cpu::LD_sp_hl()


/******************************
 *         arithmetic 
 ******************************/


void Cpu::INC_rr(uint16_t& x) {
    ++x;
}

void Cpu::DEC_rr(uint16_t& x) {
    --x;
}