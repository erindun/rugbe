#ifndef MMU_HPP
#define MMU_HPP
#include <array>
#include <cstdint>
class Cpu;

// Wrapper class for an array serving as the system's MMU.

class Mmu {
    private:
        std::array<uint8_t, 65536> mmu;
        Cpu* cpu;

    public:
        Mmu(Cpu*);

        uint8_t& at(int i) {
            return mmu.at(i);
        }

        uint8_t read(uint16_t);
        void write(uint16_t, uint8_t);
        
        void load_rom(const char*);
        void test_boot_rom();
};

#endif // MMU_HPP