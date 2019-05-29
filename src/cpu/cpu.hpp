#ifndef CPU_HPP
#define CPU_HPP
#include <array>
#include <cstdint>

#include "registers.hpp"
#include "../mmu/mmu.hpp"

class Cpu {
    public:
        Cpu(Mmu* mmu);
        void load_rom(const char* filepath);
        void emulate();
        void disassemble_op();

    private:
        Mmu* mmu;
        uint16_t pc;
        Registers reg;
        uint16_t sp;
        int cycles;

        // Read data from memory
        template <typename T>
        uint8_t read_mmu(T addr) { cycles += 4; return mmu->at(addr); }

        // Write data into memory
        template <typename T> 
        void write_mmu(T addr, uint8_t data) { cycles += 4; mmu->at(addr) = data; }

        // Get immediate data
        uint8_t get_n();
        uint16_t get_nn();
        int8_t get_i();


        // Instructions

        // load/store/move
        void LD_r_x(uint8_t&, uint8_t);
        void LD_xxp_x(uint16_t, uint8_t);
        void LD_rr_nn(uint16_t&);
        void LD_nnp_rr(uint16_t);
        void LD_rr_rri(uint16_t, uint16_t);
        void LD_rr_rr(uint16_t, uint16_t);
        void LDH_np_a();
        void LDH_a_np();
        void LD_cp_a();
        void LD_a_cp();
        void POP_rr(uint8_t, uint8_t);
        void PUSH_rr(uint8_t, uint8_t);

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
        void JP_hl();
        void CALL_nn(bool c = true);
        void RET();
        void RETI();
        void RET_c(bool);
        void RST_h(int);

        // bit shift
        void RLCA();
        void RLA();
        void RRCA();
        void RRA();
        void RLC_r(uint8_t&);
        void RLC_hlp();
        void RRC_r(uint8_t&);
        void RRC_hlp();
        void RL_r(uint8_t&);
        void RL_hlp();
        void RR_r(uint8_t&);
        void RR_hlp();
        void SLA_r(uint8_t&);
        void SLA_hlp();
        void SRA_r(uint8_t&);
        void SRA_hlp();
        void SWAP_r(uint8_t&);
        void SWAP_hlp();
        void SRL_r(uint8_t&);
        void SRL_hlp();
        void BIT_b_r(int, uint8_t);
        void BIT_b_hlp(int);
        void RES_b_r(int, uint8_t);
        void RES_b_hlp(int);
        void SET_b_r(int, uint8_t);
        void SET_b_hlp(int);
};

#endif // CPU_HPP