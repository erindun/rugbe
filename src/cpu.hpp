#ifndef CPU_HPP
#define CPU_HPP
#include <array>
#include <cstdint>

class Cpu {
    private:
        std::array<uint8_t, 65536> mmu;
        uint16_t pc;

    public:
        Cpu();
        void disassemble_op();
        void load_rom(const char* filepath);
};

#endif // CPU_HPP