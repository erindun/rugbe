#ifndef MMU_HPP
#define MMU_HPP
#include <array>
#include <cstdint>

// Wrapper class for an array serving as the system's MMU.

class Mmu {
    private:
        std::array<uint8_t, 65536> mmu;

    public:
        Mmu() { mmu.fill(0); }

        uint8_t& at(int i) {
            return mmu.at(i);
        }

        void load_rom(const char*);
        void test_boot_rom();
};

#endif // MMU_HPP