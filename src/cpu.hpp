#ifndef CPU_HPP
#define CPU_HPP
#include <array>
#include <cstdint>

#include "registers.hpp"

class Cpu {
    public:
        Cpu();
        void load_rom(const char* filepath);
        void emulate();
        void disassemble_op();

    private:
        std::array<uint8_t, 65536> mmu;
        Registers reg;
        uint16_t pc;
        uint16_t sp;
        int cycles;

        // Read data from memory
        template <typename T>
        uint8_t read_mmu(T addr) { cycles += 4; return mmu.at(addr); }

        // Write data into memory
        template <typename T> 
        void write_mmu(T addr, uint8_t data) { cycles += 4; mmu.at(addr) = data; }

        // Get immediate data
        uint8_t get_n();
        uint16_t get_nn();
        int8_t get_i();


        // Instructions

        // load/store/move
        void LD_r_x(uint8_t&, uint8_t);
        void LD_rrp_r(uint16_t, uint8_t);
        void LD_rr_nn(uint16_t&);
        void LD_nnp_rr(uint16_t);
        void LD_rr_rri(uint16_t, uint16_t);
        void LD_rr_rr(uint16_t, uint16_t);
        void POP_xx(uint8_t, uint8_t);
        void PUSH_xx(uint16_t);

        // arithmetic
        void INC_r(uint8_t&);
        void INC_rrp(uint16_t);
        void DEC_r(uint8_t&);
        void DEC_rrp(uint16_t);
        void ADD_a_x(uint8_t);
        void SUB_a_x(uint8_t);
        void ADC_a_x(uint8_t);
        void SBC_a_x(uint8_t);
        void AND_a_x(uint8_t);
        void XOR_a_x(uint8_t);
        void OR_a_x(uint8_t);
        void CP_a_x(uint8_t);
        void INC_rr(uint16_t&);
        void DEC_rr(uint16_t&);
        void ADD_hl_rr(uint16_t);
        void ADD_sp_i();

        // jumps/calls
        void JR_i(bool c = true);
        void JP_nn(bool c = true);
        void JP_hlp();
        void CALL_nn(bool c = true);
        void RET();
        void RETI();
        void RET_c(bool);
        void RST_h(int);
};

#endif // CPU_HPP