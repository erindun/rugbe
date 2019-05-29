#include "mmu/mmu.hpp"
#include "cpu/cpu.hpp"
#include "ppu/ppu.hpp"

class GameBoy {
    private:
        Mmu mmu;
        Cpu cpu;
        Ppu ppu;
    public:
        GameBoy(const char* filepath) : ppu {&cpu}, cpu {Cpu(&mmu)}  {
            mmu.load_rom(filepath);
        }
        void emulate() { 
            cpu.emulate();
            ppu.step_clock();
        }
        void test_boot_rom() { mmu.test_boot_rom(); }
};