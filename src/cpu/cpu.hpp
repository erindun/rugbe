#ifndef CPU_HPP
#define CPU_HPP
#include <array>
#include <cstdint>

#include "registers.hpp"
class Mmu;
class Ppu;

class Cpu {
    public:
        Cpu(Mmu*, Ppu*);
        void load_rom(const char* filepath);
        void execute_instruction();
        void disassemble_op();
        int cycles;

    private:
        Mmu* mmu;
        Ppu* ppu;
        uint16_t pc;
        bool increment_pc;
        Registers reg;
        uint16_t sp;

        // Dispatch cycles to other components
        void dispatch_cycles();

        // Retrieve values frequently accessed by instructions
        uint8_t get_n();
        uint16_t get_nn();
        int8_t get_i();
        uint8_t get_hlp();

        // Push/pop values to stack
        void push(uint8_t, uint8_t);
        void pop(uint8_t&, uint8_t&);

        // Bit shift algorithms
        void rlc(uint8_t&);
        void rl(uint8_t&);
        void rrc(uint8_t&);
        void rr(uint8_t&);
        void sla(uint8_t&);
        void sra(uint8_t&);
        void swap(uint8_t&);
        void srl(uint8_t&);

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
        void DAA();
        void CPL();
        void CCF();
        void SCF();
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