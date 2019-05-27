#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>

class Cpu {
    private:
        std::array<uint8_t, 65536> mmu;
        uint16_t pc;

    public:
        Cpu();
        void disassemble_op();
        void load_rom(const char* filepath);
};