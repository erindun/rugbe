#include <iostream>
#include <thread>
#include <chrono>
#include "gameboy.hpp"

int main(int argc, char** argv) {
    // Load ROM into Game Boy
    GameBoy gb(argv[1]);

    // Initialize Game Boy to state for testing boot ROM
    gb.test_boot_rom();

    // Emulation loop
    while (true) {
        gb.emulate();

        // Slow down so that instructions are readable
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}