#include <iostream>
#include <thread>
#include <chrono>
#include "cpu.hpp"

int main(int argc, char** argv) {
    // Initialize components
    Mmu mmu;
    Cpu cpu = Cpu(&mmu);

    // Load ROM into memory
    mmu.load_rom(argv[1]);

    // Emulation loop
    while (true) {
        cpu.emulate();

        // Slow down so that instructions are readable
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}