#ifndef CPU_HPP
#define CPU_HPP
#include <array>
#include <cstdint>

class Cpu {
    public:
        Cpu();
        void load_rom(const char* filepath);
        void emulate();
        void disassemble_op();

    private:
        std::array<uint8_t, 65536> mmu;
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
        void LD_r_r(uint8_t&, uint8_t);
        void LD_r_rrp(uint8_t&, uint8_t);
        void LD_rrp_r(uint8_t&, uint8_t);
        void LD_rr_nn(uint16_t&);
        void LD_nnp_rr(uint16_t);
        void LD_rr_rri(uint16_t, uint16_t);
        void LD_rr_rr(uint16_t, uint16_t);
        void POP_rr(uint8_t, uint8_t);
        void PUSH_rr(uint16_t);

        // arithmetic
        void INC_rr(uint16_t&);
        void DEC_rr(uint16_t&);
};

#endif // CPU_HPP