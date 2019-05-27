#include <thread>
#include <chrono>
#include "cpu.hpp"

int main(int argc, char** argv) {
    Cpu cpu;
    cpu.load_rom(argv[1]);
    while (true) {
        cpu.disassemble_op();

        // Slow down so that instructions are readable
        std::this_thread::sleep_for(std::chrono::milliseconds(1000) );
    }
}