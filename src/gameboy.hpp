#include "mmu/mmu.hpp"
#include "cpu/cpu.hpp"

class GameBoy {
    private:
        Mmu mmu;
        Cpu cpu;
    public:
        GameBoy(const char* filepath) : cpu {Cpu(&mmu)} {
            mmu.load_rom(filepath);
        }
        void emulate() { cpu.emulate(); }
        void test_boot_rom() { mmu.test_boot_rom(); }
};