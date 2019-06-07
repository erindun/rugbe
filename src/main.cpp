#include <iostream>
#include <thread>
#include <chrono>
#include <SDL2/SDL.h>

#include "video/video.hpp"
#include "gameboy.hpp"

int main(int argc, char** argv) {
    // Setup video
    setup_video();

    // Load ROM into Game Boy
    GameBoy gb(argv[1]);

    // Initialize Game Boy to state for testing boot ROM
    gb.test_boot_rom();


    // Emulation loop
    while (true) {
        gb.emulate();
    }

    return 0;
}