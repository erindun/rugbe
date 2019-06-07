#ifndef VIDEO_HPP
#define VIDEO_HPP
#include <SDL2/SDL.h>
#include "../ppu/ppu.hpp"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 576;

void setup_video();
void draw(std::array<Pixel, 160 * 144>);

#endif // VIDEO_HPP